#+
#§  \file       win/env.sh
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§


abs_top_builddir='/home/dev1usr/Project/NHI1'

TL="$abs_top_builddir/theLink"
TB="$abs_top_builddir/theBrain"
TG="$abs_top_builddir/theGuard"

PATH=".:$TL/tests:$TL/example/c:$TL/example/cc:$TL/acmds:$TB/abrain:$TB/atrans:$TG/aguard:$PATH"
LD_LIBRARY_PATH="$TL/libmsgque/.libs:$TL/tclmsgque/.libs:$TL/pymsgque/.libs:$TL/perlmsgque/.libs:$TL/javamsgque/.libs:$TL/ccmsgque/.libs:$TL/perlmsgque/Net-PerlMsgque/blib/arch/auto/Net/PerlMsgque:${LD_LIBRARY_PATH:-}"
TCLLIBPATH="$TL/tclmsgque/.libs"
PYTHONPATH="$TL/pymsgque/.libs"
CLASSPATH=".:$TL/javamsgque/javamsgque.jar:$TL/example/java"
MONO_PATH=".:$TL/csmsgque:$TL/example/csharp"
PERL5LIB="$TL/perlmsgque/Net-PerlMsgque/blib/lib"

export PATH LD_LIBRARY_PATH TCLLIBPATH PYTHONPATH CLASSPATH MONO_PATH PERL5LIB

PACKAGE='NHI1'
PACKAGE_VERSION='0.14'
PYTHON='python.exe'
JAVA='java.exe'
TCLSH='tclsh.exe'
PERL='perl.exe'
PHP=''
DOT=''
DOXYGEN=''
RUBY=''

# C# works with "mono" (mono.exec) or with windows native (NATIVE)
#set CLREXEC ''
CLREXEC='mono.exe'

USE_TCL='yes'
USE_PERL='yes'
USE_PHP='yes'
USE_CSHARP='yes'
USE_GO='yes'
USE_VB='no'
USE_PYTHON='yes'
USE_RUBY='yes'
USE_JAVA='yes'
USE_CXX='yes'
USE_PHP='yes'
USE_BRAIN='yes'
USE_GUARD='yes'

PATH_SEP=';'
EXEEXT='.exe'

export PACKAGE PACKAGE_VERSION host build PERL DOT \
        DOXYGEN PYTHON RUBY PHP JAVA TCLSH PATH_SEP EXEEXT
