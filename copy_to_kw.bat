@echo off

cd /d "%~dp0"

set "INCLUDE_TARGET=..\KalaTestProject\_external_shared\KalaWindow\include"
set "DEBUG_TARGET=..\KalaTestProject\_external_shared\KalaWindow\debug"
set "RELEASE_TARGET=..\KalaTestProject\_external_shared\KalaWindow\release"

if exist "%INCLUDE_TARGET%" (
	rmdir /S /Q "%INCLUDE_TARGET%"
)
if exist "%DEBUG_TARGET%" (
	rmdir /S /Q "%DEBUG_TARGET%"
)
mkdir "%DEBUG_TARGET%"
if exist "%RELEASE_TARGET%" (
	rmdir /S /Q "%RELEASE_TARGET%"
)
mkdir "%RELEASE_TARGET%"

set "INCLUDE_ORIGIN=include"

set "DEBUG_DLL=debug_opengl\KalaWindowD.dll"
set "DEBUG_LIB=debug_opengl\KalaWindowD.lib"
set "DEBUG_PDB=debug_opengl\KalaWindowD.pdb"

set "RELEASE_DLL=release_opengl\KalaWindow.dll"
set "RELEASE_LIB=release_opengl\KalaWindow.lib"

if not exist "%INCLUDE_ORIGIN%" (
	echo WARNING: Cannot copy 'include origin' because it does not exist!
) else (
	xcopy "%INCLUDE_ORIGIN%" "%INCLUDE_TARGET%" /E /H /Y /I
)

call :SafeCopy "%DEBUG_DLL%" "%DEBUG_TARGET%" "debug dll"
call :SafeCopy "%DEBUG_LIB%" "%DEBUG_TARGET%" "debug lib"
call :SafeCopy "%DEBUG_PDB%" "%DEBUG_TARGET%" "debug pdb"

call :SafeCopy "%RELEASE_DLL%" "%RELEASE_TARGET%" "release dll"
call :SafeCopy "%RELEASE_LIB%" "%RELEASE_TARGET%" "release lib"

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