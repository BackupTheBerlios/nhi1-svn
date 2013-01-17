#+
#§  \file       theLink/tests/MakeTclTest.bash
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

set -u

LANG="C"
. ../../env.sh
export LANG
TEE=yes

if [[ ${WINDIR:-} != "" ]] ; then
  PATH=$PATH:$LD_LIBRARY_PATH
  EXT=.exe
  MONO=""
else
  EXT=""
  MONO=$CLREXEC
fi

USAGE() {
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

Setup the test-environment, usually you dont specify the following optione:

   -ur:  use-remote
   -st:  strace
   -vg:  valgrind:
   -sp:  valgrind: gen-suppressions
   -lc:  valgrind: leak-chec
   -sr:  valgrind: leak-chec + show-reachable

EOF
  exit 1
}

PREFIX=""
POSTFIX="cat"

(( $# == 0 )) && USAGE
[[ $1 == "-h" ]] && USAGE
[[ $1 == "-ur" ]] && {
  shift
  set -- "$@" --tcp --port 7777 --thread
}
[[ $1 == "-vg" ]] && {
  PREFIX="valgrind --trace-children=yes --num-callers=36 --quiet"
  POSTFIX="grep -v DWARF2"
  shift
}
[[ $1 == "-gdb" ]] && {
  PREFIX="gdb -d ../perlmsgque/Net*/ --tui --args"
  POSTFIX=""
  TEE=no
  shift
}
[[ $1 == "-ddd" ]] && {
  PREFIX="ddd --args"
  TEE=no
  shift
}
[[ $1 == "-kdbg" ]] && {
  PREFIX="/opt/kde3/bin/kdbg"
  TEE=no
  shift
}
[[ $1 == "-lc" ]] && {
  PREFIX="valgrind --trace-children=yes --leak-check=full --num-callers=36 --quiet"
  POSTFIX="grep -v DWARF2"
  shift
}
[[ $1 == "-sr" ]] && {
  PREFIX="valgrind --trace-children=yes --leak-check=full --show-reachable=yes --num-callers=36 --quiet"
  POSTFIX="grep -v DWARF2"
  shift
}
[[ $1 == "-st" ]] && {
  PREFIX="strace"
  shift
}
[[ $1 == "-sp" ]] && {
  PREFIX="valgrind --trace-children=yes --leak-check=full --num-callers=36 --quiet --gen-suppressions=all"
  POSTFIX="grep -v DWARF2"
  shift
}
[[ $1 == "-mdb" ]] && {
  MONO=mdb
  shift
}

ID=$1; shift
CMDDIR=$(cd $(dirname $ID);pwd)
CMD=$(basename $ID)

# fix the CMD path
[[ ! -e "$CMDDIR/$CMD" ]] && {
  # look for CMD in the same directory but in the source tree
  CMDDIR="$abs_top_srcdir/${CMDDIR#$abs_top_builddir}"
  [[ ! -e  "$CMDDIR/$CMD" ]] && {
    echo "unable to find the command: '$ID'"
  }
}

# find executable is CMD is a script-file
case "$CMD" in
  *.tcl|*.test)   EXE="$TCLSH";;
  *.java)	  EXE="$JAVA"; CMD="${CMD%%.java}";;
  *.py)		  EXE="$PYTHON";;
  *.pl)		  EXE="$PERL";;
  *.rb)		  EXE="$RUBY";;
  *.php)	  EXE="$PHP";;
  *.exe)	  EXE="$MONO";;
  *.go)		  EXE="${CMD%\.*}$EXT"; CMD="";;
  *.cc)		  EXE="${CMD%\.*}$EXT"; CMD="";;
  *.c)		  EXE="${CMD%\.*}$EXT"; CMD="";;
  *)		  EXE="";;
esac
#set -x 

if [[ $TEE == "yes" ]] ; then
  if [[ $PREFIX == *kdbg* ]] ; then
    T="$CMD $@"
    exec $PREFIX $EXE -a "$T"   2>&1 | $POSTFIX | tee /tmp/$CMD.log
  else
    exec $PREFIX $EXE "$CMDDIR/$CMD" "$@" 2>&1 | $POSTFIX | tee /tmp/$CMD.log
  fi
else
  if [[ $PREFIX == *kdbg* ]] ; then
    T="$CMD $@"
    set $EXE
    EXE="$1"; shift
    T="$@ $T"
    exec $PREFIX $EXE -a "$T"
  else
    if [[ "$POSTFIX" == "cat" ]] ; then
      exec $PREFIX $EXE "$CMDDIR/$CMD" "$@" 2>&1 | $POSTFIX
    else
      exec $PREFIX $EXE "$CMDDIR/$CMD" "$@"
    fi
  fi
fi

