#+
#§  \file       theLink/example/ruby/MyServer.rb
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

class MyServer < MqS
  def initialize(tmpl = nil)
    super()
    ConfigSetServerSetup(method(:ServerSetup))
  end
  def HLWO
    SendSTART()
    SendC("Hello World")
    SendRETURN()
  end
  def ServerSetup
    ServiceCreate("HLWO",method(:HLWO))
  end
end

srv = FactoryNew(MyServer)
begin
  srv.LinkCreate($0,ARGV)
  srv.ProcessEvent(MqS::WAIT_FOREVER)
rescue Exception => ex
  srv.ErrorSet(ex)
ensure
  srv.Exit()
end

