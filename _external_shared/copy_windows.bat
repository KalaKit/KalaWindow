@echo off

set "PROJECT_NAME=KalaWindow"

echo Starting to copy %PROJECT_NAME%...
echo.

set "PROJECT_ROOT=%~dp0"

set "CRASH_RELEASE_DLL_ORIGIN=%PROJECT_ROOT%\KalaCrashHandler\release\KalaCrashHandler.dll"
set "CRASH_DEBUG_DLL_ORIGIN=%PROJECT_ROOT%\KalaCrashHandler\debug\KalaCrashHandlerD.dll"

set "FREETYPE_RELEASE_DLL_ORIGIN=%PROJECT_ROOT%\FreeType\release\freetype.dll"
set "FREETYPE_DEBUG_DLL_ORIGIN=%PROJECT_ROOT%\FreeType\debug\freetyped.dll"

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

if not exist "%FREETYPE_RELEASE_DLL_ORIGIN%" (
	echo Failed to find freetype origin release dll from '%FREETYPE_RELEASE_DLL_ORIGIN%'!
	pause
	exit /b 1
)
if not exist "%FREETYPE_DEBUG_DLL_ORIGIN%" (
	echo Failed to find freetype origin debug dll from '%FREETYPE_DEBUG_DLL_ORIGIN%'!
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

set "FREETYPE_RELEASE_DLL_TARGET=%TARGET_ROOT%\release\freetype.dll"
set "FREETYPE_DEBUG_DLL_TARGET=%TARGET_ROOT%\debug\freetyped.dll"

:: Copy dll to target path
copy /Y "%CRASH_RELEASE_DLL_ORIGIN%" "%CRASH_RELEASE_DLL_TARGET%"
copy /Y "%CRASH_DEBUG_DLL_ORIGIN%" "%CRASH_DEBUG_DLL_TARGET%"

copy /Y "%FREETYPE_RELEASE_DLL_ORIGIN%" "%FREETYPE_RELEASE_DLL_TARGET%"
copy /Y "%FREETYPE_DEBUG_DLL_ORIGIN%" "%FREETYPE_DEBUG_DLL_TARGET%"

echo.
echo Finished copying %PROJECT_NAME% DLLs!

pause
exit /b 0
