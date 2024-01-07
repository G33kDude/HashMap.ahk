#Requires AutoHotkey v2.0

; Include Yunit files
#include Lib\Yunit\Yunit.ahk
#include Lib\Yunit\Window.ahk

; Include dev HashMap
#Include ..\Src\HashMap.ahk

; Include test suites
#Include TestSuite.ahk

Yunit.Use(YunitWindow).Test(TestSuite)