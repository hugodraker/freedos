@echo off
echo ==========================================
echo    VHD NTFS Creator & File Populator
echo ==========================================
echo.

:: Define the path for the new VHD (saves in the same folder as this script)
set VHD_PATH=%~dp0TinyDisk_NTFS.vhd

echo Creating diskpart instructions...
(
echo create vdisk file="%VHD_PATH%" maximum=10 type=fixed
echo select vdisk file="%VHD_PATH%"
echo attach vdisk
echo create partition primary
echo format fs=ntfs quick label="TinyNTFS"
echo assign letter=V
) > "%TEMP%\vhd_script.txt"

echo Running DiskPart to build and format the VHD...
diskpart /s "%TEMP%\vhd_script.txt"

:: Cleanup the temporary diskpart script
del "%TEMP%\vhd_script.txt"

echo.
echo Populating VHD with dummy text files on drive V:...
if exist V:\ (
    echo This is the content for the first dummy text file. > V:\dummy_file_one.txt
    echo This is the content for the second dummy text file. > V:\dummy_file_two.txt
    echo [OK] Two dummy text files created successfully on V:\
) else (
    echo [ERROR] Drive V: could not be accessed automatically.
)

echo.
echo ==========================================
echo Process complete! VHD is located at:
echo %VHD_PATH%
echo ==========================================
pause