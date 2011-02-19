<?php
#+
#§  \file       theLink/example/php/Filter6.php
#§  \brief      \$Id$
#§  
#§  (C) 2011 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

#$stderr = fopen("php://stderr", "w");

class Filter6 extends MqS implements iServerSetup, iServerCleanup, iEvent {
  public function __construct($tmpl=NULL) {
    parent::__construct($tmpl);
    $this->ConfigSetIgnoreExit(TRUE);
    $this->fh = NULL;
  }
  public function ServerSetup() {
    $ftr = $this->ServiceGetFilter();
    $this->ServiceCreate("LOGF", array(&$this, 'LOGF'));
    $this->ServiceCreate("EXIT", array(&$this, 'EXITX'));
    $this->ServiceCreate("+ALL", array(&$this, 'FilterIn'));
    $this->ServiceStorage("PRNT");
    $ftr->ServiceCreate("WRIT",  array(&$ftr , 'WRIT'));
    $ftr->ServiceProxy("+TRT");
  }
  public function ServerCleanup() {
    if ($this->fh != NULL) {
      fclose($this->fh);
    }
    $this->fh = NULL;
  }
  public function WRIT() {
    $fh = $this->ServiceGetFilter()->fh;
    fwrite($fh, $this->ReadC() . "\n");
    fflush($fh);
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
      $this->ReadForward($ftr);
    } else {
      $this->fh = fopen($this->ReadC(), "a");
    }
    $this->SendRETURN();
  }
  public function EXITX() {
    exit(1);
  }
  public function Event() {
    if ($this->StorageCount() == 0) {
      $this->ErrorSetCONTINUE();
    } else {
      $id = 0;
      try {
	$ftr = $this->ServiceGetFilter();
        $ftr->LinkConnect();
	$id = $this->StorageSelect();
	$this->ReadForward($ftr);
      } catch (Exception $ex) {
        $this->ErrorSet($ex);
        if ($this->ErrorIsEXIT()) {
          $this->ErrorReset();
          return;
        } else {
          $this->ErrorWrite();
	}
      }
      $this->StorageDelete($id);
    }
  }
  public function FilterIn() {
    $this->StorageInsert();
    $this->SendRETURN();
  }
}
$srv = FactoryAdd('transFilter', 'Filter6')->New();
try {
  $srv->LinkCreate($argv);
  $srv->ProcessEvent(MqS::WAIT_FOREVER);
} catch (Exception $ex) {
  $srv->ErrorSet($ex);
}
$srv->Exit();

?>

