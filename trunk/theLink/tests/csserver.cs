/**
 *  \file       theLink/tests/csserver.cs
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

using System;
using csmsgque;
using System.Collections.Generic;

namespace example {

  sealed  class Client : MqS, IBgError {

    public int i;

    void IBgError.BgError () {
      MqS master = SlaveGetMaster();
      if (master != null) {
	master.ErrorC ("BGERROR", ErrorGetNum(), ErrorGetText());
	master.SendERROR ();
      }
    }

    public void LinkCreate (int debug) {
      ConfigSetDebug(debug);
      base.LinkCreate("@", "server", "--name", "test-server");
    }

    public void ECOI_CB () {
      i = ReadI();
    }
  }

  sealed  class ClientERR2 : MqS {

    public void LinkCreate (int debug) {
      ConfigSetDebug(debug);
      ConfigSetName("cl-err-1");
      base.LinkCreate("@", "DUMMY");
    }
  }

  sealed class Server : MqS, IServerSetup, IServerCleanup {

    private Client[] cl = new Client[3];
    private MqBufferS buf = null;

  // ########################################################################

    public Server (MqS tmpl) : base(tmpl) {
    }

    void IServerCleanup.ServerCleanup () {
      for (int i=0;i<3;i++) {
	if (cl[i] == null) continue;
	cl[i].LinkDelete();
	cl[i] = null;
      }
    }

    void IServerSetup.ServerSetup () {
      if (SlaveIs()) {
	// add "slave" services here
      } else {
	for (int i=0;i<3;i++) {
	  cl[i] = new Client();
	  cl[i].ConfigSetName("cl-" + i);
	  cl[i].ConfigSetSrvName("sv-" + i);
	}
	// add "master" services here
	ServiceCreate("ECOI", ECOI);
	ServiceCreate("ECOU", ECOU);
	ServiceCreate("SETU", SETU);
	ServiceCreate("GETU", GETU);
	ServiceCreate("ECOW", new ECOW());
	ServiceCreate("ECOS", ECOS);
	ServiceCreate("ECOD", ECOD);
	ServiceCreate("ECOF", ECOF);
	ServiceCreate("ECOC", ECOC);
	ServiceCreate("ECOL", ECOL);
	ServiceCreate("ECLI", ECLI);
	ServiceCreate("LST1", LST1);
	ServiceCreate("LST2", LST2);
	ServiceCreate("ECUL", ECUL);
	ServiceCreate("ECOO", ECOO);
	ServiceCreate("ECOY", ECOY);
	ServiceCreate("ECOB", ECOB);
	ServiceCreate("BUF1", BUF1);
	ServiceCreate("BUF2", BUF2);
	ServiceCreate("BUF3", BUF3);
	ServiceCreate("ERR2", ERR2);
	ServiceCreate("ERR3", ERR3);
	ServiceCreate("ERR4", ERR4);
	ServiceCreate("ERRT", ERRT);
	ServiceCreate("CSV1", CSV1);
	ServiceCreate("SLEP", SLEP);
	ServiceCreate("USLP", USLP);
	ServiceCreate("ECON", ECON);
	ServiceCreate("MSQT", MSQT);
	ServiceCreate("SND1", SND1);
	ServiceCreate("SND2", SND2);
	ServiceCreate("REDI", REDI);
	ServiceCreate("RDUL", RDUL);
	ServiceCreate("GTCX", GTCX);
	ServiceCreate("GTTO", GTTO);
	ServiceCreate("LANG", LANG);
	ServiceCreate("INIT", INIT);
	ServiceCreate("CNFG", CNFG);
	ServiceCreate("ERLR", ERLR);
	ServiceCreate("ERLS", ERLS);
	ServiceCreate("CFG1", CFG1);
	ServiceCreate("PRNT", PRNT);
	ServiceCreate("TRNS", TRNS);
	ServiceCreate("TRN2", TRN2);
      }
    }

  // ########################################################################

    private void PRNT () {

      SendSTART();
      SendC(LinkGetCtxId() + " - " + ReadC());
      SendEND_AND_WAIT("WRIT");
      SendRETURN();
    }

    private void CFG1 () {
      String cmd = ReadC();

      SendSTART();

      if (cmd == "Buffersize") {
	int old = ConfigGetBuffersize();
	ConfigSetBuffersize (ReadI());
	SendI (ConfigGetBuffersize());
	ConfigSetBuffersize (old);
      } else if (cmd == "Debug") {
	int old = ConfigGetDebug();
	ConfigSetDebug (ReadI());
	SendI (ConfigGetDebug());
	ConfigSetDebug (old);
      } else if (cmd == "Timeout") {
	long old = ConfigGetTimeout();
	ConfigSetTimeout (ReadW());
	SendW (ConfigGetTimeout());
	ConfigSetTimeout (old);
      } else if (cmd == "Name") {
	string old = ConfigGetName();
	ConfigSetName (ReadC());
	SendC (ConfigGetName());
	ConfigSetName (old);
      } else if (cmd == "SrvName") {
	string old = ConfigGetSrvName();
	ConfigSetSrvName (ReadC());
	SendC (ConfigGetSrvName());
	ConfigSetSrvName (old);
      } else if (cmd == "Ident") {
	string old = FactoryCtxIdentGet();
	FactoryCtxDefaultSet (ReadC());
	bool check = LinkGetTargetIdent() == ReadC();
	SendSTART();
	SendC (FactoryCtxIdentGet());
	SendO (check);
	FactoryCtxIdentSet (old);
      } else if (cmd == "IsSilent") {
	bool old = ConfigGetIsSilent();
	ConfigSetIsSilent (ReadO());
	SendO (ConfigGetIsSilent());
	ConfigSetIsSilent (old);
      } else if (cmd == "IsString") {
	bool old = ConfigGetIsString();
	ConfigSetIsString (ReadO());
	SendO (ConfigGetIsString());
	ConfigSetIsString (old);
      } else if (cmd == "IoUds") {
	string old = ConfigGetIoUdsFile();
	ConfigSetIoUdsFile (ReadC());
	SendC (ConfigGetIoUdsFile());
	ConfigSetIoUdsFile (old);
      } else if (cmd == "IoTcp") {
	string h,p,mh,mp;
	string hv,pv,mhv,mpv;
	h  = ConfigGetIoTcpHost ();
	p  = ConfigGetIoTcpPort ();
	mh = ConfigGetIoTcpMyHost ();
	mp = ConfigGetIoTcpMyPort ();
	hv = ReadC();
	pv = ReadC();
	mhv = ReadC();
	mpv = ReadC();
	ConfigSetIoTcp (hv,pv,mhv,mpv);
	SendC (ConfigGetIoTcpHost());
	SendC (ConfigGetIoTcpPort());
	SendC (ConfigGetIoTcpMyHost());
	SendC (ConfigGetIoTcpMyPort());
	ConfigSetIoTcp (h,p,mh,mp);
      } else if (cmd == "IoPipe") {
	int old = ConfigGetIoPipeSocket();
	ConfigSetIoPipeSocket (ReadI());
	SendI (ConfigGetIoPipeSocket());
	ConfigSetIoPipeSocket (old);
      } else if (cmd == "StartAs") {
	START old = ConfigGetStartAs();
	ConfigSetStartAs ((START)ReadI());
	SendI ((int)ConfigGetStartAs());
	ConfigSetStartAs (old);
      } else {
	ErrorC ("CFG1", 1, "invalid command: " + cmd);
      }
      SendRETURN();
    }

    public void ERLR () {
      SendSTART();
      ReadL_START();
      ReadL_START();
      SendRETURN();
    }

    public void ERLS () {
      SendSTART();
      SendL_START();
      SendU (ReadU());
      SendL_START();
      SendU (ReadU());
      SendRETURN();
    }

    public void CNFG () {
      SendSTART();
      SendO(ConfigGetIsServer());
      SendO(LinkIsParent());
      SendO(SlaveIs());
      SendO(ConfigGetIsString());
      SendO(ConfigGetIsSilent());
      SendO(LinkIsConnected());
      SendC(ConfigGetName());
      SendI(ConfigGetDebug());
      SendI(LinkGetCtxId());
      SendC(ServiceGetToken());
      SendRETURN();
    }

    public void INIT () {
      int max = ReadGetNumItems();
      string[] list = new string[max];
      SendSTART();
      for (int i=0; i<max; i++) {
	list[i] = ReadC();
      }
      MqS.Init(list);
      SendRETURN();
    }

    public void LANG () {
      string s = ReadC();
      SendSTART();
	if (s == "NullReferenceException-1") {
	  throw new System.NullReferenceException("myMessage");
	} else if (s == "NullReferenceException-2") {
	  throw new System.NullReferenceException();
	} else {
	  SendC("nothig to do!!");
	}
      SendRETURN();
    }

    public void GTTO () {
      SendSTART();
      SendC(ServiceGetToken());
      SendRETURN();
    }

    public void RDUL () {
      ReadY();
      ReadS();
      ReadI();
      ReadW();
      ReadU();
    }

    public void GTCX () {
      SendSTART();
      SendI(LinkGetCtxId());
      SendRETURN();
    }

    public void REDI() {
      ReadI();
    }

    public void SND1 () {
      string s = ReadC();
      int id = ReadI();

      SendSTART();
        if (s == "START") {
	  Server parent = (Server) LinkGetParent();
          if (parent != null && parent.cl[id].LinkIsConnected()) {
            cl[id].LinkCreateChild(parent.cl[id]);
          } else {
            cl[id].LinkCreate(ConfigGetDebug());
          }
        } else if (s == "START2") {
          // object already created ERROR
          cl[id].LinkCreate(ConfigGetDebug());
          cl[id].LinkCreate(ConfigGetDebug());
        } else if (s == "START3") {
	  Client parent = new Client();
          // parent not connected ERROR
          cl[id].LinkCreateChild(parent);
        } else if (s == "START4") {
          // master not connected ERROR
          cl[id].SlaveWorker(0);
        } else if (s == "START5") {
          // the 'master' have to be a 'parent' without 'child' objects
	  // 'slave' identifer out of range (0 <= 10000000 <= 1023)
          SlaveWorker(id, "--name", "wk-cl-" + id, "--srvname", "wk-sv-" + id);
        } else if (s == "STOP") {
          cl[id].LinkDelete();
        } else if (s == "SEND") {
          cl[id].SendSTART();
          String TOK = ReadC();
	  ReadProxy(cl[id]);
          cl[id].SendEND(TOK);
        } else if (s == "WAIT") {
          cl[id].SendSTART();
	  ReadProxy(cl[id]);
          cl[id].SendEND_AND_WAIT("ECOI", 5);
          SendI(cl[id].ReadI()+1);
        } else if (s == "CALLBACK") {
          cl[id].SendSTART();
	  ReadProxy(cl[id]);
          cl[id].i = -1;
          cl[id].SendEND_AND_CALLBACK("ECOI", cl[id].ECOI_CB);
          cl[id].ProcessEvent(10, MqS.WAIT.ONCE);
          SendI(cl[id].i+1);
        } else if (s == "ERR-1") {
          cl[id].SendSTART();
	  try {
	    ReadProxy(cl[id]);
	    cl[id].SendEND_AND_WAIT("ECOI", 5);
	  } catch (Exception ex) {
	    ErrorSet(ex);
	    SendI(ErrorGetNum());
	    SendC(ErrorGetText());
	    ErrorReset();
	  }
        }
      SendRETURN();
    }

    int i = 0;
    int j = 0;
    private void SetMyInt () {
     ((Server) SlaveGetMaster()).i = ReadI();
    }

    public void SND2 () {
      string s = ReadC();
      int id = ReadI();
      MqS cl = SlaveGet(id);

      SendSTART();
        if (s == "CREATE") {
          List<string> LIST = new List<string>();
          while (ReadItemExists())
	    LIST.Add(ReadC());
	  LIST.Add("--name");
	  LIST.Add("wk-cl-" + id);
	  LIST.Add("@");
	  LIST.Add("--name");
	  LIST.Add("wk-sv-" + id);
          SlaveWorker(id, LIST.ToArray());
        } else if (s == "CREATE2") {
	  Client c = new Client();
	  c.LinkCreate(ConfigGetDebug());
	  SlaveCreate (id, c);
        } else if (s == "DELETE") {
          SlaveDelete(id);
          SendC(SlaveGet(id) == null ? "OK" : "ERROR");
        } else if (s == "SEND") {
          cl.SendSTART();
          String TOK = ReadC();
	  ReadProxy(cl);
          cl.SendEND(TOK);
        } else if (s == "WAIT") {
          cl.SendSTART();
	  cl.SendN(ReadN());
          cl.SendEND_AND_WAIT("ECOI", 5);
          SendI(cl.ReadI()+1);
        } else if (s == "CALLBACK") {
          cl.SendSTART();
	  ReadProxy(cl);
          i = -1;
          cl.SendEND_AND_CALLBACK("ECOI", ((Server)cl).SetMyInt);
          cl.ProcessEvent(10, MqS.WAIT.ONCE);
          SendI(i+1);
        } else if (s == "MqSendEND_AND_WAIT") {
          string TOK = ReadC();
          cl.SendSTART();
          while (ReadItemExists())
	    ReadProxy(cl);
          cl.SendEND_AND_WAIT(TOK, 5);
          while (cl.ReadItemExists())
	    cl.ReadProxy(this);
        } else if (s == "MqSendEND") {
          string TOK = ReadC();
          cl.SendSTART();
          while (ReadItemExists())
	    ReadProxy(cl);
          cl.SendEND(TOK);
          return;
        } else if (s == "ERR-1") {
	  ClientERR2 c = new ClientERR2();
	  c.LinkCreate(ConfigGetDebug());
        } else if (s == "isSlave") {
          SendO(cl.SlaveIs());
        }
      SendRETURN();
    }

    public void MSQT () {
      SendSTART();
      if (ConfigGetDebug() != 0) {
	SendC ("debug");
	SendI (ConfigGetDebug());
      }
      if (ConfigGetIsString() == false)
	SendC ("binary");
      if (ConfigGetIsSilent() == true)
	SendC ("silent");
      SendC ("sOc");
      SendC (ConfigGetIsServer() ? "SERVER" : "CLIENT");
      SendC ("pOc");
      SendC (LinkIsParent() ? "PARENT" : "CHILD");
      SendRETURN();
    }

    public void ECON () {
      SendSTART();
      SendC(ReadC() + "-" + ConfigGetName());
      SendRETURN();
    }

    public void CSV1 () {
      int num;

      // call an other service
      SendSTART ();
      num = ReadI();
      num++;
      SendI (num);
      SendEND_AND_WAIT ("CSV2", 10);

      // read the answer and send the result back
      SendSTART ();
      num = ReadI();
      num++;
      SendI (num);
      SendRETURN ();
    }

    public void SLEP () {
      SendSTART();
      int i = ReadI();
      System.Threading.Thread.Sleep(i*1000);
      SendI(i);
      SendRETURN();
    }

    public void USLP () {
      SendSTART();
      int i = ReadI();
      System.Threading.Thread.Sleep(i/1000);
      SendI(i);
      SendRETURN();
    }

    public void ERR2 () {
      SendSTART();
      SendC("some data");
      ErrorC("Ot_ERR2", 10, "some error");
      SendRETURN();
    }

    public void ERR3 () {
      SendSTART();
      SendRETURN();
      SendRETURN();
    }

    public void ERR4 () {
      Environment.Exit (1);
    }

    public void ERRT () {
      SendSTART();
      ErrorC("MYERR", 9, ReadC());
      SendERROR();
    }

    public void BUF1 () {
      MqBufferS buf = ReadU();
      char typ = buf.Type;
      SendSTART();
      SendC(typ.ToString());
      switch (typ) {
	case 'Y': SendY(buf.Y); break;
	case 'O': SendO(buf.O); break;
	case 'S': SendS(buf.S); break;
	case 'I': SendI(buf.I); break;
	case 'F': SendF(buf.F); break;
	case 'W': SendW(buf.W); break;
	case 'D': SendD(buf.D); break;
	case 'C': SendC(buf.C); break;
	case 'B': SendB(buf.B); break;
      }
      SendRETURN();
    }

    public void BUF2 () {
      SendSTART();
      for (int i=0; i<3; i++) {
	MqBufferS buf = ReadU();
	SendC(buf.Type.ToString());
	SendU(buf);
      }
      SendRETURN();
    }

    public void BUF3 () {
      SendSTART();
      MqBufferS buf = ReadU();
      SendC(buf.Type.ToString());
      SendU(buf);
      SendI(ReadI());
      SendU(buf);
      SendRETURN();
    }

    public void ECOB () {
      SendSTART();
      SendB(ReadB());
      SendRETURN();
    }

    public void ECOY () {
      SendSTART();
      SendY(ReadY());
      SendRETURN();
    }

    public void ECOO () {
      SendSTART();
      SendO(ReadO());
      SendRETURN();
    }

    public void ECUL () {
      SendSTART();
      SendY(ReadY());
      SendS(ReadS());
      SendI(ReadI());
      SendW(ReadW());
      ReadProxy(this);
      SendRETURN();
    }

    public void LST1 () {
      SendSTART();
      SendL_END();
      SendRETURN();
    }

    public void LST2 () {
      SendSTART();
      ReadL_END();
      SendRETURN();
    }

    void EchoList(bool doincr) {
      MqBufferS buf;
      while (ReadItemExists()) {
	buf = ReadU();
	if (buf.Type == 'L') {
	  ReadL_START (buf);
	  SendL_START ();
	  EchoList (doincr);
	  SendL_END ();
	  ReadL_END ();
	} else if (doincr) {
	  SendI (buf.I+1);
	} else {
	  SendU (buf);
	}
      }
    }

    void ECOL () {
      SendSTART();
      ReadL_START ();
      SendL_START ();
      EchoList(false);
      SendL_END ();
      ReadL_END ();
      SendRETURN();
    }

    void ECLI () {
      MqBufferS opt = ReadU();
      bool doincr = (opt.Type == 'C' && opt.C == "--incr");
      if (!doincr) ReadUndo();
      SendSTART();
      EchoList(doincr);
      SendRETURN();
    }


    void ECOC () {
      SendSTART();
      SendC(ReadC());
      SendRETURN();
    }

    void ECOF () {
      SendSTART();
      SendF(ReadF());
      SendRETURN();
    }

    void ECOD () {
      SendSTART();
      SendD(ReadD());
      SendRETURN();
    }

    void ECOS () {
      SendSTART();
      SendS(ReadS());
      SendRETURN();
    }

    void ECOI () {
      SendSTART();
      SendI(ReadI());
      SendRETURN();
    }

    void ECOU () {
      SendSTART();
      SendU(ReadU());
      SendRETURN();
    }

    void SETU () {
      buf = ReadU();
    }

    void GETU () {
      SendSTART();
      SendU(buf);
      SendRETURN();
    }

    void TRNS () {
      SendSTART ();
      SendT_START ("TRN2");
      SendI (9876);
      SendT_END ();
      SendI ( ReadI() );
      SendEND_AND_WAIT ("ECOI");
      ProcessEvent (MqS.WAIT.ONCE);
      SendSTART ();
      SendI (i);
      SendI (j);
      SendRETURN ();
    }

    void TRN2 () {
      ReadT_START ();
      i = ReadI ();
      ReadT_END ();
      j = ReadI ();
    }

  // ########################################################################

    static void Main(string[] args) {
      Server srv = MqFactoryS<Server>.New("server");
      try {
	srv.LinkCreate(args);
	srv.LogC("test",1,"this is the log test\n");
	srv.ProcessEvent(MqS.WAIT.FOREVER);
      } catch (Exception ex) {
	srv.ErrorSet (ex);
      } finally {
	srv.Exit();
      }
    }

  } // finish class "server"

  public class ECOW : IService {
    void IService.Service (MqS ctx) {
      ctx.SendSTART();
      ctx.SendW(ctx.ReadW());
      ctx.SendRETURN();
    }
  }

} // finish namespace "example"

