echo off

setlocal enabledelayedexpansion

call plugins-list.bat

for %%A in %PLUG_LIST% do (
    for /f "tokens=1 delims=-" %%B in ('echo %%A') do (
    	set PLUG_NAME_NO_VERSION=%%B
    )
    
    set PLUG_AAX="C:\Program Files\Common Files\Avid\Audio\Plug-Ins\BL-!PLUG_NAME_NO_VERSION!.aaxplugin\Contents\x64\BL-!PLUG_NAME_NO_VERSION!.aaxplugin"

    echo "PLUG: " BL-!PLUG_NAME_NO_VERSION!.aaxplugin
    
    "C:\Program Files (x86)\PACEAntiPiracy\Eden\Fusion\Versions\5\wraptool.exe" verify --in !PLUG_AAX!
)
