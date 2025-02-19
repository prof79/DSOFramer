/***************************************************************************
 * MAINENTRY.CPP
 *
 * Main DLL Entry and Required COM Entry Points.
 *
 *  Copyright ?999-2004; Microsoft Corporation. All rights reserved.
 *  Written by Microsoft Developer Support Office Integration (PSS DSOI)
 *
 *  This code is provided via KB 311765 as a sample. It is not a formal
 *  product and has not been tested with all containers or servers. Use it
 *  for educational purposes only. See the EULA.TXT file included in the
 *  KB download for full terms of use and restrictions.
 *
 *  THIS CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 *  EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 ***************************************************************************/

// Init the GUIDS for the control...
#define INITGUID

#include "dsoframer.h"

HINSTANCE        v_hModule        = nullptr;   // DLL module handle
HANDLE           v_hPrivateHeap   = nullptr;   // Private Memory Heap
ULONG            v_cLocks         = 0;      // Count of server locks
HICON            v_icoOffDocIcon  = nullptr;   // Small office icon (for caption bar)
BOOL             v_fUnicodeAPI    = FALSE;  // Flag to determine if we should us Unicode API
BOOL             v_fWindows2KPlus = FALSE;

CRITICAL_SECTION v_csecThreadSynch;

////////////////////////////////////////////////////////////////////////
//
// DllMain -- OCX Main Entry
//
extern "C" BOOL APIENTRY DllMain(HINSTANCE hDllHandle, DWORD dwReason, LPVOID /*lpReserved*/)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        v_hModule = hDllHandle;
        v_hPrivateHeap = HeapCreate(0, 0x1000, 0);
        v_icoOffDocIcon = (HICON)LoadImage(hDllHandle, MAKEINTRESOURCE(IDI_SMALLOFFDOC), IMAGE_ICON, 16, 16, 0);
        {
            v_fWindows2KPlus = IsWindowsXPOrGreater();

#ifdef UNICODE
            v_fUnicodeAPI = TRUE;
#else
            v_fUnicodeAPI = FALSE;
#endif
        }
        InitializeCriticalSection(&v_csecThreadSynch);
        DisableThreadLibraryCalls(hDllHandle);
        break;

    case DLL_PROCESS_DETACH:
        if (v_icoOffDocIcon)
        {
            DeleteObject(v_icoOffDocIcon);
        }

        if (v_hPrivateHeap)
        {
            HeapDestroy(v_hPrivateHeap);
        }
        
        DeleteCriticalSection(&v_csecThreadSynch);
        
        break;
    }

    return TRUE;
}

#ifdef DSO_MIN_CRT_STARTUP
extern "C" BOOL APIENTRY _DllMainCRTStartup(HINSTANCE hDllHandle, DWORD dwReason, LPVOID lpReserved)
{
    return DllMain(hDllHandle, dwReason, lpReserved);
}
#endif

////////////////////////////////////////////////////////////////////////
//
// Standard COM DLL Entry Points
//
//
////////////////////////////////////////////////////////////////////////
// DllCanUnloadNow
//
//
STDAPI DllCanUnloadNow(void)
{
    return ((v_cLocks == 0) ? S_OK : S_FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// DllGetClassObject
//
//  Returns IClassFactory instance for FramerControl. We only support
//  this one object for creation.
//
STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID FAR* ppv)
{
    HRESULT hr;

    CDsoFramerClassFactory *pcf;

    CHECK_NULL_RETURN(ppv, E_POINTER);

    *ppv = nullptr;

    // The only component we can create is the BinderControl...
    if (rclsid != CLSID_FramerControl)
    {
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    // Create the needed class factory...
    pcf = new CDsoFramerClassFactory();

    CHECK_NULL_RETURN(pcf, E_OUTOFMEMORY);

    // Get requested interface.
    if (FAILED(hr = pcf->QueryInterface(riid, ppv)))
    {
        *ppv = nullptr;

        delete pcf;
    }
    else
    {
        InterlockedIncrement((LPLONG)&v_cLocks);
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////
//
// DllRegisterServer
//
//  Registration of the OCX.
//
STDAPI DllRegisterServer()
{
    HRESULT hr = S_OK;

    HKEY    hk, hk2;
    DWORD   dwret;
    TCHAR   szbuffer[256]{};
    LPWSTR  pwszModule;

    ITypeInfo* pti;

    size_t cbBufferPlusTerm = 0;

    szbuffer[0] = _T('\0');

    // If we can't find the path to the DLL, we can't register...
    if (!FGetModuleFileName(v_hModule, &pwszModule))
    {
        return E_FAIL;
    }

    StringCchCat(szbuffer, ARRAYSIZE(szbuffer), _T("CLSID\\"));

#ifdef UNICODE

    MyStringCchCatWA(szbuffer, ARRAYSIZE(szbuffer), DSOFRAMERCTL_CLSIDSTR);

#else

    StringCchCatA(szbuffer, ARRAYSIZE(szbuffer), DSOFRAMERCTL_CLSIDSTR);

#endif

    // Setup the CLSID. This is the most important. If there is a critical failure,
    // we will set HR = GetLastError and return...

    if ((dwret = RegCreateKeyEx(
                    HKEY_CLASSES_ROOT,
                    szbuffer, 0, nullptr, 0, KEY_WRITE, nullptr, &hk, nullptr)) != ERROR_SUCCESS)
    {
        DsoMemFree(pwszModule);

        return HRESULT_FROM_WIN32(dwret);
    }

#ifdef UNICODE

    MyStringCchCopyWA(szbuffer, ARRAYSIZE(szbuffer), DSOFRAMERCTL_SHORTNAME);

#else

    StringCchCopy(szbuffer, ARRAYSIZE(szbuffer), DSOFRAMERCTL_SHORTNAME);

#endif
    
    cbBufferPlusTerm = (MyStringCchLength(szbuffer) + 1) * sizeof(TCHAR);

    RegSetValueEx(hk, nullptr, 0, REG_SZ, (BYTE *)szbuffer, cbBufferPlusTerm);

    // Setup the InprocServer32 key...
    dwret = RegCreateKeyEx(hk, _T("InprocServer32"), 0, nullptr, 0, KEY_WRITE, nullptr, &hk2, nullptr);

    if (dwret == ERROR_SUCCESS)
    {
        StringCchCopy(szbuffer, ARRAYSIZE(szbuffer), _T("Apartment"));

        cbBufferPlusTerm = (MyStringCchLength(szbuffer) + 1) * sizeof(TCHAR);

        RegSetValueEx(hk2, _T("ThreadingModel"), 0, REG_SZ, (BYTE *)szbuffer, cbBufferPlusTerm);

        // We call a wrapper function for this setting since the path should be
        // stored in Unicode to handle non-ANSI file path names on some systems.
        // This wrapper will convert the path to ANSI if we are running on Win9x.
        // The rest of the Reg calls should be OK in ANSI since they do not
        // contain non-ANSI/Unicode-specific characters...
        if (!FSetRegKeyValue(hk2, pwszModule))
        {
            hr = E_ACCESSDENIED;
        }

        RegCloseKey(hk2);

        dwret = RegCreateKeyEx(hk, _T("ProgID"), 0, nullptr, 0, KEY_WRITE, nullptr, &hk2, nullptr);

        if (dwret == ERROR_SUCCESS)
        {
#ifdef UNICODE
            MyStringCchCopyWA(szbuffer, ARRAYSIZE(szbuffer), DSOFRAMERCTL_PROGID);
#else
            StringCchCopy(szbuffer, ARRAYSIZE(szbuffer), DSOFRAMERCTL_PROGID);
#endif

            cbBufferPlusTerm = (MyStringCchLength(szbuffer) + 1) * sizeof(TCHAR);

            RegSetValueEx(hk2, nullptr, 0, REG_SZ, (BYTE *)szbuffer, cbBufferPlusTerm);

            RegCloseKey(hk2);
        }
    }
    else
    {
        hr = HRESULT_FROM_WIN32(dwret);
    }

    if (SUCCEEDED(hr))
    {
        dwret = RegCreateKeyEx(hk, _T("Control"), 0, nullptr, 0, KEY_WRITE, nullptr, &hk2, nullptr);

        if (dwret == ERROR_SUCCESS)
        {
            RegCloseKey(hk2);
        }
        else
        {
            hr = HRESULT_FROM_WIN32(dwret);
        }
    }

    // If we succeeded so far, andle the remaining (non-critical) reg keys...
    if (SUCCEEDED(hr))
    {
        dwret = RegCreateKeyEx(hk, _T("ToolboxBitmap32"), 0, nullptr, 0, KEY_WRITE, nullptr, &hk2, nullptr);

        if (dwret == ERROR_SUCCESS)
        {
            LPWSTR pwszT = DsoCopyStringCat(pwszModule, L",102");

            if (pwszT)
            {
                FSetRegKeyValue(hk2, pwszT);

                DsoMemFree(pwszT);
            }

            RegCloseKey(hk2);
        }

        dwret = RegCreateKeyEx(hk, _T("TypeLib"), 0, nullptr, 0, KEY_WRITE, nullptr, &hk2, nullptr);

        if (dwret == ERROR_SUCCESS)
        {
#ifdef UNICODE
            MyStringCchCopyWA(szbuffer, ARRAYSIZE(szbuffer), DSOFRAMERCTL_TLIBSTR);
#else
            StringCchCopy(szbuffer, ARRAYSIZE(szbuffer), DSOFRAMERCTL_TLIBSTR);
#endif

            cbBufferPlusTerm = (MyStringCchLength(szbuffer) + 1) * sizeof(TCHAR);

            RegSetValueEx(hk2, nullptr, 0, REG_SZ, (BYTE *)szbuffer, cbBufferPlusTerm);
            
            RegCloseKey(hk2);
        }

        dwret = RegCreateKeyEx(hk, _T("Version"), 0, nullptr, 0, KEY_WRITE, nullptr, &hk2, nullptr);

        if (dwret == ERROR_SUCCESS)
        {
#ifdef UNICODE
            MyStringCchCopyWA(szbuffer, ARRAYSIZE(szbuffer), DSOFRAMERCTL_VERSIONSTR);
#else
            StringCchCopy(szbuffer, ARRAYSIZE(szbuffer), DSOFRAMERCTL_VERSIONSTR);
#endif
            
            cbBufferPlusTerm = (MyStringCchLength(szbuffer) + 1) * sizeof(TCHAR);

            RegSetValueEx(hk2, nullptr, 0, REG_SZ, (BYTE *)szbuffer, cbBufferPlusTerm);
            
            RegCloseKey(hk2);
        }

        dwret = RegCreateKeyEx(hk, _T("MiscStatus"), 0, nullptr, 0, KEY_WRITE, nullptr, &hk2, nullptr);

        if (dwret == ERROR_SUCCESS)
        {
            StringCchCopy(szbuffer, ARRAYSIZE(szbuffer), _T("131473"));
            
            cbBufferPlusTerm = (MyStringCchLength(szbuffer) + 1) * sizeof(TCHAR);

            RegSetValueEx(hk2, nullptr, 0, REG_SZ, (BYTE *)szbuffer, cbBufferPlusTerm);
            
            RegCloseKey(hk2);
        }

        dwret = RegCreateKeyEx(hk, _T("DataFormats\\GetSet\\0"), 0, nullptr, 0, KEY_WRITE, nullptr, &hk2, nullptr);

        if (dwret == ERROR_SUCCESS)
        {
            StringCchCopy(szbuffer, ARRAYSIZE(szbuffer), _T("3,1,32,1"));
            
            cbBufferPlusTerm = (MyStringCchLength(szbuffer) + 1) * sizeof(TCHAR);

            RegSetValueEx(hk2, nullptr, 0, REG_SZ, (BYTE *)szbuffer, cbBufferPlusTerm);

            RegCloseKey(hk2);
        }
    }

    RegCloseKey(hk);

    DsoMemFree(pwszModule);

    // This should catch any critical failures during setup of CLSID...
    RETURN_ON_FAILURE(hr);

    TCHAR tszProgId[256];

#ifdef UNICODE

    MyStringCchCopyWA(tszProgId, ARRAYSIZE(tszProgId), DSOFRAMERCTL_PROGID);

#else
    
    StringCchCopy(tszProgId, ARRAYSIZE(tszProgId), DSOFRAMERCTL_PROGID);

#endif

    // Setup the ProgID (non-critical)...
    if (RegCreateKeyEx(HKEY_CLASSES_ROOT, tszProgId, 0,
            nullptr, 0, KEY_WRITE, nullptr, &hk, nullptr) == ERROR_SUCCESS)
    {
#ifdef UNICODE

        MyStringCchCopyWA(szbuffer, ARRAYSIZE(szbuffer), DSOFRAMERCTL_FULLNAME);

#else

        StringCchCopy(szbuffer, ARRAYSIZE(szbuffer), DSOFRAMERCTL_FULLNAME);

#endif

        cbBufferPlusTerm = (MyStringCchLength(szbuffer) + 1) * sizeof(TCHAR);

        RegSetValueEx(hk, nullptr, 0, REG_SZ, (BYTE *)szbuffer, cbBufferPlusTerm);

        if (RegCreateKeyEx(hk, _T("CLSID"), 0,
                nullptr, 0, KEY_WRITE, nullptr, &hk2, nullptr) == ERROR_SUCCESS)
        {
#ifdef UNICODE
            MyStringCchCopyWA(szbuffer, ARRAYSIZE(szbuffer), DSOFRAMERCTL_CLSIDSTR);
#else
            StringCchCopy(szbuffer, ARRAYSIZE(szbuffer), DSOFRAMERCTL_CLSIDSTR);
#endif
            
            cbBufferPlusTerm = (MyStringCchLength(szbuffer) + 1) * sizeof(TCHAR);

            RegSetValueEx(hk2, nullptr, 0, REG_SZ, (BYTE *)szbuffer, cbBufferPlusTerm);
            
            RegCloseKey(hk2);
        }

        RegCloseKey(hk);
    }

    // Load the type info (this should register the lib once)...
    hr = DsoGetTypeInfoEx(
        LIBID_DSOFramer,
        0,
        DSOFRAMERCTL_VERSION_MAJOR,
        DSOFRAMERCTL_VERSION_MINOR,
        v_hModule,
        CLSID_FramerControl,
        &pti
    );

    if (SUCCEEDED(hr))
    {
        pti->Release();
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////
//
// RegRecursiveDeleteKey
//
//  Helper function called by DllUnregisterServer for nested key removal.
//
static HRESULT RegRecursiveDeleteKey(HKEY hkParent, LPCTSTR pszSubKey)
{
    HRESULT hr = S_OK;

    HKEY hk;
    DWORD dwret, dwsize;
    FILETIME time ;
    
    TCHAR szbuffer[512];

    dwret = RegOpenKeyEx(hkParent, pszSubKey, 0, KEY_ALL_ACCESS, &hk);

    if (dwret != ERROR_SUCCESS)
    {
        return HRESULT_FROM_WIN32(dwret);
    }

    // Enumerate all of the decendents of this child...
    dwsize = 510;

    while (RegEnumKeyEx(hk, 0, szbuffer, &dwsize, nullptr, nullptr, nullptr, &time) == ERROR_SUCCESS)
    {
        // If there are any sub-folders, delete them first (to make NT happy)...
        hr = RegRecursiveDeleteKey(hk, szbuffer);

        if (FAILED(hr))
        {
            break;
        }

        dwsize = 510;
    }

    // Close the child...
    RegCloseKey(hk);

    RETURN_ON_FAILURE(hr);

    // Delete this child.
    dwret = RegDeleteKey(hkParent, pszSubKey);

    if (dwret != ERROR_SUCCESS)
    {
        hr = HRESULT_FROM_WIN32(dwret);
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////
//
// DllUnregisterServer
//
//  Removal code for the OCX.
//
STDAPI DllUnregisterServer()
{
    HRESULT hr;

    constexpr auto bufsize = 256;

    TCHAR dsoframerctl_clsidstr[bufsize];
    TCHAR dsoframerctl_tlibstr[bufsize];

    TCHAR dsoframerctl_clsidstr_path[bufsize];
    TCHAR dsoframerctl_tlibstr_path[bufsize];

#ifdef UNICODE

    MyStringCchCopyWA(dsoframerctl_clsidstr, ARRAYSIZE(dsoframerctl_clsidstr), DSOFRAMERCTL_CLSIDSTR);
    MyStringCchCopyWA(dsoframerctl_tlibstr, ARRAYSIZE(dsoframerctl_tlibstr), DSOFRAMERCTL_TLIBSTR);

#else

    StringCchCopyA(dsoframerctl_clsidstr, ARRAYSIZE(dsoframerctl_clsidstr), DSOFRAMERCTL_CLSIDSTR);
    StringCchCopyA(dsoframerctl_tlibstr, ARRAYSIZE(dsoframerctl_tlibstr), DSOFRAMERCTL_TLIBSTR);

#endif

    StringCchPrintf(
        dsoframerctl_clsidstr_path,
        ARRAYSIZE(dsoframerctl_clsidstr_path),
        _T("CLSID\\%s"),
        dsoframerctl_clsidstr
    );
    StringCchPrintf(
        dsoframerctl_tlibstr_path,
        ARRAYSIZE(dsoframerctl_tlibstr_path),
        _T("TypeLib\\%s"),
        dsoframerctl_tlibstr
    );

    hr = RegRecursiveDeleteKey(HKEY_CLASSES_ROOT, dsoframerctl_clsidstr_path);
    
    if (SUCCEEDED(hr))
    {
        TCHAR tszProgId[256];

#ifdef UNICODE
        MyStringCchCopyWA(tszProgId, ARRAYSIZE(tszProgId), DSOFRAMERCTL_PROGID);
#else
        StringCchCopy(tszProgId, ARRAYSIZE(tszProgId), DSOFRAMERCTL_PROGID);
#endif

        RegRecursiveDeleteKey(HKEY_CLASSES_ROOT, tszProgId);
        RegRecursiveDeleteKey(HKEY_CLASSES_ROOT, dsoframerctl_tlibstr_path);
    }

    // This means the key does not exist (i.e., the DLL
    // was alreay unregistered, so return OK)...
    if (hr == 0x80070002)
    {
        hr = S_OK;
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////
//
// DllInstall -- Optional entry point for custom setup/cleanup
//
STDAPI DllInstall(BOOL bInstall, _In_opt_ LPCWSTR pszCmdLine)
{
    HRESULT hr = S_OK;

    if (bInstall)
    {
        // Custom installation logic
        ODS(_T("DllInstall: Installing\n"));

        hr = DllRegisterServer();
        
        if (FAILED(hr))
        {
            TRACE1(_T("DllInstall: DllRegisterServer failed: %X\n"), hr);
        }
    }
    else
    {
        // Custom uninstallation logic
        ODS(_T("DllInstall: Uninstalling\n"));

        hr = DllUnregisterServer();
        
        if (FAILED(hr))
        {
            TRACE1(_T("DllInstall: DllUnregisterServer failed: %X\n"), hr);
        }
    }

    return hr;
}
