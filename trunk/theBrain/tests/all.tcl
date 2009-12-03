#+
#§  \file       theBrain/tests/all.tcl
#§  \brief      \$Id$
#§  
#§  (C) 2002 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

set TS_SCRIPT [info script]
source [file join [file dirname [info script]] brain.tcl]

singleProcess false

## switch to test directory
## test if the start was allready in test directory "make check"
testsDirectory [file normalize [file dirname [info script]]]
if {[file tail [pwd]] ne "tests"} {
  ## check if I am in the parent of tests directory
  if {[file isdirectory [file join . tests]]} {
    cd [file join . tests]
  } else {
    cd [file dirname [info script]]
  }
}

## Darwin need to set the variable DYLD_LIBRARY_PATH first
#if {$tcl_platform(os) eq "Darwin"} {
#    set ROOT    [file dirname [file dirname [file normalize [info script]]]]
#    set BUILD   [file dirname [pwd]]
#    set T1	[file nativename [file normalize [file join $ROOT src .libs]]]
#    set T2	[file nativename [file normalize [file join $BUILD src .libs]]]
#    if {[info exists env(DYLD_LIBRARY_PATH)]} {
#	set env(DYLD_LIBRARY_PATH) "$T1:$T2:$env(DYLD_LIBRARY_PATH)"
#    } else {
#	set env(DYLD_LIBRARY_PATH) "$T1:$T2"
#    }
#    unset ROOT BUILD T1 T2
#}

tcltest::verbose {pass body error}

# just run the files specified by the following line
#configure -file s*.test

tcltest::runAllTests

