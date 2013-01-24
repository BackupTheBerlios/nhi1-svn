#!/bin/bash

set -u

## =======================================================================
##
## Setup Environment 
##
##   performance ...
##
## =======================================================================

## delete all environment variables to setup a "clean" build environment
[[ "${CLEAN:-}" != "yes" ]] && exec /usr/bin/env - \
  CLEAN="yes" TERM="$TERM" HOME="$HOME" MACHTYPE="$MACHTYPE" "$0" "$@"

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
    eval export $var=\"\$add\${array[*]:+:}\${array[*]:-}\"
    unset IFS
}
export -f add2var

Usage() {
  exec $SOURCE_HOME/configure --help
}

##
## =======================================================================
##

export PATH=/bin:/usr/bin
export LD_LIBRARY_PATH=

# I use ccache for compiling
export CCACHE_DIR="/build/dev01/Cache"
export CC="ccache gcc"
export CXX="ccache g++"
export CTAGSFLAGS="--c-kinds=+p"

SOURCE_HOME=$(dirname $(readlink -f $0))

##  Retrieve environment data
declare -A lang
declare -a opt

eval "$($SOURCE_HOME/bin/SetupEnv $0 "${@:-}" <<-'EOF'

G_Shell='bash'
# ---------------------------------------------------------------------
G_Date='$Date$'
G_Revision='$Revision$'
G_Author='$Author$'
G_Source='$URL$'
# --------------------------------------------------------------------
G_Description='wrapper to setup the Build-Environment for the "configure" script.
the configure-parameters are available in detail below'
G_HelpProc='Usage'
# -------------------------------------------------------------------
G_Option local-data-file default
G_Argument 'use-perf:Perf:0:use performance code:B'
G_Argument 'use-thread:Thread:0:use thread code:B'
for l in tcl perl python php cxx java csharp go ruby ; do
  G_Argument "lang-$l:lang[$l]:0:add '$l' language binding:B"
done

EOF
  )"

exit

for l in ${!lang[@]} ; do
  (( ${lang[$l]} )) && opt+=(--enable-$l)
done

if (( $Perf )) ; then
  opt+=(--enable-static)
  if (( $Thread )) ; then
    add2var PATH	      $HOME/ext/$MACHTYPE/performance_thread/bin
    add2var LD_LIBRARY_PATH   $HOME/ext/$MACHTYPE/performance_thread/lib
    opt+=(--enable-threads)
  else
    add2var PATH	      $HOME/ext/$MACHTYPE/performance_nothread/bin
    add2var LD_LIBRARY_PATH   $HOME/ext/$MACHTYPE/performance_nothread/lib
  fi
else
  if (( $Thread )) ; then
    add2var PATH	      $HOME/ext/$MACHTYPE/thread/bin
    add2var LD_LIBRARY_PATH   $HOME/ext/$MACHTYPE/thread/lib
    opt+=(--enable-threads)
  else
    add2var PATH	      $HOME/ext/$MACHTYPE/nothread/bin
    add2var LD_LIBRARY_PATH   $HOME/ext/$MACHTYPE/nothread/lib
  fi
fi

rm -fr /tmp/libmsgque-install

bash -norc $SOURCE_HOME/configure    \
		    --prefix=/tmp/libmsgque-install \
		    ${G_Argv:-} "${opt[@]}" \

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
