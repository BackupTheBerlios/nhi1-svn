/**
 *  \file       theLink/example/java/mulserver.java
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
final class mulserver extends MqS implements IServerSetup {
  public mulserver(MqS tmpl) {
    super(tmpl);
  }
  class MMUL implements IService {
    public void Service (MqS ctx) throws MqSException {
      SendSTART();
      SendD(ReadD() * ReadD());
      SendRETURN();
    }
  }
  public void ServerSetup() throws MqSException {
    ServiceCreate("MMUL", new MMUL());
  }
  public static void main(String[] argv) {
    MqS.Init("java", "example.mulserver");
    mulserver srv = MqFactoryS.New("mulserver", mulserver.class);
    try {
      srv.LinkCreate(argv);
      srv.ProcessEvent(MqS.WAIT.FOREVER);
    } catch (Throwable e) {
      srv.ErrorSet(e);
    }
    srv.Exit();
  }
}

