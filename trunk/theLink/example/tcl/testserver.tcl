#+
#§  \file       theLink/example/tcl/testserver.tcl
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
proc GTCX {ctx} {
  $ctx SendSTART
  $ctx SendI [$ctx LinkGetCtxId]
  $ctx SendC "+"
  if {[$ctx LinkIsParent]} {
    $ctx SendI -1
  } else {
    $ctx SendI [[$ctx LinkGetParent] LinkGetCtxId]
  }
  $ctx SendC "+"
  $ctx SendC [$ctx ConfigGetName]
  $ctx SendC ":"
  $ctx SendRETURN
}
proc ServerConfig {ctx} {
  $ctx ServiceCreate "GTCX" GTCX
}
tclmsgque Main {
  set srv [tclmsgque MqS]
  $srv ConfigSetServerSetup ServerConfig
  $srv ConfigSetFactory
  if {[catch {
    $srv LinkCreate {*}$argv
    $srv ProcessEvent $MqS_WAIT_FOREVER
  }]} {
    $srv ErrorSet
  }
  $srv Exit
}






