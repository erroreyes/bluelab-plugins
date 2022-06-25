echo off

setlocal enabledelayedexpansion

for /f %%f in ('dir /b *.exe') do (
	echo %%f 
	.\%%f /silent
	rem /quiet
)

