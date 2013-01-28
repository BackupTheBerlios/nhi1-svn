<?php
#+
#:  \file       theLink/example/php/MyServer.php
#:  \brief      \$Id$
#:  
#:  (C) 2010 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

class MyServer extends MqS implements iServerSetup {
  public function ServerSetup() {
    $this->ServiceCreate('HLWO', array(&$this, 'HLWO'));
  }
  public function HLWO() {
    $this->SendSTART();
    $this->SendC("Hello World");
    $this->SendRETURN();
  }
}

$srv = FactoryAdd('MyServer')->New();
try {
  $srv->LinkCreate($argv);
  $srv->ProcessEvent(MqS::WAIT_FOREVER);
} catch (Exception $ex) {
  $srv->ErrorSet($ex);
}
$srv->Exit();

?>
