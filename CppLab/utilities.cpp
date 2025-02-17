// utilities.cpp

#include "utilities.h"

////////////////////////////////////////////////////////////////////////
// Core Utility Functions
//
////////////////////////////////////////////////////////////////////////
// Heap Allocation (Private Heap)
//
extern HANDLE v_hPrivateHeap;   // Private Memory Heap

STDAPI_(LPVOID) DsoMemAlloc(DWORD cbSize)
{
    CHECK_NULL_RETURN(v_hPrivateHeap, nullptr);

    return HeapAlloc(v_hPrivateHeap, HEAP_ZERO_MEMORY, cbSize);
}

STDAPI_(void) DsoMemFree(LPVOID ptr)
{
    if ((v_hPrivateHeap) && (ptr))
    {
        HeapFree(v_hPrivateHeap, 0, ptr);
    }
}

////////////////////////////////////////////////////////////////////////
// CUSTOM BY PROF79
//
size_t MyStringCchLength(LPCTSTR pctsz)
{
    size_t cch = 0;

    auto hr = StringCchLength(pctsz, STRSAFE_MAX_CCH, &cch);

    // TODO: Check hr

    return cch;
}

size_t MyStringCchLengthA(LPCSTR pcsz)
{
    size_t cch = 0;

    auto hr = StringCchLengthA(pcsz, STRSAFE_MAX_CCH, &cch);

    // TODO: Check hr

    return cch;
}

size_t MyStringCchLengthW(LPCWSTR pcwsz)
{
    size_t cch = 0;

    auto hr = StringCchLengthW(pcwsz, STRSAFE_MAX_CCH, &cch);

    // TODO: Check hr

    return cch;
}

HRESULT MyStringCchCatWA(STRSAFE_LPWSTR pszDest, size_t cchDest, STRSAFE_LPCSTR pszSrc)
{
    LPWSTR lpwszTemp = nullptr;

    lpwszTemp = DsoConvertToLPWSTR(pszSrc);

    auto hr = StringCchCatW(pszDest, cchDest, lpwszTemp);

    DsoMemFree(lpwszTemp);

    return hr;
}

HRESULT MyStringCchCopyWA(STRSAFE_LPWSTR pszDest, size_t cchDest, STRSAFE_LPCSTR pszSrc)
{
    LPWSTR lpwszTemp = nullptr;

    lpwszTemp = DsoConvertToLPWSTR(pszSrc);

    auto hr = StringCchCopyW(pszDest, cchDest, lpwszTemp);

    DsoMemFree(lpwszTemp);

    return hr;
}

////////////////////////////////////////////////////////////////////////
// Global String Functions
//
////////////////////////////////////////////////////////////////////////
// DsoConvertToUnicodeEx
//
STDAPI DsoConvertToUnicodeEx(LPCSTR pszMbcsString, DWORD cbMbcsLen, LPWSTR pwszUnicode, DWORD cbUniLen, UINT uiCodePage)
{
    DWORD cbRet;
    UINT iCode = CP_ACP;

    if (IsValidCodePage(uiCodePage))
    {
        iCode = uiCodePage;
    }

    CHECK_NULL_RETURN(pwszUnicode, E_POINTER);

    pwszUnicode[0] = L'\0';

    CHECK_NULL_RETURN(pszMbcsString, E_POINTER);
    CHECK_NULL_RETURN(cbMbcsLen, E_INVALIDARG);
    CHECK_NULL_RETURN(cbUniLen, E_INVALIDARG);

    cbRet = MultiByteToWideChar(iCode, 0, pszMbcsString, cbMbcsLen, pwszUnicode, cbUniLen);

    if (cbRet == 0)
    {
        return E_WIN32_LASTERROR;
    }

    pwszUnicode[cbRet] = L'\0';

    return S_OK;
}

////////////////////////////////////////////////////////////////////////
// DsoConvertToLPWSTR
//
//  Takes a MBCS string and returns a LPWSTR allocated on private heap.
//
STDAPI_(LPWSTR) DsoConvertToLPWSTR(LPCSTR pszMbcsString)
{
    LPWSTR pwsz = nullptr;
    size_t cblen, cbnew;

    if (pszMbcsString)
    {
        cblen = MyStringCchLengthA(pszMbcsString);

        if (cblen > 0)
        {
            cbnew = ((cblen + 1) * sizeof(WCHAR));

            if ((pwsz = (LPWSTR)DsoMemAlloc(cbnew)) != nullptr)
            {
                if (FAILED(DsoConvertToUnicodeEx(pszMbcsString, cblen, pwsz, cbnew, GetACP())))
                {
                    DsoMemFree(pwsz);

                    pwsz = nullptr;
                }
            }
        }
    }

    return pwsz;
}
