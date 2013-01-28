<?php
#+
#:  \file       theLink/example/php/MyClient.php
#:  \brief      \$Id$
#:  
#:  (C) 2010 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

$srv = new MqS();
try {
  $srv->ConfigSetName("MyClient");
  $srv->LinkCreate($argv);
  $srv->SendSTART();
  $srv->SendEND_AND_WAIT("HLWO");
  print($srv->ReadC());
} catch (Exception $ex) {
  $srv->ErrorSet($ex);
}
$srv->Exit();

?>
