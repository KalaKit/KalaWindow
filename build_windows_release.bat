@echo off

set "PROJECT_ROOT=%~dp0"
cd "%PROJECT_ROOT%"

set "BUILD_DIR=%PROJECT_ROOT%build-release"
set "INSTALL_DIR=%PROJECT_ROOT%install-release"

set "PRESET=windows-release"
set "INSTALLER_BUILD_DIR=build-release"

if exist "%BUILD_DIR%" (
    echo [INFO] Removing existing build directory
    rmdir /s /q "%BUILD_DIR%"
)

if exist "%INSTALL_DIR%" (
    echo [INFO] Removing existing install directory
    rmdir /s /q "%INSTALL_DIR%"
)

:: Ensure Visual Studio environment is set up correctly
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" || (
    echo [ERROR] Failed to set up Visual Studio environment.
    exit /b 1
)

:: Record start time
for /f "tokens=1-4 delims=:.," %%a in ("%TIME%") do set "TIME_START=%%a:%%b:%%c"

echo [INFO] Configuring with preset: %PRESET%
cmake --preset %PRESET%
if errorlevel 1 (
    echo [ERROR] Configuration failed
	pause
    exit /b 1
)

echo [INFO] Building with preset: %PRESET%
cmake --build --preset %PRESET%
if errorlevel 1 (
    echo [ERROR] Build failed
	pause
    exit /b 1
)

echo [INFO] Installing to "%INSTALLER_BUILD_DIR%"
cmake --install "%INSTALLER_BUILD_DIR%"
if errorlevel 1 (
    echo [ERROR] Install failed
	pause
    exit /b 1
)

:: Record end time
for /f "tokens=1-4 delims=:.," %%a in ("%TIME%") do set "TIME_END=%%a:%%b:%%c"

:: Success message
echo [SUCCESS] Project built and installed successfully.
echo ---------------------------------------------
echo Shared library: "%INSTALL_DIR%\bin"
echo Static library: "%INSTALL_DIR%\lib"
echo Include headers: "%INSTALL_DIR%\include"
echo Build duration: %TIME_START% - %TIME_END%
echo ---------------------------------------------

pause
exit /b 0
