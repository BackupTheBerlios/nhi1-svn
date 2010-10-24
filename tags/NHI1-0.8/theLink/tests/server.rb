#+
#§  \file       theLink/tests/server.rb
#§  \brief      \$Id$
#§  
#§  (C) 2010 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

require "rubymsgque"

class Client < MqS
  def initialize
    ConfigSetFactory(lambda {Client.new})
    ConfigSetBgError(method(:BgError))
    ConfigSetSrvName("test-server")
    #ConfigSetName("client")
    super()
  end
  def LinkCreate(debug)
    ConfigSetDebug(debug)
    super("client", "@", "SELF")
  end
  def BgError
    master = SlaveGetMaster();
    if master != nil
      master.ErrorC("BGERROR", ErrorGetNum(), ErrorGetText());
      master.SendERROR();
    end
  end
end

class ClientERR < MqS
  def initialize
    ConfigSetSrvName("test-server")
    ConfigSetName("test-client")
    super()
  end
  def LinkCreate(debug)
    ConfigSetDebug(debug)
    super("client", "@", "SELF")
  end
end

class ClientERR2 < MqS
  def initialize
    ConfigSetName("cl-err-1")
    super()
  end
  def LinkCreate(debug)
    ConfigSetDebug(debug)
    super("client", "@", "DUMMY")
  end
end

class Server < MqS
  attr_reader :cl
  def initialize
    #$stderr.puts("initialize-1 -----------" + self.to_s)
    #$stderr.flush
    ConfigSetName("server")
    ConfigSetIdent("test-server")
    ConfigSetServerSetup(method(:ServerSetup))
    ConfigSetServerCleanup(method(:ServerCleanup))
    ConfigSetFactory(lambda {Server.new})
    @buf = nil
    super()
  end

  def ServerCleanup
    for i in 0..2
      if @cl[i] != nil
        @cl[i].Delete()
        @cl[i] = nil
      end
    end
  end

  def ServerSetup
    if (SlaveIs() == false)

      # initialize objects
      @cl = [Client.new, Client.new, Client.new]
      for i in 0..2 
        @cl[i].ConfigSetName("cl-" + i.to_s)
        @cl[i].ConfigSetSrvName("sv-" + i.to_s)
      end

      # add "master" services here
      ServiceCreate("SETU", method(:SETU))
      ServiceCreate("GETU", method(:GETU))

      ServiceCreate("PRNT", method(:PRNT))
      ServiceCreate("TRNS", method(:TRNS))
      ServiceCreate("TRN2", method(:TRN2))
      ServiceCreate("GTTO", method(:GTTO))
      ServiceCreate("MSQT", method(:MSQT))
      ServiceCreate("CNFG", method(:CNFG))
      ServiceCreate("SND1", method(:SND1))
      ServiceCreate("SND2", method(:SND2))
      ServiceCreate("REDI", method(:REDI))
      ServiceCreate("GTCX", method(:GTCX))
      ServiceCreate("CSV1", method(:CSV1))
      ServiceCreate("SLEP", method(:SLEP))
      ServiceCreate("USLP", method(:USLP))
      ServiceCreate("CFG1", method(:CFG1))
      ServiceCreate("INIT", method(:INIT))
      ServiceCreate("LST1", method(:LST1))
      ServiceCreate("LST2", method(:LST2))

      ServiceCreate("BUF1", method(:BUF1))
      ServiceCreate("BUF2", method(:BUF2))
      ServiceCreate("BUF3", method(:BUF3))

      ServiceCreate("ERR2", method(:ERRX))
      ServiceCreate("ERR3", method(:ERRX))
      ServiceCreate("ERR4", method(:ERRX))
      ServiceCreate("ERR5", method(:ERRX))
      ServiceCreate("ERR6", method(:ERRX))
      ServiceCreate("ERRT", method(:ERRT))

      ServiceCreate("ECOY", method(:ECOY))
      ServiceCreate("ECOO", method(:ECOO))
      ServiceCreate("ECOS", method(:ECOS))
      ServiceCreate("ECOI", method(:ECOI))
      ServiceCreate("ECOW", method(:ECOW))
      ServiceCreate("ECOF", method(:ECOF))
      ServiceCreate("ECOD", method(:ECOD))
      ServiceCreate("ECOC", method(:ECOC))
      ServiceCreate("ECOB", method(:ECOB))
      ServiceCreate("ECOU", method(:ECOU))
      ServiceCreate("ECON", method(:ECON))
      ServiceCreate("ECOL", method(:ECOL))
      ServiceCreate("ECLI", method(:ECLI))
      ServiceCreate("ERLR", method(:ERLR))
      ServiceCreate("ERLS", method(:ERLS))
      ServiceCreate("ECUL", method(:ECUL))
      ServiceCreate("RDUL", method(:RDUL))
    end
  end

  def RDUL
    ReadY()
    ReadS()
    ReadI()
    ReadW()
    ReadU()
  end

  def ECUL
    SendSTART()
    SendY(ReadY())
    SendS(ReadS())
    SendI(ReadI())
    SendW(ReadW())
    ReadProxy(self)
    SendRETURN()
  end

  def SETU
    @buf = ReadU()
  end

  def GETU
    SendSTART()
    SendU(@buf)
    SendRETURN()
    @buf = nil
    Sleep(1)
  end

  def ERLR
    SendSTART()
    ReadL_START()
    ReadL_START()
    SendRETURN()
  end

  def ERLS
    SendSTART()
    SendL_START()
    SendU(ReadU())
    SendL_START()
    SendU(ReadU())
    SendRETURN()
  end

  def EchoList(doincr)
    while ReadItemExists()
      buf = ReadU()
      if buf.GetType() == "L"
        ReadL_START(buf)
        SendL_START()
        EchoList(doincr)
        SendL_END()
        ReadL_END()
      elsif doincr == true
        SendI(buf.GetI()+1)
      else
        SendU(buf)
      end
    end
  end

  def ECLI
    opt = ReadU()
    doincr = (opt.GetType() == "C" && opt.GetC() == "--incr")
    ReadUndo() if doincr == false
    SendSTART()
    EchoList(doincr)
    SendRETURN()
  end

  def ECOL
    SendSTART()
    ReadL_START()
    SendL_START()
    EchoList(false)
    SendL_END()
    ReadL_END()
    SendRETURN()
  end

  def LST1
    SendSTART()
    SendL_END()
    SendRETURN()
  end

  def LST2
    SendSTART()
    ReadL_END()
    SendRETURN()
  end

  def INIT
    SendSTART()
    list = []
    while ReadItemExists()
      list << ReadC()
    end
    Init(list)
    SendRETURN()
  end

  def CFG1
    cmd = ReadC()
    SendSTART()
    case cmd
      when "Buffersize"
        old = ConfigGetBuffersize()
        ConfigSetBuffersize(ReadI())
        SendI(ConfigGetBuffersize())
        ConfigSetBuffersize(old)
      when "Debug"
        old = ConfigGetDebug()
        ConfigSetDebug(ReadI())
        SendI(ConfigGetDebug())
        ConfigSetDebug(old)
      when "Timeout"
        old = ConfigGetTimeout()
        ConfigSetTimeout(ReadW())
        SendW(ConfigGetTimeout())
        ConfigSetTimeout(old)
      when "Name"
        old = ConfigGetName()
        ConfigSetName(ReadC())
        SendC(ConfigGetName())
        ConfigSetName(old)
      when "SrvName"
        old = ConfigGetSrvName()
        ConfigSetSrvName(ReadC())
        SendC(ConfigGetSrvName())
        ConfigSetSrvName(old)
      when "Ident"
        old = ConfigGetIdent()
        ConfigSetIdent(ReadC())
        check = LinkGetTargetIdent() == ReadC()
        SendSTART()
        SendC(ConfigGetIdent())
        SendO(check);
        ConfigSetIdent(old)
      when "IsSilent"
        old = ConfigGetIsSilent()
        ConfigSetIsSilent(ReadO())
        SendO(ConfigGetIsSilent())
        ConfigSetIsSilent(old)
      when "IsString"
        old = ConfigGetIsString()
        ConfigSetIsString(ReadO())
        SendO(ConfigGetIsString())
        ConfigSetIsString(old)
      when "IoUds"
        old = ConfigGetIoUdsFile()
        ConfigSetIoUdsFile(ReadC())
        SendC(ConfigGetIoUdsFile())
        ConfigSetIoUdsFile(old)
      when "IoTcp"
        h  = ConfigGetIoTcpHost()
        p  = ConfigGetIoTcpPort()
        mh = ConfigGetIoTcpMyHost()
        mp = ConfigGetIoTcpMyPort()
        hv = ReadC()
        pv = ReadC()
        mhv = ReadC()
        mpv = ReadC()
        ConfigSetIoTcp(hv,pv,mhv,mpv)
        SendC(ConfigGetIoTcpHost())
        SendC(ConfigGetIoTcpPort())
        SendC(ConfigGetIoTcpMyHost())
        SendC(ConfigGetIoTcpMyPort())
        ConfigSetIoTcp(h,p,mh,mp)
      when "IoPipe"
        old = ConfigGetIoPipeSocket()
        ConfigSetIoPipeSocket(ReadI())
        SendI(ConfigGetIoPipeSocket())
        ConfigSetIoPipeSocket(old)
      when "StartAs"
        old = ConfigGetStartAs()
        ConfigSetStartAs(ReadI())
        SendI(ConfigGetStartAs())
        ConfigSetStartAs(old)
      else
        ErrorC("CFG1", 1, "invalid command: " + cmd)
    end
    SendRETURN();
  end

  def USLP
    i = ReadI()
    USleep(i)
    SendSTART()
    SendI(i)
    SendRETURN()
  end

  def SLEP
    i = ReadI()
    Sleep(i)
    SendSTART()
    SendI(i)
    SendRETURN()
  end

  def CSV1
    # call an other service
    SendSTART()
    SendI(ReadI()+1)
    SendEND_AND_WAIT("CSV2", 10)

    # read the answer and send the result back
    SendSTART()
    SendI(ReadI()+1)
    SendRETURN()
  end

  def ECON
    SendSTART()
    SendC(ReadC() + "-" + ConfigGetName())
    SendRETURN()
  end

  def Callback2(ctx)
    @i = ctx.ReadI()
  end

  def SND2
    s  = ReadC()
    id = ReadI()
    cl = SlaveGet(id)
    SendSTART()
    case s
      when "CREATE"
        args = []
        while ReadItemExists()
          args.push(ReadC())
        end
        args.push("--name", "wk-cl-" + id.to_s, "@", "--name", "wk-sv-" + id.to_s)
        SlaveWorker(id, args)
      when "CREATE2"
        slv = Client.new
        slv.LinkCreate(ConfigGetDebug())
        SlaveCreate(id, slv);
      when "CREATE3"
        slv = ClientERR.new
        slv.LinkCreate(ConfigGetDebug())
        SlaveCreate(id, slv);
      when "DELETE"
        SlaveDelete(id);
        SlaveGet(id) == nil ?  SendC("OK") : SendC("ERROR")
      when "SEND"
        cl.SendSTART();
        tok = ReadC();
        cl.SendU(ReadU());
        cl.SendEND(tok);
      when "WAIT"
        cl.SendSTART();
        cl.SendN(ReadN());
        cl.SendEND_AND_WAIT("ECOI", 5);
        SendI(cl.ReadI()+1);
      when "CALLBACK"
        cl.SendSTART();
        cl.SendU(ReadU());
        @i = -1;
        cl.SendEND_AND_CALLBACK("ECOI", method(:Callback2));
        cl.ProcessEvent(10,MqS::WAIT::ONCE);
        SendI(@i+1);
      when "MqSendEND_AND_WAIT"
        tok = ReadC()
        cl.SendSTART()
        while ReadItemExists()
          cl.SendU(ReadU())
        end
        cl.SendEND_AND_WAIT(tok, 5);
        while cl.ReadItemExists()
          SendU(cl.ReadU())
        end
      when "MqSendEND"
        tok = ReadC()
        cl.SendSTART()
        while ReadItemExists()
          cl.SendU(ReadU())
        end
        cl.SendEND(tok);
        return
      when "ERR-1"
        slv = ClientERR2.new
        slv.LinkCreate(ConfigGetDebug())
      when "isSlave"
        SendO(cl.SlaveIs());
    end

    SendRETURN()
  end

  def GTCX
    SendSTART()
    SendI(LinkGetCtxId())
    SendRETURN()
  end

  def REDI
    @i = ReadI()
  end

  def Callback(ctx)
    @i = ctx.ReadI()
  end

  def SND1
    s = ReadC()
    id = ReadI()
    SendSTART()
    case s
      when "START"
        parent = LinkGetParent()
        if parent != nil and parent.cl[id].LinkIsConnected()  
          @cl[id].LinkCreateChild(parent.cl[id])
        else
          @cl[id].LinkCreate(ConfigGetDebug())
        end
      when "START2" 
        @cl[id].LinkCreate(ConfigGetDebug())
        @cl[id].LinkCreate(ConfigGetDebug())
      when "START3" 
        parent = Client.new
        @cl[id].LinkCreateChild(parent)
      when "START4" 
        @cl[id].SlaveWorker(0)
      when "START5" 
        SlaveWorker(id, "--name", "wk-cl-" + id.to_s, "--srvname", "wk-sv-" + id.to_s, "--spawn")
      when "STOP" 
        @cl[id].LinkDelete()
      when "SEND" 
        @cl[id].SendSTART()
        tok = ReadC()
        @cl[id].SendU(ReadU())
        @cl[id].SendEND(tok)
      when "WAIT" 
        @cl[id].SendSTART()
        ReadProxy(@cl[id])
        @cl[id].SendEND_AND_WAIT("ECOI", 5)
        SendI(@cl[id].ReadI()+1)
      when "CALLBACK" 
        @cl[id].SendSTART()
        @cl[id].SendU(ReadU())
        @i = -1
        @cl[id].SendEND_AND_CALLBACK("ECOI", method(:Callback))
        @cl[id].ProcessEvent(10,MqS::WAIT::ONCE)
        SendI(@i+1)
      when "ERR-1" 
        @cl[id].SendSTART()
        begin
          ReadProxy(@cl[id])
          @cl[id].SendEND_AND_WAIT("ECOI", 5)
        rescue Exception => ex
          ErrorSet(ex)
          SendI(ErrorGetNum())
          SendC(ErrorGetText())
          ErrorReset()
        end
    end
    SendRETURN()
  end

  def CNFG
    SendSTART()
    SendO(ConfigGetIsServer())
    SendO(LinkIsParent())
    SendO(SlaveIs())
    SendO(ConfigGetIsString())
    SendO(ConfigGetIsSilent())
    SendO(LinkIsConnected())
    SendC(ConfigGetName())
    SendI(ConfigGetDebug())
    SendI(LinkGetCtxId())
    SendC(ServiceGetToken())
    SendRETURN()
  end

  def MSQT
    SendSTART()
    debug = ConfigGetDebug()
    if debug != 0
      SendC("debug")
      SendI(debug)
    end
    SendC("binary") if not ConfigGetIsString()
    SendC("silent") if ConfigGetIsSilent()
    SendC("sOc");
    ConfigGetIsServer() ? SendC("SERVER") : SendC("CLIENT")
    SendC("pOc");
    LinkIsParent() ? SendC("PARENT") : SendC("CHILD")
    SendRETURN()
  end

  def GTTO
    SendSTART()
    SendC(ServiceGetToken())
    SendRETURN()
  end

  def TRN2
    ReadT_START()
    @i = ReadI()
    ReadT_END()
    @j = ReadI()
  end

  def TRNS
    SendSTART()
    SendT_START("TRN2")
    SendI(9876)
    SendT_END()
    SendI( ReadI() )
    SendEND_AND_WAIT("ECOI")
    ProcessEvent(MqS::WAIT::ONCE)
    SendSTART()
    SendI(@i)
    SendI(@j)
    SendRETURN()
  end

  def ERRT
    SendSTART();
    ErrorC("MYERR", 9, ReadC());
    SendERROR();
  end

  def ERRX
    SendSTART()
    case ServiceGetToken()
      when "ERR2"
        SendC("some data")
        ErrorC("Ot_ERR2", 10, "some error");
      when "ERR3"
        SendRETURN()
      when "ERR4"
        Process.exit!(1)
      when "ERR5"
        ReadProxy(ReadU())
      when "ERR6"
        SendU(self)
    end
    SendRETURN()
  end

  def PRNT
    SendSTART()
    SendC(LinkGetCtxId().to_s + " - " + ReadC())
    SendEND_AND_WAIT("WRIT");
    SendRETURN()
  end

  def ECOC
    SendSTART()
    SendC(ReadC())
    SendRETURN()
  end

  def ECOW
    SendSTART()
    SendW(ReadW())
    SendRETURN()
  end

  def ECOS
    SendSTART()
    SendS(ReadS())
    SendRETURN()
  end

  def ECOF
    SendSTART()
    SendF(ReadF())
    SendRETURN()
  end

  def ECOD
    SendSTART()
    SendD(ReadD())
    SendRETURN()
  end

  def ECOY
    SendSTART()
    SendY(ReadY())
    SendRETURN()
  end

  def BUF1
    buf = ReadU()
    typ = buf.GetType()
    SendSTART()
    SendC(typ)
    case typ
      when "Y" then SendY(buf.GetY())
      when "O" then SendO(buf.GetO())
      when "S" then SendS(buf.GetS())
      when "I" then SendI(buf.GetI())
      when "F" then SendF(buf.GetF())
      when "W" then SendW(buf.GetW())
      when "D" then SendD(buf.GetD())
      when "C" then SendC(buf.GetC())
      when "B" then SendB(buf.GetB())
    end
    SendRETURN()
  end

  def BUF3
    SendSTART()
    buf = ReadU();
    SendC(buf.GetType())
    SendU(buf)
    SendI(ReadI())
    SendU(buf)
    SendRETURN()
  end

  def BUF2
    SendSTART()
    for i in 1..3 do
      buf = ReadU();
      SendC(buf.GetType())
      SendU(buf)
    end
    SendRETURN()
  end

  def ECOO
    SendSTART()
    SendO(ReadO())
    SendRETURN()
  end

  def ECOB
    SendSTART()
    SendB(ReadB())
    SendRETURN()
  end

  def ECOU
    SendSTART()
    SendU(ReadU())
    SendRETURN()
  end

  def ECOI
    SendSTART()
    SendI(ReadI())
    SendRETURN()
  end

end

## --------------------------------------------------------
##    Main
##

srv = Server.new

begin
  srv.LinkCreate($0,ARGV)
  srv.LogC("test",1,"this is the log test\n")
  srv.ProcessEvent(MqS::WAIT::FOREVER)
rescue SignalException
  #ignore
rescue Exception => ex
  srv.ErrorSet(ex)
ensure
  srv.Exit()
end

# vim: softtabstop=2:tabstop=8:shiftwidth=2:expandtab
