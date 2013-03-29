
@ECHO ON
call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /Debug /x64 /win7 2>NUL
@ECHO OFF

cd Net-PerlMsgque

set perl=%1&shift
set prefix=%1&shift
set abs_top_builddir=%1&shift
set abs_srcdir=%1&shift
set abs_builddir=%1&shift

REM echo :%perl%:
REM echo :%prefix%:
REM echo :%abs_top_builddir%:
REM echo :%abs_srcdir%:
REM echo :%abs_builddir%:

@ECHO ON
%perl% Makefile.PL PREFIX="%prefix%" ^
  INC="-Wall -I. -I%abs_top_builddir% -I%abs_srcdir%/../libmsgque" ^
  OBJECT="$(O_FILES) %abs_builddir%/../libmsgque/.libs/libmsgque.dll.a %abs_builddir%/../sqlite/.libs/libtmp.a" ^
  DEFINE="-DPIC -DDLL_EXPORT"
@ECHO OFF



