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
  MONO=mono
fi

USAGE() {
  echo "usage $0 ?-ur? ?-vg|-gdb|-kdbg|-ddd|-lc|-sr|-sp|-st? args..."
  echo "   ur:  use-remote"
  echo "   st:  strace"
  echo "   sp:  valgrind: --gen-suppressions"
  exit 1
}

PREFIX=""
[[ $1 == "-h" ]] && USAGE
[[ $1 == "-ur" ]] && {
  shift
  set -- "$@" --tcp --port 7777 --thread
}
[[ $1 == "-vg" ]] && {
  PREFIX="valgrind --trace-children=yes --num-callers=36 --quiet"
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
  shift
}
[[ $1 == "-sr" ]] && {
  PREFIX="valgrind --trace-children=yes --leak-check=full --show-reachable=yes --num-callers=36 --quiet"
  shift
}
[[ $1 == "-st" ]] && {
  PREFIX="strace"
  shift
}
[[ $1 == "-sp" ]] && {
  PREFIX="valgrind --trace-children=yes --leak-check=full --num-callers=36 --quiet --gen-suppressions=all"
  shift
}
[[ $1 == "-mdb" ]] && {
  MONO=mdb
  shift
}
(( $# == 0 )) && USAGE

CMD=$1; shift

case "$CMD" in
  *.tcl|*.test)   EXE="$TCLSH";;
  *.java)	  EXE="$JAVA"; CMD="$(basename $CMD .java)";;
  *.py)		  EXE="$PYTHON";;
  *.pl)		  EXE="$PERL";;
  *.rb)		  EXE="$RUBY";;
  *.exe)	  EXE="$MONO";;
  *.cc)		  EXE=""; CMD="${CMD%\.*}$EXT";;
  *.c)		  EXE=""; CMD="${CMD%\.*}$EXT";;
  *)		  EXE="";;
esac

#set -x 

if [[ $TEE == "yes" ]] ; then
  if [[ $PREFIX == *kdbg* ]] ; then
    T="$CMD $@"
    exec $PREFIX $EXE -a "$T" 2>&1 | tee /tmp/$(basename $CMD).log
  else
    exec $PREFIX $EXE $CMD "$@" 2>&1 | tee /tmp/$(basename $CMD).log
  fi
else
  if [[ $PREFIX == *kdbg* ]] ; then
    T="$CMD $@"
    exec $PREFIX $EXE -a "$T"
  else
    exec $PREFIX $EXE $CMD "$@"
  fi
fi

