#!/usr/bin/env tclsh
#+
#§  \file       bin/random.tcl
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

proc usage {} {
  puts stderr "usage $::argv0 keyLength"
  exit 1
}

if {[llength $argv] != 1} usage
set NUM [lindex $argv 0]
if {![string is integer -strict $NUM] || $NUM <= 0} usage

puts "#define KEY_LENGTH $NUM"
puts "MQ_BINB KEY_DATA\[] = {"

for {set i 0} {$i<$NUM} {incr i} {
  puts -nonewline "  "
  for {set y 0} {$y<20 && $i<$NUM} {incr y;incr i} {
    puts -nonewline [format "0x%02x, " [expr {int(256 *rand())}]]
  }
  if {$i<$NUM} {
    puts ""
  } else {
    puts "0x00"
  }
}

puts "};"

exit 0
