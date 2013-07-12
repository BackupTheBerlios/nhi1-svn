#!/usr/bin/env Nhi1Exec
#+
#:  \file       bin/Nhi1Label.tcl
#:  \brief      \$Id$
#:  
#:  (C) 2009 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

set LHOME [file normalize [file join [file dirname $argv0] ..]]

array set LABELUSE {
    .					{ ChangeLog .sh INSTALL .sh COPYING .sh NEWS .sh AUTHORS .sh configure.ac .sh 
					  mqconfig.h.in .c THANKS .sh env.sh.in .sh env.tcl.in .sh README_windows .sh }
    ./theLink/tclmsgque			{ tclmsgque.tcl.in .sh }
    ./theLink/msgqueforphp		{ php.ini.in .phpini }
    ./theLink/acmds			{ aexec .tcl .README.acmds .sh }
    ./win				{ README.win .sh }
    ./performance			{ env.sh.in .sh }
    ./bin				{ Nhi1Label .sh Nhi1Config .sh rcBuildFs .sh Nhi1BuildLanguage .sh Nhi1Exec .sh 
					  Nhi1Docs .sh Nhi1Tags .sh }
    ./lxc				{ * .sh }
    ./tests				{ TestControl.bash.in .sh }
    ./sbin				{ LbMain .sh log .sh SetupEnv .sh w3m .sh *.bat.in .bat}
    ./etc				{ .alias .sh .bashrc .sh .vimrc .vim }
    ./theLink/gomsgque			{ MakeGoMsgque.inc.in .sh }
    ./theLink/gomsgque/src/gomsgque	{ gomsgque.go.in .go }
    ./theLink/tests			{ README.tests .sh exec.env.in .sh }
    ./performance			{ *.in .sh }
}

array set LABELIGNORE {
    ./build				{ * }
    ./bin				{ *.sw* Makefile Makefile.in tags }
    ./etc				{ *.sw* Makefile Makefile.in tags }
    ./sbin				{ *.sw* Makefile Makefile.in tags }
    ./tests				{ *.sw* Makefile Makefile.in tags }
    ./html				{ * }
    ./man				{ * }
    ./binary-build			{ * }
    ./lxc				{ Makefile.in }
    ./performance			{ Makefile Makefile.in nothread thread docs .TIP env.sh *.log tmp* *.md *.sw* }
    ./m4				{ libtool.m4 ltoptions.m4 ltsugar.m4 ltversion.m4 lt~obsolete.m4 }

    ./theLink				{ Makefile Makefile.in }
    ./theLink/docs			{ * }
    ./theLink/tests			{ Makefile Makefile.in tags *.sw* .vimrc .kdbgrc.* *.md }
    ./theLink/acmds			{ Makefile Makefile.in *.o .deps .libs tags *.sw* astarter atool docs }
    ./theLink/sqlite			{ Makefile Makefile.in *.sw* .deps .libs sqlite3.c sqlite3.h sqlite3ext.h tags libtmp* }
    ./theLink/example/csharp		{ *.mdb .vimrc Makefile.in Makefile tags .vpath_hook *.dll *.sw* }
    ./theLink/example/c			{ *.o .deps tags mulclient mulserver manfilter testserver testclient Makefile
					  *.sw* MyClient MyServer Makefile.in Filter1 Filter2 Filter3 Filter4 Filter5
					  Filter6 client server docs }
    ./theLink/example/cc		{ *.o .deps tags mulclient mulserver manfilter testserver testclient Makefile
					  *.sw* MyClient MyServer Makefile.in Filter1 Filter2 Filter3 Filter4 Filter5
					  Filter6 server }
    ./theLink/example/java		{ example *.stamp *.sw* Makefile Makefile.in .vpath_hook tags}
    ./theLink/example/tcl		{ *.sw* Makefile.in Makefile tags }
    ./theLink/example/python		{ *.sw* Makefile.in Makefile}
    ./theLink/example/ruby		{ *.sw* Makefile.in Makefile}
    ./theLink/example/perl		{ *.sw* Makefile.in Makefile Switch.pm}
    ./theLink/example/php		{ *.sw* Makefile.in Makefile}
    ./theLink/example/go		{ *.sw* Makefile.in Makefile *.log MyClient MyServer Filter3 mulclient
					  manfilter mulserver testserver testclient Filter1 Filter2 Filter4 Filter5
					  server Filter6 }
    ./theLink/example/vb		{ *.sw* Makefile.in Makefile *.xml .vpath_hook *.dll }
    ./theLink/example			{ tags }

    ./theLink/libmsgque			{ *.sw* *.lo *.o *.la .libs .deps Makefile.in depcomp Makefile tags *.old}
    ./theLink/tclmsgque			{ Makefile Makefile.in *.lo *.o *.la .deps .libs tags *.sw* pkgIndex.tcl }
    ./theLink/tclmsgque/tmp		{ pkgIndex.tcl }
    ./theLink/pymsgque			{ Makefile Makefile.in *.lo *.o *.la .deps .libs tags *.sw* .C build }
    ./theLink/rubymsgque		{ Makefile Makefile.in *.lo *.o *.la .deps .libs tags *.sw* .C build *.rb }
    ./theLink/perlmsgque		{ Net-PerlMsgque Makefile Makefile.in tags *.sw*}
    ./theLink/gomsgque			{ Makefile Makefile.in MakeGoMsgque.inc }
    ./theLink/javamsgque		{ Makefile Makefile.in *.lo *.o *.la .deps .libs javamsgque tags *.sw*
					  MqS_Main.h Manifest.txt *.in *.stamp *.jar javamsgque_MqS.h
					  javamsgque_MqS_WAIT.h javamsgque_MqConfigS.h javamsgque_MqBufferS.h 
					  javamsgque_MqS_TIMEOUT.h javamsgque_MqS_START.h javamsgque_MqFactoryS.h
					  javamsgque_MqDumpS.h .vpath_hook }
    ./theLink/ccmsgque			{ .libs .deps *.lo *.o *.in Makefile *.la tags *.sw* }
    ./theLink/csmsgque			{ Makefile *.in tags *.dll *.exe .vimrc .libs .deps libtest* *.mdb 
							  *.sw* .vpath_hook }
    ./theLink/msgqueforphp		{ Makefile Makefile.in test.php php.ini tags }
    ./theLink/msgqueforphp/MsgqueForPhp {
	  acinclude.m4    config.sub     include          Makefile            modules       tags
	  aclocal.m4      configure      install-sh       Makefile.fragments  MsgqueForPhp.la  tests
	  autom4te.cache  config.log     configure.in     libtool       Makefile.global     tmp-php.ini
	  build           config.w32     Makefile.objects MsgqueForPhp.php *.sw* php.tags
	  config.guess    config.nice    CREDITS          missing       .libs  .deps
	  config.h*       config.status  EXPERIMENTAL     ltmain.sh     mkinstalldirs       run-tests.php *.lo}

    ./theBrain				{ Makefile.in Makefile }
    ./theBrain/tests			{ tags tmp.* Makefile.in Makefile .libs .deps *.o client tc* check.* *.sw*}
    ./theBrain/libbrain			{ *.sw* *.c *.h *.lo *.o *.la .libs .deps Makefile.in depcomp Makefile tags }
    ./theBrain/abrain			{ *.sw* *.o *.la .libs .deps Makefile.in Makefile tags abrain }
    ./theBrain/atrans			{ *.sw* *.o *.la .libs .deps Makefile.in Makefile tags atrans }

    ./theGuard				{ Makefile.in Makefile }
    ./theGuard/tests			{ tags tmp.* Makefile.in Makefile .libs .deps *.o client *.sw* }
    ./theGuard/aguard			{ *.sw* *.o *.la .libs .deps Makefile.in Makefile tags aguard key.h }

    ./save				{ openfacts2.berlios.de }

    ./win				{ c cc csharp vb win Makefile.in Makefile win.sln .CHANGE }
    .					{ aclocal.m4 ltmain.sh libtool.m4 install-sh depcomp Makefile.in 
					  configure config.guess config.sub .PERSONAL missing autom4te.cache 
					  config.log mqconfig.h.in libtool config.status stamp-h1 
					  Makefile mqconfig.h NHI1-* doxygen.* doxygen-doc distcheck.out
					  .OtConfig compile binary-dist
					  tags .IDEEN config.out INSTALL out.log env.sh .IMPORTANT .CLEAN env.tcl
					  discheck.out msgqueforphp *.bak mqconfig.h.in~ *.log .PUT_HTML
					  .GREP_FOR_SOURCE_CODE .PHP.info build_test html .ToDo
					  ar-lib *.sw* }
}

set LLabel [list								\
    "%PREFIX%  \\file       %FILE%"						\
    "%PREFIX%  \\brief      \\%ID%"						\
    "%PREFIX%  "								\
    "%PREFIX%  (C) %YEAR% - NHI - #1 - Project - Group"				\
    "%PREFIX%  "								\
    "%PREFIX%  \\version    \\%REV%"						\
    "%PREFIX%  \\author     EMail: aotto1968 at users.berlios.de"		\
    "%PREFIX%  \\attention  this software has GPL permissions to copy"		\
	"%PREFIX%              please contact AUTHORS for additional information"   \
]

source [file join $LHOME sbin LbMain]

