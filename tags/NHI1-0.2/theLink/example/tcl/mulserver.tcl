#+
#§  \file       theLink/example/tcl/mulserver.tcl
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
proc MMUL {ctx} {
  $ctx SendSTART
  $ctx SendD [expr {[$ctx ReadD] * [$ctx ReadD]}]
  $ctx SendRETURN
}
proc ServerConfig {ctx} {
  $ctx ServiceCreate "MMUL" MMUL
}
tclmsgque Main {
  set srv [tclmsgque MqS]
  $srv ConfigSetName MyMulServer
  $srv ConfigSetFactory
  $srv ConfigSetServerSetup ServerConfig
  if {[catch {
    $srv LinkCreate {*}$argv
    $srv ProcessEvent -wait FOREVER
  }]} {
    $srv ErrorSet
  }
  $srv Exit
}


