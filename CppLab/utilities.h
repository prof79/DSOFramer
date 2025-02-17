// utilities.h

#pragma once

#include "CppLab.h"

////////////////////////////////////////////////////////////////////////
// Fixed Win32 Errors as HRESULTs
//
constexpr auto E_WIN32_BUFFERTOOSMALL = 0x8007007A;                       // HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)
constexpr auto E_WIN32_ACCESSVIOLATION = 0x800701E7;                       // HRESULT_FROM_WIN32(ERROR_INVALID_ADDRESS)
constexpr auto E_VBA_NOREMOTESERVER = 0x800A01CE;

#define E_WIN32_LASTERROR               (0x80070000 | GetLastError())       // Assured Error with last Win32 code

////////////////////////////////////////////////////////////////////////
// Macros
//
#define CHECK_NULL_RETURN(v, e) if ((v) == NULL) return (e)

////////////////////////////////////////////////////////////////////////
// Heap Allocation
//
STDAPI_(LPVOID) DsoMemAlloc(DWORD cbSize);
STDAPI_(void)   DsoMemFree(LPVOID ptr);

////////////////////////////////////////////////////////////////////////
// CUSTOM BY PROF79
//
size_t MyStringCchLength(LPCTSTR pctsz);
size_t MyStringCchLengthA(LPCSTR pctsz);
size_t MyStringCchLengthW(LPCWSTR pcwsz);
HRESULT MyStringCchCatWA(STRSAFE_LPWSTR pszDest, size_t cchDest, STRSAFE_LPCSTR pszSrc);
HRESULT MyStringCchCopyWA(STRSAFE_LPWSTR pszDest, size_t cchDest, STRSAFE_LPCSTR pszSrc);

////////////////////////////////////////////////////////////////////////
// String Manipulation Functions
//
STDAPI DsoConvertToUnicodeEx(LPCSTR pszMbcsString, DWORD cbMbcsLen, LPWSTR pwszUnicode, DWORD cbUniLen, UINT uiCodePage);

STDAPI_(LPWSTR) DsoConvertToLPWSTR(LPCSTR pszMbcsString);
