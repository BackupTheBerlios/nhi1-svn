#+
#§  \file       theLink/tests/all.tcl
#§  \brief      \$Id$
#§  
#§  (C) 2002 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

if {![llength $argv]} {
  lappend argv --full-testing --only-num 3
}

source [file join [file dirname [info script]] library.tcl]

singleProcess false

## switch to test directory
## test if the start was allready in test directory "make check"
testsDirectory $testdir

## set debugging level
tcltest::verbose {pass body error}

# just run the files specified by the following line
#configure -file {[c-z]*.test}

tcltest::runAllTests

