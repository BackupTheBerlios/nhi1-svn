#+
#:  \file       theLink/example/tcl/server.tcl
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

package require Tcl 8.5

## load the TclMsgque extension
package require TclMsgque

proc Ot_INIT {ctx} {
  $ctx SendSTART
  set LIST [list]
  while {[$ctx ReadItemExists]} {
    lappend LIST [$ctx ReadC]
  }
  tclmsgque Init {*}$LIST
  $ctx SendRETURN
}

proc Ot_RDUL {ctx} {
  $ctx ReadY
  $ctx ReadS
  $ctx ReadI
  $ctx ReadW
  $ctx ReadB
}

proc Ot_ECUL {ctx} {
  $ctx SendSTART
  $ctx SendY [$ctx ReadY]
  $ctx SendS [$ctx ReadS]
  $ctx SendI [$ctx ReadI]
  $ctx SendW [$ctx ReadW]
  $ctx ReadProxy $ctx
  $ctx SendRETURN
}

proc Ot_ECHO {ctx} {
  $ctx SendSTART
  switch -exact [$ctx ServiceGetToken] {
    ECOY  { $ctx SendY [$ctx ReadY] }
    ECOO  { $ctx SendO [$ctx ReadO] }
    ECOS  { $ctx SendS [$ctx ReadS] }
    ECOI  { $ctx SendI [$ctx ReadI] }
    ECOF  { $ctx SendF [$ctx ReadF] }
    ECOW  { $ctx SendW [$ctx ReadW] }
    ECOD  { $ctx SendD [$ctx ReadD] }
    ECOC  { $ctx SendC [$ctx ReadC] }
    ECOU  { $ctx ReadProxy $ctx	    }
    ECOB  { $ctx SendB [$ctx ReadB] }
    ECON  { $ctx SendC [$ctx ReadC]-[$ctx ConfigGetName] }
  }
  $ctx SendRETURN
}

proc Ot_Error {ctx} {
  $ctx SendSTART
  switch -exact [$ctx ServiceGetToken] {
    ERR2  {
      $ctx ErrorC Ot_ERR2 10 "some error"
    }
    ERR3  {
      # double return error
      $ctx SendRETURN
    }
    ERR4  {
      exit 1
    }
    ERR5  {
      $ctx ReadProxy [$ctx ReadU]
    }
    ERR6  {
      $ctx SendU $ctx
    }
  }
  $ctx SendRETURN
}

proc Ot_ERRT {ctx} {
  $ctx SendSTART
  $ctx ErrorC "MYERR" 9 [$ctx ReadC]
  $ctx SendERROR
}

proc Ot_MSQT {ctx} {
  $ctx SendSTART
  if {[$ctx ConfigGetDebug]} {
    $ctx SendC debug
    $ctx SendI [$ctx ConfigGetDebug]
  }
  #$ctx SendC timeout
  #$ctx SendI [$ctx ConfigGetTimeout]
  #$ctx SendC buffersize
  #$ctx SendI [$ctx ConfigGetBuffersize]
  #$ctx SendC name
  #$ctx SendC [$ctx ConfigGetName]
  #$ctx SendC srvname
  #$ctx SendC [$ctx ConfigGetSrvName]
  if {![$ctx ConfigGetIsString]} {
    $ctx SendC binary
  }
  if {[$ctx ConfigGetIsSilent]} {
    $ctx SendC silent
  }
  $ctx SendC "sOc"
  $ctx SendC [expr {[$ctx ConfigGetIsServer] ? "SERVER" : "CLIENT"}]
  $ctx SendC "pOc"
  $ctx SendC [expr {[$ctx LinkIsParent] ? "PARENT" : "CHILD"}]
  $ctx SendRETURN
}

proc Ot_GTCX {ctx} {
  $ctx SendSTART
  $ctx SendI [$ctx LinkGetCtxId]
  $ctx SendRETURN
}

proc Ot_GTTO {ctx} {
  $ctx SendSTART
  $ctx SendC [$ctx ServiceGetToken]
  $ctx SendRETURN
}

proc Ot_CSV1 {ctx} {
  # call an other service
  $ctx SendSTART
  set num [$ctx ReadI]
  incr num
  $ctx SendI $num
  $ctx SendEND_AND_WAIT CSV2 10

  # read the answer and send the result back
  $ctx SendSTART
  set num [$ctx ReadI]
  incr num
  $ctx SendI $num
  $ctx SendRETURN
}

proc Ot_SLEP {ctx} {
  set i [$ctx ReadI]
  after [expr {$i * 1000}]
  $ctx SendSTART
  $ctx SendRETURN
}

proc Ot_USLP {ctx} {
  set i [$ctx ReadI]
  after [expr {$i / 1000}]
  $ctx SendSTART
  $ctx SendI $i
  $ctx SendRETURN
}

proc EchoList {ctx doincr} {
  while {[$ctx ReadItemExists]} {
    set buf [$ctx ReadU]
    if {[$buf GetType] eq "L"} {
      $ctx ReadL_START $buf
      $ctx SendL_START
      EchoList $ctx $doincr
      $ctx SendL_END
      $ctx ReadL_END
    } elseif { $doincr } {
      $ctx SendI [expr {[$buf GetI]+1}]
    } else {
      $ctx SendU $buf
    }
  }
}

proc Ot_ECLI {ctx} {
  set opt   [$ctx ReadU]
  set doincr  [expr {[$opt GetType] eq "C" && [$opt GetC] == "--incr"}]
  if {!$doincr} {
    $ctx ReadUndo
  }
  rename $opt ""
  $ctx SendSTART
  EchoList $ctx $doincr
  $ctx SendRETURN
}

proc Ot_ECOL {ctx} {
  $ctx SendSTART
  $ctx ReadL_START
  $ctx SendL_START
  EchoList $ctx no
  $ctx SendL_END
  $ctx ReadL_END
  $ctx SendRETURN
}

proc Ot_EVAL {ctx} {
  $ctx SendSTART
  eval [$ctx ReadC]
  $ctx SendRETURN
}

proc Ot_SETU {ctx} {
  $ctx dict set buf [[$ctx ReadU] Dup]
}

proc Ot_GETU {ctx} {
  set buf [$ctx dict get buf]
  $ctx SendSTART
  $ctx SendU $buf
  $buf Delete
  $ctx SendRETURN
}

proc Ot_REDI {ctx} {
  ## just read the data ... do nothing else
  $ctx ReadI
}

proc Callback {ctx} {
  $ctx dict set i [$ctx ReadI]
}

proc BgError {ctx} {
  set master [$ctx SlaveGetMaster]
  if {$master ne ""} {
    $master ErrorC "BGERROR" [$ctx ErrorGetNum] [$ctx ErrorGetText]
    $master SendERROR
  }
}

proc ClientCreateParent {ctx debug startAs} {
  $ctx ConfigSetDebug $debug
  $ctx ConfigSetStartAs $startAs
  $ctx ConfigSetBgError BgError
  $ctx LinkCreate @ server --name test-server
}

proc ClientERRCreateParent {ctx debug} {
  $ctx ConfigSetDebug $debug
  $ctx ConfigSetBgError BgError
  $ctx ConfigSetName test-client
  $ctx ConfigSetSrvName test-server
  $ctx LinkCreate @ server
}

proc ClientERR2CreateParent {ctx debug} {
  $ctx ConfigSetDebug $debug
  $ctx ConfigSetName cl-err-1
  $ctx LinkCreate @ DUMMY
}

proc ClientCreateChild {ctx parent} {

  $ctx LinkCreateChild $parent
}

proc Ot_SND1 {ctx} {
  
  set cmd [$ctx ReadC]
  set id  [$ctx ReadI]

  # link the context specific data into the procedure namespace
  set cl  [$ctx dict get $id]

  $ctx SendSTART
  switch -exact $cmd {
    START {
      set parent  [$ctx LinkGetParent]
      if {$parent ne "" && [[$parent dict get $id] LinkIsConnected]} {
	ClientCreateChild $cl [$parent dict get $id]
      } else {
	ClientCreateParent $cl [$ctx ConfigGetDebug] [$ctx ConfigGetStartAs]
      }
    }
    START2 {
      ClientCreateParent $cl [$ctx ConfigGetDebug] [$ctx ConfigGetStartAs]
      ClientCreateParent $cl [$ctx ConfigGetDebug] [$ctx ConfigGetStartAs]
    }
    START3 {
      ClientCreateChild $cl [tclmsgque MqS]
    }
    START4 {
      $cl SlaveWorker 0
    }
    START5 {
      $ctx SlaveWorker $id --name wk-cl-$id @ --name wk-sv-$id 
    }
    STOP {
      $cl LinkDelete
    }
    SEND {
      set TOK [$ctx ReadC]
      $cl SendSTART
      $ctx ReadProxy $cl
      $cl SendEND $TOK
    }
    WAIT {
      $cl SendSTART
      $ctx ReadProxy $cl
      $cl SendEND_AND_WAIT "ECOI" 5
      $ctx SendI [expr {[$cl ReadI] + 1}]
    }
    CALLBACK {
      $cl dict set i -1
      $cl SendSTART
      $ctx ReadProxy $cl
      $cl SendEND_AND_CALLBACK "ECOI" Callback
      $cl ProcessEvent -timeout 10 -wait ONCE
      $ctx SendI [expr {[$cl dict get i] + 1}]
    }
    ERR-1 {
      $cl SendSTART
      if {[catch {
	$ctx ReadProxy $cl
	$cl SendEND_AND_WAIT "ECOI" 5
      }]} {
	$ctx ErrorSet
	$ctx SendI [$ctx ErrorGetNum]
	$ctx SendC [$ctx ErrorGetText]
	$ctx ErrorReset
      }
    }
  }
  $ctx SendRETURN
}

proc Callback2 {ctx} {
  $ctx dict set i [$ctx ReadI]
}

proc Ot_SND2 {ctx} {
  
  set cmd [$ctx ReadC]
  set id  [$ctx ReadI]

  # link the context specific data into the procedure namespace
  set cl [$ctx SlaveGet $id]

  $ctx SendSTART
  switch -exact $cmd {
    CREATE {
      $ctx SlaveWorker $id {*}[$ctx ReadAll] --name wk-cl-$id @ --name wk-sv-$id
    }
    CREATE2 {
      set c [tclmsgque MqS]
      ClientCreateParent $c [$ctx ConfigGetDebug] [$ctx ConfigGetStartAs]
      $ctx SlaveCreate $id $c
    }
    CREATE3 {
      set c [tclmsgque MqS]
      ClientERRCreateParent $c [$ctx ConfigGetDebug]
      $ctx SlaveCreate $id $c
    }
    DELETE {
      $ctx SlaveDelete $id
      $ctx SendC [expr {[$ctx SlaveGet $id] eq "" ? "OK" : "ERROR"}]
    }
    SEND {
      set TOK [$ctx ReadC]
      $cl SendSTART
      $ctx ReadProxy $cl
      $cl SendEND $TOK
    }
    WAIT {
      $cl SendSTART
      $cl SendN [$ctx ReadN]
      $cl SendEND_AND_WAIT "ECOI" 5
      $ctx SendI [expr {[$cl ReadI] + 1}]
    }
    CALLBACK {
      $cl dict set i -1
      $cl SendSTART
      $ctx ReadProxy $cl
      $cl SendEND_AND_CALLBACK "ECOI" Callback2
      $cl ProcessEvent -timeout 10 -wait ONCE
      $ctx SendI [expr {[$cl dict get i] + 1}]
    }
    MqSendEND_AND_WAIT {
      set TOK [$ctx ReadC]
      $cl SendSTART
      while {[$ctx ReadItemExists]} {
	$ctx ReadProxy $cl
      }
      $cl SendEND_AND_WAIT $TOK 5
      while {[$cl ReadItemExists]} {
	$cl ReadProxy $ctx      
      }
    }
    MqSendEND {
      set TOK [$ctx ReadC]
      $cl SendSTART
      while {[$ctx ReadItemExists]} {
	$ctx ReadProxy $cl
      }
      $cl SendEND $TOK
      return
    }
    ERR-1 {
      set c [tclmsgque MqS]
      ClientERR2CreateParent $c [$ctx ConfigGetDebug]
    }
    isSlave {
      $ctx SendO [$cl SlaveIs]
    }
  }
  $ctx SendRETURN
}

proc Ot_ROT1 {ctx} {
  
  set cmd   [$ctx ReadC]
  set name  [$ctx ReadC]

  $ctx SendSTART
  switch -exact $cmd {
    CREATE {
      $ctx LinkCreateRoute $name
    }
  }
  $ctx SendRETURN
}

proc Ot_BUF1 {ctx} {
  set buf [$ctx ReadU]
  set typ [$buf GetType]
  $ctx SendSTART
  $ctx SendC $typ
  switch -exact $typ {
    "Y" { $ctx SendY [$buf GetY] }
    "O" { $ctx SendO [$buf GetO] }
    "S" { $ctx SendS [$buf GetS] }
    "I" { $ctx SendI [$buf GetI] }
    "F" { $ctx SendF [$buf GetF] }
    "W" { $ctx SendW [$buf GetW] }
    "D" { $ctx SendD [$buf GetD] }
    "C" { $ctx SendC [$buf GetC] }
    "B" { $ctx SendB [$buf GetB] }
  }
  rename $buf ""
  $ctx SendRETURN 
}

proc Ot_BUF2 {ctx} {
  $ctx SendSTART

  foreach i {1 2 3} {
    set buf [$ctx ReadU]
    $ctx SendC [$buf GetType]
    $ctx SendU $buf
    rename $buf ""
  }

  $ctx SendRETURN 
}

proc Ot_BUF3 {ctx} {
  $ctx SendSTART
  set buf [$ctx ReadU]
  $ctx SendC [$buf GetType]
  $ctx SendU $buf
  $ctx SendI [$ctx ReadI]
  $ctx SendU $buf
  rename $buf ""
  $ctx SendRETURN 
}

proc Ot_LST1 {ctx} {
  $ctx SendSTART
  $ctx SendL_END
  $ctx SendRETURN 
}

proc Ot_LST2 {ctx} {
  $ctx SendSTART
  $ctx ReadL_END
  $ctx SendRETURN 
}

proc Ot_CNFG {ctx} {
  $ctx SendSTART
  $ctx SendO [$ctx ConfigGetIsServer]
  $ctx SendO [$ctx LinkIsParent]
  $ctx SendO [$ctx SlaveIs]
  $ctx SendO [$ctx ConfigGetIsString]
  $ctx SendO [$ctx ConfigGetIsSilent]
  $ctx SendO [$ctx LinkIsConnected]
  $ctx SendC [$ctx ConfigGetName]
  $ctx SendI [$ctx ConfigGetDebug]
  $ctx SendI [$ctx LinkGetCtxId]
  $ctx SendC [$ctx ServiceGetToken]
  $ctx SendRETURN 
}

proc Ot_ERLR {ctx} {
  $ctx SendSTART
  $ctx ReadL_START
  $ctx ReadL_START
  $ctx SendRETURN 
}

proc Ot_ERLS {ctx} {
  $ctx SendSTART
  $ctx SendL_START
  $ctx SendU [$ctx ReadU]
  $ctx SendL_START
  $ctx SendU [$ctx ReadU]
  $ctx SendRETURN 
}

proc Ot_CFG1 {ctx} {
  set cmd [$ctx ReadC]

  $ctx SendSTART

  switch -exact $cmd {
    "Buffersize" {
      set old [$ctx ConfigGetBuffersize] 
      $ctx ConfigSetBuffersize [$ctx ReadI]
      $ctx SendI [$ctx ConfigGetBuffersize]
      $ctx ConfigSetBuffersize $old
    }
    "Debug" {
      set old [$ctx ConfigGetDebug] 
      $ctx ConfigSetDebug [$ctx ReadI]
      $ctx SendI [$ctx ConfigGetDebug]
      $ctx ConfigSetDebug $old
    }
    "Timeout" {
      set old [$ctx ConfigGetTimeout] 
      $ctx ConfigSetTimeout [$ctx ReadW]
      $ctx SendW [$ctx ConfigGetTimeout]
      $ctx ConfigSetTimeout $old
    }
    "Name" {
      set old [$ctx ConfigGetName] 
      $ctx ConfigSetName [$ctx ReadC]
      $ctx SendC [$ctx ConfigGetName]
      $ctx ConfigSetName $old
    }
    "SrvName" {
      set old [$ctx ConfigGetSrvName] 
      $ctx ConfigSetSrvName [$ctx ReadC]
      $ctx SendC [$ctx ConfigGetSrvName]
      $ctx ConfigSetSrvName $old
    }
    "Storage" {
      set old [$ctx ConfigGetStorage] 
      $ctx ConfigSetStorage [$ctx ReadC]
      $ctx SendC [$ctx ConfigGetStorage]
      $ctx ConfigSetStorage $old
    }
    "Ident" {
      set old [$ctx FactoryCtxIdentGet] 
      set ident [$ctx ReadC]
      $ctx FactoryCtxSet [[tclmsgque FactoryGet] Copy $ident]
      set check [expr {[$ctx LinkGetTargetIdent] eq [$ctx ReadC]}]
      $ctx SendSTART
      $ctx SendC [$ctx FactoryCtxIdentGet]
      $ctx SendO $check
      $ctx FactoryCtxIdentSet $old
    }
    "IsSilent" {
      set old [$ctx ConfigGetIsSilent] 
      $ctx ConfigSetIsSilent [$ctx ReadO]
      $ctx SendO [$ctx ConfigGetIsSilent]
      $ctx ConfigSetIsSilent $old
    }
    "IsString" {
      set old [$ctx ConfigGetIsString] 
      $ctx ConfigSetIsString [$ctx ReadO]
      $ctx SendO [$ctx ConfigGetIsString]
      $ctx ConfigSetIsString $old
    }
    "IoUds" {
      set old [$ctx ConfigGetIoUdsFile] 
      $ctx ConfigSetIoUdsFile [$ctx ReadC]
      $ctx SendC [$ctx ConfigGetIoUdsFile]
      $ctx ConfigSetIoUdsFile $old
    }
    "IoTcp" {
      set host	  [$ctx ConfigGetIoTcpHost] 
      set port	  [$ctx ConfigGetIoTcpPort] 
      set myhost  [$ctx ConfigGetIoTcpMyHost] 
      set myport  [$ctx ConfigGetIoTcpMyPort] 
      $ctx ConfigSetIoTcp {*}[$ctx ReadAll]
      $ctx SendC  [$ctx ConfigGetIoTcpHost] 
      $ctx SendC  [$ctx ConfigGetIoTcpPort] 
      $ctx SendC  [$ctx ConfigGetIoTcpMyHost] 
      $ctx SendC  [$ctx ConfigGetIoTcpMyPort] 
      $ctx ConfigSetIoTcp $host $port $myhost $myport
    }
    "IoPipe" {
      set old [$ctx ConfigGetIoPipeSocket] 
      $ctx ConfigSetIoPipeSocket [$ctx ReadI]
      $ctx SendI [$ctx ConfigGetIoPipeSocket]
      $ctx ConfigSetIoPipeSocket $old
    }
    "StartAs" {
      set old [$ctx ConfigGetStartAs] 
      $ctx ConfigSetStartAs [$ctx ReadI]
      $ctx SendI [$ctx ConfigGetStartAs]
      $ctx ConfigSetStartAs $old
    }
    "DefaultIdent" {
      $ctx SendC [tclmsgque FactoryDefaultIdent]
    }
    default {
      $ctx ErrorC CFG1 1 "invalid command: $cmd"
    }
  }
  $ctx SendRETURN
}

proc Ot_ROUT {ctx} {
  set cmd [$ctx ReadC]

  $ctx SendSTART

  switch -exact $cmd {
    "Create" {
      set id  [$ctx ReadI]
      $ctx SlaveWorker $id --name wk-cl-$id @ --name wk-sv-$id --factory [$ctx ReadC]
    }
    "Ident" {
      $ctx SendC [$ctx FactoryCtxIdentGet]
    }
    "Resolve" {
      foreach myctx [tclmsgque Resolve [$ctx ReadC]] {
	$ctx SendC [$myctx LinkGetTargetIdent]
      }
    }
    default {
      $ctx SendC nothing
    }
  }
  $ctx SendRETURN
}

proc Ot_PRNT {ctx} {
  $ctx SendSTART
  $ctx SendC "[$ctx LinkGetCtxId] - [$ctx ReadC]"
  $ctx SendEND_AND_WAIT WRIT
  $ctx SendRETURN
}

proc Ot_TRNS {ctx} {
  $ctx SendSTART
  $ctx SendT_START
  $ctx SendI 9876
  $ctx SendT_END "TRN2"
  $ctx SendI [$ctx ReadI]
  $ctx SendEND_AND_WAIT "ECOI"
  $ctx ProcessEvent -wait ONCE
  $ctx SendSTART
  $ctx SendI [$ctx dict get "i"]
  $ctx SendI [$ctx dict get "j"]
  $ctx SendRETURN
}

proc Ot_TRN2 {ctx} {
  $ctx ReadT_START
  $ctx dict set "i" [$ctx ReadI]
  $ctx ReadT_END
  $ctx dict set "j" [$ctx ReadI]
}

proc Ot_STDB {ctx} {
  $ctx SendSTART
  $ctx StorageOpen [$ctx ReadC]
  $ctx SendRETURN
}

proc Ot_DMPL {ctx} {
  $ctx SendSTART
  $ctx SendI [[$ctx ReadDUMP] Size]
  $ctx SendRETURN
}

proc ServerSetup {ctx} {

  if {[$ctx SlaveIs]} {
    # setup "slave"
  } else {

    for {set i 0} {$i < 3} {incr i} {
      set cl [tclmsgque MqS]
      $cl ConfigSetName "cl-$i"
      $cl ConfigSetSrvName "sv-$i"
      $ctx dict set $i $cl
    }

    # add "master" services here
    $ctx ServiceCreate RDUL Ot_RDUL
    $ctx ServiceCreate ECUL Ot_ECUL

    $ctx ServiceCreate ECOY Ot_ECHO
    $ctx ServiceCreate ECOO Ot_ECHO
    $ctx ServiceCreate ECOS Ot_ECHO
    # START-TCL-SERVICE-DEFINITION
    $ctx ServiceCreate ECOI Ot_ECHO
    # END-TCL-SERVICE-DEFINITION
    $ctx ServiceCreate ECOF Ot_ECHO
    $ctx ServiceCreate ECOW Ot_ECHO
    $ctx ServiceCreate ECOD Ot_ECHO
    $ctx ServiceCreate ECOC Ot_ECHO
    $ctx ServiceCreate ECOB Ot_ECHO
    $ctx ServiceCreate ECOU Ot_ECHO
    $ctx ServiceCreate ECON Ot_ECHO

    $ctx ServiceCreate ERR1 Ot_Error
    $ctx ServiceCreate ERR2 Ot_Error
    $ctx ServiceCreate ERR3 Ot_Error
    $ctx ServiceCreate ERR4 Ot_Error
    $ctx ServiceCreate ERR5 Ot_Error
    $ctx ServiceCreate ERR6 Ot_Error
    $ctx ServiceCreate ERRT Ot_ERRT
    $ctx ServiceCreate MSQT Ot_MSQT
    $ctx ServiceCreate GTCX Ot_GTCX
    $ctx ServiceCreate GTTO Ot_GTTO
    $ctx ServiceCreate SLEP Ot_SLEP
    $ctx ServiceCreate CSV1 Ot_CSV1
    $ctx ServiceCreate USLP Ot_USLP
    $ctx ServiceCreate ECOL Ot_ECOL
    $ctx ServiceCreate ECLI Ot_ECLI
    $ctx ServiceCreate EVAL Ot_EVAL
    $ctx ServiceCreate SETU Ot_SETU
    $ctx ServiceCreate GETU Ot_GETU
    $ctx ServiceCreate SND1 Ot_SND1
    $ctx ServiceCreate SND2 Ot_SND2
    $ctx ServiceCreate REDI Ot_REDI
    $ctx ServiceCreate BUF1 Ot_BUF1
    $ctx ServiceCreate BUF2 Ot_BUF2
    $ctx ServiceCreate BUF3 Ot_BUF3
    $ctx ServiceCreate LST1 Ot_LST1
    $ctx ServiceCreate LST2 Ot_LST2
    $ctx ServiceCreate INIT Ot_INIT
    $ctx ServiceCreate CNFG Ot_CNFG
    $ctx ServiceCreate ERLR Ot_ERLR
    $ctx ServiceCreate ERLS Ot_ERLS
    $ctx ServiceCreate CFG1 Ot_CFG1
    $ctx ServiceCreate ROUT Ot_ROUT
    $ctx ServiceCreate PRNT Ot_PRNT
    $ctx ServiceCreate TRN2 Ot_TRN2
    $ctx ServiceCreate TRNS Ot_TRNS
    $ctx ServiceCreate STDB Ot_STDB
    $ctx ServiceCreate DMPL Ot_DMPL

    $ctx ServiceCreate ROT1 Ot_ROT1
  }
}

proc ServerCleanup {ctx} {
  if {[$ctx SlaveIs]} {
    # cleanup "slave"
  } else {
    [$ctx dict get 0] Delete
    [$ctx dict get 1] Delete
    [$ctx dict get 2] Delete
  }
}

proc ServerFactory {tmpl} {
#puts ServerFactory-11111111111111111111111111111111111111111111111111111
  set srv [tclmsgque MqS $tmpl]
  $srv ConfigSetServerSetup ServerSetup
  $srv ConfigSetServerCleanup ServerCleanup
  return $srv
}

# only used to start the initial process
tclmsgque Main {

  set srv [[tclmsgque FactoryAdd "server" ServerFactory] New]
  if {[catch {
    # create the initial parent-context and wait forever for events
    $srv LinkCreate {*}$argv
    $srv LogC "test" 1 "this is the log test\n"
    $srv ProcessEvent -wait FOREVER
  }]} {
    $srv ErrorSet
  }
  $srv Exit
}

# the end, do !not! use the tcl "exit" command because in "thread" mode 
# this will kill the entire server and not only the "thread"

