#include "catch2/catch.hpp"
#include "simple_fs.hpp"
#include <algorithm>

#ifdef _WIN64
#define NATIVE_SEP "\\"
#else
#define NATIVE_SEP "/"
#endif

#define NATIVE_M(X) NATIVE(X)

TEST_CASE("File system reading", "[file_system]") {
	SECTION("single root") {
		simple_fs::file_system fs;
		add_root(fs, NATIVE_M(PROJECT_ROOT));

		auto root_dir = get_root(fs);

		auto uo_file_a = peek_file(root_dir, NATIVE("CMakeLists.txt"));
		auto uo_file_b = peek_file(root_dir, NATIVE("test_main.cpp"));

		REQUIRE(bool(uo_file_a) == true);
		REQUIRE(bool(uo_file_b) == false);

		REQUIRE(get_full_name(*uo_file_a).find(NATIVE("CMake")) != std::string::npos);
	}
	SECTION("composite root") {
		simple_fs::file_system fs;
		add_root(fs, NATIVE_M(PROJECT_ROOT));
		add_root(fs, NATIVE_M(PROJECT_ROOT) NATIVE_SEP NATIVE("tests"));

		auto root_dir = get_root(fs);

		auto uo_file_a = peek_file(root_dir, NATIVE("CMakeLists.txt"));
		auto uo_file_b = peek_file(root_dir, NATIVE("test_main.cpp"));

		REQUIRE(bool(uo_file_a) == true);
		REQUIRE(bool(uo_file_b) == true);

		auto opened_lists = open_file(*uo_file_a);
		REQUIRE(get_full_name(*uo_file_b).find(NATIVE("main")) != std::string::npos);

		REQUIRE(bool(opened_lists) == true);
		REQUIRE(get_full_name(*opened_lists).find(NATIVE("CMake")) != std::string::npos);

		auto content = view_contents(*opened_lists);
		REQUIRE(content.file_size > 5);
		REQUIRE(content.data[0] == '#'); //94723
		REQUIRE(content.data[1] == '9');
		REQUIRE(content.data[2] == '4');
		REQUIRE(content.data[3] == '7');
		REQUIRE(content.data[4] == '2');
		REQUIRE(content.data[5] == '3');
	}
}

template<typename T>
bool something_is_named(std::vector<T> const& list, native_string name) {
	for(auto& val : list) {
		if(get_full_name(val).find(name) != std::string::npos)
			return true;
	}
	return false;
}

TEST_CASE("File system directories", "[file_system]") {
	SECTION("single root") {
		simple_fs::file_system fs;
		add_root(fs, NATIVE_M(PROJECT_ROOT));

		auto root_dir = get_root(fs);

		auto all_files = list_files(root_dir, NATIVE(""));

		REQUIRE(something_is_named(all_files, NATIVE("CMakeLists.txt")) == true);
		REQUIRE(something_is_named(all_files, NATIVE("Presets.json")) == true);
		REQUIRE(something_is_named(all_files, NATIVE("&*^*&()^")) == false);
		REQUIRE(something_is_named(all_files, NATIVE("tests")) == false);

		auto all_txt_files = list_files(root_dir, NATIVE(".txt"));

		REQUIRE(something_is_named(all_txt_files, NATIVE("CMakeLists.txt")) == true);
		REQUIRE(something_is_named(all_txt_files, NATIVE("Presets.json")) == false);

		auto all_dirs = list_subdirectories(root_dir);

		REQUIRE(something_is_named(all_dirs, NATIVE("tests")) == true);
		REQUIRE(something_is_named(all_dirs, NATIVE(".git")) == false);

		auto test_dir = open_directory(root_dir, NATIVE("tests"));

		auto all_test_files = list_files(test_dir, NATIVE(""));
		REQUIRE(something_is_named(all_test_files, NATIVE("Presets.json")) == false);
		REQUIRE(something_is_named(all_test_files, NATIVE("test")) == true);

		auto sub_sub_dir = list_subdirectories(test_dir);

		REQUIRE(sub_sub_dir.size() == size_t(0));
	}
	SECTION("overlapped root") {
		simple_fs::file_system fs;
		add_root(fs, NATIVE_M(PROJECT_ROOT));
		add_root(fs, NATIVE_M(PROJECT_ROOT) NATIVE_SEP NATIVE("dependencies"));

		auto root_dir = get_root(fs);

		auto all_files = list_files(root_dir, NATIVE(""));

		REQUIRE(something_is_named(all_files, NATIVE("CMakeLists.txt")) == true);
		REQUIRE(something_is_named(all_files, NATIVE("Presets.json")) == true);
		REQUIRE(something_is_named(all_files, NATIVE("&*^*&()^")) == false);
		REQUIRE(something_is_named(all_files, NATIVE("tests")) == false);

		auto all_txt_files = list_files(root_dir, NATIVE(".txt"));

		REQUIRE(something_is_named(all_txt_files, NATIVE("CMakeLists.txt")) == true);
		REQUIRE(something_is_named(all_txt_files, NATIVE("Presets.json")) == false);

		auto all_dirs = list_subdirectories(root_dir);

		REQUIRE(something_is_named(all_dirs, NATIVE("tests")) == true);
		REQUIRE(something_is_named(all_dirs, NATIVE("glew")) == true);
		REQUIRE(something_is_named(all_dirs, NATIVE(".git")) == false);

		auto glew_dir = open_directory(root_dir, NATIVE("glew"));

		auto glewmake = open_file(glew_dir, NATIVE("CMakeLists.txt"));
		REQUIRE(bool(glewmake) == true);

		auto content = view_contents(*glewmake);
		REQUIRE(content.file_size > 5);
		REQUIRE(content.data[0] == '#');
		REQUIRE(content.data[1] == 'G');
		REQUIRE(content.data[2] == 'L');
		REQUIRE(content.data[3] == 'E');
		REQUIRE(content.data[4] == 'W');
	}
}

TEST_CASE("writing special files", "[file_system]") {
	auto saves_dir = simple_fs::get_or_create_scenario_directory();
	write_file(saves_dir, NATIVE("fs_test_generated.hpp"), "// nothing to see here", uint32_t(strlen("// nothing to see here")));

	auto generated_file = open_file(saves_dir, NATIVE("fs_test_generated.hpp"));
	REQUIRE(bool(generated_file) == true);

	auto content = view_contents(*generated_file);
	REQUIRE(content.file_size == uint32_t(strlen("// nothing to see here")));
	REQUIRE(content.data[0] == '/');
	REQUIRE(content.data[1] == '/');
	REQUIRE(content.data[2] == ' ');
	REQUIRE(content.data[3] == 'n');
	REQUIRE(content.data[4] == 'o');
}