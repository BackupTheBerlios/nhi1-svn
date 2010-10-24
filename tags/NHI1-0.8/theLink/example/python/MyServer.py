#+
#§  \file       theLink/example/python/MyServer.py
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
def HLWO(ctx):
  ctx.SendSTART()
  ctx.SendC("Hello World")
  ctx.SendRETURN()
def ServerConfig(ctx):
  ctx.ServiceCreate("HLWO",HLWO)
srv = MqS()
try:
  srv.ConfigSetServerSetup(ServerConfig)
  srv.LinkCreate(sys.argv)
  srv.ProcessEvent(wait="FOREVER")
except:
  srv.ErrorSet()
finally:
  srv.Exit()



