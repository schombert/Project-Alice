#include <Windows.h>
#include <DbgHelp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <tlhelp32.h>
#include "Memoryapi.h"
#include "Shlobj.h"
#pragma comment(lib, "Shlwapi.lib")

static DWORD get_parent_pid() {
	DWORD pid = GetCurrentProcessId();
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	ZeroMemory(&pe32, sizeof(pe32));
	pe32.dwSize = sizeof(pe32);
	Process32First(hSnapshot, &pe32);
	do {
		if(pe32.th32ProcessID == pid)
			return pe32.th32ParentProcessID;
	} while(Process32Next(hSnapshot, &pe32));
	CloseHandle(hSnapshot);
	return 0;
}

static void minidump_generate(HANDLE hProcess) {
	wchar_t* local_path_out = nullptr;
	std::wstring base_path;
	if(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &local_path_out) == S_OK) {
		base_path = std::wstring(local_path_out) + L"\\Project Alice";
	}
	CoTaskMemFree(local_path_out);
	if(!base_path.empty()) {
		CreateDirectoryW(base_path.c_str(), nullptr);
		base_path += L"\\crash dumps";
		CreateDirectoryW(base_path.c_str(), nullptr);
	}

	std::wstring full_path = base_path + L'\\' + L"crash.dmp";
	HANDLE file_handle = CreateFileW(full_path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
	if(file_handle != INVALID_HANDLE_VALUE) {
		MiniDumpWriteDump(hProcess, GetProcessId(hProcess), file_handle,
			MINIDUMP_TYPE(MiniDumpNormal
				| MiniDumpWithDataSegs
				| MiniDumpWithHandleData
				| MiniDumpWithIndirectlyReferencedMemory
				| MiniDumpWithProcessThreadData
				| MiniDumpWithThreadInfo
				| MiniDumpWithAvxXStateContext
				| MiniDumpWithModuleHeaders
				| MiniDumpWithTokenInformation),
			NULL, NULL, NULL);
		SetEndOfFile(file_handle);
		CloseHandle(file_handle);
	}
}

int main(int argc, char* argv[]) {
	auto pid = get_parent_pid();
	wprintf(L"Only Alice can run this!\n");
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	minidump_generate(hProcess);
	CloseHandle(hProcess);
	return EXIT_SUCCESS;
}
