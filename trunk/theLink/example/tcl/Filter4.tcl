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
  $ctx dict lappend Itms [list [$ctx ReadBDY] [$ctx ServiceGetToken] [$ctx ServiceIsTransaction]]
  $ctx SendRETURN
}

proc FilterSetup {ctx} {
  $ctx dict set Itms [list]
  $ctx ServiceCreate "+ALL" FilterIn
}

proc FilterCleanup {ctx} {
  $ctx dict inset Itms
}

proc FilterEvent {ctx} {
  set Itms [$ctx dict get Itms]
  if {[llength $Itms] == 0} {
    # no data -> nothing to do
    $ctx ErrorSetCONTINUE
  } else {
    # with data -> try to send
    foreach {data token isTran} [lindex $Itms 0] break
    set ftr [$ctx ServiceGetFilter]
    if {[catch {
      # try to connect if not already connected
      $ftr LinkConnect
      # send data
      $ftr SendSTART
      $ftr SendBDY $data
      if {$isTran} {
	$ftr SendEND_AND_WAIT $token
      } else {
	$ftr SendEND $token
      }
    }]} {
      # on "error" do the following:
      $ftr ErrorSet
      if {[$ftr ErrorIsEXIT]} {
	# on "exit-error" -> ignore and return
	$ftr ErrorReset
	return
      } else {
	# on "normal-error" -> write message to stderr and ignore
	$ftr ErrorPrint
      }
    }
    # on "success" or on "normal-error" delete item from list
    $ctx dict set Itms [lrange $Itms 1 end]
  }
}

tclmsgque Main {
  set srv [tclmsgque MqS]
  $srv ConfigSetName Filter4
  $srv ConfigSetServerSetup FilterSetup
  $srv ConfigSetServerCleanup FilterCleanup
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

