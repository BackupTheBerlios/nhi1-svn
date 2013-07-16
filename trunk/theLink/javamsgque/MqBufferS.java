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

///\brief #MqBufferS
public class MqBufferS {

  private long hdl = 0L;

  // INTERNAL

  private MqBufferS (long data) {
    hdl = data;
  }

  /// \copybrief MqBufferDup
  public native MqBufferS Dup ();


  /// \copybrief MqBufferDelete
  public native void Delete ();

  // PUBLIC

  /// \copybrief MqBufferGetType
  public native char GetType ();

  /// \copybrief MqBufferGetY
  public native byte GetY ();
  /// \copybrief MqBufferGetO
  public native boolean GetO ();
  /// \copybrief MqBufferGetS
  public native short GetS ();
  /// \copybrief MqBufferGetI
  public native int GetI ();
  /// \copybrief MqBufferGetF
  public native float GetF ();
  /// \copybrief MqBufferGetW
  public native long GetW ();
  /// \copybrief MqBufferGetD
  public native double GetD ();
  /// \copybrief MqBufferGetC
  public native String GetC ();
  /// \copybrief MqBufferGetB
  public native byte[] GetB ();

  /// \copybrief MqBufferSetY
  public native MqBufferS SetY (byte val);
  /// \copybrief MqBufferSetO
  public native MqBufferS SetO (boolean val);
  /// \copybrief MqBufferSetS
  public native MqBufferS SetS (short val);
  /// \copybrief MqBufferSetI
  public native MqBufferS SetI (int val);
  /// \copybrief MqBufferSetF
  public native MqBufferS SetF (float val);
  /// \copybrief MqBufferSetW
  public native MqBufferS SetW (long val);
  /// \copybrief MqBufferSetD
  public native MqBufferS SetD (double val);
  /// \copybrief MqBufferSetC
  public native MqBufferS SetC (String val);
  /// \copybrief MqBufferSetB
  public native MqBufferS SetB (byte[] val);
}

/// \} Mq_Buffer_Java_API
