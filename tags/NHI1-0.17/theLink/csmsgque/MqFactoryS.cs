/**
 *  \file       theLink/csmsgque/MqFactoryS.cs
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

using System;
using System.Reflection;
using System.Runtime.InteropServices;

namespace csmsgque {

  /// \defgroup Mq_Factory_API Mq_Factory_API
  /// \ingroup Mq_Cs_API
  /// \brief \copybrief MqFactory
  /// \details \copydetails MqFactory
  /// \{

  internal enum MqFactoryE :int {
    MQ_FACTORY_NEW_INIT,
    MQ_FACTORY_NEW_CHILD,
    MQ_FACTORY_NEW_SLAVE,
    MQ_FACTORY_NEW_FORK,
    MQ_FACTORY_NEW_THREAD,
    MQ_FACTORY_NEW_FILTER
  };

  internal enum MqFactoryReturnE :int {
  /* 0 */ MQ_FACTORY_RETURN_OK,
  /* 1 */ MQ_FACTORY_RETURN_ADD_DEF_ERR,
  /* 2 */ MQ_FACTORY_RETURN_ADD_IDENT_IN_USE_ERR,
  /* 3 */ MQ_FACTORY_RETURN_CALL_ERR,
  /* 4 */ MQ_FACTORY_RETURN_ITEM_GET_ERR,
  /* 5 */ MQ_FACTORY_RETURN_NEW_ERR,
  /* 6 */ MQ_FACTORY_RETURN_DEFAULT_ERR,
  /* 7 */ MQ_FACTORY_RETURN_ADD_ERR,
  };

  /// \api \ref MqFactoryDataFreeF
  [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
  internal delegate void FactoryDataFreeF ([In,Out] ref IntPtr data);

  /// \api \ref MqFactoryDataCopyF
  [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
  internal delegate void FactoryDataCopyF ([In,Out] ref IntPtr data);

  /// \api #MqFactoryCreateF
  [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
  internal delegate MqErrorE FactoryCreateF ([In]IntPtr tmpl, MqFactoryE create, [In]IntPtr item, [In,Out] ref IntPtr contextP);

  /// \api #MqFactoryDeleteF
  [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
  internal delegate void FactoryDeleteF ([In]IntPtr context, [In] MQ_BOL doFactoryCleanup, [In]IntPtr data);

  /// \brief Factory exception
  internal class MqFactorySException : Exception
  {
    internal MqFactorySException() : base("MqFactoryS exception") {}
  }


  /// \ingroup Mq_Factory_Cs_API
  /// \api #MqFactoryS
  public class MqFactoryS<T> where T : MqS
  {

    static MqFactoryS() {
      // init default factory
      if (MqFactoryS<MqS>.DefaultIdent() == "libmsgque") {
	MqFactoryS<MqS>.Default ("csmsgque");
      }
    }

  /*****************************************************************************/
  /*                                                                           */
  /*                                Init                                       */
  /*                                                                           */
  /*****************************************************************************/

    static private FactoryCreateF   fFactoryCreate    = FactoryCreate;
    static private FactoryDeleteF   fFactoryDelete    = FactoryDelete;
    static private FactoryDataCopyF fFactoryDataCopy  = FactoryCopy;

    public IntPtr factory;

    public MqFactoryS (IntPtr hdl) {
//DEBUG.P("MqFactoryS", hdl);
      if (hdl == IntPtr.Zero) {
	throw new MqFactorySException();
      } else {
	factory = hdl;
      }
    }

    /*****************************************************************************/
  /*                                                                           */
  /*                               private                                     */
  /*                                                                           */
  /*****************************************************************************/

  //struct MqFactoryItemS* const item,
    static private MqErrorE FactoryCreate (IntPtr tmpl, MqFactoryE create, IntPtr item, ref IntPtr contextP) {
      try {
	Object[] args = new Object[1];
	args[0] = tmpl != IntPtr.Zero ? MqS.GetSelf(tmpl) : null;
	IntPtr data = MqS.MqFactoryItemGetCreateData(item);
	contextP = ((MqS)((ConstructorInfo)GCHandle.FromIntPtr(data).Target).Invoke(args)).context;
      } catch (Exception ex) {
	if (create == MqFactoryE.MQ_FACTORY_NEW_INIT) {
	  return MqErrorE.MQ_ERROR;
	} else {
	  return MqS.MqErrorSet2 (tmpl, ex);
	}
      }
      if (create != MqFactoryE.MQ_FACTORY_NEW_INIT) {
	MqS.MqSetupDup (contextP, tmpl);
      }
      return MqErrorE.MQ_OK;
    }

    static private void FactoryDelete (IntPtr context, MQ_BOL doFactoryCleanup, IntPtr item) {
      IntPtr self = MqS.MqConfigGetSelf(context);
      MqS.MqContextDelete (ref MqS.GetSelf(context).context);
      GCHandle.FromIntPtr(self).Free();
    }

    private static void FactoryCopy (ref IntPtr data) {
      data = (IntPtr)GCHandle.Alloc((ConstructorInfo)GCHandle.FromIntPtr(data).Target);
    }

    static private IntPtr getConstructor () {
      Type[] types = new Type[1];
      types[0] = typeof(MqS);
      ConstructorInfo constr = typeof(T).GetConstructor(types);
      if (constr == null) {
	throw new System.Exception("unable to get Factory constructor");
      } else {
	return (IntPtr) GCHandle.Alloc(constr);
      }
    }

  /*****************************************************************************/
  /*                                                                           */
  /*                                 public                                    */
  /*                                                                           */
  /*****************************************************************************/

    /// \api #MqFactoryAdd
    public static MqFactoryS<T> Add(string ident) {
      IntPtr data = getConstructor();
      // !attention, no "MqS.fProcFree" because the code will core at the end
//DEBUG.O(ident, typeof(T).Name);
      return new MqFactoryS<T>(
	MqS.MqFactoryAdd(new IntPtr(0x2), ident, 
	  fFactoryCreate, data, null, fFactoryDataCopy, fFactoryDelete, IntPtr.Zero, null, null)
      );
    }

    /// \api #MqFactoryAdd
    public static MqFactoryS<T> Add() {
      IntPtr data = getConstructor();
//DEBUG.P(typeof(T).Name, data);
      // !attention, no "MqS.fProcFree" because the code will core at the end
      return new MqFactoryS<T>(
	MqS.MqFactoryAdd(new IntPtr(0x2), typeof(T).Name, 
	  fFactoryCreate, data, null, fFactoryDataCopy, fFactoryDelete, IntPtr.Zero, null, null)
      );
    }

    /// \api #MqFactoryDefault
    public static MqFactoryS<T> Default(string ident) {
      IntPtr data = getConstructor();
//DEBUG.O(ident, typeof(T).Name);
      // !attention, no "MqS.fProcFree" because the code will core at the end
      return new MqFactoryS<T>(
	MqS.MqFactoryDefault(new IntPtr(0x2), ident, 
	  fFactoryCreate, data, null, fFactoryDataCopy, fFactoryDelete, IntPtr.Zero, null, null)
      );
    }

    /// \api #MqFactoryDefault
    public static MqFactoryS<T> Default() {
      IntPtr data = getConstructor();
      // !attention, no "MqS.fProcFree" because the code will core at the end
//DEBUG.P(ident,data);
      return new MqFactoryS<T>(
	MqS.MqFactoryDefault(new IntPtr(0x2), typeof(T).Name, 
	  fFactoryCreate, data, null, fFactoryDataCopy, fFactoryDelete, IntPtr.Zero, null, null)
      );
    }

    /// \api #MqFactoryDefaultIdent
    public static String DefaultIdent() {
      String str = Marshal.PtrToStringAnsi(MqS.MqFactoryDefaultIdent());
//DEBUG.O(str, typeof(T).Name);
      return str;
    }

    /// \api #MqFactoryGet
    public static MqFactoryS<T> Get(string ident) {
      return new MqFactoryS<T>(MqS.MqFactoryGet(ident));
    }

    /// \api #MqFactoryGet
    public static MqFactoryS<T> Get() {
      return new MqFactoryS<T>(MqS.MqFactoryGet(""));
    }

    /// \api #MqFactoryGetCalled
    public static MqFactoryS<T> GetCalled(string ident) {
      return new MqFactoryS<T>(MqS.MqFactoryGetCalled(ident));
    }

    /// \api #MqFactoryNew
    public T New() {
      IntPtr mqctx = MqS.MqFactoryNew(new IntPtr(0x2), IntPtr.Zero, factory);
      if (mqctx == IntPtr.Zero) {
	throw new MqFactorySException();
      } else {
	return (T) MqS.GetSelf(mqctx);
      }
    }

    /// \api #MqFactoryCopy
    public MqFactoryS<T> Copy(string ident) {
      return new MqFactoryS<T> (MqS.MqFactoryCopy(factory, ident));
    }

  } // END - class "MqFactoryS"

  /// \}

} // END - namespace "csmsgque"


