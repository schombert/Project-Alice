#include "simple_fs.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <dirent.h>
#include <codecvt>
#include <locale>

namespace simple_fs {
	file::~file() {
        if (mapping_handle) {
            if (munmap(mapping_handle, content.file_size) == -1) {
                // error
            }
        }
		if(file_descriptor != -1) {
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
            struct stat sb;
            if (fstat(file_descriptor, &sb) != -1) {
                content.file_size = sb.st_size;
                mapping_handle = mmap(0, content.file_size, PROT_READ, MAP_SHARED, file_descriptor, 0);
                if (mapping_handle == MAP_FAILED) {
                    // error
                }
                content.data = static_cast<char*>(mapping_handle);
            }
        }
	}
	file::file(int file_descriptor, native_string const& full_path) : file_descriptor(file_descriptor) {
        struct stat sb;
        if (fstat(file_descriptor, &sb) != -1) {
            content.file_size = sb.st_size;
            mapping_handle = mmap(0, content.file_size, PROT_READ, MAP_SHARED, file_descriptor, 0);
            if (mapping_handle == MAP_FAILED) {
                // error
            }
            content.data = static_cast<char*>(mapping_handle);
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
        char module_name[1024];
        ssize_t path_used = readlink("/proc/self/exe", module_name, sizeof(module_name) - 1);
        if (path_used != -1) {
            while(path_used >= 0 && module_name[path_used] != '/') {
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
		for(auto const& str : fs.ordered_roots) {
			result += ";" + str;
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
                DIR *d;
                struct dirent *dir_ent;
				const auto appended_path = dir.parent_system->ordered_roots[i] + dir.relative_path + NATIVE("/*") + extension;
                d = opendir(appended_path.c_str());
                if (d) {
                    while ((dir_ent = readdir(d)) != nullptr) {
                        // Check if it's a file. Not POSIX standard but included in Linux
                        if (dir_ent->d_type != DT_REG)
                            continue;

                        auto search_result = std::find_if(accumulated_results.begin(), accumulated_results.end(), [n = dir_ent->d_name](const auto& f) {
							return f.file_name.compare(n) == 0;
                        });
                        if(search_result == accumulated_results.end()) {
                            accumulated_results.emplace_back(dir.parent_system->ordered_roots[i] + dir.relative_path + NATIVE("/") + dir_ent->d_name, dir_ent->d_name);
                        }
                    }
                    closedir(d);
                }
			}
		} else {
			const auto appended_path = dir.relative_path + NATIVE("/*") + extension;
            DIR *d;
            struct dirent *dir_ent;
            d = opendir(appended_path.c_str());
            if (d) {
                while ((dir_ent = readdir(d)) != nullptr) {
                    // Check if it's a file. Not POSIX standard but included in Linux
                    if (dir_ent->d_type != DT_REG)
                        continue;

                    accumulated_results.emplace_back(dir.relative_path + NATIVE("/") + dir_ent->d_name, dir_ent->d_name);
                }
                closedir(d);
            }
		}
		return accumulated_results;
	}
	std::vector<directory> list_subdirectories(directory const& dir) {
		std::vector<directory> accumulated_results;
		if(dir.parent_system) {
			for(size_t i = dir.parent_system->ordered_roots.size(); i-- > 0; ) {
				const auto appended_path = dir.parent_system->ordered_roots[i] + dir.relative_path + NATIVE("/*");
                DIR *d;
                struct dirent *dir_ent;
                d = opendir(appended_path.c_str());
				if(d) {
                    while ((dir_ent = readdir(d)) != nullptr) {
                        // Check if it's a directory. Not POSIX standard but included in Linux
                        if (dir_ent->d_type != DT_DIR)
                            continue;

                        native_string const rel_name = dir.relative_path + NATIVE("/") + dir_ent->d_name;
                        if(dir_ent->d_name[0] != NATIVE('.')) {
                            auto search_result = std::find_if(accumulated_results.begin(), accumulated_results.end(), [&rel_name](const auto& s) {
                                return s.relative_path.compare(rel_name) == 0;
                            });
                            if(search_result == accumulated_results.end()) {
                                accumulated_results.emplace_back(dir.parent_system, rel_name);
                            }
                        }
                    }
                    closedir(d);
				}
			}
		} else {
			const auto appended_path = dir.relative_path + NATIVE("/*");
            DIR *d;
            struct dirent *dir_ent;
            d = opendir(appended_path.c_str());
            if(d) {
                while ((dir_ent = readdir(d)) != nullptr) {
                    // Check if it's a directory. Not POSIX standard but included in Linux
                    if (dir_ent->d_type != DT_DIR)
                        continue;

                    native_string const rel_name = dir.relative_path + NATIVE("/") + dir_ent->d_name;
                    if(dir_ent->d_name[0] != NATIVE('.')) {
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
		if(dir.parent_system) {
			for(size_t i = dir.parent_system->ordered_roots.size(); i-- > 0; ) {
				native_string full_path = dir.parent_system->ordered_roots[i] + dir.relative_path + NATIVE('/') + native_string(file_name);
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
		if(dir.parent_system) {
			for(size_t i = dir.parent_system->ordered_roots.size(); i-- > 0; ) {
				native_string full_path = dir.parent_system->ordered_roots[i] + dir.relative_path + NATIVE('/') + native_string(file_name);
                struct stat stat_buf;
                int result = stat("path/to/file", &stat_buf);
				if(result != -1 && S_ISREG(stat_buf.st_mode)) {
					return std::optional<unopened_file>(unopened_file(full_path, file_name));
				}
			}
		} else {
			native_string full_path = dir.relative_path + NATIVE('/') + native_string(file_name);
            struct stat stat_buf;
            int result = stat("path/to/file", &stat_buf);
            if(result != -1 && S_ISREG(stat_buf.st_mode)) {
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

        int file_handle = open(full_path.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if(file_handle != -1) {
            write(file_handle, file_data, file_size);
            ftruncate(file_handle, file_size);
            close(file_handle);
        }
	}

	file_contents view_contents(file const& f) {
		return f.content;
	}

	directory get_or_create_settings_directory() {
		native_string path = "/usr/local/games/Alice/settings";
		int status = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (status == -1) {
			// error
		}
		return directory(nullptr, path);
	}

	directory get_or_create_save_game_directory() {
		native_string path = "/usr/local/games/Alice/saves";
		int status = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (status == -1) {
			// error
		}
		return directory(nullptr, path);
	}

	directory get_or_create_scenario_directory() {
		native_string path = "/usr/local/games/Alice/scenarios";
		int status = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (status == -1) {
			// error
		}
		return directory(nullptr, path);
	}

    namespace win_internal {
		char16_t win1250toUTF16(char in) {
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
				/*A*/u' ',     u'\u02C7',u'\u02D8',u'\u00A2',u'\u00A3',u'\u0104',u'\u00A6',u'\u00A7',u'\u00A8',u'\u00A9',u'\u015E',u'\u00AB',u'\u00AC',u'-',     u'\u00AE',u'\u017B',
				/*B*/u'\u00B0',u'\u00B1',u'\u02DB',u'\u0142',u'\u00B4',u'\u00B5',u'\u00B6',u'\u00B7',u'\u00B8',u'\u0105',u'\u015F',u'\u00BB',u'\u013D',u'\u02DD',u'\u013E',u'\u017C',
				/*C*/u'\u0154',u'\u00C1',u'\u00C2',u'\u0102',u'\u00C4',u'\u0139',u'\u0106',u'\u00C7',u'\u010C',u'\u00C9',u'\u0118',u'\u00CB',u'\u011A',u'\u00CD',u'\u00CE',u'\u010E',
				/*D*/u'\u0110',u'\u0143',u'\u0147',u'\u00D3',u'\u00D4',u'\u0150',u'\u00D6',u'\u00D7',u'\u0158',u'\u016E',u'\u00DA',u'\u0170',u'\u00DC',u'\u00DD',u'\u0162',u'\u00DF',
				/*E*/u'\u0115',u'\u00E1',u'\u00E2',u'\u0103',u'\u00E4',u'\u013A',u'\u0107',u'\u00E7',u'\u010D',u'\u00E9',u'\u0119',u'\u00EB',u'\u011B',u'\u00ED',u'\u00EE',u'\u010F',
				/*F*/u'\u0111',u'\u0144',u'\u0148',u'\u00F3',u'\u00F4',u'\u0151',u'\u00F6',u'\u00F7',u'\u0159',u'\u016F',u'\u00FA',u'\u0171',u'\u00FC',u'\u00FD',u'\u0163',u'\u02D9'
			};

			return char16_t(converted[(uint8_t)in]);
		}
	}

	native_string win1250_to_native(std::string_view data_in) {
		std::string result;
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> converter;
		for(auto ch : data_in) {
			if (ch >= 0)
				result += ch;
			else
				result += converter.to_bytes(win_internal::win1250toUTF16(ch));
		}
		return result;
	}

	native_string utf8_to_native(std::string_view str) {
		return std::string(str);
	}

	std::string native_to_utf8(native_string_view str) {
		return std::string(str);
	}
}