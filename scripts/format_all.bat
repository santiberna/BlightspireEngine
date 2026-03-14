@echo off
setlocal enabledelayedexpansion
 
:: ============================================================
:: Edit these to suit your needs
:: ============================================================
set "TARGET_DIR=engine"
set "RECURSE=1"
set "STYLE=file"
set "EXTENSIONS=c cc cpp cxx h hh hpp hxx"
:: ============================================================
 
set "FILE_COUNT=0"
 
for %%E in (%EXTENSIONS%) do (
    if "%RECURSE%"=="1" (
        for /f "usebackq delims=" %%F in (`dir /b /s /a-d "%TARGET_DIR%\*.%%E" 2^>nul`) do (
            echo Formatting: %%F
            clang-format -i --style=%STYLE% "%%F"
            set /a FILE_COUNT+=1
        )
    ) else (
        for /f "usebackq delims=" %%F in (`dir /b /a-d "%TARGET_DIR%\*.%%E" 2^>nul`) do (
            echo Formatting: %%F
            clang-format -i --style=%STYLE% "%%F"
            set /a FILE_COUNT+=1
        )
    )
)
 
echo.
echo Done. Files formatted: !FILE_COUNT!
 
endlocal