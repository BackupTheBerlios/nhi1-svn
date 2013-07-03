#!/usr/bin/env Nhi1Exec
#+
#:  \file       sbin/KeyGen.tcl
#:  \brief      \$Id$
#:  
#:  (C) 2009 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

if {[llength $argv] > 1} {
  puts stderr "usage $::argv0 keyLength"
  exit 1
} elseif {[llength $argv] == 1} {
  set NUM [lindex $argv 0]
  if {![string is integer -strict $NUM] || $NUM <= 0} usage
} else {
  set NUM [expr {10000 + int(2735 *rand())}]
}

puts "#define KEY_LENGTH $NUM"
puts "MQ_BINB KEY_DATA\[] = {"

set i 0
while {$i<$NUM} {
  puts -nonewline "  "
  for {set y 0} {$y<20 && $i<$NUM} {incr y} {
    puts -nonewline [format "0x%02x, " [expr {int(256 *rand())}]]
    incr i
  }
  if {$i<$NUM} {
    puts ""
  } else {
    puts "0x00"
  }
}

puts "};"

exit 0
