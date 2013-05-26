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

#include "main.h"

#include <errno.h>
#include <fcntl.h>

#include <sys/stat.h>
#if !defined(MQ_IS_WIN32)
#   include <sys/time.h>
#endif

#if !defined(MQ_IS_WIN32)
#  include <libgen.h>
#endif

#if defined(MQ_IS_POSIX) && defined(__CYGWIN__)
# include <process.h>
# define _spawnlp spawnlp
#endif

#ifdef MQ_IS_WIN32
# if !defined(MQ_IS_WIN32)
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
#  define mqthread_ret_NULL NULL
#  define mqthread_stdcall
# else
#  define mqthread_ret_t unsigned
#  define mqthread_ret_NULL 0
#  if defined(_MANAGED)
//   MS switch "/clr" is active
#    define mqthread_stdcall __clrcall 
#  else
#    define mqthread_stdcall __stdcall
#  endif
# endif
#endif /* MQ_HAS_THREAD */

#define MqErrorSys(cmd) \
  MqErrorV (context, __func__, errno, \
    "can not '%s' -> ERR<%s>", MQ_CPPXSTR(cmd), strerror (errno))

#define MqErrorSysAppend(cmd,num) \
  MqErrorSAppendV (context,\
    "can not '%s' -> ERR<%s>", MQ_CPPXSTR(cmd), strerror (num))

#define MQ_CONTEXT_S context

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                               sys_memory                                  */
/*                                                                           */
/*****************************************************************************/

MQ_PTR MqSysCalloc (
  struct MqS * const context,
  MQ_SIZE nmemb,
  MQ_SIZE size
)
{
  MQ_PTR ptr = (*MqLal.SysCalloc) (nmemb, size);
  if (unlikely (ptr == NULL))
    MqErrorC (context, __func__, errno, strerror (errno));
  return ptr;
}

MQ_PTR MqSysMalloc (
  struct MqS * const context,
  MQ_SIZE size
)
{
  MQ_PTR ptr = (*MqLal.SysMalloc) (size);
  if (unlikely (ptr == NULL))
    MqErrorC (context, __func__, errno, strerror (errno));
  return ptr;
}

MQ_STR MqSysStrDup (
  struct MqS * const context,
  MQ_CST str
)
{
  if (str == NULL) {
    return NULL;
  } else {
    MQ_STR ret = (*MqLal.SysStrDup) (str);
    if (unlikely (ret == NULL))
      MqErrorC (context, __func__, errno, strerror (errno));
    return ret;
  }
}

MQ_STR MqSysStrNDup (
  struct MqS * const context,
  MQ_CST str,
  MQ_SIZE len
)
{
  if (str == NULL) {
    return NULL;
  } else {
    MQ_STR ret = (*MqLal.SysStrNDup) (str, len);
    if (unlikely (ret == NULL))
      MqErrorC (context, __func__, errno, strerror (errno));
    return ret;
  }
}

MQ_PTR MqSysRealloc (
  struct MqS * const context,
  MQ_PTR buf,
  MQ_SIZE size
)
{
  MQ_PTR ptr = (*MqLal.SysRealloc) (buf, size);
  if (unlikely (ptr == NULL))
    MqErrorC (context, __func__, errno, strerror (errno));
  return ptr;
}

/*****************************************************************************/
/*                                                                           */
/*                                sys_io                                     */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE SysUnlink (
  struct MqS * const context,
  const MQ_STR fileName
)
{
#if defined(MQ_IS_POSIX)
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
#else
  return MqErrorDb(MQ_ERROR_NOT_SUPPORTED);
#endif
}

#if defined(MQ_IS_WIN32)

#if defined(_MSC_EXTENSIONS)
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

#endif /* ! MQ_IS_WIN32 */

enum MqErrorE MqSysGetTimeOfDay (
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

static enum MqErrorE SysWait (
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
    case MQ_ID_THREAD: {
# if defined(HAVE_PTHREAD)
      if ( pthread_join ((mqthread_t)idP->val, NULL)) {
        return MqErrorSys (pthread_join);
      }
# elif defined(MQ_IS_WIN32)
      if ( WaitForSingleObject((HANDLE)idP->val, INFINITE ) == WAIT_FAILED) {
        return MqErrorSys(WaitForSingleObject);
      }
# endif
      break;
    }
    case MQ_ID_UNUSED: {
      break;
    }
  }
  return MQ_OK;
}

static enum MqErrorE SysServerFork (
  struct MqS * const context,	  ///< [in,out] error handler
  struct MqFactoryS* factory, ///< [in,out] server configuration (memroy will be freed)
  struct MqBufferLS ** argvP,	  ///< [in] command-line arguments befor #MQ_ALFA
  struct MqBufferLS ** alfaP,	  ///< [in] command-line arguments after #MQ_ALFA
  MQ_CST  name,			  ///< [in] the name of the process
  struct MqIdS * idP		  ///< [out] the process identifer
)
{
#if defined(HAVE_FORK)
  
  // fork setup !before! a fork is created
  if (context->setup.fForkInit) (*context->setup.fForkInit)(context);

  // this is used for a filter pipeline like "| atool split .. @ cut ... @ join ..."
  // argv[0] is set by the tool (cut or join) and need !not! be set by name
  MqErrorCheck(SysFork(context,idP));
  if ((*idP).val == 0UL) {
    struct MqS * newctx;
    // prevent the "client-context" from deleting in the new process
    // reason: perl has a garbage collection, after fork this "context" is in the
    // new process but with an invalid "event-link" (pIoCreate was done but pIoconnect not)
    // and "pEventCreate" can not set the "MqContextDelete_LOCK"
    context->bits.MqContextDelete_LOCK = MQ_YES;
    // create the new context
    MqErrorCheck1(MqFactoryInvoke (context, MQ_FACTORY_NEW_FORK, factory, &newctx));
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
    // the MqProcessEvent is necessary because "ParentCreate" have to come back
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
#else
  return MqErrorDb (MQ_ERROR_NOT_SUPPORTED);
#endif   /* HAVE_FORK */
}

void MqSysServerThreadMain (
  struct MqSysServerThreadMainS * argP
) 
{
  // save data local
  struct MqS * tmpl  = argP->tmpl;
  struct MqFactoryS * factory = argP->factory;
  struct MqBufferLS * argv  = argP->argv;
  struct MqBufferLS * alfa  = argP->alfa;
  struct MqS * newctx;
  // cleanup
  MqSysFree(argP);
  // create the new context
  MqErrorCheck(MqFactoryInvoke (tmpl, MQ_FACTORY_NEW_THREAD, factory, &newctx));
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
}

#if defined(MQ_HAS_THREAD)
static mqthread_ret_t mqthread_stdcall sSysServerThreadInit (
 void * data
) 
{
  MqSysServerThreadMain((struct MqSysServerThreadMainS*)data);
  return mqthread_ret_NULL;

}
#endif

static enum MqErrorE SysServerThread (
  struct MqS * const context,
  struct MqFactoryS* factory,
  struct MqBufferLS ** argvP,
  struct MqBufferLS ** alfaP,
  MQ_CST  name,		
  int state,		
  struct MqIdS * idP	
)
{
#if defined(MQ_HAS_THREAD)
  mqthread_t threadId;
# if defined(HAVE_PTHREAD)
  int ret;
# endif

  // fill thread data
  struct MqSysServerThreadMainS * argP = (struct MqSysServerThreadMainS *) MqSysMalloc(MQ_ERROR_PANIC,sizeof(*argP));
  argP->factory = factory;
  argP->argv = *argvP;
  argP->alfa = *alfaP;
  argP->tmpl = context;

  // pointers are owned by SysServerThread
  *argvP = NULL;
  *alfaP = NULL;

  // after a "thread" no "fork" is possible
  MqConfigSetIgnoreFork (context, MQ_YES);

# if defined(HAVE_PTHREAD)

  // thread attributes
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, state);
  
  // thread setup !before! a thread was created
  if (context->setup.fThreadInit) (*context->setup.fThreadInit)(context);

  // start thread
  do {
    ret = pthread_create(&threadId,&attr,sSysServerThreadInit,argP);
  } while (ret == EAGAIN);

  // cleanup attribute
  pthread_attr_destroy(&attr);

  if (unlikely(ret != 0)) {
    MqErrorDbV (MQ_ERROR_CAN_NOT_START_SERVER, name);
    goto error;
  }

# else

  if (unlikely ( (threadId = _beginthreadex(NULL, 0, sSysServerThreadInit, argP, 0, NULL)) == 0)) {
    MqErrorDbV (MQ_ERROR_CAN_NOT_START_SERVER, name);
    MqErrorSysAppend (_beginthreadex, errno);
    goto error;
  }

# endif

  // save tid
  (*idP).val = (MQ_IDNT)threadId;
  (*idP).type = MQ_ID_THREAD;

  return MQ_OK;

error:
  MqBufferLDelete (&argP->argv);
  MqBufferLDelete (&argP->alfa);
  MqSysFree (argP);
  return MQ_ERROR;
#else
  return MqErrorDb(MQ_ERROR_NOT_SUPPORTED);
#endif   /* MQ_HAS_THREAD */
}

static enum MqErrorE SysServerSpawn (
  struct MqS * const context,
  char *const *argv,
  MQ_CST  name,
  struct MqIdS * idP
)
{
  pid_t pid;
  MQ_CST cmd="SysServerSpawn";
  int err=0;

  // spawn setup !before! a spawn is created
  if (context->setup.fSpawnInit) (*context->setup.fSpawnInit)(context);

  // perl !need! sigschiled enabled at startup make it the default

#if defined(HAVE_POSIX_SPAWN)
  {
    posix_spawnattr_t sa;
    if ((err=posix_spawnattr_init(&sa)) != 0) {
      cmd="posix_spawnattr_init";
      goto error;
    }
    if (unlikely ((err=posix_spawnp(&pid, name, NULL, &sa, (char *const *) argv, __environ)) != 0)) {
      posix_spawnattr_destroy(&sa);
      cmd="posix_spawnp";
      goto error;
    }
    if ((err=posix_spawnattr_destroy(&sa)) != 0) {
      cmd="posix_spawnattr_destroy";
      goto error;
    }
  }
  goto ok;

//  ./process.h:int spawnl(int mode, const char *path, const char *argv0, ...);
//  ./process.h:int spawnle(int mode, const char *path, const char *argv0, ... /*, char * const *envp */);
//  ./process.h:int spawnlp(int mode, const char *path, const char *argv0, ...);
//  ./process.h:int spawnlpe(int mode, const char *path, const char *argv0, ... /*, char * const *envp */);
//  ./process.h:int spawnv(int mode, const char *path, const char * const *argv);
//  ./process.h:int spawnve(int mode, const char *path, const char * const *argv, const char * const *envp);
//  ./process.h:int spawnvp(int mode, const char *path, const char * const *argv);
//  ./process.h:int spawnvpe(int mode, const char *path, const char * const *argv, const char * const *envp);


#elif (defined(HAVE_FORK) || defined(HAVE_VFORK))  && defined(HAVE_EXECVP)

//#if (defined(HAVE_FORK) || defined(HAVE_VFORK))  && defined(HAVE_EXECVP)

  // fork to create the child
#if defined(HAVE_VFORK)
  if (unlikely ((pid = vfork()) == -1)) {
    cmd="vfork"
    err=errno;
    goto error;
  }
#elif defined(HAVE_FORK)
  if (unlikely ((pid = MqSysFork()) == -1)) {
    cmd="MqSysFork";
    err=errno;
    goto error;
  }
#else
  goto error;
#endif
      
  if (pid == 0) {
    // this is the child
    if (execvp(name, argv) == -1) {
      _exit (EXIT_FAILURE);
    }
    // the child will never reach this
  }
  goto ok;

#else
  {
    char buf[2048];
    char *nbuf=buf;

    // build command-line, windows _spawnvp is NOT able to handle !! whitespace !!
    // in path name argument
    for (;*argv != '\0'; argv++) {
      nbuf += sprintf(nbuf, "\"%s\" ", *argv);
    }

    // start process
    // java on windows test-cases using tcltest seems not be able to spawn new processes
    // unknown reason. command line works: 
    // >NhiExec example.Server.java --tcp --port 7777 --spawn
    errno=0;
    if (unlikely ((pid = _spawnlp (_P_NOWAIT, name, buf, NULL)) == -1)) {
    //if (unlikely ((pid = _spawnvp (_P_NOWAIT, name, (const char *const *) argv)) == -1)) {
      cmd="_spawnvp";
      err = errno;
      goto error;
    }
    goto ok;
  }
#endif

ok:
  (*idP).val = (mqpid_t)pid;
  (*idP).type = MQ_ID_PROCESS;
  return MQ_OK;

error:
  MqErrorDbV (MQ_ERROR_CAN_NOT_START_SERVER, name);
  if (err != 0) {
    MqErrorSAppendV(context, "can not '%s' -> [%i] ERR<%s>", cmd, err, strerror (err));
  }
  return MqErrorGetCodeI(context);
}

static enum MqErrorE SysUSleep (
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

static enum MqErrorE SysSleep (
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

MQ_STR MqSysBasename (
  MQ_CST const in,
  MQ_BOL includeExtension
)
{
#if defined(MQ_IS_WIN32)
  char drive[_MAX_DRIVE];
  char dir[_MAX_DIR];
  char * fname = (char *) MqSysMalloc(MQ_ERROR_PANIC, _MAX_FNAME);
  char ext[_MAX_EXT];
  _splitpath(in, drive, dir, fname, ext);
  if (includeExtension)
    return strcat(fname,ext);
  else
    return fname;
#else
  if (in == NULL) return MqSysStrDup(MQ_ERROR_PANIC, "");
  // double MqSysStrDup is necessary because LINUX may modify the input string
  // and FREEBSD modify the output string in future call's to 'basename'
  // ... Is This POSIX ?
  MQ_STR t1 = MqSysStrDup(MQ_ERROR_PANIC, (MQ_STR) in);
  const MQ_STR orig = MqSysStrDup(MQ_ERROR_PANIC, basename(t1));
  MqSysFree (t1);
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
#endif
}

enum MqErrorE SysFork (
  struct MqS * const context,
  struct MqIdS * idP
)
{
#ifdef HAVE_FORK
  if (unlikely (((*idP).val = (mqpid_t) MqSysFork()) == -1)) {
    return MqErrorSys (fork);
  }
  (*idP).type = MQ_ID_PROCESS;
  return MQ_OK;
#else
  return MqErrorDb(MQ_ERROR_NOT_SUPPORTED);
#endif
}

enum MqErrorE SysIgnorSIGCHLD (
  struct MqS * const context
)
{
#ifdef MQ_IS_POSIX
  if (unlikely ((signal(SIGCHLD, SIG_IGN)) == SIG_ERR)) {
    return MqErrorSys (signal);
  }
#endif
  return MQ_OK;
}

enum MqErrorE SysAllowSIGCHLD (
  struct MqS * const context
)
{
#ifdef MQ_IS_POSIX
  if (unlikely ((signal(SIGCHLD, SIG_DFL)) == SIG_ERR)) {
    return MqErrorSys (signal);
  }
#endif
  return MQ_OK;
}

static enum MqErrorE SysDaemonize (
  struct MqS * const context,
  MQ_CST pidfile
) 
{
#if defined(MQ_IS_POSIX) && !defined(__CYGWIN__)
  int fd, fpid;
  struct MqIdS id;

  // step 0 -> open the pidfile
  if ((fpid = open(pidfile, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) == -1) 
    return MqErrorSys (open);

  // step 1 -> delete initial process
  MqErrorCheck(SysFork(context, &id));
  if (id.val != 0UL)
    _exit(0);

  // step 2 -> create a new session
  if (setsid() == -1)
    return MqErrorSys (setsid);

  // step 3 -> fork the new process, parent write the pid
  MqErrorCheck(SysFork(context, &id));
  if (id.val != 0UL) {
    MQ_BUF buf = context->temp;
    MqBufferSetV(buf,"%lu",id.val);
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
#else
  return MqErrorDb(MQ_ERROR_NOT_SUPPORTED);
#endif
}

/// \brief thread save exit
__attribute__((noreturn)) static void SysExit (
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
__attribute__((noreturn)) static void SysAbort (void) {
  abort();
}

void MqSysFreeP(MQ_PTR data) {
  MqSysFree(data);
}

/*****************************************************************************/
/*                                                                           */
/*                           sqlite wrapper                                  */
/*                                                                           */
/*****************************************************************************/

static MQ_PTR sys_calloc (
  MQ_SIZE  nelem, 
  MQ_SIZE  elsize
) {
  register MQ_PTR ptr;  

  if (nelem == 0 || elsize == 0)
    nelem = elsize = 1;
  
  ptr = (*MqLal.SysMalloc) (nelem * elsize);
  if (ptr) memset(ptr,'\0', nelem * elsize);
  
  return ptr;
}

static MQ_STR sys_strdup (
  MQ_CST s
) {
  MQ_STR result = (MQ_STR) (*MqLal.SysMalloc) ((MQ_SIZE)strlen(s) + 1);
  if (result == (MQ_STR)0)
    return (MQ_STR)0;
  strcpy(result, s);
  return result;
}

static MQ_STR sys_strndup (
  MQ_CST s,
  MQ_SIZE l
) {
  const MQ_SIZE len = l+1;
  MQ_STR result = (MQ_STR) (*MqLal.SysMalloc) (len);
  if (result == (MQ_STR)0)
    return (MQ_STR)0;
  strncpy(result, s, len);
  return result;
}

/*****************************************************************************/
/*                                                                           */
/*                                MQ-LAL                                     */
/*                    (Language Abstraction Layer)                           */
/*                                                                           */
/*****************************************************************************/

void SysCreate(void) {
  check_sqlite (sqlite3_initialize()) {
    MqPanicC(MQ_ERROR_PANIC, __func__, -1, "unable to use 'sqlite3_initialize'");
  }   

  MqLal.SysCalloc	= (MqSysCallocF)  sys_calloc;
  MqLal.SysMalloc	= (MqSysMallocF)  sqlite3_malloc;
  MqLal.SysStrDup	= (MqSysStrDupF)  sys_strdup;
  MqLal.SysStrNDup	= (MqSysStrNDupF) sys_strndup;
  MqLal.SysRealloc	= (MqSysReallocF) sqlite3_realloc;
  MqLal.SysFree		= (MqSysFreeF)	  sqlite3_free;
#if defined(HAVE_FORK)
  MqLal.SysFork		= (MqSysForkF)	  fork;
#endif
  MqLal.SysAbort	= SysAbort;

  MqLal.SysServerSpawn = SysServerSpawn;
  MqLal.SysServerThread = SysServerThread;
  MqLal.SysServerFork = SysServerFork;
  MqLal.SysWait = SysWait;
  MqLal.SysUSleep = SysUSleep;
  MqLal.SysSleep = SysSleep;
  MqLal.SysDaemonize = SysDaemonize;
  MqLal.SysExit = SysExit;

  MqLal.SysIgnorSIGCHLD = SysIgnorSIGCHLD;
  MqLal.SysAllowSIGCHLD = SysAllowSIGCHLD;
};

void SysDelete(void) {
  check_sqlite (sqlite3_shutdown()) {
    MqPanicC(MQ_ERROR_PANIC, __func__, -1, "unable to use 'sqlite3_shutdown'");
  }   
}

END_C_DECLS
