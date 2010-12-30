/**
 *  \file       theLink/javamsgque/MqFactoryS.java
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
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

/// \brief \api #MqFactoryS
public class MqFactoryS <T extends MqS> {
  static {
    if (DefaultIdent().equals("libmsgque")) {
      Default("javamsgque", MqS.class);
    }
  }

  public static native <T extends MqS> void Default  (String ident, Class<T> callback) 
    throws NoSuchMethodError, ExceptionInInitializerError, OutOfMemoryError ;
  public static <T extends MqS> void  Default  (Class<T> callback) 
    throws NoSuchMethodError, ExceptionInInitializerError, OutOfMemoryError {
    Default(callback.getName(), callback);
  }
  public static native <T extends MqS> String DefaultIdent ();
  public static native <T extends MqS> void Add  (String ident, Class<T> callback) 
    throws NoSuchMethodError, ExceptionInInitializerError, OutOfMemoryError ;
  public static <T extends MqS> void  Add  (Class<T> callback) 
    throws NoSuchMethodError, ExceptionInInitializerError, OutOfMemoryError {
    Add(callback.getName(), callback);
  }
  public static native <T extends MqS> T  Call (String ident);
  public static native <T extends MqS> T  New  (String ident, Class<T> callback) 
    throws NoSuchMethodError, ExceptionInInitializerError, OutOfMemoryError ;
  public static <T extends MqS> T  New  (Class<T> callback) 
    throws NoSuchMethodError, ExceptionInInitializerError, OutOfMemoryError {
    return New(callback.getName(), callback);
  }
}

/// \} Mq_Java_API


