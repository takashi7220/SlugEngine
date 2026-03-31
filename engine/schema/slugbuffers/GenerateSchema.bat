@echo off
setlocal
pushd "%~dp0"


set CURRENT_DIR=%CD%
set GENERATOR_EXE_PATH=%CURRENT_DIR%\bin\SbGenerator.exe

if not exist "%CURRENT_DIR%/bin" (
    echo "Please Execute root/SetupEngine.bat"
    exit
)

echo %GENERATOR_EXE_PATH% --target_folders --include_folders --output %CURRENT_DIR%/schema/
popd
endlocal