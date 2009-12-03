#+
#§  \file       theLink/example/python/manfilter.py
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
def FTRcmd(ctx):
  ctx.SendSTART()
  while ctx.ReadItemExists():
    ctx.SendC("<" + ctx.ReadC() + ">")
  ctx.SendFTR()
srv = MqS()
try:
  srv.ConfigSetFilterFTR(FTRcmd)
  srv.ConfigSetName("ManFilter")
  srv.LinkCreate(sys.argv)
  srv.ProcessEvent(wait="FOREVER")
except:
  srv.ErrorSet()
finally:
  srv.Exit()

