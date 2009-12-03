/**
 *  \file       theLink/javamsgque/ICallback.java
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

/// \brief Interface required to setup a callback
///
/// The callback is required by the the second parameter from 
/// #javamsgque.MqS.SendEND_AND_CALLBACK
public interface ICallback {
  /// \brief function to handle the callback request
  void Callback (MqS ctx) throws MqSException;
}

