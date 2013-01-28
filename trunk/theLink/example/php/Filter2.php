<?php
#+
#:  \file       theLink/example/php/Filter2.php
#:  \brief      \$Id$
#:  
#:  (C) 2010 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

class Filter2 extends MqS implements iServerSetup {
  public function __construct($tmpl=NULL) {
    parent::__construct($tmpl);
    $this->ConfigSetName('filter');
    $this->data = array();
  }
  public function ServerSetup() {
    $this->ServiceCreate('+FTR', array(&$this, 'FTRcmd'));
    $this->ServiceProxy('+EOF');
  }
  public function FTRcmd() {
    throw new Exception('my error');
  }
}
$srv = new Filter2();
try {
  $srv->LinkCreate($argv);
  $srv->ProcessEvent(MqS::WAIT_FOREVER);
} catch (Exception $ex) {
  $srv->ErrorSet($ex);
}
$srv->Exit();

?>

