// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
DWORD APIENTRY MainThread(LPVOID param)
{
	int* health;
	health = reinterpret_cast <int*>(0x2efbfe8);
	while (true) {
		if (GetAsyncKeyState(VK_F5) & 0x80000) {
			*health = 0;
		}
		Sleep(100);
	}
	return 0;
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		CreateThread(0, 0, MainThread, hModule, 0, 0);
	}
	//MessageBoxA(NULL, "DLL injected!", "DLL injected!", MB_OK);
	//CreateThread(0, 0, MainThread, hModule, 0, 0);
    return TRUE;
}

