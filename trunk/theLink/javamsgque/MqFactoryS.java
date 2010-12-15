/**
 *  \file       theLink/javamsgque/MqS.java
 *  \brief      \$Id: MqS.java 278 2010-11-07 17:50:58Z aotto1968 $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 278 $
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

/// \defgroup Mq_Java_API Mq_Java_API
/// \{
/// \brief \copybrief Mq_C_API
/// \copydoc Mq_C_API

/// \brief The \b JavaMsgque Package
package javamsgque;

/// \brief \api #MqS
public class MqFactoryS <T extends MqS> {

  public static native <T extends MqS> void Add  (String ident, Class<T> callback) 
    throws NoSuchMethodError, ExceptionInInitializerError, OutOfMemoryError ;
  public static native <T extends MqS> T  Call (String ident);
  public static native <T extends MqS> T  New  (String ident, Class<T> callback) 
    throws NoSuchMethodError, ExceptionInInitializerError, OutOfMemoryError ;
}

/// \} Mq_Java_API


