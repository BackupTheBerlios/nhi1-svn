#+
#:  \file       theLink/example/tcl/Filter2.tcl
#:  \brief      \$Id$
#:  
#:  (C) 2009 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:
package require TclMsgque
set data [list]
proc FTRcmd {ctx} {
  error "my error"
}
set srv [tclmsgque MqS]
$srv ConfigSetIsServer yes
$srv ConfigSetName filter
if {[catch {
  $srv LinkCreate {*}$argv
  $srv ServiceCreate "+FTR" FTRcmd
  $srv ServiceProxy  "+EOF"
  $srv ProcessEvent -wait FOREVER
}]} {
  $srv ErrorSet
}
$srv Exit



