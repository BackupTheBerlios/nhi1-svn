<?php
#+
#§  \file       theLink/example/php/Filter4.php
#§  \brief      \$Id$
#§  
#§  (C) 2010 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

#$stderr = fopen("php://stderr", "w");

class Filter4 extends MqS implements iServerSetup, iServerCleanup, iEvent, iFactory {
  public function __construct() {
    $this->ConfigSetIgnoreExit(TRUE);
    $this->ConfigSetIdent("transFilter");
    $this->ConfigSetName('Filter4');
    $this->items = array();
    $this->fh = NULL;
    parent::__construct();
  }
  public function Factory() {
    return new Filter4();
  }
  public function ServerSetup() {
    $ftr = $this->ServiceGetFilter();
    $this->ServiceCreate("LOGF", array(&$this, 'LOGF'));
    $this->ServiceCreate("EXIT", array(&$this, 'EXITX'));
    $this->ServiceCreate("+ALL", array(&$this, 'FilterIn'));
    $ftr->ServiceCreate("WRIT",  array(&$ftr , 'WRIT'));
  }
  public function ServerCleanup() {
    $ftr = $this->ServiceGetFilter();
    if ($ftr->fh != NULL) {
      fclose($ftr->fh);
    }
    $ftr->fh = NULL;
  }
  public function WRIT() {
    fwrite($this->fh, $this->ReadC() . "\n");
    fflush($this->fh);
    $this->SendRETURN();
  }
  public function ErrorWrite() {
    fwrite($this->fh, "ERROR: " . $this->ErrorGetText() . "\n");
    fflush($this->fh);
    $this->ErrorReset();
  }
  public function LOGF() {
    $ftr = $this->ServiceGetFilter();
    if ($ftr->LinkGetTargetIdent() == "transFilter") {
      $ftr->SendSTART();
      $ftr->SendC($this->ReadC());
      $ftr->SendEND_AND_WAIT("LOGF");
    } else {
      $ftr->fh = fopen($this->ReadC(), "a");
    }
    $this->SendRETURN();
  }
  public function EXITX() {
    $this->ErrorSetEXIT();
  }
  public function Event() {
    if (count($this->items) == 0) {
      $this->ErrorSetCONTINUE();
    } else {
      $ftr = $this->ServiceGetFilter();
      try {
        $ftr->LinkConnect();
        $ftr->SendBDY($this->items[0]);
      } catch (Exception $ex) {
        $ftr->ErrorSet($ex);
        if ($ftr->ErrorIsEXIT()) {
          $ftr->ErrorReset();
          return;
        } else {
          $ftr->ErrorWrite();
	}
      }
      array_shift($this->items);
    }
  }
  public function FilterIn() {
    $this->items[] = $this->ReadBDY();
    $this->SendRETURN();
  }
}
$srv = new Filter4();
try {
  $srv->LinkCreate($argv);
  $srv->ProcessEvent(MqS::WAIT_FOREVER);
} catch (Exception $ex) {
  $srv->ErrorSet($ex);
}
$srv->Exit();

?>
