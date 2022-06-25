[Setup]
AppName=BL-DUET
AppContact=contact@bluelab-plugs.com
AppCopyright=Copyright (C) 2021 BlueLab | Audio Plugins
AppPublisher=BlueLab | Audio Plugins
AppPublisherURL=http://www.bluelab-plugs.com
AppSupportURL=http://www.bluelab-plugs.com
AppVersion=6.0.1
VersionInfoVersion=1.0.0
DefaultDirName={pf}\BL-DUET
DefaultGroupName=BlueLab\BL-DUET
Compression=lzma2
SolidCompression=yes
OutputDir=.\
ArchitecturesInstallIn64BitMode=x64
OutputBaseFilename=BL-DUET-v6.0.1[DEMO]-Installer
LicenseFile=license.rtf
SetupLogging=yes
ShowComponentSizes=no
; WizardImageFile=installer_bg-win.bmp
; WizardSmallImageFile=installer_icon-win.bmp

SetupIconFile=..\resources\BL-DUET.ico
UninstallIconFile=..\resources\BL-DUET.ico

[Types]
Name: "full"; Description: "Full installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Messages]
WelcomeLabel1=Welcome to the BL-DUET Demo installer
SetupWindowTitle=BL-DUET [DEMO] installer
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
Name: "manual"; Description: "User guide"; Types: full custom; Flags: fixed

[Dirs] 
;Name: "{cf32}\Avid\Audio\Plug-Ins\BL-DUET.aaxplugin\"; Attribs: readonly; Components:aax_32; 
;; Name: "{cf64}\Avid\Audio\Plug-Ins\BL-DUET.aaxplugin\"; Attribs: readonly; Check: Is64BitInstallMode; Components:aax_64; 
;; Name: "{cf32}\VST3\BL-DUET.vst3\"; Attribs: readonly; Components:vst3_32; 
;; Name: "{cf64}\VST3\BL-DUET.vst3\"; Attribs: readonly; Check: Is64BitInstallMode; Components:vst3_64; 

[Files]
Source: "..\build-win\vst2\Win32\Release\BL-DUET.dll"; DestDir: {code:GetVST2Dir_32}; Check: not Is64BitInstallMode; Components:vst2_32; Flags: ignoreversion;
Source: "..\build-win\vst2\x64\Release\BL-DUET.dll"; DestDir: {code:GetVST2Dir_64}; Check: Is64BitInstallMode; Components:vst2_64; Flags: ignoreversion;

Source: "..\build-win\vst3\Win32\Release\BL-DUET.vst3"; DestDir: "{cf}\VST3\"; Check: not Is64BitInstallMode; Components:vst3_32; Flags: ignoreversion;
Source: "..\build-win\vst3\x64\Release\BL-DUET.vst3"; DestDir: "{cf64}\VST3\"; Check: Is64BitInstallMode; Components:vst3_64; Flags: ignoreversion;

Source: "..\build-win\aax\Win32\Release\BL-DUET.aaxplugin"; DestDir: "{cf32}\Avid\Audio\Plug-Ins\BL-DUET.aaxplugin\"; Components:aax_32; Flags: ignoreversion recursesubdirs;
Source: "..\build-win\aax\x64\Release\BL-DUET.aaxplugin"; DestDir: "{cf}\Avid\Audio\Plug-Ins\BL-DUET.aaxplugin\"; Components:aax_64; Flags: ignoreversion recursesubdirs;

Source: "..\manual\BL-DUET_manual-EN.pdf"; DestDir: "{app}"
Source: "readme-win-demo.rtf"; DestDir: "{app}"; DestName: "readme.rtf"; Flags: isreadme

[Icons]
;Name: "{group}\BL-DUET"; Filename: "{app}\BL-DUET.exe"
Name: "{group}\User guide"; Filename: "{app}\BL-DUET_manual-EN.pdf"
;Name: "{group}\Changelog"; Filename: "{app}\changelog.txt"
;Name: "{group}\readme"; Filename: "{app}\readme.rtf"
Name: "{group}\Uninstall BL-DUET"; Filename: "{app}\unins000.exe"

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
    VST2DirPage_64.Values[0] := ExpandConstant('{reg:HKLM\SOFTWARE\VST,VSTPluginsPath|{pf}\Steinberg\VSTPlugins}\');

    VST2DirPage_32 := CreateInputDirPage(wpSelectDir,
      'Confirm 32-Bit VST2 Plugin Directory', '',
      'Select the folder in which setup should install the 32-bit VST2 Plugin, then click Next.',
      False, '');
    VST2DirPage_32.Add('');
    VST2DirPage_32.Values[0] := ExpandConstant('{reg:HKLM\SOFTWARE\WOW6432NODE\VST,VSTPluginsPath|{pf32}\Steinberg\VSTPlugins}\');
  end else begin
    VST2DirPage_32 := CreateInputDirPage(wpSelectDir,
      'Confirm 32-Bit VST2 Plugin Directory', '',
      'Select the folder in which setup should install the 32-bit VST2 Plugin, then click Next.',
      False, '');
    VST2DirPage_32.Add('');
    VST2DirPage_32.Values[0] := ExpandConstant('{reg:HKLM\SOFTWARE\VST,VSTPluginsPath|{pf}\Steinberg\VSTPlugins}\');
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
