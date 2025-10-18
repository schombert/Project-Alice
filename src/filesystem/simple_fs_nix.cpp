#include <dirent.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <codecvt>
#include <locale>

#include "simple_fs.hpp"
#include "text.hpp"

namespace simple_fs {
file::~file() {
#if defined(_GNU_SOURCE) || defined(_DEFAULT_SOURCE) || defined(_BSD_SOURCE) || defined(_SVID_SOURCE)
	if(mapping_handle) {
		if(munmap(mapping_handle, content.file_size) == -1) {
			// error
		}
	}
#else
	if(file_buffer) {
		free(file_buffer);
	}
#endif
	if(file_descriptor != -1) {
		close(file_descriptor);
	}
}

file::file(file&& other) noexcept {
	file_descriptor = other.file_descriptor;
#if defined(_GNU_SOURCE) || defined(_DEFAULT_SOURCE) || defined(_BSD_SOURCE) || defined(_SVID_SOURCE)
	mapping_handle = other.mapping_handle;
#else
	file_buffer = other.file_buffer;
#endif
	content = other.content;
	absolute_path = std::move(other.absolute_path);

	other.file_descriptor = -1;
#if defined(_GNU_SOURCE) || defined(_DEFAULT_SOURCE) || defined(_BSD_SOURCE) || defined(_SVID_SOURCE)
	other.mapping_handle = nullptr;
#else
	other.file_buffer = nullptr;
#endif
}
void file::operator=(file&& other) noexcept {
	file_descriptor = other.file_descriptor;
#if defined(_GNU_SOURCE) || defined(_DEFAULT_SOURCE) || defined(_BSD_SOURCE) || defined(_SVID_SOURCE)
	mapping_handle = other.mapping_handle;
#else
	file_buffer = other.file_buffer;
#endif
	content = other.content;
	absolute_path = std::move(other.absolute_path);

	other.file_descriptor = -1;
#if defined(_GNU_SOURCE) || defined(_DEFAULT_SOURCE) || defined(_BSD_SOURCE) || defined(_SVID_SOURCE)
	other.mapping_handle = nullptr;
#else
	other.file_buffer = nullptr;
#endif
}

file::file(native_string const& full_path) {
	file_descriptor = open(full_path.c_str(), O_RDONLY | O_NONBLOCK);
	if(file_descriptor != -1) {
		absolute_path = full_path;
		struct stat sb;
		if(fstat(file_descriptor, &sb) != -1) {
			content.file_size = sb.st_size;
#if _POSIX_C_SOURCE >= 200112L
			posix_fadvise(file_descriptor, 0, static_cast<off_t>(content.file_size), POSIX_FADV_WILLNEED);
#endif
#if defined(_GNU_SOURCE) || defined(_DEFAULT_SOURCE) || defined(_BSD_SOURCE) || defined(_SVID_SOURCE)
			mapping_handle = mmap(0, content.file_size, PROT_READ, MAP_PRIVATE, file_descriptor, 0);
			if(mapping_handle == MAP_FAILED) {
				// error
			}
			content.data = static_cast<char*>(mapping_handle);
#else
			file_buffer = malloc(content.file_size);
			read(file_descriptor, file_buffer, content.file_size);
			content.data = static_cast<char*>(file_buffer);
#endif
		}
	}
}
file::file(int file_descriptor, native_string const& full_path) : file_descriptor(file_descriptor) {
	absolute_path = full_path;
	struct stat sb;
	if(fstat(file_descriptor, &sb) != -1) {
		content.file_size = sb.st_size;
#if defined(_GNU_SOURCE) || defined(_DEFAULT_SOURCE) || defined(_BSD_SOURCE) || defined(_SVID_SOURCE)
		mapping_handle = mmap(0, content.file_size, PROT_READ, MAP_PRIVATE, file_descriptor, 0);
		if(mapping_handle == MAP_FAILED) {
			// error
		}
		content.data = static_cast<char*>(mapping_handle);
#else
		file_buffer = malloc(content.file_size);
		read(file_descriptor, file_buffer, content.file_size);
		content.data = static_cast<char*>(file_buffer);
#endif
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
	char module_name[1024];
	ssize_t path_used = -1;
	if((path_used = readlink("/proc/self/exe", module_name, sizeof(module_name) - 1)) != -1) {
		while(path_used >= 0 && module_name[path_used] != '/') {
			module_name[path_used] = '\0';
			--path_used;
		}
	} else {
		char tmpbuf[512];
		snprintf(tmpbuf, sizeof(tmpbuf), "/proc/%u/exe", unsigned(getpid()));
		if((path_used = readlink(tmpbuf, module_name, sizeof(module_name) - 1)) != -1) {
			while(path_used >= 0 && module_name[path_used] != '/') {
				module_name[path_used] = '\0';
				--path_used;
			}
		} else {
			// error
			std::abort();
		}
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
			auto const appended_path = dir.parent_system->ordered_roots[i] + dir.relative_path;
			if(simple_fs::is_ignored_path(*dir.parent_system, appended_path + NATIVE("/"))) {
				continue;
			}

			DIR* d = opendir(appended_path.c_str());
			if(d) {
				struct dirent* dir_ent = nullptr;
				while((dir_ent = readdir(d)) != nullptr) {
					// Check if it's a file. Not POSIX standard but included in Linux
					if(dir_ent->d_type != DT_REG)
						continue;

					// Check if the file is of the right extension
					if(extension && extension[0] != 0) {
						char* dot = strrchr(dir_ent->d_name, '.');
						if(!dot || dot == dir_ent->d_name)
							continue;
						if(strcmp(dot, extension))
							continue;
					}

					if(impl::contains_non_ascii(dir_ent->d_name))
						continue;

					auto search_result = std::find_if(accumulated_results.begin(), accumulated_results.end(),
							[n = dir_ent->d_name](auto const& f) { return f.file_name.compare(n) == 0; });
					if(search_result == accumulated_results.end()) {
						accumulated_results.emplace_back(
								dir.parent_system->ordered_roots[i] + dir.relative_path + NATIVE("/") + dir_ent->d_name, dir_ent->d_name);
					}
				}
				closedir(d);
			}
		}
	} else {
		auto const appended_path = dir.relative_path;
		DIR* d = opendir(appended_path.c_str());
		if(d) {
			struct dirent* dir_ent = nullptr;
			while((dir_ent = readdir(d)) != nullptr) {
				// Check if it's a file. Not POSIX standard but included in Linux
				if(dir_ent->d_type != DT_REG)
					continue;

				// Check if the file is of the right extension
				if(extension && extension[0] != 0) {
					char* dot = strrchr(dir_ent->d_name, '.');
					if(!dot || dot == dir_ent->d_name)
						continue;
					if(strcmp(dot, extension))
						continue;
				}

				if(impl::contains_non_ascii(dir_ent->d_name))
					continue;

				accumulated_results.emplace_back(dir.relative_path + NATIVE("/") + dir_ent->d_name, dir_ent->d_name);
			}
			closedir(d);
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
			auto const appended_path = dir.parent_system->ordered_roots[i] + dir.relative_path;
			if(simple_fs::is_ignored_path(*dir.parent_system, appended_path + NATIVE("/"))) {
				continue;
			}
			DIR* d = opendir(appended_path.c_str());
			if(d) {
				struct dirent* dir_ent = nullptr;
				while((dir_ent = readdir(d)) != nullptr) {
					// Check if it's a directory. Not POSIX standard but included in Linux
					if(dir_ent->d_type != DT_DIR)
						continue;

					if(impl::contains_non_ascii(dir_ent->d_name))
						continue;

					native_string const rel_name = dir.relative_path + NATIVE("/") + dir_ent->d_name;
					if(dir_ent->d_name[0] != NATIVE('.')) {
						auto search_result = std::find_if(accumulated_results.begin(), accumulated_results.end(),
								[&rel_name](auto const& s) { return s.relative_path.compare(rel_name) == 0; });
						if(search_result == accumulated_results.end()) {
							accumulated_results.emplace_back(dir.parent_system, rel_name);
						}
					}
				}
				closedir(d);
			}
		}
	} else {
		auto const appended_path = dir.relative_path;
		DIR* d = opendir(appended_path.c_str());
		if(d) {
			struct dirent* dir_ent = nullptr;
			while((dir_ent = readdir(d)) != nullptr) {
				// Check if it's a directory. Not POSIX standard but included in Linux
				if(dir_ent->d_type != DT_DIR)
					continue;

				if(impl::contains_non_ascii(dir_ent->d_name))
					continue;

				native_string const rel_name = dir.relative_path + NATIVE("/") + dir_ent->d_name;
				if(dir_ent->d_name[0] != NATIVE('.')) {
					accumulated_results.emplace_back(nullptr, rel_name);
				}
			}
			closedir(d);
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
	return directory(dir.parent_system, dir.relative_path + NATIVE('/') + native_string(directory_name));
}

std::optional<file> open_file(directory const& dir, native_string_view file_name) {
	if(dir.parent_system) {
		for(size_t i = dir.parent_system->ordered_roots.size(); i-- > 0;) {
			native_string dir_path = dir.parent_system->ordered_roots[i] + dir.relative_path;
			native_string full_path = dir_path + NATIVE('/') + native_string(file_name);
			if(simple_fs::is_ignored_path(*dir.parent_system, full_path)) {
				continue;
			}
			int file_descriptor = open(full_path.c_str(), O_RDONLY | O_NONBLOCK);
			if(file_descriptor != -1) {
				return std::optional<file>(file(file_descriptor, full_path));
			}
		}
	} else {
		native_string full_path = dir.relative_path + NATIVE('/') + native_string(file_name);
		int file_descriptor = open(full_path.c_str(), O_RDONLY | O_NONBLOCK);
		if(file_descriptor != -1) {
			return std::optional<file>(file(file_descriptor, full_path));
		}
	}
	return std::optional<file>{};
}

std::optional<file> open_file(directory const& dir, std::vector<native_string_view> file_names) {
	if(dir.parent_system) {
		for(size_t i = dir.parent_system->ordered_roots.size(); i-- > 0;) {
			for(auto file_name : file_names) {
				native_string dir_path = dir.parent_system->ordered_roots[i] + dir.relative_path;
				native_string full_path = dir_path + NATIVE('/') + native_string(file_name);
				if(simple_fs::is_ignored_path(*dir.parent_system, full_path)) {
					continue;
				}
				int file_descriptor = open(full_path.c_str(), O_RDONLY | O_NONBLOCK);
				if(file_descriptor != -1) {
					return std::optional<file>(file(file_descriptor, full_path));
				}
			}
		}
	} else {
		for(auto file_name : file_names) {
			native_string full_path = dir.relative_path + NATIVE('/') + native_string(file_name);
			int file_descriptor = open(full_path.c_str(), O_RDONLY | O_NONBLOCK);
			if(file_descriptor != -1) {
				return std::optional<file>(file(file_descriptor, full_path));
			}
		}
	}
	return std::optional<file>{};
}

std::optional<unopened_file> peek_file(directory const& dir, native_string_view file_name) {
	if(dir.parent_system) {
		for(size_t i = dir.parent_system->ordered_roots.size(); i-- > 0;) {
			native_string full_path = dir.parent_system->ordered_roots[i] + dir.relative_path + NATIVE('/') + native_string(file_name);
			if(simple_fs::is_ignored_path(*dir.parent_system, full_path)) {
				continue;
			}
			struct stat stat_buf;
			int result = stat(full_path.c_str(), &stat_buf);
			if(result != -1 && S_ISREG(stat_buf.st_mode)) {
				return std::optional<unopened_file>(unopened_file(full_path, file_name));
			}
		}
	} else {
		native_string full_path = dir.relative_path + NATIVE('/') + native_string(file_name);
		struct stat stat_buf;
		int result = stat(full_path.c_str(), &stat_buf);
		if(result != -1 && S_ISREG(stat_buf.st_mode)) {
			return std::optional<unopened_file>(unopened_file(full_path, file_name));
		}
	}
	return std::optional<unopened_file>{};
}

void add_ignore_path(file_system& fs, native_string_view replaced_path) {
	fs.ignored_paths.emplace_back(replaced_path);
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

native_string get_full_name(directory const& dir) {
	return dir.relative_path;
}

native_string get_dir_name(directory const& dir) {
	size_t found;
	found = dir.relative_path.find_last_of(NATIVE("/"));
	return dir.relative_path.substr(found + 1);
}


void write_file(directory const& dir, native_string_view file_name, char const* file_data, uint32_t file_size) {
	if(dir.parent_system)
		std::abort();

	native_string full_path = dir.relative_path + NATIVE('/') + native_string(file_name);

	mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
	int file_handle = open(full_path.c_str(), O_RDWR | O_CREAT | O_TRUNC, mode);
	if(file_handle != -1) {
		ssize_t written = 0;
		int64_t size_remaining = file_size;
		do {
			written = write(file_handle, file_data, size_t(size_remaining));
			file_data += written;
			size_remaining -= written;
		} while(written >= 0 && size_remaining > 0);

		fsync(file_handle);
		close(file_handle);
	}
}

void append_file(directory const& dir, native_string_view file_name, char const* file_data, uint32_t file_size) {
	if(dir.parent_system)
		std::abort();

	native_string full_path = dir.relative_path + NATIVE('/') + native_string(file_name);

	mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
	int file_handle = open(full_path.c_str(), O_RDWR | O_CREAT | O_APPEND, mode);
	if(file_handle != -1) {
		ssize_t written = 0;
		int64_t size_remaining = file_size;
		do {
			written = write(file_handle, file_data, size_t(size_remaining));
			file_data += written;
			size_remaining -= written;
		} while(written >= 0 && size_remaining > 0);

		fsync(file_handle);
		close(file_handle);
	}
}

file_contents view_contents(file const& f) {
	return f.content;
}

void make_directories(native_string const& path) {
	char current_path[FILENAME_MAX];
	mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

	strcpy(current_path, path.c_str());

	for(char* p = strchr(current_path + 1, '/'); p; p = strchr(p + 1, '/')) {
		*p = '\0';
		int status = mkdir(current_path, mode);
		if(status == -1) {
			// error
		}
		*p = '/';
	}
}

directory get_or_create_settings_directory() {
	native_string path = native_string(getenv("HOME")) + "/.local/share/Alice/settings/";
	make_directories(path);

	return directory(nullptr, path);
}

directory get_or_create_save_game_directory() {
	native_string path = native_string(getenv("HOME")) + "/.local/share/Alice/saves/";
	make_directories(path);

	return directory(nullptr, path);
}

directory get_or_create_root_documents() {
	native_string path = native_string(getenv("HOME")) + "/.local/share/Alice/";
	make_directories(path);

	return directory(nullptr, path);
}

directory get_or_create_templates_directory() {
	native_string path = native_string(getenv("HOME")) + "/.local/share/Alice/templates/";
	make_directories(path);

	return directory(nullptr, path);
}
directory get_or_create_gamerules_directory() {
	native_string path = native_string(getenv("HOME")) + "/.local/share/Alice/gamerules/";
	make_directories(path);

	return directory(nullptr, path);
}

directory get_or_create_oos_directory() {
	native_string path = native_string(getenv("HOME")) + "/.local/share/Alice/oos/";
	make_directories(path);

	return directory(nullptr, path);
}

directory get_or_create_data_dumps_directory() {
	native_string path = native_string(getenv("HOME")) + "/.local/share/Alice/data_dumps/";
	make_directories(path);

	return directory(nullptr, path);
}

directory get_or_create_scenario_directory() {
	native_string path = native_string(getenv("HOME")) + "/.local/share/Alice/scenarios/";
	make_directories(path);

	return directory(nullptr, path);
}

native_string win1250_to_native(std::string_view data_in) {
	std::string result;
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
	for(auto ch : data_in) {
		if(ch >= 0)
			result += ch;
		else
			result += converter.to_bytes(text::win1250toUTF16(ch));
	}
	return result;
}

native_string utf8_to_native(std::string_view str) {
	return std::string(str);
}

std::string native_to_utf8(native_string_view str) {
	return std::string(str);
}

namespace linux_fs_detail {
	uint32_t assemble_codepoint(uint16_t high, uint16_t low) noexcept {
		uint32_t high_bits = (high & 0x03FF) << 10;
		uint32_t low_bits = low & 0x03FF;
		uint32_t temp = high_bits | low_bits;
		return temp + 0x10000;
	}
	bool is_low_surrogate(uint16_t char_code) noexcept {
		return char_code >= 0xDC00 && char_code <= 0xDFFF;
	}
	bool is_high_surrogate(uint16_t char_code) noexcept {
		return char_code >= 0xD800 && char_code <= 0xDBFF;
	}


	uint32_t codepoint_from_utf8(char const*& read_position, char const* end) {

		uint8_t byte1 = read_position < end ? uint8_t(*read_position) : uint8_t(0);
		uint8_t byte2 = read_position + 1 < end ? uint8_t(*(read_position + 1)) : uint8_t(0);
		uint8_t byte3 = read_position + 2 < end ? uint8_t(*(read_position + 2)) : uint8_t(0);
		uint8_t byte4 = read_position + 3 < end ? uint8_t(*(read_position + 3)) : uint8_t(0);

		if((byte1 & 0x80) == 0) {
			read_position += 1;
			return uint32_t(byte1);
		} else if((byte1 & 0xE0) == 0xC0) {
			read_position += 2;
			return uint32_t(byte2 & 0x3F) | (uint32_t(byte1 & 0x1F) << 6);
		} else  if((byte1 & 0xF0) == 0xE0) {
			read_position += 3;
			return uint32_t(byte3 & 0x3F) | (uint32_t(byte2 & 0x3F) << 6) | (uint32_t(byte1 & 0x0F) << 12);
		} else if((byte1 & 0xF8) == 0xF0) {
			read_position += 4;
			return uint32_t(byte4 & 0x3F) | (uint32_t(byte3 & 0x3F) << 6) | (uint32_t(byte2 & 0x3F) << 12) | (uint32_t(byte1 & 0x07) << 18);
		} else {
			read_position += 1;
			return 0;
		}
	}

	bool requires_surrogate_pair(uint32_t codepoint) {
		return codepoint >= 0x10000;
	}

	struct surrogate_pair {
		uint16_t high = 0; // aka leading
		uint16_t low = 0; // aka trailing
	};

	surrogate_pair make_surrogate_pair(uint32_t val) noexcept {
		uint32_t v = val - 0x10000;
		uint32_t h = ((v >> 10) & 0x03FF) | 0xD800;
		uint32_t l = (v & 0x03FF) | 0xDC00;
		return surrogate_pair{ uint16_t(h), uint16_t(l) };
	}
}

std::string utf16_to_utf8(std::u16string_view str) {
	size_t pos = 0;
	std::string result;

	while(pos < str.length()) {
		if(simple_fs::linux_fs_detail::is_low_surrogate(uint16_t(str[pos]))) {
			//ignore
		} else {
			uint32_t codepoint = 0;
			if(simple_fs::linux_fs_detail::is_high_surrogate(uint16_t(str[pos])) && pos + 1 < str.length()) {
				codepoint = simple_fs::linux_fs_detail::assemble_codepoint(uint16_t(str[pos]), uint16_t(str[pos + 1]));
			} else {
				codepoint = uint32_t(str[pos]);
			}
			if(codepoint <= 0x7F) {
				result += char(codepoint);
			} else if(codepoint <= 0x7FF) {
				result += char(0b11000000 | ((codepoint >> 6) & 0b00011111));
				result += char(0b10000000 | (codepoint & 0b00111111));
			} else if(codepoint <= 0xFFFF) {
				result += char(0b11100000 | ((codepoint >> 12) & 0b00001111));
				result += char(0b10000000 | ((codepoint >> 6) & 0b00111111));
				result += char(0b10000000 | (codepoint & 0b00111111));
			} else {
				result += char(0b11110000 | ((codepoint >> 18) & 0b00000111));
				result += char(0b10000000 | ((codepoint >> 12) & 0b00111111));
				result += char(0b10000000 | ((codepoint >> 6) & 0b00111111));
				result += char(0b10000000 | (codepoint & 0b00111111));
			}
		}
		++pos;
	}
	return result;
}
std::u16string utf8_to_utf16(std::string_view str) {
	std::u16string result;

	char const* read_position = str.data();
	auto end = read_position + str.length();
	while(read_position < end) {
		auto cp = simple_fs::linux_fs_detail::codepoint_from_utf8(read_position, end);
		if(simple_fs::linux_fs_detail::requires_surrogate_pair(cp)) {
			auto p = simple_fs::linux_fs_detail::make_surrogate_pair(cp);
			result += char16_t(p.high);
			result += char16_t(p.low);
		} else {
			result += char16_t(cp);
		}
	}

	return result;
}

std::string remove_double_backslashes(std::string_view data_in) {
	std::string res;
	res.reserve(data_in.size());
	for(uint32_t i = 0; i < data_in.size(); ++i) {
		if(data_in[i] == '\\') {
			res += '/';
			if(i + 1 < data_in.size() && data_in[i + 1] == '\\')
				++i;
		} else {
			res += data_in[i];
		}
	}
	return res;
}

native_string correct_slashes(native_string_view path) {
	std::string res;
	res.reserve(path.size());
	for(size_t i = 0; i < path.size(); i++) {
		res += path[i] == '\\' ? '/' : path[i];
	}
	return res;
}

} // namespace simple_fs
