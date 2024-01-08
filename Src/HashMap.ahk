#Requires AutoHotkey v2.0
#Include Lib\MCL.ahk\MCL.ahk

class HashMap {
    static __New() {
        try {
            SetWorkingDir A_LineFile "\.."
            this.lib := MCL.FromC(FileRead("HashMap.c"))
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

        this.lib.DebugStr := CallbackCreate((a) => Print(StrGet(a)), "Cdecl")
        this.lib.DebugPtr := CallbackCreate((a) => Print(Format("{:08x}", a)), "Cdecl")
        this.lib.DebugStrPtr := CallbackCreate((a, b) => Print(StrGet(a) " " Format("{:08x}", b)), "Cdecl")
    }

    static Call(p*) => ComObjFromPtr(this.lib.NewComHashMap()).Set(p*)
}
