#pragma once
#include "native_types_nix.hpp"
#include "unordered_dense.h"

// this file should contain the four class definitions of the types
// required for simple fs: file_system, directory, unopened_file, and file
// all in the namespace simple_fs, all classes

namespace simple_fs {
class file_system {
	std::vector<native_string> ordered_roots;
	std::vector<native_string> ignored_paths;

	void operator=(file_system const& other) = delete;
	void operator=(file_system&& other) = delete;

	public:
	friend std::optional<file> open_file(directory const& dir, native_string_view file_name);
	friend void reset(file_system& fs);
	friend void add_root(file_system& fs, native_string_view root_path);
	// will be added relative to the location that the executable file exists in
	friend void add_relative_root(file_system& fs, native_string_view root_path);
	friend directory get_root(file_system const& fs);
	friend native_string extract_state(file_system const& fs);
	friend void restore_state(file_system& fs, native_string_view data);
	friend std::vector<directory> list_subdirectories(directory const& dir);
	friend std::vector<unopened_file> list_files(directory const& dir, native_char const* extension);
	friend std::optional<unopened_file> peek_file(directory const& dir, native_string_view file_name);
	friend void add_ignore_path(file_system& fs, native_string_view replaced_path);
	friend std::vector<native_string> list_roots(file_system const& fs);
	friend bool is_ignored_path(file_system const& fs, native_string_view path);
};

class directory {
	file_system const* parent_system = nullptr;
	native_string relative_path;

	public:
	directory(file_system const* parent_system, native_string_view relative_path)
			: parent_system(parent_system), relative_path(relative_path) { }

	friend directory get_root(file_system const& fs);
	friend std::optional<file> open_file(directory const& dir, native_string_view file_name);
	friend std::vector<unopened_file> list_files(directory const& dir, native_char const* extension);
	friend std::vector<directory> list_subdirectories(directory const& dir);
	friend std::optional<file> open_file(directory const& dir, native_string_view file_name);
	friend std::optional<unopened_file> peek_file(directory const& dir, native_string_view file_name);
	friend void write_file(directory const& dir, native_string_view file_name, char const* file_data, uint32_t file_size);
	friend directory open_directory(directory const& dir, native_string_view directory_name);
	friend native_string get_full_name(directory const& dir);
};

class unopened_file {
	native_string absolute_path;
	native_string file_name;

	public:
	unopened_file(native_string_view absolute_path, native_string_view file_name)
			: absolute_path(absolute_path), file_name(file_name) { }

	friend std::optional<file> open_file(unopened_file const& f);
	friend std::vector<unopened_file> list_files(directory const& dir, native_char const* extension);
	friend native_string get_full_name(unopened_file const& f);
	friend native_string get_file_name(unopened_file const& f);
};

class file {
	int file_descriptor = -1;
	void* mapping_handle = nullptr;

	native_string absolute_path;
	file_contents content;

	file(native_string const& full_path);
	file(int file_descriptor, native_string const& full_path);

	public:
	file(file const& other) = delete;
	file(file&& other) noexcept;
	void operator=(file const& other) = delete;
	void operator=(file&& other) noexcept;
	~file();

	friend std::optional<file> open_file(directory const& dir, native_string_view file_name);
	friend std::optional<file> open_file(unopened_file const& f);
	friend class std::optional<file>;
	friend file_contents view_contents(file const& f);
	friend native_string get_full_name(file const& f);
};
} // namespace simple_fs
