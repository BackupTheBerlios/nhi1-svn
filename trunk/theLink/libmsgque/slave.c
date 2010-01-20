/**
 *  \file       theLink/libmsgque/slave.c
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "main.h"
#include "slave.h"
#include "error.h"
#include "bufferL.h"
#include "link.h"
#include "config.h"

#define MQ_CONTEXT_S context

BEGIN_C_DECLS

struct MqLinkSlaveS {
  struct MqS * context;	    ///< link to the msgque object

  // slaves link
  struct MqS ** slaves;	    ///< SLAVE: \c NULL, MASTER a list of pointers of \e slave object 
  MQ_SIZE used;		    ///< number of slave objects in slaves[]
  MQ_SIZE size;		    ///< size of slaves[]
};

/*****************************************************************************/
/*                                                                           */
/*                                C R E A T E                                */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE
pSlaveBqError (
  struct MqS * const context,
  MQ_PTR data
)
{
  enum MqErrorE ret = MqSendERROR (context->config.master);
  MqErrorReset (context);
  return ret;
}

static enum MqErrorE
pSlaveChildCreate (
  struct MqLinkSlaveS * const   slave,
  MQ_SIZE const		    id,
  struct MqS * const slave_context
)
{
  struct MqS * const context = slave->context;

  // ERROR, ID is no valid identifer
  if (id < 0 || id >= MQ_SLAVE_MAX)
    return MqErrorDbV(MQ_ERROR_ID_OUT_OF_RANGE, "slave", 0, id, MQ_SLAVE_MAX-1);

  // ERROR, if ID is already used
  if (slave->slaves && id >= 0 && id < slave->used && slave->slaves[id] != NULL)
    return MqErrorDbV(MQ_ERROR_ID_IN_USE, "slave", id);

  // get the new max
  if (id >= slave->used) {
    slave->used = (id+1);
  }

  // everything is OK, 1. check if slave array is available and has enough space
  if (slave->slaves == NULL) {
    slave->size = 10;
    slave->slaves = (struct MqS **) MqSysCalloc(MQ_ERROR_PANIC, 10, sizeof(struct MqS*));
  }

  // array to small ?
  if (slave->size < slave->used) {
    MQ_SIZE oldsize = slave->size;
    slave->size = (2*slave->used > MQ_SLAVE_MAX ? MQ_SLAVE_MAX : 2*slave->used);
    slave->slaves = (struct MqS **) MqSysRealloc(MQ_ERROR_PANIC, slave->slaves, slave->size * sizeof(struct MqS*));
    memset(slave->slaves+oldsize, 0, slave->size-oldsize);
  }

  slave->slaves[id] = slave_context;
  pConfigSetMaster (slave_context, context, id);
  // set background error handler
  if (slave_context->setup.BgError.fFunc == NULL)
    MqConfigSetBgError (slave_context, pSlaveBqError, NULL, NULL, NULL);
  return MQ_OK;
}

static enum MqErrorE
pSlaveParentCreate (
  struct MqLinkSlaveS * const   slave,
  MQ_SIZE const		    id,
  struct MqS * const	    slave_context
)
{
  struct MqS * const  context = slave->context;

  // ERROR, if MASTER-MSGQUE is a CHILD or if it is a PARENT with CHILDs available
  if (MQ_IS_CHILD(context) || context->link.childs != NULL)
    return MqErrorDbV(MQ_ERROR_MASTER_SLAVE, "master");

  // ERROR, if SLAVE-MSGQUE is a SERVER or is a CHILD or if it is a PARENT with CHILDs available
  if (MQ_IS_SERVER(slave_context) || MQ_IS_CHILD(slave_context) || slave_context->link.childs != NULL)
    return MqErrorDbV(MQ_ERROR_MASTER_SLAVE_CLIENT, "slave");

  return pSlaveChildCreate(slave, id, slave_context);
}

static void
pSlaveItemDelete (
  struct MqLinkSlaveS const * const slave,
  MQ_SIZE id
)
{
  struct MqS * slave_context;
  if (slave->slaves == NULL || id < 0 || id >= slave->used || slave->slaves[id] == NULL) return;
  // attention !! SLAVE set "slave->slaves[id]" use a teporary variable as work-around
  slave_context = slave->slaves[id];
  MqDLogV(slave->context, 5, "delete SLAVE <%p>\n", (void*) slave_context);
  MqLinkDelete (slave_context);
  slave->slaves[id] = NULL;
}

static void
pSlaveItemShutdown (
  struct MqLinkSlaveS const * const slave,
  MQ_SIZE id
)
{
  struct MqS * slave_context;
  if (slave->slaves == NULL || id < 0 || id >= slave->used || slave->slaves[id] == NULL) return;
  // attention !! SLAVE set "slave->slaves[id]" use a teporary variable as work-around
  slave_context = slave->slaves[id];
  MqDLogV(slave->context, 5, "shutdown SLAVE <%p>\n", (void*) slave_context);
  pMqShutdown (slave_context);
}

enum MqErrorE
pSlaveCreate (
  struct MqS * const context,
  struct MqLinkSlaveS ** const out
)
{
  struct MqLinkSlaveS * const slave = (struct MqLinkSlaveS *) MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof (*slave));
  if (unlikely(slave == NULL)) goto error;
  if (likely (out != NULL)) *out = slave;
  slave->context = context;

  // if child ... create all slave object-childs of parent too
  if (MQ_IS_CHILD(context)) {
    struct MqS * const master = context;
    struct MqS const * const parent = context->config.parent;
    struct MqLinkSlaveS * parent_slave = parent->link.slave;
    MQ_SIZE id;

    if (parent_slave->used != 0)
      MqDLogC (context,3,"START-CREATE-SLAVE-CHILD\n");

    // I'm a CHILD of a MASTER -> add a CHILD to all of my SLAVES too
    for (id=0; id<parent_slave->used; id++) {
      struct MqS * newctx;
      struct MqS * slave_parent = parent_slave->slaves[id];
      // check id PARENT has a SLAVE with ID
      if (slave_parent == NULL) continue;
      // create the CHILD of a SLAVE
      // a SLAVE is allways a CLIENT
      if (MqErrorCheckI(pCallFactory(slave_parent, MQ_FACTORY_NEW_CHILD, slave_parent->setup.Factory, &newctx))) {
	MqErrorCopy (context, slave_parent);
	goto error;
      }
      pConfigSetParent(newctx, slave_parent);
      pConfigSetMaster(newctx, master, id);
      if (master->config.srvname)
	MqConfigSetSrvName(newctx, master->config.srvname);
      // create the child
      if (MqLinkCreate (newctx, NULL) == MQ_ERROR) {
	MqErrorCopy(context, newctx);
	MqContextDelete(&newctx);
	goto error;
      }
      // add slave_context as slave
      if (pSlaveChildCreate(slave, id, newctx) == MQ_ERROR) {
	MqContextDelete(&newctx);
	goto error;
      }
    }

  } else {    // SlaveWorker - PARENT

    // add my link to the master
    if (context->config.master != NULL) {
      struct MqS * const master = context->config.master;

      // set to NULL to avoid "double" use
      pConfigSetMaster (context, NULL, context->config.master_id);

      MqDLogC (context,3,"START-CREATE-SLAVE-WORKER\n");

      // ERROR, if MASTER is not a SERVER
      if (MQ_IS_CLIENT(master)) {
	return MqErrorDbV(MQ_ERROR_CONFIGURATION_REQUIRED, "slave-worker", "MqConfigS::isServer = MQ_YES");
      }

      // ERROR, if MASTER is NOT a PARENT or is a PARENT and CHILDS are available
      if (MQ_IS_CHILD(master) || master->link.childs != NULL) {
	return MqErrorDbV(MQ_ERROR_MASTER_SLAVE, "master");
      }

      // create a LINK between MYSELF and the MASTER
      if (MqErrorCheckI (pSlaveChildCreate(master->link.slave, context->config.master_id, context))) {
	MqErrorCopy (context, master);
	goto error;
      }
    }
  }

error:
  return MqErrorStack (context);
}

void
pSlaveShutdown (
  struct MqLinkSlaveS * const slave
)
{
  MQ_SIZE id;
  if (unlikely(slave == NULL)) return;
  for (id=0; id<slave->used; id++) {
    pSlaveItemShutdown(slave, id);
  }
}

void
pSlaveDelete (
  struct MqLinkSlaveS ** const slaveP
)
{
  struct MqLinkSlaveS * const slave = *slaveP;
  struct MqS * context;
  MQ_SIZE id;

  if (unlikely(slave == NULL)) return;
  context = slave->context;

  // delete my link(s) to the slaves
  for (id=0; id<slave->used; id++) {
    pSlaveItemDelete(slave, id);
  }
  MqSysFree(slave->slaves);

  // delete my link to the master
  if (context->config.master != NULL) {
    if (context->config.master_id < context->config.master->link.slave->used) 
      context->config.master->link.slave->slaves[context->config.master_id] = NULL;
    pConfigSetMaster (context, NULL, 0);
  }

  // delete the slave object
  MqSysFree(*slaveP);
}

/*****************************************************************************/
/*                                                                           */
/*                                P U B L I C                                */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
MqSlaveWorker (
  struct MqS * const  context,
  MQ_SIZE const	      id,
  struct MqBufferLS ** argvP
)
{
  if (unlikely(context->link.bits.onCreate == MQ_NO)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "master", "not");
  } else {
    struct MqS * newctx;
    struct MqBufferLS * argv = (argvP == NULL ? NULL : *argvP);
    // argv is noe "owend" by "MqSlaveWorker"
    if (argvP != NULL) *argvP = NULL;
    MqErrorCheck (pCallFactory (context, MQ_FACTORY_NEW_SLAVE, context->setup.Factory, &newctx));
    pConfigSetMaster(newctx, context, id);
    if (argv == NULL) {
      argv = MqBufferLCreateArgsV(context, "worker-client", MQ_ALFA_STR, "WORKER", NULL);
    } else {
      MQ_SIZE idx = MqBufferLSearchC(argv, MQ_ALFA_STR, 1, 0);
      // insert "worker-client" on position "0"
      MqBufferLAppend(argv, MqBufferCreateC (MQ_ERROR_PANIC, "worker-client"), 0);
      // insert "WORKER" just after "@"
      if (idx == -1) {
	MqBufferLAppendC(argv, MQ_ALFA_STR);
	MqBufferLAppendC(argv, "WORKER");
      } else {
	MqBufferLAppend(argv, MqBufferCreateC (MQ_ERROR_PANIC, "WORKER"), idx+2);
      }
    }
    MqErrorCheck1 (MqLinkCreate(newctx,&argv));
    MqErrorCheck1 (MqCheckForLeftOverArguments(newctx,&argv));
    return MQ_OK;
error1:
    MqErrorCopy (context, newctx);
    MqContextDelete(&newctx);
error:
    MqBufferLDelete(&argv);
    return MqErrorStack(context);
  }
}

enum MqErrorE
MqSlaveCreate (
  struct MqS * const  context,
  MQ_SIZE const	      id,
  struct MqS * const  slave_context
)
{
  // the slave object now belongs to 'context'.
  // using the following line:
  slave_context->link.bits.doFactoryCleanup = MQ_YES;
  // the initial object will be delete by libmsgque even if it
  // was created outsite
  return pSlaveParentCreate(context->link.slave, id, slave_context);
}

enum MqErrorE
MqSlaveDelete (
  struct MqS * const  context,
  MQ_SIZE const id
)
{
  // ERROR, if MASTER-MSGQUE is a CHILD
  if (MQ_IS_CHILD(context))
    return MqErrorDb(MQ_ERROR_SLAVE_CHILD_DELETE);
  pSlaveItemDelete(context->link.slave, id);
  MqErrorReset(context);
  return MQ_OK;
}

struct MqS *
MqSlaveGet (
  struct MqS const * const  context,
  MQ_SIZE const id
)
{
  return (id < 0 || id >= context->link.slave->used ? NULL : context->link.slave->slaves[id]);
}

struct MqS *
MqSlaveGetMaster (
  struct MqS const * const context
)
{
  return MqSlaveGetMasterI(context);
}

int
MqSlaveIs (
  struct MqS const * const context
)
{
  return MqSlaveIsI(context);
}

END_C_DECLS



