#!/bin/bash
#+
#:  \file       sbin/GetFromInstall.bash
#:  \brief      \$Id$
#:  
#:  (C) 2009 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

Error() {
  echo "$@" 1>&2
  exit 1
}

(( $# != 1 )) && Error "usage: $0 dir"
D="$1"

test ! -d "$D" && Error "error: \"$D\" is no directory"

for f in *.c *.h ; do
  t="$D/$(basename $f)"
  if test -f "$t" ; then
    cp -v "$t" .
  else
    echo "warning: file \"$f\" does not belong to \"dir\""
  fi
done
