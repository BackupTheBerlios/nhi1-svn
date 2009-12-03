/**
 *  \file       theLink/libmsgque/event.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef EVENT_H
#   define EVENT_H

BEGIN_C_DECLS

struct MqEventS;

void pEventAdd ( struct MqS * const , MQ_SOCK * const);

enum MqErrorE pEventCheck (struct MqS*const, struct MqEventS*, enum MqIoSelectE const, struct timeval*const);
enum MqErrorE pEventStart (struct MqS*const, struct MqEventS*, struct timeval const*const, EventCreateF const);
enum MqErrorE pEventCreate (struct MqS*const, struct MqEventS**const);
void pEventShutdown ( struct MqS const * const );
void pEventDelete ( struct MqS const * const );
struct MqS const * pEventSocketFind (struct MqS const * const, MQ_SOCK );

#if _DEBUG
void pEventLog( struct MqS const * const , struct MqEventS *, MQ_CST const);
#endif

#define DLogEvent(ev) printf("%s(%s:%d) -> pid<%i>, id<%li>, event<%p>, ref<%i>\n", __func__, __FILE__, __LINE__,\
      mq_getpid(), pthread_self(), ev, (ev!=NULL?ev->refCount:-1));fflush(stdout);

END_C_DECLS

#endif /* EVENT_H */

