--TEST--
pledge() function - violate the promises from pledge()
--FILE--
<?php
pledge('stdio');
exec('ls');
?>
--EXPECTF--
Abort trap (core dumped) 

Termsig=6
