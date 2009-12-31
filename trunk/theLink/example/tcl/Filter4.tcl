#+
#§  \file       theLink/example/tcl/Filter4.tcl
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

proc FilterIn {ctx} {
  $ctx dict set Itms [concat [$ctx dict get Itms] [$ctx ReadB] [$ctx ConfigGetToken] [$ctx ConfigGetIsTransaction]]
}

proc FilterSetup {ctx} {
  $ctx dict set Itms [list]
  $ctx ServiceCreate "+ALL" FilterIn
}

proc FilterEvent {ctx} {
  set Itms [$ctx dict get Itms]
  if {[llength $Itms]} {
    set ftr [$ctx ConfigGetFilter]
    foreach {data token isTran} [lindex $Itms 0] break
    $ftr SendSTART
    $ftr SendBDY $data
    if {$isTran} {
      $ftr SendEND_AND_WAIT $ftr $token
    } else {
      $ftr SendEND $ftr $token
    }
    $ctx dict set Itms [lrange $Itms 1 end]
  } else {
    $ctx ErrorSetCONTINUE
  }
}

tclmsgque Main {
  set srv [tclmsgque MqS]
  $srv ConfigSetName Filter3
  $srv ConfigSetServerSetup ServerSetup
  $srv ConfigSetEvent FilterEvent
  $srv ConfigSetIgnoreExit yes
  $srv ConfigSetFactory
  if {[catch {
    $srv LinkCreate {*}$argv
    $srv ProcessEvent -wait FOREVER
  }]} {
    $srv ErrorSet
  }
  $srv Exit
}

