@echo off

set "CRASH_ROOT=%~dp0"

set "INSTALL_RELEASE=%CRASH_ROOT%install-release"
set "INSTALL_DEBUG=%CRASH_ROOT%install-debug"
set "BUILD_RELEASE=%CRASH_ROOT%build-release"
set "BUILD_DEBUG=%CRASH_ROOT%build-debug"

:: Remove old build and install folders
if exist "%INSTALL_RELEASE%" rmdir /S /Q "%INSTALL_RELEASE%"
if exist "%INSTALL_DEBUG%" rmdir /S /Q "%INSTALL_DEBUG%"
if exist "%BUILD_RELEASE%" rmdir /S /Q "%BUILD_RELEASE%"
if exist "%BUILD_DEBUG%" rmdir /S /Q "%BUILD_DEBUG%"

:: Build and install
cmd /c "build_windows_release.bat"
cmd /c "build_windows_debug.bat"

set "ORIGIN_RELEASE_DLL=%INSTALL_RELEASE%\bin\KalaCrashHandler.dll"
set "ORIGIN_RELEASE_LIB=%INSTALL_RELEASE%\lib\KalaCrashHandler.lib"
set "ORIGIN_DEBUG_DLL=%INSTALL_DEBUG%\bin\KalaCrashHandlerD.dll"
set "ORIGIN_DEBUG_LIB=%INSTALL_DEBUG%\lib\KalaCrashHandlerD.lib"
set "ORIGIN_HEADER=%CRASH_ROOT%\install-release\include\crashHandler.hpp"

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
if not exist "%ORIGIN_HEADER%" (
	echo Failed to find origin header from '%ORIGIN_HEADER%'!
	pause
	exit /b 1
)

set "TARGET_ROOT=%CRASH_ROOT%..\KalaWindow\_external_shared\KalaCrashHandler"

if not exist "%TARGET_ROOT%" (
	echo Failed to find target root from '%TARGET_ROOT%'!
	pause
	exit /b 1
)

set "TARGET_RELEASE_DLL=%TARGET_ROOT%\release\KalaCrashHandler.dll"
set "TARGET_RELEASE_LIB=%TARGET_ROOT%\release\KalaCrashHandler.lib"
set "TARGET_DEBUG_DLL=%TARGET_ROOT%\debug\KalaCrashHandlerD.dll"
set "TARGET_DEBUG_LIB=%TARGET_ROOT%\debug\KalaCrashHandlerD.lib"
set "TARGET_HEADER=%TARGET_ROOT%\crashHandler.hpp"

:: Create release and debug folders in case they dont exist yet
if not exist "%TARGET_ROOT%\release" mkdir "%TARGET_ROOT%\release"
if not exist "%TARGET_ROOT%\debug" mkdir "%TARGET_ROOT%\debug"

:: Copy dll files, lib files and header file to target path
copy /Y "%ORIGIN_RELEASE_DLL%" "%TARGET_RELEASE_DLL%"
copy /Y "%ORIGIN_RELEASE_LIB%" "%TARGET_RELEASE_LIB%"
copy /Y "%ORIGIN_DEBUG_DLL%" "%TARGET_DEBUG_DLL%"
copy /Y "%ORIGIN_DEBUG_LIB%" "%TARGET_DEBUG_LIB%"
copy /Y "%ORIGIN_HEADER%" "%TARGET_HEADER%"

echo Successfully installed KalaCrashHandler!

pause
exit /b 0
