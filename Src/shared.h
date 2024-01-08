
#ifndef AHK_SHARED
#define AHK_SHARED

#include <mcl.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <oaidl.h>

MCL_IMPORT(int, msvcrt, _wcsicmp, (const short*, const short*));
MCL_IMPORT(int, msvcrt, wcslen, (const short*));
MCL_IMPORT(BSTR, OleAut32, SysAllocString, (short*));
MCL_IMPORT(UINT, OleAut32, SysStringLen, (BSTR));
MCL_IMPORT(HRESULT, OleAut32, VarCmp, (VARIANT*, VARIANT*, LCID, ULONG));
MCL_IMPORT(HRESULT, OleAut32, VariantCopy, (VARIANT*, const VARIANT*));
MCL_IMPORT(HRESULT, OleAut32, VariantClear, (VARIANT*));

#define AS_IDISPATCH(p) ((IDispatch*)(void*)p)

// Uncomment to generate debug build
#define AHK_DEBUG_BUILD

#ifdef AHK_DEBUG_BUILD
MCL_EXPORT_GLOBAL(DebugStr, Ptr)
void (*DebugStr)(short*);
MCL_EXPORT_GLOBAL(DebugPtr, Ptr)
void (*DebugPtr)(void*);
MCL_EXPORT_GLOBAL(DebugStrPtr, Ptr)
void (*DebugStrPtr)(short*, void*);
#else
#define DebugStr(a)
#define DebugPtr(a)
#define DebugStrPtr(a, b)
#endif

#endif
