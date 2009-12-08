#+
#§  \file       theLink/example/tcl/Filter2.tcl
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§
package require TclMsgque
set data [list]
proc FTRcmd {ctx} {
  error "my error"
}
set srv [tclmsgque MqS]
$srv ConfigSetName filter
$srv ConfigSetFilterFTR FTRcmd
if {[catch {
  $srv LinkCreate {*}$argv
  $srv ProcessEvent -wait FOREVER
}]} {
  $srv ErrorSet
}
$srv Exit

