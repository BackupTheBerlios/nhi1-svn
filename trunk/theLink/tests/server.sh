#!/bin/sh -xv
#+
#§  \file       tests/server.sh
#§  \brief      \$Id: server.sh 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

ddd --gdb --args ./server "$@"

