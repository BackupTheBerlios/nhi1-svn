/**
 *  \file       theLink/javamsgque/MqDumpS.java
 *  \brief      \$Id$
 *  
 *  (C) 2011 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

package javamsgque;

//
// ========================================================================
/// \ingroup Mq_Java_API
/// \defgroup Mq_Dump_Java_API Mq_Dump_Java_API
/// \brief package data dump object type
/// \{
//

///\brief #MqDumpS
public class MqDumpS {

  private long hdl = 0L;
  
  private native long DumpFree();

  private MqDumpS (long data) {
    hdl = data;
  }

  public void finalize () {
    hdl = DumpFree();
  }
}

/// \} Mq_Dump_Java_API
