#pragma once
#include "native_types_win.hpp"

#ifndef UNICODE
#define UNICODE
#endif
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "Windows.h"

// this file should contain the four class definitions of the types 
// required for simple fs: file_system, directory, unopened_file, and file
// all in the namespace simple_fs, all classes


namespace simple_fs {
	class file_system {
		std::vector<native_string> ordered_roots;

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
	};


	class directory {
		native_string relative_path;
		file_system const* parent_system = nullptr;
	public:
		directory(file_system const* parent_system, native_string_view relative_path) : parent_system(parent_system), relative_path(relative_path) {}

		friend directory get_root(file_system const& fs);
		friend std::optional<file> open_file(directory const& dir, native_string_view file_name);
		friend std::vector<unopened_file> list_files(directory const& dir, native_char const* extension);
		friend std::vector<directory> list_subdirectories(directory const& dir);
		friend std::optional<file> open_file(directory const& dir, native_string_view file_name);
		friend std::optional<unopened_file> peek_file(directory const& dir, native_string_view file_name);
		friend void write_file(directory const& dir, native_string_view file_name, char const* file_data, uint32_t file_size);
	};


	class unopened_file {
		native_string file_name;
		native_string absolute_path;
	public:
		unopened_file(native_string_view absolute_path, native_string_view file_name) : absolute_path(absolute_path), file_name(file_name) {}

		friend std::optional<file> open_file(unopened_file const& f);
		friend std::vector<unopened_file> list_files(directory const& dir, native_char const* extension);
	};


	class file {
		HANDLE file_handle = INVALID_HANDLE_VALUE;
		HANDLE mapping_handle = nullptr;

		native_string absolute_path;
		file_contents content;

		file(native_string const& full_path);
		file(HANDLE file_handle, native_string const& full_path);
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
	};
}
