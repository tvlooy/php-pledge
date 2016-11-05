--TEST--
pledge() function - read file via pledge()
--FILE--
<?php
file_get_contents(__FILE__);
var_dump(pledge('rpath stdio'));
file_get_contents(__FILE__);
var_dump(pledge('stdio'));
file_get_contents(__FILE__);
?>
--EXPECTF--
bool(true)
bool(true)
Abort trap (core dumped) 

Termsig=6
