/**
 *  \file       javamsgque/IServerCleanup.java
 *  \brief      \$Id: IServerCleanup.java 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

package javamsgque;

/// \sameas{#MqConfigSetServerCleanup}
public interface IServerCleanup {
  void ServerCleanup () throws MqSException ;
}
