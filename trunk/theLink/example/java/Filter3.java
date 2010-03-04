/**
 *  \file       theLink/example/java/Filter3.java
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

class Filter3 extends MqS implements IFactory, IServerSetup {

  public MqS Factory() {
    return new Filter3();
  }

  public void ServerSetup() throws MqSException {
    MqS ftr = ServiceGetFilter();
    ServiceProxy("+ALL"); 
    ServiceProxy("+TRT"); 
    ftr.ServiceProxy("+ALL");
    ftr.ServiceProxy("+TRT");
  }

  public static void main(String[] argv) {
    MqS.Init("java", "example.Filter3");
    Filter3 srv = new Filter3();
    try {
      srv.ConfigSetName("Filter3");
      srv.LinkCreate(argv);
      srv.ProcessEvent(MqS.WAIT.FOREVER);
    } catch (Throwable e) {
      srv.ErrorSet(e);
    } finally {
      srv.Exit();
    }
  }
}



