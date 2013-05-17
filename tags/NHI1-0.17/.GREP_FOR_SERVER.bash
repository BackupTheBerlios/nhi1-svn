#+
#:  \file       .GREP_FOR_SERVER.bash
#:  \brief      \$Id$
#:  
#:  (C) 2010 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:
exec ps -eaf | egrep -i '(filter|server)' | egrep -v '(ksmserver|nepomukserver|egrep|GREP_FOR_SERVER|vi )'
