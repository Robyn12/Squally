// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <windows.h>
#include <psapi.h>
#include <TlHelp32.h>


DWORD APIENTRY GetModuleBase(const wchar_t* ModuleName, DWORD ProcessId) {
	// This structure contains lots of goodies about a module
	MODULEENTRY32 ModuleEntry = { 0 };
	// Grab a snapshot of all the modules in the specified process
	HANDLE SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcessId);

	if (!SnapShot)
		return NULL;

	// You have to initialize the size, otherwise it will not work
	ModuleEntry.dwSize = sizeof(ModuleEntry);

	// Get the first module in the process
	if (!Module32First(SnapShot, &ModuleEntry)) {
		return NULL;
	}

	do {
		// Check if the module name matches the one we're looking for
		if (!wcscmp(ModuleEntry.szModule, ModuleName)) {
			// If it does, close the snapshot handle and return the base address
			CloseHandle(SnapShot);
			return (DWORD)ModuleEntry.modBaseAddr;
		}
		// Grab the next module in the snapshot
	} while (Module32Next(SnapShot, &ModuleEntry));

	// We couldn't find the specified module, so return NULL
	CloseHandle(SnapShot);
	return NULL;
}
DWORD APIENTRY MainThread(LPVOID param)
{
	const wchar_t* name = L"Squally.exe";
	int pid = GetCurrentProcessId();
	DWORD baseAddr = GetModuleBase(name, pid);
	int* health;
	int offset;
	offset = 0x20CBFE8;
	DWORD tmp = baseAddr + offset;
	/*
	wchar_t buffer[60];
	wsprintf(buffer, L"addr:0x%x and baseaddr %x", tmp, baseAddr);
	MessageBox(NULL, buffer, L"Title", MB_OK);
	*/
	health = reinterpret_cast <int*>(tmp);
	while (true) {
		if (GetAsyncKeyState(VK_F5) & 0x80000) {
			*health = 0;
		}
		Sleep(100);
	}
	return 0;
}
BOOL APIENTRY DllMain(HMODULE hModule,
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
