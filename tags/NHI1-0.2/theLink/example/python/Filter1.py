#+
#§  \file       theLink/example/python/Filter1.py
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
data = []
def FTRcmd(ctx):
  L = []
  while ctx.ReadItemExists():
    L.append("<" + ctx.ReadC() + ">")
  data.append(L)
def EOFcmd(ctx):
  for L in data:
    ctx.SendSTART()
    for I in L:
      ctx.SendC(I)
    ctx.SendFTR()
srv = MqS()
try:
  srv.ConfigSetFilterFTR(FTRcmd)
  srv.ConfigSetFilterEOF(EOFcmd)
  srv.ConfigSetName("filter")
  srv.LinkCreate(sys.argv)
  srv.ProcessEvent(wait="FOREVER")
except:
  srv.ErrorSet()
finally:
  srv.Exit()

