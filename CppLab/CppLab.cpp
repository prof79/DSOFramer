// CppLab.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "CppLab.h"

HANDLE v_hPrivateHeap = nullptr;   // Private Memory Heap

int main()
{
    __try
    {
        v_hPrivateHeap = HeapCreate(0, 0x1000, 0);

        std::wcout << _T("Hello Unicode!\n\n");

        TCHAR szbuffer[256]{};

        szbuffer[0] = _T('\0');

        size_t cchszbuffer = ARRAYSIZE(szbuffer);

        StringCchCat(szbuffer, ARRAYSIZE(szbuffer), _T("CLSID\\"));

#ifdef UNICODE

        MyStringCchCatWA(szbuffer, ARRAYSIZE(szbuffer), DSOFRAMERCTL_CLSIDSTR);

#else

        StringCchCatA(szbuffer, ARRAYSIZE(szbuffer), DSOFRAMERCTL_CLSIDSTR);

#endif

        std::wcout << _T("Buffer: ") << szbuffer << std::endl << std::endl;
    }
    __finally
    {
        if (v_hPrivateHeap)
        {
            HeapDestroy(v_hPrivateHeap);
        }
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
