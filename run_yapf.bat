@ECHO off
SETLOCAL enableextensions enabledelayedexpansion
PUSHD .
CD /d "%~dp0\.."

REM --------------------------------------------------------------------------------------
REM 	Runs yapf format on all the python files in the project
REM --------------------------------------------------------------------------------------

WHERE /Q yapf
IF %ERRORLEVEL% NEQ 0 (
	ECHO Could not find yapf
	PAUSE
	POPD
	ENDLOCAL
	EXIT /B !ERRORLEVEL!
)

yapf --version
CALL :RunYapfOnDirectories ^
	soagen

POPD
@ENDLOCAL
EXIT /B 0

:RunYapfOnDirectories
(
	FOR %%I IN (%*) DO (
		IF EXIST "%%~I" (
			ECHO Formatting files in "%%~I"
			FOR %%X IN (py) DO (
				FOR /F %%J IN ('DIR /B /S "%%~I\*.%%X" 2^> nul') DO (
					yapf -i "%%J"
					IF !ERRORLEVEL! NEQ 0 (
						ECHO Error formatting %%J
						PAUSE
						POPD
						ENDLOCAL
						EXIT /B !ERRORLEVEL!
					)
				)
			)
		)
	)
	EXIT /B
)
