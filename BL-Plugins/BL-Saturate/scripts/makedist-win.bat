echo off

REM Must call this script with argument: 0 for normal, 1 for demo
REM If called without argument, it will say "echo was unexpected at this time"

REM - batch file to build MSVS project and zip the resulting binaries (or make installer)
REM - updating version numbers requires python and python path added to %PATH% env variable 
REM - zipping requires 7zip in %ProgramFiles%\7-Zip\7z.exe
REM - building installer requires innotsetup in "%ProgramFiles(x86)%\Inno Setup 5\iscc"
REM - AAX codesigning requires wraptool tool added to %PATH% env variable and aax.key/.crt in .\..\..\..\Certificates\

set PT_GUID="24C71FC0-C7BF-11E7-AD14-005056875CC3"

if %1 == 1 (echo Making BL-Saturate Windows DEMO VERSION distribution ...) else (echo Making BL-Saturate Windows FULL VERSION distribution ...)

echo "touching source"

REM #bluelab comment
REM This line copies Rebalance.cpp to "scripts". It should touch it instead.
REM copy /b ..\*.cpp+,,

echo ------------------------------------------------------------------
echo Updating version numbers ...

call python prepare_resources-win.py %1
call python update_installer_version.py %1

cd ..\

echo ------------------------------------------------------------------
echo Building ...

if exist "%ProgramFiles(x86)%" (goto 64-Bit) else (goto 32-Bit)

if not defined DevEnvDir (
:32-Bit
echo 32-Bit O/S detected
call "%ProgramFiles%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_x64
goto END

:64-Bit
echo 64-Bit Host O/S detected
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_x64
goto END
:END
)


REM - set preprocessor macros like this, for instance to enable demo build:
if %1 == 1 (
set CMDLINE_DEFINES="DEMO_VERSION=1"
REM -copy ".\resources\img\AboutBox_Demo.png" ".\resources\img\AboutBox.png"
) else (
set CMDLINE_DEFINES="DEMO_VERSION=0"
REM -copy ".\resources\img\AboutBox_Registered.png" ".\resources\img\AboutBox.png"
)

REM - Could build individual targets like this:
REM - msbuild BL-Saturate-app.vcxproj /p:configuration=release /p:platform=win32

echo Building 32 bit binaries...
msbuild BL-Saturate.sln /p:configuration=release /p:platform=win32 /nologo /verbosity:minimal /fileLogger /m /flp:logfile=build-win.log;errorsonly 

echo Building 64 bit binaries...
msbuild BL-Saturate.sln /p:configuration=release /p:platform=x64 /nologo /verbosity:minimal /fileLogger /m /flp:logfile=build-win.log;errorsonly;append

REM --echo Copying AAX Presets

REM --echo ------------------------------------------------------------------
REM --echo Code sign AAX binary...
REM --info at pace central, login via iLok license manager https://www.paceap.com/pace-central.html

REM set WRAPTOOL="wraptool.exe"

REM move .\build-win\aax\Win32\Release\BL-Saturate.aaxplugin .\build-win\aax\Win32\Release\BL-Saturate.aaxplugin_unsigned

REM %WRAPTOOL% sign --verbose --account nicolas.suede --password "Shells222" --keyfile "..\..\..\Certificate\BlueLab.p12" --keypassword "FY@Somewh3re" --wcguid %PT_GUID% --in .\build-win\aax\Win32\Release\BL-Saturate.aaxplugin_unsigned --out .\build-win\aax\Win32\Release\BL-Saturate.aaxplugin

REM move .\build-win\aax\x64\Release\BL-Saturate.aaxplugin .\build-win\aax\x64\Release\BL-Saturate.aaxplugin_unsigned

REM %WRAPTOOL% sign --verbose --account nicolas.suede --password "Shells222" --keyfile "..\..\..\Certificate\BlueLab.p12" --keypassword "FY@Somewh3re" --wcguid %PT_GUID% --in .\build-win\aax\x64\Release\BL-Saturate.aaxplugin_unsigned --out .\build-win\aax\x64\Release\BL-Saturate.aaxplugin

REM - Make Installer (InnoSetup)

echo ------------------------------------------------------------------
echo Making Installer ...

if exist "%ProgramFiles(x86)%" (goto 64-Bit-is) else (goto 32-Bit-is)

REM /cc
:32-Bit-is
"%ProgramFiles%\Inno Setup 5\iscc" /Q ".\installer\BL-Saturate.iss"
goto END-is

REM /cc
:64-Bit-is
"%ProgramFiles(x86)%\Inno Setup 5\iscc" /Q ".\installer\BL-Saturate.iss"
goto END-is

:END-is

REM - Codesign Installer for Windows 8+
REM -"C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Bin\signtool.exe" sign /f "XXXXX.p12" /p XXXXX /d "BL-Saturate Installer" ".\installer\BL-Saturate Installer.exe"

REM -if %1 == 1 (
REM -copy ".\installer\BL-Saturate Installer.exe" ".\installer\BL-Saturate Demo Installer.exe"
REM -del ".\installer\BL-Saturate Installer.exe"
REM -)

REM - ZIP
echo ------------------------------------------------------------------
echo Making Zip File ...

call python scripts\make_zip.py %1

echo ------------------------------------------------------------------
echo Printing log file to console...

type build-win.log
