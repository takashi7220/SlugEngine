@echo off
pushd %~dp0
setlocal enabledelayedexpansion

set CURRENT_DIR=%~dp0
set TMP_DIR=%CURRENT_DIR%.tmp/
set SOURCE_DIR=%CURRENT_DIR%source
set CACHE_FILE=%TMP_DIR%/cache.txt

set GENERATOR=Visual Studio 18 2026
set ARCH=x64
set TOOL_CHAIN=v145

set BUILD_ROOT=%TMP_DIR%_workspace/build/
set INSTALL_ROOT=%TMP_DIR%_workspace/install/

set LIB_TYPE=SHARED
set CONFIGURATIONS=Debug Release

if exist %CACHE_FILE% (
    echo slang installed
    exit /b 0
)

for %%C in (%CONFIGURATIONS%) do (
    if /I "%%C"=="Debug" (
        set MSVC_RUNTIME=MultiThreadedDebug
    ) else (
        set MSVC_RUNTIME=MultiThreaded
    )

    set BUILD_DIR=%BUILD_ROOT%\%%C
    set INSTALL_DIR=%INSTALL_ROOT%\%%C

    cmake -S "%SOURCE_DIR%" ^
          -B "!BUILD_DIR!" ^
          -G "%GENERATOR%" ^
          -A %ARCH% ^
          -T %TOOL_CHAIN% ^
          -DCMAKE_C_FLAGS="/utf-8" ^
          -DCMAKE_CXX_FLAGS="/utf-8" ^
          -DCMAKE_INSTALL_PREFIX="!INSTALL_DIR!" ^
          -DCMAKE_MSVC_RUNTIME_LIBRARY=!MSVC_RUNTIME! ^
          -DSLANG_LIB_TYPE=%LIB_TYPE% ^
          -DSLANG_SLANG_LLVM_FLAVOR=DISABLE ^
          -DSLANG_ENABLE_TESTS=OFF ^
          -DSLANG_ENABLE_EXAMPLES=OFF ^
          -DSLANG_ENABLE_SLANGC=ON ^
          -DSLANG_ENABLE_SLANGD=OFF ^
          -DSLANG_ENABLE_SLANGI=OFF ^
          -DSLANG_ENABLE_REPLAYER=OFF ^
          -DSLANG_ENABLE_GFX=OFF ^
          -DSLANG_ENABLE_SLANG_RHI=OFF ^
          -DSLANG_ENABLE_CUDA=OFF ^
          -DSLANG_ENABLE_OPTIX=OFF ^
          -DSLANG_ENABLE_NVAPI=OFF ^
          -DSLANG_ENABLE_AFTERMATH=OFF ^
          -DSLANG_ENABLE_XLIB=OFF ^
          -DSLANG_ENABLE_SPIRV_TOOLS_MIMALLOC=OFF ^
          -DSLANG_ENABLE_DXIL=ON ^
          -DSLANG_LIB_TYPE=SHARED

    cmake --build "!BUILD_DIR!" --config %%C --parallel
    cmake --install "!BUILD_DIR!" --config %%C
)

robocopy %INSTALL_ROOT%Debug/lib %TMP_DIR%lib/Debug  /E /R:1 /W:1
robocopy %INSTALL_ROOT%Release/lib %TMP_DIR%lib/Release  /E /R:1 /W:1
robocopy %INSTALL_ROOT%Release/include %TMP_DIR%include/slang  /E /R:1 /W:1
robocopy %INSTALL_ROOT%Release/bin %TMP_DIR%bin  /E /R:1 /W:1
echo slang installed > %CACHE_FILE%

endlocal
popd
exit /b 0