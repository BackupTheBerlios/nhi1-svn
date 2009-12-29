#+
#§  \file       theLink/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

#SUBDIRS = libmsgque example/c
SUBDIRS = libmsgque 
if USE_CXX
SUBDIRS += ccmsgque
SUBDIRS += example/cc
endif
if USE_JAVA
SUBDIRS += javamsgque
SUBDIRS += example/java
endif
if USE_PYTHON
SUBDIRS += pymsgque
SUBDIRS += example/python
endif
if USE_CSHARP
SUBDIRS += csmsgque
SUBDIRS += example/csharp
endif
if USE_PHP
SUBDIRS += phpmsgque
SUBDIRS += example/php
endif
if USE_PERL
SUBDIRS += perlmsgque
SUBDIRS += example/perl
endif
if USE_TCL
SUBDIRS += tclmsgque
SUBDIRS += example/tcl
SUBDIRS += tests
endif
if USE_VB
SUBDIRS += example/vb
endif
#SUBDIRS += acmds docs
SUBDIRS += docs
