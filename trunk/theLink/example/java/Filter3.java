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

class Filter3 extends MqS implements IServerSetup {

  public Filter3(MqS tmpl) {
    super(tmpl);
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
    Filter3 srv = MqFactoryS.Add("Filter3", Filter3.class).New();
    try {
      srv.LinkCreate(argv);
      srv.ProcessEvent(MqS.WAIT.FOREVER);
    } catch (Throwable e) {
      srv.ErrorSet(e);
    } finally {
      srv.Exit();
    }
  }
}

