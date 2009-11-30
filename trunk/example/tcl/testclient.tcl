#+
#§  \file       example/tcl/testclient.tcl
#§  \brief      \$Id: testclient.tcl 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§
package require TclMsgque

set server  [file join [file dirname [info script]] testserver.tcl]

proc Get {ctx} {
  $ctx SendSTART
  $ctx SendEND_AND_WAIT "GTCX"
  append RET [$ctx ConfigGetName]
  append RET "+"
  append RET [$ctx ReadC]
  append RET [$ctx ReadC]
  append RET [$ctx ReadC]
  append RET [$ctx ReadC]
  append RET [$ctx ReadC]
  append RET [$ctx ReadC]
  return $RET
}

# create object
set LIST [list c0 c00 c01 c000 c1 c10 c100 c101]
foreach F $LIST {
  set FH($F) [tclmsgque MqS]
}

## create link
$FH(c0)	  LinkCreate		    "--name" "c0"   "--srvname" "s0" "--debug" $env(TS_DEBUG) "@" "tclsh" $server
$FH(c00)  LinkCreateChild $FH(c0)   "--name" "c00"  "--srvname" "s00"
$FH(c01)  LinkCreateChild $FH(c0)   "--name" "c01"  "--srvname" "s01"
$FH(c000) LinkCreateChild $FH(c00)  "--name" "c000" "--srvname" "s000"
$FH(c1)	  LinkCreate     	    "--name" "c1"   "--srvname" "s1" {*}$argv
$FH(c10)  LinkCreateChild $FH(c1)   "--name" "c10"  "--srvname" "s10"
$FH(c100) LinkCreateChild $FH(c10)  "--name" "c100" "--srvname" "s100"
$FH(c101) LinkCreateChild $FH(c10)  "--name" "c101" "--srvname" "s101"

## do the tests
foreach F $LIST {
  puts [Get $FH($F)]
}

## do the cleanup
$FH(c0) LinkDelete
$FH(c1) LinkDelete


