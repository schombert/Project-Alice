#include <Windows.h>
#include <DbgHelp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <tlhelp32.h>

static void minidump_generate(HANDLE hProcess) {
	HANDLE hFile = CreateFileW(L"crash.dmp", GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE)) {
		BOOL bResult = MiniDumpWriteDump(
			hProcess,
			GetProcessId(hProcess),
			hFile,
			MINIDUMP_TYPE(MiniDumpNormal
				| MiniDumpWithDataSegs
				| MiniDumpWithFullMemory
				| MiniDumpWithHandleData
				| MiniDumpWithIndirectlyReferencedMemory
				| MiniDumpWithProcessThreadData
				| MiniDumpWithFullMemoryInfo
				| MiniDumpWithThreadInfo
				| MiniDumpWithAvxXStateContext
				| MiniDumpWithModuleHeaders
				| MiniDumpWithTokenInformation),
			NULL,
			NULL,
			NULL);
		if(bResult == TRUE) {
			wprintf(L"Wrote dump to crash.dmp!\n");
		} else {
			wprintf(L"error: Unable to write dump!\n");
		}
		CloseHandle(hFile);
	} else {
		wprintf(L"error: Unable to create crash.dmp file!\n");
		return;
	}
}

static void get_process_by_name(LPCWSTR name, void (*f)(HANDLE)) {
	// See https://stackoverflow.com/questions/865152/how-can-i-get-a-process-handle-by-its-name-in-c
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	BOOL bFound = FALSE;
	if(Process32First(hSnapshot, &entry) == TRUE) {
		while(Process32Next(hSnapshot, &entry) == TRUE) {
			if(lstrcmpW(entry.szExeFile, name) == 0) {
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
				f(hProcess);
				CloseHandle(hProcess);
				bFound = TRUE;
			}
		}
	}
	CloseHandle(hSnapshot);
	if(bFound == FALSE) {
		wprintf(L"error: Need to run Alice.exe while running DbgAlice!\n");
		return;
	}
}

int main(int argc, char* argv[]) {
	get_process_by_name(L"Alice.exe", minidump_generate);
	return EXIT_SUCCESS;
}
