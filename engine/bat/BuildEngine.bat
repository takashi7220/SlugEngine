@echo off
setlocal
pushd "%~dp0"

if not defined SLUG_INITIALIZED_ENVIRONMENT (
     call %~dp0../../SetupEnvironment.bat
)

popd
endlocal