@echo off

cd /d "%~dp0"

set "EXTERNAL_TARGET=..\Circuit-chan\_external_shared"
set "DEBUG_TARGET=..\Circuit-chan\_external_shared\KalaWindow\debug"
set "RELEASE_TARGET=..\Circuit-chan\_external_shared\KalaWindow\release"

set "DOCS_ORIGIN=docs"
set "INCLUDE_ORIGIN=include"
set "GLM_ORIGIN=_external_shared\glm"
set "STB_ORIGIN=_external_shared\stb"
set "IMGUI_ORIGIN=_external_shared\imgui"
set "MINIAUDIO_ORIGIN=_external_shared\miniaudio"
set "OPENGL_ORIGIN=_external_shared\OpenGL"

set "DEBUG_DLL=debug\_KWnd03d.dll"
set "DEBUG_LIB=debug\_KWnd03d.lib"
set "DEBUG_PDB=debug\_KWnd03d.pdb"

set "RELEASE_DLL=release\_KWnd03.dll"
set "RELEASE_LIB=release\_KWnd03.lib"

set "README_FILE=README.md"
set "LICENSE_FILE=LICENSE.md"
set "LOGO_FILE=logo.png"

set "CLEAN_IMGUI=..\Circuit-chan\_external_shared\delete_imgui_vulkan_files.bat"

:: =====================================
:: Copy new files
:: =====================================

::Docs target is always overwritten
call :CopyFolder "%DOCS_ORIGIN%" "%EXTERNAL_TARGET%\KalaWindow\docs" "docs origin" "yes"

::Include target is always overwritten
call :CopyFolder "%INCLUDE_ORIGIN%" "%EXTERNAL_TARGET%\KalaWindow\include" "include origin" "yes"

call :CopyFolder "%GLM_ORIGIN%" "%EXTERNAL_TARGET%\glm" "glm"
call :CopyFolder "%STB_ORIGIN%" "%EXTERNAL_TARGET%\stb" "stb"
call :CopyFolder "%IMGUI_ORIGIN%" "%EXTERNAL_TARGET%\imgui" "imgui"
call :CopyFolder "%MINIAUDIO_ORIGIN%" "%EXTERNAL_TARGET%\miniaudio" "miniaudio"

call :CopyFolder "%OPENGL_ORIGIN%" "%EXTERNAL_TARGET%\OpenGL" "OpenGL"

call :CopyFile "%DEBUG_DLL%" "%DEBUG_TARGET%" "debug dll" "yes"
call :CopyFile "%DEBUG_LIB%" "%DEBUG_TARGET%" "debug lib" "yes"
call :CopyFile "%DEBUG_PDB%" "%DEBUG_TARGET%" "debug pdb" "yes"

call :CopyFile "%RELEASE_DLL%" "%RELEASE_TARGET%" "release dll" "yes"
call :CopyFile "%RELEASE_LIB%" "%RELEASE_TARGET%" "release lib" "yes"

call :CopyFile "%README_FILE%" "%EXTERNAL_TARGET%\KalaWindow" "readme file" "yes"
call :CopyFile "%LICENSE_FILE%" "%EXTERNAL_TARGET%\KalaWindow" "license file" "yes"
call :CopyFile "%LOGO_FILE%" "%EXTERNAL_TARGET%\KalaWindow" "logo file" "yes"

goto :Done

:: =====================================
:: Copy and optionally overwrite a folder
:: %1 = origin folder path, %2 = target folder path, %3 = title, %4 = overwrite (yes or no)
:: =====================================
:CopyFolder
if not exist "%~1" (
	echo WARNING: Cannot copy '%~3' because it does not exist!
) else (
	if "%~4"=="yes" (
		echo Copying "%~1" to "%~2"
		xcopy "%~1" "%~2" /E /H /Y /I >nul
	) else (
		if exist "%~2" (
			echo Skipping copying '%~3' because it already exists!
		) else (
			echo Copying "%~1" to "%~2"
			xcopy "%~1" "%~2" /E /H /Y /I >nul
		)
	)
)

exit /b

:: =====================================
:: Copy and optionally overwrite a file
:: %1 = origin file path, %2 = target file path, %3 = title, %4 = overwrite (yes or no)
:: =====================================
:CopyFile
if not exist "%~1" (
	echo WARNING: Cannot copy '%~3' because it does not exist!
) else (
	if "%~4"=="yes" (
		echo Copying "%~1" to "%~2"
		copy /Y "%~1" "%~2" >nul
	) else (
		if exist "%~2" (
			echo Skipping copying '%~3' because it already exists!
		) else (
			echo Copying "%~1" to "%~2"
			copy /Y "%~1" "%~2" >nul
		)
	)
)

exit /b

:Done
echo Finished copying Circuit Chan files and folders!

::Remove Vulkan files from imgui backends folder if any exist
call "%CLEAN_IMGUI%"

pause
exit /b 0