#+
#§  \file       theGuard/tests/library.tcl
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

if 0 {

##
## define the language list
if {![info exists env(LNG_LST)]} {
  set env(LNG_LST) {guard}
}

##
## define the local server
array set TS_SERVER [list \
  guard  [file normalize [file join [pwd] .. aguard aguard]] \
]

##
## define the server list
if {![info exists env(SRV_LST)]} {
  set env(SRV_LST) [list \
    guard.pipe.pipe \
    guard.tcp.fork guard.tcp.thread guard.tcp.spawn \
    guard.uds.fork guard.uds.thread guard.uds.spawn \
  ]
}

}

##
## define the directory environment
if {![info exists testdir]} {
  set testdir [file normalize [file join [file dirname [info script]]]]
  set linksrcdir [file join [file dirname [info script]] .. .. theLink]
  set linkbuilddir  [file normalize [file join [pwd] .. .. theLink] ]
}
set linktestdir [file normalize [file join [file dirname [info script]] .. .. theLink tests]]

##
## load the test library
source [file join [file dirname [info script]] .. .. theLink tests library.tcl]

##
## add guard specific data
append env(PATH) "$PATH_SEP[file nativename [file join $testdir .. aguard]]"

##
## check if GUARD is available
testConstraint guard $USE_GUARD

