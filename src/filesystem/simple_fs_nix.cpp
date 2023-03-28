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
    if (mapping_handle) {
        if (munmap(mapping_handle, content.file_size) == -1) {
            // error
        }
    }
    if (file_descriptor != -1) {
        close(file_descriptor);
    }
}

file::file(file&& other) noexcept {
    file_descriptor = other.file_descriptor;
    mapping_handle = other.mapping_handle;
    content = other.content;
    absolute_path = std::move(other.absolute_path);

    other.file_descriptor = -1;
    other.mapping_handle = nullptr;
}
void file::operator=(file&& other) noexcept {
    file_descriptor = other.file_descriptor;
    mapping_handle = other.mapping_handle;
    content = other.content;
    absolute_path = std::move(other.absolute_path);

    other.file_descriptor = -1;
    other.mapping_handle = nullptr;
}

file::file(native_string const& full_path) {
    file_descriptor = open(full_path.c_str(), O_RDONLY | O_NONBLOCK);
    if (file_descriptor != -1) {
        absolute_path = full_path;
        struct stat sb;
        if (fstat(file_descriptor, &sb) != -1) {
            content.file_size = sb.st_size;
            mapping_handle = mmap(0, content.file_size, PROT_READ, MAP_PRIVATE, file_descriptor, 0);
            if (mapping_handle == MAP_FAILED) {
                // error
            }
            content.data = static_cast<char*>(mapping_handle);
        }
    }
}
file::file(int file_descriptor, native_string const& full_path) : file_descriptor(file_descriptor) {
    absolute_path = full_path;
    struct stat sb;
    if (fstat(file_descriptor, &sb) != -1) {
        content.file_size = sb.st_size;
        mapping_handle = mmap(0, content.file_size, PROT_READ, MAP_PRIVATE, file_descriptor, 0);
        if (mapping_handle == MAP_FAILED) {
            // error
        }
        content.data = static_cast<char*>(mapping_handle);
    }
}

std::optional<file> open_file(unopened_file const& f) {
    std::optional<file> result(file{f.absolute_path});
    if (!result->content.data) {
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
    char module_name[1024];
    ssize_t path_used = readlink("/proc/self/exe", module_name, sizeof(module_name) - 1);
    if (path_used != -1) {
        while (path_used >= 0 && module_name[path_used] != '/') {
            module_name[path_used] = '\0';
            --path_used;
        }
    }

    fs.ordered_roots.push_back(native_string(module_name) + native_string(root_path));
}

directory get_root(file_system const& fs) {
    return directory(&fs, NATIVE(""));
}

native_string extract_state(file_system const& fs) {
    native_string result;
    for (auto const& str : fs.ordered_roots) {
        result += ";" + str;
    }
    return result;
}
void restore_state(file_system& fs, native_string_view data) {
    fs.ordered_roots.clear();

    native_char const* position = data.data();
    native_char const* end = data.data() + data.length();
    while (position < end) {
        auto next_semicolon = std::find(position, end, NATIVE(';'));

        fs.ordered_roots.emplace_back(position, next_semicolon);
        position = next_semicolon + 1;
    }
}

std::vector<unopened_file> list_files(directory const& dir, native_char const* extension) {
    std::vector<unopened_file> accumulated_results;
    if (dir.parent_system) {
        for (size_t i = dir.parent_system->ordered_roots.size(); i-- > 0;) {
            DIR* d;
            struct dirent* dir_ent;
            const auto appended_path = dir.parent_system->ordered_roots[i] + dir.relative_path;
            if(simple_fs::is_ignored_path(*dir.parent_system, appended_path)) {
                continue;
            }
            d = opendir(appended_path.c_str());
            if (d) {
                while ((dir_ent = readdir(d)) != nullptr) {
                    // Check if it's a file. Not POSIX standard but included in Linux
                    if (dir_ent->d_type != DT_REG)
                        continue;
					
					// Check if the file is of the right extension
					if (strlen(extension)) {
					    char* dot = strrchr(dir_ent->d_name, '.');
						if(!dot || dot == dir_ent->d_name)
							continue;
						if (strcmp(dot, extension))
							continue;
					}

                    auto search_result = std::find_if(accumulated_results.begin(), accumulated_results.end(), [n = dir_ent->d_name](const auto& f) {
                        return f.file_name.compare(n) == 0;
                    });
                    if (search_result == accumulated_results.end()) {
                        accumulated_results.emplace_back(dir.parent_system->ordered_roots[i] + dir.relative_path + NATIVE("/") + dir_ent->d_name, dir_ent->d_name);
                    }
                }
                closedir(d);
            }
        }
    } else {
        const auto appended_path = dir.relative_path;
        DIR* d;
        struct dirent* dir_ent;
        d = opendir(appended_path.c_str());
        if (d) {
            while ((dir_ent = readdir(d)) != nullptr) {
                // Check if it's a file. Not POSIX standard but included in Linux
                if (dir_ent->d_type != DT_REG)
                    continue;
					
				// Check if the file is of the right extension
				if (strlen(extension)) {
					char* dot = strrchr(dir_ent->d_name, '.');
					if(!dot || dot == dir_ent->d_name)
						continue;
					if (strcmp(dot, extension))
						continue;
				}

                accumulated_results.emplace_back(dir.relative_path + NATIVE("/") + dir_ent->d_name, dir_ent->d_name);
            }
            closedir(d);
        }
    }
    return accumulated_results;
}
std::vector<directory> list_subdirectories(directory const& dir) {
    std::vector<directory> accumulated_results;
    if (dir.parent_system) {
        for (size_t i = dir.parent_system->ordered_roots.size(); i-- > 0;) {
            DIR* d;
            struct dirent* dir_ent;
            const auto appended_path = dir.parent_system->ordered_roots[i] + dir.relative_path;
            if(simple_fs::is_ignored_path(*dir.parent_system, appended_path)) {
                continue;
            }
            d = opendir(appended_path.c_str());
            if (d) {
                while ((dir_ent = readdir(d)) != nullptr) {
                    // Check if it's a directory. Not POSIX standard but included in Linux
                    if (dir_ent->d_type != DT_DIR)
                        continue;

                    native_string const rel_name = dir.relative_path + NATIVE("/") + dir_ent->d_name;
                    if (dir_ent->d_name[0] != NATIVE('.')) {
                        auto search_result = std::find_if(accumulated_results.begin(), accumulated_results.end(), [&rel_name](const auto& s) {
                            return s.relative_path.compare(rel_name) == 0;
                        });
                        if (search_result == accumulated_results.end()) {
                            accumulated_results.emplace_back(dir.parent_system, rel_name);
                        }
                    }
                }
                closedir(d);
            }
        }
    } else {
        const auto appended_path = dir.relative_path;
        DIR* d;
        struct dirent* dir_ent;
        d = opendir(appended_path.c_str());
        if (d) {
            while ((dir_ent = readdir(d)) != nullptr) {
                // Check if it's a directory. Not POSIX standard but included in Linux
                if (dir_ent->d_type != DT_DIR)
                    continue;

                native_string const rel_name = dir.relative_path + NATIVE("/") + dir_ent->d_name;
                if (dir_ent->d_name[0] != NATIVE('.')) {
                    accumulated_results.emplace_back(nullptr, rel_name);
                }
            }
            closedir(d);
        }
    }
    return accumulated_results;
}

directory open_directory(directory const& dir, native_string_view directory_name) {
    return directory(dir.parent_system, dir.relative_path + NATIVE('/') + native_string(directory_name));
}

std::optional<file> open_file(directory const& dir, native_string_view file_name) {
    if (dir.parent_system) {
        for (size_t i = dir.parent_system->ordered_roots.size(); i-- > 0;) {
            native_string dir_path = dir.parent_system->ordered_roots[i] + dir.relative_path;
            if(simple_fs::is_ignored_path(*dir.parent_system, dir_path)) {
                continue;
            }
            native_string full_path = dir_path + NATIVE('/') + native_string(file_name);
            int file_descriptor = open(full_path.c_str(), O_RDONLY | O_NONBLOCK);
            if (file_descriptor != -1) {
                return std::optional<file>(file(file_descriptor, full_path));
            }
        }
    } else {
        native_string full_path = dir.relative_path + NATIVE('/') + native_string(file_name);
        int file_descriptor = open(full_path.c_str(), O_RDONLY | O_NONBLOCK);
        if (file_descriptor != -1) {
            return std::optional<file>(file(file_descriptor, full_path));
        }
    }
    return std::optional<file>{};
}

std::optional<unopened_file> peek_file(directory const& dir, native_string_view file_name) {
    if (dir.parent_system) {
        for (size_t i = dir.parent_system->ordered_roots.size(); i-- > 0;) {
            native_string full_path = dir.parent_system->ordered_roots[i] + dir.relative_path + NATIVE('/') + native_string(file_name);
            if(simple_fs::is_ignored_path(*dir.parent_system, full_path)) {
                continue;
            }
            struct stat stat_buf;
            int result = stat(full_path.c_str(), &stat_buf);
            if (result != -1 && S_ISREG(stat_buf.st_mode)) {
                return std::optional<unopened_file>(unopened_file(full_path, file_name));
            }
        }
    } else {
        native_string full_path = dir.relative_path + NATIVE('/') + native_string(file_name);
        struct stat stat_buf;
        int result = stat(full_path.c_str(), &stat_buf);
        if (result != -1 && S_ISREG(stat_buf.st_mode)) {
            return std::optional<unopened_file>(unopened_file(full_path, file_name));
        }
    }
    return std::optional<unopened_file>{};
}

void add_replace_path_rule(file_system& fs, native_string_view replaced_path, native_string_view new_path) {
    fs.replace_paths.insert(std::pair{ replaced_path, new_path });
}

std::vector<native_string> list_roots(file_system const& fs) {
    return fs.ordered_roots;
}

bool is_ignored_path(file_system const& fs, native_string_view path) {
    for(const auto& replace_path : fs.replace_paths)
        if(replace_path.first == path)
            return true;
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

void write_file(directory const& dir, native_string_view file_name, char const* file_data, uint32_t file_size) {
    if (dir.parent_system)
        std::abort();

    native_string full_path = dir.relative_path + NATIVE('/') + native_string(file_name);

    mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    int file_handle = open(full_path.c_str(), O_RDWR | O_CREAT | O_TRUNC, mode);
    if (file_handle != -1) {
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

    for (char* p = strchr(current_path + 1, '/'); p; p = strchr(p + 1, '/')) {
        *p = '\0';
        int status = mkdir(current_path, mode);
        if (status == -1) {
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

directory get_or_create_scenario_directory() {
    native_string path = native_string(getenv("HOME")) + "/.local/share/Alice/scenarios/";
    make_directories(path);

    return directory(nullptr, path);
}

native_string win1250_to_native(std::string_view data_in) {
    std::string result;
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    for (auto ch : data_in) {
        if (ch >= 0)
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

native_string correct_slashes(native_string path) {
    std::string res;
    res.reserve(path.size());
    for(size_t i = 0; i < path.size(); i++) {
        res += path[i] == '\\' ? '/' : path[i];
    }
    return res;
}

}  // namespace simple_fs
