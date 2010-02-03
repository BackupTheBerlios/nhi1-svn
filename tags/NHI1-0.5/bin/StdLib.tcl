#+
#§  \file       bin/StdLib.tcl
#§  \brief      \$Id$
#§  
#§  (C) 2004 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

package provide StdLib 2.0

namespace eval StdLib {
    variable    STDIN	stdin	    ;# the stdin channel
    variable    STDOUT	stdout	    ;# the stdout channel
    variable    STDERR	stderr	    ;# the stderr channel
    variable    ERROR	EXIT	    ;# valid EXIT, PUTS, ERROR, IGNORE

    variable    LOCK		    ;# Array of "fileevent" lock's -> Filter
}

##--
proc ::StdLib::StdErr { Str } {
    puts stderr $Str
}

##--

proc ::StdLib::Error { Str } {
    if { $Str != {child process exited abnormally} } {
        StdErr "ERROR: $Str"
    }
    exit 1
}

proc ::StdLib::ErrMsg { Pre Args } {
    set RET ""
    if {$Args eq %NO%} { 
        set Args    $Pre 
        set Pre	    ERROR
    }
    append RET "\n$Pre: [ lindex $Args 0 ]" \n
    foreach a [ lrange $Args 1 end ] { 
        append RET "    $a" \n
    }
    append RET \n

    return $RET

}

proc ::StdLib::ErrorN { Pre { Args %NO% } } {
    StdErr [ ErrMsg $Pre $Args ]
    exit 1
}

proc ::StdLib::ErrorS { Pre { Args %NO% } } {
    _ErrHdl [ ErrMsg $Pre $Args ]
}

proc ::StdLib::ErrorM  { Str } { _ErrHdl $Str }
proc ::StdLib::Warning { Str } { StdErr "WARNING: $Str" }

##--

proc ::StdLib::NULL {} {
    variable    NULL
    if {![ info exists NULL ]} { set NULL [ open /dev/null w ] }
    return $NULL
}

##--

proc ::StdLib::_Push_STDIN { VAL } {
    variable    STDIN
    upvar 2        __STDIN SV
    lappend SV    $STDIN
    set STDIN    $VAL
}

proc ::StdLib::_Push_STDOUT { VAL } {
    variable    STDOUT
    upvar 2        __STDOUT SV
    lappend SV    $STDOUT
    set STDOUT    $VAL
}

proc ::StdLib::_Push_STDERR { VAL } {
    variable    STDERR
    upvar 2        __STDERR SV
    lappend SV    $STDERR
    set STDERR    $VAL
}

proc ::StdLib::_Push_ERROR { VAL } {
    variable    ERROR
    upvar 2        __ERROR SV
    lappend SV    $ERROR
    set ERROR    $VAL
}

proc ::StdLib::__Push_CHANNEL { C1 C2 C3 } {
    variable    STDIN
    variable    STDOUT    
    variable    STDERR
    upvar 3 __CHANNEL _CHANNEL
    lappend _CHANNEL $STDIN $STDOUT $STDERR
    set STDIN    $C1
    set    STDOUT    $C2
    set    STDERR    $C3
}

proc ::StdLib::_Push_CHANNEL_DEFAULT {} {
    __Push_CHANNEL stdin stdout stderr
}

proc ::StdLib::_Push_CHANNEL_NULL {} {
    __Push_CHANNEL stdin [ NULL ] [ NULL ]
}

proc ::StdLib::Push { args } {
    foreach { TYP VAL } $args {
        switch -exact $TYP {
            CHANNEL    {
                switch -exact $VAL {
                    DEFAULT    { _Push_CHANNEL_DEFAULT }
                    NULL    { _Push_CHANNEL_NULL }
                    default    { Error "::StdLib::Push unknown $TYP-VAL \"$VAL\"" }
                }
            }
            STDIN    { _Push_STDIN  $VAL }
            STDOUT    { _Push_STDOUT $VAL }
            STDERR    { _Push_STDERR $VAL }
            ERROR    { _Push_ERROR  $VAL }
            default { Error "::StdLib::Push unknown TYP \"$TYP\"" }
        }
    }
}

proc ::StdLib::_Pop_STDIN {} {
    variable    STDIN
    upvar 2        __STDIN    SV
    set STDIN    [ lindex $SV end ]
    set SV        [ lrange $SV 0 end-1 ]
}

proc ::StdLib::_Pop_STDOUT {} {
    variable    STDOUT
    upvar 2        __STDOUT    SV
    set STDOUT    [ lindex $SV end ]
    set SV        [ lrange $SV 0 end-1 ]
}

proc ::StdLib::_Pop_STDERR {} {
    variable    STDERR
    upvar 2        __STDERR    SV
    set STDERR    [ lindex $SV end ]
    set SV        [ lrange $SV 0 end-1 ]
}

proc ::StdLib::_Pop_ERROR {} {
    variable    ERROR
    upvar 2        __ERROR    SV
    set ERROR    [ lindex $SV end ]
    set SV        [ lrange $SV 0 end-1 ]
}

proc ::StdLib::_Pop_CHANNEL {} {
    variable    STDIN
    variable    STDOUT    
    variable    STDERR
    upvar 2 __CHANNEL _CHANNEL
    foreach {STDIN STDOUT STDERR} [lrange $_CHANNEL end-2 end] break
    set _CHANNEL [lrange $_CHANNEL 0 end-3]
}

proc ::StdLib::Pop { args } {
    foreach TYP $args {
        switch -exact $TYP {
            CHANNEL    { _Pop_CHANNEL }
            STDIN    { _Pop_STDIN   }
            STDOUT    { _Pop_STDOUT  }
            STDERR    { _Pop_STDERR  }
            ERROR    { _Pop_ERROR   }
            default { Error "::StdLib::Push unknown TYP \"$TYP\"" }
        }
    }
}

proc ::StdLib::Freeze {} {
    variable    STDIN
    variable    STDOUT
    variable    STDERR
    variable    ERROR
    upvar    __FREEZE    FREEZE
    set FREEZE [ list $STDIN $STDOUT $STDERR $ERROR ]
}

proc ::StdLib::Release {} {
    variable    STDIN
    variable    STDOUT
    variable    STDERR
    variable    ERROR
    upvar    __FREEZE    FREEZE
    foreach {STDIN STDOUT STDERR ERROR} $FREEZE break
}

##--

proc ::StdLib::_ErrHdl { ERR } {
    variable    ERROR
    switch -exact $ERROR {
        EXIT    {Error $ERR}
        PUTS    {StdErr "$ERR"}
        ERROR    {error $ERR}
        IGNORE    {}
        default    {Error "unknown error type \"$ERROR\"\ncan't handle \"$ERR\""}
    }
}

##
## ---------------------------------------------------
##

proc ::StdLib::Exec { CMD } {
    variable    STDIN
    variable    STDOUT
    variable    STDERR

    if {[catch {eval exec $CMD <@$STDIN >@$STDOUT 2>@$STDERR} ERR]} { 
        _ErrHdl $ERR 
    }
}

proc ::StdLib::Open { CMD } {
    package require System
    if {[catch { open [ concat | $CMD ] r } FH]} { _ErrHdl $FH }
    fconfigure $FH -buffering line
    return $FH
}

proc ::StdLib::Close { FH } {
    if {[catch { close $FH } ERR]} { _ErrHdl $ERR }
}

###################################################################

##
## ---------------------------------------------------
##

proc Print { args } {
    if {[ info level ] > 1} {
        set RET "[ lindex [ info level -1 ] 0 ] ==> "
    } else {
        set RET "0 ==> "
    }
    foreach arg $args {
        set T [ uplevel set $arg ]
        if {[catch { llength $T } L ]} { set L none }
        lappend RET "$arg<${T}($L)>"
    }
    puts $RET
}

proc PrintList {list} {
    upvar $list List
    puts "$list -->"
    set I 0
    foreach L $List {
	puts [format {  %2i: %s} $I $L]
	incr I
    }
    puts "$list <--"
}

#proc SEval { Cmd } {
#    if {[ catch $Cmd ERR ]} { puts $ERR }
#}

proc SCUM { { Max 999 } } {
    set i 0
    for { set I [ expr { [ info level ] - 1 } ]} { $I > 0 } { incr I -1 } {
        puts "($I) : [ info level $I ]"
        incr i
        if { $i > $Max } break
    }
}

proc CUM { { NUM %NO% } } {
    if {![ string equal $NUM {%NO%} ]} {
        upvar __CUM __CUM
        if {![ info exists __CUM ]} { set __CUM 0 }
        if { $__CUM == $NUM } { exit 0 }
        incr __CUM
    }
    puts "Call: [ info level -1 ]"
}

proc DelDir { Dir } {
    if {[ catch { file delete -force $Dir } ERR ]} {
        set NS    [ uplevel namespace current ]
        ::StdLib::ErrorN     $NS  [ list $ERR ]
    }
}

proc DelIfExist { File } {
    if {[ file exists $File ]} {
        if {[ file isdirectory $File ]} {
            DelDir $File
        } else {
            file delete -force -- $File
        }
    }
}

proc FilterList { List Pat } {
    set Idx     0
    set List2   {}
    while {[ set Idx [ lsearch -glob $List $Pat ]] != -1} {
        lappend List2   [ lindex $List $Idx ]
        set List        [ lreplace $List 0 $Idx ]
    }
    return $List2
}

##
## ---------------------------------------------------
## File Functions
##

proc ReadFile { F } {
    set FH    [ open $F r ]
    set RET    [ read $FH [ file size $F ] ]
    close $FH
    return $RET
}

proc WriteFile { F S {mode w}} {
    set FH    [ open $F $mode ]
    puts -nonewline $FH $S
    close $FH
}

proc Patch { F L } {
    WriteFile $F [ string map $L [ ReadFile $F ] ]
}

proc MkDir { Dir } {
    if {[ catch { file mkdir $Dir } ERR ]} {
        set NS    [ uplevel namespace current ]
        ::StdLib::ErrorN     $NS  [ list $ERR ]
    }
}

proc Touch { File } {
    if {[ file exists $File ]} {
        file mtime $File [ clock seconds ]
    } else {
        close [ open $File w ]
    }
}

proc GetNoLinkFile { File } {
    if {![ file exists $File ]} { return $File }
    while {[string equal [file type $File] {link}]} {
        set lnk [file readlink $File]
        if {[string equal [ file pathtype $lnk ] {relative}]} {
            set File [ file join [ file dirname $File ] $lnk ]
        } else {
            set File $lnk
        }
    }
    return $File
}

proc CopyNoLink { SFile TFile } {
    set SFile     [ GetNoLinkFile $SFile ]
    set TFile    [ GetNoLinkFile $TFile ]

    if {[ file isdirectory $TFile ]} {
        set TFile [ file join $TFile [ file tail $SFile ] ]
    }

  ## HR:010730  notwendig wegen BWidget -> BWinit
    file mkdir [ file dirname $TFile ]
    file copy -force $SFile $TFile

    Touch $TFile
}

proc DeepCopy { SFile TFile } {
    set DIR    [ file dirname $TFile ]
    if {![ file exists $DIR ]} {
        file mkdir $DIR
    }
    if {![ file isdirectory $DIR ]} {
        error "file \"$DIR\" is no directory -> can't \"DeepCopy\""
    }
  ## -force need by option PkgLib "Force" to overwrite existsing files
    file copy -force $SFile $TFile
}

proc IsOlder { SFile TFile } {
    if {[ file readable $TFile ]} {
        if {[catch {file stat $SFile _SFile} ]} { return 0 }
        if {[catch {file stat $TFile _TFile} ]} { return 0 }
        return [ expr { $_SFile(mtime) < $_TFile(mtime) } ]
    }
    return 0
}

proc CopyIfNewer { SFile PDIR } {
    if {![ file exists $SFile ]} {
        ::StdLib::ErrorN "file \"$SFile\" does not exist"
    }
    set TFile [ file join $PDIR [ file tail $SFile ] ]
    if {[ IsOlder $SFile $TFile ]} return
    CopyNoLink $SFile $TFile
}

##
## ---------------------------------------------------
## "Dir" Functions
##

namespace eval ::Dir {

    proc Push { Dir } {
        upvar __DIR  DIR
        lappend DIR [pwd]
        cd $Dir
    }

    proc Pop {} {
        upvar __DIR  DIR
        cd [ lindex $DIR end ]
        set DIR [ lreplace $DIR end end ]
    }
}

##
## ---------------------------------------------------
## "Test" Functions
##

namespace eval ::Is {

    proc Error { Str } {
        ::StdLib::StdErr "IS ERROR: $Str -> please check \"Compiler\" setup"
        exit 1
    }

    proc Executable { File } {

        if {![ file exists $File ]} {
            Error "file \"$File\" does not exist"
        }
        if {![ file executable $File ]} {
            Error "can't exec file \"$File\""
        }
    }

    proc Readable { File } { 
        if {![ file readable $File ]} { 
            Error "file \"$File\" is not readable"
        }
    }

    proc Directory { File } { 
        if {![ file isdirectory $File ]} { 
            Error "file \"$File\" is not a directory"
        }
    }
}

namespace eval ::Question {

    proc YesNo { Str } {
        while 1 {
            puts -nonewline stdout "\n$Str (YES|no): "
            gets stdin ANS
            if {![ string length $ANS ]} { set ANS y }
            switch -glob -- $ANS {
                [Yy]            -
                [Yy][Ee]        -
                [Yy][Ee][Ss]    { return yes }
                [Nn]            -
                [Nn][Oo]        { return no }
            }
            puts "please answer \"yes\" or \"no\" :"
        }
    }
}

##=================================================================================
proc LVarPop { list { Idx 0 } } {
##- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
## delete element of list
##- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    upvar $list List
    set Elm [ lindex $List $Idx ]
    set List [ lreplace $List $Idx $Idx ]
    return $Elm
}

##=================================================================================
proc LUnique { List } {
##- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
## make sorted list unique
##- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    set RET    {}
    set OLD    {}
    foreach L $List {
        if { $L == $OLD } continue
        lappend RET [ set OLD $L ]
    }
    return $RET
}

##=================================================================================
proc PkgProvide { PKG } {
##- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
## ...
##- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    set P1    [ package provide $PKG ]
    if {[ string length $P1 ]} return
    set P2    [ lindex [ lsort [ package versions $PKG ] ] end ]
    if {![ string length $P2 ]} {
        ::StdLib::ErrorN {PkgProvide} [ list \
            "can't figure out the release number of package \"$PKG\"" \
            "please check your installation" \
        ]
    }
    package provide $PKG $P2
}

##=================================================================================
proc EnvGet { VAR default } {
##- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
## return a environment variable or a default vaulue
##- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    global env
    if {[info exists env($VAR)]} {
	return $env($VAR)
    }

    return $default
}
