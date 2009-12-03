/**
 *  \file       theLink/javamsgque/IService.java
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

/// \brief Interface required to setup a service
public interface IService {
  /// \brief function to handle the service request
  void Service (MqS ctx) throws MqSException;
}

