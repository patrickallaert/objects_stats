objects_stats
=============

Retrieve PHP object stats using get_objects_count()

Usage examples:
---------------

If no argument is provided, the number of all objects in the object store as an associative array ('class name' ⇒ count) will be returned.
```php
print_r (get_objects_count());
// Array ()
 
$foo = new stdClass;
 
print_r (get_objects_count());
// Array ( [stdClass] => 1 )
 
$bar = new stdClass;
 
print_r (get_objects_count());
// Array ( [stdClass] => 2 )
 
$bar = null;
 
print_r (get_objects_count());
// Array ( [stdClass] => 1 )
 
$foo = null;
 
print_r (get_objects_count());
// Array ()
```
If a class name is provided, the number of objects of the specified class in the object store will be returned.
```php
print get_objects_count('stdClass');
// 0
 
$foo = new stdClass;
 
print get_objects_count('stdClass');
// 1
 
$bar = new stdClass;
 
print get_objects_count('stdClass');
// 2
 
$bar = null;
 
print get_objects_count('stdClass');
// 1
 
$foo = null;
 
print get_objects_count('stdClass');
// 0
```
If an object is provided, the number of objects of the specifiied objects class in the object store will be returned. The return value is always ≥ 1.
```php
$foo = new stdClass;
 
print get_objects_count($foo);
// 1
 
$bar = new stdClass;
 
print get_objects_count($bar);
// 2
 
$bar = null;
 
print get_objects_count($foo);
// 1
```
If an an array is provided, it will be the treated as an inclusive indexed array of class names. An associative array ('class name' ⇒ count) will be returned.
```php
print_r (get_objects_count(array('stdClass')));
// Array ( [stdClass] => 0 )
 
$foo = new stdClass;
 
print_r (get_objects_count(array('stdClass')));
// Array ( [stdClass] => 1 )
 
$bar = new stdClass;
 
print_r (get_objects_count(array('stdClass')));
// Array ( [stdClass] => 2 )
 
$bar = null;
 
print_r (get_objects_count(array('stdClass')));
// Array ( [stdClass] => 1 )
 
$foo = null;
 
print_r (get_objects_count(array('stdClass')));
// Array ( [stdClass] => 0 )
```
