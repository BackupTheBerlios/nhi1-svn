#+
#§  \file       theLink/example/ruby/Filter4.rb
#§  \brief      \$Id: LbMain 26 2009-12-03 11:48:43Z aotto1968 $
#§  
#§  (C) 2010 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 26 $
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

require "rubymsgque"

class Filter4 < MqS
  attr_accessor :fh
  def initialize
    ConfigSetIgnoreExit(true)
    ConfigSetIdent("transFilter")
    ConfigSetFactory(lambda {Filter4.new})
    ConfigSetServerSetup(method(:ServerSetup))
    ConfigSetServerCleanup(method(:ServerCleanup))
    ConfigSetEvent(method(:Event))
    @itms = []
    @fh = nil
    super()
  end
  def ServerCleanup
    ftr = ServiceGetFilter()
    ftr.fh.close if ftr.fh != nil
    ftr.fh = nil
  end
  def ServerSetup
    ftr = ServiceGetFilter()
    ServiceCreate("LOGF", method(:LOGF))
    ServiceCreate("EXIT", method(:EXIT))
    ServiceCreate("+ALL", method(:FilterIn))
    ftr.ServiceCreate("WRIT", ftr.method(:WRIT))
  end
  def WRIT
    @fh.write(ReadC() + "\n")
    @fh.flush()
    SendRETURN()
  end
  def ErrorWrite
    @fh.write("ERROR: " + ErrorGetText() + "\n")
    @fh.flush
    ErrorReset()
  end
  def LOGF
    ftr = ServiceGetFilter()
    if ftr.LinkGetTargetIdent() == "transFilter" then
      ftr.SendSTART()
      ftr.SendC(ReadC())
      ftr.SendEND_AND_WAIT("LOGF")
    else
      ftr.fh = File.new(ReadC(), "a")
    end
    SendRETURN()
  end
  def EXIT
    exit
  end
  def Event
    if @itms.length == 0 then
      ErrorSetCONTINUE()
    else
      ftr = ServiceGetFilter()
      begin
        ftr.LinkConnect
        ftr.SendBDY(@itms[0])
      rescue Exception => ex
        ftr.ErrorSet(ex)
        if ftr.ErrorIsEXIT then
          ftr.ErrorReset
          return
        else
          ftr.ErrorWrite
	end
      end
      @itms.shift
    end
  end
  def FilterIn
    @itms.push(ReadBDY())
    SendRETURN()
  end
end
srv = Filter4.new
begin
  srv.LinkCreate(ARGV)
  srv.ProcessEvent(MqS::Wait::FOREVER)
rescue SignalException => ex
  # ignore
rescue Exception => ex
  srv.ErrorSet(ex)
ensure
  srv.Exit()
end
