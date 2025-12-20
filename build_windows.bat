@echo off
setlocal

:: Set the working directory to the script's location
set "PROJECT_ROOT=%~dp0"
cd "%PROJECT_ROOT%"

:: =====================================
:: [MSVC Setup] Ensure cl.exe is available for Ninja
:: =====================================
set "VS_MAIN=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
set "VS_SIDE=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

set "VS_INSIDERS_18_MAIN=C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
set "VS_INSIDERS_18_SIDE=C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

if exist "%VS_MAIN%" (
    call "%VS_MAIN%"
) else if exist "%VS_SIDE%" (
    call "%VS_SIDE%"
) else if exist "%VS_INSIDERS_18_MAIN%" (
    call "%VS_INSIDERS_18_MAIN%"
) else if exist "%VS_INSIDERS_18_SIDE%" (
    call "%VS_INSIDERS_18_SIDE%"
)else (
    echo [ERROR] Could not find vcvars64.bat from either Visual Studio Community or Build Tools.
    echo Please install Visual Studio or Build Tools and try again.
    pause
    exit /b 1
)

:: ================================
:: Run builds
:: ================================
call :BuildWithPreset debug Debug
call :BuildWithPreset release Release

echo =====================================
echo [SUCCESS] Finished building and installing!
echo =====================================
echo.
pause
exit /b 0

:: ================================
:: Build with a given preset
:: %1 = preset name, %2 = config
:: ================================
:BuildWithPreset
if "%~1"=="" (
    echo [ERROR] No preset name provided to BuildWithPreset!
    pause
    exit /b 1
)

set "PRESET=%~1"
set "CONFIG=%~2"

echo =====================================
echo [INFO] Building in %PRESET% (%CONFIG%) mode...
echo =====================================
echo.

echo [INFO] Configuring with preset: %PRESET%
cmake --preset %PRESET%
if errorlevel 1 (
    echo [ERROR] Configuration failed
    pause
    exit /b 1
)

echo [INFO] Building with preset: %PRESET% and config: %CONFIG%
cmake --build --preset %PRESET% --config %CONFIG%
if errorlevel 1 (
    echo [ERROR] Build failed
    pause
    exit /b 1
)

echo.
echo [SUCCESS] Finished building preset: %PRESET%
echo.
exit /b 0
