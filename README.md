# HashMap.ahk

This library provides a hash map based alternative implementation of
AutoHotkey's Map class, with the goal of allowing drop-in replacement for
situations where the native Map's performance limitations have been reached.

## Why

AutoHotkey's native Maps, as well as its basic Objects, use sorted arrays and
binary search as their item lookup strategy. When dealing with only a few items,
such as into the thousands, that approach works fine for almost any purpose.
However, there comes a point where having to re-arrange/re-sort the dense array
and perform logarithmically more searching steps can slow down data operations
noticeably. This can often be seen when loading large amounts of JSON data, or
when dumping a lot of SQL data into memory.

Hash maps have better performance characteristics at the large scale, because
they do not require the entire array of items to be re-arranged whenever a new
item is inserted, and because they perform searches linearly with respect to the
key size rather than logarithmically with respect to the Map size. They work by
putting each key through a hashing function that converts it into a predictable
number between 0 and the length of the items array. Once it has that number,
it can jump directly to the item without performing any search steps. The items
array is only re-arranged when the quantity of items gets too large for how much
space was originally allocated for them. In this implementation, deleting items
can also lead to a partial re-arrangement.

## How

This library uses machine code to construct objects compatible with the COM
IDispatch standard, so that they may be used directly by AutoHotkey without the
added scripting complexity of foreign function interface code like DllCall.

The objects provide standard Map methods, but implement them by passing the keys
and values as COM VARIANTs into a generic hash map implementation. The hash map
implementation has had its hashing and equality functions overridden to provide
unique hashes of VARIANT objects of different types, and to allow string
arguments to be hashed and compared by value rather than by reference. The
hashing function is a variant on [FNV-1a](
https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function)
which has been modified to accept a seed value. Arguments are staged for hashing
by dereferencing appropriate VARIANT fields to form a densely packed buffer that
includes both the type and bytes representing the data in whatever format it is
best compared. For example, a string pointer will be dereferenced to the string
itself. By this method, two VARIANTs with equal strings but differing string
pointers will still hash the same, and an integer VARIANT whose bytes happen
to match up with the characters of a string will not hash the same.

All this machine code has been compiled using the MCL MCode library, so that it
may easily import a wide variety of COM functions from the Windows API, and so
that the standard C code hash map implementation may be compiled with no special
workarounds. Once compiled, plain C constructor functions for the custom COM
objects are automatically exported to AutoHotkey and get wrapped by the
`HashMap` class to convert the IDispatch pointer returned by the constructor
function into an AutoHotkey `ComObject` wrapper, in order that it may be used
like any other object.

This approach is in stark contrast to the approach used by previous hash map
implementations, which have either used mostly pure AHK code or used MCode that
exposes a C API to be thickly wrapped with layers of functions and DllCalls
which introduce a lot of overhead. Although to my knowledge it has not yet been
done, this library also aims to be more stable than one written using the
[Native.ahk](https://www.autohotkey.com/boards/viewtopic.php?t=100197) library
by the infamous @thqby. Native.ahk attempts to generate objects from machine
code that implements AutoHotkey's internal and officialy unstable APIs, so it
can be broken easily by any updates made to the AutoHotkey interpreter. At best
it just does not work, but at worst this can lead to corruption of internal data
structures which may not be revealed right away. Because this library's object
integration is COM based in nature, it is unaffected by the changing of internal
APIs but instead *should* be portable across AHK versions (and even non-AHK
languages) so long as the MCode can be loaded into memory.

## Who

This library builds on the work of many authors and people who have come before.

* @G33kDude who has built this library
* @CloakerSmoker who co-authored the MCL library, without whom this C code
  could almost never have been compiled, let alone efficiently compiled.
* @nothings (Sean Barrett) and the Public Domain Data Structures project
  contributors, who engineered the majority of the hash map implementation.
* Uncountable other AHKers who have worked to teach new users and provide
  previous iterations of tooling.

And an honorable mention to @HelgeffegleH who built the original [hashTable](
https://github.com/HelgeffegleH/hashTable) implementation for AutoHotkey. This
project takes no code or inspiration from Helgef's project. We simply appreciate
that Helgef beat us to the punch by several years.

## Compatibility Notes

This library supports 32- and 64-bit AutoHotkey v2.0, and makes no assurances
about compatibility with future releases of AutoHotkey. However, it has been
designed with future compatibility in mind.

Still left to implemented:
* `CaseSense`

Differences from native Maps:

* You cannot set OwnProps on the HashMap. This may be supported in future
  releases, however it is not currently on the roadmap.
* The HashMap supports Float keys directly, rather than implicitly casting them
  to String keys. This may be configurable in future releases.
* There is no `__New` alias to `Set`.
* There is no `Capacity` property, as the backing hash map implementation does
  not directly expose this information, and even if it did it does not allow
  direct manipulation of the capacity.
* `CaseSense` will not support Locale mode.
* `Default` cannot be a dynamic property or determined by meta-function, but
  instead must always be set by value. If allowing OwnProps to be set on the
  HashMap makes it onto the roadmap, some support for dynamic properties may
  be considered if only for the `Default` OwnProp.
* In AHK v2.0 using a standard Map, calling `Set` to set an item key or value
  to `unset` causes that assignment to be ignored, while other assignments in
  the same parameter list may be accepted. Trying to set an item value to
  `unset` will cause AutoHotkey to raise a missing parameter exception. However,
  the HashMap treats `unset` more like a null value, and will gladly accept
  setting a value to `unset`. That `unset` will be passed back to AHK when that
  key is asked for, rather than raising an exception that the item has no value.
  When AHK v2.1 becomes stable and the allowed usage of `unset` in native AHK is
  expanded, this library will likely be modified to behave more in line with
  however v2.1 handles these scenarios.

## Static Methods

### Call

Creates a HashMap.

```ahk
HashMapObject := HashMap()
```

## Methods

### Clear

Removes all key-value pairs from a map.

```ahk
HashMapObj.Clear()
```

### Clone

Returns a shallow copy of a map.

```ahk
Clone := HashMapObj.Clone()
```

All key-value pairs are copied to the new map. Object references are copied
(like with a normal assignment), not the objects themselves.

### Delete

Removes a key-value pair from a map.

```ahk
RemovedValue := HashMapObj.Delete(Key)
```

#### Parameters

**Key**

> Type: Integer, Object, String, or Float
>
> Any single key. If the map does not contain this key, an error is thrown.

#### Return Value

Type: Any

This method returns the removed value.

### Get

Returns the value associated with a key, or a default value.

```ahk
Value := HashMapObj.Get(Key [, Default])
```

This method does the following:

* Return the value associated with *Key*, if found.
* Return the value of the *Default* parameter, if specified.
* Return the value of `MapObj.Default`, if defined.
* Throw an error.

When *Default* is omitted, this is equivalent to `MapObj[Key]`.

### Has

Returns true if the specified key has an associated value within a map,
otherwise false.

```ahk
HashMapObj.Has(Key)
```

### Set

Sets zero or more items.

```ahk
HashMapObj.Set([Key, Value, Key2, Value2, ...])
```

This is equivalent to setting each item with `HashMapObj[Key] := Value`.

#### Return Value

Type: Object

This method returns the HashMap.

### __Enum

Enumerates key-value pairs.

```ahk
For Key [, Value] in HashMapObj
```

Returns a new enumerator. This method is typically not called directly. Instead,
the map object is passed directly to a for-loop, which calls __Enum once and
then calls the enumerator once for each iteration of the loop. Each call to the
enumerator returns the next key and/or value. The for-loop's variables
correspond to the enumerator's parameters, which are:

## Properties

### Count

Retrieves the number of key-value pairs present in a map.

```ahk
Count := HashMapObj.Count
```

### CaseSense

Retrieves or sets a map's case sensitivity setting.

```ahk
CurrentSetting := HashMapObj.CaseSense
```

```ahk
HashMapObj.CaseSense := NewSetting
```

*CurrentSetting* is *NewSetting* if assigned, otherwise On by default (but note
that this property only retrieves the string variant of the current setting).

*NewSetting* is one of the following strings or integers (boolean):

**On** or **1** (true): Key lookups are case-sensitive. This is the default
setting.

**Off** or **0** (false): Key lookups are not case-sensitive, i.e. the letters
A-Z are considered identical to their lowercase counterparts.

Attempting to assign to this property causes an exception to be thrown if the
Map is not empty.

### Default

Defines the default value returned when a key is not found.

```ahk
MapObj.Default := Value
```

To clear the default, set it to `unset`.

```ahk
MapObj.Default := unset
```

### __Item

Retrieves or sets the value of a key-value pair.

```ahk
Value := MapObj[Key]
Value := MapObj.__Item[Key] ; This variant is not supported
```

```ahk
MapObj[Key] := Value
MapObj.__Item[Key] := Value
```