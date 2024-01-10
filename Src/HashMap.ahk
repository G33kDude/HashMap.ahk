#Requires AutoHotkey v2.0
#Include Lib\MCL.ahk\MCL.ahk

class HashMap {
    static version := "1.0.0-git-dev"

    static MCode := this._Load()

    static _Load() {
        try {
            SetWorkingDir A_LineFile "\.."
            lib := MCL.FromC(FileRead("HashMap.c"), { flags: '-D AHK_DEBUG_BUILD' })
        } catch (Error as e) {
            g := Gui()
            g.AddEdit("readonly r20 w640 h480", e.message)
            g.Show()
            closed := false
            g.OnEvent("close", (*) => closed := true)
            while !closed
                Sleep 100
            ExitApp
        }

        Print(text) {
            static console := DllCall("AllocConsole")
            FileOpen("CONOUT$", "w", "UTF-8-RAW").Write(text "`n")
        }

        lib.DebugStr := CallbackCreate((a) => Print(StrGet(a)), "Cdecl")
        lib.DebugPtr := CallbackCreate((a) => Print(Format("{:08x}", a)), "Cdecl")
        lib.DebugStrPtr := CallbackCreate((a, b) => Print(StrGet(a) " " Format("{:08x}", b)), "Cdecl")

        return lib
    }

    static Call(p*) => ComObjFromPtr(this.MCode.NewComHashMap()).Set(p*)
}
