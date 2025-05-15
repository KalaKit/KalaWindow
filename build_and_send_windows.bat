@echo off

set "PROJECT_ROOT=%~dp0"
cd "%PROJECT_ROOT%"

:: First build in release and debug

cmd /c "build_all_windows.bat"
if errorlevel 1 (
    exit /b 1
)

:: And then send to targets

cmd /c "testproj_windows.bat"
if errorlevel 1 (
    exit /b 1
)

cmd /c "video_windows.bat"
if errorlevel 1 (
    exit /b 1
)