#+
#§  \file       theLink/example/python/testserver.py
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
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

