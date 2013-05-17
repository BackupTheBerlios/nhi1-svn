<?php
#+
#:  \file       theLink/example/php/manfilter.php
#:  \brief      \$Id$
#:  
#:  (C) 2010 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

class ManFilter extends MqS implements iServerSetup {
  public function ServerSetup() {
    $this->ServiceCreate('+FTR', array(&$this, 'FTRcmd'));
    $this->ServiceProxy('+EOF');
  }
  public function FTRcmd() {
    $ftr = $this->ServiceGetFilter();
    $ftr->SendSTART();
    while ($this->ReadItemExists()) {
      $ftr->SendC("<" . $this->ReadC() . ">");
    }
    $ftr->SendEND_AND_WAIT('+FTR');
    $this->SendRETURN();
  }
}
$srv = new ManFilter();
try {
  $srv->ConfigSetName('filter');
  $srv->LinkCreate($argv);
  $srv->ProcessEvent(MqS::WAIT_FOREVER);
} catch (Exception $ex) {
  $srv->ErrorSet($ex);
}
$srv->Exit();

?>

