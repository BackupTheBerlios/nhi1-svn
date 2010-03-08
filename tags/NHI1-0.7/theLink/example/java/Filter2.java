/**
 *  \file       theLink/example/java/Filter2.java
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

class Filter2 extends MqS implements IFactory {

  public MqS Factory() {
    return new Filter2();
  }

  // service definition from IFilter
  public static class FTR implements IService {
    public void Service(MqS ctx) throws MqSException {
      throw new RuntimeException("my error");
    }
  }

  public static void main(String[] argv) {
    Filter2 srv = new Filter2();
    try {
      srv.ConfigSetName("filter");
      srv.ConfigSetIsServer(true);
      srv.LinkCreate(argv);
      srv.ServiceCreate("+FTR", new Filter2.FTR()); 
      srv.ServiceProxy ("+EOF"); 
      srv.ProcessEvent(MqS.WAIT.FOREVER);
    } catch (Throwable e) {
      srv.ErrorSet(e);
    } finally {
      srv.Exit();
    }
  }
}
