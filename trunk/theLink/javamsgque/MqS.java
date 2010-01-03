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

///\brief The \javamsgque MqS Package
package javamsgque;

/** \defgroup javamain_api MqS Class
\{
\brief Java-Msgque Super-Class

The #javamsgque.MqS class ist used as super-class for a \javamsgque based application.
The class is a \e abstract because and requires a top-level class wrapper to implement
virtual functions like:
 - void #javamsgque::IServerSetup::ServerSetup() throws MqSException
 - void #javamsgque::IServerCleanup::ServerCleanup() throws MqSException
 - void #javamsgque::IFactory::Factory() throws MqSException
 .

This is an example for a valid application class:
\code
import javamsgque.*;
class MyApplication extends MqS implements IServerSetup {
....
}
\endcode
*/


///\brief Java-Msgque Super-Class
public abstract class MqS {

  static {
    System.loadLibrary("javamsgque");
  }

  private long hdl;

//
// ========================================================================
/// \ingroup javamain_api
/// \defgroup javamisc_api Misc Functions
/// \{
/// \brief A collection of Misc Functions

  /// \brief constructor used to create a \e parent object
  public native void LinkCreate(String... args) throws MqSException;

  /// \brief constructor used to create a \e child object
  public native void LinkCreateChild(MqS parent, String... args) throws MqSException;

  private native void ContextCreate();
  private native void ContextDelete();

  /// \brief default constructor used for the object factory
  protected MqS() {
    ContextCreate();
  }

  /// \brief if the object is connected (hdl != 0) -> disconnect it
  protected void finalize() {
    ContextDelete();
  } 

  /// \api #MqLinkDelete
  public native void LinkDelete ();

  /// \api #MqExit
  public native void Exit ();

  protected static native void	Init	    (String... argv);

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

  private native void	pProcessEvent	    (int timeout, int flags)  throws MqSException;

  /// \api #MqProcessEvent, wait for \e timeout seconds and process event or raise an error
  public void ProcessEvent (int timeout, WAIT flags) throws MqSException {
    pProcessEvent(timeout,flags.GetFlag());
  }
  /// \api #MqProcessEvent, wait application user default time
  public void ProcessEvent (WAIT flags) throws MqSException {
    pProcessEvent(-2,flags.GetFlag());
  }
  /// \api #MqProcessEvent, don't wait just check for an event
  public void ProcessEvent () throws MqSException {
    pProcessEvent(-2,WAIT.NO.GetFlag());
  }

  /// \api #MqErrorC
  public native void    ErrorC              (String func, int num, String message);
  public native void    ErrorRaise          ()  throws MqSException;
  public native void    ErrorReset          ();

  /// \api #MqErrorC
  public native void    ErrorSet            (Throwable ex);

  /// \api #MqErrorC
  public native int     ErrorGetNum         ();
  public native String  ErrorGetText        ();
  public native void    ErrorPrint          ();

/// \}

//
// ========================================================================
/// \ingroup javamain_api
/// \defgroup javaservice_api Service Management
/// \{
/// \brief Setup and delete services
///
/// A service is usually defined on a server (interface #javamsgque.IServerSetup)
/// in the #javamsgque.IServerSetup.ServerSetup function.
///
  /// \api #MqServiceCreate
  public native void	ServiceCreate	    (String token, IService service)  throws MqSException;
  /// \api #MqServiceProxy
  public native void	ServiceProxy	    (String token)  throws MqSException;
  /// \api #MqServiceProxy
  public native void	ServiceProxy	    (String token, int id)  throws MqSException;
  /// \api #MqServiceDelete
  public native void	ServiceDelete	    (String token)  throws MqSException;

/// \}
//
// ========================================================================
/// \ingroup javamain_api
/// \defgroup javasend_api Send Data
/// \{
/// \brief Collect and send data using a LibMsgque package

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
  /// \api #SendERROR
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

/// \}
//
// ========================================================================
/// \ingroup javamain_api
/// \defgroup javaread_api Read Data
/// \{
/// \brief Read and split a \libmsgque package into several items
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
  /// \api #MqReadGetNumItems
  public native int	ReadGetNumItems	    ()	throws MqSException;
  /// \api #MqReadItemExists
  public native boolean	ReadItemExists	    ()	throws MqSException;
  /// \api #MqReadUndo
  public native void	ReadUndo	    ()	throws MqSException;

/// \}

//
// ========================================================================
/// \ingroup javamain_api
/// \defgroup javaconfig_api Access To Configuration Data
/// \{
/// \brief Configuration data of the \javamsgque object
//

  /// \api #MqConfigSetName
  public native void ConfigSetName	      (String val);
  /// \api #MqConfigSetSrvName
  public native void ConfigSetSrvName	      (String val);
  /// \api #MqConfigSetIdent
  public native void ConfigSetIdent	      (String val);
  /// \api #MqConfigCheckIdent
  public native boolean ConfigCheckIdent      (String val);
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
  public native void ConfigSetStartAs	      (int val);
  /// \api #MqConfigSetIoTcp
  public native void ConfigSetIoTcp	      (String host, String port, String myhost, String myport);
  /// \api #MqConfigSetIoUds
  public native void ConfigSetIoUds	      (String file);
  /// \api #MqConfigSetIoPipe
  public native void ConfigSetIoPipe	      (int socket);

  /// \api #MqConfigGetIsServer
  public native boolean	ConfigGetIsServer     ();
  /// \api #MqConfigGetIsParent
  public native boolean	ConfigGetIsParent     ();
  /// \api #MqConfigGetIsSlave
  public native boolean	ConfigGetIsSlave      ();
  /// \api #MqConfigGetIsString
  public native boolean	ConfigGetIsString     ();
  /// \api #MqConfigGetIsSilent
  public native boolean	ConfigGetIsSilent     ();
  /// \api #MqConfigGetIsConnected
  public native boolean	ConfigGetIsConnected  ();
  /// \api #MqConfigGetBuffersize
  public native int	ConfigGetBuffersize   ();
  /// \api #MqConfigGetTimeout
  public native long	ConfigGetTimeout      ();
  /// \api #MqConfigGetName
  public native String	ConfigGetName	      ();
  /// \api #MqConfigGetSrvName
  public native String	ConfigGetSrvName      ();
  /// \api #MqConfigGetIdent
  public native String	ConfigGetIdent	      ();
  /// \api #MqConfigGetDebug
  public native int	ConfigGetDebug	      ();
  /// \api #MqConfigGetParent
  public native MqS	ConfigGetParent	      ();
  /// \api #MqConfigGetMaster
  public native MqS	ConfigGetMaster	      ();
  /// \api #MqConfigGetFilter
  public native MqS	ConfigGetFilter	      () throws MqSException;
  /// \api #MqConfigGetFilter
  public native MqS	ConfigGetFilter	      (int id) throws MqSException;
  /// \api #MqConfigGetCtxId
  public native int	ConfigGetCtxId	      ();
  /// \api #MqConfigGetToken
  public native String	ConfigGetToken	      ();
  /// \api #MqConfigGetIsTransaction
  public native boolean	ConfigGetIsTransaction();
  /// \api #MqConfigGetStartAs
  public native int	ConfigGetStartAs      ();
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

/// \}
//
// ========================================================================
/// \ingroup javamain_api
/// \defgroup javaslave_api Manage Slave Objects
/// \{
/// \brief Configuration data of the \javamsgque object
//

  public native void	SlaveWorker	    (int id, String... args)  throws MqSException;
  /// \api #MqSlaveCreate
  public native void	SlaveCreate	    (int id, MqS slave)	      throws MqSException;
  /// \api #MqSlaveDelete
  public native void	SlaveDelete	    (int id)		      throws MqSException;
  /// \api #MqSlaveGet
  public native MqS	SlaveGet	    (int id);

/// \}
}

/// \}   end MqS

