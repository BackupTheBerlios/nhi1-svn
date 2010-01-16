#+
#§  \file       win/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

EXTRA_DIST = \
.MAKE_DIST.bash \
Makefile.am \
Makefile.in \
README.win \
mqconfig.h \
env.sh \
env.tcl \
c \
cc \
csharp \
vb \
win \
win.sln

dist-hook:
	find $(distdir) -type d -name ".svn" -exec rm -fr "{}" ';' 2>/dev/null; true
