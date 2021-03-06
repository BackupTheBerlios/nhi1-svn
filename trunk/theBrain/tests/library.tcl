#+
#:  \file       theBrain/tests/library.tcl
#:  \brief      \$Id$
#:  
#:  (C) 2009 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

set TS_LIBRARY [info script]

##
## setup configuration data
array set DB_LST {
  file	test.db
  mem	:memory:
  temp	{}
}

array set SQL {
  testtbl   "CREATE TABLE testtbl (id INTEGER PRIMARY KEY, dval double, tval TEXT NOT NULL);"
  droptbl   "DROP TABLE testtbl;"
  doupltbl  "CREATE TABLE testtbl (id INTEGER PRIMARY KEY, dval double, tval TEXT);"
  simptbl   "CREATE TABLE simptbl (id INTEGER PRIMARY KEY);"
  errtbl    "CREATE TOBLE errtbl  (id INTEGER PRIMARY KEY, dval double, tval TEXT);"

  testins   "/* in:IDC */ INSERT INTO testtbl (id, dval, tval) VALUES (?, ?, ?);"

  testsel   "/* in:I out:DC */ SELECT dval, tval FROM testtbl WHERE id < ?;"
}

##
## setup database files
array set DB_LST {
  file  test.db
  mem   :memory:
  temp  {}
}

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
  set testdir         [file normalize [file join [file dirname [info script]]]]
  set linksrcdir      [file normalize [file join [file dirname [info script]] .. .. theLink]]
  set linkbuilddir    [file normalize [file join [pwd] .. .. theLink]]
}

##
## load the test library
source [file join $linksrcdir tests library.tcl]

##
## check if BRAIN is available
testConstraint brain $USE_BRAIN

