#+
#§  \file       pymsgque/setup.py
#§  \brief      \$Id: setup.py 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§
from distutils.core import setup, Extension
setup(
  name='pymsgque',
  version='3.2',
  description='libmsgque python module',
  author='Andreas Otto',
  author_email='aotto1968@users.sourceforge.net',
  url='http://libmsgque.sourceforge.net/',
  ext_modules=[Extension(
      'pymsgque', 
      [
	'config_python.c', 'get_python.c',  'misc_python.c',    'msgque_python.c',  
	'read_python.c',   'send_python.c', 'service_python.c', 'test_python.c'
      ],
      include_dirs=['../src'],
      library_dirs=['../src/.libs'],
      libraries=['msgque']
  )],
)

