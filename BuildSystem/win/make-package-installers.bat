rem rename the local installers and copy them to BuildSystem/Installers

echo off

set INSTALLERS_PATH=C:\Documents\Dev\BuildSystem-win\Installers

setlocal enabledelayedexpansion

call plugins-list.bat
call plug-path.bat

for %%A in %PLUG_LIST% do (
    set PLUG_PATH=!PLUGS_PATH!/%%A

	echo %%A

    set PLUG_NAME_NO_VERSION=""
   
	for /F "tokens=1 delims=- " %%B in ("%%A") do (
		set PLUG_NAME_NO_VERSION=%%B
	)
	
    set INSTALLER_SRC_PATH="!PLUG_PATH!\installer\BL-!PLUG_NAME_NO_VERSION!-Installer.exe"
	set INSTALLER_DST_PATH="!INSTALLERS_PATH!\BL-%%A-Installer.exe"

	copy /y !INSTALLER_SRC_PATH! !INSTALLER_DST_PATH!
)
