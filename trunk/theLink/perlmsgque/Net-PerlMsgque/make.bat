@ECHO OFF

:: cygwin over SSH missing this environment variable
:: IF "x%ProgramFiles(x86)%x"=="xx" SET "ProgramFiles(x86)=%ProgramFiles%"

@ECHO ON
call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /Debug /x64 /win7 2>NUL
@ECHO OFF

cd Net-PerlMsgque

set MAKE=
set MAKEFLAGS=

@ECHO ON
nmake.exe %~1
@ECHO OFF
