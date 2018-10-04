--TEST--
unveil() function - increase permissions when no longer possible
--FILE--
<?php
unveil(__DIR__, 'r');
unveil();
unveil('/etc/', 'r');
?>
--EXPECTF--
Fatal error: Uncaught Exception: Attempt to increase permissions in %s:%d
Stack trace:
#0 %s(%d): unveil('/etc/', 'r')
#1 {main}
  thrown in %s on line %d
