#+
#§  \file       theLink/tests/library.tcl
#§  \brief      \$Id$
#§  
#§  (C) 2002 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

#set tcl_traceExec 1

if {![info exists env(TS_LIBRARY)]} {
  set env(TS_LIBRARY) [info script]
}

##
## -----------------------------------------------------------------------
## test environment setup
## 

if {![info exists testdir]} {
  set testdir [file normalize [file dirname [info script]]]
  set linksrcdir  [file dirname $testdir]
  set linkbuilddir  [file normalize [file join [pwd] ..] ]
}

source [file join $linkbuilddir .. env.tcl]

## C# using different double format depending on
## system language -> we test only the default language
set env(LANG) C

## we use the "eval" replacement "{*}"
package require Tcl 8.5

## helper (for debugging)
proc Print {args} {
  if {[info level] > 1} {
    set STR "[lindex [info level -1] 0] -> "
  } else {
    set STR "GLOBAL -> "
  }
  foreach a $args {
    append STR "$a<[uplevel set $a]>, "
  }
  puts $STR
}

proc Error {args} {
  puts stderr "ERROR: $args"
  exit 1
}

## setup the path to find the !newly! created executable first
lappend PATH [file nativename [file dirname [info nameofexecutable]]]
lappend PATH [file nativename [file join $linkbuilddir javamsgque .libs]]
lappend PATH [file nativename [file join $linkbuilddir csmsgque]]
lappend PATH [file nativename [file join $linkbuilddir tests]]
lappend PATH [file nativename [file join $linkbuilddir acmds]]
lappend PATH [file nativename [file join $linkbuilddir libmsgque .libs]]
lappend PATH [file nativename [file join $linkbuilddir ccmsgque .libs]]
lappend PATH [file nativename [file join $linkbuilddir tclmsgque .libs]]
lappend PATH [file nativename [file normalize .libs]]
lappend PATH [file nativename [file normalize .]]

switch -exact $tcl_platform(platform) {
    windows { 
	lappend PATH {C:\WINDOWS\system32}
	lappend PATH {C:\cygwin\bin}
	lappend PATH {C:\cygwin\usr\bin}
	lappend PATH {C:\cygwin\usr\X11R6\bin}
	set KILL tskill.exe
    }
    unix {
	lappend PATH /bin
	lappend PATH /usr/bin
	set KILL kill
    }
    default { 
	set KILL kill
    }
}
set env(PATH) "[join $PATH $PATH_SEP]$PATH_SEP$env(PATH)"

## setup TCL path
set TCLLIBPATH [list]
lappend TCLLIBPATH [file join $linkbuilddir tclmsgque .libs]
lappend auto_path [file join $linkbuilddir tclmsgque .libs]
set env(TCLLIBPATH) $TCLLIBPATH

## setup PYTHON path
set PYTHONPATH [list]
lappend PYTHONPATH [file nativename [file join $linkbuilddir tests]]
lappend PYTHONPATH [file nativename [file join $linkbuilddir pymsgque]]
lappend PYTHONPATH [file nativename [file join $linkbuilddir pymsgque .libs]]
set env(PYTHONPATH) [join $PYTHONPATH $PATH_SEP]
#Print env(PYTHONPATH)

## setup RUBY path
set RUBYLIB [list]
lappend RUBYLIB [file nativename [file join $linkbuilddir tests]]
lappend RUBYLIB [file nativename [file join $linkbuilddir rubymsgque]]
lappend RUBYLIB [file nativename [file join $linkbuilddir rubymsgque .libs]]
set env(RUBYLIB) [join $RUBYLIB $PATH_SEP]
#Print env(RUBYLIB)

## setup JAVA classpath
set CLASSPATH [list]
lappend CLASSPATH [file nativename [file join $linkbuilddir javamsgque javamsgque.jar]]
lappend CLASSPATH [file nativename [file join $linkbuilddir tests]]
lappend CLASSPATH [file nativename [file join $linkbuilddir example java]]
set env(CLASSPATH) [join $CLASSPATH $PATH_SEP]
#Print env(CLASSPATH)

## setup C# search path
set MONO_PATH [list]
lappend MONO_PATH [file nativename [file join $linkbuilddir tests]]
lappend MONO_PATH [file nativename [file join $linkbuilddir csmsgque]]
lappend MONO_PATH [file nativename [file join $linkbuilddir example csharp]]
set env(MONO_PATH) [join $MONO_PATH $PATH_SEP]
#Print env(MONO_PATH)

## setup PERL search path
set env(PERL5LIB) [file nativename [file join $linkbuilddir perlmsgque Net-PerlMsgque blib lib]]

## setup LIBRARY search path
set LIBRARY_PATH [list]
lappend LIBRARY_PATH [file nativename [file join $linkbuilddir libmsgque .libs]]
lappend LIBRARY_PATH [file nativename [file join $linkbuilddir javamsgque .libs]]
lappend LIBRARY_PATH [file nativename [file join $linkbuilddir tclmsgque .libs]]
lappend LIBRARY_PATH [file nativename [file join $linkbuilddir tests .libs]]
lappend LIBRARY_PATH [file nativename [file join $linkbuilddir perlmsgque Net-PerlMsgque blib arch auto Net PerlMsgque]]
set env(LD_LIBRARY_PATH) "[join $LIBRARY_PATH $PATH_SEP]$PATH_SEP$env(LD_LIBRARY_PATH)"

if {$tcl_platform(os) eq "FreeBSD"} {
    set WAIT 1000
} else {
    set WAIT 0
}

## tcl8.5 introduced an other precision as on 8.4 (reset back)
set tcl_precision 12

## the default io is PIPE
set io  Pipe

##
## -----------------------------------------------------------------------
## option procs
## 

proc Pop {_argv} {
  upvar $_argv argv
  if {[llength $argv] == 0} {
    return -code break
  }
  set ret [lindex $argv 0]
  set argv [lrange $argv 1 end]
  return $ret
}

proc optB {_argv opt {def 0}} {
  upvar $_argv argv
  set IDX 0
  while {[set IDX [lsearch -start $IDX -exact $argv $opt]] != -1} {
    set argv  [lreplace $argv $IDX $IDX]
    set def   1
  }
  return $def
}

proc optV {_argv opt {def ""}} {
  upvar $_argv argv
  set IDX 0
  while {[set IDX [lsearch -start $IDX -exact $argv $opt]] != -1} {
    set IDX2  [expr {$IDX+1}]
    set def   [lindex $argv $IDX2]
    set argv  [lreplace $argv $IDX $IDX2]
  }
  return $def
}

# delete options
proc optVD {_argv args} {
  upvar $_argv argv
  foreach opt $args {
    while {[set IDX [lsearch -exact $argv $opt]] != -1} {
      set argv    [lreplace $argv $IDX [expr {$IDX+1}]]
    }
  }
}

proc optSet {_argv opt def} {
  upvar $_argv argv
  if {[lsearch -exact $argv $opt] == -1} {
    if {$def ne ""} { lappend argv $opt $def }
    return 0
  } else {
    return 1
  }
}

proc envGet {VAR} {
    global env
    if {[info exists env($VAR)]} {
	return $env($VAR)
    }
    return ""
}

# this code is needed to filter "VAR" list with "args" regexp
proc filterGet {VAR args} {
  set NOT no
  set OR no
  while {[string index $VAR 0] == {-}} {
    switch -- $VAR {
      -not {
	set NOT yes
	set VAR [lindex $args 0]
	set args [lrange $args 1 end]
      }
      -or {
	set OR yes
	set VAR [lindex $args 0]
	set args [lrange $args 1 end]
      }
      default {
	Error "unknown option '$VAR'"
      }
    }
  }
  set RET [list]
  set LEN [llength $args]
  foreach e [envGet $VAR] {
    set RESULT 0
    foreach f $args {
      incr RESULT [regexp "$f" $e]
    }
    if {$OR} {
      if {$NOT} {
	if {$RESULT == 0} {
	  lappend RET $e
	}
      } else {
	if {$RESULT != 0} {
	  lappend RET $e
	}
      }
    } else {
      if {$NOT} {
	if {$RESULT != $LEN} {
	  lappend RET $e
	}
      } else {
	if {$RESULT == $LEN} {
	  lappend RET $e
	}
      }
    }
  }
  if {[llength RET] == 0} {
    error "$VAR is empty"
  }
  return $RET
}

proc numGet { num } {
    set ret [list]
    if {$::env(PAR_LST)} {
	set ret $::env(PAR_LST)
    } else {
	for {set i 1} {$i <= $num} {incr i} {
	    lappend ret $i
	}
    }
    return $ret
}

proc langGet {server} {
  return [lindex [split $server .] 0]
}

proc getPrefix {srv} {
    global env
    set RET [list]

    # prefix (debugger)
    if {[string match {*strace*} $srv]} {
	lappend RET strace -f -F
    }
    if {[string match {*gdb*} $srv]} {
	lappend RET gdb --tui --args
    }
    if {[string match {*ddd*} $srv]} {
	lappend RET ddd --gdb --args
    }
    return $RET
}

proc getPostfix {srv} {
    return [list --debug $::env(TS_DEBUG)]
}

if {![array exists TS_SERVER]} {
  array set TS_SERVER [list \
    csharp  [list {*}$CLREXEC [file join $linksrcdir tests csserver.exe]] \
    vb	    [list {*}$CLREXEC [file join $linksrcdir tests vbserver.exe]] \
    python  [list {*}$PYTHON  [file join $linksrcdir tests server.py]]	  \
    ruby    [list {*}$RUBY    [file join $linksrcdir tests server.rb]]	  \
    java    [list {*}$JAVA    example.Server]				  \
    tcl	    [list {*}$TCLSH   [file join $linksrcdir tests server.tcl]]	  \
    perl    [list {*}$PERL -w [file join $linksrcdir tests server.pl]]	  \
    php	    [list {*}$PHP     [file join $linksrcdir tests server.php]]	  \
    go	    [file join $linksrcdir gomsgque src server$::EXEEXT]	  \
    jdb	    [list jdb	      Server]					  \
    cc	    ccserver$::EXEEXT						  \
    c	    server$::EXEEXT						  \
  ]
}

proc getServer {srv} {
  set RET [list]

  # prefix (debugger)
  if {![string match "*noprefix*" $srv]} {
    lappend RET {*}[getPrefix $srv]
  }
  foreach {srv io start} [split $srv .] break

  # main executable
  lappend RET {*}$::TS_SERVER($srv)

  # postfix
  lappend RET {*}[getPostfix $srv]

  # startup
  if {$start ne ""} {
    lappend RET --$start
  }

  return $RET
}

proc getFilter {srv} {

  set RET [list]

  # prefix (debugger)
  lappend RET {*}[getPrefix $srv]

  # setup args
  regexp {^(.+)\.(\w+)\.(\w+)\.(\w+)$} $srv all path lng io start

  # main executable
  lappend RET {*}[getExampleExecutable $path.$lng]

  # postfix
  lappend RET {*}[getPostfix $srv]

  # startup
  if {$start ne ""} {
    lappend RET --$start
  }

  return $RET
}

proc getServerOnly {srv} {
  return $::TS_SERVER([lindex [split $srv .] 0])
}

proc getClient {args} {
  global env
  set RET [list]

  # prefix (debugger)
  lappend RET {*}[getPrefix c]

  # main executable
  lappend RET {*}$args

  # postfix
  lappend RET {*}[getPostfix c]

  # startup
  if {$env(TS_STARTUP_AS) ne "NO"} {
    lappend RET {*}$env(TS_STARTUP_AS)
  }

  return $RET
}

proc getATool {arg} {
    global env
    set RET [list]

    # prefix (debugger)
    if {$arg eq "split"} {
      lappend RET {*}[getPrefix c]
    }

    # main executable
    lappend RET atool $arg

    # postfix
    lappend RET {*}[getPostfix c]

    # startup
    if {$env(TS_STARTUP_AS) ne "NO"} {
      lappend RET {*}$env(TS_STARTUP_AS)
    }

    return $RET
}

proc getExampleExecutable {srv} {
  global env
  set RET [list]

  regexp {^(.+)\.(\w+)$} $srv all path lng

  if {[file exists $path]} {
    set RET $path
  } else {
    switch $lng {
      python	{ lappend RET {*}$::PYTHON [file join $::linksrcdir example python $path.py] }
      ruby	{ lappend RET {*}$::RUBY [file join $::linksrcdir example ruby $path.rb] }
      perl	{ lappend RET {*}$::PERL [file join $::linksrcdir example perl $path.pl] }
      php	{ lappend RET {*}$::PHP [file join $::linksrcdir example php $path.php] }
      go	{ lappend RET [file join $::linkbuilddir example c $path$::EXEEXT] }
      java	{ lappend RET {*}$::JAVA example.$path }
      csharp	{ lappend RET {*}$::CLREXEC [file join $::linkbuilddir example csharp $path.exe] }
      vb	{ lappend RET {*}$::CLREXEC [file join $::linkbuilddir example vb $path.exe] }
      tcl	{ lappend RET {*}$::TCLSH [file join $::linksrcdir example tcl $srv] }
      cc	{ lappend RET [file join $::linkbuilddir example cc $path$::EXEEXT] }
      c		{ lappend RET [file join $::linkbuilddir example c $path$::EXEEXT] }
      *		{ Error "invalid example: $srv" }
    }
  }
  return $RET
}

proc getExample {srv} {
    global env
    set RET [list]

    # prefix (debugger)
    if {[string match -nocase "*client*" $srv]} {
      lappend RET {*}[getPrefix $srv]
    }

    # get executable
    lappend RET {*}[getExampleExecutable $srv]

    # postfix
    lappend RET {*}[getPostfix $srv]

    # startup
    if {$env(TS_STARTUP_AS) ne "NO"} {
      lappend RET {*}$env(TS_STARTUP_AS)
    }

    return $RET
}

set portNUM [expr {int(rand()*30000)+10000}]

proc dummy_server {channel clientaddr clientport} {
  close $channel
}

proc FindFreePort {} {
  global env
  ## check if a user set default is available
  if {$env(TS_PORT) ne "PORT"} {
    return $env(TS_PORT)
  }
  ## generate unique port
  global portNUM
  set TRY 10
  while true {
    incr portNUM
    if {[catch {socket -server dummy_server $portNUM} FH ]} {
      Print FH
      incr TRY -1
      if {$TRY < 0} {
	Error "PANIC: unable to get a free port"
      }
      continue
    }
    close $FH
    break
  }
  return $portNUM
}

set CLEANUP_FILES [list]
set fileNUM [expr {int(rand()*30000)+10000}]

proc FindFreeFile {{ext uds}} {
  global env
  ## check if a user set default is available
  if {$ext eq "uds" && $env(TS_FILE) ne "FILE"} {
    return $env(TS_FILE)
  } elseif {$ext eq "pid" && $env(TS_PID) ne "PID"} {
    return $env(TS_PID)
  }
  ## generate unique filename
  global fileNUM CLEANUP_FILES
  while true {
    incr fileNUM
    set FILE    [file join . ${fileNUM}.$ext]
    if {![file exists $FILE]} break
  }
  lappend CLEANUP_FILES $FILE
  return $FILE
}

proc FindFreeConnection {srv} {
  set RET [list]
  foreach {lng con sta} [split $srv .] break
  switch $con {
    pipe  {
      lappend RET --pipe
    }
    tcp	  {
      lappend RET --tcp --port [FindFreePort]
    }
    uds	  {
      lappend RET --uds --file [FindFreeFile]
    }
  }
  return $RET
}

proc MkUnique {list} {
  set D [list]
  foreach d $list {
    switch -exact -- $d {
      FILE	{lappend D [FindFreeFile uds]}
      PORT	{lappend D [FindFreePort]}
      PID	{lappend D [FindFreeFile pid]}
      default	{lappend D $d}
    }
  }
  return $D
}

proc SetConstraints {args} {
  global env
  set ALL false
  if {[lindex $args 0] eq "all"} {
    set ALL true
  } elseif {[llength $args] == 1} {
    set N 1
    set B binary
    set S [lindex $args 0]
  } elseif {[llength $args] == 3} {
    foreach {N B S} $args break
  } else {
    Error "expect 'all' or 3 arguments to SetConstraints: one NUMBER, BIN_LST and SRV_LST item"
  }
  if {$ALL} {
    foreach c {parent child child2 child3} {
      testConstraint $c yes
    }
    foreach c [envGet BIN_LST] {
      testConstraint $c yes
    }
    foreach c {c cc tcl java csharp python ruby perl php go} {
      testConstraint $c [expr {[lsearch -glob [envGet SRV_LST] "$c.*"] != -1}]
    }
    foreach c {pipe uds tcp fork spawn thread server} {
      testConstraint $c [expr {[lsearch -glob [envGet SRV_LST] "*$c*"] != -1}]
    }
  } else {
    # 1. cleanup all constraint
    foreach c {string binary uds tcp pipe c cc tcl java csharp python ruby perl php go fork
		  thread spawn server parent child child2 child3} {
      testConstraint $c no
    }
    # 2. string/binary
    testConstraint $N yes
    if {$N == 1} {
      testConstraint parent yes
    } else {
      testConstraint child yes
      testConstraint child$N yes
    }
    # 3. string/binary
    testConstraint $B yes
    # 4. the server parts
    foreach c [split $S .] {
      testConstraint $c yes
    }
  }
}

##
## -----------------------------------------------------------------------
## init
## 

if {![info exists env(BIN_LST)]} {
  set env(BIN_LST) {string binary}
}
if {![info exists env(COM_LST)]} {
  set env(COM_LST) {uds tcp pipe}
}
if {![info exists env(LNG_LST)]} {
  set env(LNG_LST) {c cc tcl python ruby java csharp perl php go vb}
}
if {![info exists env(START_LST)]} {
  set env(START_LST) {fork thread spawn}
}
if {![info exists env(SRV_LST)]} {
  set env(SRV_LST) {
    c.pipe.pipe
    c.tcp.fork
    c.tcp.thread
    c.tcp.spawn
    c.uds.fork
    c.uds.thread
    c.uds.spawn
    tcl.pipe.pipe
    tcl.tcp.fork
    tcl.tcp.thread
    tcl.tcp.spawn
    tcl.uds.fork
    tcl.uds.thread
    tcl.uds.spawn
    java.pipe.pipe
    java.tcp.thread
    java.uds.thread
    java.tcp.spawn
    java.uds.spawn
    python.pipe.pipe
    python.tcp.spawn
    python.tcp.fork
    python.uds.spawn
    python.uds.fork
    ruby.pipe.pipe
    ruby.tcp.spawn
    ruby.tcp.fork
    ruby.uds.spawn
    ruby.uds.fork
    csharp.pipe.pipe
    csharp.tcp.thread
    csharp.uds.thread
    csharp.tcp.spawn
    csharp.uds.spawn
    cc.pipe.pipe
    cc.tcp.fork
    cc.tcp.thread
    cc.tcp.spawn
    cc.uds.fork
    cc.uds.thread
    cc.uds.spawn
    perl.pipe.pipe
    perl.tcp.spawn
    perl.tcp.fork
    perl.tcp.thread
    perl.uds.spawn
    perl.uds.fork
    perl.uds.thread
    php.pipe.pipe
    php.tcp.spawn
    php.tcp.fork
    php.uds.spawn
    php.uds.fork
    go.pipe.pipe
    vb.pipe.pipe
    vb.tcp.spawn
    vb.tcp.thread
    vb.uds.spawn
    vb.uds.thread
  }
}

# windows has some restrictions :(
if {$::tcl_platform(platform) eq "windows"} {
  set env(SRV_LST) [filterGet -not SRV_LST uds]
  set env(SRV_LST) [filterGet -not SRV_LST fork]
  set env(COM_LST) [filterGet -not COM_LST uds]
}

# without tcl thread support no tcl thread server
if {[info exists ::tcl_platform(threaded)]} {
  set env(SRV_LST)   [filterGet -not SRV_LST tcl fork]
} else {
  set env(SRV_LST)   [filterGet -not SRV_LST tcl thread]
}

# without --enable-java no java
if {!$USE_JAVA} {
  set env(SRV_LST) [filterGet -not SRV_LST java]
  set env(LNG_LST) [filterGet -not LNG_LST java]
}

# without --enable-cxx no c++
if {!$USE_CXX} {
  set env(SRV_LST) [filterGet -not SRV_LST cc]
  set env(LNG_LST) [filterGet -not LNG_LST cc]
}

# without --enable-python no python
if {!$USE_PYTHON} {
  set env(SRV_LST) [filterGet -not SRV_LST python]
  set env(LNG_LST) [filterGet -not LNG_LST python]
}

# without --enable-ruby no ruby
if {!$USE_RUBY} {
  set env(SRV_LST) [filterGet -not SRV_LST ruby]
  set env(LNG_LST) [filterGet -not LNG_LST ruby]
}

# without --enable-perl no perl
if {!$USE_PERL} {
  set env(SRV_LST) [filterGet -not SRV_LST perl]
  set env(LNG_LST) [filterGet -not LNG_LST perl]
}

# without --enable-php no php
if {!$USE_PHP} {
  set env(SRV_LST) [filterGet -not SRV_LST php]
  set env(LNG_LST) [filterGet -not LNG_LST php]
}

# without --enable-csharp no C#
if {!$USE_CSHARP} {
  set env(SRV_LST) [filterGet -not SRV_LST csharp]
  set env(LNG_LST) [filterGet -not LNG_LST csharp]
}

# without --enable-vb no VB.NET
if {!$USE_VB} {
  set env(SRV_LST) [filterGet -not SRV_LST vb]
  set env(LNG_LST) [filterGet -not LNG_LST vb]
}

# set default values
if {![info exists env(USE_REMOTE)]}	  { set env(USE_REMOTE)	      false	}
if {![info exists env(TS_DEBUG)]}	  { set env(TS_DEBUG)	      0		}
if {![info exists env(TS_TIMEOUT)]}	  { set env(TS_TIMEOUT)	      20	}
if {![info exists env(TS_SETUP)]}	  { set env(TS_SETUP)	      false	}
if {![info exists env(TS_HOST)]}	  { set env(TS_HOST)	      localhost	}
if {![info exists env(TS_PORT)]}	  { set env(TS_PORT)	      PORT	}
if {![info exists env(TS_FILE)]}	  { set env(TS_FILE)	      FILE	}
if {![info exists env(TS_PID)]}		  { set env(TS_PID)	      PID	}
if {![info exists env(TS_MAX)]}		  { set env(TS_MAX)	      -1	}
if {![info exists env(TS_STARTUP_AS)]}	  { set env(TS_STARTUP_AS)    NO	}
if {![info exists env(PAR_LST)]}	  { set env(PAR_LST)	      0		}
if {[info exists env(TS_FILTER)]} {
  if {$env(TS_FILTER) ne "NO"} {
    set env(TS_FILTER_SERVER)  $env(TS_FILTER)
    set env(TS_FILTER_CLIENT)  $env(TS_FILTER)
  }
} else {
  set env(TS_FILTER)        NO
}
if {![info exists env(TS_FILTER_SERVER)]} { set env(TS_FILTER_SERVER) NO	}
if {![info exists env(TS_FILTER_CLIENT)]} { set env(TS_FILTER_CLIENT) NO	}
set TS_HELP_MSGQUE false

# try to analyse the "argv" array
set args [list -verbose {start pass body error}]
while {true} {
  set arg [Pop argv]
  switch -exact $arg {
    "--testing"	{
      set argv [list --only-pipe --only-binary --max 5 --only-num 1 {*}$argv]
    }
    "--mem-testing" {
      set argv [list --only-pipe --only-binary --max 5 {*}$argv]
    }
    "--full-testing" {
      set argv [list --only-binary --max 5 {*}$argv]
    }
    "--thread-testing" {
      set argv [list --only-binary --max 5 --only-thread --only-tcp {*}$argv]
    }
    "--fork-testing" {
      set argv [list --only-binary --max 5 --only-fork --only-uds {*}$argv]
    }
    "--spawn-testing" {
      set argv [list --only-binary --max 5 --only-spawn --only-tcp {*}$argv]
    }
    "--remote-testing" {
      set argv [list --only-tcp --port 7777 --only-thread --only-binary --use-remote --max 5 {*}$argv]
    }
    "--use-remote" {
      set env(USE_REMOTE) true
    }
    "--debug" {
      set env(TS_DEBUG)	  [Pop argv]
    }
    "--timeout" {
      set env(TS_TIMEOUT) [Pop argv]
    }
    "--setup" {
      set env(TS_SETUP)	  true
    }
    "--host" {
      set env(TS_HOST)	  [Pop argv]
    }
    "--port" {
      set env(TS_PORT)	  [Pop argv]
    }
    "--file" {
      set env(TS_FILE)	  [Pop argv]
    }
    "--pid" {
      set env(TS_PID)	  [Pop argv]
    }
    "--max" {
      set env(TS_MAX)	  [Pop argv]
    }
    "--filter" {
      set env(TS_FILTER)  [Pop argv]
    }
    "--filter-server" {
      set env(TS_FILTER_SERVER)  [Pop argv]
    }
    "--filter-client" {
      set env(TS_FILTER_CLIENT)  [Pop argv]
    }
    "--only-string" -
    "--only-binary" {
      set env(BIN_LST) [string range $arg 7 end]
    }
    "--server" {
      set env(SRV_LST) [Pop argv]
    }
    "--only-c" -
    "--only-tcl" -
    "--only-java" -
    "--only-csharp" -
    "--only-vb" -
    "--only-python" -
    "--only-ruby" -
    "--only-perl" -
    "--only-php" -
    "--only-go" -
    "--only-cc" {
      set T		[string range $arg 7 end]
      set env(LNG_LST)	$T
      set env(SRV_LST)	[filterGet SRV_LST "\\m$T\\M"]
    }
    "--only-uds" -
    "--only-pipe" -
    "--only-tcp" {
      set T		[string range $arg 7 end]
      set env(COM_LST)	$T
      set env(SRV_LST)	[filterGet SRV_LST $T]
    }
    "--only-fork" -
    "--only-thread" -
    "--only-spawn" {
      set T		  [string range $arg 7 end]
      set env(START_LST)  [filterGet START_LST $T]
      set env(SRV_LST)	  [filterGet SRV_LST $T]
    }
    "--use-fork" -
    "--use-thread" -
    "--use-spawn" {
      set T		      [string range $arg 6 end]
      set env(START_LST)      [filterGet START_LST $T]
      set env(TS_STARTUP_AS)  --$T
    }
    "--only-num"  {
      set env(PAR_LST)	[Pop argv]
    }
    "-h" -
    "--help" {
      puts "USAGE [file tail $argv0]: OPTIONS ...."
      puts ""
      puts " testing ARRAGEMENTS"
      puts "  LANG = c|cc|tcl|java|python|ruby|csharp|perl|php|go|vb"
      puts "  ........................................... programming-language"
      puts "  COM  = pipe|uds|tcp ....................... communication-layer"
      puts "  TYPE = string|binary ...................... package-type"
      puts "  MODE = fork|thread|spawn .................. startup-mode"
      puts ""
      puts " testing OPTIONS"
      puts "  --only-TYPE ...... only use 'package-type' for testing"
      puts "  --only-COM ....... only use 'communication-layer' for testing"
      puts "  --only-LANG ...... only use 'programming-language' for testing"
      puts "  --only-MODE ...... only use 'startup-mode' for server+pipe starup"
      puts "  --use-MODE ....... only use 'startup-mode' for server startup"
      puts ""
      puts "  --only-num # ..... test only with parent/child number #"
      puts "  --use-remote ..... use remote server for tcp/udp connection"
      puts "  --max # .......... set the maximun of allowed processes to #"
      puts "  --debug # ........ set debug level between 0 and 9"
      puts "  --host STR ....... use host as TCP/IP connection host"
      puts "  --port # or STR .. use port as TCP/IP connection port"
      puts "  --file STR ....... use file as Unix Domain Socket connection file"
      puts "  --pid STR ........ use file as --daemon pid file"
      puts "  --server LANG.COM(.MODE) ... only use this server"
      puts "  --setup .......... print additional setup information"
      puts "  --filter exec .... use 'exec' as filter"
      puts "  --help-msgque .... get libmsgque specific help"
      puts "  --help / -h ...... get help"
      puts ""
      puts " compound OPTIONS"
      puts "  --testing  ....... --only-binary --only-pipe --max 5 --only-num 1"
      puts "  --mem-testing .... --only-binary --only-pipe --max 5"
      puts "  --remote-testing . --only-binary/thread/tcp, --port 7777 and --use-remote --max 5"
      puts "  --thread-testing . --only-binary --max 5 --only-thread --only-tcp"
      puts "  --fork-testing ... --only-binary --max 5 --only-fork --only-tcp"
      puts "  --spawn-testing .. --only-binary --max 5 --only-spawn --only-tcp"
      exit 1
    }
    "--help-msgque" {
      set TS_HELP_MSGQUE true
    }
    default {
      lappend args $arg
    }
  }
} ;# no argv

# delete everything from COM_LST which doesn't belongs to SRV_LST
if {![llength [filterGet SRV_LST pipe]]} {
  ## without "pipe" in "SRV_LST" we don't need "pipe" in "COM_LST"
  set env(COM_LST) [filterGet -not COM_LST pipe]
}

package require tcltest
namespace import -force ::tcltest::*

configure {*}$::args

## some basic restrictions
testConstraint local [expr {$env(USE_REMOTE) ? no : yes}]
testConstraint use_remote $env(USE_REMOTE)
testConstraint filter [expr {$env(TS_FILTER) ne "NO"}]

##
## -----------------------------------------------------------------------
## match
##

proc test_variable {variable result} {
    return [expr {[uplevel set $variable] == $result}]
}
customMatch variable test_variable

proc _expr {x y} {
    return [expr {$x == $y}]
}
customMatch expr _expr

##
##--------------------------------------------------------------------------
## Init TestFbg
##

## autoload the package
package require TclMsgque

if {$TS_HELP_MSGQUE} {
  puts [tclmsgque MqS --help-msgque]
  exit 0
}

testConstraint has_thread [tclmsgque support thread]

# does libmsgque support thread
if {![tclmsgque support thread]} {
  set env(SRV_LST) [filterGet -not SRV_LST thread]
  set env(START_LST) [filterGet -not START_LST thread]
}

# does libmsgque support fork
if {![tclmsgque support fork]} {
  set env(SRV_LST) [filterGet -not SRV_LST fork]
  set env(START_LST) [filterGet -not START_LST fork]
}

# check if list has still data
foreach l {SRV_LST COM_LST LNG_LST START_LST} {
  if {![llength $env($l)]} {
    Error "$l is empty"
  }
}

##
## -----------------------------------------------------------------------
## TestFbg
##

proc optB {_argv opt} {
    upvar $_argv argv
    if {[set IDX [lsearch -exact $argv $opt]] != -1} {
        set argv    [lreplace $argv $IDX $IDX]
        return 1
    } else {
        return 0
    }
}

##
## ---------------------------------------------------------------------------------------------------
## Public High
##

proc SendL { ctx list } {
    foreach {cmd val} $list {
      if {$cmd eq "SendL"} {
	$ctx SendL_START
	SendL $ctx $val
	$ctx SendL_END
      } else {
	$ctx $cmd $val
      }
    }
}
  
proc Call { ctx send token args } {
    $ctx SendSTART
    SendL $ctx $args
    $ctx $send $token
}
  
proc Echo0 { ctx token args } {
    Call $ctx SendEND_AND_WAIT $token {*}$args
}
  
proc Echo1 { ctx token read args} {
    Call $ctx SendEND_AND_WAIT $token {*}$args
    return  [$ctx $read]
}
  
proc CallCB { ctx token cb args} {
    $ctx SendSTART
    SendL $ctx $args
    $ctx SendEND_AND_CALLBACK $token $cb
}

proc EchoL { ctx token args } {
    Call $ctx SendEND_AND_WAIT $token {*}$args
    return  [$ctx ReadAll]
}
  
##
## -----------------------------------------------------------------------
## procs
##

proc Start {mode isError id cl {clname ""} {srvname ""}} {
  global env FH FH_LAST Start_PREFIX
  if {$env(TS_SETUP)} { Print id cl }
  if {[info exists FH($id)]} {
    $FH($id) ConfigReset
  } else {
    set FH($id) [tclmsgque MqS]
  }
  $FH($id) ConfigSetDebug $env(TS_DEBUG)
  $FH($id) ConfigSetTimeout $env(TS_TIMEOUT)
  $FH($id) ConfigSetIsString [expr {$mode eq "string"}]
  if {$clname ne ""} {
    $FH($id) ConfigSetName $clname
  }
  if {$srvname ne ""} {
    $FH($id) ConfigSetSrvName $srvname
  }
  set FH_LAST $FH($id)
  if {[info exists Start_PREFIX]} {
    eval $Start_PREFIX
  }
  if {$isError} {
    if {[catch {$FH_LAST {*}$cl} ERROR]} { error $ERROR }
  } else {
    set NUM 10
    while {[catch {$FH_LAST {*}$cl} ERROR]} {
      puts "RETRY: $cl"
      incr NUM
      if {$NUM > 10} { Error $ERROR }
    }
  }
  if {$env(TS_SETUP)} { puts "Start <-" }
}

proc Setup {num mode com server args} {
  global env PIDFILE FH FH_LAST PORT FILE SERVER_OUTPUT
  if {[llength $num] == 1} {
    set numParent   1
    set numChild    $num
  } else {
    foreach {numParent numChild} $num break
  }
  if {$env(TS_SETUP)} {
    Print num mode com server args
  }
  set serverSilent  [optB args --server-silent]
  set isError	    [optB args --error]
  set setup	    [optV args --setup]
  set setup_parent  [optV args --setup-parent]
  set bgerror	    [optV args --bgerror]
  set filter	    [optV args --filter NO]
  if {$filter ne "NO"} {
    set filter_server $filter
    set filter_client $filter
  } else {
    set filter_server [optV args --filter-server $env(TS_FILTER_SERVER)]
    set filter_client [optV args --filter-client $env(TS_FILTER_CLIENT)]
  }
  unset -nocomplain SERVER_OUTPUT

  ## 1. setup variables
  lappend comargs --$com
  # replase all FILE, PORT and PID placeholder
  set args [MkUnique $args]

  # check for PIDFILE
  set PIDFILE [optV args --daemon]
  if {$PIDFILE ne ""} {
    set DAEMON [list --daemon $PIDFILE]
  } else {
    set DAEMON [list]
    unset PIDFILE
  }

  # init client-parent (cargs), client-child (cargs) and server (sargs) arguments
  set cargs	[list]
  set sargs	[list]

  ## 2. for NON-PIPE server start the server as --fork/--thread/--spawn once
  if {$com ne "pipe"} {
    ## ...
    switch -exact -- $com {
      tcp	{
	set PORT    [envGet TS_PORT]
	if {$PORT eq "PORT"} {set PORT [FindFreePort]}
	lappend comargs --port [optV args --port $PORT]

	# set host for client and server
	lappend comargs --host [optV args --host [envGet TS_HOST]]
      }
      uds	{
	set FILE    [envGet TS_FILE]
	if {$FILE eq "FILE"} {set FILE [FindFreeFile]}
	lappend comargs --file [optV args --file $FILE]
      }
    }
    lappend sargs {*}$args
    optVD sargs --buffersize --timeout --myhost --myport
    set sargs [MkUnique $sargs]
    if {$serverSilent} { lappend sargs --silent }
    if {!$env(USE_REMOTE)} {
      if {$filter_server ne "NO"} {
	set sl [list {*}[getFilter $filter_server.$server] {*}$DAEMON]
	lappend sl --name fs {*}$comargs @ {*}[getServerOnly $server] {*}$sargs
      } else {
	set sl [list {*}[getServer $server] {*}$DAEMON {*}$sargs {*}$comargs]
      }
      if {$env(TS_SETUP)} {
	puts "Setup: start server"
      }
      Bg {*}$sl
      after $::WAIT
    }
  }
  
  ## 3. create new tclmsgque object
  set FH_LAST	""

  lappend cargs {*}$args
  for {set PNO 0} {$PNO<$numParent} {incr PNO} {

    ## prepare parent arguments
    if {$filter_client ne "NO"} {
      set cl [list LinkCreate {*}$cargs @ {*}[getFilter $filter_client.$server] --name fc @ {*}$comargs]
    } else {
      set cl [list LinkCreate {*}$cargs {*}$comargs]
    }
    if {$com eq "pipe"} { 
      if {$filter_server ne "NO"} {
	lappend cl @ {*}[getFilter $filter_server.$server] --name fs {*}$comargs
	if {$serverSilent} { lappend cl --silent }
	lappend cl @ {*}[getServerOnly $server] {*}$sargs
      } else {
	lappend cl @ {*}[getServer $server] 
	if {$serverSilent} { lappend cl --silent }
      }
    }
    Start $mode $isError $PNO-0 $cl client-$PNO server-$PNO
    if {$bgerror ne ""} {
      $FH_LAST ConfigSetBgError $bgerror
    }
    if {$setup ne ""} {
      uplevel $setup
    }
    if {$setup_parent ne ""} {
      uplevel $setup_parent
    }

    ## start the childs
    for {set CNO 1} {$CNO<$numChild} {incr CNO} {
      ## prepare child arguments
      set cl [list LinkCreateChild $FH_LAST]
      ## start the child 
      Start $mode yes $PNO-$CNO $cl 
      if {$bgerror ne ""} {
	$FH_LAST ConfigSetBgError $bgerror
      }
      if {$setup ne ""} {
	uplevel $setup
      }
    }
  }
}

proc Cleanup {args} {
#Print args
  global CLEANUP_PID CLEANUP_FILES PIDFILE FH FH_LAST env SERVER_OUTPUT

  array set OPT {-wait 0}
  array set OPT $args

## 2. close all tclmsgque objects
  foreach num [lsort [array names FH {*-0}]] {
    if {$env(TS_SETUP)} { Print num }
    $FH($num) LinkDelete
  }
  foreach num [lsort [array names FH]] {
    $FH($num) ConfigReset
  }
  unset -nocomplain FH_LAST
## 3. if a separate server was startet (NON-PIPE communication) kill this server
  if {!$::env(USE_REMOTE)} {
    if {[info exists PIDFILE]} {
      if {[file exists $PIDFILE]} {
	set FP [open $PIDFILE r]
	lappend CLEANUP_PID [read $FP]
	close $FP
      }
      unset PIDFILE
    }
    if {[info exists CLEANUP_PID]} {
      after $OPT(-wait)
      foreach PID $CLEANUP_PID {
	Kill $PID
      }
      unset CLEANUP_PID
    }
    if {[info exists CLEANUP_FILES] && [llength $CLEANUP_FILES]} {
      file delete -force {*}$CLEANUP_FILES
    }
  }
}

proc freeTests {} {
  Cleanup
  uplevel cleanupTests
}

proc Example {config client server args} {
  global env CLEANUP_PID PIDFILE FH FH_LAST
  unset -nocomplain CLEANUP_PID PIDFILE FH FH_LAST
  foreach {lng com start} [split $config .] break
  if {$env(TS_SETUP)} {
    Print lng com start client server
  }
  set filter [optV args --filter NO]

  ## 1. setup variables
  lappend comargs --$com
  # replase all FILE, PORT and PID placeholder
  set args [MkUnique $args]

  # init client-parent (cargs), client-child (cargs) and server (sargs) arguments
  set cargs	[list]
  set sargs	[list]

  ## 2. for NON-PIPE server start the server as --fork/--thread/--spawn once
  if {$com ne "pipe"} {
    ## ...
    switch -exact -- $com {
      tcp	{
	set PORT    [envGet TS_PORT]
	if {$PORT eq "PORT"} {set PORT [FindFreePort]}
	lappend comargs --port [optV args --port $PORT]

	# set host for client and server
	lappend comargs --host [optV args --host [envGet TS_HOST]]
      }
      uds	{
	set FILE    [envGet TS_FILE]
	if {$FILE eq "FILE"} {set FILE [FindFreeFile]}
	lappend comargs --file [optV args --file $FILE]
      }
    }
    lappend sargs {*}$args
    optVD sargs --buffersize --timeout --myhost --myport
    set sargs [MkUnique $sargs]
    if {!$env(USE_REMOTE)} {
      if {$filter ne "NO"} {
	set sl [list {*}[getExample $filter.$lng] --$start --name fs {*}$comargs @ {*}[getExample $server.$lng] {*}$sargs]
      } else {
	set sl [list {*}[getExample $server.$lng] --$start {*}$sargs {*}$comargs]
      }
      if {$env(TS_SETUP)} {
	Print sl
      }
      Bg {*}$sl
      after $::WAIT
    }
  }
  
  ## 3. start client
  set cl [list {*}[getExample $client.$lng] {*}$cargs {*}$comargs]
  if {$com eq "pipe"} { 
    if {$filter ne "NO"} {
      lappend cl @ {*}[getExample $filter.$lng] --name fs @ {*}[getExample $server.$lng]
    } else {
      lappend cl @ {*}[getExample $server.$lng]
    }
  }
  if {$env(TS_SETUP)} { Print cl }
  set RET [Exec {*}$cl]

  ## 4. cleanup
  Cleanup

  return $RET
}

proc Exec {args} {
  if {$::env(TS_SETUP)} {
    Print args
  }
  #if {[catch {exec {*}$args >&@stdout} ERR]} {}
  if {[catch {exec {*}$args} ERR]} {
    return [lindex $::errorCode 0]-[lindex $::errorCode 2]-$ERR
  } else {
    return "$ERR"
  }
}

proc MakeFile {init name} {
  set timeout [expr {[clock seconds] + $::env(TS_TIMEOUT)}]
  set RET ""
  while {[clock seconds] < $timeout} {
    if {[catch {makeFile $init $name} RET]} {
      puts "MakeFile: $RET, try again"
      after 500
    } else {
      break
    }
  }
  return $RET
}

proc Bg {args} {
  if {$::env(TS_SETUP)} { Print args }
  set PID [exec {*}$args >&@stdout &]
  if {$::env(TS_SETUP)} { Print PID }
  lappend ::CLEANUP_PID $PID
}

proc BgAct {ch cmd} {
  if {[eof $ch]} {
    close $ch
  } else {
    $cmd $ch
  }
}

proc BgProc {p args} {
  if {$::env(TS_SETUP)} {
    Print args
  }
  set OUTPUT [open [list | {*}$args 2>@stderr] r]
  fconfigure $OUTPUT -buffering line -blocking yes
  fileevent $OUTPUT readable [list BgAct $OUTPUT $p]
  return $OUTPUT
}

proc WaitMSec {msec} {
  global done
  set done 1
  after $msec {set ::done 0}
  while {$done} {
    update
  }
}

proc WaitEOF {fh} {
  catch {
    while {![eof $fh]} {
      update
    }
  }
}

proc WaitOnFileToken {file token} {
  set end [expr {[clock seconds] + $::env(TS_TIMEOUT)}]
  while {true} {
    set RET [list]
    set fh [open $file r]
    while {![eof $fh]} {
      gets $fh line
      lappend RET $line
      if {[string first $token $line] != -1} {
	return [join $RET \n]
      } 
      if {[clock seconds] >= $end} {
	lappend RET "timeout ($::env(TS_TIMEOUT) sec)"
	return [join $RET \n]
      }
    }
    close $fh
  }
}

proc Kill {P} {
  catch {exec $::KILL -9 $P}
}

proc Create {I CON} {
  global env
  for {set i 0} {$i < $I} {incr i} {
    set FH_CUR [tclmsgque MqS]
    if {$i == 0} {
      set FH_FIRST $FH_CUR
      $FH_FIRST ConfigSetName     "client"
      $FH_FIRST ConfigSetTimeout  $env(TS_TIMEOUT)
      $FH_FIRST ConfigSetDebug    $env(TS_DEBUG)
      $FH_FIRST LinkCreate	  {*}$CON
    } else {
      $FH_CUR LinkCreateChild $FH_LAST
    }
    set FH_LAST $FH_CUR
  }
  return [list $FH_FIRST $FH_LAST]
}

proc ExecLines {start end args} {
    catch {uplevel $args} MSG
    return [join [lrange [split $MSG \n] $start $end ] \n]
}

proc RET_start {} {
  set ::RET [list START]
}

proc RET_end {} {
  global RET
  lappend RET END
  return [join $RET \n]
}

proc RET_add {id args} {
  global RET errorCode
  set errorCode NONE
  lappend RET ${id}-CATCH:[catch {{*}$args} ERR]
  lappend RET ${id}-RET:$ERR
  lappend RET ${id}-CODE:$errorCode
}

proc RET_BG {ctx} {
  RET_add BG-NUM $ctx ErrorGetNum
  RET_add BG-TEXT $ctx ErrorGetText
  $ctx ErrorReset
}


