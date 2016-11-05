--TEST--
pledge() function - invalid promise for pledge()
--FILE--
<?php
try {
    pledge('no_such_promise');
} catch (\PledgeException $e) {
    var_dump($e->getMessage());
}
?>
--EXPECTF--
string(34) "Invalid promise in promises string"
