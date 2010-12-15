/**
 *  \file       theLink/example/java/Filter5.java
 *  \brief      \$Id: Filter5.java 125 2010-01-06 15:24:29Z aotto1968 $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 125 $
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
package example;
import java.util.ArrayList;
import javamsgque.*;

// F1 ********************************************************

class F1 extends MqS implements IService, IServerSetup {
  public F1(MqS tmpl) {
    super(tmpl);
  }

  public void ServerSetup() throws MqSException {
    ServiceCreate("+FTR", this);
    ServiceProxy("+EOF");
  }

  public void Service(MqS ctx) throws MqSException {
    MqS ftr = ctx.ServiceGetFilter();
    ftr.SendSTART();
    ftr.SendC("F1");
    ftr.SendC(ftr.ConfigGetName());
    ftr.SendI(ftr.ConfigGetStartAs().GetFlag());
    while (ctx.ReadGetNumItems() != 0) {
      ftr.SendC(ctx.ReadC());
    }
    ftr.SendEND_AND_WAIT("+FTR");
    ctx.SendRETURN();
  }
}

// F2 ********************************************************

class F2 extends MqS implements IService, IServerSetup {
  public F2(MqS tmpl) {
    super(tmpl);
  }

  public void ServerSetup() throws MqSException {
    ServiceCreate("+FTR", this);
    ServiceProxy("+EOF");
  }

  public void Service(MqS ctx) throws MqSException {
    MqS ftr = ctx.ServiceGetFilter();
    ftr.SendSTART();
    ftr.SendC("F2");
    ftr.SendC(ftr.ConfigGetName());
    ftr.SendI(ftr.ConfigGetStartAs().GetFlag());
    while (ctx.ReadGetNumItems() != 0) {
      ftr.SendC(ctx.ReadC());
    }
    ftr.SendEND_AND_WAIT("+FTR");
    ctx.SendRETURN();
  }
}

// F3 ********************************************************

class F3 extends MqS implements IService, IServerSetup {
  public F3(MqS tmpl) {
    super(tmpl);
  }

  public void ServerSetup() throws MqSException {
    ServiceCreate("+FTR", this);
    ServiceProxy("+EOF");
  }

  public void Service(MqS ctx) throws MqSException {
    MqS ftr = ctx.ServiceGetFilter();
    ftr.SendSTART();
    ftr.SendC("F3");
    ftr.SendC(ftr.ConfigGetName());
    ftr.SendI(ftr.ConfigGetStartAs().GetFlag());
    while (ctx.ReadGetNumItems() != 0) {
      ftr.SendC(ctx.ReadC());
    }
    ftr.SendEND_AND_WAIT("+FTR");
    ctx.SendRETURN();
  }
}

// Filter5 **************************************************
class Filter5 {
  public static void main(String[] argv) {
    MqS.Init("java", "example.Filter5");
    MqFactoryS.Add("F1", F1.class);
    MqFactoryS.Add("F2", F2.class);
    MqFactoryS.Add("F3", F3.class);
    MqS srv = MqFactoryS.Call(argv[0]);
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


