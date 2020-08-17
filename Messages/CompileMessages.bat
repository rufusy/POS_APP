@echo off

set SDK_VERSION=SDK11.16.7.PatchD

set SDK_DIR=%ProgramFiles%\TeliumSDK\%SDK_VERSION%
if not exist "%SDK_DIR%" set SDK_DIR=C:\Program Files\TeliumSDK\%SDK_VERSION%
if not exist "%SDK_DIR%" (
	echo SDK not found !
	echo %SDK_DIR%
	pause
	goto :eof
)
set TOOL_DIR=%SDK_DIR%\Tools\FW_TPLUS\MSG tool
if not exist "%TOOL_DIR%" set TOOL_DIR=%SDK_DIR%\tools\MSG tool
if not exist "%TOOL_DIR%" (
	echo MSG tool in SDK not found !
	echo %SDK_DIR%
	pause
	goto :eof
)

rem ===========================================================
rem
rem  generate English Messages
rem
rem ===========================================================

rem 1- convert h file to MSG file

"%TOOL_DIR%\BuildMSG" -c -tCLESS_msgEnglish.h -hMessagesDefinitions.h LANG.MSG


rem ===========================================================
rem
rem  generate French Messages (same command as previous without -C)
rem
rem ===========================================================

"%TOOL_DIR%\BuildMSG" -tCLESS_msgFrench.h -hMessagesDefinitions.h LANG.MSG


rem ===========================================================
rem
rem  generate French Messages (same command as previous without -C)
rem
rem ===========================================================

"%TOOL_DIR%\BuildMSG" -tCLESS_msgSwahili.h -hMessagesDefinitions.h LANG.MSG

rem ===========================================================
rem
rem  sign LANG.MSG before loadind it in the terminal
rem
rem ===========================================================


rem 2- create LANG.c file to include in the sample
"%TOOL_DIR%\MSG2C" LANG.MSG CLESS_lang.c

move /y MessagesDefinitions.h ..\Inc
move /y CLESS_lang.c ..\Src

if not "%~1" == "/nopause" pause
