/**
 *  \file       libmsgque/debug.h
 *  \brief      \$Id: debug.h 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef MQ_DEBUG_H
#define MQ_DEBUG_H

/*****************************************************************************/
/*                                                                           */
/*                              msgque/types                                 */
/*                                                                           */
/*****************************************************************************/

#define MX(s) printf("%s(%s:%d) -> %s \n", __func__, __FILE__, __LINE__, #s);fflush(stdout);

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

#define IX(x,s) MqDLogX(x,__func__,0,"(%s:%d) -> " #x "<" #s ">\n", __FILE__, __LINE__);

#define I0 IX(MQ_CONTEXT_S,0000000000000000)
#define I1 IX(MQ_CONTEXT_S,1111111111111111)
#define I2 IX(MQ_CONTEXT_S,2222222222222222)
#define I3 IX(MQ_CONTEXT_S,3333333333333333)
#define I4 IX(MQ_CONTEXT_S,4444444444444444)
#define I5 IX(MQ_CONTEXT_S,5555555555555555)
#define I6 IX(MQ_CONTEXT_S,6666666666666666)
#define I7 IX(MQ_CONTEXT_S,7777777777777777)
#define I8 IX(MQ_CONTEXT_S,8888888888888888)
#define I9 IX(MQ_CONTEXT_S,9999999999999999)
#define IA IX(MQ_CONTEXT_S,aaaaaaaaaaaaaaaa)
#define IB IX(MQ_CONTEXT_S,bbbbbbbbbbbbbbbb)
#define IC IX(MQ_CONTEXT_S,cccccccccccccccc)
#define ID IX(MQ_CONTEXT_S,dddddddddddddddd)
#define IE IX(MQ_CONTEXT_S,eeeeeeeeeeeeeeee)

#define XI0(x) IX(x,0000000000000000)
#define XI1(x) IX(x,1111111111111111)
#define XI2(x) IX(x,2222222222222222)
#define XIE(x) IX(x,EEEEEEEEEEEEEEEE)

#define MV(f,v) printf("%s(%s:%d) -> " #v "<" f ">\n", __func__, __FILE__, __LINE__, v);fflush(stdout);
#define MVA(f,v,a) printf("%s(%s:%d) -> " #v "<" f ">\n", __func__, __FILE__, __LINE__, a);fflush(stdout);

#define printI(var)	MV(MQ_FORMAT_I,	var)
#define printFo(var)	MV("%o",	var)
#define printH(var)	MV("%c",	var)
#define printW(var)	MV(MQ_FORMAT_W, var)
#define printP(var)	MV("%p",	var)
#define printD(var)	MV(MQ_FORMAT_D,	var)
#define printF(var)	MV(MQ_FORMAT_F,	var)
#define printC(var)	MV(MQ_FORMAT_C,	var)
#define printc(var)	MV("%c",	var)
#define printO(var)	MVA("%s",	#var,	(var==MQ_YES?"yes":"no"));

#define printCP(txt,var)  MVA("%p",txt,var)

#define MLV(x,f,v) MqDLogX(x,__func__,0,"(%s:%d) -> " #v "<" f ">\n", __FILE__, __LINE__, v);
#define MLV2(x,f,t,v) MqDLogX(x,__func__,0,"(%s:%d) -> %s<" f ">\n", __FILE__, __LINE__, t, v);

#define printLP(var)	MLV(MQ_CONTEXT_S, "%p",        var)
#define printLI(var)	MLV(MQ_CONTEXT_S, MQ_FORMAT_I, var)
#define printLW(var)	MLV(MQ_CONTEXT_S, MQ_FORMAT_W, var)
#define printLO(var)	MLV(MQ_CONTEXT_S, MQ_FORMAT_C, var == MQ_YES ? "yes" : "no" )
#define printLC(var)	MLV(MQ_CONTEXT_S, MQ_FORMAT_C, var)
#define printLH(var)	MLV(MQ_CONTEXT_S, "%c",        var)
#define printR(var)	MLV2(MQ_CONTEXT_S, MQ_FORMAT_C, "RET", MqLogErrorCode(ret))

#define printXLP(x,var)	  MLV(x, "%p"       , var)
#define printXLI(x,var)	  MLV(x, MQ_FORMAT_I, var)
#define printXLC(x,var)	  MLV(x, MQ_FORMAT_C, var)
#define printXLW(x,var)	  MLV(x, MQ_FORMAT_W, var)
#define printXLO(x,var)	  MLV(x, MQ_FORMAT_C, var == MQ_YES ? "yes" : "no" )
#define printXR(x,var)	  MLV2(x, MQ_FORMAT_C, "RET", MqLogErrorCode(ret))

#define printC2(var,len)  printf( "%s->" #var " = <" MQ_FORMAT_C  ">\n", __func__, MqLogC(var,len));fflush(stdout);
#define printC3(buf) printf( "%s->" #buf " = <" MQ_FORMAT_C  ">\n", __func__, MqLogC(MqBufferGetC(buf),buf->cursize));fflush(stdout);
#define printX2(var) printf( "%s->" #var " = <" "%hx"     ">\n", __func__, var);fflush(stdout);
#define printT(var)  printf( "%s->" #var " = <" MQ_FORMAT_C  ">\n", __func__, MqLogTypeName(var));fflush(stdout);
#define printPy(var) printf( "%s->" #var "(%p) = <", __func__, var); PyObject_Print(var,stdout,0); printf(">\n");fflush(stdout);

#define CL(code) if (MQ_IS_CLIENT(MQ_CONTEXT_S)) {code}

#define bufLog(fmt,MQ_VAR_ARGS) {\
    char buf[1000];\
    snprintf(buf,1000,"echo '%-5i - %-30s : " fmt "' >> /tmp/buf.log",getpid(),__func__,args);\
    system(buf);\
}

#define printMI(msgque,var)  printLog(msgque,#var " = <" MQ_FORMAT_I  ">\n", var);fflush(stdout);

#define printM(mq) MqDLogX(mq,__func__,0,"(%s:%d) -> <" #mq ">\n", __FILE__, __LINE__);
#define printU(var) MqBufferLog(MQ_CONTEXT_S, var, #var);
#define printXU(ctx,var) MqBufferLog(ctx, var, #var);
#define printUL(var) MqBufferLLog(MQ_CONTEXT_S, var, #var);
#define printXULS(x,var) MqBufferLLogS(x, var, __func__ , #var);
#define printULS(var) printXULS(MQ_CONTEXT_S, var);
#define printV(var,ap) \
    printf( "%s->" #var " = <", __func__);\
    vprintf( var,ap);\
    printf( ">\n");\
    fflush(stdout);

#define printThread(str) printf("%s(%s:%d) -> pid<%i>, id<%li> -> " #str "\n", __func__, __FILE__, __LINE__,\
      mq_getpid(), pthread_self());fflush(stdout);

#define PRT(id) printf("%s(%s:%d) -> " #id " - pid<%i>, thread<%p>\n", __func__, __FILE__, __LINE__, getpid(), \
			    (void*)pthread_self());fflush(stdout);

#define PRT1(id,ptr) printf("%s(%s:%d) -> " #id " - pid<%i>, thread<%p>, " #ptr "<%p>\n", \
  __func__, __FILE__, __LINE__, getpid(), (void*)pthread_self(), (void*)ptr);fflush(stdout);

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
