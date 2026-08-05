@echo off
pushd "%~dp0"
setlocal enabledelayedexpansion

set "CURRENT_DIR=%~dp0"
set "TMP_DIR=%CURRENT_DIR%.tmp"
set "SOURCE_DIR=%CURRENT_DIR%source"

call "%CURRENT_DIR%../SetupThirdpartyBuild.bat"

set "BUILD_ROOT=%TMP_DIR%\build"
set "INSTALL_ROOT=%TMP_DIR%\install"

echo "========= Build And Install Google Benchmark ========"

for %%C in (%SLUG_THIRDPARTY_BUILD_CONFIGURATIONS%) do (
    if /I "%%C"=="Debug" (
        set MSVC_RUNTIME=MultiThreadedDebugDLL
    ) else (
        set MSVC_RUNTIME=MultiThreadedDLL
    )

    set "BUILD_DIR=!BUILD_ROOT!\%%C"
    set "INSTALL_DIR=!INSTALL_ROOT!\%%C"

    cmake -S "%SOURCE_DIR%" ^
          -B "!BUILD_DIR!" ^
          -G "%SLUG_THIRDPARTY_BUILD_GENERATOR%" ^
          -A %SLUG_THIRDPARTY_BUILD_ARCH% ^
          -T %SLUG_THIRDPARTY_BUILD_TOOL_CHAIN% ^
          -DCMAKE_C_FLAGS="/utf-8" ^
          -DCMAKE_CXX_FLAGS="/utf-8" ^
          -DCMAKE_BUILD_TYPE=%%C ^
          -DCMAKE_INSTALL_PREFIX="!INSTALL_DIR!" ^
          -DCMAKE_MSVC_RUNTIME_LIBRARY=!MSVC_RUNTIME! ^
          -DBUILD_SHARED_LIBS=OFF ^
          -DBENCHMARK_ENABLE_TESTING=OFF ^
          -DBENCHMARK_ENABLE_GTEST_TESTS=OFF ^
          -DBENCHMARK_ENABLE_EXCEPTIONS=OFF ^
          -DBENCHMARK_ENABLE_WERROR=OFF ^
          -DBENCHMARK_ENABLE_INSTALL=ON ^
          -DBENCHMARK_INSTALL_DOCS=OFF ^
          -DBENCHMARK_INSTALL_TOOLS=OFF
    if errorlevel 1 exit /b 1

    cmake --build "!BUILD_DIR!" --config %%C --parallel
    if errorlevel 1 exit /b 1

    cmake --install "!BUILD_DIR!" --config %%C
    if errorlevel 1 exit /b 1
)

endlocal
popd
exit /b 0
