@echo off
pushd %~dp0
setlocal enabledelayedexpansion

set CURRENT_DIR=%~dp0
set TMP_DIR=%CURRENT_DIR%.tmp/
set SOURCE_DIR=%CURRENT_DIR%source

call %CURRENT_DIR%../SetupThirdpartyBuild.bat

set BUILD_ROOT=%TMP_DIR%/build/
set INSTALL_ROOT=%TMP_DIR%/install/

set LIB_TYPE=SHARED

echo "========= Build And Install Slang ========"

for %%C in (%SLUG_THIRDPARTY_BUILD_CONFIGURATIONS%) do (
    if /I "%%C"=="Debug" (
        set MSVC_RUNTIME=MultiThreadedDebug
    ) else (
        set MSVC_RUNTIME=MultiThreaded
    )

    set BUILD_DIR=%BUILD_ROOT%\%%C
    set INSTALL_DIR=%INSTALL_ROOT%\%%C

    cmake -S "%SOURCE_DIR%" ^
          -B "!BUILD_DIR!" ^
          -G "%SLUG_THIRDPARTY_BUILD_GENERATOR%" ^
          -A %SLUG_THIRDPARTY_BUILD_ARCH% ^
          -T %SLUG_THIRDPARTY_BUILD_TOOL_CHAIN% ^
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

endlocal
popd
exit /b 0