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
set "ORIGIN_HEADER1=%INPUT_ROOT%\install-release\include\window.hpp"
set "ORIGIN_HEADER2=%INPUT_ROOT%\install-release\include\input.hpp"
set "ORIGIN_HEADER4=%INPUT_ROOT%\install-release\include\opengl.hpp"
set "ORIGIN_HEADER3=%INPUT_ROOT%\install-release\include\messageloop.hpp"
set "ORIGIN_HEADER5=%INPUT_ROOT%\install-release\include\enums.hpp"
set "ORIGIN_HEADER6=%INPUT_ROOT%\install-release\include\magic_enum.hpp"

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
if not exist "%ORIGIN_HEADER1%" (
	echo Failed to find origin header 1 from '%ORIGIN_HEADER1%'!
	pause
	exit /b 1
)
if not exist "%ORIGIN_HEADER2%" (
	echo Failed to find origin header 2 from '%ORIGIN_HEADER2%'!
	pause
	exit /b 1
)
if not exist "%ORIGIN_HEADER3%" (
	echo Failed to find origin header 3 from '%ORIGIN_HEADER3%'!
	pause
	exit /b 1
)
if not exist "%ORIGIN_HEADER4%" (
	echo Failed to find origin header 4 from '%ORIGIN_HEADER4%'!
	pause
	exit /b 1
)
if not exist "%ORIGIN_HEADER5%" (
	echo Failed to find origin header 5 from '%ORIGIN_HEADER5%'!
	pause
	exit /b 1
)
if not exist "%ORIGIN_HEADER6%" (
	echo Failed to find origin header 5 from '%ORIGIN_HEADER6%'!
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
set "TARGET_HEADER1=%TARGET_ROOT%\window.hpp"
set "TARGET_HEADER2=%TARGET_ROOT%\input.hpp"
set "TARGET_HEADER3=%TARGET_ROOT%\opengl.hpp"
set "TARGET_HEADER4=%TARGET_ROOT%\messageloop.hpp"
set "TARGET_HEADER5=%TARGET_ROOT%\enums.hpp"
set "TARGET_HEADER6=%TARGET_ROOT%\magic_enum.hpp"

:: Create release and debug folders in case they dont exist yet
if not exist "%TARGET_ROOT%\release" mkdir "%TARGET_ROOT%\release"
if not exist "%TARGET_ROOT%\debug" mkdir "%TARGET_ROOT%\debug"

:: Copy dll files, lib files and header file to target path
copy /Y "%ORIGIN_RELEASE_DLL%" "%TARGET_RELEASE_DLL%"
copy /Y "%ORIGIN_RELEASE_LIB%" "%TARGET_RELEASE_LIB%"
copy /Y "%ORIGIN_DEBUG_DLL%" "%TARGET_DEBUG_DLL%"
copy /Y "%ORIGIN_DEBUG_LIB%" "%TARGET_DEBUG_LIB%"
copy /Y "%ORIGIN_HEADER1%" "%TARGET_HEADER1%"
copy /Y "%ORIGIN_HEADER2%" "%TARGET_HEADER2%"
copy /Y "%ORIGIN_HEADER3%" "%TARGET_HEADER3%"
copy /Y "%ORIGIN_HEADER4%" "%TARGET_HEADER4%"
copy /Y "%ORIGIN_HEADER5%" "%TARGET_HEADER5%"
copy /Y "%ORIGIN_HEADER6%" "%TARGET_HEADER6%"

echo Successfully installed KalaWindow!

pause
exit /b 0