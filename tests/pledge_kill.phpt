--TEST--
pledge() function - allow kill with proc promise for pledge()
--FILE--
<?php
$sigurg = 16;
$pid = posix_getpid();

var_dump(posix_kill($pid, $sigurg));
//pledge('proc stdio');
var_dump(posix_kill($pid, $sigurg));
pledge('stdio');
var_dump(posix_kill($pid, $sigurg));
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
