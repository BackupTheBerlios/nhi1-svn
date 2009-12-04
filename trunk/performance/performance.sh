#!/bin/bash
#+
#§  \file       performance/performance.sh
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

test ! -f ./env.sh && {
  echo "ERROR: you have to run the '../configure' script first"
  exit 1
}
. ./env.sh

typeset -i PORT=7777

[[ $1 == "-vg" ]] && { VG="valgrind --trace-children=yes" ; shift ; } || VG=""
[[ $1 == "-x" ]] && set -x
[[ $1 == "-v" ]] && set -v
[[ $1 == "--help-tests" ]] && HT=yes || HT=no
NUM=" --num 10 "
NUM=" "

## setup the TOTAL perf
FS=" %-20s | %-8s %-8s %-8s %-8s %-8s %-8s %-8s\n"
FI=" %-20s | %8s %8s %8s %9s %8s %8s %8s\n"
exec 3>docs/total.perf
DATE=$(date +"%Y-%m-%d %H:%M:%S")
printf "$FS" "   Tests from:    " "  send  " "  send  " "  send  " " parent  " " parent " " child  " " child  " 1>&3
printf "$FS" "$DATE"              "  END   " "CALLBACK" "  WAIT  " " create  " " delete " " create " " delete " 1>&3
printf "$FS" -------------------   --------   --------   --------   ---------   --------   --------   --------  1>&3

kill_parent=1
sig_spec=-9

function do_kill # <pid>...
{
    kill "$sig_spec" "$@"
}

function kill_children # pid
{
    local target=$1
    local pid=
    local ppid=
    local i
    # Returns alternating ids: first is pid, second is parent
    IFS="
"
    for line in $(ps -o pid,ppid | tail +2); do
	unset IFS
	set $line
	pid=$1
	ppid=$2
	(( ppid == target && pid != $$ )) && {
	    kill_children $pid
	    do_kill $pid
	}
    done
}

c=C
cc=C++
csharp=C#
java=Java
perl=Perl
python=Python
tcl=Tcl
vb=VB.NET
php=PHP

Total() {
  L=$1;
  T=$2;
  F=${L}_${T}
  local -a vals
  IFS="
"
  IDX=0
  if test -f docs/$F.perf ; then
    for LINE in $(<docs/$F.perf) ; do
      unset IFS
      [[ $LINE == *statistics* ]] && {
	## clean WhiteSpace
	for V in ${LINE:98}; do
	  vals[$IDX]=$V
	  let IDX++
	done
      }
    done

    printf "$FI" "   ${!L}" ${vals[*]} 1>&3
  else 
    printf "$FI" "   ${!L}" na. na. na. na. na. na. na. 1>&3
  fi
}

ALL="c_pipe
c_tcp_fork
c_tcp_thread
c_tcp_spawn
c_uds_fork
c_uds_thread
c_uds_spawn
cc_pipe
cc_tcp_fork
cc_tcp_thread
cc_tcp_spawn
cc_uds_fork
cc_uds_thread
cc_uds_spawn
tcl_pipe
tcl_tcp_fork
tcl_tcp_thread
tcl_tcp_spawn
tcl_uds_fork
tcl_uds_thread
tcl_uds_spawn
java_pipe
java_tcp_thread
java_tcp_spawn
java_uds_thread
java_uds_spawn
python_pipe
python_tcp_fork
python_tcp_spawn
python_uds_fork
python_uds_spawn
csharp_pipe
csharp_tcp_thread
csharp_tcp_spawn
csharp_uds_thread
csharp_uds_spawn
vb_pipe
vb_tcp_thread
vb_tcp_spawn
vb_uds_thread
vb_uds_spawn
perl_pipe
perl_tcp_fork
perl_tcp_thread
perl_tcp_spawn
perl_uds_fork
perl_uds_thread
perl_uds_spawn"

[[ $HT == "yes" ]] && {
  echo $ALL
  exit 0
}

#apply the commandline filter
R=""
for A in $ALL; do
  for C in "$@" ; do
    [[ "$A" == *$C* ]] || continue 2
  done
  R+=" $A"
done

## do the tests
#for SRV in c tcl java python ; do
for SRV in $R; do

  echo $SRV

  case $SRV in
    *thread*)	
      ENV="ENV=thread ./performance_thread.env ./local.env"
      export TESTDIR="thread/$PACKAGE-$PACKAGE_VERSION/theLink/tests"
    ;;
    total)	
      ENV=""
      export TESTDIR=""
    ;;
    *)	
      ENV="ENV=nothread ./performance_nothread.env ./local.env"
      export TESTDIR="nothread/$PACKAGE-$PACKAGE_VERSION/theLink/tests"
    ;;
  esac

  case $SRV in
    *python*)	
      SERVER="python $TESTDIR/server.py"	    
    ;;
    *csharp*)	
      SERVER="mono $TESTDIR/csserver.exe"	    
    ;;
    *vb*)	
      SERVER="mono $TESTDIR/vbserver.exe"	    
    ;;
    total)	
      SERVER=""	    
    ;;
    *java*)	
      SERVER="java example.Server" 
    ;;
    *perl*)	
      SERVER="perl $TESTDIR/server.pl" 
    ;;
    *tcl*)	
      SERVER="tclsh $TESTDIR/server.tcl" 
    ;;
    *cc*)	
      SERVER="$TESTDIR/ccserver" 
    ;;
    *c*)	
      SERVER="$TESTDIR/server "
    ;;
    *)
      echo "ERROR invalid server '$SRV'" 1>&1
      exit 1
    ;;
  esac

  case $SRV in
    *pipe*)
      CL="$VG${VG:+ }$TESTDIR/client${NUM}--all @ $SERVER"
      echo "> $CL" | tee docs/${SRV}.perf
      eval $ENV $CL 2>&1 | tee -a docs/${SRV}.perf

      continue
    ;;
    *uds*)
      COM_ARGS="--uds --file ./socket.uds"
      COM_RM="./socket.uds"
    ;;
    *tcp*)
      COM_ARGS="--tcp --host localhost --port $PORT"
      COM_RM=""
      let PORT=PORT+1
    ;;
  esac

  case $SRV in
    *thread*)	START=thread;;
    *spawn*)	START=spawn;;
    *fork*)	START=fork;;
  esac

  SV="$VG${VG:+ }$SERVER $COM_ARGS --$START"
  CL="$VG${VG:+ }$TESTDIR/client${NUM}--all $COM_ARGS"
  echo "> $SV" | tee docs/${SRV}.perf
  echo "> $CL" | tee -a docs/${SRV}.perf
  eval $ENV $SV 1>/dev/null 2>&1 &
  PID=$! 
  sleep 2
  $CL 2>&1 | tee -a docs/${SRV}.perf
  kill_children $PID

done

# calculate the total
for TST in pipe tcp_fork tcp_thread tcp_spawn uds_fork uds_thread uds_spawn; do
  echo -e "\n $TST:" 1>&3
  for LNG in c cc csharp java perl python tcl vb php; do
    Total ${LNG} ${TST}
  done
done

test -f socket.uds && rm socket.uds

