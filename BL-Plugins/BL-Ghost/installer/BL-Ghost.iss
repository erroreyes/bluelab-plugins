[Setup]
AppName=BL-Ghost
AppContact=contact@bluelab-plugs.com
AppCopyright=Copyright (C) 2021 BlueLab | Audio Plugins
AppPublisher=BlueLab | Audio Plugins
AppPublisherURL=http://www.bluelab-plugs.com
AppSupportURL=http://www.bluelab-plugs.com
AppVersion=6.3.5
VersionInfoVersion=1.0.0
DefaultDirName={pf}\BlueLab\BL-Ghost
DefaultGroupName=BlueLab\BL-Ghost
Compression=lzma2
SolidCompression=yes
OutputDir=.\
ArchitecturesInstallIn64BitMode=x64
OutputBaseFilename=BL-Ghost-v6.3.5-Installer
LicenseFile=license.rtf
SetupLogging=yes
ShowComponentSizes=no
; WizardImageFile=installer_bg-win.bmp
; WizardSmallImageFile=installer_icon-win.bmp

SetupIconFile=..\resources\BL-Ghost.ico
UninstallIconFile=..\resources\BL-Ghost.ico

[Types]
Name: "full"; Description: "Full installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Messages]
WelcomeLabel1=Welcome to the BL-Ghost installer
SetupWindowTitle=BL-Ghost installer
SelectDirLabel3=The standalone application and supporting files will be installed in the following folder.
SelectDirBrowseLabel=To continue, click Next. If you would like to select a different folder (not recommended), click Browse.

[Components]
Name: "app"; Description: "Standalone application (.exe)"; Types: full custom;
Name: "vst2_32"; Description: "32-bit VST2 Plugin (.dll)"; Types: full custom;
Name: "vst2_64"; Description: "64-bit VST2 Plugin (.dll)"; Types: full custom; Check: Is64BitInstallMode;
Name: "vst3_32"; Description: "32-bit VST3 Plugin (.vst3)"; Types: full custom;
Name: "vst3_64"; Description: "64-bit VST3 Plugin (.vst3)"; Types: full custom; Check: Is64BitInstallMode;
Name: "aax_32"; Description: "32-bit AAX Plugin (.aaxplugin)"; Types: full custom;
Name: "aax_64"; Description: "64-bit AAX Plugin (.aaxplugin)"; Types: full custom; Check: Is64BitInstallMode;
Name: "manual"; Description: "Documentation"; Types: full custom; Flags: fixed

[Dirs] 
;Name: "{cf32}\Avid\Audio\Plug-Ins\BL-Ghost.aaxplugin\"; Attribs: readonly; Components:aax_32; 
;; Name: "{cf64}\Avid\Audio\Plug-Ins\BL-Ghost.aaxplugin\"; Attribs: readonly; Check: Is64BitInstallMode; Components:aax_64; 
;; Name: "{cf32}\VST3\BL-Ghost.vst3\"; Attribs: readonly; Components:vst3_32; 
;; Name: "{cf64}\VST3\BL-Ghost.vst3\"; Attribs: readonly; Check: Is64BitInstallMode; Components:vst3_64; 

[Files]
Source: "..\build-win\BL-Ghost_Win32.exe"; DestDir: "{app}"; Check: not Is64BitInstallMode; Components:app; Flags: ignoreversion;
Source: "..\build-win\BL-Ghost_x64.exe"; DestDir: "{app}"; Check: Is64BitInstallMode; Components:app; Flags: ignoreversion;

Source: "..\build-win\vst2\Win32\Release\BL-Ghost-vst2.dll"; DestDir: {code:GetVST2Dir_32}; Components:vst2_32; Flags: ignoreversion;
Source: "..\build-win\vst2\x64\Release\BL-Ghost-vst2.dll"; DestDir: {code:GetVST2Dir_64}; Check: Is64BitInstallMode; Components:vst2_64; Flags: ignoreversion;

Source: "..\build-win\BL-Ghost.vst3\*"; Excludes: "\Contents\x86_64-win\*"; DestDir: "{cf32}\VST3\BL-Ghost.vst3"; Components:vst3_32; Flags: ignoreversion recursesubdirs;
Source: "..\build-win\BL-Ghost.vst3\Desktop.ini"; DestDir: "{cf32}\VST3\BL-Ghost.vst3\"; Components:vst3_32; Flags: overwritereadonly ignoreversion; Attribs: hidden system;
Source: "..\build-win\BL-Ghost.vst3\PlugIn.ico"; DestDir: "{cf32}\VST3\BL-Ghost.vst3\"; Components:vst3_32; Flags: overwritereadonly ignoreversion; Attribs: hidden system;

Source: "..\build-win\BL-Ghost.vst3\*"; Excludes: "\Contents\x86-win\*"; DestDir: "{cf}\VST3\BL-Ghost.vst3"; Check: Is64BitInstallMode; Components:vst3_64; Flags: ignoreversion recursesubdirs;
Source: "..\build-win\BL-Ghost.vst3\Desktop.ini"; DestDir: "{cf64}\VST3\BL-Ghost.vst3\"; Check: Is64BitInstallMode; Components:vst3_64; Flags: overwritereadonly ignoreversion; Attribs: hidden system;
Source: "..\build-win\BL-Ghost.vst3\PlugIn.ico"; DestDir: "{cf64}\VST3\BL-Ghost.vst3\"; Check: Is64BitInstallMode; Components:vst3_64; Flags: overwritereadonly ignoreversion; Attribs: hidden system;

Source: "..\build-win\BL-Ghost.aaxplugin\*"; Excludes: "\Contents\x64\*"; DestDir: "{cf32}\Avid\Audio\Plug-Ins\BL-Ghost.aaxplugin\"; Components:aax_32; Flags: ignoreversion recursesubdirs;
Source: "..\build-win\BL-Ghost.aaxplugin\Desktop.ini"; DestDir: "{cf32}\Avid\Audio\Plug-Ins\BL-Ghost.aaxplugin\"; Components:aax_32; Flags: overwritereadonly ignoreversion; Attribs: hidden system;
; Source: "..\build-win\BL-Ghost.aaxplugin\PlugIn.ico"; DestDir: "{cf32}\Avid\Audio\Plug-Ins\BL-Ghost.aaxplugin\"; Components:aax_32; Flags: overwritereadonly ignoreversion; Attribs: hidden system;

Source: "..\build-win\BL-Ghost.aaxplugin\*"; Excludes: "\Contents\Win32\*"; DestDir: "{cf}\Avid\Audio\Plug-Ins\BL-Ghost.aaxplugin\"; Check: Is64BitInstallMode; Components:aax_64; Flags: ignoreversion recursesubdirs;
Source: "..\build-win\BL-Ghost.aaxplugin\Desktop.ini"; DestDir: "{cf64}\Avid\Audio\Plug-Ins\BL-Ghost.aaxplugin\"; Check: Is64BitInstallMode; Components:aax_64; Flags: overwritereadonly ignoreversion; Attribs: hidden system;
Source: "..\build-win\BL-Ghost.aaxplugin\PlugIn.ico"; DestDir: "{cf64}\Avid\Audio\Plug-Ins\BL-Ghost.aaxplugin\"; Check: Is64BitInstallMode; Components:aax_64; Flags: overwritereadonly ignoreversion; Attribs: hidden system;

Source: "..\manual\BL-Ghost_manual-EN.pdf"; DestDir: "{app}"
Source: "readme-win.rtf"; DestDir: "{app}"; DestName: "readme.rtf"; Flags: isreadme
Source: "license.rtf"; DestDir: "{app}"; DestName: "license.rtf"

[Icons]
Name: "{group}\BL-Ghost"; Filename: "{app}\BL-Ghost.exe"
Name: "{group}\BL-Ghost Documentation"; Filename: "{app}\BL-Ghost_manual-EN.pdf"
;Name: "{group}\Changelog"; Filename: "{app}\changelog.txt"
;Name: "{group}\readme"; Filename: "{app}\readme.rtf"
Name: "{group}\Uninstall BL-Ghost"; Filename: "{app}\unins000.exe"

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
