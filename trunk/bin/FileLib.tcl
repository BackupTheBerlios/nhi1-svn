#+
#:  \file       bin/FileLib.tcl
#:  \brief      \$Id$
#:  
#:  (C) 2004 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

package require StdLib

PkgProvide FileLib

namespace eval FileLib {

    variable    SKIP        {}        ; ## skip dir's and file's in SKIP-pattern-list
}


proc ::FileLib::Push-SKIP { Val } {
    variable    SKIP
    upvar __SKIP Stack
    lappend Stack $SKIP
    set SKIP $Val
}

proc ::FileLib::Pop-SKIP {} {
    variable    SKIP
    upvar __SKIP Stack
    set SKIP  [ lindex $Stack end ]
    set Stack [ lrange $Stack 0 end-1 ]
}

##
## -------------------------------------------------------------
##

proc ::FileLib::_GetOptionValue { list Tok Val } {
    upvar $list List
    set IDX1    [ lsearch -exact $List $Tok ]
    if { $IDX1 != -1 } {
        set IDX2    $IDX1 ; incr IDX2
        set Types   [ lindex $List $IDX2 ]
        set List    [ lreplace $List $IDX1 $IDX2 ]
    } else {
        set Types    $Val
    }
    return $Types
}

##
## -------------------------------------------------------------
##

proc ::FileLib::LMatch {List File} {
    foreach L $List {
        if {[ string match $L $File ]} { 
            return 1 
        }
    }
    return 0
}

proc ::FileLib::_IfSkip { F } {
    variable SKIP
    return [LMatch $SKIP $F]
}

proc ::FileLib::_IfIgnore  {Ignore IgnAry F} {
    variable IGNORE
    set D   [file dirname $F]
    set	T   [file tail $F]
    if {![info exists IGNORE($D)]} {
        set L	[list .svn CVS . ..]

      ## read the ignore-array
	if {[llength $IgnAry] == 2 } {
	    foreach {NAME BASE} $IgnAry break
	    upvar ::$NAME LABELIGNORE
	    if {[info exists LABELIGNORE]} {
		set LBDIR   [string map [list $BASE .] [file normalize $D]]
		if {[info exists LABELIGNORE(*)]} {
		    set L [concat $L $LABELIGNORE(*)]
		}
		if {[info exists LABELIGNORE($LBDIR)]} {
		    set L [concat $L $LABELIGNORE($LBDIR)]
		}
	    }
	}

      ## read the ignore-file
	set I   [file join $D $Ignore]
	if {[file exists $I]} {
	    set FH [open $I r]
	    while {1} {
		set Line [gets $FH]
		if {[eof $FH]} break
		if {$Line eq "" || [regexp {^\s*#} $Line]} continue
		set L [concat $L [split $Line]]
	    }
	    close $FH
	}
        set IGNORE($D)    $L
    }
    return [LMatch $IGNORE($D) $T]
}

proc ::FileLib::_SubDirs {Dir Ignore IgnAry Level} {
    if {[incr Level -1] < -1} return
    set RET {}
    foreach D [ glob -nocomplain -types d -directory $Dir * .* ] {
        if {[_IfSkip $D] || [_IfIgnore $Ignore $IgnAry $D]} continue
        lappend RET $D
    }
    return $RET
}

proc ::FileLib::_SubTree {Args Deep Dir Ignore IgnAry Level} {
    upvar TTREE TTREE
    foreach D [_SubDirs $Dir $Ignore $IgnAry $Level] {
        _GetTree $Args $Deep $D $Ignore $IgnAry $Level
    }
}

proc ::FileLib::_GetTree {Args Deep Dir Ignore IgnAry Level} {  
    if {[incr Level -1] < -1} return
    upvar TTREE TTREE
    if {$Deep && $Level > -1}  {
	_SubTree $Args $Deep $Dir $Ignore $IgnAry $Level
    } 
    if {[ catch {eval glob [concat -directory [list $Dir] $Args]} FILES ]} {
        ::StdLib::_ErrHdl $FILES
        return
    }
    foreach F $FILES {
        if {[_IfSkip $F] || [_IfIgnore $Ignore $IgnAry $F]} continue
        lappend TTREE $F
    }
    if {!$Deep && $Level > -1}  {
	_SubTree $Args $Deep $Dir $Ignore $IgnAry $Level
    } 
}

##
## example:
## ::FileLib::GetTree -nocomplain -types f -directory DIR pkgIndex.tcl
## options (all "glob" options and ...) :
##   Name:           Value:       Default:        Explain:
##   --------------  -----------  --------------  ---------------------------
##   -deep           0,1          0               deep first
##   -ignorefile     FileName     .treeignore     use ignorefile with FileName
##   -ignorearray    {name base}  nothing         use array for ignoring
##   -level          signed       9999            no level
##
proc ::FileLib::GetTree {args} {
    if {[llength $args] > 1} {
	set Args $args
    } else {
	set Args [lindex $args 0]
    }
    set TTREE	{}
    set	Deep	[_GetOptionValue Args -deep	    0		]
    set Dirs	[_GetOptionValue Args -directory    .		]
    set	Ignore	[_GetOptionValue Args -ignorefile   .treeignore	]
    set	IgnAry	[_GetOptionValue Args -ignorearray  ""		]
    set	Level	[_GetOptionValue Args -level        9999	]
    _GetTree $Args $Deep $Dirs $Ignore $IgnAry $Level
    return $TTREE
}

proc ::FileLib::GetSubDirs {Dir {Ignore .treeignore} {IgnAry ""} {Level 9999}} {
    return [_SubDirs $Dir $Ignore $IgnAry $Level]
}

proc ::FileLib::IsDeadLink { File } {
    if {[ catch {
        while {[string equal [file type $File] {link}]} {
            set lnk [file readlink $File]
            if {[string equal [ file pathtype $lnk ] {relative}]} { 
                set lnk [ AbsFile [ file join [ file dirname $File ] $lnk ] ] 
            }
            set File $lnk
        }
    } ]} { 
        return 1 
    }
    return 0
}

proc ::FileLib::CopyDirNoLink { SDir TDir } {
    if {![ file isdirectory $TDir ]} { file mkdir $TDir }

    set Len        [ expr { [ string length $SDir ] - 1 } ]
    foreach D [ GetTree [ list -directory $SDir -nocomplain -types d .* * ] ] {
        file mkdir [ string replace $D 0 $Len $TDir ]
    }

    foreach F [ GetTree [ list -directory $SDir -nocomplain -types f .* * ] ] {
        file copy -force $F [ string replace $F 0 $Len $TDir ]
    }

## copy links
    set LINKS    [ GetTree [ list -directory $SDir -nocomplain -types l .* * ] ]
    while {[ set LEN [ llength $LINKS ]]} {
        set NLINKS    {}
        foreach L $LINKS {
            if {[ IsDeadLink $L ]} { lappend NLINKS $L ; continue }
            CopyFileNoLink $L [ string replace $L 0 $Len $TDir ]
        }
        if {$LEN == [ llength $NLINKS ]} {
            ::StdLib::ErrorM "dead links \"$LINKS\""
        }
        set LINKS     $NLINKS
        if {[ llength $LINKS ]} continue
        break
    }
}

proc ::FileLib::GetNoLinkFile { File { exist OK } } {
    if {![ file exists $File ]} { 
	if {$exist eq "OK"} { return $File }
	::StdLib::Error "GetNoLinkFile ERROR: file \"$File\" does not exist"
    }
    return [GetAbsNoLinkFile File]
}

proc ::FileLib::GetAbsNoLinkFile { File } {
    set File [file normalize $File]
    while {[file type $File] eq {link}} {
	set File [file normalize [file readlink $File]]
    }
    return $File
}

proc ::FileLib::CopyFileNoLink { SFile TFile } {
    set SFile    [ GetNoLinkFile $SFile ERROR ]
    set TFile    [ GetNoLinkFile $TFile ]

    if {[ file isdirectory $TFile ]} {
        set TFile [ file join $TFile [ file tail $SFile ] ]
    }

    set Dir	[ file dirname $TFile ]
    if {![file isdirectory $Dir ]} { file mkdir $Dir }

    file copy -force $SFile $TFile

    if {[ file type $TFile ] == "file"} {
        file mtime $TFile [ clock seconds ]
    }
}

proc ::FileLib::AbsFile { File { Dir §PWD§ } } {
    while {[ file tail $File ] == "."} {
        if { $File == "." } {
            set File [pwd]
	    break
        } else {
            set File [ file dirname $File ]
        }
    }
    if {[string equal [ file pathtype $File ] {relative}]} {
        set RET    [ file split [ expr {$Dir == {§PWD§} ? [pwd] : $Dir} ] ]
        foreach F [ file split $File ] {
            switch -exact -- $F {
                "."         {
                    continue
                }
                ".."     {
                    set RET [ lrange $RET 0 end-1 ]
                }
                default    {
                    lappend RET $F
                }
            }
        }
        return [ eval file join $RET ]
    }
    return $File
}

proc ::FileLib::LnDir { SDir TDir } { 

    error "not ready now"

    set Len        [ expr { [ string length $SDir ] - 1 } ]

    foreach D [ GetTree [ list -directory $SDir -nocomplain -types d * ] ] {
        file mkdir [ string replace $D 0 $Len $TDir ]
    }

    foreach L [ GetTree [ list -directory $SDir -nocomplain -types l * ] ] {
        if {[ file isdirectory $L ]} continue
        CopyFileNoLink $L [ string replace $L 0 $Len $TDir ]
    }

    foreach F [ GetTree [ list -directory $SDir -nocomplain -types f * ] ] {
        if {[ file isdirectory $F ]} continue
        file copy $F [ string replace $F 0 $Len $TDir ]
    }
}

proc ::FileLib::Readable { FILE } {
    if {![ file readable $FILE ]} {
        ::StdLib::ErrorM "file \"$FILE\" is not readable"
    }
}

proc ::FileLib::Open { FILE Typ } {
    if {[ catch { open $FILE $Typ } FH ]} {
        set END {}
        switch -exact $Typ {
            r    { set END reading }
            w    { set END writing }
        }
        ::StdLib::ErrorM "can't open file \"$FILE\" for $END"
    }
    return $FH
}

proc ::FileLib::GetList { FH } {
    return [split [ string trim [ read $FH ] ]]
}

proc ::FileLib::Close { FH } {
    catch { close $FH }
}

proc ::FileLib::MkRel { File Dir } {
    set	File	[file normalize $File]
    set	Dir [file normalize $Dir]
    set Idx [string first $Dir $File]
  ## File path does not start with dirname => nothing to do
    if {$Idx} { return $File }
  ## delete Dir from File
    set Len [string length $Dir]
  ## skip the first file separator
    incr Len
    return [string range $File $Len end]
}

proc ::FileLib::MkRelPath { File Dir } {
    set	File	[file normalize $File]
    set	Dir	[file normalize $Dir]

    set FP	[list]
    set DP	[list]
    set FG	1
    set	RE	0
    foreach f [file split $File] d [file split $Dir] {
	if {$FG} {
	    if {$f eq $d} {
		set RE 1
		continue
	    } else {
		if {!$RE} break
		set FG 0
	    }
	}
	if {$d ne ""} {
	    lappend DP ..
	}
	if {$f ne ""} {
	    lappend FP $f
	}
    }
    if {$RE} { 
	set RET [eval file join $DP $FP]
    } else {
	set RET	$File
    }

    return $RET
}

proc ::FileLib::MkAbs { File Dir } {
    if {[ string equal [ file pathtype $File ] {relative} ]} {
        return [ file join $Dir $File ]
    }
    return $File
}

proc ::FileLib::Check { AL args } {

    set File [ eval file join $args ]

    foreach A $AL {
        switch -exact -- $A {
            exist    {
                if {![ file exist $File ]} {
                    ::StdLib::ErrorM "file \"$File\" does not exist"
                }
            }
            readable    {
                if {![ file readable $File ]} {
                    ::StdLib::ErrorM "can't read file \"$File\""    
                }
            }
            writable    {
                if {![ file writable $File ]} {
                    ::StdLib::ErrorM "can't write file \"$File\""    
                }
            }
            file    {
                if {![ file isfile $File ]} {
                    ::StdLib::ErrorM "file \"$File\" is no regular file"    
                }
            }
            directory    {
                if {![ file isdirectory $File ]} {
                    ::StdLib::ErrorM "file \"$File\" is no directory"    
                }
            }
            default    {
                ::StdLib::Error "::FileLib::Check: token \"$A\" is not supported"
            }
        }
    }

    return $File
}

##
## -------------------------------------------------------------
##

proc ::FileLib::Append { File Str } {
    if {[catch { open $File a} FH]} { ::StdLib::_ErrHdl $FH }
    puts $FH $Str
    if {[catch { close $FH } ERR]} { ::StdLib::_ErrHdl $ERR }
}

proc ::FileLib::Write { File Str } {
    if {[catch { open $File w} FH]} { ::StdLib::_ErrHdl $FH }
    puts $FH $Str
    if {[catch { close $FH } ERR]} { ::StdLib::_ErrHdl $ERR }
}

proc ::FileLib::Read { File } {
    set RET {}
    if {[catch { open $File r} FH]} { ::StdLib::_ErrHdl $FH }
    set RET [ read $FH [ file size $File ] ]
    if {[catch { close $FH } ERR]} { ::StdLib::_ErrHdl $ERR }
    return $RET
}

proc ::FileLib::ReadBin { File } {
    set RET {}
    if {[catch { open $File r} FH]} { ::StdLib::_ErrHdl $FH }
    fconfigure $FH -encoding binary -translation binary
    set RET [ read $FH [ file size $File ] ]
    if {[catch { close $FH } ERR]} { ::StdLib::_ErrHdl $ERR }
    return $RET
}

proc ::FileLib::WriteBin { File Str} {
    set RET {}
    if {[catch { open $File w} FH]} { ::StdLib::_ErrHdl $FH }
    fconfigure $FH -encoding binary -translation binary
    puts -nonewline $FH $Str
    if {[catch { close $FH } ERR]} { ::StdLib::_ErrHdl $ERR }
    return $RET
}

proc ::FileLib::Delete { File } {
    if {[catch { file delete -force $File} ERR]} { ::StdLib::_ErrHdl $ERR }
}

##
## -------------------------------------------------------------
##

proc ::FileLib::RemoveDir {Dir} {

    if {[catch {file delete -force $Dir}]} {

	foreach L [ GetTree [ list -directory $Dir -nocomplain -types l * ] ] {
	    file delete -force $L
	}

	foreach F [ GetTree [ list -directory $Dir -nocomplain -types f * ] ] {
	    file delete -force $F
	}

	foreach D [ GetTree [ list -directory $Dir -nocomplain -types d * ] ] {
	    file delete -force $D
	}
	file delete -force $Dir
    }
}

##
## ---------------------------------------------------
##

proc ::FileLib::CreateTmpFile { Dir {Pattern %i}} {
    if {[file exists $Dir]} {
	if {![file isdirectory $Dir]} {
	    ::StdLib::Error "\[::FileLib::CreateTmpFile\] file <$Dir> is !no! directory"
	}
    } else {
	file mkdir $Dir
    }

    while {1} {
	set I	    [expr {round(rand()*100000.0)}]
        set NFile   [file join $Dir [format $Pattern $I]]

	if {![file exists $NFile]} break
    }
    Touch $NFile
    return $NFile
}

