<?php
#+
#§  \file       theLink/example/php/testserver.php
#§  \brief      \$Id$
#§  
#§  (C) 2010 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

class TestServer extends MqS implements iServerSetup, iFactory {
  public function Factory() {
    return new TestServer();
  }
  public function ServerSetup() {
    $this->ServiceCreate('GTCX', array(&$this, 'GTCX'));
  }
  public function GTCX() {
    $this->SendSTART();
    $this->SendI($this->LinkGetCtxId());
    $this->SendC("+");
    if ($this->LinkIsParent()) {
      $this->SendI(-1);
    } else {
      $this->SendI($this->LinkGetParent()->LinkGetCtxId());
    }
    $this->SendC("+");
    $this->SendC($this->ConfigGetName());
    $this->SendC(":");
    $this->SendRETURN();
  }
}
$srv = new TestServer();
try {
  $srv->LinkCreate($argv);
  $srv->ProcessEvent(MqS::WAIT_FOREVER);
} catch (Exception $ex) {
  $srv->ErrorSet($ex);
}
$srv->Exit();

?>
