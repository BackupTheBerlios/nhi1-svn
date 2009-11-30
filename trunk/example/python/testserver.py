#+
#§  \file       example/python/testserver.py
#§  \brief      \$Id: testserver.py 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§
import sys
from pymsgque import *
class testserver(MqS):
  def GTCX(ctx):
    ctx.SendSTART()
    ctx.SendI(ctx.ConfigGetCtxId())
    ctx.SendC("+")
    if (ctx.ConfigGetIsParent()):
      ctx.SendI(-1)
    else:
      ctx.SendI(ctx.ConfigGetParent().ConfigGetCtxId())
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

