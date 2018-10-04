--TEST--
unveil() function - pledge
--FILE--
<?php
var_dump(pledge('stdio'));
unveil(__DIR__, 'r');
?>
--EXPECTF--
bool(true)
Abort trap (core dumped) 

Termsig=6
