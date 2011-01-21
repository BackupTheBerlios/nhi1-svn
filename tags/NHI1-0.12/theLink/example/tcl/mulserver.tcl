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
proc ServerSetup {ctx} {
  $ctx ServiceCreate "MMUL" MMUL
}
proc MulServer {tmpl} {
  set srv [tclmsgque MqS $tmpl]
  $srv ConfigSetServerSetup ServerSetup
  return $srv
}

tclmsgque Main {
  set srv [tclmsgque FactoryNew "mulserver" MulServer]
  if {[catch {
    $srv LinkCreate {*}$argv
    $srv ProcessEvent -wait FOREVER
  }]} {
    $srv ErrorSet
  }
  $srv Exit
}




