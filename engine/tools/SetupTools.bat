@echo off
for /D %%D in (%BASE_DIR%*) do (
    if exist "%%D\BuildTools.bat" (
        cd %%D
        call BuildTools.bat
        cd ../
    )
)

