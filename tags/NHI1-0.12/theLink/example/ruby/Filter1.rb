#+
#§  \file       theLink/example/ruby/Filter1.rb
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

class Filter1 < MqS
  def initialize
    ConfigSetName("filter")
    ConfigSetServerSetup(method(:ServerSetup))
    @data = []
    super()
  end
  def ServerSetup
    ServiceCreate("+FTR", method(:FTRcmd))
    ServiceCreate("+EOF", method(:EOFcmd))
  end
  def FTRcmd
    list = []
    while ReadItemExists()
      list << "<" + ReadC() + ">"
    end
    @data << list
    SendRETURN()
  end
  def EOFcmd
    ftr = ServiceGetFilter()
    for list in @data
      ftr.SendSTART()
      for item in list
        ftr.SendC(item)
      end
      ftr.SendEND_AND_WAIT("+FTR")
    end
    ftr.SendSTART()
    ftr.SendEND_AND_WAIT("+EOF")
    SendRETURN()
  end
end
srv = Filter1.new
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


