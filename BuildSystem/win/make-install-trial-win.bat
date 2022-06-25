echo off

setlocal enabledelayedexpansion

set INSTALLERS_PATH=C:\Documents\Dev\BuildSystem-win\Installers\Trial

for /f %%f in ('dir /b !INSTALLERS_PATH!') do (
	echo %%f 
	!INSTALLERS_PATH!\%%f /silent
)

