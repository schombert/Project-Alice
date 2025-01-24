#include "simple_fs.hpp"
#include "simple_fs_types_win.hpp"
#include "text.hpp"

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
	file_handle = CreateFileW(full_path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
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
	std::optional<file> result(file{f.absolute_path});
	if(!result->content.data) {
		result = std::optional<file>{};
	}
	return result;
}

void reset(file_system& fs) {
	fs.ordered_roots.clear();
	fs.ignored_paths.clear();
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
		result += NATIVE(";") + str;
	}
	result += NATIVE("?");
	for(auto const& replace_path : fs.ignored_paths) {
		result += replace_path + NATIVE(";");
	}
	return result;
}

void restore_state(file_system& fs, native_string_view data) {
	simple_fs::reset(fs);
	auto break_position = std::find(data.data(), data.data() + data.length(), NATIVE('?'));
	// Parse ordered roots
	{
		auto position = data.data() + 1;
		auto end = break_position;
		while(position < end) {
			auto next_semicolon = std::find(position, end, NATIVE(';'));
			fs.ordered_roots.emplace_back(position, next_semicolon);
			position = next_semicolon + 1;
		}
	}
	// Replaced paths
	{
		auto position = break_position + 1;
		auto end = data.data() + data.length();
		while(position < end) {
			auto next_semicolon = std::find(position, end, NATIVE(';'));
			fs.ignored_paths.emplace_back(position, next_semicolon);
			position = next_semicolon + 1;
		}
	}
}

namespace impl {
bool contains_non_ascii(native_char const* str) {
	for(auto c = str; *c != 0; ++c) {
		if(int32_t(*c) > 127 || int32_t(*c) < 0)
			return true;
	}
	return false;
}
} // namespace impl

std::vector<unopened_file> list_files(directory const& dir, native_char const* extension) {
	std::vector<unopened_file> accumulated_results;
	if(dir.parent_system) {
		for(size_t i = dir.parent_system->ordered_roots.size(); i-- > 0;) {
			auto const dir_path = dir.parent_system->ordered_roots[i] + dir.relative_path;
			auto const appended_path = dir_path + NATIVE("\\*") + extension;

			if(simple_fs::is_ignored_path(*dir.parent_system, appended_path)) {
				continue;
			}

			WIN32_FIND_DATAW find_result;
			auto find_handle = FindFirstFileExW(appended_path.c_str(), FindExInfoBasic, &find_result, FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);
			if(find_handle != INVALID_HANDLE_VALUE) {
				do {
					if(!(find_result.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !impl::contains_non_ascii(find_result.cFileName)) {
						if(auto search_result = std::find_if(accumulated_results.begin(), accumulated_results.end(),
									 [n = find_result.cFileName](auto const& f) { return f.file_name.compare(n) == 0; });
								search_result == accumulated_results.end()) {

							accumulated_results.emplace_back(dir.parent_system->ordered_roots[i] + dir.relative_path + NATIVE("\\") +
																									 find_result.cFileName,
									find_result.cFileName);
						}
					}
				} while(FindNextFileW(find_handle, &find_result) != 0);
				FindClose(find_handle);
			}
		}
	} else {
		auto const appended_path = dir.relative_path + NATIVE("\\*") + extension;
		WIN32_FIND_DATAW find_result;
		auto find_handle = FindFirstFileExW(appended_path.c_str(), FindExInfoBasic, &find_result, FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);
		if(find_handle != INVALID_HANDLE_VALUE) {
			do {
				if(!(find_result.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !impl::contains_non_ascii(find_result.cFileName)) {
					accumulated_results.emplace_back(dir.relative_path + NATIVE("\\") + find_result.cFileName, find_result.cFileName);
				}
			} while(FindNextFileW(find_handle, &find_result) != 0);
			FindClose(find_handle);
		}
	}
	std::sort(accumulated_results.begin(), accumulated_results.end(), [](unopened_file const& a, unopened_file const& b) {
		return std::lexicographical_compare(std::begin(a.file_name), std::end(a.file_name), std::begin(b.file_name),
				std::end(b.file_name),
				[](native_char const& char1, native_char const& char2) { return tolower(char1) < tolower(char2); });
	});
	return accumulated_results;
}
std::vector<directory> list_subdirectories(directory const& dir) {
	std::vector<directory> accumulated_results;
	if(dir.parent_system) {
		for(size_t i = dir.parent_system->ordered_roots.size(); i-- > 0;) {
			auto const dir_path = dir.parent_system->ordered_roots[i] + dir.relative_path;
			auto const appended_path = dir_path + NATIVE("\\*");
			if(simple_fs::is_ignored_path(*dir.parent_system, appended_path)) {
				continue;
			}
			WIN32_FIND_DATAW find_result;
			auto find_handle = FindFirstFileExW(appended_path.c_str(), FindExInfoBasic, &find_result, FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);
			if(find_handle != INVALID_HANDLE_VALUE) {
				do {
					if((find_result.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !impl::contains_non_ascii(find_result.cFileName)) {
						native_string const rel_name = dir.relative_path + NATIVE("\\") + find_result.cFileName;
						if(find_result.cFileName[0] != NATIVE('.') &&
								std::find_if(accumulated_results.begin(), accumulated_results.end(),
										[&rel_name](auto const& s) { return s.relative_path.compare(rel_name) == 0; }) == accumulated_results.end()) {
							accumulated_results.emplace_back(dir.parent_system, rel_name);
						}
					}
				} while(FindNextFileW(find_handle, &find_result) != 0);
				FindClose(find_handle);
			}
		}
	} else {
		auto const appended_path = dir.relative_path + NATIVE("\\*");
		WIN32_FIND_DATAW find_result;
		auto find_handle = FindFirstFileExW(appended_path.c_str(), FindExInfoBasic, &find_result, FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);
		if(find_handle != INVALID_HANDLE_VALUE) {
			do {
				if((find_result.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !impl::contains_non_ascii(find_result.cFileName)) {
					native_string const rel_name = dir.relative_path + NATIVE("\\") + find_result.cFileName;
					if(find_result.cFileName[0] != NATIVE('.')) {
						accumulated_results.emplace_back(nullptr, rel_name);
					}
				}
			} while(FindNextFileW(find_handle, &find_result) != 0);
			FindClose(find_handle);
		}
	}
	std::sort(accumulated_results.begin(), accumulated_results.end(), [](directory const& a, directory const& b) {
		return std::lexicographical_compare(std::begin(a.relative_path), std::end(a.relative_path), std::begin(b.relative_path),
				std::end(b.relative_path),
				[](native_char const& char1, native_char const& char2) { return tolower(char1) < tolower(char2); });
	});
	return accumulated_results;
}

directory open_directory(directory const& dir, native_string_view directory_name) {
	return directory(dir.parent_system, dir.relative_path + NATIVE('\\') + native_string(directory_name));
}

native_string get_full_name(directory const& dir) {
	return dir.relative_path;
}

std::optional<native_string> get_path_to_file(directory const& dir, native_string_view file_name) {
	if(dir.parent_system) {
		for(size_t i = dir.parent_system->ordered_roots.size(); i-- > 0;) {
			native_string dir_path = dir.parent_system->ordered_roots[i] + dir.relative_path;
			native_string full_path = dir_path + NATIVE('\\') + native_string(file_name);
			return full_path;
		}
	} else {
		native_string full_path = dir.relative_path + NATIVE('\\') + native_string(file_name);
		return full_path;
	}
	return std::optional<native_string>{};
}

std::optional<file> open_file(directory const& dir, native_string_view file_name) {
	if(dir.parent_system) {
		for(size_t i = dir.parent_system->ordered_roots.size(); i-- > 0;) {
			native_string dir_path = dir.parent_system->ordered_roots[i] + dir.relative_path;
			native_string full_path = dir_path + NATIVE('\\') + native_string(file_name);
			if(simple_fs::is_ignored_path(*dir.parent_system, full_path)) {
				continue;
			}
			HANDLE file_handle = CreateFileW(full_path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
			if(file_handle != INVALID_HANDLE_VALUE) {
				return std::optional<file>(file(file_handle, full_path));
			}
		}
	} else {
		native_string full_path = dir.relative_path + NATIVE('\\') + native_string(file_name);
		HANDLE file_handle = CreateFileW(full_path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
		if(file_handle != INVALID_HANDLE_VALUE) {
			return std::optional<file>(file(file_handle, full_path));
		}
	}
	return std::optional<file>{};
}

std::optional<unopened_file> peek_file(directory const& dir, native_string_view file_name) {
	if(dir.parent_system) {
		for(size_t i = dir.parent_system->ordered_roots.size(); i-- > 0;) {
			native_string dir_path = dir.parent_system->ordered_roots[i] + dir.relative_path;
			native_string full_path = dir_path + NATIVE('\\') + native_string(file_name);
			if(simple_fs::is_ignored_path(*dir.parent_system, full_path)) {
				continue;
			}
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

void add_ignore_path(file_system& fs, native_string_view replaced_path) {
	fs.ignored_paths.emplace_back(correct_slashes(replaced_path));
}

std::vector<native_string> list_roots(file_system const& fs) {
	return fs.ordered_roots;
}

bool is_ignored_path(file_system const& fs, native_string_view path) {
	for(auto const& replace_path : fs.ignored_paths) {
		if(path.starts_with(replace_path))
			return true;
	}
	return false;
}

native_string get_full_name(unopened_file const& f) {
	return f.absolute_path;
}

native_string get_file_name(unopened_file const& f) {
	return f.file_name;
}

native_string get_full_name(file const& f) {
	return f.absolute_path;
}

void write_file(directory const& dir, native_string_view file_name, char const* file_data, uint32_t file_size) {
	if(dir.parent_system)
		std::abort();

	native_string full_path = dir.relative_path + NATIVE('\\') + native_string(file_name);
	HANDLE file_handle = CreateFileW(full_path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
	if(file_handle != INVALID_HANDLE_VALUE) {
		DWORD written_bytes = 0;
		WriteFile(file_handle, file_data, DWORD(file_size), &written_bytes, nullptr);
		(void)written_bytes;
		SetEndOfFile(file_handle);
		CloseHandle(file_handle);
	}
}

void append_file(directory const& dir, native_string_view file_name, char const* file_data, uint32_t file_size) {
	if(dir.parent_system)
		std::abort();

	native_string full_path = dir.relative_path + NATIVE('\\') + native_string(file_name);
	HANDLE file_handle = CreateFileW(full_path.c_str(), FILE_APPEND_DATA, 0, nullptr, OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
	if(file_handle != INVALID_HANDLE_VALUE) {
		DWORD written_bytes = 0;
		WriteFile(file_handle, file_data, DWORD(file_size), &written_bytes, nullptr);
		(void)written_bytes;
		SetEndOfFile(file_handle);
		CloseHandle(file_handle);
	}
}

file_contents view_contents(file const& f) {
	return f.content;
}

directory get_or_create_settings_directory() {
	wchar_t* local_path_out = nullptr;
	native_string base_path;
	if(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &local_path_out) == S_OK) {
		base_path = native_string(local_path_out) + NATIVE("\\Project Alice");
	}
	CoTaskMemFree(local_path_out);
	if(base_path.length() > 0) {
		CreateDirectoryW(base_path.c_str(), nullptr);
	}
	return directory(nullptr, base_path);
}

directory get_or_create_save_game_directory() {
	wchar_t* local_path_out = nullptr;
	native_string base_path;
	if(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &local_path_out) == S_OK) {
		base_path = native_string(local_path_out) + NATIVE("\\Project Alice");
	}
	CoTaskMemFree(local_path_out);
	if(base_path.length() > 0) {
		CreateDirectoryW(base_path.c_str(), nullptr);
		base_path += NATIVE("\\saved games");
		CreateDirectoryW(base_path.c_str(), nullptr);
	}
	return directory(nullptr, base_path);
}

directory get_or_create_root_documents() {
	wchar_t* local_path_out = nullptr;
	native_string base_path;
	if(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &local_path_out) == S_OK) {
		base_path = native_string(local_path_out) + NATIVE("\\Project Alice");
	}
	CoTaskMemFree(local_path_out);
	if(base_path.length() > 0) {
		CreateDirectoryW(base_path.c_str(), nullptr);
	}
	return directory(nullptr, base_path);
}

directory get_or_create_templates_directory() {
	native_char* local_path_out = nullptr;
	native_string base_path;
	if(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &local_path_out) == S_OK) {
		base_path = native_string(local_path_out) + NATIVE("\\Project Alice");
	}
	CoTaskMemFree(local_path_out);
	if(base_path.length() > 0) {
		CreateDirectoryW(base_path.c_str(), nullptr);
		base_path += NATIVE("\\templates");
		CreateDirectoryW(base_path.c_str(), nullptr);
	}
	return directory(nullptr, base_path);
}

directory get_or_create_oos_directory() {
	native_char* local_path_out = nullptr;
	native_string base_path;
	if(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &local_path_out) == S_OK) {
		base_path = native_string(local_path_out) + NATIVE("\\Project Alice");
	}
	CoTaskMemFree(local_path_out);
	if(base_path.length() > 0) {
		CreateDirectoryW(base_path.c_str(), nullptr);
		base_path += NATIVE("\\oos");
		CreateDirectoryW(base_path.c_str(), nullptr);
	}
	return directory(nullptr, base_path);
}

directory get_or_create_scenario_directory() {
	native_char* local_path_out = nullptr;
	native_string base_path;
	if(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &local_path_out) == S_OK) {
		base_path = native_string(local_path_out) + NATIVE("\\Project Alice");
	}
	CoTaskMemFree(local_path_out);
	if(base_path.length() > 0) {
		CreateDirectoryW(base_path.c_str(), nullptr);
		base_path += NATIVE("\\scenarios");
		CreateDirectoryW(base_path.c_str(), nullptr);
	}
	return directory(nullptr, base_path);
}

directory get_or_create_data_dumps_directory() {
	native_char* local_path_out = nullptr;
	native_string base_path;
	if(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &local_path_out) == S_OK) {
		base_path = native_string(local_path_out) + NATIVE("\\Project Alice");
	}
	CoTaskMemFree(local_path_out);
	if(base_path.length() > 0) {
		CreateDirectoryW(base_path.c_str(), nullptr);
		base_path += NATIVE("\\data_dumps");
		CreateDirectoryW(base_path.c_str(), nullptr);
	}
	return directory(nullptr, base_path);
}

char16_t win1250toUTF16(char in) {
	constexpr static char16_t converted[256] =
		//		0		1		2		3		4		5		6		7		8		9		A		B		C		D		E		F
		/*0*/{ u' ',		u'\u0001', u'\u0002', u'\u0003', u'\u0004', u' ', u' ', u' ', u' ', u'\t', u'\n', u' ', u' ', u' ', u' ', u' ',
		/*1*/	u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',
		/*2*/	u' ',		u'!',		u'\"',		u'#',		u'$',		u'%',	u'&',		u'\'',		u'(',		u')',		u'*',		u'+',		u',',		u'-',		u'.',		u'/',
		/*3*/ u'0', u'1', u'2', u'3', u'4', u'5', u'6', u'7', u'8', u'9', u':', u';', u'<', u'=', u'>', u'?',
		/*4*/ u'@', u'A', u'B', u'C', u'D', u'E', u'F', u'G', u'H', u'I', u'J', u'K', u'L', u'M', u'N', u'O',
		/*5*/ u'P', u'Q', u'R', u'S', u'T', u'U', u'V', u'W', u'X', u'Y', u'Z', u'[', u'\\', u']', u'^', u'_',
		/*6*/ u'`', u'a', u'b', u'c', u'd', u'e', u'f', u'g', u'h', u'i', u'j', u'k', u'l', u'm', u'n', u'o',
		/*7*/ u'p', u'q', u'r', u's', u't', u'u', u'v', u'w', u'x', u'y', u'z', u'{', u'|', u'}', u'~', u' ',
		/*8*/ u'\u20AC', u' ', u'\u201A', u' ', u'\u201E', u'\u2026', u'\u2020', u'\u2021', u' ', u'\u2030', u'\u0160', u'\u2039', u'\u015A', u'\u0164', u'\u017D', u'\u0179',
		/*9*/ u' ', u'\u2018', u'\u2019', u'\u201C', u'\u201D', u'\u2022', u'\u2013', u'\u2014', u' ', u'\u2122', u'\u0161',
				u'\u203A', u'\u015B', u'\u0165', u'\u017E', u'\u017A',
		/*A*/ u'\u00A0', u'\u02C7', u'\u02D8', u'\u00A2', u'\u00A3', u'\u0104', u'\u00A6', u'\u00A7', u'\u00A8', u'\u00A9',
				u'\u015E', u'\u00AB', u'\u00AC', u'-', u'\u00AE', u'\u017B',
		/*B*/ u'\u00B0', u'\u00B1', u'\u02DB', u'\u0142', u'\u00B4', u'\u00B5', u'\u00B6', u'\u00B7', u'\u00B8', u'\u0105',
				u'\u015F', u'\u00BB', u'\u013D', u'\u02DD', u'\u013E', u'\u017C',
		/*C*/ u'\u0154', u'\u00C1', u'\u00C2', u'\u0102', u'\u00C4', u'\u0139', u'\u0106', u'\u00C7', u'\u010C', u'\u00C9',
				u'\u0118', u'\u00CB', u'\u011A', u'\u00CD', u'\u00CE', u'\u010E',
		/*D*/ u'\u0110', u'\u0143', u'\u0147', u'\u00D3', u'\u00D4', u'\u0150', u'\u00D6', u'\u00D7', u'\u0158', u'\u016E',
				u'\u00DA', u'\u0170', u'\u00DC', u'\u00DD', u'\u0162', u'\u00DF',
		/*E*/ u'\u0115', u'\u00E1', u'\u00E2', u'\u0103', u'\u00E4', u'\u013A', u'\u0107', u'\u00E7', u'\u00E8', u'\u00E9',
				u'\u0119', u'\u00EB', u'\u011B', u'\u00ED', u'\u00EE', u'\u010F',
		/*F*/ u'\u0111', u'\u0144', u'\u0148', u'\u00F3', u'\u00F4', u'\u0151', u'\u00F6', u'\u00F7', u'\u0159', u'\u016F',
				u'\u00FA', u'\u0171', u'\u00FC', u'\u00FD', u'\u0163', u'\u02D9' };

	return converted[(uint8_t)in];
}

native_string win1250_to_native(std::string_view data_in) {
	native_string result;
	for(auto ch : data_in) {
		result += native_char(win1250toUTF16(ch));
	}
	return result;
}

native_string utf8_to_native(std::string_view str) {
	if(str.size() > 0) {
		auto buffer = std::unique_ptr<WCHAR[]>(new WCHAR[str.length() * 2]);
		auto chars_written = MultiByteToWideChar(CP_UTF8, 0, str.data(), int32_t(str.length()), buffer.get(), int32_t(str.length() * 2));
		return native_string(buffer.get(), size_t(chars_written));
	}
	return native_string(NATIVE(""));
}

std::string native_to_utf8(native_string_view str) {
	if(str.size() > 0) {
		auto buffer = std::unique_ptr<char[]>(new char[str.length() * 4]);
		auto chars_written = WideCharToMultiByte(CP_UTF8, 0, str.data(), int32_t(str.length()), buffer.get(), int32_t(str.length() * 4), NULL, NULL);
		return std::string(buffer.get(), size_t(chars_written));
	}
	return std::string("");
}

std::string remove_double_backslashes(std::string_view data_in) {
	std::string res;
	res.reserve(data_in.size());
	for(uint32_t i = 0; i < data_in.size(); ++i) {
		if(data_in[i] == '\\') {
			res += '\\';
			if(i + 1 < data_in.size() && data_in[i + 1] == '\\')
				++i;
		} else {
			res += data_in[i];
		}
	}
	return res;
}

native_string correct_slashes(native_string_view path) {
	native_string res;
	res.reserve(path.size());
	for(size_t i = 0; i < path.size(); i++) {
		res += path[i] == '/' ? '\\' : path[i];
	}
	return res;
}
} // namespace simple_fs
