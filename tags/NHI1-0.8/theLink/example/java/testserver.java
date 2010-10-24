/**
 *  \file       theLink/example/java/testserver.java
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

final class testserver extends MqS implements IServerSetup, IFactory {
  public MqS Factory() {
    return new testserver();
  }

  class GTCX implements IService {
    public void Service (MqS ctx) throws MqSException {
      SendSTART();
      SendI(LinkGetCtxId());
      SendC("+");
      if (LinkIsParent()) {
	SendI(-1);
      } else {
	SendI(LinkGetParent().LinkGetCtxId());
      }
      SendC("+");
      SendC(ConfigGetName());
      SendC(":");
      SendRETURN();
    }
  }

  public void ServerSetup() throws MqSException {
    ServiceCreate("GTCX", new GTCX());
  }

  public static void main(String[] argv) {
    MqS.Init("java", "example.testserver");
    testserver srv = new testserver();
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



