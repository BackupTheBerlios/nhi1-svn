#!/usr/bin/env tclsh
#+
#:  \file       theBrain/tests/all.tcl
#:  \brief      \$Id$
#:  
#:  (C) 2002 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

source [file join [file dirname [info script]] libbrain.tcl]

singleProcess false

## switch to test directory
## test if the start was allready in test directory "make check"
testsDirectory $testdir

tcltest::verbose {pass body error}

# just run the files specified by the following line
#configure -file s*.test

tcltest::runAllTests

