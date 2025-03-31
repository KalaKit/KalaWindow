@echo off

set "PROJECT_NAME=KalaWindow"

echo Starting to copy %PROJECT_NAME%...
echo.

set "PROJECT_ROOT=%~dp0"

set "CRASH_RELEASE_DLL_ORIGIN=%PROJECT_ROOT%\KalaCrashHandler\release\KalaCrashHandler.dll"
set "CRASH_DEBUG_DLL_ORIGIN=%PROJECT_ROOT%\KalaCrashHandler\debug\KalaCrashHandlerD.dll"

set "UTILS_RELEASE_DLL_ORIGIN=%PROJECT_ROOT%\KalaUtils\release\KalaUtils.dll"
set "UTILS_DEBUG_DLL_ORIGIN=%PROJECT_ROOT%\KalaUtils\debug\KalaUtilsD.dll"

if not exist "%CRASH_RELEASE_DLL_ORIGIN%" (
	echo Failed to find crash origin release dll from '%CRASH_RELEASE_DLL_ORIGIN%'!
	pause
	exit /b 1
)
if not exist "%CRASH_DEBUG_DLL_ORIGIN%" (
	echo Failed to find crash origin debug dll from '%CRASH_DEBUG_DLL_ORIGIN%'!
	pause
	exit /b 1
)

if not exist "%UTILS_RELEASE_DLL_ORIGIN%" (
	echo Failed to find utils origin release dll from '%WINDOW_RELEASE_DLL_ORIGIN%'!
	pause
	exit /b 1
)
if not exist "%UTILS_DEBUG_DLL_ORIGIN%" (
	echo Failed to find utils origin debug dll from '%WINDOW_DEBUG_DLL_ORIGIN%'!
	pause
	exit /b 1
)

set "TARGET_ROOT=%PROJECT_ROOT%..\files\external dlls"

if not exist "%TARGET_ROOT%" (
	echo Failed to find target root from '%TARGET_ROOT%'!
	pause
	exit /b 1
)

if not exist "%TARGET_ROOT%\release" mkdir "%TARGET_ROOT%\release"
if not exist "%TARGET_ROOT%\debug" mkdir "%TARGET_ROOT%\debug"

set "CRASH_RELEASE_DLL_TARGET=%TARGET_ROOT%\release\KalaCrashHandler.dll"
set "CRASH_DEBUG_DLL_TARGET=%TARGET_ROOT%\debug\KalaCrashHandlerD.dll"

set "UTILS_RELEASE_DLL_TARGET=%TARGET_ROOT%\release\KalaUtils.dll"
set "UTILS_DEBUG_DLL_TARGET=%TARGET_ROOT%\debug\KalaUtilsD.dll"

:: Copy dll to target path
copy /Y "%CRASH_RELEASE_DLL_ORIGIN%" "%CRASH_RELEASE_DLL_TARGET%"
copy /Y "%CRASH_DEBUG_DLL_ORIGIN%" "%CRASH_DEBUG_DLL_TARGET%"

copy /Y "%UTILS_RELEASE_DLL_ORIGIN%" "%UTILS_RELEASE_DLL_TARGET%"
copy /Y "%UTILS_DEBUG_DLL_ORIGIN%" "%UTILS_DEBUG_DLL_TARGET%"

echo.
echo Finished copying %PROJECT_NAME% DLLs!

pause
exit /b 0