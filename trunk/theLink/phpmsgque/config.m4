PHP_ARG_ENABLE(phpmsgque, whether to enable 'phpmsgque' support,
[  --enable-phpmsgque   Enable 'phpmsgque' support])

if test "$PHP_PHPMSGQUE" = "yes"; then
  AC_DEFINE(HAVE_PHPMSGQUE, 1, [Whether you have 'phpmsgque'])
  PHP_NEW_EXTENSION(phpmsgque, phpmsgque.c, $ext_shared)
fi
