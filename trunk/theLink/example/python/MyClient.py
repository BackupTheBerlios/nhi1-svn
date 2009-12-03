#+
#§  \file       example/python/MyClient.py
#§  \brief      \$Id: MyClient.py 507 2009-11-28 15:18:46Z dev1usr $
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
ctx = MqS()
try:
  ctx.ConfigSetName("MyClient")
  ctx.LinkCreate(sys.argv)
  ctx.SendSTART()
  ctx.SendEND_AND_WAIT("HLWO")
  print(ctx.ReadC())
except:
  ctx.ErrorSet();
finally:
  ctx.Exit()


