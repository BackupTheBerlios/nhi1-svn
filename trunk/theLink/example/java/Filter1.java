/**
 *  \file       theLink/example/java/Filter1.java
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
import java.util.ArrayList;
import javamsgque.*;

class Filter1 extends MqS implements IFactory {
  private ArrayList<ArrayList<String>> data = new ArrayList<ArrayList<String>>();

  public MqS Factory() {
    return new Filter1();
  }

  public static class FTR implements IService {
    public void Service(MqS ctx) throws MqSException {
      ArrayList<String> d = new ArrayList<String>();
      while (ctx.ReadGetNumItems() != 0) {
	d.add("<" + ctx.ReadC() + ">");
      }
      ((Filter1)ctx).data.add(d);
      ctx.SendRETURN();
    }
  }

  public static class EOF implements IService {
    public void Service (MqS ctx) throws MqSException {
      MqS ftr = ctx.ConfigGetFilter();
      for (ArrayList<String> d: ((Filter1)ctx).data) {
	ftr.SendSTART();
	for (String s: d) {
	  ftr.SendC(s);
	}
	ftr.SendEND_AND_WAIT("+FTR");
      }
      ftr.SendSTART();
      ftr.SendEND_AND_WAIT("+EOF");
      ctx.SendRETURN();
    }
  }

  public static void main(String[] argv) {
    Filter1 srv = new Filter1();
    try {
      srv.ConfigSetName("filter");
      srv.ConfigSetIsServer(true);
      srv.LinkCreate(argv);
      srv.ServiceCreate("+FTR", new Filter1.FTR()); 
      srv.ServiceCreate("+EOF", new Filter1.EOF()); 
      srv.ProcessEvent(MqS.WAIT.FOREVER);
    } catch (Throwable e) {
      srv.ErrorSet(e);
    } finally {
      srv.Exit();
    }
  }
}
