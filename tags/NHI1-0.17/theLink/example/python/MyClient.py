#+
#:  \file       theLink/example/python/MyClient.py
#:  \brief      \$Id$
#:  
#:  (C) 2009 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:
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


