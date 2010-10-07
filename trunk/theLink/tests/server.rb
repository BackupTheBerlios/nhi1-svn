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

class Server < MqS

  def initialize
    #puts("initialize-1 -----------" + self.to_s)
    ConfigSetName("server")
    ConfigSetIdent("test-server")
    ConfigSetServerSetup(method(:ServerSetup))
    ConfigSetServerCleanup(method(:ServerCleanup))
    ConfigSetFactory(lambda {Server.new})

    super()
  end

  def ServerCleanup
    #puts("ServerCleanup -----------" + self.to_s)
  end

  def ServerSetup
    #puts("ServerSetup ----------- " + self.to_s)

    # add "master" services here
    ServiceCreate("SETU", method(:SETU))
    ServiceCreate("GETU", method(:GETU))

    ServiceCreate("PRNT", method(:PRNT))

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
      when "Y" then SendY (buf.GetY())
      when "O" then SendO (buf.GetO())
      when "S" then SendS (buf.GetS())
      when "I" then SendI (buf.GetI())
      when "F" then SendF (buf.GetF())
      when "W" then SendW (buf.GetW())
      when "D" then SendD (buf.GetD())
      when "C" then SendC (buf.GetC())
      when "B" then SendB (buf.GetB())
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

  def SETU
    @buf = ReadU()
  end

  def GETU
    SendSTART()
    SendU(@buf)
    SendRETURN()
    @buf = nil
  end

end

## --------------------------------------------------------
##    Main
##

#print("1111")
#sys.stdout.flush()

srv = Server.new

begin
  srv.LinkCreate(ARGV)
  srv.LogC("test",1,"this is the log test\n")
  srv.ProcessEvent(MqS::Wait::FOREVER)
rescue exception => ex
  srv.ErrorSet(ex)
ensure
  srv.Exit()
end

# vim: softtabstop=2:tabstop=8:shiftwidth=2:expandtab
