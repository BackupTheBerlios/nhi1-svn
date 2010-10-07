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
class testserver(MqS):
  def GTCX(ctx):
    ctx.SendSTART()
    ctx.SendI(ctx.LinkGetCtxId())
    ctx.SendC("+")
    if (ctx.LinkIsParent()):
      ctx.SendI(-1)
    else:
      ctx.SendI(ctx.LinkGetParent().LinkGetCtxId())
    ctx.SendC("+");
    ctx.SendC(ctx.ConfigGetName());
    ctx.SendC(":")
    ctx.SendRETURN()
  def ServerConfig(ctx):
    ctx.ServiceCreate("GTCX",ctx.GTCX)

if __name__ == "__main__":
  srv = testserver()
  try:
    srv.ConfigSetName("testserver")
    srv.ConfigSetServerSetup(srv.ServerConfig)
    srv.ConfigSetFactory(lambda: testserver())
    srv.LinkCreate(sys.argv)
    srv.ProcessEvent(wait="FOREVER")
  except:
    srv.ErrorSet()
  finally:
    srv.Exit()
