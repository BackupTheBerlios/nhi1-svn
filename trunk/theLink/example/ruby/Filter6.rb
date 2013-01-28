#+
#:  \file       theLink/example/ruby/Filter6.rb
#:  \brief      \$Id$
#:  
#:  (C) 2011 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

require "rubymsgque"

class Filter6 < MqS
  attr_accessor :fh
  def initialize(tmpl=nil)
    super(tmpl)
    ConfigSetIgnoreExit(true)
    ConfigSetServerSetup(method(:ServerSetup))
    ConfigSetServerCleanup(method(:ServerCleanup))
    ConfigSetEvent(method(:Event))
    @fh = nil
  end
  def ServerCleanup
    @fh.close if @fh != nil
    @fh = nil
  end
  def ServerSetup
    ftr = ServiceGetFilter()
    ServiceCreate("LOGF", method(:LOGF))
    ServiceCreate("EXIT", method(:EXIT))
    ServiceCreate("+ALL", method(:FilterIn))
    ServiceStorage("PRNT")
    ftr.ServiceCreate("WRIT", ftr.method(:WRIT))
    ftr.ServiceProxy("+TRT")
  end
  def WRIT
    fh = ServiceGetFilter().fh
    fh.write(ReadC() + "\n")
    fh.flush()
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
      ReadForward(ftr)
    else
      @fh = File.new(ReadC(), "a")
    end
    SendRETURN()
  end
  def EXIT
    Process.exit!(1)
  end
  def Event
    if StorageCount() == 0 then
      ErrorSetCONTINUE()
    else
      id = 0
      begin
	ftr = ServiceGetFilter()
        ftr.LinkConnect
	id = StorageSelect()
	ReadForward(ftr)
      rescue Exception => ex
        ErrorSet(ex)
        if ErrorIsEXIT() then
          ErrorReset()
          return
        else
          ErrorWrite()
	end
      end
      StorageDelete(id)
    end
  end
  def FilterIn
    StorageInsert()
    SendRETURN()
  end
end
srv = FactoryAdd("transFilter", Filter6).New()
begin
  srv.LinkCreate($0,ARGV)
  srv.ProcessEvent(MqS::WAIT_FOREVER)
rescue SignalException => ex
  # ignore
rescue Exception => ex
  srv.ErrorSet(ex)
ensure
  srv.Exit()
end

