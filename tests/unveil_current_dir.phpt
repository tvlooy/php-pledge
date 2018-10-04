--TEST--
unveil() function - unveil current directory
--FILE--
<?php
var_dump(file_exists('/etc'));
unveil(__DIR__, 'r');
var_dump(file_exists('/etc'));
unveil('/etc/', 'r');
var_dump(file_exists('/etc'));
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
