/**
 *  \file       theLink/example/java/Filter5.java
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */


package example;

import javamsgque.*;

// F1 **********************************************************

final class F1 extends MqS implements IServerSetup {
  public F1(MqS tmpl) {
    super(tmpl);
  }

  public static class FTR implements IService {
    public void Service(MqS ctx) throws MqSException {
      MqS ftr = ctx.ServiceGetFilter();
      ftr.SendSTART();
      ftr.SendC("F1");
      ftr.SendC(ftr.ConfigGetName());
      ftr.SendI(ftr.ConfigGetStartAs().GetFlag());
      ftr.SendI(ctx.ConfigGetStatusIs());
      while (ctx.ReadGetNumItems() != 0) {
	ftr.SendC(ctx.ReadC());
      }
      ftr.SendEND_AND_WAIT("+FTR");
      ctx.SendRETURN();
    }
  }

  public void ServerSetup() throws MqSException {
    ServiceCreate("+FTR", new F1.FTR()); 
    ServiceProxy("+EOF"); 
  }
}

// F2 **********************************************************

class F2 extends MqS implements IServerSetup {
  public F2(MqS tmpl) {
    super(tmpl);
  }

  public static class FTR implements IService {
    public void Service(MqS ctx) throws MqSException {
      MqS ftr = ctx.ServiceGetFilter();
      ftr.SendSTART();
      ftr.SendC("F2");
      ftr.SendC(ftr.ConfigGetName());
      ftr.SendI(ftr.ConfigGetStartAs().GetFlag());
      ftr.SendI(ctx.ConfigGetStatusIs());
      while (ctx.ReadGetNumItems() != 0) {
	ftr.SendC(ctx.ReadC());
      }
      ftr.SendEND_AND_WAIT("+FTR");
      ctx.SendRETURN();
    }
  }

  public void ServerSetup() throws MqSException {
    ServiceCreate("+FTR", new F2.FTR()); 
    ServiceProxy("+EOF"); 
  }
}

// F3 **********************************************************

class F3 extends MqS implements IServerSetup {
  public F3(MqS tmpl) {
    super(tmpl);
  }

  public static class FTR implements IService {
    public void Service(MqS ctx) throws MqSException {
      MqS ftr = ctx.ServiceGetFilter();
      ftr.SendSTART();
      ftr.SendC("F3");
      ftr.SendC(ftr.ConfigGetName());
      ftr.SendI(ftr.ConfigGetStartAs().GetFlag());
      ftr.SendI(ctx.ConfigGetStatusIs());
      while (ctx.ReadGetNumItems() != 0) {
	ftr.SendC(ctx.ReadC());
      }
      ftr.SendEND_AND_WAIT("+FTR");
      ctx.SendRETURN();
    }
  }

  public void ServerSetup() throws MqSException {
    ServiceCreate("+FTR", new F3.FTR()); 
    ServiceProxy("+EOF"); 
  }
}

// main ********************************************************

final class Filter5 {

  public static void main(String[] argv) {
    MqS.Init("java", "example.Filter5");
    MqFactoryS.Add("F1", F1.class);
    MqFactoryS.Add("F2", F2.class);
    MqFactoryS.Add("F3", F3.class);
    MqS srv = MqFactoryS.GetCalled(argv[0]).New();
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
