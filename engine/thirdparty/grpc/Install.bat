@echo off
pushd "%~dp0"
setlocal enabledelayedexpansion

set "CURRENT_DIR=%~dp0"
set "TMP_DIR=%CURRENT_DIR%.tmp"
set "SOURCE_DIR=%CURRENT_DIR%source"

call "%CURRENT_DIR%../SetupThirdpartyBuild.bat"

set "BUILD_ROOT=%TMP_DIR%\build"
set "INSTALL_ROOT=%TMP_DIR%\install"

echo "========= Build And Install gRPC ========"

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
          -DgRPC_BUILD_TESTS=OFF ^
          -DgRPC_INSTALL=ON ^
          -DgRPC_DOWNLOAD_ARCHIVES=OFF ^
          -DgRPC_BUILD_CODEGEN=ON ^
          -DgRPC_BUILD_GRPC_CPP_PLUGIN=ON ^
          -DgRPC_BUILD_GRPC_CSHARP_PLUGIN=OFF ^
          -DgRPC_BUILD_GRPC_NODE_PLUGIN=OFF ^
          -DgRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN=OFF ^
          -DgRPC_BUILD_GRPC_PHP_PLUGIN=OFF ^
          -DgRPC_BUILD_GRPC_PYTHON_PLUGIN=OFF ^
          -DgRPC_BUILD_GRPC_RUBY_PLUGIN=OFF ^
          -DgRPC_BUILD_GRPCPP_OTEL_PLUGIN=OFF ^
          -DgRPC_BUILD_MSVC_MP_COUNT=-1 ^
          -DgRPC_ABSL_PROVIDER=module ^
          -DgRPC_CARES_PROVIDER=module ^
          -DgRPC_PROTOBUF_PROVIDER=module ^
          -DgRPC_RE2_PROVIDER=module ^
          -DgRPC_SSL_PROVIDER=module ^
          -DgRPC_ZLIB_PROVIDER=module ^
          -Dprotobuf_BUILD_TESTS=OFF ^
          -Dprotobuf_WITH_ZLIB=OFF
    if errorlevel 1 exit /b 1

    cmake --build "!BUILD_DIR!" --config %%C --parallel
    if errorlevel 1 exit /b 1

    cmake --install "!BUILD_DIR!" --config %%C
    if errorlevel 1 exit /b 1
)

endlocal
popd
exit /b 0
