#ifndef UNICODE
#define UNICODE
#endif
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <DbgHelp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <vector>
#include <tlhelp32.h>
#include "Memoryapi.h"
#include "Shlobj.h"
#include <io.h>
#pragma comment(lib, "Shlwapi.lib")

static DWORD get_parent_pid() {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hSnapshot != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 pe32;
		ZeroMemory(&pe32, sizeof(pe32));
		pe32.dwSize = sizeof(pe32);
		if(Process32First(hSnapshot, &pe32)) {
			do {
				if(pe32.th32ProcessID == GetCurrentProcessId()) {
					CloseHandle(hSnapshot);
					return pe32.th32ParentProcessID;
				}
			} while(Process32Next(hSnapshot, &pe32));
		}
		CloseHandle(hSnapshot);
	}
	wprintf(L"Unable to obtain parent process id!\n");
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

	static std::vector<HANDLE> thread_handles;
	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if(hThreadSnap != INVALID_HANDLE_VALUE) {
		THREADENTRY32 te32;
		ZeroMemory(&te32, sizeof(te32));
		te32.dwSize = sizeof(THREADENTRY32);
		if(Thread32First(hThreadSnap, &te32)) {
			do {
				if(te32.th32OwnerProcessID == GetProcessId(hProcess)) {
					HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, te32.th32ThreadID);
					if(hThread != INVALID_HANDLE_VALUE) {
						wprintf(L"Opened thread %p\n", hThread);
						thread_handles.emplace_back(hThread);
					} else {
						wprintf(L"Unable to open thread handle!\n");
					}
				}
			} while(Thread32Next(hThreadSnap, &te32));
		}
		CloseHandle(hThreadSnap);
	} else {
		wprintf(L"Unable to create thread snapshot!\n");
	}

	std::wstring full_path = base_path + L"\\crash.dmp";
	wprintf(L"Saving to %s\n", full_path.c_str());
	HANDLE file_handle = CreateFileW(full_path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
	if(file_handle != INVALID_HANDLE_VALUE) {
		BOOL bResult = MiniDumpWriteDump(hProcess, GetProcessId(hProcess), file_handle,
			MINIDUMP_TYPE(MiniDumpWithHandleData
				| MiniDumpWithProcessThreadData
				| MiniDumpWithThreadInfo
				| MiniDumpWithAvxXStateContext
				| MiniDumpWithModuleHeaders
				| MiniDumpWithTokenInformation),
			NULL, NULL, NULL);
		if(bResult == TRUE) {
			std::wstring full_message = L"The program has aborted and a crash dump has been generated\n";
			full_message += L"It will be saved on ";
			full_message += full_path;
			full_message += L"\n";
			full_message += L"Compress the crash dump with an archiver of your choice\n";
			full_message += L"Send the crash dump and report the bug\n";
			full_message += L"We're sorry for the inconvenience\n";
			MessageBoxW(NULL,
				full_message.c_str(),
				L"Crash dump", MB_OK);
		} else {
			wprintf(L"Failed to make mini dump!\n");
		}
		CloseHandle(file_handle);
	} else {
		wprintf(L"Unable to open crash.dmp file!\n");
	}

	for(auto hThread : thread_handles)
		CloseHandle(hThread);
}

int WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nCmdShow*/
) {
	if(AttachConsole(ATTACH_PARENT_PROCESS) || AttachConsole(GetCurrentProcessId())) {
		HANDLE hConIn = GetStdHandle(STD_INPUT_HANDLE);
		int fd0 = _open_osfhandle((intptr_t)hConIn, 0);
		_dup2(fd0, 0);
		HANDLE hConOut = GetStdHandle(STD_OUTPUT_HANDLE);
		int fd1 = _open_osfhandle((intptr_t)hConOut, 0);
		_dup2(fd1, 1);
		HANDLE hConErr = GetStdHandle(STD_ERROR_HANDLE);
		int fd2 = _open_osfhandle((intptr_t)hConErr, 0);
		_dup2(fd2, 2);
	}

	auto pid = get_parent_pid();
	wprintf(L"Only Alice can run this!\n");
	HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION | PROCESS_DUP_HANDLE, FALSE, pid);
	if(hProcess) {
		minidump_generate(hProcess);
		CloseHandle(hProcess);
	} else {
		wprintf(L"Unable to open process!\n");
	}
	return EXIT_SUCCESS;
}
