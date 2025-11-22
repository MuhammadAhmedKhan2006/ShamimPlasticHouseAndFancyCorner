@echo off
echo Shamim Plastic House Management System - Build Script
echo =====================================================

REM Clean previous build files
echo Cleaning previous build...
if exist debug rmdir /s /q debug
if exist release rmdir /s /q release
if exist Makefile del Makefile
if exist Makefile.Debug del Makefile.Debug
if exist Makefile.Release del Makefile.Release
if exist *.pro.user del *.pro.user

echo Cleaned build directories.

REM Generate Makefiles
echo Generating Makefiles...
qmake ShopManagement.pro

if %errorlevel% neq 0 (
    echo ERROR: qmake failed. Please check if Qt is properly installed.
    pause
    exit /b 1
)

echo qmake completed successfully.

REM Build the project
echo Building project...
mingw32-make clean
mingw32-make -j4

if %errorlevel% neq 0 (
    echo ERROR: Build failed. Check the errors above.
    pause
    exit /b 1
)

echo Build completed successfully!
echo Executable location: debug\ShopManagement.exe

REM Try to run the application
echo.
echo Do you want to run the application? (y/n)
set /p choice=
if /i "%choice%"=="y" (
    echo Starting application...
    start debug\ShopManagement.exe
)

pause
