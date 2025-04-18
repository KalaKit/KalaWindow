@echo off

set "PROJECT_ROOT=%~dp0"
cd "%PROJECT_ROOT%"

set "BUILD_DIR=%PROJECT_ROOT%build-debug"

set "PRESET=windows-debug"

if exist "%BUILD_DIR%" (
	echo [INFO] Removing existing build directory
	rmdir /s /q "%BUILD_DIR%"
)

:: Ensure Visual Studio environment is set up correctly
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" ||(
    echo [ERROR] Failed to set up Visual Studio environment.
    exit /b 1
)

:: Record start time
for /f "tokens=1-4 delims=:.," %%a in ("%TIME%") do set "TIME_START=%%a:%%b:%%c"

echo [INFO] Configuring with preset: %PRESET%
cmake --preset %PRESET%
if errorlevel 1 (
    echo [ERROR] Configuration failed
	if /i "%1"=="pause" (
	    pause
	)
    exit /b 1
)

echo [INFO] Building with preset: %PRESET%
cmake --build --preset %PRESET%
if errorlevel 1 (
    echo [ERROR] Build failed
	if /i "%1"=="pause" (
	    pause
	)
    exit /b 1
)

:: Record end time
for /f "tokens=1-4 delims=:.," %%a in ("%TIME%") do set "TIME_END=%%a:%%b:%%c"

:: Success message
echo [SUCCESS] Project built and installed successfully.
echo ---------------------------------------------
echo Shared library: "%BUILD_DIR%\bin"
echo Static library: "%BUILD_DIR%"
echo Include headers: "%BUILD_DIR%\include"
echo Build duration: %TIME_START% - %TIME_END%
echo ---------------------------------------------

if /i "%1"=="pause" (
	pause
)
exit /b 0
