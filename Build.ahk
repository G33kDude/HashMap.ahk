#Requires AutoHotkey v2.0
#Include Src\Lib\MCL.ahk\MCL.ahk

SetWorkingDir A_LineFile '\..\Src'

; Pull in the AHK file
ahk := FileRead('HashMap.ahk')

; Compile the C code and generate a standalone loader
mcode := MCL.StandaloneAHKFromC(
    FileRead('HashMap.c'),
    { flags: '-O3 -Werror' },
    { name: 'static _Load' }
)

; Indent the generated standalone loader
mcode := RegExReplace(StrReplace(mcode, '`t', '    '), 'm)^', '    ')

; Replace the inline compilation with the standalone loader
ahk := RegExReplace(ahk, 'sm)^    static _Load\(\).+?\R    }\R', mcode)

; Remove the MCL include
ahk := RegExReplace(ahk, 'm)^#Include.+MCL.ahk$', '')

; Update the version string to indicate it's a built version
ahk := RegExReplace(ahk, 'm)^    static version.+\K-dev', '-built')
RegExMatch(ahk, 'm)^    static version := "\K[^"]+', &version) ; Extract version

; Prepend the LICENSE
license := StrReplace(Trim(FileRead('..\LICENSE'), ' `t`r`n'), '`r', '')
RegExMatch(license, "m)^Copyright \(c\).+", &copyright) ; Extract copyright
license := RegExReplace(license, 'm)^Copyright \(c\).+\s+', '') ; Remove copyright
license := RegExReplace(license, 'm)^', '; ') ; Prepend comments
license := RegExReplace(license, 'm)[ \t]+$', '') ; Remove trailing whitespace
ahk := (
';`n'
'; HashMap.ahk ' version[0] '`n'
'; ' copyright[0] ' (known also as GeekDude, G33kDude)`n'
'; https://github.com/G33kDude/HashMap.ahk`n'
';`n'
license '`n'
';`n'
ahk
)

; Normalize line endings
ahk := RegExReplace(ahk, '`a)\R', '`r`n')

; Save to the Dist folder
DirCreate '..\Dist'
FileOpen('..\Dist\HashMap.ahk', 'w').Write(ahk)

; Test the build
Run A_AhkPath '\..\AutoHotkey64.exe ..\Tests\TestDist.ahk'
Run A_AhkPath '\..\AutoHotkey32.exe ..\Tests\TestDist.ahk'