/**
 *  \file       theLink/libmsgque/debug.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef MQ_DEBUG_H
#define MQ_DEBUG_H

#if defined(_MSC_VER)
// need for "getpid"
#  include <process.h>
#endif

#if !defined(mq_getpid)
# define mq_getpid() 1
#endif

#if defined(MQ_HAS_THREAD)
# include <pthread.h>
#endif

/*****************************************************************************/
/*                                                                           */
/*                              msgque/types                                 */
/*                                                                           */
/*****************************************************************************/

#if defined(MQ_HAS_THREAD)
# define MX(s) fprintf(stderr, "%s(%s:%d:%d:%p) -> %s \n", __func__, __FILE__, __LINE__, mq_getpid(), \
	      (void*) pthread_self(), #s);fflush(stderr);
#else
# define MX(s) fprintf(stderr, "%s(%s:%d:%d) -> %s \n", __func__, __FILE__, __LINE__, mq_getpid(), #s);fflush(stderr);
#endif

#define M0 MX(00000000000000000)
#define M1 MX(11111111111111111)
#define M2 MX(22222222222222222)
#define M3 MX(33333333333333333)
#define M4 MX(44444444444444444)
#define M5 MX(55555555555555555)
#define M6 MX(66666666666666666)
#define M7 MX(77777777777777777)
#define M8 MX(88888888888888888)
#define M9 MX(99999999999999999)
#define MA MX(AAAAAAAAAAAAAAAAA)
#define MB MX(BBBBBBBBBBBBBBBBB)
#define MC MX(CCCCCCCCCCCCCCCCC)
#define MD MX(DDDDDDDDDDDDDDDDD)
#define ME MX(EEEEEEEEEEEEEEEEE)
#define MF MX(FFFFFFFFFFFFFFFFF)

#define MXI(s,i) fprintf(stderr, "%s(%s:%d) -> %s - %s\n", __func__, __FILE__, __LINE__, #s, i);fflush(stderr);

#define M0I(i) MXI(00000000000000000,i)
#define M1I(i) MXI(11111111111111111,i)
#define M2I(i) MXI(22222222222222222,i)
#define M3I(i) MXI(33333333333333333,i)
#define M4I(i) MXI(44444444444444444,i)
#define M5I(i) MXI(55555555555555555,i)
#define M6I(i) MXI(66666666666666666,i)
#define M7I(i) MXI(77777777777777777,i)
#define M8I(i) MXI(88888888888888888,i)
#define M9I(i) MXI(99999999999999999,i)
#define MAI(i) MXI(AAAAAAAAAAAAAAAAA,i)
#define MBI(i) MXI(BBBBBBBBBBBBBBBBB,i)
#define MCI(i) MXI(CCCCCCCCCCCCCCCCC,i)
#define MDI(i) MXI(DDDDDDDDDDDDDDDDD,i)
#define MEI(i) MXI(EEEEEEEEEEEEEEEEE,i)
#define MFI(i) MXI(FFFFFFFFFFFFFFFFF,i)

#define MQ_IX(x,s) MqLogV(x,__func__,0, #x "<" #s "> -> (%s:%d)\n", __FILE__, __LINE__);

#define MQ_I0 MQ_IX(MQ_CONTEXT_S,0000000000000000)
#define MQ_I1 MQ_IX(MQ_CONTEXT_S,1111111111111111)
#define MQ_I2 MQ_IX(MQ_CONTEXT_S,2222222222222222)
#define MQ_I3 MQ_IX(MQ_CONTEXT_S,3333333333333333)
#define MQ_I4 MQ_IX(MQ_CONTEXT_S,4444444444444444)
#define MQ_I5 MQ_IX(MQ_CONTEXT_S,5555555555555555)
#define MQ_I6 MQ_IX(MQ_CONTEXT_S,6666666666666666)
#define MQ_I7 MQ_IX(MQ_CONTEXT_S,7777777777777777)
#define MQ_I8 MQ_IX(MQ_CONTEXT_S,8888888888888888)
#define MQ_I9 MQ_IX(MQ_CONTEXT_S,9999999999999999)
#define MQ_IA MQ_IX(MQ_CONTEXT_S,aaaaaaaaaaaaaaaa)
#define MQ_IB MQ_IX(MQ_CONTEXT_S,bbbbbbbbbbbbbbbb)
#define MQ_IC MQ_IX(MQ_CONTEXT_S,cccccccccccccccc)
#define MQ_ID MQ_IX(MQ_CONTEXT_S,dddddddddddddddd)
#define MQ_IE MQ_IX(MQ_CONTEXT_S,eeeeeeeeeeeeeeee)

#define XI0(x) MQ_IX(x,0000000000000000)
#define XI1(x) MQ_IX(x,1111111111111111)
#define XI2(x) MQ_IX(x,2222222222222222)
#define XIB(x) MQ_IX(x,BBBBBBBBBBBBBBBB)
#define XIE(x) MQ_IX(x,EEEEEEEEEEEEEEEE)

#define MQ_IXP(x,s,p) MqDLogV(x,__func__,0, "<" #s "-" #x "-%s> -> (%s:%d)\n", p, __FILE__, __LINE__);

#define MQ_I0P MQ_IXP(MQ_CONTEXT_S,0000000000000000,prefix)

#if defined(MQ_HAS_THREAD)

#define MV(f,v) fprintf(stderr,"%s(%s:%d:%d:%p) -> " #v "<" f ">\n", __func__, __FILE__, __LINE__, mq_getpid(), \
	      (void*) pthread_self(), v);fflush(stderr);
#define MVP(f,v) fprintf(stderr,"%s(%s:%d:%d:%p) -> %p:" #v "<" f ">\n", __func__, __FILE__, __LINE__, mq_getpid(), \
	      (void*) pthread_self(), v, v);fflush(stderr);
#define MVA(f,v,a) fprintf(stderr,"%s(%s:%d:%d:%p) -> " #v "<" f ">\n", __func__, __FILE__, __LINE__, mq_getpid(), \
	      (void*) pthread_self(), a);fflush(stderr);
#else

#define MV(f,v) fprintf(stderr,"%s(%s:%d:%d) -> " #v "<" f ">\n", __func__, __FILE__, __LINE__, mq_getpid(), \
	      v);fflush(stderr);
#define MVP(f,v) fprintf(stderr,"%s(%s:%d:%d) -> %p:" #v "<" f ">\n", __func__, __FILE__, __LINE__, mq_getpid(), \
	      v,v);fflush(stderr);
#define MVA(f,v,a) fprintf(stderr,"%s(%s:%d:%d) -> " #v "<" f ">\n", __func__, __FILE__, __LINE__, mq_getpid(), \
	      a);fflush(stderr);
#endif

#define printI(var)	MV(MQ_FORMAT_I,	var)
#define printUI(var)	MV("%u",	var)
#define printY(var)	MV(MQ_FORMAT_Y,	var)
#define printFo(var)	MV("%o",	var)
#define printH(var)	MV("%c",	var)
#define printW(var)	MV(MQ_FORMAT_W, var)
#define printP(var)	MV("%p",	(MQ_PTR)var)
#define printD(var)	MV(MQ_FORMAT_D,	var)
#define printF(var)	MV(MQ_FORMAT_F,	var)
#define printC(var)	MVP(MQ_FORMAT_C,var)
#define printc(var)	MV("%c",	var)
#define printX(var)	MV("%x",	var)
#define printO(var)	MVA("%s",	#var,	(var==MQ_YES?"yes":"no"));

#define printCP(txt,var)  MVA("%p",txt,var)

#define MLV(x,f,v) MqLogV(x,__func__,0,"(%s:%d) -> " #v "<" f ">\n", __FILE__, __LINE__, v);
#define MLVA(x,f,t,v) MqLogV(x,__func__,0,"(%s:%d) -> " #t "<" f ">\n", __FILE__, __LINE__, v);

#define printLP(var)	MLV(MQ_CONTEXT_S, "%p",        var)
#define printLI(var)	MLV(MQ_CONTEXT_S, MQ_FORMAT_I, var)
#define printLW(var)	MLV(MQ_CONTEXT_S, MQ_FORMAT_W, var)
#define printLO(var)	MLV(MQ_CONTEXT_S, MQ_FORMAT_C, var == MQ_YES ? "yes" : "no" )
#define printLC(var)	MLV(MQ_CONTEXT_S, MQ_FORMAT_C, var)
#define printLH(var)	MLV(MQ_CONTEXT_S, "%c",        var)
#define printR(var)	MLVA(MQ_CONTEXT_S, MQ_FORMAT_C, RET, MqLogErrorCode(ret))
#define printLV(fmt,args...)  MqDLogV(MQ_CONTEXT_S,0,fmt,args)

#define printXLP(x,var)	  MLV(x, "%p"       , var)
#define printXLI(x,var)	  MLV(x, MQ_FORMAT_I, var)
#define printXLC(x,var)	  MLV(x, MQ_FORMAT_C, var)
#define printXLW(x,var)	  MLV(x, MQ_FORMAT_W, var)
#define printXLO(x,var)	  MLV(x, MQ_FORMAT_C, var == MQ_YES ? "yes" : "no" )
#define printXR(x,var)	  MLVA(x, MQ_FORMAT_C, RET, MqLogErrorCode(ret))
#define printXLV(x,fmt,args...)  MqDLogV(x,0,fmt,args)

#define printC2(var,len)  fprintf(stderr,"%s->" #var " = <" MQ_FORMAT_C  ">\n", __func__, MqLogC(var,len));fflush(stderr);
#define printC3(buf) fprintf(stderr, "%s->" #buf " = <" MQ_FORMAT_C  ">\n", __func__, MqLogC(MqBufferGetC(buf),buf->cursize));fflush(stderr);
#define printX2(var) fprintf(stderr, "%s->" #var " = <" "%hx"     ">\n", __func__, var);fflush(stderr);
#define printT(var)  fprintf(stderr, "%s->" #var " = <" MQ_FORMAT_C  ">\n", __func__, MqLogTypeName(var));fflush(stderr);
#define printPy(var) fprintf(stderr, "%s->" #var "(%p) = <", __func__, var); PyObject_Print(var,stderr,0); fprintf(stderr,">\n");fflush(stderr);

#define CL(code) if (MQ_IS_CLIENT(MQ_CONTEXT_S)) {code}

#define bufLog(fmt,args...) {\
    char buf[1000];\
    snprintf(buf,1000,"echo '%-5i - %-30s : " fmt "' >> /tmp/buf.log",getpid(),__func__,args);\
    system(buf);\
}

#define printMI(msgque,var)  printLog(msgque,#var " = <" MQ_FORMAT_I  ">\n", var);fflush(stderr);

#define printM(mq) MqDLogV(mq,__func__,0,"(%s:%d) -> <" #mq ">\n", __FILE__, __LINE__);
#define printU(var) MqBufferLog(MQ_CONTEXT_S, var, #var);
#define printXU(ctx,var) MqBufferLog(ctx, var, #var);
#define printUL(var) MqBufferLLog(MQ_CONTEXT_S, var, #var);
#define printXULS(x,var) MqBufferLLogS(x, var, __func__ , #var);
#define printULS(var) printXULS(MQ_CONTEXT_S, var);
#define printV(var,ap) \
    fprintf(stderr, "%s->" #var " = <", __func__);\
    vprintf( var,ap);\
    fprintf(stderr, ">\n");\
    fflush(stderr);

#define printThread(str) fprintf(stderr,"%s(%s:%d) -> pid<%i>, id<%li> -> " #str "\n", __func__, __FILE__, __LINE__,\
      mq_getpid(), pthread_self());fflush(stderr);

#define PRT(id) fprintf(stderr,"%s(%s:%d) -> " #id " - pid<%i>, thread<%p>\n", __func__, __FILE__, __LINE__, getpid(), \
			    (void*)pthread_self());fflush(stderr);

#define PRT1(id,ptr) fprintf(stderr,"%s(%s:%d) -> " #id " - pid<%i>, thread<%p>, " #ptr "<%p>\n", \
  __func__, __FILE__, __LINE__, getpid(), (void*)pthread_self(), (void*)ptr);fflush(stderr);

/*****************************************************************************/
/*                                                                           */
/*                            msgque/definition                              */
/*                                                                           */
/*****************************************************************************/

#define MQ_FORMAT_In(n)	"%"	MQ_CPPSTR(n) "i"
#define MQ_FORMAT_XI(n) "%"	MQ_CPPSTR(n) "x"
#define MQ_FORMAT_Dn(n)	"%1."	MQ_CPPSTR(n) "e"
#define MQ_FORMAT_Cn(n)  "%"	MQ_CPPSTR(n) "s"

#define MQ_FORMAT_Y	"%hi"
#define MQ_FORMAT_O	"%hi"
#define MQ_FORMAT_S	"%hi"
#define MQ_FORMAT_I	"%i"
#define MQ_FORMAT_F	"%1.7e"
#define MQ_FORMAT_D	"%1.11e"
#define MQ_FORMAT_Z	"%i"
#define MQ_FORMAT_C	"%s"
#define MQ_FORMAT_B	"%p"
#define MQ_FORMAT_L	"%p"
#define MQ_FORMAT_P	"%p"
#define MQ_FORMAT_H	"%c"


#if defined(MQ_IS_POSIX)
# define MQ_FORMAT_W	  "%lli"
# define MQ_FORMAT_X	  "%llx"
# define MQ_FORMAT_Xn(n)  "%"	MQ_CPPSTR(n) "llx"
# define MQ_FORMAT_Wn(n)  "%"	MQ_CPPSTR(n) "lli"
#elif defined(MQ_IS_WIN32)
# define MQ_FORMAT_W	  "%I64i"
# define MQ_FORMAT_X	  "%I64x"
# define MQ_FORMAT_Xn(n)  "%"	MQ_CPPSTR(n) "I64x"
# define MQ_FORMAT_Wn(n)  "%"	MQ_CPPSTR(n) "I64i"
#endif

#endif /* MQ_DEBUG_H */

