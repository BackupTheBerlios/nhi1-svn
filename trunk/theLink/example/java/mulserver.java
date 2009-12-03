/**
 *  \file       example/java/mulserver.java
 *  \brief      \$Id: mulserver.java 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
package example;
import javamsgque.*;
final class mulserver extends MqS implements IServerSetup, IFactory {
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
  public MqS Factory() {
    return new mulserver();
  }
  public static void main(String[] argv) {
    MqS.Init("java", "example.mulserver");
    mulserver srv = new mulserver();
    try {
      srv.ConfigSetName("MyMulServer");
      srv.LinkCreate(argv);
      srv.ProcessEvent(MqS.WAIT.FOREVER);
    } catch (Throwable e) {
      srv.ErrorSet(e);
    }
    srv.Exit();
  }
}
