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
    ServiceCreate("+ALL", new Filter()); 
    ftr.ServiceCreate("+ALL", new Filter());
  }

  public class Filter implements IService {
    public void Service(MqS ctx) throws MqSException {
      MqS ftr = ServiceGetFilter();
      ftr.SendSTART();
      ftr.SendBDY(ReadBDY());
      if (ServiceIsTransaction()) {
	ftr.SendEND_AND_WAIT(ServiceGetToken());
	SendSTART();
	SendBDY(ftr.ReadBDY());
      } else {
	ftr.SendEND(ServiceGetToken());
      }
      SendRETURN();
    }
  }

  public static void main(String[] argv) {
    MqS.Init("java", "example.Filter3");
    Filter3 srv = new Filter3();
    try {
      srv.ConfigSetName("filter");
      srv.LinkCreate(argv);
      srv.ProcessEvent(MqS.WAIT.FOREVER);
    } catch (Throwable e) {
      srv.ErrorSet(e);
    } finally {
      srv.Exit();
    }
  }
}



