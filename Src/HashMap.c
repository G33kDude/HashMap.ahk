
#include "shared.h"
#include "stb_ds.h"

MCL_EXPORT(make, Cdecl_Ptr);

// Item type to store in the hash map
typedef struct HashItem { PackedVariant* key; PackedVariant* val; } HashItem;

// The IDispatch COM object being exported from this C code
typedef struct MyObject {
    IDispatchVtbl* lpVtbl;
    int refcount;
    HashItem* items;
} MyObject;

typedef struct ComHashMapEnum {
    IDispatchVtbl* lpVtbl;
    int refcount;
    MyObject* object;
    int index;
} ComHashMapEnum;
static const IDispatchVtbl ComHashMapEnumVtbl;

// DISPID for MyObject's IDispatch method "Set"
#define DISPID_SET 1

// DISPID for MyObject's IDispatch method "Get"
#define DISPID_GET 2

// DISPID for MyObject's IDispatch property "Count"
#define DISPID_COUNT 3

/**
 * Returns the IDispatch interface of the object when requested
 *
 * https://learn.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-queryinterface(refiid_void)
 */
HRESULT __stdcall ahk_QueryInterface(MyObject *this, REFIID riid, void** ppvObject) {
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
ULONG __stdcall ahk_AddRef(MyObject *this) {
    DebugStrPtr(L"AddRef", ++this->refcount);
    return this->refcount;
}

/**
 * Decrements the reference counter and frees the object when it hits 0
 *
 * https://learn.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-release
 */
ULONG __stdcall ahk_Release(MyObject* this) {
    DebugStrPtr(L"Release", --this->refcount);
    if (this->refcount == 0) {
        // Free the PackedVariants
        int len = this->items == NULL ? 0 : stbds_hmlen(this->items);
        for (int i = 0; i < len; i++) {
            free(this->items[i].key);
            free(this->items[i].val);
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
HRESULT __stdcall ahk_GetTypeInfoCount(MyObject *this, UINT* pctinfo) {
    DebugStr(L"GetTypeInfoCount");
    return E_NOTIMPL;
}

/**
 * Allows debug-style inspection of the COM object
 *
 * https://learn.microsoft.com/en-us/windows/win32/api/oaidl/nf-oaidl-idispatch-gettypeinfo
 */
HRESULT __stdcall ahk_GetTypeInfo(
    MyObject * this,
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
HRESULT __stdcall ahk_GetIDsOfNames(
    MyObject *this,
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
            rgDispId[i] = DISPID_SET;
        } else if (0 == _wcsicmp(rgszNames[i], L"Get")) {
            rgDispId[i] = DISPID_GET;
        } else if (0 == _wcsicmp(rgszNames[i], L"Count")) {
            rgDispId[i] = DISPID_COUNT;
        } else {
            retval = DISP_E_UNKNOWNNAME;
            rgDispId[i] = DISPID_UNKNOWN;
        }
    }
    return retval;
}

/**
 * Dispatches MyObject's method and property implementations
 *
 * https://learn.microsoft.com/en-us/windows/win32/api/oaidl/nf-oaidl-idispatch-invoke
 */
HRESULT __stdcall ahk_Invoke(
    MyObject *this,
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
    if (dispIdMember == DISPID_SET && wFlags & DISPATCH_METHOD) {
        DebugStr(L"Invoking Set");

        // Require exactly 2 params
        if (pDispParams->cArgs != 2) {
            return DISP_E_BADPARAMCOUNT;
        }

        PackedVariant* key = PackVariant(pDispParams->rgvarg[1]);
        PackedVariant* val = PackVariant(pDispParams->rgvarg[0]);

        HashItem item = {
            .key = key,
            .val = val
        };

        stbds_hmputs(this->items, item);

        pVarResult->vt = VT_DISPATCH;
        pVarResult->pdispVal = this;
        this->lpVtbl->AddRef(this);

        return S_OK;
    }

    // Get method
    if (dispIdMember == DISPID_GET && wFlags & DISPATCH_METHOD) {
        DebugStr(L"Invoking Get");

        // Require exactly 2 params
        if (pDispParams->cArgs != 1) {
            return DISP_E_BADPARAMCOUNT;
        }

        PackedVariant* key = PackVariant(pDispParams->rgvarg[0]);

        int index = stbds_hmgeti(this->items, key);

        if (index == -1) {
            pVarResult->vt = VT_BSTR;
            pVarResult->bstrVal = SysAllocString(L"Not Found");
            return 1;
        }

        VARIANT val = UnpackVariant(this->items[index].val);
        pVarResult->vt = val.vt;
        pVarResult->llVal = val.llVal;

        return S_OK;
    }

    // Count property get
    if (dispIdMember == DISPID_COUNT && wFlags & DISPATCH_PROPERTYGET) {
        pVarResult->vt = VT_I4;
        pVarResult->intVal = stbds_hmlen(this->items);
        return S_OK;
    }

    // New Enumerator
    if (dispIdMember == DISPID_NEWENUM) {
        DebugStrPtr(L"DISPID_NEWNEUM wFlags:", wFlags);

        ComHashMapEnum* chme = (ComHashMapEnum*)malloc(sizeof(ComHashMapEnum));
        chme->lpVtbl = &ComHashMapEnumVtbl;
        chme->refcount = 1;
        chme->object = this;
        chme->index = 0;
        this->lpVtbl->AddRef(this);

        pVarResult->vt = VT_DISPATCH;
        pVarResult->pdispVal = chme;
        return S_OK;
    }

    DebugStrPtr(L"Unknown DispIdMember: ", dispIdMember);

    return DISP_E_MEMBERNOTFOUND;
}

// The IDispatch VTable populated with MyObject's implementation
static const IDispatchVtbl vtable_MyObject = {
    .QueryInterface = ahk_QueryInterface,
    .AddRef = ahk_AddRef,
    .Release = ahk_Release,
    .GetTypeInfoCount = ahk_GetTypeInfoCount,
    .GetTypeInfo = ahk_GetTypeInfo,
    .GetIDsOfNames = ahk_GetIDsOfNames,
    .Invoke = ahk_Invoke,
};

////////////////////////////////////////////////////////////////////////////////
// Enum
////////////////////////////////////////////////////////////////////////////////


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
 * Dispatches MyObject's method and property implementations
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
static const IDispatchVtbl ComHashMapEnumVtbl = {
    .QueryInterface = chme_QueryInterface,
    .AddRef = chme_AddRef,
    .Release = chme_Release,
    .GetTypeInfoCount = chme_GetTypeInfoCount,
    .GetTypeInfo = chme_GetTypeInfo,
    .GetIDsOfNames = chme_GetIDsOfNames,
    .Invoke = chme_Invoke,
};


// Constructor/Factory for MyObject
MyObject* make() {
    MyObject* x = malloc(sizeof(MyObject));

    // Initialize all fields
    x->lpVtbl = &vtable_MyObject;
    x->refcount = 1;
    x->items = NULL;

    return x;
}
