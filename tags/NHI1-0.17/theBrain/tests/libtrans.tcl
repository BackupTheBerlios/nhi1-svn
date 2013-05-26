#+
#:  \file       theBrain/tests/libtrans.tcl
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
## set filter
set env(TS_FILTER_SERVER)  atrans
set env(TS_LIBRARY) [info script]

##
## define the directory environment
if {![info exists testdir]} {
  set testdir	      [file normalize [file join [file dirname [info script]]]]
  set linksrcdir      [file normalize [file join [file dirname [info script]] .. .. theLink]]
  set linkbuilddir    [file normalize [file join [pwd] .. .. theLink]]
  set guardsrcdir     [file join [file dirname [info script]] ..]
  set transbuilddir   [file dirname [pwd]]
}

##
## load the test library
source [file join $linksrcdir tests library.tcl]

##
## add guard specific data
append env(PATH) "$PATH_SEP[file nativename [file join $transbuilddir atrans]]"
