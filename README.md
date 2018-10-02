# PHP pledge

This is a PHP extension that adds support for OpenBSD's pledge and unveil system calls.

## Requirements

This extension was tested with PHP 7.2 and needs at least OpenBSD 6.4.

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

To allow read/write filesystem access, but not network access:

```
pledge('rpath wpath cpath stdio');
```

To allow inet/unix socket access and DNS queries, but not filesystem access:

```
pledge('inet unix dns stdio');
```

You will need to include the ```stdio``` promise every time because PHP will not work well without it. 
All promises are documented in [the OpenBSD pledge(2) manual page](http://man.openbsd.org/OpenBSD-current/man2/pledge.2).

If the PHP ```pledge()``` call fails, it will throw a ```\PledgeException```.

## Unveil usage

If the PHP ```unveil()``` call fails, it will throw a ```\UnveilException```.

## Notes

If you are running the php interactive shell with ```php -a``` you need these promises:

```
pledge('rpath wpath cpath tty ioctl stdio');
```

If you want to pledge Drupal8 or Symfony2 running on FPM, you need at least:

```
pledge('rpath wpath cpath inet dns flock fattr stdio');
```

So, preventing filesystem or network access seems impossible.

Just to serve a ```phpinfo()``` or "hello world" from FPM you need:

```
pledge('rpath flock inet stdio');
```

