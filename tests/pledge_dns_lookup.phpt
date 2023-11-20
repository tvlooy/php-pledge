--TEST--
pledge() function - dns lookup via pledge(), also test "error" pledge
--FILE--
<?php
// will work
gethostbyname('openbsd.org');
var_dump(pledge('stdio error dns'));
// will work
gethostbyname('openbsd.org');
// the call will fail but process will continue
var_dump(pledge('stdio error'));
echo gethostbyname('openbsd.org');
// the call will fail and process will be killed
var_dump(pledge('stdio'));
gethostbyname('openbsd.org');
?>
--EXPECT--
bool(true)
bool(true)
openbsd.orgbool(true)
Abort trap (core dumped) 

Termsig=6
