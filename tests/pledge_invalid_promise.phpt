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
--EXPECT--
string(50) "promises is malformed or contains invalid keywords"
int(22)
