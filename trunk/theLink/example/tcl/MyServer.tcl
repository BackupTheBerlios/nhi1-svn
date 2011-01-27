#+
#§  \file       theLink/example/tcl/MyServer.tcl
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

proc MyFirstService {ctx} {
  $ctx SendSTART
  $ctx SendC "Hello World"
  $ctx SendRETURN
}
proc ServerSetup {ctx} {
  $ctx ServiceCreate "HLWO" MyFirstService
}
proc MyServer {tmpl} {
  set srv [tclmsgque MqS $tmpl]
  $srv ConfigSetServerSetup ServerSetup
  return $srv
}

tclmsgque Main {
  set srv [[tclmsgque FactoryAdd MyServer] New]
  $srv ConfigSetName MyServer
  $srv ConfigSetServerSetup ServerSetup
  if {[catch {
    $srv LinkCreate {*}$argv
    $srv ProcessEvent -wait FOREVER
  }]} {
    $srv ErrorSet
  }
  $srv Exit
}




