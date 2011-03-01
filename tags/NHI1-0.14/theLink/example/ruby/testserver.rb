#+
#§  \file       theLink/example/ruby/testserver.rb
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

class TestServer < MqS
  def initialize(tmpl = nil)
    super(tmpl)
    ConfigSetServerSetup(method(:ServerConfig))
  end
  def GTCX
    SendSTART()
    SendI(LinkGetCtxId())
    SendC("+")
    if (LinkIsParent())
      SendI(-1)
    else
      SendI(LinkGetParent().LinkGetCtxId())
    end
    SendC("+")
    SendC(ConfigGetName())
    SendC(":")
    SendRETURN()
  end
  def ServerConfig
    ServiceCreate("GTCX",method(:GTCX))
  end
end
srv = FactoryAdd(TestServer).New()
begin
  srv.LinkCreate($0,ARGV)
  srv.ProcessEvent(MqS::WAIT_FOREVER)
rescue Exception => ex
  srv.ErrorSet(ex)
ensure
  srv.Exit()
end


