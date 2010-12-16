/**
 *  \file       theLink/acmds/asplit.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

/** \ingroup tools
 *  \defgroup asplit asplit
 *  \{
 *  \brief asplit \asplit_desc
 *
 *  \verbinclude asplit.help
 */

/// use the GNU redline extension
#define _GNU_SOURCE
/// solaris needs this for f(un)lockfile
#define __EXTENSIONS__

#include "mqconfig.h"
#include "msgque.h"
#include "debug.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*****************************************************************************/
/*                                                                           */
/*                               getline function                            */
/*                                                                           */
/*****************************************************************************/

#if !HAVE_GETLINE
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <errno.h>
#include <stdio.h>

#if defined(_MSC_VER)
# define size_t unsigned int
# define ssize_t unsigned int
#endif

#ifndef SIZE_MAX
# define SIZE_MAX ((size_t) -1)
#endif
#ifndef SSIZE_MAX
# define SSIZE_MAX ((ssize_t) (SIZE_MAX / 2))
#endif
#if !HAVE_FLOCKFILE
# undef flockfile
# define flockfile(x) ((void) 0)
#endif
#if !HAVE_FUNLOCKFILE
# undef funlockfile
# define funlockfile(x) ((void) 0)
#endif

/* Read up to (and including) a DELIMITER from FP into *LINEPTR (and
   NUL-terminate it).  *LINEPTR is a pointer returned from malloc (or
   NULL), pointing to *N characters of space.  It is realloc'ed as
   necessary.  Returns the number of characters read (not including
   the null terminator), or -1 on error or EOF.  */

static ssize_t
getline (char **lineptr, size_t *n, FILE *fp)
{
  int delimiter = '\n';
  ssize_t result = -1;
  size_t cur_len = 0;

  if (lineptr == NULL || n == NULL || fp == NULL)
    {
      errno = EINVAL;
      return -1;
    }

  flockfile (fp);

  if (*lineptr == NULL || *n == 0)
    {
      *n = 120;
      *lineptr = (char *) malloc (*n);
      if (*lineptr == NULL)
    {
      result = -1;
      goto unlock_return;
    }
    }

  for (;;)
    {
      int i;

      i = getc (fp);
      if (i == EOF)
    {
      result = -1;
      break;
    }

      /* Make enough space for len+1 (for final NUL) bytes.  */
      if (cur_len + 1 >= *n)
    {
      size_t needed_max =
        SSIZE_MAX < SIZE_MAX ? (size_t) SSIZE_MAX + 1 : SIZE_MAX;
      size_t needed = 2 * *n + 1;   /* Be generous. */
      char *new_lineptr;

      if (needed_max < needed)
        needed = needed_max;
      if (cur_len + 1 >= needed)
        {
          result = -1;
          goto unlock_return;
        }

      new_lineptr = (char *) realloc (*lineptr, needed);
      if (new_lineptr == NULL)
        {
          result = -1;
          goto unlock_return;
        }

      *lineptr = new_lineptr;
      *n = needed;
    }

      (*lineptr)[cur_len] = i;
      cur_len++;

      if (i == delimiter)
    break;
    }
  (*lineptr)[cur_len] = '\0';
  result = cur_len ? cur_len : result;

 unlock_return:
  funlockfile (fp);
  return result;
}
#endif
#endif /* HAVE_GETLINE */

/*****************************************************************************/
/*                                                                           */
/*                                     main                                  */
/*                                                                           */
/*****************************************************************************/

#define MSGQUE msgque

static void SplitHelp ( const char * ) __attribute__ ((noreturn));

/// \brief display help using \b -h or \b --help command-line option
/// \param base the executable usually: <tt>basename(argv[0])</tt>
static void
SplitHelp ( const char * base  )
{
    fprintf(stderr, "usage: %s [OPTION]... [ARGUMENT]... [FILES]...\n", base);
    fputs("\n", stderr);
    fprintf(stderr, " The %s tool expect input data from stdin or files as argument and send\n", base);
    fputs(" the output data as package to a msgque server. For every line from the input an\n", stderr);
    fputs(" output package is created by splitting the input line using the the delimiter -d.\n", stderr);
    fputs("\n", stderr);
    fprintf(stderr, "  %s [ARGUMENT]... syntax:\n", base);
    fprintf(stderr, "    ... | %s [OPTION]... %c aserver [OPTION]... [ARGUMENT]\n", base, MQ_ALFA);
    fputs("\n", stderr);
    fputs(MqHelp (NULL), stderr);
    fputs("\n", stderr);
    fprintf(stderr,"  %s [OPTION]:\n", base);
    fputs("    -d CHAR          the delimiter character                  (default: ' ')\n", stderr);
    fputs("    -h, --help       print this help\n", stderr);
    fputs("\n", stderr);

    exit(EXIT_SUCCESS);
}

/// \brief split the input-line(s) from \e fp into a msgque package using delimiter \e delimiter
/// \msgque
/// \param delimiter split the input line into fields using \e delimiter
/// \param fp using input-stream fp (by default stdout)
/// \retMqErrorE
static enum MqErrorE
split (
  struct MqS * const mqctx,
  char delimiter,
  FILE *fp
)
{
  size_t len = 0;
  ssize_t read;
  char *line=NULL, *cur, *cep;

  while ((read = getline(&line, &len, fp)) != -1) {
    MqErrorCheck(MqSendSTART (mqctx));
    for (cur=line; *cur != '\0'; cur=cep+1) {
      for (cep=cur; *cep != delimiter && *cep != '\n'; cep++);
      *cep = '\0';
      MqSendC (mqctx, cur);
    }
    MqErrorCheck (MqSendEND_AND_WAIT (mqctx, "+FTR", MQ_TIMEOUT_USER));
  }

error:
  MqSysFree (line);
  return MqErrorStack (mqctx);
} 

/// \tool_Create
static enum MqErrorE
SplitCreate (
  struct MqS  * ctx,
  struct MqBufferLS ** argvP
)
{
  MQ_BUF buf = NULL;
  MQ_STR delimiter = mq_strdup("\t");	// the delimiter-char to split from

  // create Msgque
  MqErrorCheck (MqLinkCreate (ctx, argvP));

  // get the delimiter character
  MqBufferLCheckOptionC (ctx, *argvP, "-d", &delimiter);

  // do the splitting
  if (argvP != NULL && *argvP != NULL && (*argvP)->cursize != 0) {
    struct MqBufferLS *argv = *argvP;
    MQ_CST fname;
    FILE *f;
    while (argv->cursize != 0) {
      MqErrorCheck (MqBufferLGetU (ctx, argv,0, &buf));
      MqErrorCheck (MqBufferGetC (buf, &fname));
      if ((f = fopen (fname, "r")) == NULL) 
	MqErrorCheck (MqErrorC (ctx, __func__, errno, strerror(errno)));
      MqErrorCheck(split(ctx, delimiter[0], f));
      if (fclose (f) == EOF) 
	MqErrorCheck (MqErrorC (ctx, __func__, errno, strerror(errno)));
      MqBufferDelete(&buf);
    }
    MqBufferLDelete(argvP);
  } else {
    MqErrorCheck(split(ctx, delimiter[0], stdin));
  }

  // finally set the End-Of-File
  MqSendSTART(ctx);
  MqErrorCheck (MqSendEND_AND_WAIT (ctx, "+EOF", MQ_TIMEOUT_USER));

  // cleanup
  MqSysFree(delimiter);

  return MQ_OK;

error:
  MqBufferLDelete (argvP);
  MqSysFree(delimiter);
  MqBufferDelete(&buf);
  MqBufferLDelete(argvP);
  MqErrorStack(ctx);
  return MqErrorSAppendC(ctx, "use '-h' or '--help' for usage");
}

enum MqErrorE
SplitFactory (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryItemS * const item,
  struct MqS **contextP
)
{
  struct MqS * const mqctx = MqContextCreate(0,tmpl);
  MqConfigSetName(mqctx, "split");
  mqctx->setup.Parent.fCreate = SplitCreate;
  mqctx->setup.fHelp = SplitHelp;
  *contextP = mqctx;
  return MQ_OK;
}

/** \} asplit */

