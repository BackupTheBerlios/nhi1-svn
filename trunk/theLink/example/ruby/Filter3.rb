#+
#:  \file       theLink/example/ruby/Filter3.rb
#:  \brief      \$Id$
#:  
#:  (C) 2010 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

require "rubymsgque"

class Filter3 < MqS
  def initialize()
    super()
    ConfigSetServerSetup(method(:ServerSetup))
  end
  def ServerSetup
    ftr = ServiceGetFilter()
    ServiceProxy("+ALL")
    ServiceProxy("+TRT")
    ftr.ServiceProxy("+ALL")
    ftr.ServiceProxy("+TRT")
  end
end

srv = FactoryAdd(Filter3).New()
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

