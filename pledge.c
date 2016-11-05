/* pledge.c */

/* Needed for OpenBSD pledge */
#include <unistd.h>
#include <errno.h>

/* Include PHP API */
#include "php.h"

/* This module's header file */
#include "php_pledge.h"

/* provide info for the reflection API */
ZEND_BEGIN_ARG_INFO(arginfo_pledge, 0)
    ZEND_ARG_INFO(0, promises)
ZEND_END_ARG_INFO();

/* define the function we want to add */
zend_function_entry pledge_functions[] = {
    PHP_FE(pledge, arginfo_pledge)
    PHP_FE_END
};

zend_module_entry pledge_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_PLEDGE_EXTNAME,
    pledge_functions,       /* Function entries */
    NULL,                   /* Module init */
    NULL,                   /* Module shutdown */
    NULL,                   /* Request init */
    NULL,                   /* Request shutdown */
    NULL,                   /* Module information */
    PHP_PLEDGE_VERSION,
    STANDARD_MODULE_PROPERTIES
};

/* Install module */
ZEND_GET_MODULE(pledge)

/* function pledge(string $promises): bool */
PHP_FUNCTION(pledge) {
    char *promises = NULL;
    size_t promises_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &promises, &promises_len) == FAILURE) {
        return;
    }

    if (pledge(promises, NULL) != 0) {
        switch(errno) {
            case EINVAL:
                zend_throw_exception(NULL, "invalid promise in promises string", 0);
                RETURN_FALSE;
            case EPERM:
                zend_throw_exception(NULL, "attempt to increase permissions", 0);
                RETURN_FALSE;
            default:
                zend_throw_exception(NULL, "pledge error", 0);
                RETURN_FALSE;
        }
    }

    RETURN_TRUE;
}

