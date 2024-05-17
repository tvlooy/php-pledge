# PHP pledge

This is a PHP extension that adds support for OpenBSD's pledge and unveil system calls.

## Requirements

This extension works with >= PHP 7.2 and needs at least OpenBSD 6.4.

## The theory

The [pledge(2) system call](http://man.openbsd.org/OpenBSD-current/man2/pledge.2) allows a program to restrict the types
of operations the program can do after that point. Unlike other similar systems, pledge is specifically designed for
programs that need to use a wide variety of operations on initialisation, but a fewer number after initialisation (when
user input will be accepted).

The pledge system call is supported on OpenBSD >= 5.9

The first call to [unveil(2) system call](http://man.openbsd.org/OpenBSD-current/man2/unveil.2) restricts the filesystem
view. Subsequent calls can open it more. To prevent further unveiling, call unveil with no parameters or drop the unveil
pledge if the program is pledged.

The unveil system call is supported on OpenBSD >= 6.4

## OpenBSD installation

This package is present in OpenBSD packages. To install it, run:

```sh
pkg_add pecl83-pledge
```

This package can also be installed from ports. To install it, make sure you
[have the ports tree](https://www.openbsd.org/faq/ports/ports.html#PortsFetch) on your machine. Next, install the port:

```sh
cd /usr/ports/www/pecl-pledge/
env FLAVOR=php83 make FETCH_PACKAGES= install
```

Using the extension:

```sh
echo 'extension=pledge' > /etc/php-8.3/pledge.ini
php-8.3 -m | grep pledge
```

You can then use the `pledge()` and `unveil()` functions, or set a pledge and unveil default with ini values:

- openbsd.pledge_promises
- openbsd.pledge_execpromises
- openbsd.unveil

Promises are the pledge promises for your main process, execpromises are for child processes (or the same as the parent
if not specified). The openbsd.unveil ini setting can receive many unveil calls eg:
/html/public:r,/var/www/html/tmp:rw would make the /var/www/html/public directory readable and the
/var/www/html/tmp readable and writeable. If you want to restrict further calls to unveil, end with null:null.

## FPM configuration

You can set promises and unveils in your PHP-FPM config. Eg:

```
php_admin_value[openbsd.pledge_promises] = stdio rpath wpath cpath fattr flock unveil inet
php_admin_value[openbsd.unveil] = /:r,/tmp:rwc,/htdocs/var/log:rwc,/htdocs/var/cache:rwc,null:null
```

An simplified httpd example config:

```
chroot "/var/www"

server "example.com" {
    listen on * port 80
    root "/htdocs/public"
    directory index "index.php"

    # Assets not served by PHP
    location match "\.(css|gif|jpg|png|js)$" {
        pass
    }

    location match "/specific-path-1" {
        request rewrite "/index.php/%1"
        fastcgi socket "/run/php-fpm-specific-path-1.sock"
    }

    location match "/specific-path-2" {
        request rewrite "/index.php/%1"
        fastcgi socket "/run/php-fpm-specific-path-2.sock"
    }

    # The default PHP handler
    location match "^/(.+)$" {
        request rewrite "/index.php/%1"
        fastcgi socket "/run/php-fpm.sock"
    }
}
```

With a simplified PHP-FPM config:

```
[global]
include=/etc/php-fpm.d/*.conf

[specific-path-1]
user = www
group = www
listen.owner = www
listen.group = www
listen.mode = 0660

pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3

chroot = /var/www
pm.max_requests = 1

listen = /var/www/run/php-fpm-specific-path-1.sock
php_admin_value[openbsd.pledge_promises] = stdio rpath wpath cpath fattr flock unveil
php_admin_value[openbsd.unveil] = /:r,/tmp:rwc,/htdocs/var/log:rwc,/htdocs/var/cache:rwc

[specific-path-2]
user = www
group = www
listen.owner = www
listen.group = www
listen.mode = 0660

pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3

chroot = /var/www
pm.max_requests = 1

listen = /var/www/run/php-fpm-specific-path-2.sock
php_admin_value[openbsd.pledge_promises] = stdio rpath wpath cpath fattr flock unveil
php_admin_value[openbsd.unveil] = /:r,/tmp:rwc,/htdocs/var/log:rwc,/htdocs/var/cache:rwc

[www]
user = www
group = www
listen.owner = www
listen.group = www
listen.mode = 0660

pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3

chroot = /var/www
pm.max_requests = 1

listen = /var/www/run/php-fpm.sock
php_admin_value[openbsd.pledge_promises] = stdio rpath wpath cpath fattr flock unveil inet
php_admin_value[openbsd.unveil] = /:r,/tmp:rwc,/htdocs/var/log:rwc,/htdocs/var/cache:rwc
```

Don't forget to call `unveil(null, null);` in your PHP userland to disallow future unveil calls, or specify null:null as
the last argument eg:

```
php_admin_value[openbsd.unveil] = /:r,/tmp:rwc,/htdocs/var/log:rwc,/htdocs/var/cache:rwc,null:null
```

## Build from source

Install a release from PECL [security/pledge](https://pecl.php.net/package/pledge) or build the latest from source:

```sh
git clone https://github.com/tvlooy/php-pledge
cd php-pledge
phpize
./configure
make
doas make install
```

Add the following to your configuration to enable the extension:

```
extension=pledge
```

Run the tests with:

```sh
NO_INTERACTION=1 make test
```

## Pledge usage

All promises are documented in [the OpenBSD pledge(2) manual page](http://man.openbsd.org/OpenBSD-current/man2/pledge.2).

When pledging your program keep an eye on ```/var/log/messages``` to see your violations.

To allow read/write filesystem access, but not network access:

```php
pledge('stdio rpath wpath cpath');
```

To allow inet/unix socket access and DNS queries, but not filesystem access:

```php
pledge('stdio inet unix dns');
```

If the PHP ```pledge()``` call fails, it will throw a ```\PledgeException```.

The ```error``` pledge will make the call fail but will not kill the program. An example:

```sh
$ php \
    -dopenbsd.pledge_promises='stdio dns' \
    -r 'echo gethostbyname("openbsd.org");'
199.185.178.80
 
$ php \
    -dopenbsd.pledge_promises='stdio error' \
    -r 'echo gethostbyname("openbsd.org");'
openbsd.org

$ php \
    -dopenbsd.pledge_promises='stdio' \
    -r 'echo gethostbyname("openbsd.org");'
Abort trap (core dumped)
```

To set promises for an execve child:

```php
pledge(null, 'stdio rpath tty error');
pcntl_exec('/bin/ls');
```

## Unveil usage

```php
# Works
print_r(scandir('/var'));

# Limit read only access to /var/spool
unveil('/var/spool', 'r');

# No longer works
print_r(scandir('/var'));
# Works
print_r(scandir('/var/spool'));

# Disallow more unveils
unveil();

# No longer works
unveil('/var', 'r');
```

If the PHP ```unveil()``` call fails, it will throw a ```\UnveilException```.

You can also restrict a process to /var/empty with unveil from the CLI:

```sh
$ php \
    -dopenbsd.unveil='/var/empty:r,null:null' \
    -dopenbsd.pledge_promises='stdio dns' \
    -r 'echo gethostbyname("openbsd.org");'
199.185.178.80
```

## Usage tips

See [ctors/pledge-symfony-routing](https://packagist.org/packages/ctors/pledge-symfony-routing) for Symfony framework
support.

Be careful where you pledge / unveil! If you run PHP with mod_php, you will be pledging an Apache child processes! If
you pledge / unveil php_fpm you will be pledging it for the lifetime of the process, not just the request!

Set `pm.max_requests = 1` in the PHP-FPM config to always start with a new process.

For your web SAPI, make sure you limit the amount of loaded extensions. If you only need ```phar```, ```pcntl```, ...
in your CLI, then don't load them in your web SAPI.

Unveil can for example be applied to FPM processes in addition to or as an alternative to chroot. Note that PHP-FPM runs
chroot-ed by default on OpenBSD, but you can't pledge chroot, so you can't start FPM with a openbsd.pledge_promises.

Ways to configure:

  - set `openbsd.pledge_promises` and/or `openbsd.unveil` in `php.ini`
  - or set `php_admin_value[openbsd.pledge_promises]` and/or `php_admin_value[openbsd.unveil]` in the PHP-FPM config
  - and/or apply `pledge()` and `unveil()` in your userland code

If you want to reuse pledged / unveiled processes, you can work around failing calls by checking if the process is
already running with a restricted view. Eg:

```php
if (is_dir('/etc')) {
    unveil(__DIR__, 'r');
}
pledge('stdio rpath flock inet');
```

## A word about FFI

Note that with PHP >= 7.4 you could also use the Foreign Function Interface (FFI) extension that is in core to call libc
functions. But the FFI extension is not built by default in the OpenBSD ports/packages. And, it does not come with ini
settings like this pledge extension, so you cannot hook pledge() and unveil() into the PHP engine startup.

```sh
$ cat test_ffi.php       
<?php

$libc = FFI::cdef('
    int unveil(const char *path, const char *permissions);
', 'libc.so.97');

var_dump(count(scandir('/etc')));
$libc->unveil(__DIR__, 'r');
scandir('/etc');

$ php -dextension=ffi test_ffi.php      
int(77)

Warning: scandir(/etc): failed to open dir: No such file or directory in /home/tvl/test_ffi.php on line 9

Warning: scandir(): (errno 2): No such file or directory in /home/tvl/test_ffi.php on line 9
```

### Limiting network calls

There currently is nothing to support "unveiling" of network resources. But this can be achieved with PF. Eg:

```
block out proto {tcp udp} user your_fpm_user
pass out proto tcp to $mysql_db port 3306 user your_fpm_user
pass out proto tcp to $some_rest_api port 443 user your_fpm_user
```

### Package

Run `pecl package` to package this extension.

On OpenBSD, get a base64 sha256 checksum with `sha256 -b *.tgz` and the file size with `ls -l *.tgz`.
