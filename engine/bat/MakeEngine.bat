@echo off
setlocal
pushd "%~dp0"

if not defined SLUG_INITIALIZED_ENVIRONMENT (
     call %~dp0../../SetupEnvironment.bat
)

rem git update
git submodule update --init --recursive
if not %errorlevel%==0 (
     popd
     endlocal
     echo git submodule update error.
     exit /b 1
)

rem remake workspace
if exist %SLUG_ENGINE_WORKSPACE_PATH% (
     rmdir /s /q %SLUG_ENGINE_WORKSPACE_PATH%
)

rem make engine
pushd %SLUG_ENGINE_ROOT_PATH%
cmake -B %SLUG_ENGINE_BUILD_PATH% -A x64 -G "Visual Studio 18 2026"
if not %errorlevel%==0 (
     popd
     endlocal
     echo cmake error.
     exit /b 1
)
popd

popd
endlocal