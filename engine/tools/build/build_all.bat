@echo off
pushd %~dp0

for /D %%D in (%BASE_DIR%*) do (
    if exist "%%D/build.bat" (
        call %%D/build.bat
    )
)

popd