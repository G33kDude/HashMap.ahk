
#include "ComHashMapEnum.h"
#include "stb_ds.h"

/**
 * Returns the IDispatch interface of the object when requested
 *
 * https://learn.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-queryinterface(refiid_void)
 */
HRESULT __stdcall chme_QueryInterface(ComHashMapEnum *this, REFIID riid, void** ppvObject) {
    DebugStrPtr(L"CHME QueryInterface", riid->Data1);
    if (riid->Data1 == 0x00020400) {
        this->lpVtbl->AddRef(this);
        *ppvObject = this;
        return S_OK;
    }
    return E_NOINTERFACE;
}

/**
 * Increments the reference counter
 *
 * https://learn.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-addref
 */
ULONG __stdcall chme_AddRef(ComHashMapEnum *this) {
    DebugStrPtr(L"AddRef", ++this->refcount);
    return this->refcount;
}

/**
 * Decrements the reference counter and frees the object when it hits 0
 *
 * https://learn.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-release
 */
ULONG __stdcall chme_Release(ComHashMapEnum *this) {
    DebugStrPtr(L"Release", --this->refcount);
    // if (this->refcount == 0) {
    //     this->object->lpVtbl->Release(this->object);
    //     return 0;
    // }
    return this->refcount;
}

/**
 * Allows debug-style inspection of the COM object
 * 
 * https://learn.microsoft.com/en-us/windows/win32/api/oaidl/nf-oaidl-idispatch-gettypeinfocount
 */
HRESULT __stdcall chme_GetTypeInfoCount(ComHashMapEnum *this, UINT* pctinfo) {
    DebugStr(L"GetTypeInfoCount");
    return E_NOTIMPL;
}

/**
 * Allows debug-style inspection of the COM object
 *
 * https://learn.microsoft.com/en-us/windows/win32/api/oaidl/nf-oaidl-idispatch-gettypeinfo
 */
HRESULT __stdcall chme_GetTypeInfo(
    ComHashMapEnum *this,
    /* [in] */ UINT iTInfo,
    /* [in] */ LCID lcid,
    // /* [out] */ ITypeInfo **ppTInfo);
    /* [out] */ void **ppTInfo) {
    DebugStr(L"GetTypeInfo");
    return E_NOTIMPL;
}

/**
 * Translates script-given method and property names to unique DISPID values.
 *
 * https://learn.microsoft.com/en-us/windows/win32/api/oaidl/nf-oaidl-idispatch-getidsofnames
 */
HRESULT __stdcall chme_GetIDsOfNames(
    ComHashMapEnum *this,
    /* [in] */ REFIID riid,
    /* [in] */ LPOLESTR *rgszNames,
    /* [in] */ UINT cNames,
    /* [in] */ LCID lcid,
    /* [out] */ DISPID *rgDispId) {
    HRESULT retval = S_OK;
    for (int i = 0; i < cNames; i++) {
        retval = DISP_E_UNKNOWNNAME;
        rgDispId[i] = DISPID_UNKNOWN;
    }
    return retval;
}

/**
 * Dispatches ComHashMapEnum's method and property implementations
 *
 * https://learn.microsoft.com/en-us/windows/win32/api/oaidl/nf-oaidl-idispatch-invoke
 */
HRESULT __stdcall chme_Invoke(
    ComHashMapEnum *this,
    /* [in] */ DISPID dispIdMember,
    /* [in] */ REFIID riid,
    /* [in] */ LCID lcid,
    /* [in] */ WORD wFlags,
    /* [out][in] */ DISPPARAMS *pDispParams,
    /* [out][opt] */ VARIANT *pVarResult,
    /* [out][opt] */ EXCEPINFO *pExcepInfo,
    /* [out][opt] */ UINT *puArgErr) {
    DebugStr(L"CHME Invoke");
    DebugStrPtr(L"CHME cArgs", pDispParams->cArgs);

    if (dispIdMember == DISPID_VALUE && wFlags & DISPATCH_METHOD) {
        DebugStrPtr(L"CHME VT1", pDispParams->rgvarg[1].vt);
        DebugStrPtr(L"CHME VT0", pDispParams->rgvarg[0].vt);
        DebugStrPtr(L"CHME VT TARGET", VT_VARIANT | VT_BYREF);
        DebugStrPtr(L"CHME hmlen", stbds_hmlen(this->object->items));

        if (pDispParams->cArgs != 2) {
            return DISP_E_BADPARAMCOUNT;
        }

        if (
            pDispParams->rgvarg[1].vt != (VT_VARIANT | VT_BYREF) ||
            pDispParams->rgvarg[0].vt != (VT_VARIANT | VT_BYREF)
        ) {
            return DISP_E_BADVARTYPE;
        }

        if (this->index >= stbds_hmlen(this->object->items)) {
            // return 0
            pVarResult->vt = VT_I4;
            pVarResult->intVal = 0;
            return S_OK;
        }

        // Retrieve the HashMap item
        HashItem item = this->object->items[this->index];

        VARIANT vtKey = UnpackVariant(item.key);
        pDispParams->rgvarg[1].pvarVal->vt = vtKey.vt;
        pDispParams->rgvarg[1].pvarVal->llVal = vtKey.llVal;

        VARIANT vtVal = UnpackVariant(item.val);
        pDispParams->rgvarg[0].pvarVal->vt = vtVal.vt;
        pDispParams->rgvarg[0].pvarVal->llVal = vtVal.llVal;

        this->index++;

        pVarResult->vt = VT_I4;
        pVarResult->intVal = 1;
        return S_OK;
    }

    DebugStrPtr(L"CHME Unknown DispIdMember: ", dispIdMember);

    return DISP_E_MEMBERNOTFOUND;
}

// The IDispatch VTable populated with MyObject's implementation
static const ComHashMapEnumVtbl COM_HASH_MAP_ENUM_VTBL = {
    .QueryInterface = chme_QueryInterface,
    .AddRef = chme_AddRef,
    .Release = chme_Release,
    .GetTypeInfoCount = chme_GetTypeInfoCount,
    .GetTypeInfo = chme_GetTypeInfo,
    .GetIDsOfNames = chme_GetIDsOfNames,
    .Invoke = chme_Invoke,
};

ComHashMapEnum* NewComHashMapEnum(ComHashMap* chm) {
    ComHashMapEnum* this = (ComHashMapEnum*)malloc(sizeof(ComHashMapEnum));

    this->lpVtbl = &COM_HASH_MAP_ENUM_VTBL;
    this->refcount = 1;
    this->object = chm;
    chm->lpVtbl->AddRef(chm);
    this->index = 0;

    return this;
}
