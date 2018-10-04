--TEST--
unveil() function - read only permissions
--FILE--
<?php
unveil(__DIR__, 'r');
var_dump(is_writable(__DIR__));
file_put_contents('test', 'test');
?>
--EXPECTF--
bool(true)

Warning: file_put_contents(test): failed to open stream: No such file or directory in %s on line %d
