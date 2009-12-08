#+
#§  \file       theLink/example/tcl/mulclient.tcl
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§
package require TclMsgque
set ctx [tclmsgque MqS]
$ctx LinkCreate --name "MyMul" {*}$argv
$ctx SendSTART
$ctx SendD 3.67 
$ctx SendD 22.3 
$ctx SendEND_AND_WAIT "MMUL" 
puts [$ctx ReadD]
$ctx Exit

