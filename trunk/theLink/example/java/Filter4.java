/**
 *  \file       theLink/example/java/Filter4.java
 *  \brief      \$Id: Filter4.java 98 2009-12-22 12:38:02Z aotto1968 $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 98 $
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

package example;
import javamsgque.*;
import java.util.Queue;
import java.util.LinkedList;
import java.io.FileWriter;

class Filter4 extends MqS implements IServerSetup, IServerCleanup, IEvent, IService {

  private Queue<MqDumpS> itms = new LinkedList<MqDumpS>();
  private FileWriter FH = null;

  public Filter4(MqS tmpl) {
    super(tmpl);
  }

  private void ErrorWrite() {
    try {
      FH.write("ERROR: " + ErrorGetText() + "\n");
      FH.flush();
    } catch (Throwable ex) {
      ErrorPrint(ex);
    } finally {
      ErrorReset();
    }
  }

  private class LOGF implements IService {
    public void Service (MqS ctx) throws MqSException {
      MqS ftr = ServiceGetFilter();
      if (ftr.LinkGetTargetIdent().equals("transFilter")) {
	ReadForward(ftr);
      } else {
	try {
	  FH = new FileWriter(ReadC(), true);
	} catch (Throwable ex) {
	  ErrorPrint(ex);
	}
      }
      SendRETURN();
    }
  }

  private class EXIT implements IService {
    public void Service (MqS ctx) throws MqSException {
      Exit();
    }
  }

  private class WRIT implements IService {
    public void Service (MqS ctx) throws MqSException {
      try {
	FH.write(ctx.ReadC() + "\n");
	FH.flush();
      } catch (Throwable ex) {
	ErrorPrint(ex);
      }
      ctx.SendRETURN();
    }
  }

  public void Service (MqS ctx) throws MqSException {
    itms.add(ReadDUMP());
    SendRETURN();
  }

  public void Event() throws MqSException {
    MqS ftr = ServiceGetFilter();
    MqDumpS it = itms.peek();
    if (it == null) {
      ErrorSetCONTINUE();
    } else {
      try {
	ftr.LinkConnect();
	ReadForward(ftr,it);
      } catch (Throwable ex) {
	ErrorSet(ex);
	if (ErrorIsEXIT()) {
	  ErrorReset();
	  ftr.LinkConnect();
	  return;
	} else {
	  ErrorWrite();
	}
      }
      itms.remove();
    }
  }

  public void ServerCleanup() throws MqSException {
    try {
      if (FH != null) FH.close();
    } catch (Throwable ex) {
      ErrorPrint(ex);
    }
  }

  public void ServerSetup() throws MqSException {
    MqS ftr = ServiceGetFilter();
    ServiceCreate("LOGF", new LOGF()); 
    ServiceCreate("EXIT", new EXIT()); 
    ServiceCreate("+ALL", this); 
    ftr.ServiceCreate("WRIT", new WRIT());
    ftr.ServiceProxy("+TRT");
  }

  public static void main(String[] argv) {
    MqS.Init("java", "example.Filter4");
    MqFactoryS.Default("transFilter", Filter4.class);
    Filter4 srv = new Filter4(null);
    //Filter4 srv = MqFactoryS.Add("transFilter", Filter4.class).New();
    try {
      srv.ConfigSetIgnoreExit(true);
      srv.LinkCreate(argv);
      srv.ProcessEvent(MqS.WAIT.FOREVER);
    } catch (Throwable e) {
      srv.ErrorSet(e);
    } finally {
      srv.Exit();
    }
  }
}

