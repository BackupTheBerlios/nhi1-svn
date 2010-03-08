#+
#§  \file       theLink/example/tcl/Filter1.tcl
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
  global data
  set L [list]
  while {[$ctx ReadItemExists]} {
    lappend L "<[$ctx ReadC]>"
  }
  lappend data $L
  $ctx SendRETURN
}
proc EOFcmd {ctx} {
  global data
  set ftr [$ctx ServiceGetFilter]
  foreach L $data {
    $ftr SendSTART
    foreach I $L {
      $ftr SendC $I
    }
    $ftr SendEND_AND_WAIT "+FTR"
  }
  $ftr SendSTART
  $ftr SendEND_AND_WAIT "+EOF"
  $ctx SendRETURN
}
set srv [tclmsgque MqS]
$srv ConfigSetName filter
$srv ConfigSetIsServer yes
$srv ConfigSetFactory
if {[catch {
  $srv LinkCreate {*}$argv
  $srv ServiceCreate "+FTR" FTRcmd
  $srv ServiceCreate "+EOF" EOFcmd
  $srv ProcessEvent -wait FOREVER
}]} {
  $srv ErrorSet
}
$srv Exit


