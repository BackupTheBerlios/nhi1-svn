#!/usr/bin/env Nhi1Exec
#+
#:  \file       sbin/change.tcl
#:  \brief      \$Id$
#:  
#:  (C) 2009 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

if {[lindex $argv 0] eq "-h"} {
  puts {USAGE: change.tcl [-t] [-s] "FromPat/Text" "ToPat/Text" files...}
  puts {         -t ... test}
  puts {         -s ... use TEXT for "From/To" (default RegExp)}
  exit 0
}

if {[lindex $argv 0] eq "-t"} {
  set TEST	true
  set argv	[lrange $argv 1 end]
} else {
  set TEST	false
}

if {[lindex $argv 0] eq "-s"} {
  set STRING  true
  set argv	[lrange $argv 1 end]
} else {
  set STRING  false
}

set Von		[ lindex $argv 0 ]
set Nach	[ lindex $argv 1 ]
set HOME	$env(HOME)

puts "$Von - $Nach"

set Ret 0

if {![file isdirectory $HOME/tmp]} {file mkdir $HOME/tmp}

if {![llength $argv]} {
	puts stderr "usage: [file tail $argv0] ?-t? ?-s? \[from\] \[to\] \[arg...\]"
	exit 0
}

foreach F [lrange $argv 2 end] {

	set F [file normalize $F]

	if { [file isdirectory $F] || ![file readable $F] } continue

	set HR 	[ open $F r ]
	set	Str	[ read $HR ]
	close $HR

	if {$STRING} {
		if {[string first "$Von" $Str] == -1} continue
	} else {
		if {![regexp -line -- "$Von" $Str]} continue
	}

	set Ret 0
	set Write 0
	set Path [file join $HOME tmp [ file tail $F ]]

	puts ""
	puts [ format {Bearbeiten von: %30s - Transformation ==> Von: %-20s, Nach: %-20s} \
		[string range $F end-30 end] [string range $Von 0 20] [string range $Nach 0 20] ]

	if {$TEST} {
		set HW [open $Path w]
	} else {
		if {![ file writable $F ]} { file attributes $F -permissions +w ; set Write 1 ; }
		exec cp -ab --backup=numbered $F $Path
		exec cp -a 	$F $Path
		set HW [open $F w]
	}

	if {$STRING} {
		puts $HW [string map [list "$Von" "$Nach"] $Str]
	} else {
		puts $HW [regsub -line -all -- $Von $Str $Nach]
	}

	close $HW
	if {$TEST} {
		puts [exec grep "$Nach" $Path]
	} else {
		if {[ file size $F ] == 0} {
			file copy $Path $F
			puts stderr "ERROR: Can't change $F"
		}
		if {$Write} { file attributes $F -permissions -w }
	}
}

exit $Ret

# vim: tabstop=4 shiftwidth=4
