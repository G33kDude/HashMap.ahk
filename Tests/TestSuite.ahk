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

    End() {
    }
}
