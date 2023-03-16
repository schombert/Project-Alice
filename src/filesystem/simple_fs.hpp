#pragma once

#ifdef _WIN64
// WINDOWS typedefs go here
#include "native_types_win.hpp"

#else
// LINUX typedefs go here
#include "native_types_nix.hpp"
#endif

#include <stdint.h>
#include <vector>
#include <optional>

namespace simple_fs {
	class file;
	class directory;
	class unopened_file;
	class file_system;

	struct file_contents {
		char const* data = nullptr;
		uint32_t file_size = 0;
	};
}

#ifdef _WIN64
// WINDOWS typedefs go here
#include "simple_fs_types_win.hpp"

#else
// LINUX typedefs go here
#include "simple_fs_types_nix.hpp"
#endif


namespace simple_fs {
	// general file system functions
	// root paths should include the trailing separator
	void reset(file_system& fs);
	void add_root(file_system& fs, native_string_view root_path);
	// will be added relative to the location that the executable file exists in (but it is stored as an absolute path)
	void add_relative_root(file_system& fs, native_string_view root_path);
	directory get_root(file_system const& fs);

	// functions for saving and restoring its state
	native_string extract_state(file_system const& fs);
	void restore_state(file_system& fs, native_string_view data);

	// directory functions
	std::vector<unopened_file> list_files(directory const& dir, native_char const* extension);
	std::vector<directory> list_subdirectories(directory const& dir);
	std::optional<file> open_file(directory const& dir, native_string_view file_name);
	std::optional<unopened_file> peek_file(directory const& dir, native_string_view file_name);
	directory open_directory(directory const& dir, native_string_view directory_name);
	native_string get_full_name(directory const& f);

	// write_file will clear an existing file, if it exists, will create a new file if it does not
	void write_file(directory const& dir, native_string_view file_name, char const* file_data, uint32_t file_size);

	// unopened file functions
	std::optional<file> open_file(unopened_file const& f);
	native_string get_full_name(unopened_file const& f);

	// opened file functions
	file_contents view_contents(file const& f);
	native_string get_full_name(file const& f);

	// functions that operate outside of a filesystem object
	directory get_or_create_save_game_directory();
	directory get_or_create_scenario_directory();
	directory get_or_create_settings_directory();

	// necessary for reading paths out of data from inside older paradox files:
	// even on linux, this must do something, because win1250 isn't ascii or utf8
	native_string win1250_to_native(std::string_view data_in);

	// necessary for reading paths out of data from inside files we may create:
	// on linux, this just has to call the string constructor
	native_string utf8_to_native(std::string_view data_in);
	std::string native_to_utf8(native_string_view data_in);

	std::string remove_double_backslashes(std::string_view data_in); // unfortunately, paradox decided to escape their paths ...
	native_string correct_slashes(native_string path);
}

