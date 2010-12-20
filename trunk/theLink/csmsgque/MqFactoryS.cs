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

/// \defgroup Mq_Cs_API Mq_Cs_API
/// \brief \copybrief Mq_C_API
/// \details \copydetails Mq_C_API
/// \{

namespace csmsgque {

  internal enum MqFactoryE :int {
    MQ_FACTORY_NEW_INIT,
    MQ_FACTORY_NEW_CHILD,
    MQ_FACTORY_NEW_SLAVE,
    MQ_FACTORY_NEW_FORK,
    MQ_FACTORY_NEW_THREAD,
    MQ_FACTORY_NEW_FILTER
  };

  /// \api \ref MqFactoryCreateF
  [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
  internal delegate MqErrorE MqFactoryCreateF ([In]IntPtr tmpl, MqFactoryE create, [In]IntPtr item, [In,Out] ref IntPtr contextP);

  /// \api \ref MqFactoryDeleteF
  [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
  internal delegate void MqFactoryDeleteF ([In]IntPtr context, [In] MQ_BOL doFactoryCleanup, [In]IntPtr data);

  /// \ingroup Mq_Factory_Cs_API
  /// \api #MqFactoryS
  public static class MqFactoryS<T> where T : MqS
  {

  /*****************************************************************************/
  /*                                                                           */
  /*                                Init                                       */
  /*                                                                           */
  /*****************************************************************************/

    private const CallingConvention MSGQUE_CC = CallingConvention.Cdecl;
    private const CharSet MSGQUE_CS = CharSet.Ansi;
    private const string MSGQUE_DLL = "libmsgque";

    static private MqFactoryCreateF fFactoryCreate  = FactoryCreate;
    static private MqFactoryDeleteF fFactoryDelete  = FactoryDelete;

/*
    static MqFactoryS() {
      MqFactoryS<MqS>.Default ("csmsgque");
    }
*/

  /*****************************************************************************/
  /*                                                                           */
  /*                             link to "C"                                   */
  /*                                                                           */
  /*****************************************************************************/

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqFactoryDefault")]
    private static extern void MqFactoryDefault(string ident,
      [In]MqFactoryCreateF FactoryCreate, [In]IntPtr CreateData, [In]MqTokenDataFreeF CreateFree,
      [In]MqFactoryDeleteF FactoryDelete, [In]IntPtr DeleteData, [In]MqTokenDataFreeF DeleteFree
    );

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqFactoryAdd")]
    private static extern void MqFactoryAdd(string ident,
      [In]MqFactoryCreateF FactoryCreate, [In]IntPtr CreateData, [In]MqTokenDataFreeF CreateFree,
      [In]MqFactoryDeleteF FactoryDelete, [In]IntPtr DeleteData, [In]MqTokenDataFreeF DeleteFree
    );

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqFactoryNew")]
    private static extern IntPtr MqFactoryNew(string ident,
      [In]MqFactoryCreateF FactoryCreate, [In]IntPtr CreateData, [In]MqTokenDataFreeF CreateFree,
      [In]MqFactoryDeleteF FactoryDelete, [In]IntPtr DeleteData, [In]MqTokenDataFreeF DeleteFree
    );

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqFactoryCall")]
    private static extern IntPtr MqFactoryCall(string ident);

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
	  DEBUG.O("Factory", ex);
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

  /*****************************************************************************/
  /*                                                                           */
  /*                                 public                                    */
  /*                                                                           */
  /*****************************************************************************/

    public static void Add(string ident) {
      IntPtr data = getConstructor();
      // !attention, no "MqS.fProcFree" becaus ethe code will core at the end
      MqFactoryAdd(ident, fFactoryCreate, data, null, fFactoryDelete, IntPtr.Zero, null); }

    public static void Add() {
      IntPtr data = getConstructor();
      // !attention, no "MqS.fProcFree" becaus ethe code will core at the end
      MqFactoryAdd(typeof(T).Name, fFactoryCreate, data, null, fFactoryDelete, IntPtr.Zero, null); }

    internal static void Default(string ident) {
      IntPtr data = getConstructor();
      // !attention, no "MqS.fProcFree" becaus ethe code will core at the end
//DEBUG.P(ident,data);
      MqFactoryDefault(ident, fFactoryCreate, data, null, fFactoryDelete, IntPtr.Zero, null); }

    internal static void Default() {
      IntPtr data = getConstructor();
      // !attention, no "MqS.fProcFree" becaus ethe code will core at the end
//DEBUG.P(ident,data);
      MqFactoryDefault(typeof(T).Name, fFactoryCreate, data, null, fFactoryDelete, IntPtr.Zero, null); }

    public static T New(string ident) {
      IntPtr data = getConstructor();
      // !attention, no "MqS.fProcFree" becaus ethe code will core at the end
//DEBUG.P(ident,data);
      return (T) MqS.GetSelf(MqFactoryNew(ident, fFactoryCreate, data, null, fFactoryDelete, IntPtr.Zero, null)); }

    public static T New() {
      IntPtr data = getConstructor();
      // !attention, no "MqS.fProcFree" becaus ethe code will core at the end
//DEBUG.P(ident,data);
      return (T) MqS.GetSelf(MqFactoryNew(typeof(T).Name, fFactoryCreate, data, null, fFactoryDelete, IntPtr.Zero, null)); }

    public static T Call(string ident) {
      return (T) MqS.GetSelf(MqFactoryCall(ident));
    }

  } // END - class "MqFactoryS"
} // END - namespace "csmsgque"

/// \}

