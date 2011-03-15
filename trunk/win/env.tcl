#+
#§  \file       win/env.tcl
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

set PACKAGE_VERSION {0.14}
set PYTHON {python.exe}
set RUBY {}
set JAVA {java.exe}
set TCLSH {tclsh.exe}
set PERL {perl.exe}
set PHP {}

# C# works with "mono" (mono.exec) or with windows native (NATIVE)
set CLREXEC {}
#set CLREXEC {mono.exe}

set PATH_SEP {;}
set EXEEXT {.exe}
set srcdir {.}

array set env [list   \
  PYTHON    $PYTHON   \
  RUBY      $RUBY     \
  JAVA      $JAVA     \
  TCLSH     $TCLSH    \
  PERL      $PERL     \
  PHP       $PHP      \
  CLREXEC   $CLREXEC  \
]

set USE_TCL {yes}
set USE_PERL {no}
set USE_PHP {no}
set USE_CSHARP {yes}
set USE_GO {no}
set USE_VB {yes}
set USE_PYTHON {no}
set USE_RUBY {no}
set USE_JAVA {yes}
set USE_CXX {yes}
set USE_PHP {no}
set USE_BRAIN {no}
set USE_GUARD {no}

