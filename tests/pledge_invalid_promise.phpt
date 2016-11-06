--TEST--
pledge() function - invalid promise for pledge()
--FILE--
<?php
try {
    pledge('no_such_promise');
} catch (\PledgeException $e) {
    var_dump($e->getMessage());
    var_dump($e->getCode());
}
?>
--EXPECTF--
string(34) "Invalid promise in promises string"
int(22)
