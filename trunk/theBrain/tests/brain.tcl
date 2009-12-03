#+
#§  \file       theBrain/tests/brain.tcl
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

##
## define the language list
if {![info exists env(LNG_LST)]} {
  set env(LNG_LST) {db}
}

##
## define the local server
array set TS_SERVER [list \
  db  [file normalize [file join [file dirname [info script]] .. libbrain brain]] \
]

##
## define the server list
if {![info exists env(SRV_LST)]} {
  set env(SRV_LST) [list \
    db.pipe.pipe \
    db.tcp.fork db.tcp.thread db.tcp.spawn \
    db.uds.fork db.uds.thread db.uds.spawn \
  ]
}

##
## define the directory environment
if {![info exists testdir]} {
  set testdir [file normalize [file dirname [info script]]]
  set topdir  [file dirname $testdir]
}

##
## load the test library
source [file join [file dirname [info script]] library.tcl]

##
## check if BRAIN is available
testConstraint brain $USE_BRAIN

