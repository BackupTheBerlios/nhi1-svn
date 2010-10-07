#+
#§  \file       theLink/example/ruby/mulclient.rb
#§  \brief      \$Id: LbMain 26 2009-12-03 11:48:43Z aotto1968 $
#§  
#§  (C) 2010 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 26 $
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§
require "rubymsgque"
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
