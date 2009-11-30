#+
#§  \file       win/env.tcl
#§  \brief      \$Id: env.tcl 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

set PYTHON {python.exe}
set JAVA {java.exe}
set TCLSH {tclsh.exe}
set PERL {perl.exe}

# C# works with "mono" (mono.exec) or with windows native (NATIVE)
set CSEXEC {NATIVE}
#set CSEXEC {mono.exe}

set PATH_SEP {;}
set EXEEXT {.exe}
set srcdir {.}
set USE_CXX {yes}
