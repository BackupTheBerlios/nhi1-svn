#+
#§  \file       theLink/example/python/mulclient.py
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
ctx = MqS()
try:
  ctx.ConfigSetName("MyMul")
  ctx.LinkCreate(sys.argv)
  ctx.SendSTART()
  ctx.SendD(3.67)
  ctx.SendD(22.3)
  ctx.SendEND_AND_WAIT("MMUL")
  print(ctx.ReadD())
except:
  ctx.ErrorSet()
finally:
  ctx.Exit()


