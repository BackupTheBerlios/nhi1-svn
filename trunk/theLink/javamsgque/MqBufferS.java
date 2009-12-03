/**
 *  \file       theLink/javamsgque/MqBufferS.java
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

package javamsgque;

//
// ========================================================================
/// \ingroup javamain_api
/// \defgroup java_MqBufferS_api MqBufferS
/// \{
/// \brief \api #MqBufferS
//

///\brief \api #MqBufferS
public class MqBufferS {

  private long hdl = 0L;

  // INTERNAL

  private MqBufferS (long data) {
    hdl = data;
  }

  // PUBLIC

  /// \api #MqBufferGetType
  ///
  /// Use the following sequence to check and extract typeless data
  /// \code
  /// MqBufferS buf = ctx.ReadU();
  /// if (buf.GetType() == 'I') {
  ///   int i = buf.GetI();
  ///   ...
  /// }
  /// \endcode
  public native char GetType ();

  /// \api #MqBufferGetY
  public native byte GetY ();

  /// \api #MqBufferGetO
  public native boolean GetO ();

  /// \api #MqBufferGetS
  public native short GetS ();

  /// \api #MqBufferGetI
  public native int GetI ();

  /// \api #MqBufferGetF
  public native float GetF ();

  /// \api #MqBufferGetW
  public native long GetW ();

  /// \api #MqBufferGetD
  public native double GetD ();

  /// \api #MqBufferGetB
  public native byte[] GetB ();

  /// \api #MqBufferGetC
  public native String GetC ();
}

/// \}
