/**
 *  \file       theLink/javamsgque/MqS.java
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

/// \brief \api #MqS
public class MqS {

  static {
    System.loadLibrary("javamsgque");
  }

  private long hdl;

//
// ========================================================================
/// \defgroup Mq_Config_Java_API Mq_Config_Java_API
/// \ingroup Mq_Java_API
/// \{
/// \brief \copybrief Mq_Config_C_API
/// \copydoc Mq_Config_C_API
//

  /// \api #MqStartE
  public enum START {
    DEFAULT(0), FORK(1), THREAD(2), SPAWN(3);

    private int flag;

    START(int setFlag)
    {
      flag = setFlag;
    }

    public int GetFlag()
    {
      return(flag);
    }

    public static START SetFlag(int value)
    {
      return START.class.getEnumConstants()[value];
    }
  }

  /// \api #MqInitCreate
  public static native void	Init	      (String... argv);

  /// \api #MqConfigSetName
  public native void ConfigSetName	      (String val);
  /// \api #MqConfigSetSrvName
  public native void ConfigSetSrvName	      (String val);
  /// \api #MqConfigSetBuffersize
  public native void ConfigSetBuffersize      (int val);
  /// \api #MqConfigSetDebug
  public native void ConfigSetDebug	      (int val);
  /// \api #MqConfigSetTimeout
  public native void ConfigSetTimeout	      (long val);
  /// \api #MqConfigSetIsSilent
  public native void ConfigSetIsSilent	      (boolean val);
  /// \api #MqConfigSetIsServer
  public native void ConfigSetIsServer	      (boolean val);
  /// \api #MqConfigSetIsString
  public native void ConfigSetIsString	      (boolean val);
  /// \api #MqConfigSetIgnoreExit
  public native void ConfigSetIgnoreExit      (boolean val);
  /// \api #MqConfigSetStartAs
  private native void pConfigSetStartAs	      (int val);
  /// \api #MqConfigSetStartAs
  public void ConfigSetStartAs (START val) {
    pConfigSetStartAs(val.GetFlag());
  }
  /// \api #MqConfigSetIoTcp
  public native void ConfigSetIoTcp	      (String host, String port, String myhost, String myport);
  /// \api #MqConfigSetIoUdsFile
  public native void ConfigSetIoUdsFile	      (String file);
  /// \api #MqConfigSetIoPipeSocket
  public native void ConfigSetIoPipeSocket    (int socket);

  /// \api #MqConfigGetIsServer
  public native boolean	ConfigGetIsServer     ();
  /// \api #MqConfigGetIsString
  public native boolean	ConfigGetIsString     ();
  /// \api #MqConfigGetIsSilent
  public native boolean	ConfigGetIsSilent     ();
  /// \api #MqConfigGetBuffersize
  public native int	ConfigGetBuffersize   ();
  /// \api #MqConfigGetTimeout
  public native long	ConfigGetTimeout      ();
  /// \api #MqConfigGetName
  public native String	ConfigGetName	      ();
  /// \api #MqConfigGetSrvName
  public native String	ConfigGetSrvName      ();
  /// \api #MqConfigGetDebug
  public native int	ConfigGetDebug	      ();
  /// \api #MqConfigGetStartAs
  private native int	pConfigGetStartAs     ();
  /// \api #MqConfigGetStartAs
  public START	ConfigGetStartAs()  {
    return START.SetFlag(pConfigGetStartAs());
  }
  /// \api #MqConfigGetStatusIs
  public native int 	ConfigGetStatusIs     ();
  /// \api #MqConfigGetIoTcpHost
  public native String ConfigGetIoTcpHost     ();
  /// \api #MqConfigGetIoTcpPort
  public native String ConfigGetIoTcpPort     ();
  /// \api #MqConfigGetIoTcpMyHost
  public native String ConfigGetIoTcpMyHost   ();
  /// \api #MqConfigGetIoTcpMyPort
  public native String ConfigGetIoTcpMyPort   ();
  /// \api #MqConfigGetIoUdsFile
  public native String ConfigGetIoUdsFile     ();
  /// \api #MqConfigGetIoPipeSocket
  public native int ConfigGetIoPipeSocket     ();

/// \} Mq_Config_Java_API

//
// ========================================================================
/// \defgroup Mq_Context_Java_API Mq_Context_Java_API
/// \ingroup Mq_Java_API
/// \{
/// \brief \copybrief Mq_Context_C_API
/// \copydoc Mq_Context_C_API

  private native void ContextCreate(MqS tmpl);
  private native void ContextDelete();

  /// \api #MqContextCreate
  protected MqS(MqS tmpl) {
    //System.out.println("MqS -> 11111");
    ContextCreate(tmpl);
  }

  /// \api #MqContextCreate
  protected MqS() {
    //System.out.println("MqS -> 22222");
    ContextCreate(null);
  }

  /// \api #MqContextDelete
  protected void finalize() {
    ContextDelete();
  } 

  /// \api #MqExit
  public native void Exit ();

  /// \api #MqLogC
  public native void LogC (String prefix, int level, String text);

/// \} Mq_Context_Java_API

//
// ========================================================================
/// \defgroup Mq_Link_Java_API Mq_Link_Java_API
/// \ingroup Mq_Java_API
/// \{
/// \brief \copybrief Mq_Link_C_API
/// \copydoc Mq_Link_C_API

  /// \api #MqLinkCreate
  public native void LinkCreate (String... args) throws MqSException;

  /// \api #MqLinkCreateChild
  public native void LinkCreateChild (MqS parent, String... args) throws MqSException;

  /// \api #MqLinkDelete
  public native void LinkDelete ();

  /// \api #MqLinkConnect
  public native void LinkConnect ();

  /// \api #MqLinkGetParent
  public native MqS LinkGetParent ();

  /// \api #MqLinkGetTargetIdent
  public native String LinkGetTargetIdent ();

  /// \api #MqLinkIsParent
  public native boolean	LinkIsParent ();

  /// \api #MqLinkIsConnected
  public native boolean	LinkIsConnected ();

  /// \api #MqLinkGetCtxId
  public native int LinkGetCtxId ();

/// \} Mq_Link_java_API

//
// ========================================================================
/// \defgroup Mq_Service_Java_API Mq_Service_Java_API
/// \ingroup Mq_Java_API
/// \{
/// \brief \copybrief Mq_Service_C_API
/// \copydoc Mq_Service_C_API
///

  /// \api #MqWaitOnEventE
  public enum WAIT {
    NO(0), ONCE(1), FOREVER(2);

    private int flag;

    WAIT(int setFlag)
    {
      flag = setFlag;
    }

    public int GetFlag()
    {
      return(flag);
    }
  }

  /// \api default TIMEOUT values
  public enum TIMEOUT {
    DEFAULT(-1), USER(-2), MAX(-2);

    private int flag;

    TIMEOUT(int setFlag)
    {
      flag = setFlag;
    }

    public int GetFlag()
    {
      return(flag);
    }
  }

  /// \api #MqServiceGetToken
  public native String	ServiceGetToken	    ();
  /// \api #MqServiceGetFilter
  public native MqS	ServiceGetFilter    () throws MqSException;
  /// \api #MqServiceGetFilter
  public native MqS	ServiceGetFilter    (int id) throws MqSException;
  /// \api #MqServiceIsTransaction
  public native boolean	ServiceIsTransaction();

  /// \api #MqServiceCreate
  public native void	ServiceCreate	    (String token, IService service)  throws MqSException;
  /// \api #MqServiceProxy
  public native void	ServiceProxy	    (String token)  throws MqSException;
  /// \api #MqServiceProxy
  public native void	ServiceProxy	    (String token, int id)  throws MqSException;
  /// \api #MqServiceDelete
  public native void	ServiceDelete	    (String token)  throws MqSException;

  private native void	pProcessEvent	    (int timeout, int flags)  throws MqSException;

  /// \api #MqProcessEvent
  public void ProcessEvent (int timeout, WAIT flags) throws MqSException {
    pProcessEvent(timeout,flags.GetFlag());
  }
  /// \api #MqProcessEvent
  public void ProcessEvent (TIMEOUT timeout, WAIT flags) throws MqSException {
    pProcessEvent(timeout.GetFlag(),flags.GetFlag());
  }
  /// \api #MqProcessEvent
  public void ProcessEvent (WAIT flags) throws MqSException {
    pProcessEvent(MqS.TIMEOUT.DEFAULT.GetFlag(),flags.GetFlag());
  }
  /// \api #MqProcessEvent
  public void ProcessEvent () throws MqSException {
    pProcessEvent(MqS.TIMEOUT.DEFAULT.GetFlag(),WAIT.NO.GetFlag());
  }

/// \} Mq_Service_Java_API

//
// ========================================================================
/// \defgroup Mq_Error_Java_API Mq_Error_Java_API
/// \ingroup Mq_Java_API
/// \{
/// \brief \copybrief Mq_Error_C_API
/// \copydoc Mq_Error_C_API
//

  /// \api #MqErrorC
  public native void    ErrorC              (String func, int num, String message);
  /// \brief throw an exception with the data from #MqErrorS
  public native void    ErrorRaise          ()  throws MqSException;
  /// \api #MqErrorReset
  public native void    ErrorReset          ();

  /// \api #MqErrorSet
  public native void    ErrorSet            (Throwable ex);
  /// \api #MqErrorSetCONTINUE
  public native void    ErrorSetCONTINUE    ();
  /// \api #MqErrorSetEXIT
  public native void    ErrorSetEXIT	    ();
  /// \api #MqErrorIsEXIT
  public native boolean	ErrorIsEXIT	    ();

  /// \api #MqErrorGetNum
  public native int     ErrorGetNum         ();
  /// \api #MqErrorGetText
  public native String  ErrorGetText        ();
  /// \api #MqErrorPrint
  public native void    ErrorPrint          ();
  /// \api #MqErrorPrint
  public void ErrorPrint (Throwable ex) {
    ErrorSet (ex);
    ErrorPrint ();
  }

/// \} Mq_Error_Java_API

//
// ========================================================================
/// \defgroup Mq_Read_Java_API Mq_Read_Java_API
/// \ingroup Mq_Java_API
/// \{
/// \brief \copybrief Mq_Read_C_API
/// \copydoc Mq_Read_C_API
//

  /// \api #MqReadI
  public native byte	ReadY		    ()	throws MqSException;
  /// \api #MqReadI
  public native boolean	ReadO		    ()	throws MqSException;
  /// \api #MqReadI
  public native short	ReadS		    ()	throws MqSException;
  /// \api #MqReadI
  public native int	ReadI		    ()	throws MqSException;
  /// \api #MqReadF
  public native float	ReadF		    ()	throws MqSException;
  /// \api #MqReadW
  public native long	ReadW		    ()	throws MqSException;
  /// \api #MqReadD
  public native double	ReadD		    ()	throws MqSException;
  /// \api #MqReadC
  public native String	ReadC		    ()	throws MqSException;
  /// \api #MqReadB
  public native byte[]	ReadB		    ()	throws MqSException;
  /// \api #MqReadN
  public native byte[]	ReadN		    ()	throws MqSException;
  /// \api #MqReadBDY
  public native byte[]	ReadBDY		    ()	throws MqSException;
  /// \api #MqReadU
  public native MqBufferS ReadU		    ()	throws MqSException;
  /// \api #MqReadProxy
  public native void	ReadProxy    (MqS hdl)	throws MqSException;
  /// \api #MqReadL_START
  public native void	ReadL_START	    ()	throws MqSException;
  /// \api #MqReadL_START
  public native void	ReadL_START (MqBufferS buf)	throws MqSException;
  /// \api #MqReadL_END
  public native void	ReadL_END	    ()	throws MqSException;
  /// \api #MqReadT_START
  public native void	ReadT_START	    ()	throws MqSException;
  /// \api #MqReadT_START
  public native void	ReadT_START (MqBufferS buf)	throws MqSException;
  /// \api #MqReadT_END
  public native void	ReadT_END	    ()	throws MqSException;
  /// \api #MqReadGetNumItems
  public native int	ReadGetNumItems	    ()	throws MqSException;
  /// \api #MqReadItemExists
  public native boolean	ReadItemExists	    ()	throws MqSException;
  /// \api #MqReadUndo
  public native void	ReadUndo	    ()	throws MqSException;

/// \}

//
// ========================================================================
/// \defgroup Mq_Send_Java_API Mq_Send_Java_API
/// \ingroup Mq_Java_API
/// \{
/// \brief \copybrief Mq_Send_C_API
/// \copydoc Mq_Send_C_API

  /// \api #MqSendSTART
  public native void	SendSTART		();
  /// \api #MqSendEND
  public native void	SendEND			(String token)	throws MqSException;
  /// \api #MqSendEND_AND_WAIT
  public native void	SendEND_AND_WAIT	(String token) throws MqSException;
  /// \api #MqSendEND_AND_WAIT
  public native void	SendEND_AND_WAIT	(String token, int timeout) throws MqSException;
  /// \api #MqSendEND_AND_CALLBACK
  public native void	SendEND_AND_CALLBACK	(String token, ICallback callback)  throws MqSException;
  /// \api #MqSendRETURN
  public native void	SendRETURN		()  throws MqSException;
  /// \api #MqSendERROR
  public native void    SendERROR		() throws MqSException;
  /// \api #MqSendY
  public native void	SendY			(byte y)  throws MqSException;
  /// \api #MqSendO
  public native void	SendO			(boolean o) throws MqSException;
  /// \api #MqSendS
  public native void	SendS			(short s) throws MqSException;
  /// \api #MqSendI
  public native void	SendI			(int i)	throws MqSException;
  /// \api #MqSendF
  public native void	SendF			(float f) throws MqSException;
  /// \api #MqSendW
  public native void	SendW			(long w)  throws MqSException;
  /// \api #MqSendD
  public native void	SendD			(double l)  throws MqSException;
  /// \api #MqSendC
  public native void	SendC			(String s)  throws MqSException;
  /// \api #MqSendB
  public native void	SendB			(byte[] b)  throws MqSException;
  /// \api #MqSendN
  public native void	SendN			(byte[] b)  throws MqSException;
  /// \api #MqSendBDY
  public native void	SendBDY			(byte[] b)  throws MqSException;
  /// \api #MqSendU
  public native void	SendU			(MqBufferS buf)	throws MqSException;
  /// \api #MqSendL_START
  public native void	SendL_START		()  throws MqSException;
  /// \api #MqSendL_END
  public native void	SendL_END		()  throws MqSException;
  /// \api #MqSendT_START
  public native void	SendT_START		(String token)  throws MqSException;
  /// \api #MqSendT_END
  public native void	SendT_END		()  throws MqSException;

/// \} Mq_Send_Java_API

//
// ========================================================================
/// \defgroup Mq_Slave_Java_API Mq_Slave_Java_API
/// \ingroup Mq_Java_API
/// \{
/// \brief \copybrief Mq_Slave_C_API
/// \copydoc Mq_Slave_C_API
//

  /// \api #MqSlaveWorker
  public native void	SlaveWorker	    (int id, String... args)  throws MqSException;
  /// \api #MqSlaveCreate
  public native void	SlaveCreate	    (int id, MqS slave)	      throws MqSException;
  /// \api #MqSlaveDelete
  public native void	SlaveDelete	    (int id)		      throws MqSException;
  /// \api #MqSlaveGet
  public native MqS	SlaveGet	    (int id);
  /// \api #MqSlaveGetMaster
  public native MqS	SlaveGetMaster	    ();
  /// \api #MqSlaveIs
  public native boolean	SlaveIs		    ();

/// \} Mq_Slave_Java_API

//
// ========================================================================
/// \defgroup Mq_Factory_Java_API Mq_Factory_Java_API
/// \ingroup Mq_Java_API
/// \{
/// \brief \copybrief Mq_Factory_C_API
/// \copydoc Mq_Factory_C_API
//

  /// \api #MqFactoryCtxIdentSet
  public native void FactoryCtxIdentSet	      (String val);
  /// \api #MqFactoryCtxIdentGet
  public native String FactoryCtxIdentGet     ();
  /// \api #MqConfigSetDefaultFactory
  public native void FactoryCtxDefaultSet     (String ident);

/// \} Mq_Factory_Java_API
}

/// \} Mq_Java_API

