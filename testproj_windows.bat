@echo off

set "PROJECT_ROOT=%~dp0"
cd "%PROJECT_ROOT%"

set "BUILD_RELEASE=%PROJECT_ROOT%build-release"
set "BUILD_DEBUG=%PROJECT_ROOT%build-debug"

set "ORIGIN_RELEASE_DLL=%BUILD_RELEASE%\bin\KalaWindow.dll"
set "ORIGIN_RELEASE_LIB=%BUILD_RELEASE%\lib\KalaWindow.lib"
set "ORIGIN_DEBUG_DLL=%BUILD_DEBUG%\bin\KalaWindowD.dll"
set "ORIGIN_DEBUG_LIB=%BUILD_DEBUG%\lib\KalaWindowD.lib"
set "ORIGIN_FOLDER=%PROJECT_ROOT%\build-release\include"

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
if not exist "%ORIGIN_FOLDER%" (
	echo Failed to find origin folder from '%ORIGIN_FOLDER%'!
	pause
	exit /b 1
)

set "TARGET_ROOT=%PROJECT_ROOT%..\KalaTestProject\_external_shared\KalaWindow"

if not exist "%TARGET_ROOT%" (
	echo Failed to find target root from '%TARGET_ROOT%'!
	pause
	exit /b 1
)

set "TARGET_RELEASE_DLL=%TARGET_ROOT%\release\KalaWindow.dll"
set "TARGET_RELEASE_LIB=%TARGET_ROOT%\release\KalaWindow.lib"
set "TARGET_DEBUG_DLL=%TARGET_ROOT%\debug\KalaWindowD.dll"
set "TARGET_DEBUG_LIB=%TARGET_ROOT%\debug\KalaWindowD.lib"
set "TARGET_FOLDER=%TARGET_ROOT%"

:: Create release and debug folders in case they dont exist yet
if not exist "%TARGET_ROOT%\release" mkdir "%TARGET_ROOT%\release"
if not exist "%TARGET_ROOT%\debug" mkdir "%TARGET_ROOT%\debug"

:: Copy dll files, lib files and header file to target path
copy /Y "%ORIGIN_RELEASE_DLL%" "%TARGET_RELEASE_DLL%"
copy /Y "%ORIGIN_RELEASE_LIB%" "%TARGET_RELEASE_LIB%"
copy /Y "%ORIGIN_DEBUG_DLL%" "%TARGET_DEBUG_DLL%"
copy /Y "%ORIGIN_DEBUG_LIB%" "%TARGET_DEBUG_LIB%"
xcopy "%ORIGIN_FOLDER%" "%TARGET_FOLDER%" /E /I /Y

pause
exit /b 0
