@echo off

set "PROJECT_ROOT=%~dp0"
cd "%PROJECT_ROOT%"

set CLEAN_ARG=
if "%1" == "clean" set CLEAN_ARG=clean

set "BUILD_RELEASE=%PROJECT_ROOT%build-release"
set "BUILD_DEBUG=%PROJECT_ROOT%build-debug"

:: Remove old build folders
if exist "%BUILD_RELEASE%" rmdir /S /Q "%BUILD_RELEASE%"
if exist "%BUILD_DEBUG%" rmdir /S /Q "%BUILD_DEBUG%"

echo =====================================
echo [INFO] Copying external shared files...
echo =====================================
echo.

cd _external_shared
cmd /c "copy_windows.bat"
if errorlevel 1 (
    echo [ERROR] Copy failed.
    pause
    exit /b 1
)
cd ..

echo =====================================
echo [INFO] Building KalaWindow in Release mode...
echo =====================================
echo.

cmd /c "build_windows_release.bat %CLEAN_ARG%"
if errorlevel 1 (
    echo [ERROR] Release build failed.
    pause
    exit /b 1
)

echo.
echo =====================================
echo [INFO] Building KalaWindow in Debug mode...
echo =====================================
echo.

cmd /c "build_windows_debug.bat %CLEAN_ARG%"
if errorlevel 1 (
    echo [ERROR] Debug build failed.
    pause
    exit /b 1
)

echo.
echo =====================================
echo [SUCCESS] Finished building and installing KalaWindow!
echo =====================================
echo.