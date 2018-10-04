--TEST--
pledge() function - increase permissions with pledge()
--FILE--
<?php
pledge('stdio');
try {
    pledge('inet stdio');
} catch (\PledgeException $e) {
    var_dump($e->getMessage());
    var_dump($e->getCode());
}
?>
--EXPECT--
string(31) "Attempt to increase permissions"
int(1)
