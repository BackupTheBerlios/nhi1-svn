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

class Filter4 extends MqS implements IFactory, IServerSetup, IEvent, IService {

  private class FilterItmS {
    public String token;
    public Boolean isTransaction;
    public byte[] bdy;
  }
  private Queue<FilterItmS> itms = new LinkedList<FilterItmS>();
  private String File = null;

  public MqS Factory() {
    return new Filter4();
  }

  public void ServerSetup() throws MqSException {
    ServiceCreate("+ALL", this); 
    ServiceCreate("LOGF", new LOGF()); 
    ServiceCreate("EXIT", new EXIT()); 
    ServiceGetFilter().ServiceCreate("WRIT", new WRIT());
  }

  public void Event() throws MqSException {
    FilterItmS it = itms.peek();
    if (it == null) {
      ErrorSetCONTINUE();
    } else {
      MqS ftr = ServiceGetFilter();
      try {
	ftr.LinkConnect();
	ftr.SendSTART();
	ftr.SendBDY(it.bdy);
	if (it.isTransaction) {
	  ftr.SendEND_AND_WAIT(it.token);
	} else {
	  ftr.SendEND(it.token);
	}
      } catch (Throwable ex) {
	ErrorSet(ex);
	if (ErrorIsEXIT()) {
	  ErrorReset();
	  return;
	} else {
	  ErrorWrite();
	}
      }
      itms.remove();
    }
  }

  private void ErrorWrite() {
    if (File == null) {
      ErrorPrint();
    } else {
      try {
	FileWriter FH = new FileWriter(File, true);
	FH.write("ERROR: " + ErrorGetText() + "\n");
	FH.close();
      } catch (Throwable ex) {
	ErrorSet(ex);
	ErrorPrint();
      } finally {
	ErrorReset();
      }
    }
  }

  private class WRIT implements IService {
    public void Service (MqS ctx) throws MqSException {
      MqS ftr = ServiceGetFilter();
      ftr.SendRETURN();
      try {
	FileWriter FH = new FileWriter(File, true);
	FH.write(ftr.ReadC() + "\n");
	FH.close();
      } catch (Throwable ex) {
	ErrorSet(ex);
	ErrorPrint();
      }
    }
  }

  private class LOGF implements IService {
    public void Service (MqS ctx) throws MqSException {
      MqS ftr = ServiceGetFilter();
      File = ReadC();
      if (ftr.LinkGetTargetIdent().equals("transFilter")) {
	ftr.SendSTART();
	ftr.SendC(File);
	ftr.SendEND_AND_WAIT("LOGF");
      }
      SendRETURN();
    }
  }

  private class EXIT implements IService {
    public void Service (MqS ctx) throws MqSException {
      System.exit(0);
    }
  }

  public void Service(MqS ctx) throws MqSException {
    FilterItmS it = new FilterItmS();
    it.bdy = ReadBDY();
    it.token = ServiceGetToken();
    it.isTransaction = ServiceIsTransaction();
    itms.add(it);
    SendRETURN();
  }

  public static void main(String[] argv) {
    MqS.Init("java", "example.Filter4");
    Filter4 srv = new Filter4();
    try {
      srv.ConfigSetIgnoreExit(true);
      srv.ConfigSetIdent("transFilter");
      srv.ConfigSetName("Filter4");
      srv.LinkCreate(argv);
      srv.ProcessEvent(MqS.WAIT.FOREVER);
    } catch (Throwable e) {
      srv.ErrorSet(e);
    } finally {
      srv.Exit();
    }
  }
}

