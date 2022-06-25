[Setup]
AppName=BL-NoiseRemover
AppContact=contact@bluelab-plugs.com
AppCopyright=Copyright (C) 2021 BlueLab | Audio Plugins
AppPublisher=BlueLab | Audio Plugins
AppPublisherURL=http://www.bluelab-plugs.com
AppSupportURL=http://www.bluelab-plugs.com
AppVersion=6.2.4
VersionInfoVersion=1.0.0
DefaultDirName={pf}\BlueLab\BL-NoiseRemover
DefaultGroupName=BlueLab\BL-NoiseRemover
Compression=lzma2
SolidCompression=yes
OutputDir=.\
ArchitecturesInstallIn64BitMode=x64
OutputBaseFilename=BL-NoiseRemover-v6.2.4-Installer
LicenseFile=license.rtf
SetupLogging=yes
ShowComponentSizes=no
; WizardImageFile=installer_bg-win.bmp
; WizardSmallImageFile=installer_icon-win.bmp

SetupIconFile=..\resources\BL-NoiseRemover.ico
UninstallIconFile=..\resources\BL-NoiseRemover.ico

[Types]
Name: "full"; Description: "Full installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Messages]
WelcomeLabel1=Welcome to the BL-NoiseRemover installer
SetupWindowTitle=BL-NoiseRemover installer
SelectDirLabel3=The standalone application and supporting files will be installed in the following folder.
SelectDirBrowseLabel=To continue, click Next. If you would like to select a different folder (not recommended), click Browse.

[Components]
;Name: "app"; Description: "Standalone application (.exe)"; Types: full custom;
Name: "vst2_32"; Description: "32-bit VST2 Plugin (.dll)"; Types: full custom;
Name: "vst2_64"; Description: "64-bit VST2 Plugin (.dll)"; Types: full custom; Check: Is64BitInstallMode;
Name: "vst3_32"; Description: "32-bit VST3 Plugin (.vst3)"; Types: full custom;
Name: "vst3_64"; Description: "64-bit VST3 Plugin (.vst3)"; Types: full custom; Check: Is64BitInstallMode;
Name: "aax_32"; Description: "32-bit AAX Plugin (.aaxplugin)"; Types: full custom;
Name: "aax_64"; Description: "64-bit AAX Plugin (.aaxplugin)"; Types: full custom; Check: Is64BitInstallMode;
Name: "manual"; Description: "Documentation"; Types: full custom; Flags: fixed

[Dirs] 
;Name: "{cf32}\Avid\Audio\Plug-Ins\BL-NoiseRemover.aaxplugin\"; Attribs: readonly; Components:aax_32; 
;; Name: "{cf64}\Avid\Audio\Plug-Ins\BL-NoiseRemover.aaxplugin\"; Attribs: readonly; Check: Is64BitInstallMode; Components:aax_64; 
;; Name: "{cf32}\VST3\BL-NoiseRemover.vst3\"; Attribs: readonly; Components:vst3_32; 
;; Name: "{cf64}\VST3\BL-NoiseRemover.vst3\"; Attribs: readonly; Check: Is64BitInstallMode; Components:vst3_64; 

[Files]
Source: "..\build-win\vst2\Win32\Release\BL-NoiseRemover-vst2.dll"; DestDir: {code:GetVST2Dir_32}; Components:vst2_32; Flags: ignoreversion;
Source: "..\build-win\vst2\x64\Release\BL-NoiseRemover-vst2.dll"; DestDir: {code:GetVST2Dir_64}; Check: Is64BitInstallMode; Components:vst2_64; Flags: ignoreversion;

Source: "..\build-win\BL-NoiseRemover.vst3\*"; Excludes: "\Contents\x86_64-win\*"; DestDir: "{cf32}\VST3\BL-NoiseRemover.vst3"; Components:vst3_32; Flags: ignoreversion recursesubdirs;
Source: "..\build-win\BL-NoiseRemover.vst3\Desktop.ini"; DestDir: "{cf32}\VST3\BL-NoiseRemover.vst3\"; Components:vst3_32; Flags: overwritereadonly ignoreversion; Attribs: hidden system;
Source: "..\build-win\BL-NoiseRemover.vst3\PlugIn.ico"; DestDir: "{cf32}\VST3\BL-NoiseRemover.vst3\"; Components:vst3_32; Flags: overwritereadonly ignoreversion; Attribs: hidden system;

Source: "..\build-win\BL-NoiseRemover.vst3\*"; Excludes: "\Contents\x86-win\*"; DestDir: "{cf}\VST3\BL-NoiseRemover.vst3"; Check: Is64BitInstallMode; Components:vst3_64; Flags: ignoreversion recursesubdirs;
Source: "..\build-win\BL-NoiseRemover.vst3\Desktop.ini"; DestDir: "{cf64}\VST3\BL-NoiseRemover.vst3\"; Check: Is64BitInstallMode; Components:vst3_64; Flags: overwritereadonly ignoreversion; Attribs: hidden system;
Source: "..\build-win\BL-NoiseRemover.vst3\PlugIn.ico"; DestDir: "{cf64}\VST3\BL-NoiseRemover.vst3\"; Check: Is64BitInstallMode; Components:vst3_64; Flags: overwritereadonly ignoreversion; Attribs: hidden system;

Source: "..\build-win\BL-NoiseRemover.aaxplugin\*"; Excludes: "\Contents\x64\*"; DestDir: "{cf32}\Avid\Audio\Plug-Ins\BL-NoiseRemover.aaxplugin\"; Components:aax_32; Flags: ignoreversion recursesubdirs;
Source: "..\build-win\BL-NoiseRemover.aaxplugin\Desktop.ini"; DestDir: "{cf32}\Avid\Audio\Plug-Ins\BL-NoiseRemover.aaxplugin\"; Components:aax_32; Flags: overwritereadonly ignoreversion; Attribs: hidden system;
; Source: "..\build-win\BL-NoiseRemover.aaxplugin\PlugIn.ico"; DestDir: "{cf32}\Avid\Audio\Plug-Ins\BL-NoiseRemover.aaxplugin\"; Components:aax_32; Flags: overwritereadonly ignoreversion; Attribs: hidden system;

Source: "..\build-win\BL-NoiseRemover.aaxplugin\*"; Excludes: "\Contents\Win32\*"; DestDir: "{cf}\Avid\Audio\Plug-Ins\BL-NoiseRemover.aaxplugin\"; Check: Is64BitInstallMode; Components:aax_64; Flags: ignoreversion recursesubdirs;
Source: "..\build-win\BL-NoiseRemover.aaxplugin\Desktop.ini"; DestDir: "{cf64}\Avid\Audio\Plug-Ins\BL-NoiseRemover.aaxplugin\"; Check: Is64BitInstallMode; Components:aax_64; Flags: overwritereadonly ignoreversion; Attribs: hidden system;
Source: "..\build-win\BL-NoiseRemover.aaxplugin\PlugIn.ico"; DestDir: "{cf64}\Avid\Audio\Plug-Ins\BL-NoiseRemover.aaxplugin\"; Check: Is64BitInstallMode; Components:aax_64; Flags: overwritereadonly ignoreversion; Attribs: hidden system;

Source: "..\manual\BL-NoiseRemover_manual-EN.pdf"; DestDir: "{app}"
Source: "readme-win.rtf"; DestDir: "{app}"; DestName: "readme.rtf"; Flags: isreadme
Source: "license.rtf"; DestDir: "{app}"; DestName: "license.rtf"

[Icons]
;Name: "{group}\BL-NoiseRemover"; Filename: "{app}\BL-NoiseRemover.exe"
Name: "{group}\BL-NoiseRemover Documentation"; Filename: "{app}\BL-NoiseRemover_manual-EN.pdf"
;Name: "{group}\Changelog"; Filename: "{app}\changelog.txt"
;Name: "{group}\readme"; Filename: "{app}\readme.rtf"
Name: "{group}\Uninstall BL-NoiseRemover"; Filename: "{app}\unins000.exe"

[Code]
var
  OkToCopyLog : Boolean;
  VST2DirPage_32: TInputDirWizardPage;
  VST2DirPage_64: TInputDirWizardPage;

procedure InitializeWizard;
begin
  if IsWin64 then begin
    VST2DirPage_64 := CreateInputDirPage(wpSelectDir,
    'Confirm 64-Bit VST2 Plugin Directory', '',
    'Select the folder in which setup should install the 64-bit VST2 Plugin, then click Next.',
    False, '');
    VST2DirPage_64.Add('');
    VST2DirPage_64.Values[0] := ExpandConstant('{reg:HKLM\SOFTWARE\VST,VSTPluginsPath|{pf}\VSTPlugins}\');

    VST2DirPage_32 := CreateInputDirPage(wpSelectDir,
      'Confirm 32-Bit VST2 Plugin Directory', '',
      'Select the folder in which setup should install the 32-bit VST2 Plugin, then click Next.',
      False, '');
    VST2DirPage_32.Add('');
    VST2DirPage_32.Values[0] := ExpandConstant('{reg:HKLM\SOFTWARE\WOW6432NODE\VST,VSTPluginsPath|{pf32}\VSTPlugins}\');
  end else begin
    VST2DirPage_32 := CreateInputDirPage(wpSelectDir,
      'Confirm 32-Bit VST2 Plugin Directory', '',
      'Select the folder in which setup should install the 32-bit VST2 Plugin, then click Next.',
      False, '');
    VST2DirPage_32.Add('');
    VST2DirPage_32.Values[0] := ExpandConstant('{reg:HKLM\SOFTWARE\VST,VSTPluginsPath|{pf}\VSTPlugins}\');
  end;
end;

function GetVST2Dir_32(Param: String): String;
begin
  Result := VST2DirPage_32.Values[0]
end;

function GetVST2Dir_64(Param: String): String;
begin
  Result := VST2DirPage_64.Values[0]
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssDone then
    OkToCopyLog := True;
end;

procedure DeinitializeSetup();
begin
  if OkToCopyLog then
    FileCopy (ExpandConstant ('{log}'), ExpandConstant ('{app}\InstallationLogFile.log'), FALSE);
  RestartReplace (ExpandConstant ('{log}'), '');
end;

[UninstallDelete]
Type: files; Name: "{app}\InstallationLogFile.log"
