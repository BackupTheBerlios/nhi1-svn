#+
#§  \file       example/tcl/MyServer.tcl
#§  \brief      \$Id: MyServer.tcl 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

package require TclMsgque
proc MyFirstService {ctx} {
  $ctx SendSTART
  $ctx SendC "Hello World"
  $ctx SendRETURN
}
proc ServerSetup {ctx} {
  $ctx ServiceCreate "HLWO" MyFirstService
}

tclmsgque Main {
  set srv [tclmsgque MqS]
  $srv ConfigSetFactory
  $srv ConfigSetServerSetup ServerSetup
  if {[catch {
    $srv LinkCreate {*}$argv
    $srv ProcessEvent -wait FOREVER
  }]} {
    $srv ErrorSet
  }
  $srv Exit
}


