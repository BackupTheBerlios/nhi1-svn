/**
 *  \file       theLink/example/java/Filter1.java
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
import java.util.ArrayList;
import javamsgque.*;

class Filter1 extends MqS implements IFilterFTR, IFilterEOF {
  private ArrayList<ArrayList<String>> data = new ArrayList<ArrayList<String>>();

  // service definition
  public void FTR () throws MqSException {
    ArrayList<String> d = new ArrayList<String>();
    while (ReadGetNumItems() != 0) {
      d.add("<" + ReadC() + ">");
    }
    data.add(d);
  }

  // service definition
  public void EOF () throws MqSException {
    for (ArrayList<String> d: data) {
      SendSTART();
      for (String s: d) {
	SendC(s);
      }
      SendFTR(10);
    }
  }

  public static void main(String[] argv) {
    Filter1 srv = new Filter1();
    try {
      srv.ConfigSetName("filter");
      srv.LinkCreate(argv);
      srv.ProcessEvent(MqS.WAIT.FOREVER);
    } catch (Throwable e) {
      srv.ErrorSet(e);
    } finally {
      srv.Exit();
    }
  }
}
