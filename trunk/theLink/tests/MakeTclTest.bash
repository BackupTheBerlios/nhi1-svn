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
  echo "usage $0 ?-ur? ?-vg|-gdb|-kdbg|-ddd|-lc|-sr|-sp|-st? args..."
  echo "   ur:  use-remote"
  echo "   st:  strace"
  echo "   vg:  valgrind:"
  echo "   sp:  valgrind: gen-suppressions"
  echo "   lc:  valgrind: leak-chec"
  echo "   sr:  valgrind: leak-chec + show-reachable"
  exit 1
}

PREFIX=""
POSTFIX="cat"
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
(( $# == 0 )) && USAGE

CMD=$1; shift
ID=$CMD

case "$CMD" in
  *.tcl|*.test)   EXE="$TCLSH";;
  *.java)	  EXE="$JAVA"; CMD="${CMD%%.java}"; ID=$CMD;;
  *.py)		  EXE="$PYTHON";;
  *.pl)		  EXE="$PERL";;
  *.rb)		  EXE="$RUBY";;
  *.php)	  EXE="$PHP";;
  *.exe)	  EXE="$MONO";;
  *.cc)		  EXE="${CMD%\.*}$EXT"; ID=$CMD; CMD="";;
  *.c)		  EXE="${CMD%\.*}$EXT"; ID=$CMD; CMD="";;
  *)		  EXE="";;
esac

#set -x 

if [[ $TEE == "yes" ]] ; then
  if [[ $PREFIX == *kdbg* ]] ; then
    T="$CMD $@"
    exec $PREFIX $EXE -a "$T"   2>&1 | $POSTFIX | tee /tmp/$(basename $ID).log
  else
    exec $PREFIX $EXE $CMD "$@" 2>&1 | $POSTFIX | tee /tmp/$(basename $ID).log
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
      exec $PREFIX $EXE $CMD "$@" 2>&1 | $POSTFIX
    else
      exec $PREFIX $EXE $CMD "$@"
    fi
  fi
fi

