# HashMap.ahk

You can't set OwnProps on the HashMap.

The HashMap supports actual Float keys.

There is no __New.

There is no Capacity property.

CaseSense does not support Locale mode.

Default cannot be a dynamic property or determined by meta-function.

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

To clear the default, 

```ahk
MapObj.Default :=
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