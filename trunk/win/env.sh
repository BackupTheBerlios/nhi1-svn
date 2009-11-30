#+
#§  \file       win/env.sh
#§  \brief      \$Id: env.sh 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

PACKAGE='libmsgque'
PACKAGE_VERSION='3.8'
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

