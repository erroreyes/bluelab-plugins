# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# For each target create a dummy rule so the target does not have to exist
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib:
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib:
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib:
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib:


# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.aiff_rw_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/aiff_rw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/aiff_rw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/aiff_rw_test


PostBuild.alaw_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/alaw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/alaw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/alaw_test


PostBuild.benchmark.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/benchmark
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/benchmark:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/benchmark


PostBuild.channel_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/channel_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/channel_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/channel_test


PostBuild.checksum_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/checksum_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/checksum_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/checksum_test


PostBuild.chunk_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/chunk_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/chunk_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/chunk_test


PostBuild.command_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/command_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/command_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/command_test


PostBuild.compression_size_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/compression_size_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/compression_size_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/compression_size_test


PostBuild.cpp_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/cpp_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/cpp_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/cpp_test


PostBuild.dither_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/dither_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/dither_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/dither_test


PostBuild.dwvw_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/dwvw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/dwvw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/dwvw_test


PostBuild.error_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/error_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/error_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/error_test


PostBuild.external_libs_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/external_libs_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/external_libs_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/external_libs_test


PostBuild.fix_this.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/fix_this
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/fix_this:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/fix_this


PostBuild.floating_point_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/floating_point_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/floating_point_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/floating_point_test


PostBuild.format_check_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/format_check_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/format_check_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/format_check_test


PostBuild.header_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/header_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/header_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/header_test


PostBuild.headerless_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/headerless_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/headerless_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/headerless_test


PostBuild.largefile_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/largefile_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/largefile_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/largefile_test


PostBuild.locale_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/locale_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/locale_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/locale_test


PostBuild.long_read_write_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/long_read_write_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/long_read_write_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/long_read_write_test


PostBuild.lossy_comp_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/lossy_comp_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/lossy_comp_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/lossy_comp_test


PostBuild.misc_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/misc_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/misc_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/misc_test


PostBuild.multi_file_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/multi_file_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/multi_file_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/multi_file_test


PostBuild.ogg_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/ogg_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/ogg_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/ogg_test


PostBuild.pcm_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/pcm_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/pcm_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/pcm_test


PostBuild.peak_chunk_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/peak_chunk_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/peak_chunk_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/peak_chunk_test


PostBuild.pipe_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/pipe_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/pipe_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/pipe_test


PostBuild.raw_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/raw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/raw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/raw_test


PostBuild.rdwr_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/rdwr_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/rdwr_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/rdwr_test


PostBuild.scale_clip_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/scale_clip_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/scale_clip_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/scale_clip_test


PostBuild.sfversion.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/sfversion
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/sfversion:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/sfversion


PostBuild.sndfile.Debug:
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.dylib:
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.dylib


PostBuild.sndfile-cmp.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-cmp
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-cmp:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-cmp


PostBuild.sndfile-concat.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-concat
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-concat:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-concat


PostBuild.sndfile-convert.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-convert
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-convert:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-convert


PostBuild.sndfile-deinterleave.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-deinterleave
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-deinterleave:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-deinterleave


PostBuild.sndfile-info.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-info
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-info:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-info


PostBuild.sndfile-interleave.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-interleave
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-interleave:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-interleave


PostBuild.sndfile-metadata-get.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-metadata-get
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-metadata-get:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-metadata-get


PostBuild.sndfile-metadata-set.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-metadata-set
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-metadata-set:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-metadata-set


PostBuild.sndfile-play.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-play
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-play:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-play


PostBuild.sndfile-salvage.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-salvage
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-salvage:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Debug/sndfile-salvage


PostBuild.stdin_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/stdin_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/stdin_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/stdin_test


PostBuild.stdio_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/stdio_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/stdio_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/stdio_test


PostBuild.stdout_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/stdout_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/stdout_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/stdout_test


PostBuild.string_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/string_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/string_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/string_test


PostBuild.test_main.Debug:
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/test_main:
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/test_main


PostBuild.ulaw_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/ulaw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/ulaw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/ulaw_test


PostBuild.virtual_io_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/virtual_io_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/virtual_io_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/virtual_io_test


PostBuild.win32_ordinal_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/win32_ordinal_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/win32_ordinal_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/win32_ordinal_test


PostBuild.win32_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/win32_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/win32_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/win32_test


PostBuild.write_read_test.Debug:
PostBuild.sndfile.Debug: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/write_read_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/write_read_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Debug/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Debug/write_read_test


PostBuild.aiff_rw_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/aiff_rw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/aiff_rw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/aiff_rw_test


PostBuild.alaw_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/alaw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/alaw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/alaw_test


PostBuild.benchmark.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/benchmark
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/benchmark:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/benchmark


PostBuild.channel_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/channel_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/channel_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/channel_test


PostBuild.checksum_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/checksum_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/checksum_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/checksum_test


PostBuild.chunk_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/chunk_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/chunk_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/chunk_test


PostBuild.command_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/command_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/command_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/command_test


PostBuild.compression_size_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/compression_size_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/compression_size_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/compression_size_test


PostBuild.cpp_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/cpp_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/cpp_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/cpp_test


PostBuild.dither_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/dither_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/dither_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/dither_test


PostBuild.dwvw_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/dwvw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/dwvw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/dwvw_test


PostBuild.error_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/error_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/error_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/error_test


PostBuild.external_libs_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/external_libs_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/external_libs_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/external_libs_test


PostBuild.fix_this.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/fix_this
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/fix_this:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/fix_this


PostBuild.floating_point_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/floating_point_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/floating_point_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/floating_point_test


PostBuild.format_check_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/format_check_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/format_check_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/format_check_test


PostBuild.header_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/header_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/header_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/header_test


PostBuild.headerless_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/headerless_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/headerless_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/headerless_test


PostBuild.largefile_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/largefile_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/largefile_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/largefile_test


PostBuild.locale_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/locale_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/locale_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/locale_test


PostBuild.long_read_write_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/long_read_write_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/long_read_write_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/long_read_write_test


PostBuild.lossy_comp_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/lossy_comp_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/lossy_comp_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/lossy_comp_test


PostBuild.misc_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/misc_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/misc_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/misc_test


PostBuild.multi_file_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/multi_file_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/multi_file_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/multi_file_test


PostBuild.ogg_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/ogg_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/ogg_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/ogg_test


PostBuild.pcm_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/pcm_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/pcm_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/pcm_test


PostBuild.peak_chunk_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/peak_chunk_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/peak_chunk_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/peak_chunk_test


PostBuild.pipe_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/pipe_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/pipe_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/pipe_test


PostBuild.raw_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/raw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/raw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/raw_test


PostBuild.rdwr_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/rdwr_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/rdwr_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/rdwr_test


PostBuild.scale_clip_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/scale_clip_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/scale_clip_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/scale_clip_test


PostBuild.sfversion.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/sfversion
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/sfversion:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/sfversion


PostBuild.sndfile.Release:
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.dylib:
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.dylib


PostBuild.sndfile-cmp.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-cmp
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-cmp:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-cmp


PostBuild.sndfile-concat.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-concat
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-concat:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-concat


PostBuild.sndfile-convert.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-convert
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-convert:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-convert


PostBuild.sndfile-deinterleave.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-deinterleave
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-deinterleave:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-deinterleave


PostBuild.sndfile-info.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-info
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-info:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-info


PostBuild.sndfile-interleave.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-interleave
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-interleave:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-interleave


PostBuild.sndfile-metadata-get.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-metadata-get
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-metadata-get:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-metadata-get


PostBuild.sndfile-metadata-set.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-metadata-set
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-metadata-set:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-metadata-set


PostBuild.sndfile-play.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-play
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-play:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-play


PostBuild.sndfile-salvage.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-salvage
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-salvage:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/Release/sndfile-salvage


PostBuild.stdin_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/stdin_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/stdin_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/stdin_test


PostBuild.stdio_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/stdio_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/stdio_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/stdio_test


PostBuild.stdout_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/stdout_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/stdout_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/stdout_test


PostBuild.string_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/string_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/string_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/string_test


PostBuild.test_main.Release:
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/test_main:
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/test_main


PostBuild.ulaw_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/ulaw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/ulaw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/ulaw_test


PostBuild.virtual_io_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/virtual_io_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/virtual_io_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/virtual_io_test


PostBuild.win32_ordinal_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/win32_ordinal_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/win32_ordinal_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/win32_ordinal_test


PostBuild.win32_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/win32_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/win32_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/win32_test


PostBuild.write_read_test.Release:
PostBuild.sndfile.Release: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/write_read_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/write_read_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/Release/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/Release/write_read_test


PostBuild.aiff_rw_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/aiff_rw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/aiff_rw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/aiff_rw_test


PostBuild.alaw_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/alaw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/alaw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/alaw_test


PostBuild.benchmark.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/benchmark
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/benchmark:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/benchmark


PostBuild.channel_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/channel_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/channel_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/channel_test


PostBuild.checksum_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/checksum_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/checksum_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/checksum_test


PostBuild.chunk_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/chunk_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/chunk_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/chunk_test


PostBuild.command_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/command_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/command_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/command_test


PostBuild.compression_size_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/compression_size_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/compression_size_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/compression_size_test


PostBuild.cpp_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/cpp_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/cpp_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/cpp_test


PostBuild.dither_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/dither_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/dither_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/dither_test


PostBuild.dwvw_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/dwvw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/dwvw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/dwvw_test


PostBuild.error_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/error_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/error_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/error_test


PostBuild.external_libs_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/external_libs_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/external_libs_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/external_libs_test


PostBuild.fix_this.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/fix_this
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/fix_this:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/fix_this


PostBuild.floating_point_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/floating_point_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/floating_point_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/floating_point_test


PostBuild.format_check_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/format_check_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/format_check_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/format_check_test


PostBuild.header_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/header_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/header_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/header_test


PostBuild.headerless_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/headerless_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/headerless_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/headerless_test


PostBuild.largefile_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/largefile_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/largefile_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/largefile_test


PostBuild.locale_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/locale_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/locale_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/locale_test


PostBuild.long_read_write_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/long_read_write_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/long_read_write_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/long_read_write_test


PostBuild.lossy_comp_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/lossy_comp_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/lossy_comp_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/lossy_comp_test


PostBuild.misc_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/misc_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/misc_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/misc_test


PostBuild.multi_file_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/multi_file_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/multi_file_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/multi_file_test


PostBuild.ogg_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/ogg_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/ogg_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/ogg_test


PostBuild.pcm_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/pcm_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/pcm_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/pcm_test


PostBuild.peak_chunk_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/peak_chunk_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/peak_chunk_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/peak_chunk_test


PostBuild.pipe_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/pipe_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/pipe_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/pipe_test


PostBuild.raw_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/raw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/raw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/raw_test


PostBuild.rdwr_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/rdwr_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/rdwr_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/rdwr_test


PostBuild.scale_clip_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/scale_clip_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/scale_clip_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/scale_clip_test


PostBuild.sfversion.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/sfversion
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/sfversion:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/sfversion


PostBuild.sndfile.MinSizeRel:
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.dylib:
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.dylib


PostBuild.sndfile-cmp.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-cmp
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-cmp:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-cmp


PostBuild.sndfile-concat.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-concat
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-concat:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-concat


PostBuild.sndfile-convert.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-convert
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-convert:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-convert


PostBuild.sndfile-deinterleave.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-deinterleave
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-deinterleave:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-deinterleave


PostBuild.sndfile-info.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-info
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-info:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-info


PostBuild.sndfile-interleave.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-interleave
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-interleave:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-interleave


PostBuild.sndfile-metadata-get.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-metadata-get
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-metadata-get:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-metadata-get


PostBuild.sndfile-metadata-set.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-metadata-set
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-metadata-set:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-metadata-set


PostBuild.sndfile-play.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-play
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-play:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-play


PostBuild.sndfile-salvage.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-salvage
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-salvage:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/MinSizeRel/sndfile-salvage


PostBuild.stdin_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/stdin_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/stdin_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/stdin_test


PostBuild.stdio_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/stdio_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/stdio_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/stdio_test


PostBuild.stdout_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/stdout_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/stdout_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/stdout_test


PostBuild.string_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/string_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/string_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/string_test


PostBuild.test_main.MinSizeRel:
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/test_main:
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/test_main


PostBuild.ulaw_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/ulaw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/ulaw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/ulaw_test


PostBuild.virtual_io_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/virtual_io_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/virtual_io_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/virtual_io_test


PostBuild.win32_ordinal_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/win32_ordinal_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/win32_ordinal_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/win32_ordinal_test


PostBuild.win32_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/win32_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/win32_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/win32_test


PostBuild.write_read_test.MinSizeRel:
PostBuild.sndfile.MinSizeRel: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/write_read_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/write_read_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/MinSizeRel/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/MinSizeRel/write_read_test


PostBuild.aiff_rw_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/aiff_rw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/aiff_rw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/aiff_rw_test


PostBuild.alaw_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/alaw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/alaw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/alaw_test


PostBuild.benchmark.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/benchmark
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/benchmark:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/benchmark


PostBuild.channel_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/channel_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/channel_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/channel_test


PostBuild.checksum_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/checksum_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/checksum_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/checksum_test


PostBuild.chunk_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/chunk_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/chunk_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/chunk_test


PostBuild.command_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/command_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/command_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/command_test


PostBuild.compression_size_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/compression_size_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/compression_size_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/compression_size_test


PostBuild.cpp_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/cpp_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/cpp_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/cpp_test


PostBuild.dither_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/dither_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/dither_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/dither_test


PostBuild.dwvw_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/dwvw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/dwvw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/dwvw_test


PostBuild.error_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/error_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/error_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/error_test


PostBuild.external_libs_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/external_libs_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/external_libs_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/external_libs_test


PostBuild.fix_this.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/fix_this
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/fix_this:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/fix_this


PostBuild.floating_point_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/floating_point_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/floating_point_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/floating_point_test


PostBuild.format_check_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/format_check_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/format_check_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/format_check_test


PostBuild.header_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/header_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/header_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/header_test


PostBuild.headerless_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/headerless_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/headerless_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/headerless_test


PostBuild.largefile_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/largefile_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/largefile_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/largefile_test


PostBuild.locale_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/locale_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/locale_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/locale_test


PostBuild.long_read_write_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/long_read_write_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/long_read_write_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/long_read_write_test


PostBuild.lossy_comp_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/lossy_comp_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/lossy_comp_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/lossy_comp_test


PostBuild.misc_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/misc_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/misc_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/misc_test


PostBuild.multi_file_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/multi_file_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/multi_file_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/multi_file_test


PostBuild.ogg_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/ogg_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/ogg_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/ogg_test


PostBuild.pcm_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/pcm_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/pcm_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/pcm_test


PostBuild.peak_chunk_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/peak_chunk_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/peak_chunk_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/peak_chunk_test


PostBuild.pipe_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/pipe_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/pipe_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/pipe_test


PostBuild.raw_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/raw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/raw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/raw_test


PostBuild.rdwr_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/rdwr_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/rdwr_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/rdwr_test


PostBuild.scale_clip_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/scale_clip_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/scale_clip_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/scale_clip_test


PostBuild.sfversion.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/sfversion
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/sfversion:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/sfversion


PostBuild.sndfile.RelWithDebInfo:
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.dylib:
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.dylib


PostBuild.sndfile-cmp.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-cmp
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-cmp:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-cmp


PostBuild.sndfile-concat.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-concat
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-concat:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-concat


PostBuild.sndfile-convert.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-convert
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-convert:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-convert


PostBuild.sndfile-deinterleave.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-deinterleave
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-deinterleave:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-deinterleave


PostBuild.sndfile-info.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-info
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-info:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-info


PostBuild.sndfile-interleave.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-interleave
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-interleave:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-interleave


PostBuild.sndfile-metadata-get.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-metadata-get
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-metadata-get:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-metadata-get


PostBuild.sndfile-metadata-set.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-metadata-set
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-metadata-set:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-metadata-set


PostBuild.sndfile-play.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-play
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-play:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-play


PostBuild.sndfile-salvage.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-salvage
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-salvage:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/programs/RelWithDebInfo/sndfile-salvage


PostBuild.stdin_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/stdin_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/stdin_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/stdin_test


PostBuild.stdio_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/stdio_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/stdio_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/stdio_test


PostBuild.stdout_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/stdout_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/stdout_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/stdout_test


PostBuild.string_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/string_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/string_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/string_test


PostBuild.test_main.RelWithDebInfo:
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/test_main:
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/test_main


PostBuild.ulaw_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/ulaw_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/ulaw_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/ulaw_test


PostBuild.virtual_io_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/virtual_io_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/virtual_io_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/virtual_io_test


PostBuild.win32_ordinal_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/win32_ordinal_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/win32_ordinal_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/win32_ordinal_test


PostBuild.win32_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/win32_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/win32_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/win32_test


PostBuild.write_read_test.RelWithDebInfo:
PostBuild.sndfile.RelWithDebInfo: /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/write_read_test
/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/write_read_test:\
	/Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/src/RelWithDebInfo/libsndfile.SOVERSION.dylib
	/bin/rm -f /Users/applematuer/Documents/Dev/plugin-development/Libs/libsndfile-1.0.28/tests/RelWithDebInfo/write_read_test


