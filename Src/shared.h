
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
// #define AHK_DEBUG_BUILD

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

#if MCL_BITNESS == 64
#define FNV_PRIME 1099511628211u
#define FNV_OFFSET_BASIS 14695981039346656037u
#else
#define FNV_PRIME 16777619u
#define FNV_OFFSET_BASIS 2166136261u
#endif

size_t fnv1a(void* data, size_t len, size_t seed) {
  size_t hash = FNV_OFFSET_BASIS;
  hash ^= seed;
  for (size_t i = 0; i < len; i++) {
    hash *= FNV_PRIME;
    hash ^= ((char*)data)[i];
  }
  return hash;
}

#define STBDS_HASH_BYTES_CUSTOM
size_t stbds_hash_bytes(void* p, size_t len, size_t seed) {
  VARIANT vt = *(VARIANT*)p;
  UINT cbBuf = 0;
  VARTYPE* buf;
  if (vt.vt == VT_BSTR) {
    // Hash type + string length (including null terminator)
    cbBuf = SysStringLen(vt.bstrVal) + 2;
    buf = (VARTYPE*)malloc(sizeof(VARTYPE) + cbBuf);
    buf[0] = vt.vt;
    memcpy(&buf[1], vt.bstrVal, cbBuf);
  } else if (vt.vt == VT_I4 || vt.vt == VT_R4) {
    // Hash type + 4 bytes
    cbBuf = sizeof(uint32_t);
    buf =(VARTYPE*) malloc(sizeof(VARTYPE) + cbBuf);
    buf[0] = vt.vt;
    memcpy(&buf[1], &vt.uintVal, cbBuf);
  } else if (vt.vt == VT_DISPATCH) {
    // Hash type + pointer
    cbBuf = sizeof(PVOID);
    buf =(VARTYPE*) malloc(sizeof(VARIANT*) + cbBuf);
    buf[0] = vt.vt;
    memcpy(&buf[1], &vt.pvarVal, cbBuf);
  } else { // if (vt.vt == VT_I8 || vt.vt == VT_R8) {
    // Hash type + 8 bytes
    cbBuf = sizeof(uint64_t);
    buf =(VARTYPE*) malloc(sizeof(VARTYPE) + cbBuf);
    buf[0] = vt.vt;
    memcpy(&buf[1], &vt.ullVal, cbBuf);
  }
  size_t hash = fnv1a(buf, 2 + cbBuf, seed);
  DebugStrPtr(L"hash", hash);
  return hash;
}

#define STBDS_IS_KEY_EQUAL_CUSTOM
static int stbds_is_key_equal(void *a, size_t elemsize, void *key, size_t keysize, size_t keyoffset, int mode, size_t i) {
  VARIANT vtA = *(VARIANT*)((char*)a + elemsize * i + keyoffset);
  VARIANT vtKey = *(VARIANT*)key;

  if (vtA.vt == VT_DISPATCH && vtKey.vt == VT_DISPATCH) {
    return vtA.pdispVal == vtKey.pdispVal;
  }

  return 1 == VarCmp(&vtA, &vtKey, 0, 0);
}

#endif
