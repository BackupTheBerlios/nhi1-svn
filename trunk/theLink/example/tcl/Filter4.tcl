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

proc ErrorWrite {ctx} {
  set FH [$ctx dict get FH]
  puts $FH "ERROR: [$ctx ErrorGetText]"
  flush $FH
  $ctx ErrorReset
}

proc LOGF {ctx} {
  set ftr [$ctx ServiceGetFilter]
  if {[$ftr LinkGetTargetIdent] == "transFilter"} {
    $ctx ReadForward $ftr
  } else {
    set FH [open [$ctx ReadC] a]
    $ctx dict set FH $FH
  }
  $ctx SendRETURN
}

proc EXIT {ctx} {
  $ctx ErrorSetEXIT
}

proc WRIT {ftr} {
  set FH [[$ftr ServiceGetFilter] dict get FH]
  puts $FH [$ftr ReadC]
  flush $FH
  $ftr SendRETURN
}

proc FilterIn {ctx} {
  $ctx dict lappend Itms [$ctx ReadDUMP]
  $ctx SendRETURN
}

proc FilterSetup {ctx} {
  set ftr [$ctx ServiceGetFilter]
  $ctx dict set Itms [list]
  $ctx dict set FH ""
  $ctx ServiceCreate "LOGF" LOGF
  $ctx ServiceCreate "EXIT" EXIT
  $ctx ServiceCreate "+ALL" FilterIn
  $ftr ServiceCreate "WRIT" WRIT
  $ftr ServiceProxy  "+TRT"
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
    return
  } elseif {[catch {
    # with data -> try to send
    set data [lindex $Itms 0]
    set ftr [$ctx ServiceGetFilter]
    # try to connect if not already connected
    $ftr LinkConnect
    # setup the BDY data from storage
    $ctx ReadLOAD $data
    # send BDY data to the link-target
    $ctx ReadForward $ftr
  }]} {
    # on "error" do the following:
    $ctx ErrorSet
    if {[$ctx ErrorIsEXIT]} {
      # on "exit-error" -> ignore and return
      $ctx ErrorReset
      return
    } else {
      # on "normal-error" -> write message to file and ignore
      ErrorWrite $ctx
    }
  }
  # on "success" or on "normal-error" delete item from list
  $ctx dict set Itms [lrange [$ctx dict get Itms] 1 end]
}

tclmsgque FactoryDefault "transFilter"

tclmsgque Main {
  set srv [tclmsgque MqS]
  $srv ConfigSetServerSetup FilterSetup
  $srv ConfigSetServerCleanup FilterCleanup
  $srv ConfigSetEvent FilterEvent
  $srv ConfigSetIgnoreExit yes
  if {[catch {
    $srv LinkCreate {*}$argv
    $srv ProcessEvent -wait FOREVER
  }]} {
    $srv ErrorSet
  }
  $srv Exit
}

