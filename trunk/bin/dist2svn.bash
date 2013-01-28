#!/bin/bash
#+
#:  \file       bin/dist2svn.bash
#:  \brief      \$Id$
#:  
#:  (C) 2009 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

for f in $(tar -tzvf libmsgque-3.4.tar.gz | awk '{print $6}') ; do
  f=${f#*/}
  test -d "$f" && continue
  svn info "$f" 1>/dev/null 2>/dev/null && continue
  echo $f
done
