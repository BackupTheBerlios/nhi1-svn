/**
 *  \file       theLink/libmsgque/sys.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#define MQ_IN_SYS_C
#include "main.h"

#include "sys.h"
#include "error.h"
#include "mq_io.h"
#include "log.h"

#include <errno.h>
#include <fcntl.h>

#if !defined(_MSC_VER)
#  include <libgen.h>
#endif

#ifdef MQ_IS_WIN32
# if !defined(_MSC_VER)
//	we require WindowsXP or higher -> used for "getaddrinfo"
#   define WINVER WindowsXP
#   include <w32api.h>
# endif
#   include <windows.h>
#   include <process.h>
#else
#   include <sys/wait.h>
#  if defined(HAVE_POSIX_SPAWN)
#   include <spawn.h>
#  endif
#  if defined(HAVE_SIGNAL_H)
#   include <signal.h>
#  endif
#endif

#if defined(MQ_HAS_THREAD)
# if defined(HAVE_PTHREAD)
#  define mqthread_ret_t void*
#  define mqthread_stdcall
# elif defined(MQ_IS_WIN32)
#  define mqthread_ret_t unsigned
#  if defined(_MANAGED)
//   MS awitch "/clr" is active
#    define mqthread_stdcall
#  else
#    define mqthread_stdcall __stdcall
#  endif
# endif
#endif /* MQ_HAS_THREAD */

#define MqErrorSys(cmd) \
  MqErrorV (context, __func__, errno, \
    "can not '%s' -> ERR<%s>", MQ_CPPXSTR(cmd), strerror (errno))

#define MqErrorSysAppend(cmd) \
  MqErrorSAppendV (context,\
    "can not '%s' -> ERR<%s>", MQ_CPPXSTR(cmd), strerror (errno))

#define MQ_CONTEXT_S context

BEGIN_C_DECLS

#ifdef HAVE_FORK
static MqForkF mq_fork = fork;
#else
static MqForkF mq_fork = NULL;
#endif

#ifdef HAVE_VFORK
static MqVForkF mq_vfork = vfork;
#else
static MqVForkF mq_vfork = NULL;
#endif

/*****************************************************************************/
/*                                                                           */
/*                               sys_memory                                  */
/*                                                                           */
/*****************************************************************************/

static MQ_PTR 
SysCalloc (
  struct MqS * const context,
  MQ_SIZE nmemb,
  MQ_SIZE size
)
{
  MQ_PTR ptr = calloc (nmemb, size);
  if (unlikely (ptr == NULL))
    MqErrorC (context, __func__, errno, strerror (errno));
  return ptr;
}

static MQ_PTR 
SysMalloc (
  struct MqS * const context,
  MQ_SIZE size
)
{
  MQ_PTR ptr = malloc (size);
  if (unlikely (ptr == NULL))
    MqErrorC (context, __func__, errno, strerror (errno));
  return ptr;
}

static MQ_PTR
SysRealloc (
  struct MqS * const context,
  MQ_PTR buf,
  MQ_SIZE size
)
{
  MQ_PTR ptr = realloc (buf, size);
  if (unlikely (ptr == NULL))
    MqErrorC (context, __func__, errno, strerror (errno));
  return ptr;
}

static void
SysFreeP (
  MQ_PTR ptr
)
{
  if ( likely((ptr) != (NULL)) ) free(ptr);
}

/*****************************************************************************/
/*                                                                           */
/*                                sys_env                                    */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE
SysGetEnv (
  struct MqS * const context,
  MQ_CST str,
  MQ_STR *ret
)
{
  if ((*ret = (MQ_STR) getenv (str)) == NULL) {
    return MqErrorV (context, __func__, errno, "can not read the environment variable \"%s\"", str);
  }
  return MQ_OK;
}

/*****************************************************************************/
/*                                                                           */
/*                                sys_io                                     */
/*                                                                           */
/*****************************************************************************/

#if defined(MQ_IS_POSIX)
static enum MqErrorE
SysUnlink (
  struct MqS * const context,
  const MQ_STR fileName
)
{
  struct stat buf;
  if (unlikely (stat (fileName, &buf) == -1)) {
    switch (errno) {
      case ENOENT:
	return MQ_OK;
      default:
	return MqErrorV (context, __func__, errno,
	  "can not access file <%s> ERR<%s>", fileName, strerror (errno));
    }
  }
  if (unlikely (unlink (fileName) == -1 && errno != ENOENT)) {
    return MqErrorV (context, __func__, errno,
                     "can not delete file <%s> ERR<%s>", fileName, strerror (errno));
  }
  return MQ_OK;
}
#endif

#if defined(_MSC_VER)

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif
 
static int gettimeofday(
    struct mq_timeval *tv, 
    struct mq_timezone *tz
)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag;
 
  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);
 
    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;
 
    /*converting file time to unix epoch*/
    tmpres /= 10;  /*convert into microseconds*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS; 
    tv->tv_sec = (long)(tmpres / 1000000UL);
    tv->tv_usec = (long)(tmpres % 1000000UL);
  }
 
  if (NULL != tz)
  {
    if (!tzflag)
    {
      _tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }
 
  return 0;
}

#endif /* ! _MSC_VER */

static enum MqErrorE
SysGetTimeOfDay (
  struct MqS * const context,
  struct mq_timeval * tv,
  struct mq_timezone * tz
)
{
  if (unlikely (gettimeofday (tv, tz) == -1)) {
    return MqErrorSys (gettimeofday);
  }
  return MQ_OK;
}

/*****************************************************************************/
/*                                                                           */
/*                                sys_misc                                   */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE
SysWait (
  struct MqS * const context,
  const struct MqIdS *idP
)
{
  errno = 0;
  switch (idP->type) {
    case MQ_ID_PROCESS: {
/*
  we don't wait for a process because this is buggy (for example BSD wait forever)
#if defined(MQ_IS_POSIX)
      if ( waitpid (idP->val.process, NULL, 0) == -1) {
        return MqErrorSys (waitpid);
      }
#elif defined(MQ_IS_WIN32)
      if ( _cwait (NULL, idP->val.process, _WAIT_CHILD) == -1) {
        return MqErrorSys (waitpid);
      }
#endif
*/
      break;
    }
#if defined(MQ_HAS_THREAD)
    case MQ_ID_THREAD: {
# if defined(HAVE_PTHREAD)
      if ( pthread_join (idP->val.thread, NULL)) {
        return MqErrorSys (pthread_join);
      }
# elif defined(MQ_IS_WIN32)
      if ( WaitForSingleObject((HANDLE)idP->val.thread, INFINITE ) == WAIT_FAILED) {
        return MqErrorSys(WaitForSingleObject);
      }
# endif
      break;
    }
#endif   /* MQ_HAS_THREAD */
    case MQ_ID_UNUSED: {
      break;
    }
  }
  return MQ_OK;
}

#if defined(HAVE_FORK)
static enum MqErrorE
SysServerFork (
  struct MqS * const context,  ///< [ini,out] error handler
  struct MqFactoryS   factory,	      ///< [in,out] server configuration (memroy will be freed)
  struct MqBufferLS ** argvP,	      ///< [in] command-line arguments befor #MQ_ALFA
  struct MqBufferLS ** alfaP,	      ///< [in] command-line arguments after #MQ_ALFA
  MQ_CST  name,			      ///< [in] the name of the process
  struct MqIdS * idP		      ///< [out] the process identifer
)
{
  
  // fork setup !before! a fork is created
  if (context->setup.fForkInit) (*context->setup.fForkInit)(context);

  // this is used for a filter pipeline like "| atool split .. @ cut ... @ join ..."
  // argv[0] is set by the tool (cut or join) and need !not! be set by name
  MqErrorCheck(MqSysFork(context,idP));
  if ((*idP).val.process == 0) {
    struct MqS * newctx;
    // prevent the "client-context" from deleting in the new process
    // reason: perl has a garbage collection, after fork this "context" is in the
    // new process but with an invalid "event-link" (pIoCreate was done but pIoconnect not)
    // and "pEventCreate" can not set the "MqContextDelete_LOCK"
    context->bits.MqContextDelete_LOCK = MQ_YES;
    // create the new context
    MqErrorCheck1(pCallFactory (context, MQ_FACTORY_NEW_FORK, factory, &newctx));
    // add my configuration
    newctx->statusIs = (enum MqStatusIsE) (newctx->statusIs | MQ_STATUS_IS_FORK);
    // the new CONTEXT is always a SERVER
    newctx->setup.isServer = MQ_YES;
    // join argvP alfaP
    if (alfaP != NULL && *alfaP != NULL) {
      MqBufferLAppendC(*argvP, MQ_ALFA_STR);
      MqBufferLMove(*argvP, alfaP);
    }
    // create link
    MqErrorCheck1(MqLinkCreate(newctx,argvP));
    // the MqProcessEvent is necessary becasue "ParentCreate" have to come back
    // ifNot: java create object will fail
    if (newctx->setup.isServer == MQ_YES)
      MqProcessEvent(newctx, MQ_TIMEOUT_USER, MQ_WAIT_FOREVER);
error1:
    MqExit(newctx);
  }
  MqBufferLDelete (argvP);
  MqBufferLDelete (alfaP);
  // do not create a "defunc" process
  MqErrorCheck (MqSysIgnorSIGCHLD(context));
  return MQ_OK;

error:
  return MqErrorDbV (MQ_ERROR_CAN_NOT_START_SERVER, name);
}
#endif   /* HAVE_FORK */

#if defined(MQ_HAS_THREAD)

struct SysServerThreadCreateS {
  struct MqS * tmpl;
  struct MqFactoryS   factory;
  struct MqBufferLS * argv;
  struct MqBufferLS * alfa;
};

static mqthread_ret_t mqthread_stdcall sSysServerThreadCreate (
  void * data
) 
{
  struct SysServerThreadCreateS * argP = (struct SysServerThreadCreateS *) data;
  // save data local
  struct MqS * tmpl  = argP->tmpl;
  struct MqFactoryS factory = argP->factory;
  struct MqBufferLS * argv  = argP->argv;
  struct MqBufferLS * alfa  = argP->alfa;
  struct MqS * newctx;
  // cleanup
  free(argP);
  // create the new context
  MqErrorCheck(pCallFactory (tmpl, MQ_FACTORY_NEW_THREAD, factory, &newctx));
  // add my configuration
  newctx->statusIs = (enum MqStatusIsE) (newctx->statusIs | MQ_STATUS_IS_THREAD);
  // the new CONTEXT is always a SERVER
  newctx->setup.isServer = MQ_YES;
  // join argvP alfaP
  if (alfa != NULL) {
    MqBufferLAppendC(argv, MQ_ALFA_STR);
    MqBufferLMove(argv, &alfa);
  }
  // create link
  MqErrorCheck(MqLinkCreate(newctx, &argv));
  //MqDLogC(msgque,0,"THREAD exit");
  // the MqProcessEvent is necessary because "ParentCreate" have to come back
  // ifNot: java create object will fail
  if (newctx->setup.isServer == MQ_YES) {
    MqProcessEvent(newctx, MQ_TIMEOUT_USER, MQ_WAIT_FOREVER);
  }
error:
  MqBufferLDelete (&argv);
  MqExit(newctx);
  return (mqthread_ret_t) NULL;
}

/// \brief start a new thread
static enum MqErrorE
SysServerThread (
  struct MqS * const context,  ///< [in,out] error handler
  struct MqFactoryS factory,	      ///< [in,out] server configuration (memory will be freed)
  struct MqBufferLS ** argvP,	      ///< [in] command-line arguments befor #MQ_ALFA, owned by SysServerThread
  struct MqBufferLS ** alfaP,	      ///< [in] command-line arguments after #MQ_ALFA, owned by SysServerThread
  MQ_CST  name,			      ///< [in] the name of the process
  int state,			      ///< [in] detachstate of the thread \c PTHREAD_CREATE_DETACHED or 
				      ///< \c PTHREAD_CREATE_JOINABLE
  struct MqIdS * idP		      ///< [out] the thread identifer
)
{
  mqthread_t threadId;
#if defined(HAVE_PTHREAD)
  int ret;
#endif

  // fill thread data
  struct SysServerThreadCreateS * argP = (struct SysServerThreadCreateS *) MqSysMalloc(MQ_ERROR_PANIC,sizeof(*argP));
  argP->factory = factory;
  argP->argv = *argvP;
  argP->alfa = *alfaP;
  argP->tmpl = context;

  // pointers are owned by SysServerThread
  *argvP = NULL;
  *alfaP = NULL;

  // after a "thread" no "fork" is possible
  MqConfigSetIgnoreFork (context, MQ_YES);

#if defined(HAVE_PTHREAD)

  // thread attributes
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, state);
  
  // thread setup !before! a thread was created
  if (context->setup.fThreadInit) (*context->setup.fThreadInit)(context);

  // start thread
  do {
    ret = pthread_create(&threadId,&attr,sSysServerThreadCreate,argP);
  } while (ret == EAGAIN);

  // cleanup attribute
  pthread_attr_destroy(&attr);

  if (unlikely(ret != 0)) {
    MqErrorDbV (MQ_ERROR_CAN_NOT_START_SERVER, name);
    goto error;
  }

#elif defined(MQ_IS_WIN32)

  if (unlikely ( (threadId = _beginthreadex(NULL, 0, sSysServerThreadCreate, argP, 0, NULL)) == 0)) {
    MqErrorDbV (MQ_ERROR_CAN_NOT_START_SERVER, name);
    MqErrorSysAppend (_beginthreadex);
    goto error;
  }

#endif

  // save tid
  (*idP).val.thread = threadId;
  (*idP).type = MQ_ID_THREAD;

  return MQ_OK;

error:
  MqBufferLDelete (&argP->argv);
  MqBufferLDelete (&argP->alfa);
  MqSysFree (argP);
  return MQ_ERROR;
}
#endif   /* MQ_HAS_THREAD */

/// \brief spawn a new process
static enum MqErrorE
SysServerSpawn (
  struct MqS * const context,  ///< [ini,out] error handler
  char * * argv,		  ///< [in] command-line arguments
  MQ_CST  name,			  ///< [in] the name of the process
  struct MqIdS * idP		  ///< [out] the process identifer
)
{
  pid_t pid;
  
  // spawn setup !before! a spawn is created
  if (context->setup.fSpawnInit) (*context->setup.fSpawnInit)(context);

#if defined(HAVE_POSIX_SPAWN)
  if (unlikely (posix_spawnp(&pid, name, NULL, NULL, (char *const *) argv, __environ) != 0)) {
    goto error;
  }
  goto ok;

#elif (defined(HAVE_FORK) || defined(HAVE_VFORK))  && defined(HAVE_EXECVP)

  // fork to create the child
  if (mq_vfork != NULL) {
    if (unlikely ((pid = mq_vfork()) == -1)) goto error;
  } else if (mq_fork != NULL) {
    if (unlikely ((pid = mq_fork()) == -1)) goto error;
  } else {
    goto error;
  }
      
  if (pid == 0) {
    // this is the child
    if (execvp(name, argv) == -1) {
      _exit (EXIT_FAILURE);
    }
    // the child will never reach this
  }
  goto ok;

#elif defined(MQ_IS_WIN32)
  char buf[2048];
  char *nbuf=buf;

  // build command-line
  for (;*argv != '\0'; argv++) {
    nbuf += sprintf(nbuf, "\"%s\" ", *argv);
  }

//printC(name)
//printC(buf)
//system("set PATH");

  // start process
  if (unlikely ((pid = _spawnlp (_P_NOWAIT, name, buf, NULL)) == -1)) {
    //printC(strerror(errno))
    goto error;
  }
  goto ok;

#else
# error unable to SysServerSpawn
#endif

ok:
  (*idP).val.process = pid;
  (*idP).type = MQ_ID_PROCESS;
  return MQ_OK;

error:
  return MqErrorDbV (MQ_ERROR_CAN_NOT_START_SERVER, name);
}

static enum MqErrorE
SysUSleep (
  struct MqS * const context,
  unsigned int const usec
)
{
#if defined(MQ_IS_POSIX)
  if (unlikely (usleep (usec))) {
    return MqErrorSys (usleep);
  }
#elif defined (MQ_IS_WIN32)
  Sleep ((usec + 500) / 1000);
#else
#   error not supported
#endif
  return MQ_OK;
}

static enum MqErrorE
SysSleep (
  struct MqS * const context,
  unsigned int const sec
)
{
#if defined(MQ_IS_POSIX)
  if (unlikely (sleep (sec))) {
    return MqErrorSys (sleep);
  }
#elif defined (MQ_IS_WIN32)
  Sleep (sec * 1000);
#else
#   error not supported
#endif
  return MQ_OK;
}

#if defined(_MSC_VER)
static MQ_STR
SysBasename (
  MQ_CST const in,
  MQ_BOL includeExtension
)
{
  char drive[10];
  char dir[512];
  char * fname = (char *) MqSysMalloc(MQ_ERROR_PANIC, 138);
  char ext[10];
  if (_splitpath_s(in, drive, 10, dir, 512, fname, 128, ext, 10) != 0)
    MqPanicV(MQ_ERROR_PANIC, __func__, 1, "unable to extract basename from '%s'", in);
  if (includeExtension)
    return strcat(fname,ext);
  else
    return fname;
}
#else
static MQ_STR
SysBasename (
  MQ_CST const in,
  MQ_BOL includeExtension
)
{
  if (in == NULL) return mq_strdup("");
  // double mq_strdup is necessary because LINUX may modify the input string
  // and FREEBSD modify the output string in future call's to 'basename'
  // ... Is This POSIX ?
  MQ_STR t1 = mq_strdup((MQ_STR) in);
  const MQ_STR orig = mq_strdup(basename(t1));
  free (t1);
  if (includeExtension == MQ_NO) {
    // delete the extension like '.EXE'
    MQ_STR end;
    for (end = orig + strlen(orig); end > orig; end--) {
      if (*end != '.') continue;
      *end = '\0';
      break;
    }
  }
  return orig;
}
#endif

#ifdef HAVE_FORK
static enum MqErrorE
SysFork (
  struct MqS * const context,
  struct MqIdS * idP
)
{
  if (unlikely (((*idP).val.process = mq_fork()) == -1)) {
    return MqErrorSys (fork);
  }
  (*idP).type = MQ_ID_PROCESS;
  return MQ_OK;
}
#endif

#ifdef MQ_IS_POSIX
static enum MqErrorE
SysIgnorSIGCHLD (
  struct MqS * const context
)
{
  if (unlikely ((signal(SIGCHLD, SIG_IGN)) == SIG_ERR)) {
    return MqErrorSys (signal);
  }
  return MQ_OK;
}

static enum MqErrorE
SysAllowSIGCHLD (
  struct MqS * const context
)
{
  if (unlikely ((signal(SIGCHLD, SIG_DFL)) == SIG_ERR)) {
    return MqErrorSys (signal);
  }
  return MQ_OK;
}
#endif /* MQ_IS_POSIX */

#if defined(MQ_IS_POSIX)
/// \brief daemonize the current process and save the resulting pid into \e pidfile
static enum MqErrorE
SysDaemonize (
  struct MqS * const context,	///< [in,out] error handler
  MQ_CST pidfile		///< [in] file to save the \e pid
) 
{
  int fd, fpid;
  struct MqIdS id;

  // step 0 -> open the pidfile
  if ((fpid = open(pidfile, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) == -1) 
    return MqErrorSys (open);

  // step 1 -> delete initial process
  MqErrorCheck(SysFork(context, &id));
  if (id.val.process != 0)
    _exit(0);

  // step 2 -> create a new session
  if (setsid() == -1)
    return MqErrorSys (setsid);

  // step 3 -> fork the new process, parent write the pid
  MqErrorCheck(SysFork(context, &id));
  if (id.val.process != 0) {
    MQ_BUF buf = context->temp;
    MqBufferSetV(buf,"%i",id.val.process);
    write(fpid,buf->cur.C,buf->cursize);
    _exit(0);
  }

  // step 4 -> close the fpid after writing
  if (close(fpid) == -1)
    return MqErrorSys (close);

  // step 5 -> close default io
  fd = open("/dev/null", O_RDONLY);
  if (fd != 0) {
    dup2(fd, 0);
    close(fd);
  }
  fd = open("/dev/null", O_WRONLY);
  if (fd != 1) {
    dup2(fd, 1);
    close(fd);
  }
  fd = open("/dev/null", O_WRONLY);
  if (fd != 2) {
    dup2(fd, 2);
    close(fd);
  }

error:
  return MqErrorStack(context);
}

#endif /* ! defined(MQ_IS_POSIX) */

/// \brief thread save exit
static void SysExit (
  int isThread,			    ///< [in] is this a thread
  int num			    ///< [in] exit number
) {
  if (isThread) {
#if defined(MQ_HAS_THREAD)
# if defined(HAVE_PTHREAD)
    pthread_detach(pthread_self());
    pthread_exit(NULL);
# elif defined(MQ_IS_WIN32)
   _endthreadex(num);
# endif
#endif   /* MQ_HAS_THREAD */
  }
  exit(num);
}

/// \brief thread save abort
static void SysAbort (void) {
  abort();
}

/*****************************************************************************/
/*                                                                           */
/*                                MQ-LAL                                     */
/*                    (Language Abstraction Layer)                           */
/*                                                                           */
/*****************************************************************************/

struct MqLalS MqLal = {
  SysCalloc,
  SysMalloc,
  SysRealloc,
  SysFreeP,
  SysServerSpawn,
#if defined(MQ_HAS_THREAD)
  SysServerThread,
#endif
#if defined(HAVE_FORK)
  SysServerFork,
  SysFork,
#endif
  SysGetTimeOfDay,
  SysWait,
  SysUSleep,
  SysSleep,
  SysBasename,
#if defined(MQ_IS_POSIX)
  SysIgnorSIGCHLD,
  SysAllowSIGCHLD,
  SysDaemonize,
  SysUnlink,
#endif
  SysExit,
  SysAbort,
  SysGetEnv
};

END_C_DECLS

