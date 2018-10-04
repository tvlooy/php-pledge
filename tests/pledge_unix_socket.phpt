--TEST--
pledge() function - unix sockets via pledge()
--FILE--
<?php
socket_create(AF_UNIX, SOCK_STREAM, 0);
var_dump(pledge('unix stdio'));
socket_create(AF_UNIX, SOCK_STREAM, 0);
var_dump(pledge('stdio'));
socket_create(AF_UNIX, SOCK_STREAM, 0);
?>
--EXPECT--
bool(true)
bool(true)
Abort trap (core dumped) 

Termsig=6
