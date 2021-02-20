#! /usr/bin/perl

package FwRpmPackage;

use strict;
use warnings;

BEGIN {
   use Exporter   ();
   our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

   # if using RCS/CVS, this may be preferred
   $VERSION = sprintf "%d.%03d", q$Revision: 1.7 $ =~ /(\d+)/g;

   @ISA         = qw (Exporter);
   @EXPORT      = qw (&proctalk &get_state &get_dependencies &closure
                      &get_dependencies_closure 
                      &reverse_provides &parse_depends);
   %EXPORT_TAGS = ( );     # eg: TAG => [ qw!name1 name2! ],

   # your exported package globals go here,
   # as well as any optionally exported functions
   @EXPORT_OK   = qw ();
}
our @EXPORT_OK;

use IO::Pipe;
use POSIX ":sys_wait_h";

#---------------------------------------------------------------------
#                              proctalk                               
# 
# Fork a process and talk to it over a pipe pair.
#---------------------------------------------------------------------

sub proctalk ($$)
  {
    my ($parent_code, $child_code) = @_;

    my $par_to_child = new IO::Pipe;
    my $child_to_par = new IO::Pipe;

    my $pid;

    if ($pid = fork ())
      {
        # parent

        $par_to_child->writer ();
        $child_to_par->reader ();

        $parent_code-> ($child_to_par, $par_to_child);

        undef $par_to_child;
        undef $child_to_par;

        waitpid ($pid, 0);

        die "subprocess failed" if $?;
      }
    else
      {
        # child

        $par_to_child->reader ();
        $child_to_par->writer ();

        $child_code-> ($par_to_child, $child_to_par);

        die "child_code failed to exit";
      }
  }

#---------------------------------------------------------------------
#                              get_state                              
# 
# Get all of the installed packages and versions.
#---------------------------------------------------------------------

sub get_state ()
  {
    my %state;

    proctalk (
      sub
        {
          my ($readfh, $writefh) = @_;

          while (defined ($_ = <$readfh>))
            {
              chomp;

              my ($package, $version) = split /\s+/, $_, 2;

              $state{$package} = $version;
            }
        },
      sub
        {
          my ($readfh, $writefh) = @_;

          close STDIN;
          open STDIN, "<&", $readfh or die "can't dup STDIN: $!";

          close STDOUT;
          open STDOUT, ">&", $writefh or die "can't dup STDOUT: $!";

          close STDERR unless $ENV{"FW_TRACE"};

          exec "rpm",
               "-qa",
               '--queryformat',
               '%-{name}\t%{version}-%{release}\n';
        }
    );

    return (\%state, {});
  }

#---------------------------------------------------------------------
#                           get_dependencies                            
# 
# For a set of packages, identify the set of packages which are a 
# direct dependency of a member of the set.
#---------------------------------------------------------------------

sub get_dependencies ($$$$@)
  {
    my ($state, undef, $arch, $release, @packages) = @_;

    my %dependencies;
    my %deps_by_package;

    return () unless scalar @packages;

    proctalk (
      sub 
        {
          my ($readfh, $writefh) = @_;

          foreach my $package (@packages)
            {
              print $writefh "$package\n";
            }

          $writefh->close ();

          my $in_depends;
          my $package;

          while (defined ($_ = <$readfh>))
            {
              chomp;

              my ($package, undef) = split /\s+/, $_;

              # TODO: rpm -qR lists all sorts of wierd stuff ...
              next unless $state->{$package};

              scalar map { $deps_by_package{$package}->{$_} = 1;
                           $dependencies{$_} = 1 }
                     parse_depends ($state, $arch, $_, $release);
            }
        },
      sub
        {
          my ($readfh, $writefh) = @_;

          close STDIN;
          open STDIN, "<&", $readfh or die "can't dup STDIN: $!";

          close STDOUT;
          open STDOUT, ">&", $writefh or die "can't dup STDOUT: $!";

          close STDERR unless $ENV{"FW_TRACE"};

          exec "xargs", "rpm", "-qR", "--" or die "exec failed: $!";
        }
    );

    return (wantarray) ? keys %dependencies : \%deps_by_package;
    return undef;
  }

#---------------------------------------------------------------------
#                               closure                               
# 
# Form the closure of an operation on a set.
#---------------------------------------------------------------------

sub closure ($@)
  {
    my ($func, @packages) = @_;

    my %pkghash = map { $_ => 1 } @packages;

    my $finished;

    do
      {
        my @deps = $func-> (@packages);

        $finished = 1;

        @packages = map { $finished = 0; $pkghash{$_} = 1; $_ }
                    grep { ! exists $pkghash{$_} } @deps;
      }
    while (! $finished);

    return keys %pkghash;
  }

#---------------------------------------------------------------------
#                       get_dependencies_closure                        
# 
# For a set of packages, identify all installed packages which a 
# member of the set depends upon, either directly or indirectly.  
#---------------------------------------------------------------------

sub get_dependencies_closure ($$$$@)
  {
    my ($state, $virtual, $arch, $release, @packages) = @_;

    return 
      closure (sub { get_dependencies ($state, $virtual, $arch, $release, @_) },
               @packages);
  }

#---------------------------------------------------------------------
#                          reverse_provides                           
# 
# (Attempt to) find an installed package which provides a given
# package.
#---------------------------------------------------------------------

sub reverse_provides ($$)
  {
    my ($state, $package) = @_;
    my $reverse_provider;

    my $cmd = "rpm -q --whatprovides "
             ."--queryformat '%-{name} %{version}-%{release}\n' "
             .$package;
    my $provides = `$cmd`;
    $provides =~ m/^(\S+) / or die "unexpected rpm output: $provides";
    $reverse_provider = $1 if $state->{$1};

    return $reverse_provider;
  }

#---------------------------------------------------------------------
#                              rpmvercmp                              
# 
# Hopefully just like librpm.
# http://glu.ttono.us/articles/2007/09/21/rpm-version-comparison-revisited
#---------------------------------------------------------------------

sub rpmvercmp ($$)
{
  my ($a, $b) = @_;

  return 0 if $a eq $b;

  my @aver = split /[^[:alnum]]/, $a;
  my @bver = split /[^[:alnum]]/, $b;

  return 1 if @aver > @bver;
  return -1 if @aver < @bver;

  my $cmp = 0;

  foreach my $av (@aver)
    {
      my $bv = shift @bver;

      if ($av =~ /\D/ || $bv =~ /\D/)
        {
          $cmp = ($av cmp $bv);
        }
      else
        {
          $cmp = ($av <=> $bv);
        }

      return $cmp if $cmp;
    }

  return 0;
}

#---------------------------------------------------------------------
#                             enforce_op                              
# 
# Check whether $installed $op $version is true.
#---------------------------------------------------------------------

sub enforce_op ($$$)
  {
    my ($operation, $installed, $version) = @_;

    if (! defined ($operation) || $operation eq "")
      {
        return 1;
      }
    else
      {
        my $cmp = rpmvercmp ($installed, $version);

        if ($operation eq "<<" || $operation eq "<")
          {
            return ($cmp < 0);
          }
        elsif ($operation eq "<=")
          {
            return ($cmp <= 0);
          }
        elsif ($operation eq "=")
          {
            return ($cmp == 0);
          }
        elsif ($operation eq ">=")
          {
            return ($cmp >= 0);
          }
        elsif ($operation eq ">>" || $operation eq ">")
          {
            return ($cmp > 0);
          }
        else
          {
            return 0;
          }
      }
  }

#---------------------------------------------------------------------
#                            parse_depends                            
# 
# Parse FW_PACKAGE_BUILD_DEPENDENCIES, which is in debian build-time
# dependency format.  Returns the set of installed packages which 
# satisfy the dependencies.
#---------------------------------------------------------------------

sub parse_depends ($$$$)
  {
    my ($state, $arch, $depends, $release) = @_;

    my %packages;

# libc6 (>= 2.2.1), exim | mail-transport-agent
# kernel-headers-2.2.10 [!hurd-i386], hurd-dev [hurd-i386]
# erlang-nox (= INSTALLED)

    my @pkgspecs = split /,\s*/, $depends;

  SPEC:  foreach my $spec (split /,\s*/, $depends)
      {
  OPTION: foreach my $option (split /\|\s*/, $spec)
          {
            $option =~ 
              m/^(\S+)\s*(\((<<|<=|>=|>>|<(?!=)|=|>(?!=))\s*([^\s\)]*)\))?/ or
              die "can't parse dependencies '$depends' (option '$option')";

            my $p = $1;
            my $op = $3;
            my $rawv = defined ($4) ? $4 : "";
            my $version = ($rawv eq "INSTALLED") ? $state->{$p} : $rawv;

            if ($option =~ m/\[(!)?(.*)\]/)
              {
                my $not = $1;
                my $restrict = $2;

                next SPEC if ($not && $restrict eq $arch);
                next SPEC if (! $not && $restrict ne $arch);
              }

            if ($state->{$p})
              {
                if (enforce_op ($op, $state->{$p}, $version))
                  {
                    $packages{$p} = 
                      (defined ($op) && $op ne "") ? "$op $version"
                                                   : ">= $state->{$p}";
                    next SPEC;
                  }
              }
            else
              {
                my $rev_p = reverse_provides ($state, $p);

                if ($rev_p && enforce_op ($op, $state->{$rev_p}, $version))
                  {
                    $packages{$rev_p} =
                      (defined ($op) && $op ne "") ? "$op $version"
                                                   : ">= $state->{$rev_p}";
                    next SPEC;
                  }
              }
          }

        die "package/rpm/dependency-closure: fatal: '$spec' not installed\n" 
          if $release eq "yes";

        warn "package/rpm/dependency-closure: warning: '$spec' not installed\n" 
      }

    return (wantarray) ? keys %packages : \%packages;
  }

END { }       # module clean-up code here (global destructor)

1;  # don't forget to return a true value from the file

