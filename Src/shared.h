
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

/**
 * A "pickled" Variant where a string is written in memory just after the `type`
 * field.
 */
typedef struct PackedVariant {
    // Length of the variable-length value field
    int64_t len;

    // The variant type
    int64_t type;

    // The value of the variant, which can be data of any arbitrary size
    union {
        uint32_t intVal; // 4-byte width value, like an integer
        uint64_t llVal; // 8-byte width value, like a long long
        short wstrVal; // variable width value, like a string
    };
} PackedVariant;

/**
 * Convert a VARIANT to a PackedVariant
 */
PackedVariant* PackVariant(VARIANT vt) {
    int64_t len = 8;
    if (vt.vt == VT_BSTR) {
        len = wcslen(vt.bstrVal) * 2 + 2;
    }
    PackedVariant* packed = (PackedVariant*)malloc(sizeof(PackedVariant) + len);
    memset(packed, 0, sizeof(PackedVariant) + len);
    packed->len = len;
    packed->type = vt.vt;
    if (vt.vt == VT_BSTR) {
        memcpy(&packed->intVal, vt.bstrVal, len);
    } else if (vt.vt == VT_I4 || vt.vt == VT_R4) {
        packed->intVal = vt.intVal;
    } else if (vt.vt == VT_I8 || vt.vt == VT_R8) {
        packed->llVal = vt.llVal;
    } else if (vt.vt == VT_DISPATCH) {
        packed->llVal = vt.llVal;
        vt.pdispVal->lpVtbl->AddRef(vt.pdispVal);
    } else {
        packed->llVal = vt.llVal;
    }
    return packed;
}

/**
 * Convert a PackedVariant to a VARIANT (that must be freed)
 */
VARIANT UnpackVariant(PackedVariant* pv) {
    VARIANT vt;
    vt.vt = pv->type;
    if (vt.vt == VT_BSTR)
        vt.bstrVal = SysAllocString(&pv->wstrVal);
    else
        vt.llVal = pv->llVal;
    if (vt.vt == VT_DISPATCH)
        vt.pdispVal->lpVtbl->AddRef(vt.pdispVal);
    return vt;
}

#endif
