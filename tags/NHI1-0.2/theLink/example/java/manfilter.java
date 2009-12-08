/**
 *  \file       theLink/example/java/manfilter.java
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
package example;
import javamsgque.*;
class manfilter extends MqS implements IFilterFTR {
  public void FTR () throws MqSException {
    SendSTART();
    while (ReadItemExists()) {
      SendC("<" + ReadC() + ">");
    }
    SendFTR(10);
  }
  public static void main(String[] argv) {
    manfilter srv = new manfilter();
    try {
      srv.ConfigSetName("filter");
      srv.LinkCreate(argv);
      srv.ProcessEvent(MqS.WAIT.FOREVER);
    } catch (Throwable e) {
      srv.ErrorSet(e);
    }
    srv.Exit();
  }
}
