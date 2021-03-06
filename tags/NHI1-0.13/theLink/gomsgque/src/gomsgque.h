/**
 *  \file       theLink/gomsgque/src/gomsgque.h
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include <stdlib.h>
#include "msgque.h"
#include "debug.h"

extern MQ_CST sGO;
extern MQ_CST sERROR;

//typedef struct { void *t; void *v; } GoInterface;

void gomsgque_Init ();
void gomsgque_ConfigSetServerSetup (struct MqS * const, void *data);
void gomsgque_ConfigSetServerCleanup (struct MqS * const, void *data);
void gomsgque_ConfigSetBgError (struct MqS * const, void *data);
void gomsgque_ConfigSetEvent (struct MqS * const, void *data);
void gomsgque_ConfigSetFactory (struct MqS * const, MQ_CST const, MQ_PTR);
void gomsgque_ConfigSetSetup (struct MqS * const);
enum MqErrorE gomsgque_ServiceCreate (struct MqS * const, MQ_TOK const token, MQ_PTR data);
enum MqErrorE gomsgque_ServiceCreate2 (struct MqS * const, MQ_TOK const token, MQ_PTR data);
enum MqErrorE gomsgque_SendEND_AND_CALLBACK (struct MqS * const, MQ_TOK const token, MQ_PTR data);
enum MqErrorE gomsgque_SendEND_AND_CALLBACK2 (struct MqS * const, MQ_TOK const token, MQ_PTR data);
void gomsgque_SysServerThreadMain(struct MqSysServerThreadMainS *data, MQ_PTR chn);

struct MqFactoryS * gomsgque_FactoryAdd (MQ_CST const ident, MQ_PTR data);
struct MqFactoryS * gomsgque_FactoryDefault (MQ_CST const ident, MQ_PTR data);
struct MqS * gomsgque_FactoryNew (struct MqFactoryS * const item);

