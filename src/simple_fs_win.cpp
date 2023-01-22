#include "simple_fs.hpp"

#ifndef UNICODE
#define UNICODE
#endif
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "Windows.h"
#include "Memoryapi.h"
#include "Shlobj.h"
#include <cstdlib>

#pragma comment(lib, "Shlwapi.lib")

namespace simple_fs {
	file::~file() {
		if(mapping_handle) {
			if(content.data)
				UnmapViewOfFile(content.data);
			CloseHandle(mapping_handle);
		}
		if(file_handle != INVALID_HANDLE_VALUE) {
			CloseHandle(file_handle);
		}
	}

	file::file(file&& other) noexcept : absolute_path(std::move(other.absolute_path)) {
		mapping_handle = other.mapping_handle;
		file_handle = other.file_handle;
		other.mapping_handle = nullptr;
		other.file_handle = INVALID_HANDLE_VALUE;
		content = other.content;
	}
	void file::operator=(file&& other) noexcept {
		mapping_handle = other.mapping_handle;
		file_handle = other.file_handle;
		other.mapping_handle = nullptr;
		other.file_handle = INVALID_HANDLE_VALUE;
		content = other.content;
		absolute_path = std::move(other.absolute_path);
	}

	file::file(native_string const& full_path) {
		file_handle = CreateFileW(full_path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
		if(file_handle != INVALID_HANDLE_VALUE) {
			absolute_path = full_path;
			mapping_handle = CreateFileMappingW(file_handle, nullptr, PAGE_READONLY, 0, 0, nullptr);
			if(mapping_handle) {
				content.data = (char const*)MapViewOfFile(mapping_handle, FILE_MAP_READ, 0, 0, 0);
				if(content.data) {
					_LARGE_INTEGER pvalue;
					GetFileSizeEx(file_handle, &pvalue);
					content.file_size = uint32_t(pvalue.QuadPart);
				}
			}
		}
	}
	file::file(HANDLE file_handle, native_string const& full_path) : file_handle(file_handle) {
		absolute_path = full_path;
		mapping_handle = CreateFileMappingW(file_handle, nullptr, PAGE_READONLY, 0, 0, nullptr);
		if(mapping_handle) {
			content.data = (char const*)MapViewOfFile(mapping_handle, FILE_MAP_READ, 0, 0, 0);
			if(content.data) {
				_LARGE_INTEGER pvalue;
				GetFileSizeEx(file_handle, &pvalue);
				content.file_size = uint32_t(pvalue.QuadPart);
			}
		}
	}

	std::optional<file> open_file(unopened_file const& f) {
		std::optional<file> result(file{ f.absolute_path });
		if(!result->content.data) {
			result = std::optional<file>{};
		}
		return result;
	}

	void reset(file_system& fs) {
		fs.ordered_roots.clear();
	}

	void add_root(file_system& fs, native_string_view root_path) {
		fs.ordered_roots.emplace_back(root_path);
	}

	void add_relative_root(file_system& fs, native_string_view root_path) {
		WCHAR module_name[MAX_PATH] = {};
		int32_t path_used = GetModuleFileNameW(nullptr, module_name, MAX_PATH);
		while(path_used >= 0 && module_name[path_used] != L'\\') {
			module_name[path_used] = 0;
			--path_used;
		}

		fs.ordered_roots.push_back(native_string(module_name) + native_string(root_path));
	}

	directory get_root(file_system const& fs) {
		return directory(&fs, NATIVE(""));
	}

	native_string extract_state(file_system const& fs) {
		native_string result;
		for(auto const& str : fs.ordered_roots) {
			result += L";" + str;
		}
		return result;
	}
	void restore_state(file_system& fs, native_string_view data) {
		fs.ordered_roots.clear();

		native_char const* position = data.data();
		native_char const* end = data.data() + data.length();
		while(position < end) {
			auto next_semicolon = std::find(position, end, NATIVE(';'));

			fs.ordered_roots.emplace_back(position, next_semicolon);
			position = next_semicolon + 1;
		}
	}

	std::vector<unopened_file> list_files(directory const& dir, native_char const* extension) {
		std::vector<unopened_file> accumulated_results;
		if(dir.parent_system) {
			for(size_t i = dir.parent_system->ordered_roots.size(); i-- > 0; ) {
				const auto appended_path = dir.parent_system->ordered_roots[i] + dir.relative_path + NATIVE("\\*") + extension;
				WIN32_FIND_DATAW find_result;
				auto find_handle = FindFirstFileW(appended_path.c_str(), &find_result);
				if(find_handle != INVALID_HANDLE_VALUE) {
					do {
						if(!(find_result.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
							if(auto search_result = std::find_if(accumulated_results.begin(), accumulated_results.end(), [n = find_result.cFileName](const auto& f) {
								return f.file_name.compare(n) == 0;
								}); search_result == accumulated_results.end()) {

								accumulated_results.emplace_back(dir.parent_system->ordered_roots[i] + dir.relative_path + NATIVE("\\") + find_result.cFileName, find_result.cFileName);
							}
						}
					} while(FindNextFileW(find_handle, &find_result) != 0);
					FindClose(find_handle);
				}
			}
		} else {
			const auto appended_path = dir.relative_path + NATIVE("\\*") + extension;
			WIN32_FIND_DATAW find_result;
			auto find_handle = FindFirstFileW(appended_path.c_str(), &find_result);
			if(find_handle != INVALID_HANDLE_VALUE) {
				do {
					if(!(find_result.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
						accumulated_results.emplace_back(dir.relative_path + NATIVE("\\") + find_result.cFileName, find_result.cFileName);
						
					}
				} while(FindNextFileW(find_handle, &find_result) != 0);
				FindClose(find_handle);
			}
		}
		return accumulated_results;
	}
	std::vector<directory> list_subdirectories(directory const& dir) {
		std::vector<directory> accumulated_results;
		if(dir.parent_system) {
			for(size_t i = dir.parent_system->ordered_roots.size(); i-- > 0; ) {
				const auto appended_path = dir.parent_system->ordered_roots[i] + dir.relative_path + NATIVE("\\*");
				WIN32_FIND_DATAW find_result;
				auto find_handle = FindFirstFileW(appended_path.c_str(), &find_result);
				if(find_handle != INVALID_HANDLE_VALUE) {
					do {
						if(find_result.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
							native_string const rel_name = dir.relative_path + NATIVE("\\") + find_result.cFileName;
							if(find_result.cFileName[0] != NATIVE('.')
								&& std::find_if(accumulated_results.begin(), accumulated_results.end(),
									[&rel_name](const auto& s) {
										return s.relative_path.compare(rel_name) == 0;
									}) == accumulated_results.end()) {
								accumulated_results.emplace_back(dir.parent_system, rel_name);
							}
						}
					} while(FindNextFileW(find_handle, &find_result) != 0);
					FindClose(find_handle);
				}
			}
		} else {
			const auto appended_path = dir.relative_path + NATIVE("\\*");
			WIN32_FIND_DATAW find_result;
			auto find_handle = FindFirstFileW(appended_path.c_str(), &find_result);
			if(find_handle != INVALID_HANDLE_VALUE) {
				do {
					if(find_result.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						native_string const rel_name = dir.relative_path + NATIVE("\\") + find_result.cFileName;
						if(find_result.cFileName[0] != NATIVE('.')) {
							accumulated_results.emplace_back(nullptr, rel_name);
						}
					}
				} while(FindNextFileW(find_handle, &find_result) != 0);
				FindClose(find_handle);
			}
		}
		return accumulated_results;
	}

	directory open_directory(directory const& dir, native_string_view directory_name) {
		return directory(dir.parent_system, dir.relative_path + NATIVE('\\') + native_string(directory_name));
	}

	native_string get_full_name(directory const& dir) {
		return dir.relative_path;
	}

	std::optional<file> open_file(directory const& dir, native_string_view file_name) {
		if(dir.parent_system) {
			for(size_t i = dir.parent_system->ordered_roots.size(); i-- > 0; ) {
				native_string full_path = dir.parent_system->ordered_roots[i] + dir.relative_path + NATIVE('\\') + native_string(file_name);
				HANDLE file_handle = CreateFileW(full_path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
				if(file_handle != INVALID_HANDLE_VALUE) {
					return std::optional<file>(file(file_handle, full_path));
				}
			}
		} else {
			native_string full_path = dir.relative_path + NATIVE('\\') + native_string(file_name);
			HANDLE file_handle = CreateFileW(full_path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
			if(file_handle != INVALID_HANDLE_VALUE) {
				return std::optional<file>(file(file_handle, full_path));
			}
		}
		return std::optional<file>{};
	}

	std::optional<unopened_file> peek_file(directory const& dir, native_string_view file_name) {
		if(dir.parent_system) {
			for(size_t i = dir.parent_system->ordered_roots.size(); i-- > 0; ) {
				native_string full_path = dir.parent_system->ordered_roots[i] + dir.relative_path + NATIVE('\\') + native_string(file_name);
				DWORD dwAttrib = GetFileAttributesW(full_path.c_str());
				if(dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
					return std::optional<unopened_file>(unopened_file(full_path, file_name));
				}
			}
		} else {
			native_string full_path = dir.relative_path + NATIVE('\\') + native_string(file_name);
			DWORD dwAttrib = GetFileAttributesW(full_path.c_str());
			if(dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
				return std::optional<unopened_file>(unopened_file(full_path, file_name));
			}
		}
		return std::optional<unopened_file>{};
	}

	native_string get_full_name(unopened_file const& f) {
		return f.absolute_path;
	}

	native_string get_full_name(file const& f) {
		return f.absolute_path;
	}

	void write_file(directory const& dir, native_string_view file_name, char const* file_data, uint32_t file_size) {
		if(dir.parent_system)
			std::abort();
		
		native_string full_path = dir.relative_path + NATIVE('\\') + native_string(file_name);

		HANDLE file_handle = CreateFileW(full_path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
		if(file_handle != INVALID_HANDLE_VALUE) {
			WriteFile(file_handle, file_data, DWORD(file_size), nullptr, nullptr);
			SetEndOfFile(file_handle);
			CloseHandle(file_handle);
		}
	}

	file_contents view_contents(file const& f) {
		return f.content;
	}

	directory get_or_create_settings_directory() {
		wchar_t* local_path_out = nullptr;
		std::wstring base_path;
		if(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &local_path_out) == S_OK) {
			base_path = std::wstring(local_path_out) + L"\\Project Alice";
		}
		CoTaskMemFree(local_path_out);
		if(base_path.length() > 0) {
			CreateDirectoryW(base_path.c_str(), nullptr);
		}
		return directory(nullptr, base_path);
	}

	directory get_or_create_save_game_directory() {
		wchar_t* local_path_out = nullptr;
		std::wstring base_path;
		if(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &local_path_out) == S_OK) {
			base_path = std::wstring(local_path_out) + L"\\Project Alice";
		}
		CoTaskMemFree(local_path_out);
		if(base_path.length() > 0) {
			CreateDirectoryW(base_path.c_str(), nullptr);
			base_path += NATIVE("\\saved games");
			CreateDirectoryW(base_path.c_str(), nullptr);
		}
		return directory(nullptr, base_path);
	}

	directory get_or_create_scenario_directory() {
		wchar_t* local_path_out = nullptr;
		std::wstring base_path;
		if(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &local_path_out) == S_OK) {
			base_path = std::wstring(local_path_out) + L"\\Project Alice";
		}
		CoTaskMemFree(local_path_out);
		if(base_path.length() > 0) {
			CreateDirectoryW(base_path.c_str(), nullptr);
			base_path += NATIVE("\\scenarios");
			CreateDirectoryW(base_path.c_str(), nullptr);
		}
		return directory(nullptr, base_path);
	}

	namespace win_internal {
		wchar_t win1250toUTF16(char in) {
			constexpr static char16_t converted[256] =
				//       0       1         2         3         4         5         6         7         8         9         A         B         C         D         E         F
				/*0*/{ u' ' ,  u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u'\t' ,   u'\n' ,   u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,
				/*1*/  u' ' ,  u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,
				/*2*/  u' ' ,  u'!' ,    u'\"',    u'#' ,    u'$' ,    u'%' ,    u'&' ,    u'\'',    u'(' ,    u')' ,    u'*' ,    u'+' ,    u',' ,    u'-' ,    u'.' ,    u'/' ,
				/*3*/  u'0' ,  u'1' ,    u'2' ,    u'3' ,    u'4' ,    u'5' ,    u'6' ,    u'7' ,    u'8' ,    u'9' ,    u':' ,    u';' ,    u'<' ,    u'=' ,    u'>' ,    u'?' ,
				/*4*/  u'@' ,  u'A' ,    u'B' ,    u'C' ,    u'D' ,    u'E' ,    u'F' ,    u'G' ,    u'H' ,    u'I' ,    u'J' ,    u'K' ,    u'L' ,    u'M' ,    u'N' ,    u'O' ,
				/*5*/  u'P' ,  u'Q' ,    u'R' ,    u'S' ,    u'T' ,    u'U' ,    u'V' ,    u'W' ,    u'X' ,    u'Y' ,    u'Z' ,    u'[' ,    u'\\',    u']' ,    u'^' ,    u'_' ,
				/*6*/  u'`' ,  u'a' ,    u'b' ,    u'c' ,    u'd' ,    u'e' ,    u'f' ,    u'g' ,    u'h' ,    u'i' ,    u'j' ,    u'k' ,    u'l',     u'm' ,    u'n' ,    u'o' ,
				/*7*/  u'p' ,  u'q' ,    u'r' ,    u's' ,    u't' ,    u'u' ,    u'v' ,    u'w' ,    u'x' ,    u'y' ,    u'z' ,    u'{' ,    u'|',     u'}' ,    u'~' ,    u' ' ,
				/*8*/u'\u20AC',u' ',     u'\u201A',u' ',     u'\u201E',u'\u2026',u'\u2020',u'\u2021',u' ',     u'\u2030',u'\u0160',u'\u2039',u'\u015A',u'\u0164',u'\u017D',u'\u0179',
				/*9*/u' ',     u'\u2018',u'\u2019',u'\u201C',u'\u201D',u'\u2022',u'\u2013',u'\u2014',u' ',     u'\u2122',u'\u0161',u'\u203A',u'\u015B',u'\u0165',u'\u017E',u'\u017A',
				/*A*/u'\u00A0',     u'\u02C7',u'\u02D8',u'\u00A2',u'\u00A3',u'\u0104',u'\u00A6',u'\u00A7',u'\u00A8',u'\u00A9',u'\u015E',u'\u00AB',u'\u00AC',u'-',     u'\u00AE',u'\u017B',
				/*B*/u'\u00B0',u'\u00B1',u'\u02DB',u'\u0142',u'\u00B4',u'\u00B5',u'\u00B6',u'\u00B7',u'\u00B8',u'\u0105',u'\u015F',u'\u00BB',u'\u013D',u'\u02DD',u'\u013E',u'\u017C',
				/*C*/u'\u0154',u'\u00C1',u'\u00C2',u'\u0102',u'\u00C4',u'\u0139',u'\u0106',u'\u00C7',u'\u010C',u'\u00C9',u'\u0118',u'\u00CB',u'\u011A',u'\u00CD',u'\u00CE',u'\u010E',
				/*D*/u'\u0110',u'\u0143',u'\u0147',u'\u00D3',u'\u00D4',u'\u0150',u'\u00D6',u'\u00D7',u'\u0158',u'\u016E',u'\u00DA',u'\u0170',u'\u00DC',u'\u00DD',u'\u0162',u'\u00DF',
				/*E*/u'\u0115',u'\u00E1',u'\u00E2',u'\u0103',u'\u00E4',u'\u013A',u'\u0107',u'\u00E7',u'\u010D',u'\u00E9',u'\u0119',u'\u00EB',u'\u011B',u'\u00ED',u'\u00EE',u'\u010F',
				/*F*/u'\u0111',u'\u0144',u'\u0148',u'\u00F3',u'\u00F4',u'\u0151',u'\u00F6',u'\u00F7',u'\u0159',u'\u016F',u'\u00FA',u'\u0171',u'\u00FC',u'\u00FD',u'\u0163',u'\u02D9'
			};

			return wchar_t(converted[(uint8_t)in]);
		}
	}

	native_string win1250_to_native(std::string_view data_in) {
		native_string result;
		for(auto ch : data_in) {
			result += win_internal::win1250toUTF16(ch);
		}
		return result;
	}

	native_string utf8_to_native(std::string_view str) {
		WCHAR* buffer = new WCHAR[str.length() * 2];

		auto chars_written = MultiByteToWideChar(
			CP_UTF8,
			MB_PRECOMPOSED,
			str.data(),
			int32_t(str.length()),
			buffer,
			int32_t(str.length() * 2)
		);

		std::wstring result(buffer, size_t(chars_written));

		delete[] buffer;
		return result;
	}

	std::string native_to_utf8(native_string_view str) {
		char* buffer = new char[str.length() * 4];

		auto chars_written = WideCharToMultiByte(
			CP_UTF8,
			0,
			str.data(),
			int32_t(str.length()),
			buffer,
			int32_t(str.length() * 4),
			nullptr,
			nullptr
		);

		std::string result(buffer, size_t(chars_written));

		delete[] buffer;
		return result;
	}

}
