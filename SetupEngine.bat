@echo off
setlocal
pushd "%~dp0"

echo "========== setup environment =========="
call %~dp0/SetupEnvironment.bat

echo "========== make engine ================"
echo %SLUG_ENGINE_ROOT_PATH%
call %SLUG_ENGINE_ROOT_PATH%bat\MakeEngine.bat



popd
endlocal