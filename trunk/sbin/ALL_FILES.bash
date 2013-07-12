#+
#:  \file       sbin/ALL_FILES.bash
#:  \brief      \$Id$
#:  
#:  (C) 2013 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:
exec find . -type f | egrep -v '(svn|html|tags|performance|binary-build)' 
