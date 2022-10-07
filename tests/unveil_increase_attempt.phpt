--TEST--
unveil() function - increase permissions when no longer possible
--FILE--
<?php
unveil(__DIR__, 'r');
unveil();
unveil('/etc/', 'r');
?>
--EXPECTF--
Fatal error: Uncaught Exception: An attempt to increase permissions was made, or the path was not accessible, or unveil() was called after locking in %s:%d
Stack trace:
#0 %s(%d): unveil('/etc/', 'r')
#1 {main}
  thrown in %s on line %d
