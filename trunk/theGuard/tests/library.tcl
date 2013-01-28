#+
#:  \file       theGuard/tests/library.tcl
#:  \brief      \$Id$
#:  
#:  (C) 2009 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

##
## define the directory environment
if {![info exists testdir]} {
  set testdir	      [file normalize [file join [file dirname [info script]]]]
  set linksrcdir      [file normalize [file join [file dirname [info script]] .. .. theLink]]
  set linkbuilddir    [file normalize [file join [pwd] .. .. theLink]]
  set guardsrcdir     [file join [file dirname [info script]] ..]
  set guardbuilddir   [file dirname [pwd]]
}

##
## set filter
set env(TS_FILTER)  [file join $guardbuilddir aguard aguard]
set env(TS_LIBRARY) [info script]

##
## load the test library
source [file join [file dirname [info script]] .. .. theLink tests library.tcl]

##
## add guard specific data
append env(PATH) "$PATH_SEP[file nativename [file join $guardbuilddir aguard]]"

##
## check if GUARD is available
testConstraint guard $USE_GUARD
testConstraint filter-test yes

