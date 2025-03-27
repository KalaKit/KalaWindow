@echo off

set "WINDOW_ROOT=%~dp0"

set "INSTALL_RELEASE=%WINDOW_ROOT%install-release"
set "INSTALL_DEBUG=%WINDOW_ROOT%install-debug"
set "BUILD_RELEASE=%WINDOW_ROOT%build-release"
set "BUILD_DEBUG=%WINDOW_ROOT%build-debug"

:: Remove old build and install folders
if exist "%INSTALL_RELEASE%" rmdir /S /Q "%INSTALL_RELEASE%"
if exist "%INSTALL_DEBUG%" rmdir /S /Q "%INSTALL_DEBUG%"
if exist "%BUILD_RELEASE%" rmdir /S /Q "%BUILD_RELEASE%"
if exist "%BUILD_DEBUG%" rmdir /S /Q "%BUILD_DEBUG%"

:: Build and install
cmd /c "build_windows_release.bat"
cmd /c "build_windows_debug.bat"

echo Successfully installed KalaWindow!

pause
exit /b 0