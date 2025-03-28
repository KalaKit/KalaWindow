@echo off

set "INPUT_ROOT=%~dp0"

set "INSTALL_RELEASE=%INPUT_ROOT%install-release"
set "INSTALL_DEBUG=%INPUT_ROOT%install-debug"
set "BUILD_RELEASE=%INPUT_ROOT%build-release"
set "BUILD_DEBUG=%INPUT_ROOT%build-debug"

:: Remove old build and install folders
if exist "%INSTALL_RELEASE%" rmdir /S /Q "%INSTALL_RELEASE%"
if exist "%INSTALL_DEBUG%" rmdir /S /Q "%INSTALL_DEBUG%"
if exist "%BUILD_RELEASE%" rmdir /S /Q "%BUILD_RELEASE%"
if exist "%BUILD_DEBUG%" rmdir /S /Q "%BUILD_DEBUG%"

:: Build and install
cmd /c "build_windows_release.bat"
cmd /c "build_windows_debug.bat"

set "TARGET_ROOT=%INPUT_ROOT%..\KalaTestProject\_external_shared\KalaWindow"

if not exist "%TARGET_ROOT%" (
	echo Failed to find target root from '%TARGET_ROOT%'!
	pause
	exit /b 1
)

:: Copy from origin and paste and overwrite in target
xcopy /E /Y /I "%INSTALL_RELEASE%\include" "%TARGET_ROOT%\"

echo Successfully installed KalaWindow!

pause
exit /b 0