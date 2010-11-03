<?php
#+
#§  \file       theLink/tests/server.php
#§  \brief      \$Id$
#§  
#§  (C) 2010 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

#phpinfo();

class Server extends MqS implements iServerSetup, iServerCleanup, iFactory {

  public function __construct() {
    $this->ConfigSetName("server");
    $this->ConfigSetIdent("test-server");
    parent::__construct();
  }

  public function Factory() {
    return new Server();
  }

  public function ServerCleanup() {
  }

  public function ServerSetup() {

    if ($this->SlaveIs() == false) {

      # add "master" services here
      $this->ServiceCreate("SETU", array(&$this, 'SETU'));
      $this->ServiceCreate("GETU", array(&$this, 'GETU'));
#
#      $this->ServiceCreate("PRNT", array(&$this, 'PRNT'));
#      $this->ServiceCreate("TRNS", array(&$this, 'TRNS'));
#      $this->ServiceCreate("TRN2", array(&$this, 'TRN2'));
#      $this->ServiceCreate("GTTO", array(&$this, 'GTTO'));
#      $this->ServiceCreate("MSQT", array(&$this, 'MSQT'));
#      $this->ServiceCreate("CNFG", array(&$this, 'CNFG'));
#      $this->ServiceCreate("SND1", array(&$this, 'SND1'));
#      $this->ServiceCreate("SND2", array(&$this, 'SND2'));
#      $this->ServiceCreate("REDI", array(&$this, 'REDI'));
#      $this->ServiceCreate("GTCX", array(&$this, 'GTCX'));
      $this->ServiceCreate("CSV1", array(&$this, 'CSV1'));
      $this->ServiceCreate("SLEP", array(&$this, 'SLEP'));
      $this->ServiceCreate("USLP", array(&$this, 'USLP'));
#      $this->ServiceCreate("CFG1", array(&$this, 'CFG1'));
#      $this->ServiceCreate("INIT", array(&$this, 'INIT'));
#      $this->ServiceCreate("LST1", array(&$this, 'LST1'));
#      $this->ServiceCreate("LST2", array(&$this, 'LST2'));
#
      $this->ServiceCreate("BUF1", array(&$this, 'BUF1'));
      $this->ServiceCreate("BUF2", array(&$this, 'BUF2'));
      $this->ServiceCreate("BUF3", array(&$this, 'BUF3'));
#
#      $this->ServiceCreate("ERR2", array(&$this, 'ERRX'));
#      $this->ServiceCreate("ERR3", array(&$this, 'ERRX'));
#      $this->ServiceCreate("ERR4", array(&$this, 'ERRX'));
#      $this->ServiceCreate("ERR5", array(&$this, 'ERRX'));
#      $this->ServiceCreate("ERR6", array(&$this, 'ERRX'));
#      $this->ServiceCreate("ERRT", array(&$this, 'ERRT'));
#
      $this->ServiceCreate("ECOY", array(&$this, 'ECOY'));
      $this->ServiceCreate("ECOO", array(&$this, 'ECOO'));
      $this->ServiceCreate("ECOS", array(&$this, 'ECOS'));
      $this->ServiceCreate("ECOI", array(&$this, 'ECOI'));
      $this->ServiceCreate("ECOW", array(&$this, 'ECOW'));
      $this->ServiceCreate("ECOF", array(&$this, 'ECOF'));
      $this->ServiceCreate("ECOD", array(&$this, 'ECOD'));
      $this->ServiceCreate("ECOC", array(&$this, 'ECOC'));
      $this->ServiceCreate("ECOB", array(&$this, 'ECOB'));
      $this->ServiceCreate("ECOU", array(&$this, 'ECOU'));
#      $this->ServiceCreate("ECON", array(&$this, 'ECON'));
#      $this->ServiceCreate("ECOL", array(&$this, 'ECOL'));
#      $this->ServiceCreate("ECLI", array(&$this, 'ECLI'));
#      $this->ServiceCreate("ERLR", array(&$this, 'ERLR'));
#      $this->ServiceCreate("ERLS", array(&$this, 'ERLS'));
#      $this->ServiceCreate("ECUL", array(&$this, 'ECUL'));
#      $this->ServiceCreate("RDUL", array(&$this, 'RDUL'));
    }
  }

  public function SLEP() {
    $i = $this->ReadI();
    sleep($i);
    $this->SendSTART();
    $this->SendI($i);
    $this->SendRETURN();
  }

  public function USLP() {
    $i = $this->ReadI();
    usleep($i);
    $this->SendSTART();
    $this->SendI($i);
    $this->SendRETURN();
  }

  public function CSV1() {
    # call an other service
    $this->SendSTART();
    $this->SendI($this->ReadI()+1);
    $this->SendEND_AND_WAIT("CSV2", 10);

    # read the answer and send the result back
    $this->SendSTART();
    $this->SendI($this->ReadI()+1);
    $this->SendRETURN();
  }

  public function ECOY() {
    $this->SendSTART();
    $this->SendY($this->ReadY());
    $this->SendRETURN();
  }

  public function ECOW() {
    $this->SendSTART();
    $this->SendW($this->ReadW());
    $this->SendRETURN();
  }

  public function ECOC() {
    $this->SendSTART();
    $this->SendC($this->ReadC());
    $this->SendRETURN();
  }

  public function ECOS() {
    $this->SendSTART();
    $this->SendS($this->ReadS());
    $this->SendRETURN();
  }

  public function ECOD() {
    $this->SendSTART();
    $this->SendD($this->ReadD());
    $this->SendRETURN();
  }

  public function ECOF() {
    $this->SendSTART();
    $this->SendF($this->ReadF());
    $this->SendRETURN();
  }

  public function BUF1() {
    $buf = $this->ReadU();
    $typ = $buf->GetType();
    $this->SendSTART();
    $this->SendC($typ);
    switch ($typ) {
      case "Y" : $this->SendY($buf->GetY()); break;
      case "O" : $this->SendO($buf->GetO()); break;
      case "S" : $this->SendS($buf->GetS()); break;
      case "I" : $this->SendI($buf->GetI()); break;
      case "F" : $this->SendF($buf->GetF()); break;
      case "W" : $this->SendW($buf->GetW()); break;
      case "D" : $this->SendD($buf->GetD()); break;
      case "C" : $this->SendC($buf->GetC()); break;
      case "B" : $this->SendB($buf->GetB()); break;
    }
    $this->SendRETURN();
  }

  public function BUF3() {
    $this->SendSTART();
    $buf = $this->ReadU();
    $this->SendC($buf->GetType());
    $this->SendU($buf);
    $this->SendI($this->ReadI());
    $this->SendU($buf);
    $this->SendRETURN();
  }

  public function BUF2() {
    $this->SendSTART();
    for ($i=0; $i<3; $i++) {
      $buf = $this->ReadU();
      $this->SendC($buf->GetType());
      $this->SendU($buf);
    }
    $this->SendRETURN();
  }

  public function ECOO() {
    $this->SendSTART();
    $this->SendO($this->ReadO());
    $this->SendRETURN();
  }

  public function ECOB() {
    $this->SendSTART();
    $this->SendB($this->ReadB());
    $this->SendRETURN();
  }

  public function ECOI() {
    $this->SendSTART();
    $this->SendI($this->ReadI());
    $this->SendRETURN();
  }

  public function ECOU() {
    $this->SendSTART();
    $this->SendU($this->ReadU());
    $this->SendRETURN();
  }

  public function SETU() {
    $this->buf = $this->ReadU();
  }

  public function GETU() {
    $this->SendSTART();
    $this->SendU($this->buf);
    $this->SendRETURN();
    $this->buf = NULL;
  }

}

$ctx = new Server();

try {
  $ctx->LinkCreate($argv);
  $ctx->LogC("test",1,"this is the log test\n");
  $ctx->ProcessEvent(MqS::WAIT_FOREVER);
} catch (Exception $e) {
  $ctx->ErrorSet($e);
}
$ctx->Exit();
?>
