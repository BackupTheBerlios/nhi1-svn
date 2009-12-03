#+
#§  \file       example/python/manfilter.py
#§  \brief      \$Id: manfilter.py 507 2009-11-28 15:18:46Z dev1usr $
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

