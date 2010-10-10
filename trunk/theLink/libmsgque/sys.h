/**
 *  \file       theLink/libmsgque/sys.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef SYS_H
#   define SYS_H

#   include <sys/stat.h>

#if !defined(_MSC_VER)
#   include <sys/time.h>
#endif

struct stat;

BEGIN_C_DECLS

/** \defgroup sys_api INTERNAL: Unix/Linux/Windows API
 *  \{
 *  \brief system API layer
 *
 * All functions provided by the underlying OS and system libraries
 * are located in this file. Every external Function has additional
 * code to map OS/system specific error codes/messages into
 * msgque standard error codes/massages
 */

/*****************************************************************************/
/*                                                                           */
/*                                sys_env                                    */
/*                                                                           */
/*****************************************************************************/

/** \brief secure reading of an environment variable
 *
 * Environment variables are out of control by the programmer and
 * are sources of error. the following code is used to secure read,
 * write and delete environment variables and integrate the error
 * cases into the msgque error-handling.
 *
 * \context
 * \param name the name of the environment variable (z.B. \c HOME)
 * \param ret point to the resulting buffer space used to store the data of the environment variable
 * \return the error ststus
 */
#define MqSysGetEnv(context,name,ret) (*MqLal.SysGetEnv)(context,name,ret)

/*****************************************************************************/
/*                                                                           */
/*                                sys_io                                     */
/*                                                                           */
/*****************************************************************************/

#if defined(MQ_IS_POSIX)
#define MqSysUnlink(context,fileName) (*MqLal.SysUnlink)(context,fileName)
#endif

/*****************************************************************************/
/*                                                                           */
/*                                sys_misc                                   */
/*                                                                           */
/*****************************************************************************/

#define MqSysExit(isThread,num) (*MqLal.SysExit)(isThread,num)

#define MqSysAbort() (*MqLal.SysAbort)()

#define MqSysWait(context,idP) (*MqLal.SysWait)(context,idP)

#if defined(HAVE_FORK)
# define MqSysServerFork(context,factory,argvP,alfaP,name,idP) \
  (*MqLal.SysServerFork)(context,factory,argvP,alfaP,name,idP)
#endif

#if defined(MQ_HAS_THREAD)
# define MqSysServerThread(context,factory,argvP,alfaP,name,state,idP) \
  (*MqLal.SysServerThread)(context,factory,argvP,alfaP,name,state,idP)
#endif

#define MqSysServerSpawn(context,argv,name,idP) \
  (*MqLal.SysServerSpawn)(context,argv,name,idP)

#ifdef HAVE_FORK
#define MqSysFork(context,idP) (*MqLal.SysFork)(context,idP)
#endif

#ifdef MQ_IS_POSIX
# define MqSysIgnorSIGCHLD(context) (*MqLal.SysIgnorSIGCHLD)(context)
# define MqSysAllowSIGCHLD(context) (*MqLal.SysAllowSIGCHLD)(context)
# define MqSysDaemonize(context,pidfile) (*MqLal.SysDaemonize)(context,pidfile)
#else
# define MqSysIgnorSIGCHLD(error) MQ_OK
# define MqSysAllowSIGCHLD(error) MQ_OK
# define MqSysDaemonize(error,pidfile) MQ_OK
#endif

/** \} */

END_C_DECLS

#endif /* SYS_H */





