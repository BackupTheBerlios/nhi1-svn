#+
#:  \file       theLink/example/ruby/MyServer.rb
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

srv = FactoryAdd(MyServer).New()
begin
  srv.LinkCreate($0,ARGV)
  srv.ProcessEvent(MqS::WAIT_FOREVER)
rescue Exception => ex
  srv.ErrorSet(ex)
ensure
  srv.Exit()
end

