# PHP pledge

This is a PHP extension that adds support for OpenBSD's pledge and unveil system calls.

## Requirements

This extension was tested with PHP 7.0, 7.1, 7.2 and needs at least OpenBSD 6.4.

## The theory

The [pledge(2) system call](http://man.openbsd.org/OpenBSD-current/man2/pledge.2) allows a program to restrict the types
of operations the program can do after that point. Unlike other similar systems, pledge is specifically designed for
programs that need to use a wide variety of operations on initialization, but a fewer number after initialization (when
user input will be accepted).

The pledge system call is supported on OpenBSD >= 5.9

The first call to [unveil(2) system call](http://man.openbsd.org/OpenBSD-current/man2/unveil.2) restricts the filesystem
view. Subsequent calls can open it more. To prevent furter unveiling, call unveil with no parameters or drop the unveil
pledge if the program is pledged.

The unveil system call is supported on OpenBSD >= 6.4

## Build

```
git clone https://github.com/tvlooy/php-pledge
cd php-pledge
phpize
./configure
make
doas make install
```

Add the following to your configuration to enable the extension:

```
extension=pledge.so
```

Run the tests with:

```
NO_INTERACTION=1 make test
```

## Pledge usage

Note: You will need to include the ```stdio``` promise every time because PHP will not work well without it.

All promises are documented in [the OpenBSD pledge(2) manual page](http://man.openbsd.org/OpenBSD-current/man2/pledge.2).

When pleding you program keep an eye on ```/var/log/messages``` to see your violations.

To allow read/write filesystem access, but not network access:

```php
pledge('stdio rpath wpath cpath');
```

To allow inet/unix socket access and DNS queries, but not filesystem access:

```php
pledge('stdio inet unix dns');
```

If the PHP ```pledge()``` call fails, it will throw a ```\PledgeException```.

To set pledges on an execve child:

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

## Usage tips

Be carefull what you pledge! If you run PHP with mod_php, you will be pledging and Apache child processes! If you pledge
php_fpm you will be pledging it for the lifetime of the process, not just the request!

For your web SAPI, make sure you limit the amount of loaded extensions. If you only need ```phar```, ```pcntl```, ...
in your CLI, then don't load them in your web SAPI.

Unveil can for example be applied to FPM processes in addition to or as an alternative to chroot.

Pledging CLI processes is the most convenient usecase. Examples:

If you are running the php interactive shell with ```php -a``` you need these promises:

```php
pledge('stdio rpath wpath cpath tty ioctl');
```

If you want to pledge Drupal8 or Symfony2 running on FPM, you need at least:

```php
pledge('stdio rpath wpath cpath inet dns flock fattr');
```

Preventing filesystem or network access with pledge seems impossible.

Just to serve a ```phpinfo()``` or "hello world" from FPM you need:

```php
pledge('stdio rpath flock inet');
```

You can then further limit filesystem access with unveil(). If you are in web SAPI, again remember that you are not unveiling
for one request but for all subsequent requests.

### Limiting network calls

There currently is nothing to support "unveiling" of network resources. But this can be achieved with PF. Eg:

```
block out proto {tcp udp} user your_fpm_user
pass out proto tcp to $mysql_db port 3306 user your_fpm_user
pass out proto tcp to $some_rest_api port 443 user your_fpm_user
```
