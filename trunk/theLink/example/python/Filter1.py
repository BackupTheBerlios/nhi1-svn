#+
#§  \file       example/python/Filter1.py
#§  \brief      \$Id: Filter1.py 507 2009-11-28 15:18:46Z dev1usr $
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

