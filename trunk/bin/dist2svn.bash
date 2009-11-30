#!/bin/bash
#+
#§  \file       bin/dist2svn.bash
#§  \brief      \$Id: dist2svn.bash 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

for f in $(tar -tzvf libmsgque-3.4.tar.gz | awk '{print $6}') ; do
  f=${f#*/}
  test -d "$f" && continue
  svn info "$f" 1>/dev/null 2>/dev/null && continue
  echo $f
done
