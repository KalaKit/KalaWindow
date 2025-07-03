@echo off

cd /d "%~dp0"

set "INCLUDE_TARGET=..\KalaTestProject\_external_shared\KalaWindow\include"
set "DEBUG_OPENGL_TARGET=..\KalaTestProject\_external_shared\KalaWindow\debug_opengl"
set "DEBUG_VULKAN_TARGET=..\KalaTestProject\_external_shared\KalaWindow\debug_vulkan"
set "RELEASE_OPENGL_TARGET=..\KalaTestProject\_external_shared\KalaWindow\release_opengl"
set "RELEASE_VULKAN_TARGET=..\KalaTestProject\_external_shared\KalaWindow\release_vulkan"

if exist "%INCLUDE_TARGET%" (
	rmdir /S /Q "%INCLUDE_TARGET%"
)
if exist "%DEBUG_OPENGL_TARGET%" (
	rmdir /S /Q "%DEBUG_OPENGL_TARGET%"
)
mkdir "%DEBUG_OPENGL_TARGET%"
if exist "%DEBUG_VULKAN_TARGET%" (
	rmdir /S /Q "%DEBUG_VULKAN_TARGET%"
)
mkdir "%DEBUG_VULKAN_TARGET%"
if exist "%RELEASE_OPENGL_TARGET%" (
	rmdir /S /Q "%RELEASE_OPENGL_TARGET%"
)
mkdir "%RELEASE_OPENGL_TARGET%"
if exist "%RELEASE_VULKAN_TARGET%" (
	rmdir /S /Q "%RELEASE_VULKAN_TARGET%"
)
mkdir "%RELEASE_VULKAN_TARGET%"

set "INCLUDE_ORIGIN=include"

set "DEBUG_OPENGL_DLL=debug_opengl\KalaWindow_openglD.dll"
set "DEBUG_OPENGL_LIB=debug_opengl\KalaWindow_openglD.lib"
set "DEBUG_OPENGL_PDB=debug_opengl\KalaWindow_openglD.pdb"

set "DEBUG_VULKAN_DLL=debug_vulkan\KalaWindow_vulkanD.dll"
set "DEBUG_VULKAN_LIB=debug_vulkan\KalaWindow_vulkanD.lib"
set "DEBUG_VULKAN_PDB=debug_vulkan\KalaWindow_vulkanD.pdb"

set "RELEASE_OPENGL_DLL=release_opengl\KalaWindow_opengl.dll"
set "RELEASE_OPENGL_LIB=release_opengl\KalaWindow_opengl.lib"

set "RELEASE_VULKAN_DLL=release_vulkan\KalaWindow_vulkan.dll"
set "RELEASE_VULKAN_LIB=release_vulkan\KalaWindow_vulkan.lib"

if not exist "%INCLUDE_ORIGIN%" (
	echo WARNING: Cannot copy 'include origin' because it does not exist!
) else (
	xcopy "%INCLUDE_ORIGIN%" "%INCLUDE_TARGET%" /E /H /Y /I
)

call :SafeCopy "%DEBUG_OPENGL_DLL%" "%DEBUG_OPENGL_TARGET%" "debug opengl dll"
call :SafeCopy "%DEBUG_OPENGL_LIB%" "%DEBUG_OPENGL_TARGET%" "debug opengl lib"
call :SafeCopy "%DEBUG_OPENGL_PDB%" "%DEBUG_OPENGL_TARGET%" "debug opengl pdb"

call :SafeCopy "%DEBUG_VULKAN_DLL%" "%DEBUG_VULKAN_TARGET%" "debug vulkan dll"
call :SafeCopy "%DEBUG_VULKAN_LIB%" "%DEBUG_VULKAN_TARGET%" "debug vulkan lib"
call :SafeCopy "%DEBUG_VULKAN_PDB%" "%DEBUG_VULKAN_TARGET%" "debug vulkan pdb"

call :SafeCopy "%RELEASE_OPENGL_DLL%" "%RELEASE_OPENGL_TARGET%" "release opengl dll"
call :SafeCopy "%RELEASE_OPENGL_LIB%" "%RELEASE_OPENGL_TARGET%" "release opengl lib"

call :SafeCopy "%RELEASE_VULKAN_DLL%" "%RELEASE_VULKAN_TARGET%" "release vulkan dll"
call :SafeCopy "%RELEASE_VULKAN_LIB%" "%RELEASE_VULKAN_TARGET%" "release vulkan lib"

goto :Done

:SafeCopy
:: %1 = source file, %2 = target folder, %3 = description
if not exist "%~1" (
	echo WARNING: Cannot copy '%~3' because it does not exist!
) else (
	echo Copying "%~1" to "%~2"
	copy /Y "%~1" "%~2\" >nul
)
exit /b


:Done
echo Finished copying files and folders!

pause
exit /b 0