--TEST--
pledge() function - increase permissions with pledge()
--FILE--
<?php
pledge('stdio');
try {
    pledge('inet stdio');
} catch (\Exception $e) {
    var_dump($e->getMessage());
}
?>
--EXPECTF--
string(31) "attempt to increase permissions"
