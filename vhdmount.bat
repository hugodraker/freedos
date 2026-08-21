@echo off
setlocal

:: 1. Check for Administrator privileges upfront and elevate if needed
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo Elevating to Administrator...
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\elevate.vbs"
    
    :: Re-pass all original command-line parameters cleanly to the elevated instance
    if /i "%~1"=="-u" (
        echo UAC.ShellExecute "%~s0", "-u " ^& """" ^& "%~f2" ^& """", "", "runas", 1 >> "%temp%\elevate.vbs"
    ) else (
        echo UAC.ShellExecute "%~s0", """" ^& "%~f1" ^& """", "", "runas", 1 >> "%temp%\elevate.vbs"
    )
    
    cscript //nologo "%temp%\elevate.vbs"
    del "%temp%\elevate.vbs"
    exit /b
)

:: 2. Validate parameters
if "%~1"=="" (
    echo Usage:
    echo   %~nx0 "C:\path\to\disk.vhd"       (Mounts the VHD)
    echo   %~nx0 -u "C:\path\to\disk.vhd"     (Unmounts the VHD)
    pause
    exit /b 1
)

:: 3. Determine Mode and Target Path strictly based on position
if /i "%~1"=="-u" (
    set "ACTION=UNMOUNT"
    set "VHD_PATH=%~f2"
) else (
    set "ACTION=MOUNT"
    set "VHD_PATH=%~f1"
)

if "%VHD_PATH%"=="" (
    echo [ERROR] No VHD path provided.
    pause
    exit /b 1
)

if not exist "%VHD_PATH%" (
    echo [ERROR] File not found: "%VHD_PATH%"
    pause
    exit /b 1
)

:: 4. Build and execute DiskPart script
if "%ACTION%"=="UNMOUNT" (
    echo Unmounting VHD: "%VHD_PATH%" ...
    (
    echo select vdisk file="%VHD_PATH%"
    echo detach vdisk
    ) > "%TEMP%\vhd_action.txt"
) else (
    echo Mounting VHD: "%VHD_PATH%" ...
    (
    echo select vdisk file="%VHD_PATH%"
    echo attach vdisk
    ) > "%TEMP%\vhd_action.txt"
)

diskpart /s "%TEMP%\vhd_action.txt"
del "%TEMP%\vhd_action.txt"

echo.
echo [OK] Action completed successfully.
pause
endlocal