rem delete the installed files in the different install folders

echo off

setlocal enabledelayedexpansion

call plugins-list.bat
call plug-path.bat

set 
for %%A in %PLUG_LIST% do (
    set PLUG_PATH=!PLUGS_PATH!/%%A

	echo %%A

    set PLUG_NAME_NO_VERSION=""
   
	for /F "tokens=1 delims=- " %%B in ("%%A") do (
		set PLUG_NAME_NO_VERSION=%%B
	)
	
	call "C:\Program Files\BlueLab\!PLUG_NAME_NO_VERSION!\unins000.exe" /silent
)