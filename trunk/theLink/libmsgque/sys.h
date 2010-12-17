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
# define SYS_H

BEGIN_C_DECLS

enum MqErrorE SysFork (struct MqS *const, struct MqIdS *);
enum MqErrorE SysIgnorSIGCHLD (struct MqS * const);
enum MqErrorE SysAllowSIGCHLD (struct MqS * const);
enum MqErrorE SysUnlink (struct MqS * const, const MQ_STR);

#define MqSysFork()   (*MqLal.SysFork)()
#define MqSysAbort()  (*MqLal.SysAbort)()
#define MqSysServerSpawn(context, argv, name, idP) \
  (*MqLal.SysServerSpawn)(context, argv, name, idP)
#define MqSysServerThread(context, factory, argvP, alfaP, name, state, idP) \
  (*MqLal.SysServerThread)(context, factory, argvP, alfaP, name, state, idP)
#define MqSysServerFork(context, factory, argvP, alfaP, name, idP) \
  (*MqLal.SysServerFork)(context, factory, argvP, alfaP, name, idP)
#define MqSysDaemonize(context,pidfile) (*MqLal.SysDaemonize)(context,pidfile)

END_C_DECLS

#endif /* SYS_H */

