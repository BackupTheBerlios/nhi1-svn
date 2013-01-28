#+
#:  \file       theLink/example/python/server.py
#:  \brief      \$Id$
#:  
#:  (C) 2009 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

import sys
import os
import time
from pymsgque import *

class Client(MqS):

  def __init__(self, tmpl=None):
    self.ConfigSetBgError(self.BgError)
    self.ConfigSetSrvName("test-server")
    MqS.__init__(self)

  def LinkCreate(self, debug, startAs):
    self.ConfigSetDebug(debug)
    self.ConfigSetStartAs(startAs)
    super().LinkCreate(["client", "@", "server"])

  def BgError(self):
    master = self.SlaveGetMaster();
    if (master != None):
      master.ErrorC ("BGERROR", self.ErrorGetNum(), self.ErrorGetText());
      master.SendERROR();

class ClientERR2(MqS):

  def __init__(self):
    self.ConfigSetName("cl-err-1")
    MqS.__init__(self)

  def LinkCreate(self, debug):
    self.ConfigSetDebug(debug)
    super().LinkCreate(["client", "@", "DUMMY"])

class Server(MqS):

  def __init__(self, tmpl=None):
    #print("__init__ -----------", self)
    self.ConfigSetServerSetup(self.ServerSetup)
    self.ConfigSetServerCleanup(self.ServerCleanup)
    MqS.__init__(self)

  def ServerCleanup(self):
    #print("ServerCleanup -----------", self)
    # free objects
    for i in range(3):
      if self.cl[i] != None:
        #print("ServerCleanup -> self.cl[", i, "] = ", self.cl[i]); sys.stdout.flush()
        self.cl[i].Delete()
        self.cl[i] = None

  def ServerSetup(self):
    #print("ServerSetup ----------- ", self)
    if (self.SlaveIs() == False):
      # initialize objects
      self.cl = [Client(), Client(), Client()]
      for i in range(3):
        #print("ServerSetup -> self.cl[", i, "] = ", self.cl[i]); sys.stdout.flush()
        self.cl[i].ConfigSetName("cl-" + str(i))
        self.cl[i].ConfigSetSrvName("sv-" + str(i))

      # add "master" services here
      self.ServiceCreate("ECOS", self.ECOS)
      self.ServiceCreate("ECOI", self.ECOI)
      self.ServiceCreate("ECOW", self.ECOW)
      self.ServiceCreate("ECOF", self.ECOF)
      self.ServiceCreate("ECOD", self.ECOD)
      self.ServiceCreate("ECOC", self.ECOC)
      self.ServiceCreate("ECOU", self.ECOU)
      self.ServiceCreate("ECON", self.ECON)

      self.ServiceCreate("SETU", self.SETU)
      self.ServiceCreate("GETU", self.GETU)
      self.ServiceCreate("MSQT", self.MSQT)
      self.ServiceCreate("SND1", self.SND1)
      self.ServiceCreate("SND2", self.SND2)
      self.ServiceCreate("REDI", self.REDI)
      self.ServiceCreate("GTCX", self.GTCX)
      self.ServiceCreate("GTTO", self.GTTO)
      self.ServiceCreate("ECOL", self.ECOL)
      self.ServiceCreate("ECLI", self.ECLI)
      self.ServiceCreate("ERR2", self.Error)
      self.ServiceCreate("ERR3", self.Error)
      self.ServiceCreate("ERR4", self.Error)
      self.ServiceCreate("ERRT", self.ERRT)
      self.ServiceCreate("ERR5", self.Error)
      self.ServiceCreate("ERR6", self.Error)
      self.ServiceCreate("ECOY", self.ECOY)
      self.ServiceCreate("ECOO", self.ECOO)
      self.ServiceCreate("ECOB", self.ECOB)
      self.ServiceCreate("SLEP", self.SLEP)
      self.ServiceCreate("USLP", self.USLP)
      self.ServiceCreate("CSV1", self.CSV1)
      self.ServiceCreate("ECUL", self.ECUL)
      self.ServiceCreate("RDUL", self.RDUL)
      self.ServiceCreate("BUF1", self.BUF1)
      self.ServiceCreate("BUF2", self.BUF2)
      self.ServiceCreate("BUF3", self.BUF3)
      self.ServiceCreate("LST1", self.LST1)
      self.ServiceCreate("LST2", self.LST2)
      self.ServiceCreate("INIT", self.INIT)
      self.ServiceCreate("CNFG", self.CNFG)
      self.ServiceCreate("ERLR", self.ERLR)
      self.ServiceCreate("ERLS", self.ERLS)
      self.ServiceCreate("CFG1", self.CFG1)
      self.ServiceCreate("PRNT", self.PRNT)
      self.ServiceCreate("TRNS", self.TRNS)
      self.ServiceCreate("TRN2", self.TRN2)
      self.ServiceCreate("STDB", self.STDB)
      self.ServiceCreate("DMPL", self.DMPL)

  def DMPL (self):
    self.SendSTART ()
    self.SendI (self.ReadDUMP().Size())
    self.SendRETURN ()

  def STDB (self):
    self.SendSTART ()
    self.StorageOpen (self.ReadC())
    self.SendRETURN ()

  def TRNS (self):
    self.SendSTART ()
    self.SendT_START ()
    self.SendI (9876)
    self.SendT_END ("TRN2")
    self.SendI ( self.ReadI() )
    self.SendEND_AND_WAIT ("ECOI")
    self.ProcessEvent (MqS_WAIT_ONCE)
    self.SendSTART ()
    self.SendI (self.i)
    self.SendI (self.j)
    self.SendRETURN ()

  def TRN2 (self):
    self.ReadT_START ()
    self.i = self.ReadI ()
    self.ReadT_END ()
    self.j = self.ReadI ()

  def PRNT(self):
    self.SendSTART()
    self.SendC(str(self.LinkGetCtxId()) + " - " + self.ReadC())
    self.SendEND_AND_WAIT("WRIT");
    self.SendRETURN()

  def CFG1(self):
    cmd = self.ReadC()

    self.SendSTART()

    if cmd == "Buffersize":
      old = self.ConfigGetBuffersize()
      self.ConfigSetBuffersize (self.ReadI())
      self.SendI (self.ConfigGetBuffersize())
      self.ConfigSetBuffersize (old)
    elif cmd == "Debug":
      old = self.ConfigGetDebug()
      self.ConfigSetDebug (self.ReadI())
      self.SendI (self.ConfigGetDebug())
      self.ConfigSetDebug (old)
    elif cmd == "Timeout":
      old = self.ConfigGetTimeout()
      self.ConfigSetTimeout (self.ReadW())
      self.SendW (self.ConfigGetTimeout())
      self.ConfigSetTimeout (old)
    elif cmd == "Name":
      old = self.ConfigGetName()
      self.ConfigSetName (self.ReadC())
      self.SendC (self.ConfigGetName())
      self.ConfigSetName (old)
    elif cmd == "SrvName":
      old = self.ConfigGetSrvName()
      self.ConfigSetSrvName (self.ReadC())
      self.SendC (self.ConfigGetSrvName())
      self.ConfigSetSrvName (old)
    elif cmd == "Storage":
      old = self.ConfigGetStorage()
      self.ConfigSetStorage (self.ReadC())
      self.SendC (self.ConfigGetStorage())
      self.ConfigSetStorage (old)
    elif cmd == "Ident":
      old = self.FactoryCtxIdentGet()
      self.FactoryCtxSet (FactoryGet().Copy (self.ReadC()))
      check = self.LinkGetTargetIdent() == self.ReadC()
      self.SendSTART()
      self.SendC (self.FactoryCtxIdentGet())
      self.SendO (check);
      self.FactoryCtxIdentSet (old)
    elif cmd == "IsSilent":
      old = self.ConfigGetIsSilent()
      self.ConfigSetIsSilent (self.ReadO())
      self.SendO (self.ConfigGetIsSilent())
      self.ConfigSetIsSilent (old)
    elif cmd == "IsString":
      old = self.ConfigGetIsString()
      self.ConfigSetIsString (self.ReadO())
      self.SendO (self.ConfigGetIsString())
      self.ConfigSetIsString (old)
    elif cmd == "IoUds":
      old = self.ConfigGetIoUdsFile()
      self.ConfigSetIoUdsFile (self.ReadC())
      self.SendC (self.ConfigGetIoUdsFile())
      self.ConfigSetIoUdsFile (old)
    elif cmd == "IoTcp":
      h  = self.ConfigGetIoTcpHost   ()
      p  = self.ConfigGetIoTcpPort   ()
      mh = self.ConfigGetIoTcpMyHost ()
      mp = self.ConfigGetIoTcpMyPort ()
      hv = self.ReadC()
      pv = self.ReadC()
      mhv = self.ReadC()
      mpv = self.ReadC()
      self.ConfigSetIoTcp (hv,pv,mhv,mpv)
      self.SendC (self.ConfigGetIoTcpHost())
      self.SendC (self.ConfigGetIoTcpPort())
      self.SendC (self.ConfigGetIoTcpMyHost())
      self.SendC (self.ConfigGetIoTcpMyPort())
      self.ConfigSetIoTcp (h,p,mh,mp)
    elif cmd == "IoPipe":
      old = self.ConfigGetIoPipeSocket()
      self.ConfigSetIoPipeSocket (self.ReadI())
      self.SendI (self.ConfigGetIoPipeSocket())
      self.ConfigSetIoPipeSocket (old)
    elif cmd == "StartAs":
      old = self.ConfigGetStartAs()
      self.ConfigSetStartAs (self.ReadI())
      self.SendI (self.ConfigGetStartAs())
      self.ConfigSetStartAs (old)
    elif cmd == "DefaultIdent":
      self.SendC (FactoryDefaultIdent())
    else:
      self.ErrorC ("CFG1", 1, "invalid command: " + cmd)
    self.SendRETURN();

  def ERLR (self):
    self.SendSTART();
    self.ReadL_START();
    self.ReadL_START();
    self.SendRETURN();

  def ERLS (self):
    self.SendSTART();
    self.SendL_START();
    self.SendU(self.ReadU());
    self.SendL_START();
    self.SendU(self.ReadU());
    self.SendRETURN();

  def CNFG (self):
    self.SendSTART();
    self.SendO(self.ConfigGetIsServer());
    self.SendO(self.LinkIsParent());
    self.SendO(self.SlaveIs());
    self.SendO(self.ConfigGetIsString());
    self.SendO(self.ConfigGetIsSilent());
    self.SendO(self.LinkIsConnected());
    self.SendC(self.ConfigGetName());
    self.SendI(self.ConfigGetDebug());
    self.SendI(self.LinkGetCtxId());
    self.SendC(self.ServiceGetToken());
    self.SendRETURN();

  def INIT(self):
    self.SendSTART()
    LIST = []
    while self.ReadItemExists() :
      LIST.append(self.ReadC())
    Init(LIST)
    self.SendRETURN()

  def LST1(self):
    self.SendSTART()
    self.SendL_END()
    self.SendRETURN()

  def LST2(self):
    self.SendSTART()
    self.ReadL_END()
    self.SendRETURN()

  def BUF1(self):
    buf = self.ReadU()
    typ = buf.GetType()
    self.SendSTART()
    self.SendC(typ)
    if typ == "Y":
        self.SendY (buf.GetY())
    elif typ == "O":
        self.SendO (buf.GetO())
    elif typ == "S": 
        self.SendS (buf.GetS())
    elif typ == "I": 
        self.SendI (buf.GetI())
    elif typ == "F": 
        self.SendF (buf.GetF())
    elif typ == "W": 
        self.SendW (buf.GetW())
    elif typ == "D": 
        self.SendD (buf.GetD())
    elif typ == "C": 
        self.SendC (buf.GetC())
    elif typ == "B": 
        self.SendB (buf.GetB())
    self.SendRETURN()

  def BUF2(self):
    self.SendSTART()
    for i in range(3):
      buf = self.ReadU();
      self.SendC(buf.GetType())
      self.SendU(buf)
    self.SendRETURN()

  def BUF3(self):
    self.SendSTART()
    buf = self.ReadU();
    self.SendC(buf.GetType())
    self.SendU(buf)
    self.SendI(self.ReadI())
    self.SendU(buf)
    self.SendRETURN()

  def RDUL(self):
    self.ReadY()
    self.ReadS()
    self.ReadI()
    self.ReadW()
    self.ReadU()

  def ECUL(self):
    self.SendSTART()
    self.SendY(self.ReadY())
    self.SendS(self.ReadS())
    self.SendI(self.ReadI())
    self.SendW(self.ReadW())
    self.ReadProxy(self)
    self.SendRETURN()

  def CSV1(self):
    # call an other service
    self.SendSTART ()
    num = self.ReadI()
    num += 1
    self.SendI (num)
    self.SendEND_AND_WAIT ("CSV2", 10)

    # read the answer and send the result back
    self.SendSTART ()
    num = self.ReadI()
    num += 1
    self.SendI (num)
    self.SendRETURN ()

  def SLEP(self):
    i = self.ReadI()
    time.sleep(i)
    self.SendSTART()
    self.SendI(i)
    self.SendRETURN()

  def USLP(self):
    self.SendSTART()
    i = self.ReadI()
    time.sleep(1.0/i)
    self.SendI(i)
    self.SendRETURN()

  def ECOB(self):
    self.SendSTART()
    self.SendB(self.ReadB())
    self.SendRETURN()

  def ECOO(self):
    self.SendSTART()
    self.SendO(self.ReadO())
    self.SendRETURN()

  def ECOY(self):
    self.SendSTART()
    self.SendY(self.ReadY())
    self.SendRETURN()

  def Error(self):
    t = self.ServiceGetToken()
    self.SendSTART()
    if t == "ERR2" :
      self.SendC("some data")
      self.ErrorC("Ot_ERR2", 10, "some error");
    elif t == "ERR3" :
      self.SendRETURN()
    elif t == "ERR4" :
      os._exit(1)
    elif t == "ERR5" :
      self.ReadProxy(self.ReadU())
    elif t == "ERR6" :
      self.SendU(self)
    self.SendRETURN()

  def ERRT(self):
    self.SendSTART();
    self.ErrorC("MYERR", 9, self.ReadC());
    self.SendERROR();

  # my services
  def EchoList(self, doincr):
    while self.ReadItemExists() :
      buf = self.ReadU()
      if buf.GetType() == "L" :
        self.ReadL_START (buf)
        self.SendL_START ()
        self.EchoList (doincr)
        self.SendL_END ()
        self.ReadL_END ()
      elif doincr == True :
        self.SendI (buf.GetI()+1)
      else:
        self.SendU (buf)

  def ECLI (self):
    opt = self.ReadU()
    doincr = (opt.GetType() == "C" and opt.GetC() == "--incr")
    if doincr == False:
      self.ReadUndo()
    self.SendSTART()
    self.EchoList(doincr)
    self.SendRETURN()

  def ECOL (self):
    self.SendSTART()
    self.ReadL_START ()
    self.SendL_START ()
    self.EchoList(None)
    self.SendL_END ()
    self.ReadL_END ()
    self.SendRETURN()

  def REDI(self):
    i = self.ReadI()

  def GTCX(self):
    self.SendSTART()
    self.SendI(self.LinkGetCtxId())
    self.SendRETURN()

  def GTTO(self):
    self.SendSTART()
    self.SendC(self.ServiceGetToken())
    self.SendRETURN()

  def ECOS(self):
    self.SendSTART()
    self.SendS(self.ReadS())
    self.SendRETURN()

  def ECOI(self):
    self.SendSTART()
    self.SendI(self.ReadI())
    self.SendRETURN()

  def ECOW(self):
    self.SendSTART()
    self.SendW(self.ReadW())
    self.SendRETURN()

  def ECOF(self):
    self.SendSTART()
    self.SendF(self.ReadF())
    self.SendRETURN()

  def ECOD(self):
    self.SendSTART()
    self.SendD(self.ReadD())
    self.SendRETURN()

  def ECOC(self):
    self.SendSTART()
    self.SendC(self.ReadC())
    self.SendRETURN()

  def ECOU(self):
    self.SendSTART()
    self.SendU(self.ReadU())
    self.SendRETURN()

  def ECON(self):
    self.SendSTART()
    self.SendC(self.ReadC() + "-" + self.ConfigGetName())
    self.SendRETURN()

  def MSQT(self):
    self.SendSTART()
    debug = self.ConfigGetDebug()
    if debug != 0:
      self.SendC ("debug")
      self.SendI (debug)
    if not self.ConfigGetIsString():
      self.SendC ("binary")
    if self.ConfigGetIsSilent():
      self.SendC ("silent")
    self.SendC ("sOc");
    if self.ConfigGetIsServer():
      self.SendC("SERVER")
    else:
      self.SendC("CLIENT")
    self.SendC ("pOc");
    if self.LinkIsParent():
      self.SendC("PARENT")
    else:
      self.SendC("CHILD")
    self.SendRETURN()

  def SETU(self):
    self.buf = self.ReadU().Dup()

  def GETU(self):
    self.SendSTART()
    self.SendU(self.buf)
    self.SendRETURN()
    self.buf.Delete()
    self.buf = None

  def Callback (self, ctx):
    self.i = ctx.ReadI();

  def SND1 (self):
    s = self.ReadC()
    id = self.ReadI()
    #print("SND1 -> act = ", s, ", id = ", id); sys.stdout.flush()
    self.SendSTART()
    if s == "START" :
      parent = self.LinkGetParent();
      #print("START -> self = ", self, ", cl = ", self.cl[id], ", type = ", type(self.cl[id])); sys.stdout.flush()
      if parent != None and parent.cl[id].LinkIsConnected()  :
        #print("CHILD");sys.stdout.flush()
        self.cl[id].LinkCreateChild(parent.cl[id]);
      else:
        #print("PARENT");sys.stdout.flush()
        self.cl[id].LinkCreate(self.ConfigGetDebug(),self.ConfigGetStartAs());
    elif s == "START2" :
      self.cl[id].LinkCreate(self.ConfigGetDebug(),self.ConfigGetStartAs());
      self.cl[id].LinkCreate(self.ConfigGetDebug(),self.ConfigGetStartAs());
    elif s == "START3" :
      parent = Client()
      self.cl[id].LinkCreateChild(parent);
    elif s == "START4" :
      self.cl[id].SlaveWorker(0);
    elif s == "START5" :
      self.SlaveWorker(id, ["--name", "wk-cl-" + str(id), "--srvname", "wk-sv-" + str(id)]);
    elif s == "STOP" :
      #print("STOP -> self = ", self, ", cl = ", self.cl[id], ", type = ", type(self.cl[id]))
      self.cl[id].LinkDelete();
    elif s == "SEND" :
      self.cl[id].SendSTART();
      TOK = self.ReadC();
      self.cl[id].SendU(self.ReadU());
      self.cl[id].SendEND(TOK);
    elif s == "WAIT" :
      self.cl[id].SendSTART();
      self.ReadProxy(self.cl[id]);
      self.cl[id].SendEND_AND_WAIT("ECOI", 5);
      self.SendI(self.cl[id].ReadI()+1);
    elif s == "CALLBACK" :
      self.cl[id].SendSTART();
      self.cl[id].SendU(self.ReadU());
      self.i = -1;
      self.cl[id].SendEND_AND_CALLBACK("ECOI", self.Callback);
      self.cl[id].ProcessEvent(10, MqS_WAIT_ONCE);
      self.SendI(self.i+1);
    elif s == "ERR-1" :
      self.cl[id].SendSTART();
      try:
        self.ReadProxy(self.cl[id]);
        self.cl[id].SendEND_AND_WAIT("ECOI", 5);
      except:
        self.ErrorSet()
        self.SendI(self.ErrorGetNum())
        self.SendC(self.ErrorGetText())
        self.ErrorReset()
    self.SendRETURN()

  def Callback2 (self, ctx):
    ctx.SlaveGetMaster().i = ctx.ReadI();

  def SND2 (self):
    s = self.ReadC()
    id = self.ReadI()
    cl = self.SlaveGet(id)
    self.SendSTART()
    if s == "CREATE" :
      args = []
      while self.ReadItemExists() :
        args.append(self.ReadC())
      args.extend(["--name", "wk-cl-" + str(id), "@", "--name", "wk-sv-" + str(id)])
      self.SlaveWorker(id, args)
    elif s == "CREATE2" :
      slv = Client()
      slv.LinkCreate(self.ConfigGetDebug(),self.ConfigGetStartAs())
      self.SlaveCreate(id, slv);
    elif s == "DELETE" :
      self.SlaveDelete(id);
      if (self.SlaveGet(id) == None):
        self.SendC("OK")
      else :
        self.SendC("ERROR")
    elif s == "SEND" :
      cl.SendSTART();
      TOK = self.ReadC();
      cl.SendU(self.ReadU());
      cl.SendEND(TOK);
    elif s == "WAIT" :
      cl.SendSTART();
      cl.SendN(self.ReadN());
      cl.SendEND_AND_WAIT("ECOI", 5);
      self.SendI(cl.ReadI()+1);
    elif s == "CALLBACK" :
      cl.SendSTART();
      cl.SendU(self.ReadU());
      self.i = -1;
      cl.SendEND_AND_CALLBACK("ECOI", self.Callback2);
      cl.ProcessEvent(10, MqS_WAIT_ONCE);
      self.SendI(self.i+1);
    elif s == "MqSendEND_AND_WAIT" :
      TOK = self.ReadC()
      cl.SendSTART()
      while self.ReadItemExists() :
        cl.SendU(self.ReadU())
      cl.SendEND_AND_WAIT(TOK, 5);
      while cl.ReadItemExists() :
        self.SendU(cl.ReadU())
    elif s == "MqSendEND" :
      TOK = self.ReadC()
      cl.SendSTART()
      while self.ReadItemExists() :
        cl.SendU(self.ReadU())
      cl.SendEND(TOK);
      return
    elif s == "ERR-1" :
      slv = ClientERR2()
      slv.LinkCreate(self.ConfigGetDebug())
    elif s == "isSlave" :
      self.SendO(cl.SlaveIs());
      
    self.SendRETURN()

## --------------------------------------------------------
##    Main
##

srv = FactoryAdd("server", Server).New();

try:
  srv.LinkCreate(sys.argv)
  srv.LogC("test",1,"this is the log test\n")
  srv.ProcessEvent(MqS_WAIT_FOREVER)

except:
  srv.ErrorSet()

finally:
  srv.Exit()

# vim: softtabstop=2:tabstop=8:shiftwidth=2:expandtab
