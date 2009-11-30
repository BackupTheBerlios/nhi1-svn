#+
#§  \file       example/tcl/MyClient.tcl
#§  \brief      \$Id: MyClient.tcl 507 2009-11-28 15:18:46Z dev1usr $
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
$ctx ConfigSetName "MyClient"
$ctx LinkCreate {*}$argv
$ctx SendSTART
$ctx SendEND_AND_WAIT "HLWO" 
puts [$ctx ReadC]
$ctx Exit


