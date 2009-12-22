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

#set argv [list --only-c --testing]
#lappend argv --only-perl -match "async-F*" --only-pipe

set TS_SCRIPT [info script]
source [file join [file dirname [info script]] library.tcl]

singleProcess false

## switch to test directory
## test if the start was allready in test directory "make check"
testsDirectory $testdir

## set debugging level
tcltest::verbose {pass body error}

# just run the files specified by the following line
#configure -file example.test

tcltest::runAllTests

