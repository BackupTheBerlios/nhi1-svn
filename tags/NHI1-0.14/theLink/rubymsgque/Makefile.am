#+
#§  \file       theLink/rubymsgque/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2010 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

AM_LIBTOOLFLAGS = --tag=disable-static

pkglib_LTLIBRARIES = rubymsgque.la

rubymsgque_la_SOURCES =	msgque_ruby.h context_ruby.c read_ruby.c send_ruby.c error_ruby.c \
			  MqS_ruby.c MqSException_ruby.c MqBufferS_ruby.c \
			    misc_ruby.c config_ruby.c service_ruby.c link_ruby.c \
			      slave_ruby.c sys_ruby.c MqFactoryS_ruby.c MqDumpS_ruby.c \
				storage_ruby.c


# use this if you want to include the libmsgque files into the rubymsgque libraray
rubymsgque_la_CFLAGS = -I$(srcdir)/../libmsgque $(RUBY_CPPFLAGS) $(AM_CFLAGS) -DMQ_IGNORE_EXTERN
rubymsgque_la_LIBADD = ../libmsgque/libtmp.la $(RUBY_EXTRA_LIBS)

rubymsgque_la_LDFLAGS = -shared -module -avoid-version -no-undefined

