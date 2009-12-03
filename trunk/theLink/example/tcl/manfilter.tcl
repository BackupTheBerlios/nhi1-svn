#+
#§  \file       example/tcl/manfilter.tcl
#§  \brief      \$Id: manfilter.tcl 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§
package require TclMsgque
proc FTRcmd {ctx} {
  $ctx SendSTART
  while {[$ctx ReadItemExists]} {
    $ctx SendC "<[$ctx ReadC]>"
  }
  $ctx SendFTR
}
set srv [tclmsgque MqS]
$srv ConfigSetName ManFilter
$srv ConfigSetFilterFTR FTRcmd
if {[catch {
  $srv LinkCreate {*}$argv
  $srv ProcessEvent -wait FOREVER
}]} {
  $srv ErrorSet
}
$srv Exit

