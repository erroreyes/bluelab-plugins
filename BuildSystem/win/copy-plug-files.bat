rem Copy plug files in BL-Template-win
rem First, we must have installed the plugin using installer

echo off

setlocal enabledelayedexpansion

call plugins-list.bat

for %%A in %PLUG_LIST% do (
    for /f "tokens=1 delims=-" %%B in ('echo %%A') do (
    	set PLUG_NAME_NO_VERSION=%%B
    )
    echo "PLUG: " BL-!PLUG_NAME_NO_VERSION!

    copy "C:\Program Files (x86)\VST\BL-!PLUG_NAME_NO_VERSION!.dll" "BL-Template-win\VST\Win32"

    copy "C:\Program Files\Steinberg\VSTPlugins\BL-!PLUG_NAME_NO_VERSION!.dll" "BL-Template-win\VST\x64"

    copy "C:\Program Files (x86)\Common Files\VST3\BL-!PLUG_NAME_NO_VERSION!.vst3" "BL-Template-win\VST3\Win32"

    copy "C:\Program Files\Common Files\VST3\BL-!PLUG_NAME_NO_VERSION!.vst3" "BL-Template-win\VST3\x64"

    xcopy "C:\Program Files (x86)\Common Files\Avid\Audio\Plug-Ins\BL-!PLUG_NAME_NO_VERSION!.aaxplugin" "BL-Template-win\AAX\Win32\BL-!PLUG_NAME_NO_VERSION!.aaxplugin" /E /H /R /Q /Y /K
    
    xcopy "C:\Program Files\Common Files\Avid\Audio\Plug-Ins\BL-!PLUG_NAME_NO_VERSION!.aaxplugin" "BL-Template-win\AAX\x64\BL-!PLUG_NAME_NO_VERSION!.aaxplugin" /E /H /R /Q /Y /K
)
