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

set "ORIGIN_RELEASE_DLL=%INSTALL_RELEASE%\bin\KalaWindow.dll"
set "ORIGIN_RELEASE_LIB=%INSTALL_RELEASE%\lib\KalaWindow.lib"
set "ORIGIN_DEBUG_DLL=%INSTALL_DEBUG%\bin\KalaWindowD.dll"
set "ORIGIN_DEBUG_LIB=%INSTALL_DEBUG%\lib\KalaWindowD.lib"

if not exist "%ORIGIN_RELEASE_DLL%" (
	echo Failed to find origin release dll from '%ORIGIN_RELEASE_DLL%'!
	pause
	exit /b 1
)
if not exist "%ORIGIN_RELEASE_LIB%" (
	echo Failed to find origin release lib from '%ORIGIN_RELEASE_LIB%'!
	pause
	exit /b 1
)
if not exist "%ORIGIN_DEBUG_DLL%" (
	echo Failed to find origin debug dll from '%ORIGIN_DEBUG_DLL%'!
	pause
	exit /b 1
)
if not exist "%ORIGIN_DEBUG_LIB%" (
	echo Failed to find origin debug lib from '%ORIGIN_DEBUG_LIB%'!
	pause
	exit /b 1
)

set "TARGET_ROOT=%INPUT_ROOT%..\KalaTestProject\_external_shared\KalaWindow"

if not exist "%TARGET_ROOT%" (
	echo Failed to find target root from '%TARGET_ROOT%'!
	pause
	exit /b 1
)

set "TARGET_RELEASE_DLL=%TARGET_ROOT%\release\KalaWindow.dll"
set "TARGET_RELEASE_LIB=%TARGET_ROOT%\release\KalaWindow.lib"
set "TARGET_DEBUG_DLL=%TARGET_ROOT%\debug\KalaWindowD.dll"
set "TARGET_DEBUG_LIB=%TARGET_ROOT%\debug\KalaWindowD.lib"

:: Copy dll files to target path
copy /Y "%ORIGIN_RELEASE_DLL%" "%TARGET_RELEASE_DLL%"
copy /Y "%ORIGIN_RELEASE_LIB%" "%TARGET_RELEASE_LIB%"
copy /Y "%ORIGIN_DEBUG_DLL%" "%TARGET_DEBUG_DLL%"
copy /Y "%ORIGIN_DEBUG_LIB%" "%TARGET_DEBUG_LIB%"

:: Copy header files and inl files to target path
xcopy /E /Y /I "%INSTALL_RELEASE%\include" "%TARGET_ROOT%\"

echo Successfully installed KalaWindow!

pause
exit /b 0