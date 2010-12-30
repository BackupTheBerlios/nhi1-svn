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


$stderr = fopen("php://stderr", "w");

class Client extends MqS implements iBgError, iFactory {
  public function __construct() {
    $this->ConfigSetSrvName("test-server");
    parent::__construct();
  }
  public function Factory() {
    return new Client();
  }
  public function LinkCreate($debug) {
    $this->ConfigSetDebug($debug);
    parent::LinkCreate("client", "@", "SELF");
  }
  public function BgError() {
    $master = $this->SlaveGetMaster();
    if ($master != NULL) {
      $master->ErrorC("BGERROR", $this->ErrorGetNum(), $this->ErrorGetText());
      $master->SendERROR();
    }
  }
}

class ClientERR extends MqS {
  public function __construct() {
    $this->ConfigSetSrvName("test-server");
    $this->ConfigSetName("test-client");
    parent::__construct();
  }
  public function LinkCreate($debug) {
    $this->ConfigSetDebug($debug);
    parent::LinkCreate("client", "@", "SELF");
  }
}

class ClientERR2 extends MqS {
  public function __construct() {
    $this->ConfigSetName("cl-err-1");
    parent::__construct();
  }
  public function LinkCreate($debug) {
    $this->ConfigSetDebug($debug);
    parent::LinkCreate("client", "@", "DUMMY");
  }
}

class Server extends MqS implements iServerSetup, iServerCleanup, iFactory {

  public $cl = array();
  public $i  = NULL;

  public function __construct() {
    $this->ConfigSetName("server");
    $this->FactoryCtxIdentSet("test-server");
    parent::__construct();
  }

  public function Factory() {
    return new Server();
  }

  public function ServerCleanup() {
    for ($i=0;$i<2;$i++) {
      if ($this->cl[$i] != NULL) {
        $this->cl[$i]->Delete();
        $this->cl[$i] = NULL;
      }
    }
  }

  public function ServerSetup() {

    if ($this->SlaveIs() == false) {

      # initialize objects
      $this->cl = array(new Client(), new Client(), new Client());
      for ($i=0;$i<2;$i++) {
        $this->cl[$i]->ConfigSetName("cl-" . $i);
        $this->cl[$i]->ConfigSetSrvName("sv-" . $i);
      }

      # add "master" services here
      $this->ServiceCreate("SETU", array(&$this, 'SETU'));
      $this->ServiceCreate("GETU", array(&$this, 'GETU'));

      $this->ServiceCreate("PRNT", array(&$this, 'PRNT'));
      $this->ServiceCreate("TRNS", array(&$this, 'TRNS'));
      $this->ServiceCreate("TRN2", array(&$this, 'TRN2'));
      $this->ServiceCreate("GTTO", array(&$this, 'GTTO'));
      $this->ServiceCreate("MSQT", array(&$this, 'MSQT'));
      $this->ServiceCreate("CNFG", array(&$this, 'CNFG'));
      $this->ServiceCreate("SND1", array(&$this, 'SND1'));
      $this->ServiceCreate("SND2", array(&$this, 'SND2'));
      $this->ServiceCreate("REDI", array(&$this, 'REDI'));
      $this->ServiceCreate("GTCX", array(&$this, 'GTCX'));
      $this->ServiceCreate("CSV1", array(&$this, 'CSV1'));
      $this->ServiceCreate("SLEP", array(&$this, 'SLEP'));
      $this->ServiceCreate("USLP", array(&$this, 'USLP'));
      $this->ServiceCreate("CFG1", array(&$this, 'CFG1'));
      $this->ServiceCreate("INIT", array(&$this, 'INITX'));
      $this->ServiceCreate("LST1", array(&$this, 'LST1'));
      $this->ServiceCreate("LST2", array(&$this, 'LST2'));

      $this->ServiceCreate("BUF1", array(&$this, 'BUF1'));
      $this->ServiceCreate("BUF2", array(&$this, 'BUF2'));
      $this->ServiceCreate("BUF3", array(&$this, 'BUF3'));

      $this->ServiceCreate("ERR2", array(&$this, 'ERRX'));
      $this->ServiceCreate("ERR3", array(&$this, 'ERRX'));
      $this->ServiceCreate("ERR4", array(&$this, 'ERRX'));
      $this->ServiceCreate("ERR5", array(&$this, 'ERRX'));
      $this->ServiceCreate("ERR6", array(&$this, 'ERRX'));
      $this->ServiceCreate("ERRT", array(&$this, 'ERRT'));

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
      $this->ServiceCreate("ECON", array(&$this, 'ECON'));
      $this->ServiceCreate("ECOL", array(&$this, 'ECOL'));
      $this->ServiceCreate("ECLI", array(&$this, 'ECLI'));
      $this->ServiceCreate("ERLR", array(&$this, 'ERLR'));
      $this->ServiceCreate("ERLS", array(&$this, 'ERLS'));
      $this->ServiceCreate("ECUL", array(&$this, 'ECUL'));
      $this->ServiceCreate("RDUL", array(&$this, 'RDUL'));
    }
  }

  public function RDUL() {
    $this->ReadY();
    $this->ReadS();
    $this->ReadI();
    $this->ReadW();
    $this->ReadU();
  }

  public function ECUL() {
    $this->SendSTART();
    $this->SendY($this->ReadY());
    $this->SendS($this->ReadS());
    $this->SendI($this->ReadI());
    $this->SendW($this->ReadW());
    $this->ReadProxy($this);
    $this->SendRETURN();
  }

  public function CNFG() {
    $this->SendSTART();
    $this->SendO($this->ConfigGetIsServer());
    $this->SendO($this->LinkIsParent());
    $this->SendO($this->SlaveIs());
    $this->SendO($this->ConfigGetIsString());
    $this->SendO($this->ConfigGetIsSilent());
    $this->SendO($this->LinkIsConnected());
    $this->SendC($this->ConfigGetName());
    $this->SendI($this->ConfigGetDebug());
    $this->SendI($this->LinkGetCtxId());
    $this->SendC($this->ServiceGetToken());
    $this->SendRETURN();
  }

  public function Callback2($ctx) {
    $this->i = $ctx->ReadI();
  }

  public function SND2() {
    $s  = $this->ReadC();
    $id = $this->ReadI();
    $cl = $this->SlaveGet($id);
    $this->SendSTART();
    switch ($s) {
      case "CREATE":
        $args = array();
        while ($this->ReadItemExists()) {
          $args[] = ReadC();
        }
        array_push(&$args, "--name", "wk-cl-" . $id, "@", "--name", "wk-sv-" . $id);
        $this->SlaveWorker($id, $args);
	break;
      case "CREATE2":
        $slv = new Client();
        $slv->LinkCreate($this->ConfigGetDebug());
        $this->SlaveCreate($id, $slv);
	break;
      case "CREATE3":
        $slv = new ClientERR();
        $slv->LinkCreate($this->ConfigGetDebug());
        $this->SlaveCreate($id, $slv);
	break;
      case "DELETE":
        $this->SlaveDelete($id);
        $this->SlaveGet($id) == NULL ? $this->SendC("OK") : $this->SendC("ERROR");
	break;
      case "SEND":
        $cl->SendSTART();
        $tok = $this->ReadC();
        $cl->SendU($this->ReadU());
        $cl->SendEND($tok);
	break;
      case "WAIT":
        $cl->SendSTART();
        $cl->SendN($this->ReadN());
        $cl->SendEND_AND_WAIT("ECOI", 5);
        $this->SendI($cl->ReadI()+1);
	break;
      case "CALLBACK":
        $cl->SendSTART();
        $cl->SendU($this->ReadU());
        $this->i = -1;
        $cl->SendEND_AND_CALLBACK("ECOI", array(&$this, "Callback2"));
        $cl->ProcessEvent(10,MqS::WAIT_ONCE);
        $this->SendI($this->i+1);
	break;
      case "MqSendEND_AND_WAIT":
        $tok = $this->ReadC();
        $cl->SendSTART();
        while ($this->ReadItemExists()) {
          $cl->SendU($this->ReadU());
        }
        $cl->SendEND_AND_WAIT($tok, 5);
        while ($cl->ReadItemExists()) {
          $this->SendU($cl->ReadU());
        }
	break;
      case "MqSendEND":
        $tok = $this->ReadC();
        $cl->SendSTART();
        while ($this->ReadItemExists()) {
          $cl->SendU($this->ReadU());
        }
        $cl->SendEND($tok);
        return;
      case "ERR-1":
        $slv = new ClientERR2();
        $slv->LinkCreate($this->ConfigGetDebug());
	break;
      case "isSlave":
        $this->SendO($cl->SlaveIs());
	break;
    }
    $this->SendRETURN();
  }

  public function GTCX() {
    $this->SendSTART();
    $this->SendI($this->LinkGetCtxId());
    $this->SendRETURN();
  }

  public function REDI() {
    $this->i = $this->ReadI();
  }

  public function Callback($ctx) {
    $this->i = $ctx->ReadI();
  }

  public function SND1() {
    global $stderr;
    $s = $this->ReadC();
    $id = $this->ReadI();
    $this->SendSTART();
    switch ($s) {
      case "START":
        $parent = $this->LinkGetParent();
        if ($parent != NULL and $parent->cl[$id]->LinkIsConnected()) {
#fwrite($stderr,"111111111111111111111111111111\n");
#fwrite($stderr,"THIS\n");
#var_dump($this->cl[$id]);
          $this->cl[$id]->LinkCreateChild($parent->cl[$id]);
        } else {
          $this->cl[$id]->LinkCreate($this->ConfigGetDebug());
        }
	break;
      case "START2":
        $this->cl[$id]->LinkCreate($this->ConfigGetDebug());
        $this->cl[$id]->LinkCreate($this->ConfigGetDebug());
	break;
      case "START3":
        $parent = new Client();
        $this->cl[$id]->LinkCreateChild($parent);
	break;
      case "START4":
        $this->cl[$id]->SlaveWorker(0);
	break;
      case "START5":
        $this->SlaveWorker($id, "--name", "wk-cl-" . $id, "--srvname", "wk-sv-" . $id, "--spawn");
	break;
      case "STOP":
        $this->cl[$id]->LinkDelete();
	break;
      case "SEND":
        $this->cl[$id]->SendSTART();
        $tok = $this->ReadC();
        $this->cl[$id]->SendU($this->ReadU());
        $this->cl[$id]->SendEND($tok);
	break;
      case "WAIT":
        $this->cl[$id]->SendSTART();
        $this->ReadProxy($this->cl[$id]);
        $this->cl[$id]->SendEND_AND_WAIT("ECOI", 5);
        $this->SendI($this->cl[$id]->ReadI()+1);
	break;
      case "CALLBACK":
        $this->cl[$id]->SendSTART();
        $this->cl[$id]->SendU($this->ReadU());
        $this->i = -1;
        $this->cl[$id]->SendEND_AND_CALLBACK("ECOI", array(&$this, 'Callback'));
        $this->cl[$id]->ProcessEvent(10,MqS::WAIT_ONCE);
        $this->SendI($this->i+1);
	break;
      case "ERR-1":
        $this->cl[$id]->SendSTART();
        try {
          $this->ReadProxy($this->cl[$id]);
          $this->cl[$id]->SendEND_AND_WAIT("ECOI", 5);
        } catch (Exception $ex) {
          $this->ErrorSet($ex);
          $this->SendI($this->ErrorGetNum());
          $this->SendC($this->ErrorGetText());
          $this->ErrorReset();
        }
	break;
    }
    $this->SendRETURN();
  }

  public function PRNT() {
    $this->SendSTART();
    $this->SendC($this->LinkGetCtxId() . " - " . $this->ReadC());
    $this->SendEND_AND_WAIT("WRIT");
    $this->SendRETURN();
  }

  public function TRN2() {
    $this->ReadT_START();
    $this->i = $this->ReadI();
    $this->ReadT_END();
    $this->j = $this->ReadI();
  }

  public function TRNS() {
    $this->SendSTART();
    $this->SendT_START("TRN2");
    $this->SendI(9876);
    $this->SendT_END();
    $this->SendI($this->ReadI());
    $this->SendEND_AND_WAIT("ECOI");
    $this->ProcessEvent(MqS::WAIT_ONCE);
    $this->SendSTART();
    $this->SendI($this->i);
    $this->SendI($this->j);
    $this->SendRETURN();
  }

  public function GTTO() {
    $this->SendSTART();
    $this->SendC($this->ServiceGetToken());
    $this->SendRETURN();
  }

  public function MSQT() {
    $this->SendSTART();
    $debug = $this->ConfigGetDebug();
    if ($debug != 0) {
      $this->SendC("debug");
      $this->SendI($debug);
    }
    if (!$this->ConfigGetIsString()) $this->SendC("binary");
    if ($this->ConfigGetIsSilent())  $this->SendC("silent");
    $this->SendC("sOc");
    $this->ConfigGetIsServer() ? $this->SendC("SERVER") : $this->SendC("CLIENT");
    $this->SendC("pOc");
    $this->LinkIsParent() ? $this->SendC("PARENT") : $this->SendC("CHILD");
    $this->SendRETURN();
  }

  public function ECON() {
    $this->SendSTART();
    $this->SendC($this->ReadC() . "-" . $this->ConfigGetName());
    $this->SendRETURN();
  }

  public function ERLS() {
    $this->SendSTART();
    $this->SendL_START();
    $this->SendU($this->ReadU());
    $this->SendL_START();
    $this->SendU($this->ReadU());
    $this->SendRETURN();
  }

  public function ERLR() {
    $this->SendSTART();
    $this->ReadL_START();
    $this->ReadL_START();
    $this->SendRETURN();
  }

  private function EchoList($doincr) {
    while ($this->ReadItemExists()) {
      $buf = $this->ReadU();
      if ($buf->GetType() == "L") {
        $this->ReadL_START($buf);
        $this->SendL_START();
        $this->EchoList($doincr);
        $this->SendL_END();
        $this->ReadL_END();
      } else if ($doincr == TRUE) {
        $this->SendI($buf->GetI()+1);
      } else {
        $this->SendU($buf);
      }
    }
  }

  public function ECLI() {
    $opt = $this->ReadU();
    $doincr = ($opt->GetType() == "C" && $opt->GetC() == "--incr");
    if ($doincr == FALSE) $this->ReadUndo();
    $this->SendSTART();
    $this->EchoList($doincr);
    $this->SendRETURN();
  }

  public function ECOL() {
    $this->SendSTART();
    $this->ReadL_START();
    $this->SendL_START();
    $this->EchoList(FALSE);
    $this->SendL_END();
    $this->ReadL_END();
    $this->SendRETURN();
  }

  public function LST1() {
    $this->SendSTART();
    $this->SendL_END();
    $this->SendRETURN();
  }

  public function LST2() {
    $this->SendSTART();
    $this->ReadL_END();
    $this->SendRETURN();
  }

  public function INITX() {
    $this->SendSTART();
    $list = array();
    while ($this->ReadItemExists()) {
      $list[] = $this->ReadC();
    }
    MqS::Init($list);
    $this->SendRETURN();
  }

  public function ERRT() {
    $this->SendSTART();
    $this->ErrorC("MYERR", 9, $this->ReadC());
    $this->SendERROR();
  }

  public function ERRX() {
    $this->SendSTART();
    switch ($this->ServiceGetToken()) {
      case "ERR2":
        $this->SendC("some data");
        $this->ErrorC("Ot_ERR2", 10, "some error");
	break;
      case "ERR3":;
        $this->SendRETURN();
	break;
      case "ERR4":
        exit(1);
	break;
      case "ERR5":
        $this->ReadProxy($this->ReadU());
	break;
      case "ERR6":
        $this->SendU(self);
	break;
    }
    $this->SendRETURN();
  }

  public function CFG1() {
    $cmd = $this->ReadC();
    $this->SendSTART();
    switch ($cmd) {
      case "Buffersize":
        $old = $this->ConfigGetBuffersize();
        $this->ConfigSetBuffersize($this->ReadI());
        $this->SendI($this->ConfigGetBuffersize());
        $this->ConfigSetBuffersize($old);
	break;
      case "Debug":
        $old = $this->ConfigGetDebug();
        $this->ConfigSetDebug($this->ReadI());
        $this->SendI($this->ConfigGetDebug());
        $this->ConfigSetDebug($old);
	break;
      case "Timeout":
        $old = $this->ConfigGetTimeout();
        $this->ConfigSetTimeout($this->ReadW());
        $this->SendW($this->ConfigGetTimeout());
        $this->ConfigSetTimeout($old);
	break;
      case "Name":
        $old = $this->ConfigGetName();
        $this->ConfigSetName($this->ReadC());
        $this->SendC($this->ConfigGetName());
        $this->ConfigSetName($old);
	break;
      case "SrvName":
        $old = $this->ConfigGetSrvName();
        $this->ConfigSetSrvName($this->ReadC());
        $this->SendC($this->ConfigGetSrvName());
        $this->ConfigSetSrvName($old);
	break;
      case "Ident":
        $old = $this->FactoryCtxIdentGet();
        $this->FactoryCtxIdentSet($this->ReadC());
        $check = $this->LinkGetTargetIdent() == $this->ReadC();
        $this->SendSTART();
        $this->SendC($this->FactoryCtxIdentGet());
        $this->SendO($check);;
        $this->FactoryCtxIdentSet($old);
	break;
      case "IsSilent":
        $old = $this->ConfigGetIsSilent();
        $this->ConfigSetIsSilent($this->ReadO());
        $this->SendO($this->ConfigGetIsSilent());
        $this->ConfigSetIsSilent($old);
	break;
      case "IsString":
        $old = $this->ConfigGetIsString();
        $this->ConfigSetIsString($this->ReadO());
        $this->SendO($this->ConfigGetIsString());
        $this->ConfigSetIsString($old);
	break;
      case "IoUds":
        $old = $this->ConfigGetIoUdsFile();
        $this->ConfigSetIoUdsFile($this->ReadC());
        $this->SendC($this->ConfigGetIoUdsFile());
        $this->ConfigSetIoUdsFile($old);
	break;
      case "IoTcp":
        $h  = $this->ConfigGetIoTcpHost();
        $p  = $this->ConfigGetIoTcpPort();
        $mh = $this->ConfigGetIoTcpMyHost();
        $mp = $this->ConfigGetIoTcpMyPort();
        $hv = $this->ReadC();
        $pv = $this->ReadC();
        $mhv = $this->ReadC();
        $mpv = $this->ReadC();
        $this->ConfigSetIoTcp($hv,$pv,$mhv,$mpv);
        $this->SendC($this->ConfigGetIoTcpHost());
        $this->SendC($this->ConfigGetIoTcpPort());
        $this->SendC($this->ConfigGetIoTcpMyHost());
        $this->SendC($this->ConfigGetIoTcpMyPort());
        $this->ConfigSetIoTcp($h,$p,$mh,$mp);
	break;
      case "IoPipe":
        $old = $this->ConfigGetIoPipeSocket();
        $this->ConfigSetIoPipeSocket($this->ReadI());
        $this->SendI($this->ConfigGetIoPipeSocket());
        $this->ConfigSetIoPipeSocket($old);
	break;
      case "StartAs":
        $old = $this->ConfigGetStartAs();
        $this->ConfigSetStartAs($this->ReadI());
        $this->SendI($this->ConfigGetStartAs());
        $this->ConfigSetStartAs($old);
	break;
      default:
        ErrorC("CFG1", 1, "invalid command: " + cmd);
    }
    $this->SendRETURN();
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


