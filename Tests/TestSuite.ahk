#Requires AutoHotkey v2.0

class TestSuite {
    Begin() {
        this.message := "Expected {} but produced {}"
    }

    IsObject() {
        hm := HashMap()
        Yunit.Assert(IsObject(hm), "HashMap is not an object")
    }

    IsComObject() {
        hm := HashMap()
        Yunit.Assert(hm is ComObject, "HashMap is not a COM object")
    }

    SetGetIntInt() {
        hm := HashMap()
        hm.Set(42, 42)
        result := hm.Get(42)
        Yunit.Assert(result == 42, "Did not retrieve same int!")
    }

    SetGetStrStr() {
        hm := HashMap()
        hm.Set("42", "42")
        result := hm.Get("42")
        Yunit.Assert(result == "42", "Did not retrieve same str!")
    }

    SetGetFloatFloat() {
        hm := HashMap()
        hm.Set(0.1, 0.1)
        result := hm.Get(0.1)
        Yunit.Assert(result == 0.1, "Did not retrieve same str!")
    }

    SetGetObjObj() {
        o := []
        hm := HashMap()
        hm.Set(o, o)
        result := hm.Get(o)
        Yunit.Assert(result == o, "Did not retrieve same obj!")
    }

    SetOverwriteRefCount() {
        o := []
        hm := HashMap()

        countStart := ObjRelease(ObjPtrAddRef(o))
        hm.Set(1, o)
        countSet := ObjRelease(ObjPtrAddRef(o))
        hm.Set(1, 2)
        countEnd := ObjRelease(ObjPtrAddRef(o))

        Yunit.Assert(countSet > countStart, "Clone messed up the ref count")
        Yunit.Assert(countStart == countEnd, "Clone messed up the ref count")
    }

    Count() {
        hm := HashMap()
        hm.Set(1, 2)
        result := hm.Count
        Yunit.Assert(result == 1, "Count is not correct!")
    }

    SetRejectsWrongParamCount() {
        hm := HashMap()
        try {
            hm.Set()
            Yunit.Assert(false, "Set did not reject wrong param count!")
        }

        try {
            hm.Set(1)
            Yunit.Assert(false, "Set did not reject wrong param count!")
        }

        try {
            hm.Set(1, 2)
        } catch {
            Yunit.Assert(false, "Set rejected correct param count!")
        }

        try {
            hm.Set(1, 2, 3)
            Yunit.Assert(false, "Set did not reject wrong param count!")
        }
    }

    GetRejectsWrongParamCount() {
        hm := HashMap()
        hm.Set(1, 2)

        try {
            hm.Get()
            Yunit.Assert(false, "Get did not reject wrong param count!")
        }

        try {
            hm.Get(1)
        } catch {
            Yunit.Assert(false, "Get rejected correct param count!")
        }

        try {
            hm.Get(1, 2)
            Yunit.Assert(false, "Get did not reject wrong param count!")
        }
    }

    IterateDouble() {
        hm := HashMap()
        hm.Set(1, 2)
        hm.Set("a", "b")
        hm.Set(1.0, 2.0)
        out := ","
        for k, v in hm
            out .= k ":" v ","
        Yunit.Assert(
            InStr(out, ",1:2,") && InStr(out, ",a:b,") && InStr(out, ",1.0:2.0,"),
            "Did not iterate correct!"
        )
    }

    IterateSingle() {
        hm := HashMap()
        hm.Set(1, 2)
        hm.Set("a", "b")
        hm.Set(1.0, 2.0)
        out := ","
        for k in hm
            out .= k ","
        Yunit.Assert(
            InStr(out, ",1,") && InStr(out, ",a,") && InStr(out, ",1.0,"),
            "Did not iterate correctly!"
        )
    }

    SetByItem() {
        hm := HashMap()
        hm['alpha'] := 'beta' ; Please don't crash
    }

    GetByItem() {
        hm := HashMap()
        hm.Set('alpha', 'beta')
        result := hm['alpha']
        Yunit.Assert(result == 'beta', 'Did not retrieve same str!')
    }

    GetUnset() {
        hm := HashMap()
        try {
            hm.Get('alpha')
            Yunit.Assert(false, 'Get did not error on unset!')
        }
    }

    RefCount() {
        hm := HashMap()

        o := []
        countStart := ObjRelease(ObjPtrAddRef(o))

        hm.Set(o, o)
        countAfterSet := ObjRelease(ObjPtrAddRef(o))
        Yunit.Assert(countAfterSet > countStart, "Ref count after set not greater than start")

        hm.Get(o)
        countAfterGet := ObjRelease(ObjPtrAddRef(o))
        Yunit.Assert(countAfterGet == countAfterSet, "Ref count after get changed!")

        hm := ""
        countAfterClear := ObjRelease(ObjPtrAddRef(o))
        Yunit.Assert(countAfterClear == countStart, "Ref count after clear wrong!")
    }

    SetMultiple() {
        hm := HashMap()
        hm.Set(1, 2, 3, 4)
        Yunit.Assert(hm.Count == 2, "Set did not set multiple items")
    }

    CreateWithValue() {
        hm := HashMap(1, 2)
        Yunit.Assert(hm.Count == 1, "Create did not set an item")
    }

    CreateWithMultiple() {
        hm := HashMap(1, 2, 3, 4)
        Yunit.Assert(hm.Count == 2, "Create did not set enough items")
    }

    DeleteKey() {
        hm := HashMap()

        o := {}
        countStart := ObjRelease(ObjPtrAddRef(o))
        hm.Set(o, o)
        countAfterSet := ObjRelease(ObjPtrAddRef(o))
        hm.Delete(o)
        countEnd := ObjRelease(ObjPtrAddRef(o))

        Yunit.assert(countAfterSet > countStart, "Reference count did not increase!")
        Yunit.assert(countStart == countEnd, "Reference count did not revert!")

        try {
            hm.Get(o)
            Yunit.assert(false, "Item not removed!")
        }
    }

    Clear() {
        hm := HashMap()

        o := {}
        countStart := ObjRelease(ObjPtrAddRef(o))

        hm.Set(o, o)
        hm.Set(1, 2)
        hm.Set('a', 'b')
        countAfterSet := ObjRelease(ObjPtrAddRef(o))

        Yunit.assert(countAfterSet > countStart, "Reference count did not increase!")
        Yunit.assert(hm.Count == 3, "There are not 3 items now!")

        hm.Clear()
        countEnd := ObjRelease(ObjPtrAddRef(o))

        Yunit.assert(countStart == countEnd, "Reference count did not revert!")
        Yunit.assert(hm.Count == 0, "There are not 0 items now!")

        try {
            hm.Get(o)
            Yunit.assert(false, "Item not removed!")
        }
    }

    GetWithDefault() {
        hm := HashMap()
        o := []
        countStart := ObjRelease(ObjPtrAddRef(o))
        result := hm.Get(1, o)
        Yunit.Assert(result == o, "Get did not get the right default")
        result := ""
        countEnd := ObjRelease(ObjPtrAddRef(o))
        Yunit.Assert(countStart == countEnd, "Get messed up the ref count")
    }

    Has() {
        hm := HashMap()
        hm.Set(1, 2)
        hm.Set(3, 4)
        Yunit.Assert(hm.Has(1), "Did not have key it should")
        Yunit.Assert(!hm.Has(2), "Had key it should not")
    }

    GetWithPresetDefault() {
        hm := HashMap()
        try {
            hm.Get(1)
            Yunit.Assert(false, "Get did not throw")
        }

        o := []
        countStart := ObjRelease(ObjPtrAddRef(o))

        hm.Default := o
        Yunit.Assert(hm.Get(1) == o, "Get did not get the right default")

        Yunit.Assert(hm.Default == o, "Default did not get the right default")

        hm.Default := unset
        try {
            hm.Get(1)
            Yunit.Assert(false, "Get did not throw")
        }

        countEnd := ObjRelease(ObjPtrAddRef(o))
        Yunit.Assert(countStart == countEnd, "Default messed up the ref count")
    }

    Clone() {
        hm := HashMap()
        o := []
        countStart := ObjRelease(ObjPtrAddRef(o))
        hm.Set(1, o)
        clone := hm.Clone()
        hm.Set(1, 2)
        Yunit.Assert(clone[1] == o, "Clone was changed by source")
        clone := ""
        countEnd := ObjRelease(ObjPtrAddRef(o))
        Yunit.Assert(countStart == countEnd, "Clone messed up the ref count")
    }

    AccessByItemProp() {
        hm := HashMap()
        hm.__Item[1] := 2
        hm.__Item[3] := 4
        Yunit.Assert(hm.__Item[1] == 2)
        Yunit.Assert(hm.__Item[3] == 4)
    }

    End() {
    }
}
