/**
 *  \file       libmsgque/sys.h
 *  \brief      \$Id: sys.h 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
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
/*                                 sys_init                                  */
/*                                                                           */
/*****************************************************************************/

/*
typedef void (
  MQ_DECL *AtExitP
) (
  void
);
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
enum MqErrorE SysGetEnv (
  struct MqS * const context,
  MQ_CST name,
  MQ_STR *ret
);

enum MqErrorE SysSetEnv (
  struct MqS * const context,
  MQ_CST name,
  MQ_CST str
);

enum MqErrorE SysUnSetEnv (
  struct MqS * const context,
  MQ_CST name
);

/*****************************************************************************/
/*                                                                           */
/*                                sys_file                                   */
/*****************************************************************************/

/*
enum MqErrorE SysOpen (
  struct MqS * const context,
  MQ_STR pathname,
  int flags,
  mode_t mode,
  MQ_INT *fh
);

enum MqErrorE SysStat (
  struct MqS * const context,
  const MQ_STR file,
  struct stat *st
);
*/

/*****************************************************************************/
/*                                                                           */
/*                                sys_io                                     */
/*                                                                           */
/*****************************************************************************/

/*
enum MqErrorE SysRead (
  struct MqS * const context,
  MQ_INT hdl,
  MQ_BIN buf,
  const MQ_SIZE numBytes
);

enum MqErrorE SysWrite (
  struct MqS * const context,
  MQ_INT hdl,
  MQ_BIN buf,
  const MQ_SIZE numBytes
);

enum MqErrorE SysClose (
  struct MqS * const context,
  MQ_INT *hdl
);

*/

#if defined(MQ_IS_POSIX)
enum MqErrorE SysUnlink (
  struct MqS * const context,
  const MQ_STR fileName
);
#endif

/*****************************************************************************/
/*                                                                           */
/*                                sys_misc                                   */
/*                                                                           */
/*****************************************************************************/

/*
enum MqErrorE SysAtExit (
  struct MqS * const context,
  AtExitP proc
);
*/

void SysExit (
  int isThread,
  int num
) __attribute__ ((noreturn));

void SysAbort (
  void
) __attribute__ ((noreturn));

enum MqErrorE SysWait (struct MqS * const, const struct MqIdS*);

#if defined(HAVE_FORK)
enum MqErrorE SysServerFork (
  struct MqS * const,
  struct MqFactoryS,
  struct MqBufferLS **,
  struct MqBufferLS **,
  MQ_CST,
  struct MqIdS *
);
#endif

#if defined(MQ_HAS_THREAD)
enum MqErrorE SysServerThread (
  struct MqS * const,
  struct MqFactoryS,
  struct MqBufferLS **,
  struct MqBufferLS **,
  MQ_CST,
  int,
  struct MqIdS *
);
#endif

enum MqErrorE SysServerSpawn (
  struct MqS * const context,
  char * * argv,
  MQ_CST name,
  struct MqIdS * idP
);

#ifdef HAVE_FORK
enum MqErrorE SysFork (
  struct MqS * const context,
  struct MqIdS * idP
);
#endif

#ifdef MQ_IS_POSIX
enum MqErrorE
SysIgnorSIGCHLD (
  struct MqS * const context
);

enum MqErrorE
SysAllowSIGCHLD (
  struct MqS * const context
);

enum MqErrorE
SysDaemonize (
  struct MqS * const context,
  MQ_CST pidfile
);
#else
#define SysIgnorSIGCHLD(error) MQ_OK
#define SysAllowSIGCHLD(error) MQ_OK
#define SysDaemonize(error,pidfile) MQ_OK
#endif /* MQ_IS_POSIX */

enum MqErrorE
SysUnLink (
  struct MqS * const context,
  const MQ_STR filename
);

void SysFreeArgvArray (
  char *** argvP
);

/** \} */

END_C_DECLS

#endif /* SYS_H */


