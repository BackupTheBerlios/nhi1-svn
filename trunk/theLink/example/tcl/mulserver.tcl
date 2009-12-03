#+
#§  \file       example/tcl/mulserver.tcl
#§  \brief      \$Id: mulserver.tcl 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
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


