#!/bin/bash
#+
#:  \file       bin/svn_ps_svn_ignore.tcl
#:  \brief      \$Id: svn_ps_svn_ignore.tcl 596 2013-01-28 15:00:31Z aotto1968 $
#:  
#:  (C) 2009 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev: 596 $
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

svn status * | while read line ; do
  set -- $line
  [[ "$1" == "I" ]] && rm -fr "$2"
done

exit 0

