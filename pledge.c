/* pledge.c */

/* Needed for OpenBSD pledge/unveil */
#include <unistd.h>
#include <errno.h>

/* Include PHP API */
#include "php.h"
#include "ext/standard/info.h"
#include "ext/spl/spl_exceptions.h"
#include "Zend/zend_exceptions.h"

/* This module's header file */
#include "php_pledge.h"

/* for exceptional circumstances */
zend_class_entry *pledge_exception_ce;
zend_class_entry *unveil_exception_ce;

/* provide info for the reflection API */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pledge, 0, 0, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, promises, IS_STRING, 1)
    ZEND_ARG_TYPE_INFO(0, execpromises, IS_STRING, 1)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_unveil, 0, 0, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 1)
    ZEND_ARG_TYPE_INFO(0, permissions, IS_STRING, 1)
ZEND_END_ARG_INFO();

/* define the function we want to add */
zend_function_entry pledge_functions[] = {
    PHP_FE(pledge, arginfo_pledge)
    PHP_FE(unveil, arginfo_unveil)
    PHP_FE_END
};

/* define the ini settings we want to add */
PHP_INI_BEGIN()
    PHP_INI_ENTRY("openbsd.pledge_promises", NULL, PHP_INI_SYSTEM, NULL)
    PHP_INI_ENTRY("openbsd.pledge_execpromises", NULL, PHP_INI_SYSTEM, NULL)
    PHP_INI_ENTRY("openbsd.unveil", "", PHP_INI_SYSTEM, NULL)
PHP_INI_END()

/* create exception classes */
void init_exceptions() {
    zend_class_entry ce_pe;
    INIT_CLASS_ENTRY(ce_pe, "PledgeException", NULL);
    pledge_exception_ce = zend_register_internal_class_ex(&ce_pe, spl_ce_RuntimeException);

    zend_class_entry ce_ue;
    INIT_CLASS_ENTRY(ce_ue, "UnveilException", NULL);
    unveil_exception_ce = zend_register_internal_class_ex(&ce_ue, spl_ce_RuntimeException);
}

/* use the unveil ini value to initialize the runtime */
int init_unveil_ini() {
    char *unveil_ini;

    unveil_ini = INI_STR("openbsd.unveil");

    if (unveil_ini != NULL) {
        char *unveil_directive = strtok(unveil_ini, ",");
        char *path;
        char *permissions;

        while (unveil_directive != NULL) {
            path = malloc(strlen(unveil_directive) + 1);
            permissions = malloc(strlen(unveil_directive) + 1);

            if (sscanf(unveil_directive, "%[^:]:%s", path, permissions) != 2) {
                zend_error(E_ERROR, "Error parsing unveil directive: \"%s\"\n", unveil_directive);

                return 1;
            }

            /* disallow future unveil calls, also stop parsing the rest of the config */
            if (strcmp(path, "null") == 0 && strcmp(permissions, "null") == 0) {
                free(path);
                free(permissions);

                if (unveil(NULL, NULL) != 0) {
                    zend_error(E_ERROR, "Call to unveil(NULL, NULL) to disallow future unveil calls failed");

                    return 1;
                }

                return 0;
            }

            if (unveil(path, permissions) != 0) {
                switch (errno) {
                    case E2BIG:
                        zend_error(E_ERROR, "The addition of path would exceed the per-process limit for unveiled paths");
                        break;
                    case EFAULT:
                        zend_error(E_ERROR, "path or permissions points outside the process's allocated address space");
                        break;
                    case ENOENT:
                        zend_error(E_ERROR, "A directory in path did not exist");
                        break;
                    case EINVAL:
                        zend_error(E_ERROR, "An invalid value of permissions was used");
                        break;
                    case EPERM:
                        zend_error(E_ERROR, "An attempt to increase permissions was made, or the path was not accessible, or unveil() was called after locking");
                        break;
                    default:
                        zend_error(E_ERROR, "Unveil error (%d)", errno);
                }

                free(path);
                free(permissions);

                return 1;
            }

            free(path);
            free(permissions);

            unveil_directive = strtok(NULL, ",");
        }
    }

    return 0;
}

/* use the pledge ini values to initialize the runtime */
int init_pledge_ini() {
    char *pledge_promises_ini;
    char *pledge_execpromises_ini;

    pledge_promises_ini = INI_STR("openbsd.pledge_promises");
    pledge_execpromises_ini = INI_STR("openbsd.pledge_execpromises");

    if (pledge(pledge_promises_ini, pledge_execpromises_ini) != 0) {
        switch (errno) {
            case EFAULT:
                zend_error(E_ERROR, "promises or execpromises points outside the process's allocated address space");
                break;
            case EINVAL:
                zend_error(E_ERROR, "promises is malformed or contains invalid keywords");
                break;
            case EPERM:
                zend_error(E_ERROR, "This process is attempting to increase permissions");
                break;
            default:
                zend_error(E_ERROR, "Pledge error (%d)", errno);
        }

        return 1;
    }

    return 0;
}

/* module init */
PHP_MINIT_FUNCTION(pledge) {
    REGISTER_INI_ENTRIES();

    init_exceptions();

    return SUCCESS;
}

/* runtime init */
PHP_RINIT_FUNCTION(pledge) {
    if (init_unveil_ini() != 0) {
        return FAILURE;
    }
    if (init_pledge_ini() != 0) {
        return FAILURE;
    }

    return SUCCESS;
}

/* module info */
PHP_MINFO_FUNCTION(pledge) {
    php_info_print_table_start();
    php_info_print_table_row(2, "OpenBSD pledge/unveil support", "enabled");
    php_info_print_table_row(2, "Version", PHP_PLEDGE_VERSION);
    php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
}

/* module shutdown */
PHP_MSHUTDOWN_FUNCTION(pledge) {
    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}

zend_module_entry pledge_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_PLEDGE_EXTNAME,
    pledge_functions,       /* Function entries */
    PHP_MINIT(pledge),      /* Module init */
    NULL,                   /* Module shutdown */
    PHP_RINIT(pledge),      /* Request init */
    NULL,                   /* Request shutdown */
    PHP_MINFO(pledge),      /* Module information */
    PHP_PLEDGE_VERSION,
    STANDARD_MODULE_PROPERTIES
};

/* install module */
ZEND_GET_MODULE(pledge)

/* function pledge(?string $promises = null, ?string $execpromises = null): bool */
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

    if (pledge(promises, execpromises) == 0) {
        RETURN_TRUE;
    }

    switch (errno) {
        case EFAULT:
            zend_throw_exception(
                pledge_exception_ce,
                "promises or execpromises points outside the process's allocated address space",
                errno
            );
            break;
        case EINVAL:
            zend_throw_exception(
                pledge_exception_ce,
                "promises is malformed or contains invalid keywords",
                errno
            );
            break;
        case EPERM:
            zend_throw_exception(
                pledge_exception_ce,
                "This process is attempting to increase permissions",
                errno
            );
            break;
        default:
            zend_throw_exception(
                pledge_exception_ce,
                "Pledge error",
                errno
            );
    }

    RETURN_FALSE;
}

/* function unveil(?string $path = null, ?string $permissions = null): bool */
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

    if (unveil(path, permissions) == 0) {
        RETURN_TRUE;
    }
    
    switch (errno) {
        case E2BIG:
            zend_throw_exception(
                unveil_exception_ce,
                "The addition of path would exceed the per-process limit for unveiled paths",
                errno
            );
            break;
        case EFAULT:
            zend_throw_exception(
                unveil_exception_ce,
                "path or permissions points outside the process's allocated address space",
                errno
            );
            break;
        case ENOENT:
            zend_throw_exception(
                unveil_exception_ce,
                "A directory in path did not exist",
                errno
            );
            break;
        case EINVAL:
            zend_throw_exception(
                unveil_exception_ce,
                "An invalid value of permissions was used",
                errno
            );
            break;
        case EPERM:
            zend_throw_exception(
                unveil_exception_ce,
                "An attempt to increase permissions was made, or the path was not accessible, or unveil() was called after locking",
                errno
            );
            break;
        default:
            zend_throw_exception(
                unveil_exception_ce,
                "Unveil error",
                errno
            );
    }

    RETURN_FALSE;
}
