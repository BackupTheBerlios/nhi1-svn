#include "php_config.h"
#include "php.h"
#include "php_phpmsgque.h"

static function_entry phpmsgque_functions[] = {
    PHP_FE(phpmsgque_init, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry phpmsgque_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_PHPMSGQUE_WORLD_EXTNAME,
    phpmsgque_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_PHPMSGQUE_WORLD_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PHPMSGQUE
ZEND_GET_MODULE(phpmsgque)
#endif

PHP_FUNCTION(phpmsgque_init)
{
    RETURN_STRING("Hello World", 1);
}
