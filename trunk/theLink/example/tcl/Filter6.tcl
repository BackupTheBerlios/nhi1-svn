#+
#§  \file       theLink/example/tcl/Filter6.tcl
#§  \brief      \$Id$
#§  
#§  (C) 2011 - NHI - #1 - Project - Group
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
  $ctx StorageInsert
  $ctx SendRETURN
}

proc FilterSetup {ctx} {
  set ftr [$ctx ServiceGetFilter]
  $ctx dict set FH ""
  $ctx ServiceCreate "LOGF" LOGF
  $ctx ServiceCreate "EXIT" EXIT
  $ctx ServiceStorage "PRNT"
  $ctx ServiceCreate "+ALL" FilterIn
  $ftr ServiceCreate "WRIT" WRIT
  $ftr ServiceProxy  "+TRT"
}

proc FilterCleanup {ctx} {
  set ftr [$ctx ServiceGetFilter]
  set FH  [$ftr dict get FH]
  $ctx dict unset FH
  if {$FH ne ""} {close $FH}
}

proc FilterEvent {ctx} {
  if {[$ctx StorageCount] == 0} {
    # no data -> nothing to do
    $ctx ErrorSetCONTINUE
    return
  } elseif {[catch {
    # with data -> try to send
    set ftr [$ctx ServiceGetFilter]
    # try to connect if not already connected
    $ftr LinkConnect
    # setup Read package from storage
    set Id  [$ctx StorageSelect]
    # forward the entire BDY data to the ftr-target
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
      # continue and delete data in next step
      ErrorWrite $ctx
    }
  }
  # on "success" or on "error" delete item from storage
  $ctx StorageDelete $Id
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

