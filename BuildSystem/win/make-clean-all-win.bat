echo off

setlocal enabledelayedexpansion

call plugins-list.bat
call plug-path.bat

for %%A in %PLUG_LIST% do (
    set PLUG_PATH=!PLUGS_PATH!/%%A

    echo %%A
	
    cd !PLUG_PATH!
    
    call make-clean-all.bat
)
