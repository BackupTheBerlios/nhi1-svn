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

## setup the TOTAL - LINK perf
FSL=" %-20s | %-8s %-8s %-8s %-8s %-8s %-8s %-8s\n"
FIL=" %-20s | %8s %8s %8s %9s %8s %8s %8s\n"
exec 3>docs/total_link.perf
DATE=$(date +"%Y-%m-%d %H:%M:%S")
printf "$FSL" "   Tests from:    " "  send  " "  send  " "  send  " " parent  " " parent " " child  " " child  " 1>&3
printf "$FSL" "$DATE"              "  END   " "CALLBACK" "  WAIT  " " create  " " delete " " create " " delete " 1>&3
printf "$FSL" -------------------   --------   --------   --------   ---------   --------   --------   --------  1>&3

## setup the TOTAL - BRAIN perf
FSB=" %-20s | %-8s %-8s\n"
FIB=" %-20s | %8s %8s\n"
exec 4>docs/total_brain.perf
DATE=$(date +"%Y-%m-%d %H:%M:%S")
printf "$FSB" "   Tests from:    " "        " "        " 1>&4
printf "$FSB" "$DATE"              "  PUT   " "  GET   " 1>&4
printf "$FSB" -------------------   --------   --------  1>&4

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

function error
{
  echo "ERROR: $@"
  exit 1
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

TotalLink() {
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

    printf "$FIL" "   ${!L}" ${vals[*]} 1>&3
  else 
    printf "$FIL" "   ${!L}" na. na. na. na. na. na. na. 1>&3
  fi
}

TotalBrain() {
  L=$1;
  T=$2;
  F=${L}_${T}
  local -a vals
  IFS="
"
  IDX=0
  if test -f docs/$F.perf ; then
    FLAG=NO
    for LINE in $(<docs/$F.perf) ; do
      unset IFS
      [[ $LINE == *statistics* && $FLAG == "NO" ]] && {
	set - ${LINE:68}
	N="$1 $2"
	V1=$5
	FLAG=YES
	continue
      }
      [[ $LINE == *statistics* && $FLAG == "YES" ]] && {
	set - ${LINE:68}
	[[ $N != "$1 $2" ]] && error FORMAT
	V2=$5
	FLAG=NO
	printf "$FIL" "   $N" $V1 $V2 1>&4
	continue
      }
    done

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
perl_uds_spawn
brain_pipe
brain_tcp_fork
brain_tcp_thread
brain_tcp_spawn
brain_uds_fork
brain_uds_thread
brain_uds_spawn"

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
      export LINK_DIR="thread/$PACKAGE-$PACKAGE_VERSION/theLink"
      export BRAIN_DIR="thread/$PACKAGE-$PACKAGE_VERSION/theBrain"
    ;;
    total)	
      ENV=""
      export LINK_DIR=""
      export BRAIN_DIR=""
    ;;
    *)	
      ENV="ENV=nothread ./performance_nothread.env ./local.env"
      export LINK_DIR="nothread/$PACKAGE-$PACKAGE_VERSION/theLink"
      export BRAIN_DIR="nothread/$PACKAGE-$PACKAGE_VERSION/theBrain"
    ;;
  esac

  case $SRV in
    *brain*)	
      SERVER="$BRAIN_DIR/theBrain/theBrain"
      CLIENT="$BRAIN_DIR/tests/client"
    ;;
    *python*)	
      SERVER="python $LINK_DIR/tests/server.py"	    
      CLIENT="$LINK_DIR/tests/client"
    ;;
    *csharp*)	
      SERVER="mono $LINK_DIR/testscsserver.exe"	    
      CLIENT="$LINK_DIR/tests/client"
    ;;
    *vb*)	
      SERVER="mono $LINK_DIR/testsvbserver.exe"	    
      CLIENT="$LINK_DIR/tests/client"
    ;;
    total)	
      SERVER=""	    
      CLIENT=""
    ;;
    *java*)	
      SERVER="java example.Server" 
      CLIENT="$LINK_DIR/tests/client"
    ;;
    *perl*)	
      SERVER="perl $LINK_DIR/tests/server.pl" 
      CLIENT="$LINK_DIR/tests/client"
    ;;
    *tcl*)	
      SERVER="tclsh $LINK_DIR/tests/server.tcl" 
      CLIENT="$LINK_DIR/tests/client"
    ;;
    *cc*)	
      SERVER="$LINK_DIR/testsccserver" 
      CLIENT="$LINK_DIR/tests/client"
    ;;
    *c*)	
      SERVER="$LINK_DIR/tests/server"
      CLIENT="$LINK_DIR/tests/client"
    ;;
    *)
      echo "ERROR invalid server '$SRV'" 1>&1
      exit 1
    ;;
  esac

  case $SRV in
    *pipe*)
      CL="$VG${VG:+ }$CLIENT${NUM}--all @ $SERVER"
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
  CL="$VG${VG:+ }$CLIENT${NUM}--all $COM_ARGS"
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
    TotalLink ${LNG} ${TST}
  done
  echo -e "\n $TST:" 1>&4
  for LNG in brain; do
    TotalBrain ${LNG} ${TST}
  done
done

test -f socket.uds && rm socket.uds

