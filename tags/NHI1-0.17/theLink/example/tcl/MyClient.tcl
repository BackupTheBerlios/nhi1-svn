#+
#:  \file       theLink/example/tcl/MyClient.tcl
#:  \brief      \$Id$
#:  
#:  (C) 2009 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

package require TclMsgque
set ctx [tclmsgque MqS]
$ctx ConfigSetName "MyClient"
$ctx LinkCreate {*}$argv
$ctx SendSTART
$ctx SendEND_AND_WAIT "HLWO" 
puts [$ctx ReadC]
$ctx Exit


