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

proc ErrorWrite {ftr} {
  set FH [$ftr dict get FH]
  puts $FH "ERROR: [$ftr ErrorGetText]"
  flush $FH
  $ftr ErrorReset
}

proc LOGF {ctx} {
  set ftr [$ctx ServiceGetFilter]
  set file [$ctx ReadC]
  if {[$ftr LinkGetTargetIdent] == "transFilter"} {
    $ftr SendSTART
    $ftr SendC $file
    $ftr SendEND_AND_WAIT "LOGF"
  } else {
    set FH [open $file a]
    $ftr dict set FH $FH
  }
  $ctx SendRETURN
}

proc EXIT {ctx} {
  $ctx Exit
}

proc WRIT {ftr} {
  set FH [$ftr dict get FH]
  puts $FH [$ftr ReadC]
  flush $FH
  $ftr SendRETURN
}

proc FilterIn {ctx} {
  $ctx dict lappend Itms [list [$ctx ReadBDY] [$ctx ServiceGetToken] [$ctx ServiceIsTransaction]]
  $ctx SendRETURN
}

proc FilterSetup {ctx} {
  $ctx dict set Itms [list]
  $ctx dict set FH ""
  $ctx ServiceCreate "LOGF" LOGF
  $ctx ServiceCreate "EXIT" EXIT
  $ctx ServiceCreate "+ALL" FilterIn
  [$ctx ServiceGetFilter] ServiceCreate "WRIT" WRIT
}

proc FilterCleanup {ctx} {
  set ftr [$ctx ServiceGetFilter]
  set FH  [$ftr dict get FH]
  $ctx dict unset Itms
  $ctx dict unset FH
  if {$FH ne ""} {close $FH}
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
	# on "normal-error" -> write message to file and ignore
	ErrorWrite $ftr
      }
    }
    # on "success" or on "normal-error" delete item from list
    $ctx dict set Itms [lrange [$ctx dict get Itms] 1 end]
  }
}

tclmsgque Main {
  set srv [tclmsgque MqS]
  $srv ConfigSetName Filter4
  $srv ConfigSetServerSetup FilterSetup
  $srv ConfigSetServerCleanup FilterCleanup
  $srv ConfigSetEvent FilterEvent
  $srv ConfigSetIdent "transFilter"
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

