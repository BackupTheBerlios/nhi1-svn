#+
#§  \file       theLink/example/ruby/Filter5.rb
#§  \brief      \$Id$
#§  
#§  (C) 2011 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

require "rubymsgque"

# F1 ********************************************************

class F1 < MqS
  def initialize(tmpl = nil)
    super(tmpl)
    ConfigSetServerSetup(method(:ServerSetup))
  end
  def ServerSetup
    ServiceCreate("+FTR", method(:FTRcmd))
    ServiceProxy("+EOF")
  end
  def FTRcmd
    ftr = ServiceGetFilter()
    ftr.SendSTART()
    ftr.SendC("F1")
    ftr.SendC(ftr.ConfigGetName())
    ftr.SendI(ftr.ConfigGetStartAs())
    ftr.SendI(ConfigGetStatusIs())
    while ReadItemExists()
      ftr.SendC(ReadC())
    end
    ftr.SendEND_AND_WAIT("+FTR")
    SendRETURN()
  end
end

# F2 ********************************************************

class F2 < MqS
  def initialize(tmpl = nil)
    super(tmpl)
    ConfigSetServerSetup(method(:ServerSetup))
  end
  def ServerSetup
    ServiceCreate("+FTR", method(:FTRcmd))
    ServiceProxy("+EOF")
  end
  def FTRcmd
    ftr = ServiceGetFilter()
    ftr.SendSTART()
    ftr.SendC("F2")
    ftr.SendC(ftr.ConfigGetName())
    ftr.SendI(ftr.ConfigGetStartAs())
    ftr.SendI(ConfigGetStatusIs())
    while ReadItemExists()
      ftr.SendC(ReadC())
    end
    ftr.SendEND_AND_WAIT("+FTR")
    SendRETURN()
  end
end

# F3 ********************************************************

class F3 < MqS
  def initialize(tmpl = nil)
    super(tmpl)
    ConfigSetServerSetup(method(:ServerSetup))
  end
  def ServerSetup
    ServiceCreate("+FTR", method(:FTRcmd))
    ServiceProxy("+EOF")
  end
  def FTRcmd
    ftr = ServiceGetFilter()
    ftr.SendSTART()
    ftr.SendC("F3")
    ftr.SendC(ftr.ConfigGetName())
    ftr.SendI(ftr.ConfigGetStartAs())
    ftr.SendI(ConfigGetStatusIs())
    while ReadItemExists()
      ftr.SendC(ReadC())
    end
    ftr.SendEND_AND_WAIT("+FTR")
    SendRETURN()
  end
end

# main ******************************************************

FactoryAdd(F1) 
FactoryAdd(F2) 
FactoryAdd(F3) 

srv = FactoryCall(ARGV[0])
begin
  srv.LinkCreate($0,ARGV)
  srv.ProcessEvent(MqS::WAIT_FOREVER)
rescue SignalException => ex
  # ignore
rescue Exception => ex
  srv.ErrorSet(ex)
ensure
  srv.Exit()
end


