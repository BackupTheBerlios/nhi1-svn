/**
 *  \file       theLink/csmsgque/config.cs
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

  public partial class MqS
  {
  
  // SET - PRIVATE

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetName")]
    private static extern void MqConfigSetName([In]IntPtr context, [In]string data);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetSrvName")]
    private static extern void MqConfigSetSrvName([In]IntPtr context, [In]string data);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetIdent")]
    private static extern void MqConfigSetIdent([In]IntPtr context, [In]string data);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigCheckIdent")]
    private static extern bool MqConfigCheckIdent([In]IntPtr context, [In]string data);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetBuffersize")]
    private static extern void MqConfigSetBuffersize([In]IntPtr context, [In]int data);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetDebug")]
    private static extern void MqConfigSetDebug([In]IntPtr context, [In]int data);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetTimeout")]
    private static extern void MqConfigSetTimeout([In]IntPtr context, [In]long data);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetIsSilent")]
    private static extern void MqConfigSetIsSilent([In]IntPtr context, [In]MQ_BOL data);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetIsServer")]
    private static extern void MqConfigSetIsServer([In]IntPtr context, [In]MQ_BOL data);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetIsString")]
    private static extern void MqConfigSetIsString([In]IntPtr context, [In]MQ_BOL data);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetIgnoreExit")]
    private static extern void MqConfigSetIgnoreExit([In]IntPtr context, [In]MQ_BOL data);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetIgnoreFork")]
    private static extern void MqConfigSetIgnoreFork([In]IntPtr context, [In]MQ_BOL data);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetSetup")]
    private static extern void MqConfigSetSetup([In]IntPtr context, 
      [In]MqCreateF fChildCreate, [In]MqDeleteF fChildDelete, 
	[In]MqCreateF fParentCreate, [In]MqDeleteF fParentDelete, 
	  [In]MqExitF fProcessExit, [In]MqExitF fThreadExit);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetServerSetup")]
    private static extern void MqConfigSetServerSetup([In]IntPtr context, [In]MqTokenF fToken, [In]IntPtr data, 
      [In]MqTokenDataFreeF dataFree, [In]IntPtr dataCopy);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetServerCleanup")]
    private static extern void MqConfigSetServerCleanup([In]IntPtr context, [In]MqTokenF fToken, [In]IntPtr data, 
      [In]MqTokenDataFreeF dataFree, [In] IntPtr dataCopy);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetBgError")]
    private static extern void MqConfigSetBgError([In]IntPtr context, [In]MqTokenF fToken, [In]IntPtr data, 
      [In]MqTokenDataFreeF dataFree, [In] IntPtr dataCopy);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetEvent")]
    private static extern void MqConfigSetEvent([In]IntPtr context, [In]MqTokenF fToken, [In]IntPtr data, 
      [In]MqTokenDataFreeF dataFree, [In] IntPtr dataCopy);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetFactory")]
    private static extern void MqConfigSetFactory([In]IntPtr context, 
      [In]MqFactoryCreateF FactoryCreate, [In]IntPtr CreateData, [In]MqTokenDataFreeF CreateFree, [In] IntPtr CreateCopy,
      [In]MqFactoryDeleteF FactoryDelete, [In]IntPtr DeleteData, [In]MqTokenDataFreeF DeleteFree, [In] IntPtr DeleteCopy
    );

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetSelf")]
    private static extern void MqConfigSetSelf([In]IntPtr context, [In]IntPtr self);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetIoTcp")]
    private static extern MqErrorE MqConfigSetIoTcp([In]IntPtr context, [In]string host, [In]string port, 
				  [In]string myhost, [In]string myport );

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetIoUds")]
    private static extern MqErrorE MqConfigSetIoUds([In]IntPtr context, [In]string file);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetIoPipe")]
    private static extern MqErrorE MqConfigSetIoPipe([In]IntPtr context, [In]int socket);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetStartAs")]
    private static extern void MqConfigSetStartAs([In]IntPtr context, [In]int startAs);

  // PUBLIC

    public void   ConfigSetName		(string data) { MqConfigSetName		(context, data); }
    public void   ConfigSetSrvName	(string data) { MqConfigSetSrvName	(context, data); }
    public void   ConfigSetIdent	(string data) { MqConfigSetIdent	(context, data); }
    public bool   ConfigCheckIdent	(string data) { return MqConfigCheckIdent (context, data); }
    public void	  ConfigSetBuffersize	(int data)    { MqConfigSetBuffersize	(context, data); }
    public void	  ConfigSetDebug	(int data)    { MqConfigSetDebug	(context, data); }
    public void	  ConfigSetTimeout	(long data)   { MqConfigSetTimeout	(context, data); }
    public void	  ConfigSetIsSilent	(bool data)   { MqConfigSetIsSilent	(context, data?MQ_BOL.MQ_YES:MQ_BOL.MQ_NO); }
    public void	  ConfigSetIsServer	(bool data)   { MqConfigSetIsServer	(context, data?MQ_BOL.MQ_YES:MQ_BOL.MQ_NO); }
    public void	  ConfigSetIsString	(bool data)   { MqConfigSetIsString	(context, data?MQ_BOL.MQ_YES:MQ_BOL.MQ_NO); }
    public void	  ConfigSetIgnoreExit	(bool data)   { MqConfigSetIgnoreExit	(context, data?MQ_BOL.MQ_YES:MQ_BOL.MQ_NO); }
    public void   ConfigSetIoTcp	(string host, string port, string myhost, string myport) { 
      ErrorMqToCsWithCheck (MqConfigSetIoTcp (context, host, port, myhost, myport)); 
    }
    public void   ConfigSetIoUds	(string data) { 
      ErrorMqToCsWithCheck (MqConfigSetIoUds (context, data));
    }
    public void   ConfigSetIoPipe	(int data)    { 
      ErrorMqToCsWithCheck (MqConfigSetIoPipe (context, data)); 
    }
    public void   ConfigSetStartAs	(int data)    { MqConfigSetStartAs	(context, data); }

  // PRIVATE

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetIsServer")]
    private static extern bool MqConfigGetIsServer([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetIsFilter")]
    private static extern bool MqConfigGetIsFilter([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetIsSlave")]
    private static extern bool MqConfigGetIsSlave([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetIsString")]
    private static extern bool MqConfigGetIsString([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetIsSilent")]
    private static extern bool MqConfigGetIsSilent([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetName")]
    private static extern IntPtr MqConfigGetName([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetDebug")]
    private static extern int MqConfigGetDebug([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetMaster")]
    private static extern IntPtr MqConfigGetMaster([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetSelf")]
    private static extern IntPtr MqConfigGetSelf([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetTimeout")]
    private static extern long MqConfigGetTimeout([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetSrvName")]
    private static extern IntPtr MqConfigGetSrvName([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetIdent")]
    private static extern IntPtr MqConfigGetIdent([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetBuffersize")]
    private static extern int MqConfigGetBuffersize([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetIoTcpHost")]
    private static extern IntPtr MqConfigGetIoTcpHost([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetIoTcpPort")]
    private static extern IntPtr MqConfigGetIoTcpPort([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetIoTcpMyHost")]
    private static extern IntPtr MqConfigGetIoTcpMyHost([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetIoTcpMyPort")]
    private static extern IntPtr MqConfigGetIoTcpMyPort([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetIoUdsFile")]
    private static extern IntPtr MqConfigGetIoUdsFile([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetIoPipeSocket")]
    private static extern int MqConfigGetIoPipeSocket([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetStartAs")]
    private static extern int MqConfigGetStartAs([In]IntPtr context);

  // PUBLIC

    public bool	    ConfigGetIsServer()	    { return MqConfigGetIsServer(context); }
    public bool	    ConfigGetIsFilter()     { return MqConfigGetIsFilter(context); }
    public bool	    ConfigGetIsSlave()	    { return MqConfigGetIsSlave(context); }
    public bool	    ConfigGetIsString()     { return MqConfigGetIsString(context); }
    public bool	    ConfigGetIsSilent()     { return MqConfigGetIsSilent(context); }
    public string   ConfigGetName()	    { return Marshal.PtrToStringAnsi(MqConfigGetName(context)); }
    public string   ConfigGetSrvName()	    { return Marshal.PtrToStringAnsi(MqConfigGetSrvName(context)); }
    public string   ConfigGetIdent()	    { return Marshal.PtrToStringAnsi(MqConfigGetIdent(context)); }
    public int	    ConfigGetDebug()	    { return MqConfigGetDebug(context); }
    public long	    ConfigGetTimeout()	    { return MqConfigGetTimeout(context); }
    public int	    ConfigGetBuffersize()   { return MqConfigGetBuffersize(context); }
    public string   ConfigGetIoTcpHost()    { return Marshal.PtrToStringAnsi(MqConfigGetIoTcpHost(context)); }
    public string   ConfigGetIoTcpPort()    { return Marshal.PtrToStringAnsi(MqConfigGetIoTcpPort(context)); }
    public string   ConfigGetIoTcpMyHost()  { return Marshal.PtrToStringAnsi(MqConfigGetIoTcpMyHost(context)); }
    public string   ConfigGetIoTcpMyPort()  { return Marshal.PtrToStringAnsi(MqConfigGetIoTcpMyPort(context)); }
    public string   ConfigGetIoUdsFile()    { return Marshal.PtrToStringAnsi(MqConfigGetIoUdsFile(context)); }
    public int	    ConfigGetIoPipeSocket() { return MqConfigGetIoPipeSocket(context); }
    public int	    ConfigGetStartAs()	    { return MqConfigGetStartAs(context); }

    public MqS	    ConfigGetMaster()	    {
      IntPtr masterP = MqConfigGetMaster(context);
      return (masterP == IntPtr.Zero ? null : GetSelf(masterP));
    }

  } // END - class "MqS"
} // END - namespace "csmsgque"





