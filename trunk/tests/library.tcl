#+
#:  \file       tests/library.tcl
#:  \brief      \$Id$
#:  
#:  (C) 2013 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

#set tcl_traceExec 1

#clean "--*" commandline options
#required for: make check TESTS_ENVIRONMENT="/path/to/Nhi1Exec --only-c"
set arg [list]
foreach a $argv {
  if {![string match "--*" $a]} {
	lappend arg $a
  }
}
set argc [llength $arg]
set argv $arg
unset -nocomplain arg a

source ../env.tcl

package require tcltest
namespace import -force ::tcltest::*
verbose {start pass body error}

set testdir [file normalize [file dirname [info script]]]

unset -nocomplain env(NHI1_TOOL_ROOT)
unset -nocomplain env(TCLSH)
set dir [makeDirectory tool-root-[pid]]
set PATH $env(PATH)

if { "$TCLSH" == "" } {
  puts "ERROR: require '--with-tcl' build for testing"
  exit 1
}

proc Nhi1Config {args} {
  cd $::dir
  exec [file join $::abs_top_srcdir bin Nhi1Config] +i +s -p {*}$args |& cat
}

proc configure {args} {
  cd $::dir
  exec [file join $::abs_top_srcdir configure] {*}$args |& cat
}
