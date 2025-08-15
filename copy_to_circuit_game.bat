@echo off

cd /d "%~dp0"

:: =====================================
:: Clear old junk
:: =====================================

set "INCLUDE_TARGET=..\circuit_game\_external_shared\KalaWindow\include"
set "DEBUG_TARGET=..\circuit_game\_external_shared\KalaWindow\debug"
set "RELEASE_TARGET=..\circuit_game\_external_shared\KalaWindow\release"

if exist "%INCLUDE_TARGET%" (
	rmdir /S /Q "%INCLUDE_TARGET%\core"
	rmdir /S /Q "%INCLUDE_TARGET%\graphics"
	rmdir /S /Q "%INCLUDE_TARGET%\windows"
)

if exist "%DEBUG_TARGET%" (
	rmdir /S /Q "%DEBUG_TARGET%"
)
mkdir "%DEBUG_TARGET%"

if exist "%RELEASE_TARGET%" (
	rmdir /S /Q "%RELEASE_TARGET%"
)
mkdir "%RELEASE_TARGET%"

:: =====================================
:: Copy new files
:: =====================================

set "INCLUDE_ORIGIN=include"
set "GLM_ORIGIN=_external_shared\glm"
set "STB_IMAGE_ORIGIN=_external_shared\stb_image"
set "IMGUI_ORIGIN=_external_shared\imgui"
set "MINIAUDIO_ORIGIN=_external_shared\miniaudio"

set "DEBUG_DLL=debug\KalaWindowD.dll"
set "DEBUG_LIB=debug\KalaWindowD.lib"
set "DEBUG_PDB=debug\KalaWindowD.pdb"

set "RELEASE_DLL=release\KalaWindow.dll"
set "RELEASE_LIB=release\KalaWindow.lib"

::Include target is always overwritten
call :CopyFolder "%INCLUDE_ORIGIN%" "%INCLUDE_TARGET%" "include origin" "yes"

call :CopyFolder "%GLM_ORIGIN%" "%INCLUDE_TARGET%\glm" "glm"
call :CopyFolder "%STB_IMAGE_ORIGIN%" "%INCLUDE_TARGET%\stb_image" "stb_image"
call :CopyFolder "%IMGUI_ORIGIN%" "%INCLUDE_TARGET%\imgui" "imgui"
call :CopyFolder "%MINIAUDIO_ORIGIN%" "%INCLUDE_TARGET%\miniaudio" "miniaudio"

call :CopyFile "%DEBUG_DLL%" "%DEBUG_TARGET%" "debug dll" "yes"
call :CopyFile "%DEBUG_LIB%" "%DEBUG_TARGET%" "debug lib" "yes"
call :CopyFile "%DEBUG_PDB%" "%DEBUG_TARGET%" "debug pdb" "yes"

call :CopyFile "%RELEASE_DLL%" "%RELEASE_TARGET%" "release dll" "yes"
call :CopyFile "%RELEASE_LIB%" "%RELEASE_TARGET%" "release lib" "yes"

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

pause
exit /b 0