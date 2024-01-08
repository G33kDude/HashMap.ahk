
#include "ComHashMap.h"
#include "ComHashMapEnum.h"
#include "stb_ds.h"

#define CHM_DISPID_GET 1
#define CHM_DISPID_SET 2
#define CHM_DISPID_COUNT 3

/**
 * Returns the IDispatch interface of the object when requested
 *
 * https://learn.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-queryinterface(refiid_void)
 */
HRESULT __stdcall chm_QueryInterface(ComHashMap *this, REFIID riid, void** ppvObject) {
    DebugStrPtr(L"QueryInterface", riid->Data1);
    if (riid->Data1 == 0x00020400) { // TODO: Complete the GUID
        this->lpVtbl->AddRef(this);
        *ppvObject = this;
        return S_OK;
    }
    // b196b283 - IProvideClassInfo
    // 619f7e25 - IID_IObjectComCompatible Identifies an AutoHotkey object which was passed to a COM API and back again
    return E_NOINTERFACE;
}

/**
 * Increments the reference counter
 *
 * https://learn.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-addref
 */
ULONG __stdcall chm_AddRef(ComHashMap *this) {
    ++this->refcount;
    DebugStrPtr(L"AddRef", this->refcount);
    return this->refcount;
}

/**
 * Decrements the reference counter and frees the object when it hits 0
 *
 * https://learn.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-release
 */
ULONG __stdcall chm_Release(ComHashMap* this) {
    --this->refcount;
    DebugStrPtr(L"Release", this->refcount);
    if (this->refcount == 0) {
        // Free the variants
        int len = this->items == NULL ? 0 : stbds_hmlen(this->items);
        for (int i = 0; i < len; i++) {
            VariantClear(&this->items[i].key);
            VariantClear(&this->items[i].val);
        }
        // Free the hashmap
        stbds_hmfree(this->items);
        // Free the object
        free(this);
        return 0;
    }
    return this->refcount;
}

/**
 * Allows debug-style inspection of the COM object
 * 
 * https://learn.microsoft.com/en-us/windows/win32/api/oaidl/nf-oaidl-idispatch-gettypeinfocount
 */
HRESULT __stdcall chm_GetTypeInfoCount(ComHashMap *this, UINT* pctinfo) {
    DebugStr(L"GetTypeInfoCount");
    return E_NOTIMPL;
}

/**
 * Allows debug-style inspection of the COM object
 *
 * https://learn.microsoft.com/en-us/windows/win32/api/oaidl/nf-oaidl-idispatch-gettypeinfo
 */
HRESULT __stdcall chm_GetTypeInfo(
    ComHashMap * this,
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
HRESULT __stdcall chm_GetIDsOfNames(
    ComHashMap *this,
    /* [in] */ REFIID riid,
    /* [in] */ LPOLESTR *rgszNames,
    /* [in] */ UINT cNames,
    /* [in] */ LCID lcid,
    /* [out] */ DISPID *rgDispId) {
    HRESULT retval = S_OK;
    DebugStr(L"GetIDsOfNames");
    for (int i = 0; i < cNames; i++) {
        DebugStrPtr(rgszNames[i], i);
        if (0 == _wcsicmp(rgszNames[i], L"Set")) {
            rgDispId[i] = CHM_DISPID_SET;
        } else if (0 == _wcsicmp(rgszNames[i], L"Get")) {
            rgDispId[i] = CHM_DISPID_GET;
        } else if (0 == _wcsicmp(rgszNames[i], L"Count")) {
            rgDispId[i] = CHM_DISPID_COUNT;
        } else {
            retval = DISP_E_UNKNOWNNAME;
            rgDispId[i] = DISPID_UNKNOWN;
        }
    }
    return retval;
}

/**
 * Dispatches ComHashMap's method and property implementations
 *
 * https://learn.microsoft.com/en-us/windows/win32/api/oaidl/nf-oaidl-idispatch-invoke
 */
HRESULT __stdcall chm_Invoke(
    ComHashMap *this,
    /* [in] */ DISPID dispIdMember,
    /* [in] */ REFIID riid,
    /* [in] */ LCID lcid,
    /* [in] */ WORD wFlags,
    /* [out][in] */ DISPPARAMS *pDispParams,
    /* [out][opt] */ VARIANT *pVarResult,
    /* [out][opt] */ EXCEPINFO *pExcepInfo,
    /* [out][opt] */ UINT *puArgErr) {
    DebugStr(L"Invoke");

    // Set method
    if (
        dispIdMember == CHM_DISPID_SET && (wFlags & DISPATCH_METHOD) ||
        dispIdMember == DISPID_VALUE && (wFlags & DISPID_PROPERTYPUT)
    ) {
        DebugStr(L"Invoking Set");

        // Require exactly 2 params
        if (pDispParams->cArgs != 2) {
            return DISP_E_BADPARAMCOUNT;
        }

        VARIANT key = { .vt = VT_EMPTY };
        VariantCopy(&key, &pDispParams->rgvarg[1]);
        VARIANT val = { .vt = VT_EMPTY };
        VariantCopy(&val, &pDispParams->rgvarg[0]);

        HashItem item = {
            .key = key,
            .val = val
        };

        stbds_hmputs(this->items, item);

        pVarResult->vt = VT_DISPATCH;
        pVarResult->pdispVal = AS_IDISPATCH(this);
        this->lpVtbl->AddRef(this);

        return S_OK;
    }

    // Get method
    if (
        dispIdMember == CHM_DISPID_GET && (wFlags & DISPATCH_METHOD) ||
        dispIdMember == DISPID_VALUE && (wFlags & DISPATCH_PROPERTYGET)
    ) {
        DebugStr(L"Invoking Get");

        // Require exactly 1 param
        if (pDispParams->cArgs != 1) {
            return DISP_E_BADPARAMCOUNT;
        }

        int index = stbds_hmgeti(this->items, pDispParams->rgvarg[0]);

        if (index == -1) {
            pExcepInfo->wCode = 1001;
            pExcepInfo->bstrSource = SysAllocString(L"ComHashMap");
            pExcepInfo->bstrDescription = SysAllocString(L"Item has no value");
            return DISP_E_EXCEPTION;
        }

        VARIANT val = { .vt = VT_EMPTY };
        VariantCopy(pVarResult, &this->items[index].val);

        return S_OK;
    }

    // Count property get
    if (dispIdMember == CHM_DISPID_COUNT && wFlags & DISPATCH_PROPERTYGET) {
        pVarResult->vt = VT_I4;
        pVarResult->intVal = stbds_hmlen(this->items);
        return S_OK;
    }

    // New Enumerator
    if (dispIdMember == DISPID_NEWENUM) {
        DebugStrPtr(L"DISPID_NEWNEUM wFlags:", wFlags);

        ComHashMapEnum* chme = NewComHashMapEnum(this);

        pVarResult->vt = VT_DISPATCH;
        pVarResult->pdispVal = AS_IDISPATCH(chme);
        return S_OK;
    }

    DebugStrPtr(L"Unknown DispIdMember: ", dispIdMember);

    return DISP_E_MEMBERNOTFOUND;
}

static const ComHashMapVtbl COM_HASH_MAP_VTBL = {
    .QueryInterface = chm_QueryInterface,
    .AddRef = chm_AddRef,
    .Release = chm_Release,
    .GetTypeInfoCount = chm_GetTypeInfoCount,
    .GetTypeInfo = chm_GetTypeInfo,
    .GetIDsOfNames = chm_GetIDsOfNames,
    .Invoke = chm_Invoke,
};

ComHashMap* NewComHashMap() {
    ComHashMap* m = malloc(sizeof(ComHashMap));

    // Initialize all fields
    m->lpVtbl = &COM_HASH_MAP_VTBL;
    m->refcount = 1;
    m->items = NULL;

    return m;
}