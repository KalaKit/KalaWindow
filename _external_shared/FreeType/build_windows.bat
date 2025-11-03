@echo off
setlocal

:: Set the working directory to the script's location
set "PROJECT_ROOT=%~dp0"
cd "%PROJECT_ROOT%"

if "%PROJECT_ROOT:~-1%"=="\" set "PROJECT_ROOT=%PROJECT_ROOT:~0,-1%"

:: =====================================
:: [MSVC Setup] Ensure cl.exe is available for Ninja
:: =====================================

set "VS_MAIN=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
set "VS_SIDE=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
set "VS_INSIDERS_18_MAIN=C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat"
set "VS_INSIDERS_18_SIDE=C:\Program Files (x86)\Microsoft Visual Studio\18\Insiders\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

if exist "%VS_MAIN%" (
    call "%VS_MAIN%"
) else if exist "%VS_SIDE%" (
    call "%VS_SIDE%"
) else if exist "%VS_INSIDERS_18_MAIN%" (
    call "%VS_INSIDERS_18_MAIN%"
) else if exist "%VS_INSIDERS_18_SIDE%" (
    call "%VS_INSIDERS_18_SIDE%"
) else (
    echo [ERROR] Could not find vcvars64.bat from either Visual Studio Community or Build Tools.
    echo Please install Visual Studio or Build Tools and try again.
    pause
    exit /b 1
)

set "FREETYPE_BUILD=%PROJECT_ROOT%\build"
set "FREETYPE_INSTALL=%PROJECT_ROOT%\install"

:: =====================================
:: Build Release
:: =====================================

echo === Configuring FreeType (Release) ===
cmake -G "Ninja" -B "%FREETYPE_BUILD%\Release" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="%FREETYPE_INSTALL%\Release" -DBUILD_SHARED_LIBS=ON -DFT_DISABLE_ZLIB=TRUE -DFT_DISABLE_PNG=TRUE -DFT_DISABLE_BZIP2=TRUE -DFT_DISABLE_BROTLI=TRUE -S "%PROJECT_ROOT%."

if errorlevel 1 (
    echo [ERROR] CMake configuration failed for Release.
    pause
    exit /b %errorlevel%
)

echo.
echo === Building FreeType (Release) ===
cmake --build "%FREETYPE_BUILD%\Release" --config Release --target install
if errorlevel 1 (
    echo [ERROR] Build or install failed for Release.
    pause
    exit /b %errorlevel%
)

:: =====================================
:: Build Debug
:: =====================================

echo.
echo === Configuring FreeType (Debug) ===
cmake -G "Ninja" -B "%FREETYPE_BUILD%\Debug" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX="%FREETYPE_INSTALL%\Debug" -DBUILD_SHARED_LIBS=ON -DFT_DISABLE_ZLIB=TRUE -DFT_DISABLE_PNG=TRUE -DFT_DISABLE_BZIP2=TRUE -DFT_DISABLE_BROTLI=TRUE -S "%PROJECT_ROOT%."

if errorlevel 1 (
    echo [ERROR] CMake configuration failed for Debug.
    pause
    exit /b %errorlevel%
)

echo.
echo === Building FreeType (Debug) ===
cmake --build "%FREETYPE_BUILD%\Debug" --config Debug --target install
if errorlevel 1 (
    echo [ERROR] Build or install failed for Debug.
    pause
    exit /b %errorlevel%
)

echo.
echo Finished building FreeType!

pause
endlocal