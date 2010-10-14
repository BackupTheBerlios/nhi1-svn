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
def HLWO(ctx)
  ctx.SendSTART()
  ctx.SendC("Hello World")
  ctx.SendRETURN()
end
def ServerConfig(ctx)
  ctx.ServiceCreate("HLWO",method(:HLWO))
end
srv = MqS.new
begin
  srv.ConfigSetServerSetup(method(:ServerConfig))
  srv.LinkCreate($0,ARGV)
  srv.ProcessEvent(MqS::WAIT::FOREVER)
rescue Exception => ex
  srv.ErrorSet(ex)
ensure
  srv.Exit()
end
