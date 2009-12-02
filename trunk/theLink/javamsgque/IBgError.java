/**
 *  \file       javamsgque/IBgError.java
 *  \brief      \$Id: IBgError.java 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

package javamsgque;

/// \brief Interface required to setup a callback
///
/// The callback is required by the the second parameter from 
/// #javamsgque.MqS.SendEND_AND_CALLBACK
public interface IBgError {
  /// \brief function to handle the callback request
  void BgError () throws MqSException;
}

