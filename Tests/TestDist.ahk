#Requires AutoHotkey v2.0
#SingleInstance Off

; Include Yunit files
#include Lib\Yunit\Yunit.ahk
#include Lib\Yunit\Window.ahk

; Include dist HashMap
#Include ..\Dist\HashMap.ahk

; Include test suites
#Include TestSuite.ahk

Yunit.Use(YunitWindow).Test(TestSuite)