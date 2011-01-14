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


  /// \api #MqFactoryCreateF
  [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
  internal delegate MqErrorE FactoryCreateF ([In]IntPtr tmpl, MqFactoryE create, [In]IntPtr item, [In,Out] ref IntPtr contextP);

  /// \api #MqFactoryDeleteF
  [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
  internal delegate void FactoryDeleteF ([In]IntPtr context, [In] MQ_BOL doFactoryCleanup, [In]IntPtr data);

  /// \api #MqFactoryReturnE
  internal class MqFactorySException : Exception
  {

    private const CallingConvention MSGQUE_CC = CallingConvention.Cdecl;
    private const CharSet MSGQUE_CS = CharSet.Ansi;
    private const string MSGQUE_DLL = "libmsgque";

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqFactoryErrorMsg")]
    private static extern IntPtr MqFactoryErrorMsg([In]MqFactoryReturnE ret);

    internal MqFactorySException(MqFactoryReturnE ret) : base(Marshal.PtrToStringAnsi(MqFactoryErrorMsg(ret))) {}
  }


  /// \ingroup Mq_Factory_Cs_API
  /// \api #MqFactoryS
  public static class MqFactoryS<T> where T : MqS
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

    private const CallingConvention MSGQUE_CC = CallingConvention.Cdecl;
    private const CharSet MSGQUE_CS = CharSet.Ansi;
    private const string MSGQUE_DLL = "libmsgque";

    static private FactoryCreateF fFactoryCreate  = FactoryCreate;
    static private FactoryDeleteF fFactoryDelete  = FactoryDelete;

  /*****************************************************************************/
  /*                                                                           */
  /*                             link to "C"                                   */
  /*                                                                           */
  /*****************************************************************************/

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqFactoryDefault")]
    private static extern MqFactoryReturnE MqFactoryDefault(string ident,
      [In]FactoryCreateF FactoryCreate, [In]IntPtr CreateData, [In]MqTokenDataFreeF CreateFree,
      [In]FactoryDeleteF FactoryDelete, [In]IntPtr DeleteData, [In]MqTokenDataFreeF DeleteFree
    );

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqFactoryDefaultIdent")]
    private static extern IntPtr MqFactoryDefaultIdent();

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqFactoryAdd")]
    private static extern MqFactoryReturnE MqFactoryAdd(string ident,
      [In]FactoryCreateF FactoryCreate, [In]IntPtr CreateData, [In]MqTokenDataFreeF CreateFree,
      [In]FactoryDeleteF FactoryDelete, [In]IntPtr DeleteData, [In]MqTokenDataFreeF DeleteFree
    );

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqFactoryNew")]
    private static extern MqFactoryReturnE MqFactoryNew(string ident,
      [In]FactoryCreateF FactoryCreate, [In]IntPtr CreateData, [In]MqTokenDataFreeF CreateFree,
      [In]FactoryDeleteF FactoryDelete, [In]IntPtr DeleteData, [In]MqTokenDataFreeF DeleteFree,
      [In]IntPtr data, [In,Out]ref IntPtr context
    );

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqFactoryCall")]
    private static extern MqFactoryReturnE MqFactoryCall(string ident, [In]IntPtr data, [In,Out]ref IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqContextDelete")]
    internal static extern void MqContextDelete([In,Out]ref IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqSetupDup")]
    private static extern MqErrorE MqSetupDup([In]IntPtr context, [In]IntPtr tmpl);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqFactoryItemGetCreateData")]
    private static extern IntPtr MqFactoryItemGetCreateData([In]IntPtr item);

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
	IntPtr data = MqFactoryItemGetCreateData(item);
	contextP = ((MqS)((ConstructorInfo)GCHandle.FromIntPtr(data).Target).Invoke(args)).context;
      } catch (Exception ex) {
	if (create == MqFactoryE.MQ_FACTORY_NEW_INIT) {
	  return MqErrorE.MQ_ERROR;
	} else {
	  return MqS.MqErrorSet2 (tmpl, ex);
	}
      }
      if (create != MqFactoryE.MQ_FACTORY_NEW_INIT) {
	MqSetupDup (contextP, tmpl);
      }
      return MqErrorE.MQ_OK;
    }

    static private void FactoryDelete (IntPtr context, MQ_BOL doFactoryCleanup, IntPtr item) {
      IntPtr self = MqS.MqConfigGetSelf(context);
      MqS.MqContextDelete (ref MqS.GetSelf(context).context);
      GCHandle.FromIntPtr(self).Free();
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

    private static void ErrorCheck(MqFactoryReturnE err) {
      if (err == MqFactoryReturnE.MQ_FACTORY_RETURN_OK) return;
      throw new MqFactorySException(err);
    }

  /*****************************************************************************/
  /*                                                                           */
  /*                                 public                                    */
  /*                                                                           */
  /*****************************************************************************/

    /// \api #MqFactoryAdd
    public static void Add(string ident) {
      IntPtr data = getConstructor();
      // !attention, no "MqS.fProcFree" because the code will core at the end
      ErrorCheck(MqFactoryAdd(ident, fFactoryCreate, data, null, fFactoryDelete, IntPtr.Zero, null)); 
    }

    /// \api #MqFactoryAdd
    public static void Add() {
      IntPtr data = getConstructor();
      // !attention, no "MqS.fProcFree" because the code will core at the end
      ErrorCheck(MqFactoryAdd(typeof(T).Name, fFactoryCreate, data, null, fFactoryDelete, IntPtr.Zero, null)); 
    }

    /// \api #MqFactoryDefault
    public static void Default(string ident) {
      IntPtr data = getConstructor();
      // !attention, no "MqS.fProcFree" because the code will core at the end
//DEBUG.O(ident, typeof(T).Name);
      ErrorCheck(MqFactoryDefault(ident, fFactoryCreate, data, null, fFactoryDelete, IntPtr.Zero, null)); 
    }

    /// \api #MqFactoryDefault
    public static void Default() {
      IntPtr data = getConstructor();
      // !attention, no "MqS.fProcFree" because the code will core at the end
//DEBUG.P(ident,data);
      ErrorCheck(MqFactoryDefault(typeof(T).Name, fFactoryCreate, data, null, fFactoryDelete, IntPtr.Zero, null));
    }

    /// \api #MqFactoryDefaultIdent
    public static String DefaultIdent() {
      String str = Marshal.PtrToStringAnsi(MqFactoryDefaultIdent());
//DEBUG.O(str, typeof(T).Name);
      return str;
    }

    /// \api #MqFactoryNew
    public static T New(string ident) {
      IntPtr mqctx = IntPtr.Zero;
      IntPtr data = getConstructor();
      // !attention, no "MqS.fProcFree" because the code will core at the end
//DEBUG.P(ident,data);
      ErrorCheck(MqFactoryNew(ident, fFactoryCreate, data, null, fFactoryDelete, IntPtr.Zero, null, IntPtr.Zero, ref mqctx)); 
      return (T) MqS.GetSelf(mqctx);
    }

    /// \api #MqFactoryNew
    public static T New() {
      IntPtr mqctx = IntPtr.Zero;
      IntPtr data = getConstructor();
      // !attention, no "MqS.fProcFree" because the code will core at the end
//DEBUG.P(ident,data);
      ErrorCheck(MqFactoryNew(typeof(T).Name, fFactoryCreate, data, null, fFactoryDelete, IntPtr.Zero, null, IntPtr.Zero, ref mqctx)); 
      return (T) MqS.GetSelf(mqctx);
    }

    /// \api #MqFactoryCall
    public static T Call(string ident) {
      IntPtr mqctx = IntPtr.Zero;
      ErrorCheck(MqFactoryCall(ident, IntPtr.Zero, ref mqctx));
      return (T) MqS.GetSelf(mqctx);
    }

    /// \api #MqFactoryCall
    public static T Call() {
      IntPtr mqctx = IntPtr.Zero;
      ErrorCheck(MqFactoryCall(typeof(T).Name, IntPtr.Zero, ref mqctx));
      return (T) MqS.GetSelf(mqctx);
    }

  } // END - class "MqFactoryS"

  /// \}

} // END - namespace "csmsgque"


