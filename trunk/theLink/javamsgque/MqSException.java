/**
 *  \file       theLink/javamsgque/MqSException.java
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

///\brief The \javamsgque MqS Package
package javamsgque;

///\brief Java-Msgque Super-Class
public class MqSException extends Exception {

  private int p_num;
  private int  p_code;
  private String p_txt;

  private MqSException (int mynum, int mycode, String mytext) {
    super (mytext);
    p_txt = mytext;
    p_num = mynum;
    p_code = mycode;
  }

  public int getNum () {
    return p_num;
  }

  public String getTxt () {
    return p_txt;
  }
}
