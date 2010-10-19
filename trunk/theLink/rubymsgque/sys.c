
#include "msgque_ruby.h"

#include <errno.h>
#include <ruby/defines.h>

static ID id_join;

#define MqErrorSys(cmd) \
  MqErrorV (mqctx, __func__, errno, \
    "can not '%s' -> ERR<%s>", MQ_CPPXSTR(cmd), strerror (errno))

// *************************************************************************

static MQ_PTR SysCalloc (
  struct MqS * const mqctx,
  MQ_SIZE nmemb,
  MQ_SIZE size
)
{
  MQ_PTR ptr = xcalloc (nmemb, size);
  if (unlikely (ptr == NULL))
    MqErrorC (mqctx, __func__, errno, strerror (errno));
  return ptr;
}

static MQ_PTR SysMalloc (
  struct MqS * const mqctx,
  MQ_SIZE size
)
{
  MQ_PTR ptr = xmalloc (size);
  if (unlikely (ptr == NULL))
    MqErrorC (mqctx, __func__, errno, strerror (errno));
  return ptr;
}

static MQ_PTR SysRealloc (
  struct MqS * const mqctx,
  MQ_PTR buf,
  MQ_SIZE size
)
{
  MQ_PTR ptr = xrealloc (buf, size);
  if (unlikely (ptr == NULL))
    MqErrorC (mqctx, __func__, errno, strerror (errno));
  return ptr;
}

static void SysFreeP (
  MQ_PTR ptr
)
{
  if ( likely((ptr) != (NULL)) ) xfree(ptr);
}

static enum MqErrorE SysFork (
  struct MqS * const mqctx,
  struct MqIdS * idP
)
{
#ifdef HAVE_FORK
  rb_pid_t pid;
  switch (pid = rb_fork(0, 0, 0, Qnil)) {
    case 0:
      rb_thread_atfork();
      break;
    case -1:
      return MqErrorSys (fork);
  }
  (*idP).val = pid;
  (*idP).type = MQ_ID_PROCESS;
  return MQ_OK;
#else
  return MqErrorDb(MQ_ERROR_NOT_SUPPORTED);
#endif
}

/*
static enum MqErrorE SysServerSpawn (
  struct MqS * const mqctx,
  char * * argv,
  MQ_CST  name,
  struct MqIdS * idP
)
{ 
  rb_pid_t pid;
  VALUE args = rb_ary_new();
  MQ_BUF tmp = mqctx->temp;

  MqBufferReset(tmp);

  // spawn setup !before! a spawn is created
  if (mqctx->setup.fSpawnInit) (*mqctx->setup.fSpawnInit)(mqctx);

  // fill the args
  for (;*argv!=NULL; argv++) {
printC(*argv)
    rb_ary_push(args, CST2VAL(*argv));
  }

  // use ruby spawn
  pid = rb_spawn_err(RARRAY_LEN(args), RARRAY_PTR(args), tmp->cur.C, tmp->size);

  // check for error
  if (pid == -1) {
    return MqErrorC (mqctx, name, 1, tmp->cur.C);
  }

  // return ok
  (*idP).val = pid;
  (*idP).type = MQ_ID_PROCESS;
  return MQ_OK;
}
*/

static VALUE
sSysServerThreadInit(MQ_PTR data)
{
  MqSysServerThreadMain((struct MqSysServerThreadMainS*)data);
  return rb_last_status_get();
}

static VALUE SysServerThreadCall (VALUE data) {
  struct timeval sleep = {0, 200000};
  VALUE ret = rb_thread_create(sSysServerThreadInit, VAL2PTR(data));
  rb_thread_wait_for(sleep);
  return ret;
}

static enum MqErrorE SysServerThread (
  struct MqS * const mqctx,
  struct MqFactoryS factory,
  struct MqBufferLS ** argvP,
  struct MqBufferLS ** alfaP,
  MQ_CST  name,
  int state,
  struct MqIdS * idP
)
{
  VALUE ret;

  // fill thread data
  struct MqSysServerThreadMainS * argP = (struct MqSysServerThreadMainS *) MqSysMalloc(MQ_ERROR_PANIC,sizeof(*argP));
  argP->factory = factory;
  argP->argv = *argvP;
  argP->alfa = *alfaP;
  argP->tmpl = mqctx;

  // pointers are now owned by SysServerThread
  *argvP = NULL;
  *alfaP = NULL;

  // after a "thread" no "fork" is possible
  MqConfigSetIgnoreFork (mqctx, MQ_YES);

  // start thread
  NIL_Check(ret = NS(Rescue)(mqctx, SysServerThreadCall, PTR2VAL(argP)));

  // make thread_id persistent
  INCR_REG(ret);

  // save tid
  (*idP).val = ret;
  (*idP).type = MQ_ID_THREAD;

  return MQ_OK;

error:
  MqBufferLDelete (&argP->argv);
  MqBufferLDelete (&argP->alfa);
  MqSysFree (argP);
  return MQ_ERROR;
}

static VALUE SysExit2(VALUE dummy) {
  VALUE thread = rb_thread_current();
  DECR_REG(thread);
  rb_thread_kill(thread);
  return Qnil;
}

static VALUE SysExit3(VALUE num) {
  rb_exit((int)num);
  return Qnil;
}

static void SysExit (
  int isThread,                     ///< [in] is this a thread
  int num                           ///< [in] exit number
) {
  if (isThread) {
    rb_protect(SysExit2,Qnil,NULL);
  } else {
    rb_protect(SysExit3,num,NULL);
    ruby_finalize();
    exit(num);
  }
}

static VALUE SysWaitCall (
  VALUE thread
)
{
  return rb_funcall(thread,id_join,0,NULL);
}

static enum MqErrorE SysWait (
  struct MqS * const mqctx,
  const struct MqIdS *idP
)
{
  errno = 0;
  switch (idP->type) {
    case MQ_ID_PROCESS: {
      break;
    }
    case MQ_ID_THREAD: {
      VALUE thread = PTR2VAL((MQ_PTR)idP->val);
      NIL_Check(NS(Rescue)(mqctx, SysWaitCall, thread));
      INCR_REF(thread);
      break;
    }
    case MQ_ID_UNUSED: {
      break;
    }
  }
  return MQ_OK;

error:
  return MqErrorStack(mqctx);
}

void NS(MqS_Sys_Init)(void) {

  id_join = rb_intern("join");

  // update LAL
  MqLal.SysCalloc   = SysCalloc	  ;
  MqLal.SysMalloc   = SysMalloc	  ;
  MqLal.SysRealloc  = SysRealloc  ;
  MqLal.SysFreeP    = SysFreeP	  ;
  MqLal.SysFork	    = SysFork	  ;

  MqLal.SysServerThread = SysServerThread;
  MqLal.SysExit = SysExit;
  MqLal.SysWait = SysWait;
}


