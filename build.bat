@echo off
pushd "%~dp0"

call ".\Script\premake5.exe" --file=premake5.lua %* vs2026
set "ERR=%ERRORLEVEL%"

popd
pause
exit /b %ERR%
