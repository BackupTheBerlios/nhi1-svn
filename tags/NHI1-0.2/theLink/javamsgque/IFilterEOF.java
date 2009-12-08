/**
 *  \file       theLink/javamsgque/IFilterEOF.java
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

/// \sameas{#MqConfigSetFilterEOF}
public interface IFilterEOF {
  void EOF () throws MqSException;
}
