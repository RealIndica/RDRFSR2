// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>

void Patch() {
    DWORD jzAddress = 0x26C4CCD;
    uintptr_t base = (uintptr_t)GetModuleHandle(NULL);
    *(BYTE*)(base + jzAddress) = 0x85; //JZ > JNZ
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Patch();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

