/**
 *  \file       theLink/tests/Server.java
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

package example;

import javamsgque.*;
import java.util.List;
import java.util.ArrayList;

final class Client extends MqS implements ICallback, IFactory, IBgError {

  public int i = 0;

  public MqS Factory() {
    return new Client();
  }

  public void BgError () throws MqSException {
    MqS master = ConfigGetMaster();
    if (master != null) {
      master.ErrorC ("BGERROR", ErrorGetNum(), ErrorGetText());
      master.SendERROR ();
    }
  }

  public void Callback (MqS ctx) throws MqSException {
    i = ctx.ReadI();
  }
  
  public void LinkCreate (int debug) throws MqSException {
    ConfigSetDebug (debug);
    super.LinkCreate("@", "SELF", "--name", "test-server");
  }
}

final class ClientERR extends MqS implements ICallback {

  public int i = 0;

  public void Callback (MqS ctx) throws MqSException {
    i = ctx.ReadI();
  }
  
  public void LinkCreate (int debug) throws MqSException {
    ConfigSetDebug (debug);
    ConfigSetName ("test-client");
    ConfigSetSrvName ("test-server");
    super.LinkCreate("@", "SELF");
  }
}

final class ClientERR2 extends MqS implements IFactory {

  public int i = 0;

  public MqS Factory() {
    return new ClientERR2();
  }
  
  public void LinkCreate (int debug) throws MqSException {
    ConfigSetDebug (debug);
    ConfigSetName ("cl-err-1");
    super.LinkCreate("@", "DUMMY");
  }
}

final class Server extends MqS implements IServerSetup, IServerCleanup, IFactory {

  private int myInt;
  private MqBufferS buf;
  private Client[] cl = new Client[3];

  public MqS Factory() {
    return new Server();
  }

  public void ServerCleanup() throws MqSException {
    for (int i=0;i<3;i++) {
      cl[i] = null;
    }
  }

  public void ServerSetup() throws MqSException {
    if (ConfigGetIsSlave()) {
      // add "slave" services here
    } else {
      // setup data
      for (int i=0;i<3;i++) {
	cl[i] = new Client();
	cl[i].ConfigSetName("cl-" + i);
	cl[i].ConfigSetSrvName("sv-" + i);
      }
      // add "master" services here
      ServiceCreate("ECOI", new ECOI());
      ServiceCreate("ECOS", new ECOS());
      ServiceCreate("ECOY", new ECOY());
      ServiceCreate("ECOO", new ECOO());
      ServiceCreate("ECOW", new ECOW());
      ServiceCreate("ECOD", new ECOD());
      ServiceCreate("ECOF", new ECOF());
      ServiceCreate("ECOU", new ECOU());
      ServiceCreate("ECON", new ECON());
      ServiceCreate("ECOC", new ECOC());
      ServiceCreate("ECOL", new ECOL());
      ServiceCreate("ECUL", new ECUL());
      ServiceCreate("RDUL", new RDUL());
      ServiceCreate("ECOB", new ECOB());
      ServiceCreate("ECOM", new ECOM());

      ServiceCreate("SETU", new SETU());
      ServiceCreate("GETU", new GETU());

      ServiceCreate("ECLI", new ECLI());
      ServiceCreate("MSQT", new MSQT());
      ServiceCreate("ERR2", new ERR2());
      ServiceCreate("ERR3", new ERR3());
      ServiceCreate("ERR4", new ERR4());
      ServiceCreate("SLEP", new SLEP());
      ServiceCreate("USLP", new USLP());
      ServiceCreate("CSV1", new CSV1());
      ServiceCreate("SND1", new SND1());
      ServiceCreate("SND2", new SND2());
      ServiceCreate("REDI", new REDI());
      ServiceCreate("JAVA", new JAVA());
      ServiceCreate("GTCX", new GTCX());
      ServiceCreate("GTTO", new GTTO());
      ServiceCreate("BUF1", new BUF1());
      ServiceCreate("BUF2", new BUF2());
      ServiceCreate("BUF3", new BUF3());
      ServiceCreate("LST1", new LST1());
      ServiceCreate("LST2", new LST2());
      ServiceCreate("INIT", new INIT());
      ServiceCreate("CNFG", new CNFG());
      ServiceCreate("ERLR", new ERLR());
      ServiceCreate("ERLS", new ERLS());
      ServiceCreate("CFG1", new CFG1());
    }
  }

  class CFG1 implements IService {
    public void Service (MqS ctx) throws MqSException {
      String cmd = ReadC();
      SendSTART();
      if (cmd.equals("Buffersize")) {
	int old = ConfigGetBuffersize();
	ConfigSetBuffersize (ReadI());
	SendI (ConfigGetBuffersize());
	ConfigSetBuffersize (old);
      } else if (cmd.equals("Debug")) {
	int old = ConfigGetDebug();
	ConfigSetDebug (ReadI());
	SendI (ConfigGetDebug());
	ConfigSetDebug (old);
      } else if (cmd.equals("Timeout")) {
	long old = ConfigGetTimeout();
	ConfigSetTimeout (ReadW());
	SendW (ConfigGetTimeout());
	ConfigSetTimeout (old);
      } else if (cmd.equals("Name")) {
	String old = ConfigGetName();
	ConfigSetName (ReadC());
	SendC (ConfigGetName());
	ConfigSetName (old);
      } else if (cmd.equals("SrvName")) {
	String old = ConfigGetSrvName();
	ConfigSetSrvName (ReadC());
	SendC (ConfigGetSrvName());
	ConfigSetSrvName (old);
      } else if (cmd.equals("Ident")) {
	String old = ConfigGetIdent();
	ConfigSetIdent (ReadC());
	boolean check = ConfigCheckIdent (ReadC());
	SendSTART();
	SendC (ConfigGetIdent());
	SendO (check);
	ConfigSetSrvName (old);
      } else if (cmd.equals("IsSilent")) {
	boolean old = ConfigGetIsSilent();
	ConfigSetIsSilent (ReadO());
	SendO (ConfigGetIsSilent());
	ConfigSetIsSilent (old);
      } else if (cmd.equals("IsString")) {
	boolean old = ConfigGetIsString();
	ConfigSetIsString (ReadO());
	SendO (ConfigGetIsString());
	ConfigSetIsString (old);
      } else if (cmd.equals("IoUds")) {
	String old = ConfigGetIoUdsFile();
	ConfigSetIoUds (ReadC());
	SendC (ConfigGetIoUdsFile());
	ConfigSetIoUds (old);
      } else if (cmd.equals("IoTcp")) {
	String h,p,mh,mp;
	String hv,pv,mhv,mpv;
	h = ConfigGetIoTcpHost ();
	p = ConfigGetIoTcpPort ();
	mh = ConfigGetIoTcpMyHost ();
	mp = ConfigGetIoTcpMyPort ();
	hv = ReadC();
	pv = ReadC();
	mhv = ReadC();
	mpv = ReadC();
	ConfigSetIoTcp (hv,pv,mhv,mpv);
	SendC (ConfigGetIoTcpHost ());
	SendC (ConfigGetIoTcpPort ());
	SendC (ConfigGetIoTcpMyHost ());
	SendC (ConfigGetIoTcpMyPort ());
	ConfigSetIoTcp (h,p,mh,mp);
      } else if (cmd.equals("IoPipe")) {
	int old = ConfigGetIoPipeSocket();
	ConfigSetIoPipe (ReadI());
	SendI (ConfigGetIoPipeSocket());
	ConfigSetIoPipe (old);
      } else if (cmd.equals("StartAs")) {
	int old = ConfigGetStartAs();
	ConfigSetStartAs (ReadI());
	SendI (ConfigGetStartAs());
	ConfigSetStartAs (old);
      } else {
	ErrorC ("CFG1", 1, "invalid command: " + cmd);
      }
      SendRETURN();
    }
  }

  class ERLR implements IService {
    public void Service (MqS ctx) throws MqSException {
      ctx.SendSTART();
      ctx.ReadL_START();
      ctx.ReadL_START();
      ctx.SendRETURN();
    }
  }

  class ERLS implements IService {
    public void Service (MqS ctx) throws MqSException {
      ctx.SendSTART();
      ctx.SendL_START();
      ctx.SendU(ctx.ReadU());
      ctx.SendL_START();
      ctx.SendU(ctx.ReadU());
      ctx.SendRETURN();
    }
  }

  class CNFG implements IService {
    public void Service (MqS ctx) throws MqSException {
      ctx.SendSTART();
      ctx.SendO(ctx.ConfigGetIsServer());
      ctx.SendO(ctx.ConfigGetIsParent());
      ctx.SendO(ctx.ConfigGetIsSlave());
      ctx.SendO(ctx.ConfigGetIsString());
      ctx.SendO(ctx.ConfigGetIsSilent());
      ctx.SendO(ctx.ConfigGetIsConnected());
      ctx.SendC(ctx.ConfigGetName());
      ctx.SendI(ctx.ConfigGetDebug());
      ctx.SendI(ctx.ConfigGetCtxId());
      ctx.SendC(ctx.ConfigGetToken());
      ctx.SendRETURN();
    }
  }

// Start SND1
  class SND1 implements IService {
    public void Service (MqS ctx) throws MqSException {
      String s = ReadC();
      int id = ReadI();
      SendSTART();
	if (s.equals("START")) {
	  Server parent = (Server) ConfigGetParent();
	  if (parent != null && parent.cl[id].ConfigGetIsConnected()) {
	    cl[id].LinkCreateChild(parent.cl[id]);
	  } else {
	    cl[id].LinkCreate(ConfigGetDebug());
	  }
	} else if (s.equals("START2")) {
	  // object already created ERROR
	  cl[id].LinkCreate(ConfigGetDebug());
	  cl[id].LinkCreate(ConfigGetDebug());
	} else if (s.equals("START3")) {
	  // parent not connected ERROR
	  Client parent = new Client();
	  cl[id].LinkCreateChild(parent);
	} else if (s.equals("START4")) {
	  // master not connected ERROR
	  cl[id].SlaveWorker(0);
	} else if (s.equals("START5")) {
	  // the 'master' have to be a 'parent' without 'child' objects
	  // 'slave' identifer out of range (0 <= 10000000 <= 1023)
	  SlaveWorker(id, "--name", "cl-" + id, "--srvname", "sv-" + id, "--thread");
	} else if (s.equals("STOP")) {
	  cl[id].LinkDelete();
	} else if (s.equals("SEND")) {
	  cl[id].SendSTART();
	  String TOK = ReadC();
	  ReadProxy(cl[id]);
	  cl[id].SendEND(TOK);
	} else if (s.equals("WAIT")) {
	  cl[id].SendSTART();
	  ReadProxy(cl[id]);
	  cl[id].SendEND_AND_WAIT("ECOI", 5);
	  SendI(cl[id].ReadI()+1);
	} else if (s.equals("CALLBACK")) {
	  cl[id].SendSTART();
	  ReadProxy(cl[id]);
	  cl[id].i = -1;
	  cl[id].SendEND_AND_CALLBACK("ECOI", cl[id]);
	  cl[id].ProcessEvent(10,MqS.WAIT.ONCE);
	  SendI(cl[id].i+1);
	} else if (s.equals("ERR-1")) {
	  cl[id].SendSTART();
	  try {
	    ReadProxy(cl[id]);
	    cl[id].SendEND_AND_WAIT("ECOI", 5);
	  } catch (Throwable ex) {
	    ErrorSet(ex);
	    SendI(ErrorGetNum());
	    SendC(ErrorGetText());
	    ErrorReset();
	  }
	}
      SendRETURN();
    }
  }

// Start SND2

  class SND2 implements IService, ICallback {
    public void Callback (MqS ctx) throws MqSException {
      Server master = (Server) ctx.ConfigGetMaster();
      master.myInt = ctx.ReadI();
    }
    public void Service (MqS ctx) throws MqSException {
      String s = ReadC();
      int id = ReadI();
      MqS cl = SlaveGet(id);
      SendSTART();
	if (s.equals("CREATE")) {
	  List<String> LIST = new ArrayList<String>();
	  while (ReadItemExists()) 
	    LIST.add(ReadC());
	  LIST.add("--name");
	  LIST.add("cl-" + id);
	  LIST.add("@");
	  LIST.add("--name");
	  LIST.add("sv-" + id);
	  SlaveWorker(id, LIST.toArray(new String[0]));
	} else if (s.equals("CREATE2")) {
	  Client c = new Client();
	  c.LinkCreate(ConfigGetDebug());
	  SlaveCreate(id, c);
	} else if (s.equals("CREATE3")) {
	  ClientERR c = new ClientERR();
	  c.LinkCreate(ConfigGetDebug());
	  SlaveCreate(id, c);
	} else if (s.equals("DELETE")) {
	  SlaveDelete(id);
	  SendC(SlaveGet(id) == null ? "OK" : "ERROR");
	} else if (s.equals("SEND")) {
	  cl.SendSTART();
	  String TOK = ReadC();
	  ReadProxy(cl);
	  cl.SendEND(TOK);
	} else if (s.equals("WAIT")) {
	  cl.SendSTART();
	  ReadProxy(cl);
	  cl.SendEND_AND_WAIT("ECOI", 5);
	  SendI(cl.ReadI()+1);
	} else if (s.equals("CALLBACK")) {
	  cl.SendSTART();
	  ReadProxy(cl);
	  myInt = -1;
	  cl.SendEND_AND_CALLBACK("ECOI", this);
	  cl.ProcessEvent(10,MqS.WAIT.ONCE);
	  SendI(myInt+1);
	} else if (s.equals("MqSendEND_AND_WAIT")) {
	  String TOK = ReadC();
	  cl.SendSTART();
	  while (ReadItemExists())
	    ReadProxy(cl);
	  cl.SendEND_AND_WAIT(TOK, 5);
	  while (cl.ReadItemExists())
	    cl.ReadProxy(ctx);
	} else if (s.equals("MqSendEND")) {
	  String TOK = ReadC();
	  cl.SendSTART();
	  while (ReadItemExists())
	    ReadProxy(cl);
	  cl.SendEND(TOK);
	  return;
	} else if (s.equals("ERR-1")) {
	  ClientERR2 c = new ClientERR2();
	  c.LinkCreate(ConfigGetDebug());
	} else if (s.equals("isSlave")) {
	  SendO(cl.ConfigGetIsSlave());
	}
      SendRETURN();
    }
  }

  class INIT implements IService {
    public void Service (MqS ctx) throws MqSException {
      int max = ctx.ReadGetNumItems();
      String[] list = new String[max];
      ctx.SendSTART();
      for (int i=0; i<max; i++) {
	list[i] = ctx.ReadC();
      }
      MqS.Init(list);
      ctx.SendRETURN();
    }
  }

  class SETU implements IService {
    public void Service (MqS ctx) throws MqSException {
      buf = ctx.ReadU();
    }
  }

  class GETU implements IService {
    public void Service (MqS ctx) throws MqSException {
      ctx.SendSTART();
      ctx.SendU(buf);
      ctx.SendRETURN();
    }
  }

  public static void main(String[] args) {
    MqS.Init("java", "example.Server");
    Server srv = new Server();
    try {
      srv.ConfigSetName("server");
      srv.LinkCreate (args);
      srv.ProcessEvent(MqS.WAIT.FOREVER);
    } catch (Throwable ex) {
      srv.ErrorSet(ex);
    } finally {
      srv.Exit();
    }
  }
}

/////////////////////////////////////////////////////////
///
///  misc other service-classes
///
////////////////////////////////////////////////////////

class GTCX implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    ctx.SendI(ctx.ConfigGetCtxId());
    ctx.SendRETURN();
  }
}

class GTTO implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    ctx.SendC(ctx.ConfigGetToken());
    ctx.SendRETURN();
  }
}

class REDI implements IService {
  int i;
  public void Service (MqS ctx) throws MqSException {
    i = ctx.ReadI();
  }
}

class JAVA implements IService {
  public void Service (MqS ctx) throws MqSException {
    String s = ctx.ReadC();
    ctx.SendSTART();
      if (s.equals("NullPointerException-1")) {
	throw new NullPointerException("myMessage");
      } else if (s.equals("NullPointerException-2")) {
	throw new NullPointerException();
      } else {
	ctx.SendC("nothig to do!!");
      }
    ctx.SendRETURN();
  }
}

class ECOM implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    while (ctx.ReadItemExists()) {
      ctx.ReadProxy(ctx);
    }
    ctx.SendRETURN();
  }
}

class CSV1 implements IService {
  public void Service (MqS ctx) throws MqSException {
    int num;

    // call an other service
    ctx.SendSTART ();
    num = ctx.ReadI();
    num++;
    ctx.SendI (num);
    ctx.SendEND_AND_WAIT ("CSV2", 10);

    // read the answer and send the result back
    ctx.SendSTART ();
    num = ctx.ReadI();
    num++;
    ctx.SendI (num);
    ctx.SendRETURN ();
  }
}

class SLEP implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    int i = ctx.ReadI();
    try {
      Thread.sleep(i*1000);
    } catch (Exception ex) {
      // do nothing
    } finally {
      ctx.SendI(i);
      ctx.SendRETURN();
    }
  }
}

class USLP implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    int i = ctx.ReadI();
    try {
      Thread.sleep(i/1000);
    } catch (Exception ex) {
      // do nothing
    } finally {
      ctx.SendI(i);
      ctx.SendRETURN();
    }
  }
}

class ERR2 implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    ctx.SendC("some data");
    ctx.ErrorC("Ot_ERR2", 10, "some error");
    ctx.SendRETURN();
  }
}

class ERR3 implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    ctx.SendRETURN();
    ctx.SendRETURN();
  }
}

class ERR4 implements IService {
  public void Service (MqS ctx) throws MqSException {
    System.exit(1);
  }
}

// START-JAVA-SERVICE-PROC
class ECOI implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    ctx.SendI(ctx.ReadI());
    ctx.SendRETURN();
  }
}
// END-JAVA-SERVICE-PROC

class ECOS implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    ctx.SendS(ctx.ReadS());
    ctx.SendRETURN();
  }
}

class ECOY implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    ctx.SendY(ctx.ReadY());
    ctx.SendRETURN();
  }
}

class ECOO implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    ctx.SendO(ctx.ReadO());
    ctx.SendRETURN();
  }
}

class ECOW implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    ctx.SendW(ctx.ReadW());
    ctx.SendRETURN();
  }
}

class ECOD implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    ctx.SendD(ctx.ReadD());
    ctx.SendRETURN();
  }
}

class ECOF implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    ctx.SendF(ctx.ReadF());
    ctx.SendRETURN();
  }
}

class ECOB implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    ctx.SendB(ctx.ReadB());
    ctx.SendRETURN();
  }
}

class ECOU implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    ctx.SendU(ctx.ReadU());
    ctx.SendRETURN();
  }
}

class ECON implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    ctx.SendC(ctx.ReadC() + "-" + ctx.ConfigGetName());
    ctx.SendRETURN();
  }
}

class ECLI implements IService {

  private void EchoList(MqS ctx, boolean doincr) throws MqSException {
    MqBufferS buf;
    while (ctx.ReadGetNumItems() != 0) {
      buf = ctx.ReadU();
      if (buf.GetType() == 'L') {
	ctx.ReadL_START (buf);
	ctx.SendL_START ();
	EchoList (ctx, doincr);
	ctx.SendL_END ();
	ctx.ReadL_END ();
      } else if (doincr) {
	ctx.SendI (buf.GetI()+1);
      } else {
	ctx.SendU (buf);
      }
    }
  }

  public void Service (MqS ctx) throws MqSException {
    MqBufferS opt = ctx.ReadU();
    boolean doincr = (opt.GetType() == 'C' && opt.GetC().equals("--incr"));
    if (!doincr) ctx.ReadUndo();
    ctx.SendSTART();
    EchoList(ctx, doincr);
    ctx.SendRETURN();
  }
}

class ECOL implements IService {

  private void EchoList(MqS ctx) throws MqSException {
    MqBufferS buf;
    while (ctx.ReadItemExists()) {
      buf = ctx.ReadU();
      if (buf.GetType() == 'L') {
	ctx.ReadL_START (buf);
	ctx.SendL_START ();
	EchoList (ctx);
	ctx.SendL_END ();
	ctx.ReadL_END ();
      } else {
	ctx.SendU (buf);
      }
    }
  }

  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    ctx.ReadL_START ();
    ctx.SendL_START ();
    EchoList(ctx);
    ctx.SendL_END ();
    ctx.ReadL_END ();
    ctx.SendRETURN();
  }
}

class ECUL implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    ctx.SendY(ctx.ReadY());
    ctx.SendS(ctx.ReadS());
    ctx.SendI(ctx.ReadI());
    ctx.SendW(ctx.ReadW());
    ctx.ReadProxy(ctx);
    ctx.SendRETURN();
  }
}

class RDUL implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.ReadY();
    ctx.ReadS();
    ctx.ReadI();
    ctx.ReadW();
    ctx.ReadU();
  }
}

class ECOC implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    ctx.SendC(ctx.ReadC());
    ctx.SendRETURN();
  }
}

class MSQT implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    if (ctx.ConfigGetDebug() != 0) {
      ctx.SendC ("debug");
      ctx.SendI (ctx.ConfigGetDebug());
    }
    if (ctx.ConfigGetIsString() == false)
      ctx.SendC ("binary");
    if (ctx.ConfigGetIsSilent() == true)
      ctx.SendC ("silent");
    ctx.SendC ("sOc");
    ctx.SendC (ctx.ConfigGetIsServer() ? "SERVER" : "CLIENT");
    ctx.SendC ("pOc");
    ctx.SendC (ctx.ConfigGetIsParent() ? "PARENT" : "CHILD");
    ctx.SendRETURN();
  }
}

class BUF1 implements IService {
  public void Service (MqS ctx) throws MqSException {
    MqBufferS buf = ctx.ReadU();
    char typ = buf.GetType();
    ctx.SendSTART();
    ctx.SendC(Character.toString(typ));
    switch (typ) {
      case 'Y': ctx.SendY(buf.GetY()); break;
      case 'O': ctx.SendO(buf.GetO()); break;
      case 'S': ctx.SendS(buf.GetS()); break;
      case 'I': ctx.SendI(buf.GetI()); break;
      case 'F': ctx.SendF(buf.GetF()); break;
      case 'W': ctx.SendW(buf.GetW()); break;
      case 'D': ctx.SendD(buf.GetD()); break;
      case 'C': ctx.SendC(buf.GetC()); break;
      case 'B': ctx.SendB(buf.GetB()); break;
    }
    ctx.SendRETURN();
  }
}

class BUF2 implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();

    for (int i=0; i<3; i++) {
      MqBufferS buf = ctx.ReadU();
      ctx.SendC(Character.toString(buf.GetType()));
      ctx.SendU(buf);
    }

    ctx.SendRETURN();
  }
}

class BUF3 implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    MqBufferS buf = ctx.ReadU();
    ctx.SendC(Character.toString(buf.GetType()));
    ctx.SendU(buf);
    ctx.SendI(ctx.ReadI());
    ctx.SendU(buf);
    ctx.SendRETURN();
  }
}

class LST1 implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    ctx.SendL_END();
    ctx.SendRETURN();
  }
}

class LST2 implements IService {
  public void Service (MqS ctx) throws MqSException {
    ctx.SendSTART();
    ctx.ReadL_END();
    ctx.SendRETURN();
  }
}

