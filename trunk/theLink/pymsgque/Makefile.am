#+
#:  \file       theLink/pymsgque/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

C_FILES = MqS_python.c MqBufferS_python.c \
  msgque_python.c misc_python.c send_python.c read_python.c \
    service_python.c error_python.c config_python.c slave_python.c context_python.c \
      link_python.c storage_python.c MqFactoryS_python.c MqDumpS_python.c

H_FILES = msgque_python.h

if USE_WINSDK

.libs:
	mkdir .libs

.libs/%.obj: %.c $(H_FILES) | .libs
	$(SDK_CL) -I$(abs_top_srcdir_m)/theLink/libmsgque $(PYTHON_CFLAGS) "$(shell $(CYGPATH_M) $<)"

.libs/pymsgque.pyd: $(patsubst %.c,.libs/%.obj, $(notdir $(C_FILES))) | .libs
	$(SDK_LINK) "$@" "$^" ../libmsgque/.libs/msgque.lib $(PYTHON_LDFLAGS)

sdkbindir = $(pkglibdir)
sdkbin_DATA = .libs/pymsgque.pyd

else

# To build a python extension on windows using active-state-python at
# C:/Python32 using the mingw cross-compiler 'x86_64-w64-mingw32' of 
# cygwin was NOT sucessfull. Reason:
# 1. dll at /cygdrive/c/Windows/Sysnative/python32.dll
#    32bit cygwin get with /cygdrive/c/Windows/Sytem32 the
#    /cygdrive/c/Windows/SysWOW64 directory
# 2. the default export library /cygdrive/c/Python32/libs/python32.lib
#    copied to /cygdrive/c/Python32/libs/libpython32.dll.a does NOT
#    contail ALL required import-definition for exported data.
#    exp. undefined reference to `__imp__Py_NoneStruct'
# 3. creating an import library using 'x86_64-w64-mingw32-dlltool.exe'
#    create an empty def file.
# next step: using the msvc compiler

pkglib_LTLIBRARIES = pymsgque.la

pymsgque_la_SOURCES = $(C_FILES) $(H_FILES)

pymsgque_la_CFLAGS = -shared -I$(srcdir)/../libmsgque $(PYTHON_CFLAGS) -DMQ_IGNORE_EXTERN
pymsgque_la_LDFLAGS = -shared -module -avoid-version -no-undefined $(PYTHON_LDFLAGS)
pymsgque_la_LIBADD = ../libmsgque/libmsgque.la

if WIN32
pymsgque_la_CFLAGS += -U_DEBUG
endif

endif

