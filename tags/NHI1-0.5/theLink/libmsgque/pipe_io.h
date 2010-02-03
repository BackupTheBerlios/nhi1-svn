/**
 *  \file       theLink/libmsgque/pipe_io.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef PIPE_IO_H
#   define PIPE_IO_H

#   define PIPE_SAVE_ERROR(pipe) ((pipe)?IO_SAVE_ERROR((pipe)->io):NULL)

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                                  pipe_io                                   */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE PipeCreate (
  struct MqIoS * const io,
  struct PipeS ** const pipePtr
);

void PipeDelete (
  struct PipeS ** const pipeP
) __attribute__((nonnull));

enum MqErrorE PipeServer (
  struct PipeS * const pipe
);

enum MqErrorE PipeConnect (
  struct PipeS * const pipe
);

/*****************************************************************************/
/*                                                                           */
/*                                pipe_arg                                    */
/*                                                                           */
/*****************************************************************************/

MQ_PTR PipeGetOpt (
  struct PipeS const * const pipe,
  int const opt
);

MQ_SOCK* PipeGetServerSocket (
  struct PipeS * const pipe
);

END_C_DECLS

#endif /* PIPE_IO_H */



