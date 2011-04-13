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
/// \{
/// \brief \copybrief Mq_Dump_C_API
/// \details \copydetails Mq_Dump_C_API
//

public class MqDumpS {

  private long hdl = 0L;
  
  private native long Free();

  /// \api #MqDumpSize
  public  native int  Size();

  private MqDumpS (long data) {
    hdl = data;
  }

  public void finalize () {
    hdl = Free();
  }
}

/// \} Mq_Dump_Java_API
