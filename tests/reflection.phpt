--TEST--
test reflection of pledge() and unveil() functions
--FILE--
<?php
$r = new ReflectionFunction('pledge');
$p = $r->getParameters();
var_dump(
    (string) $r->getReturnType(),
    $p,
    (string) $p[0]->getType(),
    (string) $p[1]->getType()
);

$r = new ReflectionFunction('unveil');
$p = $r->getParameters();
var_dump(
    (string) $r->getReturnType(),
    $p,
    (string) $p[0]->getType(),
    (string) $p[1]->getType()
);
?>
--EXPECT--
string(4) "bool"
array(2) {
  [0]=>
  object(ReflectionParameter)#2 (1) {
    ["name"]=>
    string(8) "promises"
  }
  [1]=>
  object(ReflectionParameter)#3 (1) {
    ["name"]=>
    string(12) "execpromises"
  }
}
string(7) "?string"
string(7) "?string"
string(4) "bool"
array(2) {
  [0]=>
  object(ReflectionParameter)#1 (1) {
    ["name"]=>
    string(4) "path"
  }
  [1]=>
  object(ReflectionParameter)#5 (1) {
    ["name"]=>
    string(11) "permissions"
  }
}
string(7) "?string"
string(7) "?string"
