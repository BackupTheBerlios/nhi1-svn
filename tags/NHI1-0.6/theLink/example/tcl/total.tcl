#+
#§  \file       theLink/example/tcl/total.tcl
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
set total 0
proc FTR {ctx} {
  set ftr [$ctx ServiceGetFilter]
  foreach {position amount currency} [$ctx ReadAll] break
  switch -exact $currency {
    euro    {set exchange 1.3541}
    pound   {set exchange 1.9896}
    dollar  {set exchange 1}
    default {error "invalid currency"}
  }
  set amount [expr {$amount * $exchange}]
  set currency dollar
  set ::total [expr {$::total + $amount}]
  $ftr SendSTART
  $ftr SendC $position
  $ftr SendD $amount
  $ftr SendEND_AND_WAIT "+FTR"
  $ctx SendRETURN
}
proc EOF {ctx} {
  set ftr [$ctx ServiceGetFilter]
  $ftr SendSTART
  $ftr SendC total
  $ftr SendD $::total
  $ftr SendEND_AND_WAIT "+FTR"
  $ctx SendRETURN
}
tclmsgque Main {
  set srv [tclmsgque MqS]
  $srv ConfigSetName total
  $srv ConfigSetIsServer yes
  $srv ConfigSetFactory
  if {[catch {
    $srv LinkCreate {*}$argv
    $srv ServiceCreate "+FTR" FTR
    $srv ServiceCreate "+EOF" EOF
    $srv ProcessEvent -wait FOREVER
  }]} {
    $srv ErrorSet
  }
  $srv Exit
}


