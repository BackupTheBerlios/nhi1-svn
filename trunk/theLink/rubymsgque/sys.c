
#include "msgque_ruby.h"

#include <errno.h>
#include <ruby/defines.h>

//static ID id_join;

// *************************************************************************

static rb_pid_t SysFork (
)
{
  rb_pid_t pid = rb_fork(0, 0, 0, Qnil);
  if (pid == 0) rb_thread_atfork();
  return pid;
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

/*
static VALUE
sSysServerThreadInit(MQ_PTR data)
{
  MqSysServerThreadMain((struct MqSysServerThreadMainS*)data);
  //return rb_last_status_get();
  return Qnil;
}

static VALUE SysServerThread2 (VALUE data) {
  VALUE ret = rb_thread_create(sSysServerThreadInit, VAL2PTR(data));
  // make thread_id persistent
  INCR_REG(ret);
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
  NIL_Check(ret = rb_protect(SysServerThread2, PTR2VAL(argP), NULL));

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
*/

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

__attribute__ ((noreturn)) static void SysExit (
  int isThread,
  int num
) {
  if (isThread) {
    rb_protect(SysExit2,Qnil,NULL);
  } else {
    rb_protect(SysExit3,num,NULL);
    ruby_finalize();
  }
  exit(num);
}

/*
static VALUE SysWait2 (
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
  switch (idP->type) {
    case MQ_ID_PROCESS: {
      break;
    }
    case MQ_ID_THREAD: {
      rb_protect(SysWait2, PTR2VAL((MQ_PTR)idP->val), NULL);
      break;
    }
    case MQ_ID_UNUSED: {
      break;
    }
  }
  return MQ_OK;
}
*/

void NS(MqS_Sys_Init)(void) {

  //id_join = rb_intern("join");

  // update LAL
  MqLal.SysCalloc   = (MqSysCallocF)  xcalloc	  ;
  MqLal.SysMalloc   = (MqSysMallocF)  xmalloc	  ;
  MqLal.SysRealloc  = (MqSysReallocF) xrealloc	  ;
  MqLal.SysFree	    = (MqSysFreeF)    xfree	  ;
  MqLal.SysFork	    = (MqSysForkF)    SysFork	  ;

  //MqLal.SysServerThread = SysServerThread;
  MqLal.SysExit	    = SysExit;
  //MqLal.SysWait = SysWait;

  MqLal.SysSelect   = (MqSysSelectF) rb_thread_select;
}

