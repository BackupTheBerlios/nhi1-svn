#+
#:  \file       sbin/ALL_C_CC_H_FILES.bash
#:  \brief      \$Id$
#:  
#:  (C) 2010 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:
exec find . -name "*.cc" -o -name "*.h" -o -name "*.c" | egrep -v '(performance|binary-build)'
