#+
#:  \file       tests/library.tcl
#:  \brief      \$Id$
#:  
#:  (C) 2013 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:
source ../env.tcl

package require tcltest
namespace import -force ::tcltest::*
verbose {pass body error}

set testdir [file normalize [file dirname [info script]]]

