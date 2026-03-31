@echo off
set CURRENT_DIR=%CD%
set FLATC_EXTENSION_DIR=%CURRENT_DIR%\..\..\tools\_bin\FlatcExtension
set FLATC_EXTENSION_PATH=%FLATC_EXTENSION_DIR%\FlatcExtension.exe

if not exist "%FLATC_EXTENSION_PATH%" (
    echo "Please Execute root/SetupEngine.bat"
    pause
    exit
)

if not exist "%CURRENT_DIR%\_bin" (
    mkdir "%CURRENT_DIR%\_bin"
)

%FLATC_EXTENSION_PATH% --input %CURRENT_DIR%\fbs\srtf --cpp --reflect-types --reflect-names -o %CURRENT_DIR%\cpp\srtf\include\srtf --filename-suffix _fbs --gen-mutable
%FLATC_EXTENSION_PATH% --input %CURRENT_DIR%\fbs\srtf --csharp --reflect-types --reflect-names --gen-object-api -o %CURRENT_DIR%\csharp\srtf\source --gen-mutable
