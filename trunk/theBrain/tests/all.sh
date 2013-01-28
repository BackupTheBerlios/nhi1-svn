#!/usr/bin/env bash
#+
#:  \file       theBrain/tests/all.sh
#:  \brief      \$Id$
#:  
#:  (C) 2009 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

## get env
. ../../env.sh

## start the tcl test
exec ${TCLSH} $(dirname $0)/all.tcl

