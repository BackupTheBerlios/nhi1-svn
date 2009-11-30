#!/bin/bash
#+
#§  \file       win/.CreateMakefileAm.bash
#§  \brief      \$Id: .CreateMakefileAm.bash 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
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

