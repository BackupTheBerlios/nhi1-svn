#+
#§  \file       theLink/tests/MakeTclTest.bash
#§  \brief      \$Id: MakeTclTest.bash 66 2009-12-13 14:59:58Z aotto1968 $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 66 $
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

LANG="C"
PATH="$(ls -d ../../theLink/*/ | awk 'BEGIN {ORS=":";} {print $0;}'):../aguard:${PATH:-}"
export LANG PATH
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
  echo "usage $0 ?-ur? ?-vg|-gdb|-lc|-sr|-sp|-st? args..."
  echo "   ur:  use-remote"
  echo "   st:  strace"
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
  *.tcl|*.test)   PREFIX="$PREFIX tclsh$EXT $CMD";;
  *.java)	  PREFIX="$PREFIX java$EXT $(basename $CMD .java)";;
  *.py)		  PREFIX="$PREFIX python$EXT $CMD";;
  *.pl)		  PREFIX="$PREFIX perl$EXT $CMD";;
  *.exe)	  PREFIX="$PREFIX $MONO $CMD";;
  *.cc)		  PREFIX="$PREFIX ${CMD%\.*}$EXT";;
  *.c)		  PREFIX="$PREFIX ${CMD%\.*}$EXT";;
  *)		  PREFIX="$PREFIX $CMD";;
esac

if [[ $TEE == "yes" ]] ; then
  exec $PREFIX "$@" 2>&1 | tee /tmp/$(basename $CMD).log
else
  exec $PREFIX "$@"
fi

