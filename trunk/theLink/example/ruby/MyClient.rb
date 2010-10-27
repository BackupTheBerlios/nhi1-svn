#+
#§  \file       theLink/example/ruby/MyClient.rb
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
ctx = MqS.new
begin
  ctx.ConfigSetName("MyClient")
  ctx.LinkCreate($0,ARGV)
  ctx.SendSTART()
  ctx.SendEND_AND_WAIT("HLWO")
  $stdout.puts ctx.ReadC()
  $stdout.flush
rescue Exception => ex
  ctx.ErrorSet(ex)
ensure
  ctx.Exit()
end