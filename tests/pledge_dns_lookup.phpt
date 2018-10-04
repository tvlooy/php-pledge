--TEST--
pledge() function - dns lookup via pledge()
--FILE--
<?php
gethostbyname('openbsd.org');
var_dump(pledge('dns stdio'));
gethostbyname('openbsd.org');
var_dump(pledge('stdio'));
gethostbyname('openbsd.org');
?>
--EXPECT--
bool(true)
bool(true)
Abort trap (core dumped) 

Termsig=6
