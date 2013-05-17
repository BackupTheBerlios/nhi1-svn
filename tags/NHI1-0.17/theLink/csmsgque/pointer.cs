/**
 *  \file       theLink/csmsgque/pointer.cs
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
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

/// \api \ref MqDataFreeF
[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate void MqDataFreeF ([In]IntPtr context, [In,Out] ref IntPtr data);

/// \api \ref MqEventF
[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
internal delegate MqErrorE MqEventF ([In]IntPtr context);

public partial class MqS {

  /// \api public version from MqTokenF
  public delegate void Callback();
}

// END - NameSpace csmsgque
}



