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

  /// \api #MqConfigSetServerSetup 
  public interface IServerSetup {
    void ServerSetup();
  }

  /// \api #MqConfigSetServerCleanup 
  public interface IServerCleanup {
    void ServerCleanup();
  }

  /// \api #MqConfigSetFactory 
  public interface IFactory {
    MqS Factory();
  }

  /// \api #MqConfigSetBgError 
  public interface IBgError {
    void BgError();
  }

  /// \api #MqConfigSetEvent 
  public interface IEvent {
    void Event();
  }

  /// \api #MqServiceCreate
  public interface IService {
    void Service(MqS ctx);
  }

  public partial class MqS
  {

/// \defgroup Mq_Config_Cs_API Mq_Config_Cs_API
/// \ingroup Mq_Cs_API
/// \brief \copybrief Mq_Config_C_API
/// \details \copydetails Mq_Config_C_API
/// \{

    static MqS() {
      IntPtr initB = MqInitCreate();
      if(Type.GetType ("Mono.Runtime") != null) MqBufferLAppendC(initB, "mono");
      MqBufferLAppendC(initB, APP);
    }

    /// \api #MqInitCreate
    protected static void Init(params string[] argv) {
      IntPtr initB = MqInitCreate();
      foreach (string s in argv) {
	MqBufferLAppendC(initB, s);
      }
    }
  
  // SET - PRIVATE

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetName")]
    private static extern void MqConfigSetName([In]IntPtr context, [In]string data);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetSrvName")]
    private static extern void MqConfigSetSrvName([In]IntPtr context, [In]string data);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetIdent")]
    private static extern void MqConfigSetIdent([In]IntPtr context, [In]string data);

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

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetIoUdsFile")]
    private static extern MqErrorE MqConfigSetIoUdsFile([In]IntPtr context, [In]string file);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetIoPipeSocket")]
    private static extern MqErrorE MqConfigSetIoPipeSocket([In]IntPtr context, [In]int socket);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigSetStartAs")]
    private static extern void MqConfigSetStartAs([In]IntPtr context, [In]int startAs);

  // PUBLIC

    /// \api #MqConfigSetName
    public void   ConfigSetName		(string data) { MqConfigSetName		(context, data); }
    /// \api #MqConfigSetSrvName
    public void   ConfigSetSrvName	(string data) { MqConfigSetSrvName	(context, data); }
    /// \api #MqConfigSetIdent
    public void   ConfigSetIdent	(string data) { MqConfigSetIdent	(context, data); }
    /// \api #MqConfigSetBuffersize
    public void	  ConfigSetBuffersize	(int data)    { MqConfigSetBuffersize	(context, data); }
    /// \api #MqConfigSetDebug
    public void	  ConfigSetDebug	(int data)    { MqConfigSetDebug	(context, data); }
    /// \api #MqConfigSetTimeout
    public void	  ConfigSetTimeout	(long data)   { MqConfigSetTimeout	(context, data); }
    /// \api #MqConfigSetIsSilent
    public void	  ConfigSetIsSilent	(bool data)   { MqConfigSetIsSilent	(context, data?MQ_BOL.MQ_YES:MQ_BOL.MQ_NO); }
    /// \api #MqConfigSetIsServer
    public void	  ConfigSetIsServer	(bool data)   { MqConfigSetIsServer	(context, data?MQ_BOL.MQ_YES:MQ_BOL.MQ_NO); }
    /// \api #MqConfigSetIsString
    public void	  ConfigSetIsString	(bool data)   { MqConfigSetIsString	(context, data?MQ_BOL.MQ_YES:MQ_BOL.MQ_NO); }
    /// \api #MqConfigSetIgnoreExit
    public void	  ConfigSetIgnoreExit	(bool data)   { MqConfigSetIgnoreExit	(context, data?MQ_BOL.MQ_YES:MQ_BOL.MQ_NO); }
    /// \api #MqConfigSetIoTcp
    public void   ConfigSetIoTcp	(string host, string port, string myhost, string myport) { 
      ErrorMqToCsWithCheck (MqConfigSetIoTcp (context, host, port, myhost, myport)); 
    }
    /// \api #MqConfigSetIoUdsFile
    public void   ConfigSetIoUdsFile	(string data) { 
      ErrorMqToCsWithCheck (MqConfigSetIoUdsFile (context, data));
    }
    /// \api #MqConfigSetIoPipeSocket
    public void   ConfigSetIoPipeSocket	(int data)    { 
      ErrorMqToCsWithCheck (MqConfigSetIoPipeSocket (context, data)); 
    }
    /// \api #MqConfigSetStartAs
    public void   ConfigSetStartAs	(int data)    { MqConfigSetStartAs	(context, data); }

  // PRIVATE

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetIsServer")]
    private static extern MQ_BOL MqConfigGetIsServer([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetIsString")]
    private static extern MQ_BOL MqConfigGetIsString([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetIsSilent")]
    private static extern MQ_BOL MqConfigGetIsSilent([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetName")]
    private static extern IntPtr MqConfigGetName([In]IntPtr context);

    [DllImport(MSGQUE_DLL, CallingConvention=MSGQUE_CC, CharSet=MSGQUE_CS, EntryPoint = "MqConfigGetDebug")]
    private static extern int MqConfigGetDebug([In]IntPtr context);

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

    /// \api #MqConfigGetIsServer
    public bool	    ConfigGetIsServer()	    { return MqConfigGetIsServer(context) == MQ_BOL.MQ_YES; }
    /// \api #MqConfigGetIsString
    public bool	    ConfigGetIsString()     { return MqConfigGetIsString(context) == MQ_BOL.MQ_YES; }
    /// \api #MqConfigGetIsSilent
    public bool	    ConfigGetIsSilent()     { return MqConfigGetIsSilent(context) == MQ_BOL.MQ_YES; }
    /// \api #MqConfigGetName
    public string   ConfigGetName()	    { return Marshal.PtrToStringAnsi(MqConfigGetName(context)); }
    /// \api #MqConfigGetSrvName
    public string   ConfigGetSrvName()	    { return Marshal.PtrToStringAnsi(MqConfigGetSrvName(context)); }
    /// \api #MqConfigGetIdent
    public string   ConfigGetIdent()	    { return Marshal.PtrToStringAnsi(MqConfigGetIdent(context)); }
    /// \api #MqConfigGetDebug
    public int	    ConfigGetDebug()	    { return MqConfigGetDebug(context); }
    /// \api #MqConfigGetTimeout
    public long	    ConfigGetTimeout()	    { return MqConfigGetTimeout(context); }
    /// \api #MqConfigGetBuffersize
    public int	    ConfigGetBuffersize()   { return MqConfigGetBuffersize(context); }
    /// \api #MqConfigGetIoTcpHost
    public string   ConfigGetIoTcpHost()    { return Marshal.PtrToStringAnsi(MqConfigGetIoTcpHost(context)); }
    /// \api #MqConfigGetIoTcpPort
    public string   ConfigGetIoTcpPort()    { return Marshal.PtrToStringAnsi(MqConfigGetIoTcpPort(context)); }
    /// \api #MqConfigGetIoTcpMyHost
    public string   ConfigGetIoTcpMyHost()  { return Marshal.PtrToStringAnsi(MqConfigGetIoTcpMyHost(context)); }
    /// \api #MqConfigGetIoTcpMyPort
    public string   ConfigGetIoTcpMyPort()  { return Marshal.PtrToStringAnsi(MqConfigGetIoTcpMyPort(context)); }
    /// \api #MqConfigGetIoUdsFile
    public string   ConfigGetIoUdsFile()    { return Marshal.PtrToStringAnsi(MqConfigGetIoUdsFile(context)); }
    /// \api #MqConfigGetIoPipeSocket
    public int	    ConfigGetIoPipeSocket() { return MqConfigGetIoPipeSocket(context); }
    /// \api #MqConfigGetStartAs
    public int	    ConfigGetStartAs()	    { return MqConfigGetStartAs(context); }

/// \}

  } // END - class "MqS"
} // END - namespace "csmsgque"




