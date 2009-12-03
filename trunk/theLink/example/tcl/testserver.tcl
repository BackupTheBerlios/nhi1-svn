#+
#§  \file       theLink/example/tcl/testserver.tcl
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
proc GTCX {ctx} {
  $ctx SendSTART
  $ctx SendI [$ctx ConfigGetCtxId]
  $ctx SendC "+"
  if {[$ctx ConfigGetIsParent]} {
    $ctx SendI -1
  } else {
    $ctx SendI [[$ctx ConfigGetParent] ConfigGetCtxId]
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
    $srv ProcessEvent -wait FOREVER
  }]} {
    $srv ErrorSet
  }
  $srv Exit
}


