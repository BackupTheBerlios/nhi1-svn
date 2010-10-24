#+
#§  \file       theLink/example/ruby/ManFilter.rb
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
class ManFilter < MqS
  def initialize
    ConfigSetName("filter")
    ConfigSetFactory(lambda {ManFilter.new})
    ConfigSetServerSetup(method(:ServerSetup))
    super()
  end
  def ServerSetup
    ServiceCreate("+FTR", method(:FTRcmd))
    ServiceProxy("+EOF")
  end
  def FTRcmd
    ftr = ServiceGetFilter()
    ftr.SendSTART()
    while ReadItemExists()
      ftr.SendC("<" + ReadC() + ">")
    end
    ftr.SendEND_AND_WAIT("+FTR")
    SendRETURN()
  end
end
srv = ManFilter.new
begin
  srv.LinkCreate($0,ARGV)
  srv.ProcessEvent(MqS::WAIT::FOREVER)
rescue SignalException => ex
  # ignore
rescue Exception => ex
  srv.ErrorSet(ex)
ensure
  srv.Exit()
end

