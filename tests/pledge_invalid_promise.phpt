--TEST--
pledge() function - invalid promise for pledge()
--FILE--
<?php
try {
    pledge('no_such_promise');
} catch (\Exception $e) {
    var_dump($e->getMessage());
}
?>
--EXPECTF--
string(34) "invalid promise in promises string"
