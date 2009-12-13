#+
#§  \file       theLink/tests/all.tcl
#§  \brief      \$Id: all.tcl 43 2009-12-08 11:05:50Z aotto1968 $
#§  
#§  (C) 2002 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 43 $
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

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
#configure -file s*.test

tcltest::runAllTests

