#!/bin/bash

## =======================================================================
##
## Setup Environment 
##
##   performance ...
##
## =======================================================================

## delete all environment variables to setup a "clean" build environment
[[ "$HOME" != "" ]] && exec /usr/bin/env - TERM=$TERM $0 "$@"

## some useful procs
add2var() {
    (($# != 2)) && {
        echo "usage: $0 VAR Add1:Add2:..."
        echo "using arguments: $@"
        return 1
    }
    IFS=:
    local var=$1;
    local add=$2;
    typeset -i i
    declare -a array
  ## set array arguments
    array=(${!var})
  ## cleanup array
    for a in $add ; do
        for (( i=0 ; i<${#array[*]} ; i+=1 )) ; do
            [ "$a" == "${array[$i]}" ] && unset array[$i]
        done
        i=0;
    done
  ## set the new global variable
    eval export $var=\"\$add\:\${array[*]}\"
    unset IFS
}
export -f add2var

##
## =======================================================================
##

export PATH=/bin:/usr/bin
export LD_LIBRARY_PATH=

SOURCE_HOME=$(dirname $(readlink $0))

export  G_FileName="${0##*/}"
IFS=";";export  G_Args="${@:-}";unset IFS
export  G_Shell=ksh
#%%# ---------------------------------------------------------------------
export  G_Date='$Date$'
export  G_Revision='$Revision$'
export  G_Author='$Author$'
export  G_Source='$Source: /Repository/SetupEnv.CVS/ProcTemplate,v $'
#%%%# --------------------------------------------------------------------
export  G_Description='wrapper to setup the Build-Environment for the "configure" script.
the configure-parameters are available in detail below'
#export  G_HelpProc="$SOURCE_HOME/configure --help"
#%%%%# -------------------------------------------------------------------

#set -x

##  Retrieve environment data
eval "$($SOURCE_HOME/bin/SetupEnv -s -C '
  P:Perf:0:create maximum performance code:B
  T:Thread:0:use threads:B
'   )"

set +x

if (( $Perf )) ; then
  if (( $Thread )) ; then
    add2var PATH	      $HOME/ext/$MACHTYPE/performance_thread/bin
    add2var LD_LIBRARY_PATH   $HOME/ext/$MACHTYPE/performance_thread/lib
  else
    add2var PATHxi	      $HOME/ext/$MACHTYPE/performance_nothread/bin
    add2var LD_LIBRARY_PATHxi $HOME/ext/$MACHTYPE/performance_nothread/lib
  fi
else
  if (( $Thread )) ; then
    add2var PATHxi	      $HOME/ext/$MACHTYPE/thread/bin
    add2var LD_LIBRARY_PATH   $HOME/ext/$MACHTYPE/thread/lib
  else
    add2var PATH	      $HOME/ext/$MACHTYPE/nothread/bin
    add2var LD_LIBRARY_PATH   $HOME/ext/$MACHTYPE/nothread/lib
  fi
fi

rm -fr /tmp/libmsgque-install
  
bash -norc $SOURCE_HOME/configure    \
		    --prefix=/tmp/libmsgque-install \
		    "${G_Argv[@]}"  \

#		    --enable-cxx \
#		    --enable-java \
#		    --enable-csharp \
#		    --enable-go \
#		    --enable-ruby \
#		    --enable-python \
#		    --enable-perl \
#		    --enable-php \

#		    --enable-brain \
#		    --enable-guard \
#		    --enable-tcl \
#		    --enable-ruby \
#		    --enable-cxx \
#		    --enable-java \
#		    --enable-go \
#		    --enable-csharp \
#		    --enable-python \
#		    --enable-perl \
#		    --enable-php \
#		    --enable-vb \
#		    --enable-static \
