#+
#§  \file       theBrain/tests/library.tcl
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

set TS_LIBRARY [info script]

##
## define the language list
if {![info exists env(LNG_LST)]} {
  set env(LNG_LST) {db}
}

##
## define the local server
array set TS_SERVER [list \
  db  [file normalize [file join [pwd] .. abrain abrain]] \
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
  set linksrcdir [file join [file dirname [info script]] .. .. theLink]
  set linkbuilddir  [file normalize [file join [pwd] .. .. theLink] ]

}

##
## load the test library
source [file join [file dirname [info script]] .. .. theLink tests library.tcl]

##
## check if BRAIN is available
testConstraint brain $USE_BRAIN

