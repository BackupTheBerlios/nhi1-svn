<?php
#+
#:  \file       theLink/example/php/Filter1.php
#:  \brief      \$Id$
#:  
#:  (C) 2010 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

class Filter1 extends MqS implements iServerSetup {
  public function __construct($tmpl=NULL) {
    parent::__construct($tmpl);
    $this->data = array();
  }
  public function ServerSetup() {
    $this->ServiceCreate('+FTR', array(&$this, 'FTRcmd'));
    $this->ServiceCreate('+EOF', array(&$this, 'EOFcmd'));
  }
  public function FTRcmd() {
    $list = array();
    while ($this->ReadItemExists()) {
      $item = $this->ReadC();
      $list[] = "<$item>";
    }
    $this->data[] = $list;
    $this->SendRETURN();
  }
  public function EOFcmd() {
    $ftr = $this->ServiceGetFilter();
    foreach ($this->data as $list) {
      $ftr->SendSTART();
      foreach ($list as $item) {
        $ftr->SendC($item);
      }
      $ftr->SendEND_AND_WAIT('+FTR');
    }
    $ftr->SendSTART();
    $ftr->SendEND_AND_WAIT('+EOF');
    $this->SendRETURN();
  }
}
MqS::Init(PHP_BINARY, "-c", get_cfg_var('cfg_file_path'), __FILE__);
$srv = FactoryAdd('filter', 'Filter1')->New();
try {
  $srv->LinkCreate($argv);
  $srv->ProcessEvent(MqS::WAIT_FOREVER);
} catch (Exception $ex) {
  $srv->ErrorSet($ex);
}
$srv->Exit();

?>

