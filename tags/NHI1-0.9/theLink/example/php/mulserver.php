<?php
#+
#§  \file       theLink/example/php/mulserver.php
#§  \brief      \$Id$
#§  
#§  (C) 2010 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

class MulServer extends MqS implements iServerSetup, iFactory {
  public function Factory() {
    return new MulServer();
  }
  public function ServerSetup() {
    $this->ServiceCreate('MMUL', array(&$this, 'MMUL'));
  }
  public function MMUL() {
    $this->SendSTART();
    $this->SendD($this->ReadD() * $this->ReadD());
    $this->SendRETURN();
  }
}
$srv = new MulServer();
try {
  $srv->LinkCreate($argv);
  $srv->ProcessEvent(MqS::WAIT_FOREVER);
} catch (Exception $ex) {
  $srv->ErrorSet($ex);
}
$srv->Exit();

?>
