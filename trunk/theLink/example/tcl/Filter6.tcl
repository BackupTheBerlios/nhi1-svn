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
  $ctx ErrorSetEXIT
}

proc WRIT {ftr} {
puts "WRIT ============================================================"
  set FH [$ftr dict get FH]
  puts $FH [$ftr ReadC]
  flush $FH
  $ftr SendRETURN
}

proc FilterIn {ctx} {
puts "FilterIn ============================================================"
  $ctx StorageInsert
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
  set Id  [$ctx StorageSelect]
puts "FilterEvent ($Id) ============================================================"
  if {$Id == 0} {
    # no data -> nothing to do
    $ctx ErrorSetCONTINUE
    return
  } elseif {[catch {
    # with data -> try to send
    set ftr [$ctx ServiceGetFilter]
    # try to connect if not already connected
    $ftr LinkConnect
    # send entire BDY data to the link-target
    $ctx ReadBdyProxy $ftr
  }]} {
    # on "error" do the following:
    $ftr ErrorSet
    if {[$ftr ErrorIsEXIT]} {
      # on "exit-error" -> ignore and return
      $ftr ErrorReset
      return
    } else {
      # on "normal-error" -> write message to file and ignore
      # continue and delete data in next step
      ErrorWrite $ftr
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

