#+
#:  \file       theLink/tclmsgque/.MAKE_INDEX.tcl
#:  \brief      \$Id$
#:  
#:  (C) 2009 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

source ../../env.tcl

if { $tcl_platform(platform) == "windows" } {
  append env(PATH) "$PATH_SEP"
  append env(LD_LIBRARY_PATH) "$PATH_SEP"
}

if {[info exists env(LD_LIBRARY_PATH)]} {
  set ldp $env(LD_LIBRARY_PATH)
} else {
  set ldp ""
}

set env(PATH) "[file join .. libmsgque .libs]$PATH_SEP$env(PATH)"
set env(LD_LIBRARY_PATH) "[file join .. libmsgque .libs]$PATH_SEP$ldp"

pkg_mkIndex -verbose ./.libs

