#+
#§  \file       theLink/example/tcl/Filter2.tcl
#§  \brief      \$Id: Filter2.tcl 25 2009-12-03 11:05:46Z aotto1968 $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 25 $
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

package require TclMsgque

proc Filter {ctx} {
  set ftr [$ctx ConfigGetFilter]
  set bdy [$ctx ReadBDY]
  $ftr SendSTART
  $ftr SendBDY $bdy
  if {[$ctx ConfigGetIsTrans]} {
    $ftr SendEND_AND_WAIT [$ctx ConfigGetToken] -2
    $ctx SendSTART
    set bdy [$ftr ReadBDY]
    $ctx SendBDY $bdy
  } else {
    $ftr SendEND [$ctx ConfigGetToken]
  }
  $ctx SendRETURN
}

proc ServerSetup {ctx} {
  set ftr [$ctx ConfigGetFilter]
  $ctx ServiceCreate "+ALL" Filter
  $ftr ServiceCreate "+ALL" Filter
}

tclmsgque Main {
  set srv [tclmsgque MqS]
  $srv ConfigSetName Filter3
  $srv ConfigSetServerSetup ServerSetup
  $srv ConfigSetFactory
  if {[catch {
    $srv LinkCreate {*}$argv
    $srv ProcessEvent -wait FOREVER
  }]} {
    $srv ErrorSet
  }
  $srv Exit
}

