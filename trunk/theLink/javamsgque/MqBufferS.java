/**
 *  \file       theLink/javamsgque/MqBufferS.java
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
/// \ingroup Mq_Java_API
/// \defgroup Mq_Buffer_Java_API Mq_Buffer_Java_API
/// \brief \copybrief Mq_Buffer_C_API
/// \copydoc Mq_Buffer_C_API
/// \{
//

///\brief \api #MqBufferS
public class MqBufferS {

  private long hdl = 0L;

  // INTERNAL

  private MqBufferS (long data) {
    hdl = data;
  }

  // PUBLIC

  /// \copybrief MqBufferGetType
  public native char GetType ();

  /// \copybrief MqBufferGetY
  public native byte GetY ();

  /// \copybrief MqBufferGetY
  public native boolean GetO ();

  /// \copybrief MqBufferGetY
  public native short GetS ();

  /// \copybrief MqBufferGetY
  public native int GetI ();

  /// \copybrief MqBufferGetY
  public native float GetF ();

  /// \copybrief MqBufferGetY
  public native long GetW ();

  /// \copybrief MqBufferGetY
  public native double GetD ();

  /// \copybrief MqBufferGetY
  public native byte[] GetB ();

  /// \copybrief MqBufferGetY
  public native String GetC ();
}

/// \} Mq_Buffer_Java_API
