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

PACKAGE='libmsgque'
PACKAGE_VERSION='4.1'
PYTHON='python.exe'
JAVA='java.exe'
TCLSH='tclsh.exe'
PERL='perl.exe'

# C# works with "mono" (mono.exec) or with windows native (NATIVE)
#set CSEXEC {NATIVE}
CSEXEC='mono.exe'

PATH_SEP=';'
EXEEXT='.exe'

export PACKAGE PACKAGE_VERSION PYTHON JAVA TCLSH PATH_SEP EXEEXT PERL

