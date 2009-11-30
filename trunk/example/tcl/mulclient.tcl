#+
#§  \file       example/tcl/mulclient.tcl
#§  \brief      \$Id: mulclient.tcl 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
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

