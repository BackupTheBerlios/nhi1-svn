#+
#§  \file       theLink/example/tcl/total.tcl
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§
package require TclMsgque
set total 0
proc FTR {ctx} {
  foreach {position amount currency} [$ctx ReadAll] break
  switch -exact $currency {
    euro    {set exchange 1.3541}
    pound   {set exchange 1.9896}
    default {set exchange 1}
  }
  set amount [expr {$amount * $exchange}]
  set currency dollar
  set ::total [expr {$::total + $amount}]
  $ctx SendSTART
  $ctx SendC $position
  $ctx SendD $amount
  $ctx SendFTR
}
proc EOF {ctx} {
  $ctx SendSTART
  $ctx SendC total
  $ctx SendD $::total
  $ctx SendFTR
}
set srv [tclmsgque MqS]
$srv ConfigSetFilterFTR FTR
$srv ConfigSetFilterEOF EOF
if {[catch {
  $srv LinkCreate {*}$argv
  $srv ProcessEvent -wait FOREVER
}]} {
  $srv ErrorSet
}
$srv Exit

