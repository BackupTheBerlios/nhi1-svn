#+
#§  \file       theLink/example/tcl/Filter1.tcl
#§  \brief      \$Id: Filter1.tcl 353 2010-12-09 19:10:11Z aotto1968 $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 353 $
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§
package require TclMsgque

# ==================================================================
# F1
#
proc F1 {ctx} {
  set ftr [$ctx ServiceGetFilter]
  $ftr SendSTART
  $ftr SendC "F1"
  $ftr SendC [$ftr ConfigGetName]
  $ftr SendC [$ftr ConfigGetStartAs]
  while {[$ctx ReadItemExists]} {
    $ftr SendC [$ctx ReadC]
  }
  $ftr SendEND_AND_WAIT "+FTR"
  $ctx SendRETURN
}

proc F1Setup {ctx} {
  $ctx ServiceCreate "+FTR" F1
  $ctx ServiceProxy  "+EOF"
}

proc NewF1 {tmpl} {
  set ret [tclmsgque MqS $tmpl]
  $ret ConfigSetServerSetup F1Setup
  return $ret
}

# ==================================================================
# F2
#
proc F2 {ctx} {
  set ftr [$ctx ServiceGetFilter]
  $ftr SendSTART
  $ftr SendC "F2"
  $ftr SendC [$ftr ConfigGetName]
  $ftr SendC [$ftr ConfigGetStartAs]
  while {[$ctx ReadItemExists]} {
    $ftr SendC [$ctx ReadC]
  }
  $ftr SendEND_AND_WAIT "+FTR"
  $ctx SendRETURN
}

proc F2Setup {ctx} {
  $ctx ServiceCreate "+FTR" F2
  $ctx ServiceProxy  "+EOF"
}

proc NewF2 {tmpl} {
  set ret [tclmsgque MqS $tmpl]
  $ret ConfigSetServerSetup F2Setup
  return $ret
}

# ==================================================================
# F3
#
proc F3 {ctx} {
  set ftr [$ctx ServiceGetFilter]
  $ftr SendSTART
  $ftr SendC "F3"
  $ftr SendC [$ftr ConfigGetName]
  $ftr SendC [$ftr ConfigGetStartAs]
  while {[$ctx ReadItemExists]} {
    $ftr SendC [$ctx ReadC]
  }
  $ftr SendEND_AND_WAIT "+FTR"
  $ctx SendRETURN
}

proc F3Setup {ctx} {
  $ctx ServiceCreate "+FTR" F3
  $ctx ServiceProxy  "+EOF"
}

proc NewF3 {tmpl} {
  set ret [tclmsgque MqS $tmpl]
  $ret ConfigSetServerSetup F3Setup
  return $ret
}

# ==================================================================
# Main
#

# only used to start the initial process
tclmsgque Main {
  tclmsgque FactoryAdd F1 NewF1
  tclmsgque FactoryAdd F2 NewF2
  tclmsgque FactoryAdd F3 NewF3

  set srv [tclmsgque FactoryCall [lindex $argv 0]]

  if {[catch {
    $srv LinkCreate {*}$argv
    $srv ProcessEvent -wait FOREVER
  }]} {
    $srv ErrorSet
  }
  $srv Exit
}
