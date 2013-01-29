#+
#:  \file       theLink/tests/MakeTclTest.bash
#:  \brief      \$Id$
#:  
#:  (C) 2009 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

set -u

LANG="C"
. ../../env.sh
export LANG

if [[ ${WINDIR:-} != "" ]] ; then
  PATH=$PATH:$LD_LIBRARY_PATH
  EXT=.exe
  MONO=""
else
  EXT=""
  MONO=$CLREXEC
fi
export TS_DEBUG=0

Usage() {
  cat - <<-EOF
usage $0 ?-ur? ?-vg|-gdb|-kdbg|-ddd|-lc|-sr|-sp|-st? args...

The scripts used in this directory are used to verify the libmsgque
library including the the other language bindings.
Testing is done using the tcltest feature of the tcl distribution.

  1. using the test feature of the build environment
        > cd NHI1-X.X
        > make check (not available for windows)

  2. invoke an individual test script for language "python" and "perl"
        > cd NHI1-X.X/theLink/tests
        > ./MakeTclTest.bash int.test --only-python-perl

  3. invoke all tests for language "go" using "threads" and only "binary" data
        > cd NHI1-X.X/theLink/tests
        > ./MakeTclTest.bash ./all.tcl --only-go --only-threads --only-binary

  4. start "tcl" server using "tcp" and listing on port "7777" and "spawn"
     for every new connection a new server
        > cd NHI1-X.X/theLink/tests
        > ./MakeTclTest.bash ../example/tcl/server.tcl --tcp --port 7777 --spawn

Help is available with the following commands:

  1. test specific help
        > cd NHI1-X.X/theLink/tests
        > ./MakeTclTest.bash ./all.tcl --help
     or
        > ./MakeTclTest.bash ./all.tcl --help-msgque

  2. tcltest specific help
        > man tcltest

Available test scripts:

$(cd $abs_top_srcdir/theLink/tests; ls -C *.test)

Setup the test-environment, usually you don't specify the following optione:

  -ur:	  use-remote
  -st:	  strace
            > $PREFIX_st
  -vg:	  valgrind:
            > $PREFIX_vg
  -sp:	  valgrind: gen-suppressions
            > $PREFIX_sp
  -lc:	  valgrind: leak-chec
            > $PREFIX_lc
  -sr:	  valgrind: leak-chec + show-reachable
            > $PREFIX_sr
  -gdb:	  use debugger 'gdb'
            > $PREFIX_gdb
  -ddd:	  use debugger 'ddd'
            > $PREFIX_ddd
  -kdbg:  use debugger 'kdbg'
            > $PREFIX_kdbg
  -lookup:  lookup a single like:
	    > MakeTclTest -lookup server.c

EOF
  exit 1
}

#
# lookup the token
#
lookup() {
  declare -a ret=()
  declare CMDDIR=$(cd $(dirname -- $1);pwd)
  declare CMD=$(basename -- $1)

  # fix the CMD path
  [[ ! -e "$CMDDIR/$CMD" ]] && {
    # look for CMD in the same directory but in the source tree
    CMDDIR="$abs_top_srcdir/${CMDDIR#$abs_top_builddir}"
    [[ ! -e  "$CMDDIR/$CMD" ]] && {
      # look into the example directories
      case "$CMD" in
	*.tcl)      CMDDIR="$abs_top_srcdir/theLink/example/tcl";;
	*.test)     CMDDIR="$abs_top_srcdir/theLink/test/";;
	*.java)     CMDDIR="$abs_top_builddir/theLink/example/java";;
	*.py)	    CMDDIR="$abs_top_srcdir/theLink/example/python";;
	*.pl)	    CMDDIR="$abs_top_srcdir/theLink/example/perl";;
	*.rb)	    CMDDIR="$abs_top_srcdir/theLink/example/ruby";;
	*.php)      CMDDIR="$abs_top_srcdir/theLink/example/php";;
	*.exe|*.cs) CMDDIR="$abs_top_builddir/theLink/example/csharp";;
	*.vb)	    CMDDIR="$abs_top_builddir/theLink/example/vb";;
	*.go)	    CMDDIR="$abs_top_builddir/theLink/example/go";;
	*.cc)	    CMDDIR="$abs_top_builddir/theLink/example/cc";;
	*.c)	    CMDDIR="$abs_top_builddir/theLink/example/c";;
      esac
    }
  }
  CMD="$CMDDIR/$CMD"

  # find executable is CMD is a script-file
  case "$CMD" in
    *.tcl|*.test)   EXE="$TCLSH";;
    *.java)	    EXE="$JAVA"; CMD="${CMD%.*}";;
    *.py)	    EXE="$PYTHON";;
    *.pl)	    EXE="$PERL -I$abs_top_srcdir/theLink/example/perl";;
    *.rb)	    EXE="$RUBY";;
    *.php)	    EXE="$PHP";;
    *.exe)	    EXE="$MONO";;
    *.go)	    EXE=""; CMD="${CMD%.*}$EXT";;
    *.cc)	    EXE=""; CMD="${CMD%.*}$EXT";;
    *.c)	    EXE=""; CMD="${CMD%.*}$EXT";;
    *)		    EXE="";;
  esac

  [[ ! -e  "$CMD" ]] && {
    echo "unable to find the path for command: '$CMD'"
    exit 1
  }

  test -n "$EXE" && ret+=("$EXE")
  ret+=("$CMD")
  echo "${ret[@]}"
}

PREFIX=""
POSTFIX="cat"
TEE="yes"

declare -i lookup=0

PREFIX_vg="valgrind --trace-children=yes --num-callers=36 --quiet"
PREFIX_gdb="gdb -d ../perlmsgque/Net*/ --tui --args"
PREFIX_ddd="ddd --args"
PREFIX_kdbg="/opt/kde3/bin/kdbg"
PREFIX_lc="valgrind --trace-children=yes --leak-check=full --num-callers=36 --quiet"
PREFIX_sr="valgrind --trace-children=yes --leak-check=full --show-reachable=yes --num-callers=36 --quiet"
PREFIX_st="strace"
PREFIX_sp="valgrind --trace-children=yes --leak-check=full --num-callers=36 --quiet --gen-suppressions=all"


(( $# == 0 )) && Usage
[[ $1 == "-ur" ]] && {
  shift
  set -- "$@" --tcp --port 7777 --thread
}
case "$1" in
  -vg) 
    PREFIX=$PREFIX_vg
    POSTFIX="grep -v DWARF2"
    shift
  ;;
  -gdb) 
    PREFIX=$PREFIX_gdb
    POSTFIX=""
    TEE="no"
    shift
  ;;
  -ddd) 
    PREFIX=$PREFIX_ddd
    TEE="no"
    shift
  ;;
  -kdbg) 
    PREFIX=$PREFIX_kdbg
    TEE="no"
    shift
  ;;
  -lc) 
    PREFIX=$PREFIX_lc
    POSTFIX="grep -v DWARF2"
    shift
  ;;
  -sr) 
    PREFIX=$PREFIX_sr
    POSTFIX="grep -v DWARF2"
    shift
  ;;
  -st) 
    PREFIX=$PREFIX_st
    shift
  ;;
  -sp) 
    PREFIX=$PREFIX_sp
    POSTFIX="grep -v DWARF2"
    shift
  ;;
  -mdb) 
    MONO="mdb"
    shift
  ;;
  -lookup) 
    lookup "$2"
    exit
    shift
  ;;
  -*)
    Usage
  ;;
esac

# rebuild command line, expand the path, find executable etc..
#
cmdline=()
first=1
id=$(basename -- $1)
id=${id%.*}

while (( $# )) ; do
  if (( $first )) ; then
    cmdline+=( $(lookup $1) )
    first=0
  else
    cmdline+=($1)
    [[ "$1" == "@" ]] && first=1
  fi
  shift
done

echo "exec: ${cmdline[*]}"

set "${cmdline[@]}"
EXE=$1; shift

if [[ $TEE == "yes" ]] ; then
  if [[ $PREFIX == *kdbg* ]] ; then
    exec $PREFIX $EXE -a "$*" 2>&1 | $POSTFIX | tee /tmp/$id.log
  else
    exec $PREFIX $EXE "$@" 2>&1 | $POSTFIX | tee /tmp/$id.log
  fi
else
  if [[ $PREFIX == *kdbg* ]] ; then
    exec $PREFIX $EXE -a "$*}"
  else
    if [[ "$POSTFIX" == "cat" ]] ; then
      exec $PREFIX $EXE "$@" 2>&1 | $POSTFIX
    else
      exec $PREFIX $EXE "$@"
    fi
  fi
fi

