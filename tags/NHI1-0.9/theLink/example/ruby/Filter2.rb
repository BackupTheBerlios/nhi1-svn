#+
#§  \file       theLink/example/ruby/Filter2.rb
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

class Filter2 < MqS
  def initialize
    ConfigSetName("filter")
    ConfigSetFactory(lambda {Filter2.new})
    ConfigSetServerSetup(method(:ServerSetup))
    @data = []
    super()
  end
  def ServerSetup
    ServiceCreate("+FTR", method(:FTRcmd))
    ServiceProxy("+EOF")
  end
  def FTRcmd
    raise "my error"
  end
end
srv = Filter2.new
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


