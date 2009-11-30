#ifndef PHP_PHPMSGQUE_H
#define PHP_PHPMSGQUE_H 1

#define PHP_PHPMSGQUE_WORLD_VERSION "1.0"
#define PHP_PHPMSGQUE_WORLD_EXTNAME "phpmsgque"

PHP_FUNCTION(phpmsgque_init);

extern zend_module_entry phpmsgque_module_entry;
#define phpext_phpmsgque_ptr &phpmsgque_module_entry

#endif

