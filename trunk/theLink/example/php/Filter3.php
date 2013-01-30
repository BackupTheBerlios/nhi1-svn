<?php
#+
#:  \file       theLink/example/php/Filter3.php
#:  \brief      \$Id$
#:  
#:  (C) 2010 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

class Filter3 extends MqS implements iServerSetup {
  public function ServerSetup() {
    $ftr = $this->ServiceGetFilter();
    $this->ServiceProxy("+ALL");
    $this->ServiceProxy("+TRT");
    $ftr->ServiceProxy("+ALL");
    $ftr->ServiceProxy("+TRT");
  }
}
$srv = FactoryAdd('Filter3')->New();
try {
  $srv->LinkCreate($argv);
  $srv->ProcessEvent(MqS::WAIT_FOREVER);
} catch (Exception $ex) {
  $srv->ErrorSet($ex);
}
$srv->Exit();

?>

