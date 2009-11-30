#+
#§  \file       example/tcl/Filter1.tcl
#§  \brief      \$Id: Filter1.tcl 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
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
}
proc EOFcmd {ctx} {
  global data
  foreach L $data {
    $ctx SendSTART
    foreach I $L {
      $ctx SendC $I
    }
    $ctx SendFTR
  }
}
set srv [tclmsgque MqS]
$srv ConfigSetName filter
$srv ConfigSetFilterFTR FTRcmd
$srv ConfigSetFilterEOF EOFcmd
if {[catch {
  $srv LinkCreate {*}$argv
  $srv ProcessEvent -wait FOREVER
}]} {
  $srv ErrorSet
}
$srv Exit

