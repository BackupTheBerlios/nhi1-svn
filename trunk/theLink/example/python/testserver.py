#+
#§  \file       theLink/example/python/testserver.py
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§
import sys
from pymsgque import *
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
  srv = FactoryAdd(testserver).New()
  try:
    srv.ConfigSetServerSetup(srv.ServerConfig)
    srv.LinkCreate(sys.argv)
    srv.ProcessEvent(MqS_WAIT_FOREVER)
  except:
    srv.ErrorSet()
  finally:
    srv.Exit()






