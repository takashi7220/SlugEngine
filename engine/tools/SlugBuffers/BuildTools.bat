@echo off
setlocal
pushd "%~dp0"

call "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"
msbuild SlugBuffers.sln /p:Configuration=Release

popd
endlocal