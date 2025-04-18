@echo off
setlocal enabledelayedexpansion

:: Set up Visual Studio build environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 (
    echo [ERROR] Failed to set up Visual Studio environment.
    goto :fail
)

set "SOURCE_DIR=%cd%"
set "BUILD_DEBUG=%SOURCE_DIR%\build-debug"
set "INSTALL_DEBUG=%SOURCE_DIR%\install-debug"
set "BUILD_RELEASE=%SOURCE_DIR%\build-release"
set "INSTALL_RELEASE=%SOURCE_DIR%\install-release"

echo Ensuring build/install directories exist...
for %%D in ("%BUILD_DEBUG%" "%INSTALL_DEBUG%" "%BUILD_RELEASE%" "%INSTALL_RELEASE%") do (
    if not exist %%D mkdir %%D
)

:: --- DEBUG STATIC ---
echo.
echo ===== Building Debug (Static) =====
cmake -B "%BUILD_DEBUG%" -S "%SOURCE_DIR%" ^
      -D CMAKE_BUILD_TYPE=Debug ^
      -D BUILD_SHARED_LIBS=OFF ^
      -G "Ninja"
if errorlevel 1 goto :fail

cmake --build "%BUILD_DEBUG%" --config Debug
if errorlevel 1 goto :fail

cmake --install "%BUILD_DEBUG%" --config Debug --prefix "%INSTALL_DEBUG%"
if errorlevel 1 goto :fail

:: --- DEBUG SHARED (.dll) ---
echo.
echo ===== Building Debug (Shared) =====
cmake -B "%BUILD_DEBUG%" -S "%SOURCE_DIR%" ^
      -D CMAKE_BUILD_TYPE=Debug ^
      -D BUILD_SHARED_LIBS=ON ^
      -G "Ninja"
if errorlevel 1 goto :fail

cmake --build "%BUILD_DEBUG%" --config Debug
if errorlevel 1 goto :fail

cmake --install "%BUILD_DEBUG%" --config Debug --prefix "%INSTALL_DEBUG%"
if errorlevel 1 goto :fail

:: --- RELEASE STATIC ---
echo.
echo ===== Building Release (Static) =====
cmake -B "%BUILD_RELEASE%" -S "%SOURCE_DIR%" ^
      -D CMAKE_BUILD_TYPE=Release ^
      -D BUILD_SHARED_LIBS=OFF ^
      -G "Ninja"
if errorlevel 1 goto :fail

cmake --build "%BUILD_RELEASE%" --config Release
if errorlevel 1 goto :fail

cmake --install "%BUILD_RELEASE%" --config Release --prefix "%INSTALL_RELEASE%"
if errorlevel 1 goto :fail

:: --- RELEASE SHARED (.dll) ---
echo.
echo ===== Building Release (Shared) =====
cmake -B "%BUILD_RELEASE%" -S "%SOURCE_DIR%" ^
      -D CMAKE_BUILD_TYPE=Release ^
      -D BUILD_SHARED_LIBS=ON ^
      -G "Ninja"
if errorlevel 1 goto :fail

cmake --build "%BUILD_RELEASE%" --config Release
if errorlevel 1 goto :fail

cmake --install "%BUILD_RELEASE%" --config Release --prefix "%INSTALL_RELEASE%"
if errorlevel 1 goto :fail

:: --- DONE ---
echo.
echo All builds completed successfully.
echo.
echo Static+Shared install folders:
echo   - Debug:   %INSTALL_DEBUG%
echo   - Release: %INSTALL_RELEASE%
echo.
pause
exit /b 0

:fail
echo.
echo [FATAL] Build process failed.
pause
exit /b 1
