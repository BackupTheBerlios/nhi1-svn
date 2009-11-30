/**
 *  \file       csmsgque/pointer.cs
 *  \brief      \$Id: pointer.cs 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

using System;
using System.Runtime.InteropServices;

namespace csmsgque {

/// \api \ref MqCreateF
[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate MqErrorE MqCreateF ([In]IntPtr context, [In,Out]ref IntPtr argv); 

/// \api \ref MqDeleteF
[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate void MqDeleteF ([In] IntPtr context);

/// \api \ref MqExitF
[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate void MqExitF ([In]int num);

/// \api \ref MqTokenF
[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate MqErrorE MqTokenF ([In]IntPtr context, [In]IntPtr data);

/// \api \ref MqTokenDataFreeF
[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate void MqTokenDataFreeF ([In]IntPtr context, [In,Out] ref IntPtr data);

/// \api \ref MqEventF
[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate MqErrorE MqEventF ([In]IntPtr context);

/// \api \ref MqFactoryCreateF
[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate MqErrorE MqFactoryCreateF ([In]IntPtr tmpl, MqFactoryE create, [In]IntPtr data, [In,Out] ref IntPtr contextP);

/// \api \ref MqFactoryDeleteF
[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate void MqFactoryDeleteF ([In]IntPtr context, [In] MQ_BOL doFactoryCleanup, [In]IntPtr data);

public partial class MqS {

  /// \api public version from \ref MqTokenF
  public delegate void Callback();
}

// END - NameSpace csmsgque
}



