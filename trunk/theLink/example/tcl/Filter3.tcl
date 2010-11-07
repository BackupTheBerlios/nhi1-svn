#+
#§  \file       theLink/example/tcl/Filter3.tcl
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

proc ServerSetup {ctx} {
  set ftr [$ctx ServiceGetFilter]
  $ctx ServiceProxy "+ALL" 0
  $ctx ServiceProxy "+TRT" 0
  $ftr ServiceProxy "+ALL" 0
  $ftr ServiceProxy "+TRT" 0
}

tclmsgque Main {
  set srv [tclmsgque MqS]
  $srv ConfigSetName Filter3
  $srv ConfigSetServerSetup ServerSetup
  $srv ConfigSetFactory
  if {[catch {
    $srv LinkCreate {*}$argv
    $srv ProcessEvent $MqS_WAIT_FOREVER
  }]} {
    $srv ErrorSet
  }
  $srv Exit
}




