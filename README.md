# PHP pledge

This is a PHP extension that adds support for OpenBSD's pledge system call.

## The theory

The [pledge(2) system call](http://man.openbsd.org/OpenBSD-current/man2/pledge.2) allows a program to restrict the types of operations the program can do after that point. Unlike other similar systems, pledge is specifically designed for programs that need to use a wide variety of operations on initialization, but a fewer number after initialization (when user input will be accepted).

The plege system call is supported on OpenBSD >= 5.9

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

## Usage

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

## Limitations

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

