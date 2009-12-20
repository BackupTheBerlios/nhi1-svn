/**
 *  \file       theLink/example/java/manfilter.java
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
class manfilter extends MqS implements IFactory {
  public MqS Factory() {
    return new manfilter();
  }
  private static class FTR implements IService {
    public void Service (MqS ctx) throws MqSException {
      MqS ftr = ctx.ConfigGetFilter();
      ftr.SendSTART();
      while (ctx.ReadItemExists()) {
	ftr.SendC("<" + ctx.ReadC() + ">");
      }
      ftr.SendEND_AND_WAIT("+FTR");
      ctx.SendRETURN();
    }
  }
  public static void main(String[] argv) {
    manfilter srv = new manfilter();
    try {
      srv.ConfigSetName("filter");
      srv.ConfigSetIsServer(true);
      srv.LinkCreate(argv);
      srv.ServiceCreate("+FTR", new manfilter.FTR()); 
      srv.ServiceProxy ("+EOF"); 
      srv.ProcessEvent(MqS.WAIT.FOREVER);
    } catch (Throwable e) {
      srv.ErrorSet(e);
    }
    srv.Exit();
  }
}
