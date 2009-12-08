#!/bin/bash
#+
#§  \file       win/.CreateMakefileAm.bash
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

grep '^#' Makefile.am > Makefile.am.t
mv Makefile.am.t Makefile.am

echo 'EXTRA_DIST = \' >> Makefile.am

IFS="
"
for f in $(svn list -R 2>&1) ; do
  [[ "$f" == "win.sln" ]] && continue
  test -d $f && continue
  if [[ "$f" == *" "* ]] ; then
    echo "\\\"$f\\\" \\" >> Makefile.am
  else
    echo "$f \\" >> Makefile.am
  fi
done

echo "win.sln" >> Makefile.am

