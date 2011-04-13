/**
 *  \file       theLink/example/java/Filter6.java
 *  \brief      \$Id$
 *  
 *  (C) 2011 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

package example;
import javamsgque.*;
import java.util.Queue;
import java.util.LinkedList;
import java.io.FileWriter;

class Filter6 extends MqS implements IServerSetup, IServerCleanup, IEvent, IService {

  private FileWriter FH = null;

  public Filter6(MqS tmpl) {
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
      Filter6 ftr = (Filter6) ServiceGetFilter();
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
    StorageInsert();
    SendRETURN();
  }

  public void Event() throws MqSException {
    MqS ftr = ServiceGetFilter();
    if (StorageCount() == 0L) {
      ErrorSetCONTINUE();
    } else {
      long Id = 0L;
      try {
	ftr.LinkConnect();
	Id = StorageSelect();
	ReadForward(ftr);
      } catch (Throwable ex) {
	ErrorSet(ex);
	if (ErrorIsEXIT()) {
	  ftr.LinkConnect();
	  ErrorReset();
	  return;
	} else {
	  ErrorWrite();
	}
      }
      StorageDelete(Id);
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
    ServiceStorage("PRNT"); 
    ftr.ServiceCreate("WRIT", new WRIT());
    ftr.ServiceProxy("+TRT");
  }

  public static void main(String[] argv) {
    MqS.Init("java", "example.Filter6");
    Filter6 srv = MqFactoryS.Add("transFilter", Filter6.class).New();
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

