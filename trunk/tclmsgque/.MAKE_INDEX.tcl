#+
#§  \file       tclmsgque/.MAKE_INDEX.tcl
#§  \brief      \$Id: .MAKE_INDEX.tcl 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

source ../env.tcl

set env(PATH)		    "[file join .. src .libs]$PATH_SEP$env(PATH)"
set env(LD_LIBRARY_PATH)    "[file join .. src .libs]$PATH_SEP$env(LD_LIBRARY_PATH)"

pkg_mkIndex -verbose ./.libs

