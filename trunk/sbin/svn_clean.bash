#!/bin/bash
#+
#:  \file       sbin/svn_clean.bash
#:  \brief      \$Id$
#:  
#:  (C) 2009 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

svn status * | while read line ; do
  set -- $line
  [[ "$1" == "I" ]] && rm -fr "$2"
done

exit 0

