/**
 *  \file       theLink/javamsgque/MqSException.java
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

package javamsgque;

//
// ========================================================================
/// \defgroup Mq_Error_Java_API Mq_Error_Java_API
/// \ingroup Mq_Java_API
/// \{
/// \copydoc Mq_Error_C_API
//

/// \ingroup Mq_Java_Error_API
/// \api #MqErrorS
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

/// \} Mq_Error_Java_API
