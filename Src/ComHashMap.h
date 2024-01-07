
#ifndef COM_HASH_MAP
#define COM_HASH_MAP

#include "shared.h"

typedef struct HashItem { PackedVariant* key; PackedVariant* val; } HashItem;

typedef struct ComHashMap ComHashMap;

// Implements IDispatch
typedef struct ComHashMapVtbl {
    __stdcall HRESULT(*QueryInterface) (ComHashMap *This, REFIID riid, void **ppvObject);
    __stdcall ULONG(*AddRef) (ComHashMap *This);
    __stdcall ULONG(*Release) (ComHashMap *This);
    __stdcall HRESULT(*GetTypeInfoCount) (ComHashMap *This, UINT *pctinfo);
    __stdcall HRESULT(*GetTypeInfo) (ComHashMap * This, UINT iTInfo, LCID lcid, void **ppTInfo);
    __stdcall HRESULT(*GetIDsOfNames) (
        ComHashMap *This,
        REFIID riid,
        LPOLESTR *rgszNames,
        UINT cNames,
        LCID lcid,
        DISPID *rgDispId
    );
    __stdcall HRESULT(*Invoke) (
        ComHashMap *This,
        DISPID dispIdMember,
        REFIID riid,
        LCID lcid,
        WORD wFlags,
        DISPPARAMS *pDispParams,
        VARIANT *pVarResult,
        EXCEPINFO *pExcepInfo,
        UINT *puArgErr
    );
} ComHashMapVtbl;

struct ComHashMap {
    const ComHashMapVtbl* lpVtbl;
    int refcount;
    HashItem* items;
};

ComHashMap* NewComHashMap();

#endif