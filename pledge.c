/* pledge.c */

/* Needed for OpenBSD pledge/unveil */
#include <unistd.h>
#include <errno.h>

/* Include PHP API */
#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"

/* This module's header file */
#include "php_pledge.h"

/* for exceptional circumstances */
zend_class_entry *pledge_exception_ce;
zend_class_entry *unveil_exception_ce;

/* provide info for the reflection API */
ZEND_BEGIN_ARG_INFO(arginfo_pledge, 0)
    ZEND_ARG_INFO(0, promises)
    ZEND_ARG_INFO(0, execpromises)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_unveil, 0)
    ZEND_ARG_INFO(0, path)
    ZEND_ARG_INFO(0, permissions)
ZEND_END_ARG_INFO();

/* define the function we want to add */
zend_function_entry pledge_functions[] = {
    PHP_FE(pledge, arginfo_pledge)
    PHP_FE(unveil, arginfo_unveil)
    PHP_FE_END
};

PHP_MINIT_FUNCTION(pledge) {
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "PledgeException", NULL);
    pledge_exception_ce = zend_register_internal_class_ex(&ce, spl_ce_RuntimeException);

    return SUCCESS;
}

PHP_MINIT_FUNCTION(unveil) {
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "UnveilException", NULL);
    unveil_exception_ce = zend_register_internal_class_ex(&ce, spl_ce_RuntimeException);

    return SUCCESS;
}

zend_module_entry pledge_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_PLEDGE_EXTNAME,
    pledge_functions,       /* Function entries */
    PHP_MINIT(pledge),      /* Module init */
    NULL,                   /* Module shutdown */
    NULL,                   /* Request init */
    NULL,                   /* Request shutdown */
    NULL,                   /* Module information */
    PHP_PLEDGE_VERSION,
    STANDARD_MODULE_PROPERTIES
};

/* Install module */
ZEND_GET_MODULE(pledge)

/* function pledge(string $promises = null, string $execpromises = null): bool */
PHP_FUNCTION(pledge) {
    char *promises = NULL;
    size_t promises_len = 0;
    char *execpromises = NULL;
    size_t execpromises_len = 0;

    ZEND_PARSE_PARAMETERS_START(0, 2)
        Z_PARAM_OPTIONAL
        Z_PARAM_STRING_EX(promises, promises_len, 1, 0)
        Z_PARAM_STRING_EX(execpromises, execpromises_len, 1, 0)
    ZEND_PARSE_PARAMETERS_END();

    if (pledge(promises, execpromises) != 0) {
        switch (errno) {
            case EINVAL:
                zend_throw_exception(pledge_exception_ce, "Invalid promise in promises string", errno);
                RETURN_FALSE;
            case EPERM:
                zend_throw_exception(pledge_exception_ce, "Attempt to increase permissions", errno);
                RETURN_FALSE;
            default:
                zend_throw_exception(pledge_exception_ce, "Pledge error", errno);
                RETURN_FALSE;
        }
    }

    RETURN_TRUE;
}

/* function unveil(string $path = null, string $permissions = null): bool */
PHP_FUNCTION(unveil) {
    char *path = NULL;
    size_t path_len = 0;
    char *permissions = NULL;
    size_t permissions_len = 0;

    ZEND_PARSE_PARAMETERS_START(0, 2)
        Z_PARAM_OPTIONAL
        Z_PARAM_STRING_EX(path, path_len, 1, 0)
        Z_PARAM_STRING_EX(permissions, permissions_len, 1, 0)
    ZEND_PARSE_PARAMETERS_END();

    if (unveil(path, permissions) != 0) {
        switch (errno) {
            case EINVAL:
                zend_throw_exception(unveil_exception_ce, "Invalid permission value", errno);
                RETURN_FALSE;
            case EPERM:
                zend_throw_exception(unveil_exception_ce, "Attempt to increase permissions", errno);
                RETURN_FALSE;
            case E2BIG:
                zend_throw_exception(unveil_exception_ce, "Too many unveiled paths", errno);
                RETURN_FALSE;
            case ENOENT:
                zend_throw_exception(unveil_exception_ce, "No such directory", errno);
                RETURN_FALSE;
            default:
                zend_throw_exception(unveil_exception_ce, "Unveil error", errno);
                RETURN_FALSE;
        }
    }

    RETURN_TRUE;
}

