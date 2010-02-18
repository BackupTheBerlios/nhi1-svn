#!/usr/bin/env tclsh
#+
#§  \file       bin/svn_ps_svn_ignore.tcl
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

## USAGE
## add all files from the directory tree starting with
## the current directory and *not* known by svn
##   svn st
##   ? ....
## into the "svn:ignore" property

set TMPF    [file join . svn_ps_svn_ignore.tmp]

foreach {p f} [exec svn st] {
  if {$p ne "?"} continue
  lappend DIRS([file dirname $f]) [file tail $f]
}

foreach {k v} [array get DIRS] {
  set DATA [split [exec svn pg svn:ignore $k] \n]
  lappend DATA {*}$v
  set FH [open $TMPF w]
  fconfigure $FH -translation lf -encoding ascii
  puts $FH [join $DATA \n]
  close $FH
  puts ">>> $k"
  puts [exec svn ps --file $TMPF svn:ignore $k]
}

file delete $TMPF
