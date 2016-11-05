# PHP pledge

This is a PHP extension that adds support for OpenBSD's [pledge](http://man.openbsd.org/OpenBSD-current/man2/pledge.2) system call.

## The theory

The pledge system call, allows a program to restrict the types of operations the program can do after that point. Unlike other similar systems, pledge is specifically designed for programs that need to use a wide variety of operations on initialization, but a fewer number after initialization (when user input will be accepted).

## Build

```
git clone https://github.com/tvlooy/php-pledge
cd php-pledge
phpize
./configure
make
sudo make install
```

Add the following to your configuration to enable the extension:

```
extension=plegde.so
```

