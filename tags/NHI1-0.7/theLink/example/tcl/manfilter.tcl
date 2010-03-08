#+
#§  \file       theLink/example/tcl/manfilter.tcl
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
proc FTRcmd {ctx} {
  set ftr [$ctx ServiceGetFilter]
  $ftr SendSTART
  while {[$ctx ReadItemExists]} {
    $ftr SendC "<[$ctx ReadC]>"
  }
  $ftr SendEND_AND_WAIT "+FTR"
  $ctx SendRETURN
}
set srv [tclmsgque MqS]
$srv ConfigSetName ManFilter
$srv ConfigSetIsServer yes
$srv ConfigSetFactory
if {[catch {
  $srv LinkCreate {*}$argv
  $srv ServiceCreate "+FTR" FTRcmd
  $srv ServiceProxy  "+EOF"
  $srv ProcessEvent -wait FOREVER
}]} {
  $srv ErrorSet
}
$srv Exit


