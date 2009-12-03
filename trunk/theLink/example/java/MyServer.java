/**
 *  \file       example/java/MyServer.java
 *  \brief      \$Id: MyServer.java 507 2009-11-28 15:18:46Z dev1usr $
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

final class MyServer extends MqS implements IServerSetup, IFactory {

  // service to serve all incomming requests for token "HLWO"
  class MyFirstService implements IService {
    public void Service (MqS ctx) throws MqSException {
      SendSTART();
      SendC("Hello World");
      SendRETURN();
    }
  }

  // define a service as link between the token "HLWO" and the class "MyFirstService"
  public void ServerSetup() throws MqSException {
    ServiceCreate("HLWO", new MyFirstService());
  }

  // interface to create a new instance
  public MqS Factory() {
    return new MyServer();
  }

  public static void main(String[] argv) {
    MqS.Init("java", "example.MyServer");
    MyServer srv = new MyServer();
    try {
      srv.LinkCreate(argv);
      srv.ProcessEvent(MqS.WAIT.FOREVER);
    } catch (Throwable e) {
      srv.ErrorSet(e);
    }
    srv.Exit();
  }
}
