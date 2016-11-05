PHP_ARG_ENABLE(pledge, activate support for the pledge syscall,
[  --enable-pledge  Enable pledge])

if test "$PHP_PLEDGE" != "no"; then
    AC_CHECK_FUNC(pledge,, AC_MSG_ERROR([This extension needs a system that supports the pledge system call (OpenBSD >= 5.9)!]))

    PHP_NEW_EXTENSION(pledge, pledge.c, $ext_shared)
fi

