
#ifndef COM_HASH_MAP_ENUM
#define COM_HASH_MAP_ENUM

#include "shared.h"
#include "ComHashMap.h"

typedef struct ComHashMapEnum ComHashMapEnum;

// Implements IDispatch
typedef struct ComHashMapEnumVtbl {
    __stdcall HRESULT(*QueryInterface) (ComHashMapEnum *This, REFIID riid, void **ppvObject);
    __stdcall ULONG(*AddRef) (ComHashMapEnum *This);
    __stdcall ULONG(*Release) (ComHashMapEnum *This);
    __stdcall HRESULT(*GetTypeInfoCount) (ComHashMapEnum *This, UINT *pctinfo);
    __stdcall HRESULT(*GetTypeInfo) (ComHashMapEnum * This, UINT iTInfo, LCID lcid, void **ppTInfo);
    __stdcall HRESULT(*GetIDsOfNames) (
        ComHashMapEnum *This,
        REFIID riid,
        LPOLESTR *rgszNames,
        UINT cNames,
        LCID lcid,
        DISPID *rgDispId
    );
    __stdcall HRESULT(*Invoke) (
        ComHashMapEnum *This,
        DISPID dispIdMember,
        REFIID riid,
        LCID lcid,
        WORD wFlags,
        DISPPARAMS *pDispParams,
        VARIANT *pVarResult,
        EXCEPINFO *pExcepInfo,
        UINT *puArgErr
    );
} ComHashMapEnumVtbl;

struct ComHashMapEnum {
    const ComHashMapEnumVtbl* lpVtbl;
    int refcount;
    ComHashMap* object;
    int index;
};

ComHashMapEnum* NewComHashMapEnum(ComHashMap* chm);

#endif