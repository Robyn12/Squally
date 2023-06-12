// SquallyDLLHack.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <windows.h>
#include <iostream>
#include <thread>
#include <psapi.h>
#include <TlHelp32.h>
using namespace std;

DWORD getPid(const wchar_t* target) {
	DWORD pid; //This will store our Process ID, used to read/write into the memory
	HWND hwnd;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	do {
		if (wcscmp(pe32.szExeFile, target) == 0) {
			CloseHandle(hSnapshot);
			pid = pe32.th32ProcessID;
			break;
		}
	} while (Process32Next(hSnapshot, &pe32));
	//CloseHandle(hSnapshot);
	return pid;
}

HANDLE* getHandle(DWORD pid) {

	HANDLE handle = NULL;
	handle = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_CREATE_THREAD| PROCESS_QUERY_INFORMATION| PROCESS_VM_OPERATION| PROCESS_VM_WRITE| PROCESS_VM_READ, 0, pid);

	if (!(handle))
	{
		cout << "Could not get handle!\n";
		cin.get();
	}
	return &handle;
}

int main()
{
	const wchar_t* processName = L"Squally.exe";
	DWORD pID = getPid(processName);

	char dll[] = "SquallyDLL.dll";
	char dllPath[MAX_PATH] = { 0 };
	GetFullPathNameA(dll, MAX_PATH, dllPath, NULL);
	printf("%s : \n%s\n", dll, dllPath);
	//HANDLE* tmp = getHandle(pID);
	//HANDLE handle = NULL;
	HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_CREATE_THREAD| PROCESS_QUERY_INFORMATION| PROCESS_VM_OPERATION| PROCESS_VM_WRITE| PROCESS_VM_READ, 0, pID);

	printf("Pid is : %d", pID);
	//HANDLE pHandle = *tmp;
	SIZE_T bytesWritten = 0;
	LPVOID pszLibFileRemote = VirtualAllocEx(pHandle, NULL, strlen(dllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	WriteProcessMemory(pHandle, pszLibFileRemote, dllPath, strlen(dllPath) + 1, &bytesWritten);
	printf("\nBytesWritten: %d\nat:%x", bytesWritten,pszLibFileRemote);
	DWORD threadID = 0;
	HANDLE tHandle = CreateRemoteThread(pHandle, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, pszLibFileRemote, NULL, &threadID);
	WaitForSingleObject(tHandle, INFINITE);
	CloseHandle(tHandle);
	VirtualFreeEx(pHandle, dllPath, 0, MEM_RELEASE);
	CloseHandle(pHandle);
	printf("threadID:%d\n", threadID);
	
	return 0;
}

