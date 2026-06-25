@echo off
setlocal
pushd %~dp0%

if not defined SLUG_INITIALIZED_ENVIRONMENT (
     call %~dp0../../SetupEnvironment.bat
)

cd runtime/_workspace/tests/Release
for %%f in (*.exe) do (
    echo running: %%f
    "%%f"
)

popd
endlocal
