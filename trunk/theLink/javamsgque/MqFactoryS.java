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

//
// ========================================================================
/// \ingroup Mq_Java_API
/// \defgroup Mq_Factory_API Mq_Factory_API
/// \{
/// \brief \copybrief MqFactory
/// \details \copydetails MqFactory

/// \brief The \b JavaMsgque Package
package javamsgque;

/// \api #MqFactoryS
public class MqFactoryS <T extends MqS> {

  // INTERNAL

  private MqFactoryS (long hdl) {
    factory = hdl;
  }

  // PUBLIC

  public long factory = 0;

  /// \api #MqFactoryAdd
  public static native <T extends MqS> MqFactoryS<T> Add (String ident, Class<T> callback) 
    throws NoSuchMethodError, ExceptionInInitializerError, OutOfMemoryError ;

  /// \api #MqFactoryAdd
  public static <T extends MqS> MqFactoryS<T> Add (Class<T> callback) 
    throws NoSuchMethodError, ExceptionInInitializerError, OutOfMemoryError {
    return Add(callback.getName(), callback);
  }

  /// \api #MqFactoryDefault
  public static native <T extends MqS> MqFactoryS<T> Default (String ident, Class<T> callback) 
    throws NoSuchMethodError, ExceptionInInitializerError, OutOfMemoryError ;

  /// \api #MqFactoryDefault
  public static <T extends MqS> MqFactoryS<T> Default (Class<T> callback) 
    throws NoSuchMethodError, ExceptionInInitializerError, OutOfMemoryError {
    return Default(callback.getName(), callback);
  }

  /// \api #MqFactoryDefaultIdent
  public static native <T extends MqS> String DefaultIdent ();

  /// \api #MqFactoryGet
  public static native <T extends MqS> MqFactoryS<T> Get (String ident);

  /// \api #MqFactoryGet
  public static native <T extends MqS> MqFactoryS<T> Get ();

  /// \api #MqFactoryGetCalled
  public static native <T extends MqS> MqFactoryS<T> GetCalled (String ident);

  /// \api #MqFactoryNew
  public native <T extends MqS> T New () ;

  /// \api #MqFactoryCopy
  public native <T extends MqS> MqFactoryS<T> Copy (String ident);
}

/// \} Mq_Factory_API
