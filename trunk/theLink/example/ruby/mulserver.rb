#+
#§  \file       theLink/example/ruby/mulserver.rb
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
def MMUL(ctx):
  ctx.SendSTART()
  ctx.SendD(ctx.ReadD() * ctx.ReadD())
  ctx.SendRETURN()
def ServerConfig(ctx):
  ctx.ServiceCreate("MMUL",MMUL)
srv = MqS()
try:
  srv.ConfigSetName("MyMulServer")
  srv.ConfigSetServerSetup(ServerConfig)
  srv.LinkCreate(sys.argv)
  srv.ProcessEvent(wait="FOREVER")
except:
  srv.ErrorSet()
finally:
  srv.Exit()
