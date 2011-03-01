<?php
#+
#§  \file       theLink/example/php/mulclient.php
#§  \brief      \$Id$
#§  
#§  (C) 2010 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

$srv = new MqS();
try {
  $srv->ConfigSetName("MyMul");
  $srv->LinkCreate($argv);
  $srv->SendSTART();
  $srv->SendD(3.67);
  $srv->SendD(22.3);
  $srv->SendEND_AND_WAIT("MMUL");
  print($srv->ReadD());
} catch (Exception $ex) {
  $srv->ErrorSet($ex);
}
$srv->Exit();

?>
