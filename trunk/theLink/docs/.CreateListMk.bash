#!/bin/bash
#+
#§  \file       win/.CreateMakefileAm.bash
#§  \brief      \$Id:$
#§  
#§  (C) 2009 - LibMsgque - Project - Group - 2
#§  
#§  \version    \$Rev:$
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

echo 'DX_DIST = \' > list.mk

for f in $(svn list -R 2>&1) ; do
  [[ "$f" == "doxygen.sh" ]] && continue
  test -d $f && continue
  echo "$f \\" >> list.mk
done

echo "doxygen.sh" >> list.mk

