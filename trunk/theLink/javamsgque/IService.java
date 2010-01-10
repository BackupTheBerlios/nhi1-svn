/**
 *  \file       theLink/javamsgque/IService.java
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

/// \ingroup Mq_Service_Java_API
/// \brief \api #MqTokenF
public interface IService {
  void Service (MqS ctx) throws MqSException;
}

