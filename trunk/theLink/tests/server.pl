#+
#§  \file       theLink/tests/server.pl
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

use strict;
use Switch;
use Net::PerlMsgque;

$| = 1;

package Client;
use base qw(Net::PerlMsgque::MqS);

  sub LinkCreate {
    my $ctx = shift;
    my $debug = shift;
    $ctx->ConfigSetDebug($debug);
    $ctx->SUPER::LinkCreate("@", "SELF", "--name", "test-server");
  }

  sub ECOI_CB {
    my $ctx = shift;
    $ctx->DictSet("i", $ctx->ReadI());
  }

  sub BgError {
    my $ctx = shift;
    my $master = $ctx->SlaveGetMaster();
    if (defined($master)) {
      $master->ErrorC ("BGERROR", $ctx->ErrorGetNum(), $ctx->ErrorGetText());
      $master->SendERROR ();
    }
  }

  sub new {
    my $class = shift;
    my $ctx = $class->SUPER::new(@_);
    $ctx->ConfigSetBgError(\&BgError);
    $ctx->ConfigSetFactory(sub {new Client()});
    return $ctx;
  }

package ClientERR;
use base qw(Net::PerlMsgque::MqS);

  sub LinkCreate {
    my $ctx = shift;
    $ctx->ConfigSetDebug(shift);
    $ctx->ConfigSetName("test-client");
    $ctx->ConfigSetSrvName("test-server");
    $ctx->SUPER::LinkCreate("@", "SELF");
  }

  sub ECOI_CB {
    my $ctx = shift;
    $ctx->DictSet("i", $ctx->ReadI());
  }

package ClientERR2;
use base qw(Net::PerlMsgque::MqS);

  sub LinkCreate {
    my $ctx = shift;
    $ctx->ConfigSetDebug(shift);
    $ctx->ConfigSetName("cl-err-1");
    $ctx->SUPER::LinkCreate("@", "DUMMY");
  }

package Server;
use base qw(Net::PerlMsgque::MqS);

  sub ECOO {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendO($ctx->ReadO());
    $ctx->SendRETURN();
  }

  sub ECOY {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendY($ctx->ReadY());
    $ctx->SendRETURN();
  }

  sub ECOS {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendS($ctx->ReadS());
    $ctx->SendRETURN();
  }

  sub ECOI {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendI($ctx->ReadI());
    $ctx->SendRETURN();
  }

  sub ECOW {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendW($ctx->ReadW());
    $ctx->SendRETURN();
  }

  sub ECOF {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendF($ctx->ReadF());
    $ctx->SendRETURN();
  }

  sub ECOD {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendD($ctx->ReadD());
    $ctx->SendRETURN();
  }

  sub ECOC {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendC($ctx->ReadC());
    $ctx->SendRETURN();
  }

  sub ECOB {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendB($ctx->ReadB());
    $ctx->SendRETURN();
  }

  sub ECOU {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendU($ctx->ReadU());
    $ctx->SendRETURN();
  }

  sub EchoList {
    my $ctx = shift;
    my $doincr = shift;
    my $buf;
    my $code;
    while ($ctx->ReadItemExists()) {
      $buf = $ctx->ReadU();
      if ($buf->GetType() eq 'L') {
	$ctx->ReadL_START ($buf);
	$ctx->SendL_START ();
	$ctx->EchoList ($doincr);
	$ctx->SendL_END ();
	$ctx->ReadL_END ();
      } elsif ($doincr) {
	$ctx->SendI ($buf->GetI()+1);
      } else {
	$ctx->SendU ($buf);
      }
    }
  }

  sub ECLI {
    my $ctx = shift;
    my $opt = $ctx->ReadU();
    my $doincr = ($opt->GetType() eq 'C' && $opt->GetC() eq "--incr");
    if (!$doincr) {
      $ctx->ReadUndo();
    }
    $ctx->SendSTART();
    $ctx->EchoList ($doincr);
    $ctx->SendRETURN
  }

  sub ECOL {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->ReadL_START();
    $ctx->SendL_START();
    $ctx->EchoList (0);
    $ctx->SendL_END();
    $ctx->ReadL_END();
    $ctx->SendRETURN();
  }

  sub LST1 {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendL_END();
    $ctx->SendRETURN();
  }

  sub LST2 {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->ReadL_END();
    $ctx->SendRETURN();
  }

  sub ECON {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendC($ctx->ReadC() . "-" . $ctx->ConfigGetName() );
    $ctx->SendRETURN();
  }

  sub ECUL {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendY ($ctx->ReadY());
    $ctx->SendS ($ctx->ReadS());
    $ctx->SendI ($ctx->ReadI());
    $ctx->SendW ($ctx->ReadW());
    $ctx->ReadProxy ($ctx);
    $ctx->SendRETURN();
  }

  sub RDUL {
    my $ctx = shift;
    $ctx->ReadY ();
    $ctx->ReadS ();
    $ctx->ReadI ();
    $ctx->ReadW ();
    $ctx->ReadU ();
  }

  sub SETU {
    my $ctx = shift;
    my $ret = $ctx->DictSet ("buf", $ctx->ReadU ());
  }

  sub GETU {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendU ($ctx->DictGet ("buf"));
    $ctx->SendRETURN();
  }

  sub GTTO {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendC ($ctx->ServiceGetToken());
    $ctx->SendRETURN();
  }

  sub BUF1 {
    my $ctx = shift;
    my $buf = $ctx->ReadU();
    my $typ = $buf->GetType();
    $ctx->SendSTART();
    $ctx->SendC ($typ);
    switch ($typ) {
      case "Y" { $ctx->SendY ($buf->GetY) }
      case "O" { $ctx->SendO ($buf->GetO) }
      case "S" { $ctx->SendS ($buf->GetS) }
      case "I" { $ctx->SendI ($buf->GetI) }
      case "F" { $ctx->SendF ($buf->GetF) }
      case "W" { $ctx->SendW ($buf->GetW) }
      case "D" { $ctx->SendD ($buf->GetD) }
      case "C" { $ctx->SendC ($buf->GetC) }
      case "B" { $ctx->SendB ($buf->GetB) }
    }
    $ctx->SendRETURN();
  }

  sub BUF2 {
    my $ctx = shift;
    my $buf;
    $ctx->SendSTART();
    foreach my $i (1 .. 3) {
      $buf = $ctx->ReadU();
      $ctx->SendC ($buf->GetType());
      $ctx->SendU ($buf);
    }
    $ctx->SendRETURN();
  }

  sub BUF3 {
    my $ctx = shift;
    my $buf = $ctx->ReadU();
    $ctx->SendSTART();
    $ctx->SendC ($buf->GetType());
    $ctx->SendU ($buf);
    $ctx->SendI ($ctx->ReadI());
    $ctx->SendU ($buf);
    $ctx->SendRETURN();
  }

  sub ERR2 {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->ErrorC ("Ot_ERR2", 10, "some error");
    $ctx->SendRETURN();
  }

  sub ERR3 {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendRETURN();
    $ctx->SendRETURN();
  }

  sub ERR4 {
    my $ctx = shift;
    exit 1;
  }

  sub ERR5 {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->ReadProxy($ctx->ReadU);
    $ctx->SendRETURN();
  }

  sub ERR6 {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendU($ctx);
    $ctx->SendRETURN();
  }

  sub ERLR {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->ReadL_START();
    $ctx->ReadL_START();
    $ctx->SendRETURN();
  }

  sub ERLS {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendL_START();
    $ctx->SendU ($ctx->ReadU());
    $ctx->SendL_START();
    $ctx->SendU ($ctx->ReadU());
    $ctx->SendRETURN();
  }

  sub SLEP {
    my $ctx = shift;
    $ctx->SendSTART();
    sleep ($ctx->ReadI());
    $ctx->SendRETURN();
  }

  sub USLP {
    my $ctx = shift;
    my $i = $ctx->ReadI();
    $ctx->SendSTART();
    $ctx->SysUSleep($i);
    $ctx->SendI($i);
    $ctx->SendRETURN();
  }

  sub CSV1 {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendI ($ctx->ReadI()+1);
    $ctx->SendEND_AND_WAIT("CSV2");
    $ctx->SendSTART();
    $ctx->SendI ($ctx->ReadI()+1);
    $ctx->SendRETURN();
  }

  sub xINIT {
    my $ctx = shift;
    my $max = $ctx->ReadGetNumItems();
    my @list = ();
    $ctx->SendSTART();
    for (my $i=0; $i<$max; $i++) {
      push @list, $ctx->ReadC();
    }
    Net::PerlMsgque::Init(@list);
    $ctx->SendRETURN();
  }

  sub CNFG {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendO($ctx->ConfigGetIsServer());
    $ctx->SendO($ctx->LinkIsParent());
    $ctx->SendO($ctx->SlaveIs());
    $ctx->SendO($ctx->ConfigGetIsString());
    $ctx->SendO($ctx->ConfigGetIsSilent());
    $ctx->SendO($ctx->LinkIsConnected());
    $ctx->SendC($ctx->ConfigGetName());
    $ctx->SendI($ctx->ConfigGetDebug());
    $ctx->SendI($ctx->LinkGetCtxId());
    $ctx->SendC($ctx->ServiceGetToken());
    $ctx->SendRETURN();
  }

  sub MSQT {
    my $ctx = shift;
    $ctx->SendSTART();
    if ($ctx->ConfigGetDebug()) {
      $ctx->SendC ("debug");
      $ctx->SendI ($ctx->ConfigGetDebug());
    }
    if (!$ctx->ConfigGetIsString()) {
      $ctx->SendC ("binary");
    }
    if ($ctx->ConfigGetIsSilent()) {
      $ctx->SendC ("silent");
    }
    $ctx->SendC ("sOc");
    $ctx->SendC ($ctx->ConfigGetIsServer() ? "SERVER" : "CLIENT");
    $ctx->SendC ("pOc");
    $ctx->SendC ($ctx->LinkIsParent() ? "PARENT" : "CHILD");
    $ctx->SendRETURN();
  }

  sub REDI {
    my $ctx = shift;
    $ctx->ReadI ();
  }

  sub GTCX {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendI($ctx->LinkGetCtxId());
    $ctx->SendRETURN();
  }

  sub SND1 {
    my $ctx = shift;
    my $s = $ctx->ReadC();
    my $id = $ctx->ReadI();
    my $cl = $ctx->DictGet($id);

    $ctx->SendSTART();
      switch ($s) {
	case "START" {
	  my $parent = $ctx->LinkGetParent();
	  if (defined($parent) && $parent->DictGet($id)->LinkIsConnected()) {
	    $cl->LinkCreateChild($parent->DictGet($id));
	  } else {
	    $cl->LinkCreate($ctx->ConfigGetDebug());
	  }
	}
	case "START2" {
	  # object already created ERROR
	  $cl->LinkCreate($ctx->ConfigGetDebug());
	  $cl->LinkCreate($ctx->ConfigGetDebug());
	}
	case "START3" {
	  my $parent = new Client();
	  # parent not connected ERROR
	  $cl->LinkCreateChild($parent);
	}
	case "START4" {
	  # master not connected ERROR
	  $cl->SlaveWorker(0);
	}
	case "START5" {
	  # the 'master' have to be a 'parent' without 'child' objects
	  # 'slave' identifer out of range (0 <= 10000000 <= 1023)
	  $ctx->SlaveWorker($id, "--name", "wk-cl-$id", "--srvname", "wk-sv-$id", "--pipe");
	}
	case "STOP" {
	  $cl->LinkDelete();
	}
	case "SEND" {
	  my $TOK = $ctx->ReadC();
	  $cl->SendSTART();
	  $ctx->ReadProxy($cl);
	  $cl->SendEND($TOK);
	}
	case "WAIT" {
	  $cl->SendSTART();
	  $ctx->ReadProxy($cl);
	  $cl->SendEND_AND_WAIT("ECOI", 5);
	  $ctx->SendI($cl->ReadI()+1);
	}
	case "CALLBACK" {
	  $cl->DictSet("i", -1);
	  $cl->SendSTART();
	  $ctx->ReadProxy($cl);
	  $cl->SendEND_AND_CALLBACK("ECOI", \&Client::ECOI_CB);
	  $cl->ProcessEvent({timeout => 10, wait => "ONCE"});
	  $ctx->SendI($cl->DictGet("i")+1);
	}
	case "ERR-1" {
	  $cl->SendSTART();
	  eval {
	    $ctx->ReadProxy($cl);
	    $cl->SendEND_AND_WAIT("ECOI", 5);
	  };
	  if ($@) {
	    $ctx->ErrorSet ($@);
	    $ctx->SendI($ctx->ErrorGetNum());
	    $ctx->SendC($ctx->ErrorGetText());
	    $ctx->ErrorReset();
	  }
	}
      }
    $ctx->SendRETURN();
  }

  sub SND2 {
    my $ctx = shift;
    my $s  = $ctx->ReadC();
    my $id = $ctx->ReadI();
    my $cl = $ctx->SlaveGet($id);

    $ctx->SendSTART();
    switch ($s) {
      case "CREATE" {
	my @LIST = ();
	while ($ctx->ReadItemExists()) {
	  push (@LIST, $ctx->ReadC());
	}
	push (@LIST, "--name", "wk-cl-" . $id , "@", "--name", "wk-sv-" . $id);
	$ctx->SlaveWorker($id, @LIST);
      } 
      case "CREATE2" {
	my $c = new Client();
	$c->LinkCreate($ctx->ConfigGetDebug());
	$ctx->SlaveCreate ($id, $c);
      } 
      case "CREATE3" {
	my $c = new ClientERR();
	$c->LinkCreate($ctx->ConfigGetDebug());
	$ctx->SlaveCreate ($id, $c);
      } 
      case "DELETE" {
	$ctx->SlaveDelete($id);
	$ctx->SendC(defined($ctx->SlaveGet($id)) ? "ERROR" : "OK");
      } 
      case "SEND" {
	my $TOK = $ctx->ReadC();
	$cl->SendSTART();
	$ctx->ReadProxy($cl);
	$cl->SendEND($TOK);
      } 
      case "WAIT" {
	$cl->SendSTART();
	$cl->SendN($ctx->ReadN());
	$cl->SendEND_AND_WAIT("ECOI", 5);
	$ctx->SendI($cl->ReadI()+1);
      } 
      case "CALLBACK" {
	$cl->SendSTART();
	$ctx->ReadProxy($cl);
	$cl->DictSet ("i", -1);
	$cl->SendEND_AND_CALLBACK("ECOI", \&Client::ECOI_CB);
	$cl->ProcessEvent({timeout => 10, wait => "ONCE"});
	$ctx->SendI($cl->DictGet ("i")+1);
      } 
      case "MqSendEND_AND_WAIT" {
	my $TOK = $ctx->ReadC();
	$cl->SendSTART();
	while ($ctx->ReadItemExists()) {
	  $ctx->ReadProxy($cl);
	}
	$cl->SendEND_AND_WAIT($TOK, 5);
	while ($cl->ReadItemExists()) {
	  $cl->ReadProxy($ctx);
	}
      } 
      case "MqSendEND" {
	my $TOK = $ctx->ReadC();
	$cl->SendSTART();
	while ($ctx->ReadItemExists()) {
	  $ctx->ReadProxy($cl);
	}
	$cl->SendEND($TOK);
	return;
      } 
      case "ERR-1" {
	my $c = new ClientERR2();
	$c->LinkCreate($ctx->ConfigGetDebug());
      } 
      case "isSlave" {
	$ctx->SendO($cl->SlaveIs());
      }
    }
    $ctx->SendRETURN();
  }

  sub CFG1 {
    my $ctx = shift;
    my $cmd = $ctx->ReadC();

    $ctx->SendSTART();
    switch ($cmd) {
      case "Buffersize" {
	my $old = $ctx->ConfigGetBuffersize();
	$ctx->ConfigSetBuffersize ($ctx->ReadI());
	$ctx->SendI ($ctx->ConfigGetBuffersize());
	$ctx->ConfigSetBuffersize ($old);
      }
      case "Debug" {
	my $old = $ctx->ConfigGetDebug();
	$ctx->ConfigSetDebug ($ctx->ReadI());
	$ctx->SendI ($ctx->ConfigGetDebug());
	$ctx->ConfigSetDebug ($old);
      }
      case "Timeout" {
	my $old = $ctx->ConfigGetTimeout();
	$ctx->ConfigSetTimeout ($ctx->ReadW());
	$ctx->SendW ($ctx->ConfigGetTimeout());
	$ctx->ConfigSetTimeout ($old);
      }
      case "Name" {
	my $old = $ctx->ConfigGetName();
	$ctx->ConfigSetName ($ctx->ReadC());
	$ctx->SendC ($ctx->ConfigGetName());
	$ctx->ConfigSetName ($old);
      }
      case "SrvName" {
	my $old = $ctx->ConfigGetSrvName();
	$ctx->ConfigSetSrvName ($ctx->ReadC());
	$ctx->SendC ($ctx->ConfigGetSrvName());
	$ctx->ConfigSetSrvName ($old);
      }
      case "Ident" {
	my $old = $ctx->ConfigGetSrvName();
	$ctx->ConfigSetSrvName ($ctx->ReadC());
	my $check = ($ctx->LinkGetTargetIdent eq $ctx->ReadC());
	$ctx->SendSTART();
	$ctx->SendC ($ctx->ConfigGetSrvName());
	$ctx->SendO ($check);
	$ctx->ConfigSetSrvName ($old);
      }
      case "IsSilent" {
	my $old = $ctx->ConfigGetIsSilent();
	$ctx->ConfigSetIsSilent ($ctx->ReadO());
	$ctx->SendO ($ctx->ConfigGetIsSilent());
	$ctx->ConfigSetIsSilent ($old);
      }
      case "IsString" {
	my $old = $ctx->ConfigGetIsString();
	$ctx->ConfigSetIsString ($ctx->ReadO());
	$ctx->SendO ($ctx->ConfigGetIsString());
	$ctx->ConfigSetIsString ($old);
      }
      case "IoUds" {
	my $old = $ctx->ConfigGetIoUdsFile();
	$ctx->ConfigSetIoUds ($ctx->ReadC());
	$ctx->SendC ($ctx->ConfigGetIoUdsFile());
	$ctx->ConfigSetIoUds ($old);
      }
      case "IoTcp" {
	my $h  = $ctx->ConfigGetIoTcpHost   ();
	my $p  = $ctx->ConfigGetIoTcpPort   ();
	my $mh = $ctx->ConfigGetIoTcpMyHost ();
	my $mp = $ctx->ConfigGetIoTcpMyPort ();
	my $hv = $ctx->ReadC();
	my $pv = $ctx->ReadC();
	my $mhv = $ctx->ReadC();
	my $mpv = $ctx->ReadC();
	$ctx->ConfigSetIoTcp ($hv,$pv,$mhv,$mpv);
	$ctx->SendC ($ctx->ConfigGetIoTcpHost());
	$ctx->SendC ($ctx->ConfigGetIoTcpPort());
	$ctx->SendC ($ctx->ConfigGetIoTcpMyHost());
	$ctx->SendC ($ctx->ConfigGetIoTcpMyPort());
	$ctx->ConfigSetIoTcp ($h,$p,$mh,$mp);
      }
      case "IoPipe" {
	my $old = $ctx->ConfigGetIoPipeSocket();
	$ctx->ConfigSetIoPipe ($ctx->ReadI());
	$ctx->SendI ($ctx->ConfigGetIoPipeSocket());
	$ctx->ConfigSetIoPipe ($old);
      }
      case "StartAs" {
	my $old = $ctx->ConfigGetStartAs();
	$ctx->ConfigSetStartAs ($ctx->ReadI());
	$ctx->SendI ($ctx->ConfigGetStartAs());
	$ctx->ConfigSetStartAs ($old);
      }
      default {
	$ctx->ErrorC ("CFG1", 1, "invalid command: $cmd");
      }
    }
    $ctx->SendRETURN();
  }

  sub PRNT {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendC($ctx->LinkGetCtxId() . " - " . $ctx->ReadC());
    $ctx->SendEND_AND_WAIT("WRIT");
    $ctx->SendRETURN();
  }

  sub TRNS {
    my $ctx = shift;
    $ctx->SendSTART ();
    $ctx->SendT_START ("TRN2");
    $ctx->SendI (9876);
    $ctx->SendT_END ();
    $ctx->SendI ( $ctx->ReadI() );
    $ctx->SendEND_AND_WAIT ("ECOI");
    $ctx->ProcessEvent ({wait => "ONCE"});
    $ctx->SendSTART ();
    $ctx->SendI ($ctx->DictGet("i"));
    $ctx->SendI ($ctx->DictGet("j"));
    $ctx->SendRETURN ();
  }

  sub TRN2 {
    my $ctx = shift;
    $ctx->ReadT_START ();
    $ctx->DictSet ("i", $ctx->ReadI ());
    $ctx->ReadT_END ();
    $ctx->DictSet ("j", $ctx->ReadI ());
    $ctx->SendRETURN ();
  }

  sub ServerCleanup {
    my $ctx = shift;
    my $i;
    #$ctx->Log("ServerCleanup", 0, "1111111111111111\n");
    for ($i=0;$i<3;$i++) {
      if ($ctx->DictExists($i)) {
	#$ctx->Log("ServerCleanup", 0, ">>>>> cl-$i=" . $ctx->DictGet ($i) . "\n");
	undef $ctx->DictUnset($i);
	#$ctx->Log("ServerCleanup", 0, "<<<<<\n");
      }
    }
  }

  sub ServerSetup {
    my $ctx = shift;
    if ($ctx->SlaveIs()) {
      # add "slave" services here
    } else {
      #$ctx->Log("ServerSetup", 0, "22222222222222222\n");
      for (my $i=0;$i<3;$i++) {
	my $cl = new Client();
	$ctx->DictSet ($i, $cl);
	$cl->ConfigSetName("cl-" . $i);
	$cl->ConfigSetSrvName("sv-" . $i);
	#$ctx->Log("ServerSetup", 0, "cl-$i=" . $ctx->DictGet ($i) . "\n");
      }
      # add "master" services here
      $ctx->ServiceCreate("ECOO", \&ECOO);
      $ctx->ServiceCreate("ECOY", \&ECOY);
      $ctx->ServiceCreate("ECOS", \&ECOS);
      $ctx->ServiceCreate("ECOI", \&ECOI);
      $ctx->ServiceCreate("ECOW", \&ECOW);
      $ctx->ServiceCreate("ECOF", \&ECOF);
      $ctx->ServiceCreate("ECOD", \&ECOD);
      $ctx->ServiceCreate("ECOC", \&ECOC);
      $ctx->ServiceCreate("ECOB", \&ECOB);
      $ctx->ServiceCreate("ECOU", \&ECOU);
      $ctx->ServiceCreate("ECOL", \&ECOL);
      $ctx->ServiceCreate("ECLI", \&ECLI);
      $ctx->ServiceCreate("LST1", \&LST1);
      $ctx->ServiceCreate("LST2", \&LST2);
      $ctx->ServiceCreate("ECON", \&ECON);
      $ctx->ServiceCreate("ECUL", \&ECUL);
      $ctx->ServiceCreate("RDUL", \&RDUL);
      $ctx->ServiceCreate("SETU", \&SETU);
      $ctx->ServiceCreate("GETU", \&GETU);
      $ctx->ServiceCreate("GTTO", \&GTTO);
      $ctx->ServiceCreate("BUF1", \&BUF1);
      $ctx->ServiceCreate("BUF2", \&BUF2);
      $ctx->ServiceCreate("BUF3", \&BUF3);
      $ctx->ServiceCreate("ERR2", \&ERR2);
      $ctx->ServiceCreate("ERR3", \&ERR3);
      $ctx->ServiceCreate("ERR4", \&ERR4);
      $ctx->ServiceCreate("ERR5", \&ERR5);
      $ctx->ServiceCreate("ERR6", \&ERR6);
      $ctx->ServiceCreate("MSQT", \&MSQT);
      $ctx->ServiceCreate("ERLR", \&ERLR);
      $ctx->ServiceCreate("ERLS", \&ERLS);
      $ctx->ServiceCreate("SLEP", \&SLEP);
      $ctx->ServiceCreate("USLP", \&USLP);
      $ctx->ServiceCreate("CSV1", \&CSV1);
      $ctx->ServiceCreate("INIT", \&xINIT);
      $ctx->ServiceCreate("CNFG", \&CNFG);
      $ctx->ServiceCreate("SND1", \&SND1);
      $ctx->ServiceCreate("SND2", \&SND2);
      $ctx->ServiceCreate("REDI", \&REDI);
      $ctx->ServiceCreate("GTCX", \&GTCX);
      $ctx->ServiceCreate("CFG1", \&CFG1);
      $ctx->ServiceCreate("PRNT", \&PRNT);
      $ctx->ServiceCreate("TRNS", \&TRNS);
      $ctx->ServiceCreate("TRN2", \&TRN2);
    }
  }

  sub new {
    my $class = shift;
    my $ctx = $class->SUPER::new(@_);
    $ctx->ConfigSetName("server");
    $ctx->ConfigSetServerSetup(\&ServerSetup);
    $ctx->ConfigSetServerCleanup(\&ServerCleanup);
    $ctx->ConfigSetFactory(
      sub {
	new Server()
      }
    );
    return $ctx;
  }

package main;

  our $srv = new Server();

  eval {
    $srv->LinkCreate(@ARGV);
    $srv->ProcessEvent({wait => "FOREVER"});
  };
  if ($@) {
    $srv->ErrorSet($@);
  }
  $srv->Exit();

1;


