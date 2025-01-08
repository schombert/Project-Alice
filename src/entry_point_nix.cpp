#include "serialization.hpp"
#include "system_state.hpp"

static sys::state game_state;
struct scenario_file {
	native_string file_name;
	sys::mod_identifier ident;
};
static std::vector<parsers::mod_file> mod_list;
static native_string selected_scenario_file;
static uint32_t max_scenario_count = 0;
static std::vector<scenario_file> scenario_files;
//static std::vector arguments;

// Keep mod_list empty for vanilla
native_string produce_mod_path() {
	simple_fs::file_system(dummy);
	simple_fs::add_root(dummy, NATIVE("."));

	for(int32_t i = 0; i < int32_t(mod_list.size()); ++i) {
		if(mod_list[i].mod_selected == false)
			break;

		mod_list[i].add_to_file_system(dummy);
	}
	return simple_fs::extract_state(dummy);
}

void check_mods_folder() {
	simple_fs::file_system fs_root;
	simple_fs::add_root(fs_root, ".");
	auto root = get_root(fs_root);
	auto mod_dir = simple_fs::open_directory(root, NATIVE("mod"));
	auto mod_files = simple_fs::list_files(mod_dir, NATIVE(".mod"));
	parsers::error_handler err("");
	for (auto& f : mod_files) {
		auto of = simple_fs::open_file(f);
		if(of) {
			auto content = view_contents(*of);
			parsers::token_generator gen(content.data, content.data + content.file_size );
			mod_list.push_back(parsers::parse_mod_file(gen, err, parsers::mod_file_context{}));
		}
	}
}

void check_scenario_folder() {
	simple_fs::file_system fs_root;
	simple_fs::add_root(fs_root, ".");
	auto root = get_root(fs_root);
	auto sdir = simple_fs::get_or_create_scenario_directory();
	auto s_files = simple_fs::list_files(sdir, NATIVE(".bin"));
	for(auto& f : s_files) {
		auto of = simple_fs::open_file(f);
		if(of) {
			auto content = view_contents(*of);
			auto desc = sys::extract_mod_information(reinterpret_cast<uint8_t const*>(content.data), content.file_size);
			if(desc.count != 0) {
				max_scenario_count = std::max(desc.count, max_scenario_count);
				scenario_files.push_back(scenario_file{ simple_fs::get_file_name(f) , desc });
			}
		}
	}

	std::sort(scenario_files.begin(), scenario_files.end(), [](scenario_file const& a, scenario_file const& b) {
		return a.ident.count > b.ident.count;
	});
}

native_string to_hex(uint64_t v) {
		native_string ret;
		constexpr native_char digits[] = NATIVE("0123456789ABCDEF");
		do {
		 ret += digits[v & 0x0F];
		 v = v >> 4;
		} while(v != 0);
		 	return ret;
	};

// Original is void make_mod_file(). This function can also be used to create
// vanilla scenario, though when there are two bookmarks dates, it will also create
// additional scenario file alongside with the save file for the bookmark.
// Unsure how it works in windows build.
void build_scenario_file() {
	auto path = produce_mod_path();
	simple_fs::file_system fs_root;
	simple_fs::restore_state(fs_root, path);
	parsers::error_handler err("");
	auto root = get_root(fs_root);
	auto common = open_directory(root, NATIVE("common"));
	parsers::bookmark_context bookmark_context;
	if(auto f = open_file(common, NATIVE("bookmarks.txt")); f) {
		auto bookmark_content = simple_fs::view_contents(*f);
		err.file_name = "bookmarks.txt";
		parsers::token_generator gen(bookmark_content.data, bookmark_content.data + bookmark_content.file_size);
		parsers::parse_bookmark_file(gen, err, bookmark_context);
		assert(!bookmark_context.bookmark_dates.empty());
	} else {
		err.accumulated_errors += "File common/bookmarks.txt could not be opened\n";
	}

	sys::checksum_key scenario_key;

	for(uint32_t date_index = 0; date_index < uint32_t(bookmark_context.bookmark_dates.size()); date_index++) {
		err.accumulated_errors.clear();
		err.accumulated_warnings.clear();
		//
		auto game_state = std::make_unique<sys::state>();
		simple_fs::restore_state(game_state->common_fs, path);
		game_state->load_scenario_data(err, bookmark_context.bookmark_dates[date_index].date_);
		if(err.fatal)
			break;
		if(date_index == 0) {
			auto sdir = simple_fs::get_or_create_scenario_directory();
			int32_t append = 0;
			auto time_stamp = uint64_t(std::time(0));
			auto base_name = to_hex(time_stamp);
			while(simple_fs::peek_file(sdir, base_name + NATIVE("-") + std::to_string(append) + NATIVE(".bin"))) {
				++append;
			}
			++max_scenario_count;
			selected_scenario_file = base_name + NATIVE("-") + std::to_string(append) + NATIVE(".bin");
			sys::write_scenario_file(*game_state, selected_scenario_file, max_scenario_count);
			if(auto of = simple_fs::open_file(sdir, selected_scenario_file); of) {
				auto content = view_contents(*of);
				auto desc = sys::extract_mod_information(reinterpret_cast<uint8_t const*>(content.data), content.file_size);
				if(desc.count != 0) {
					scenario_files.push_back(scenario_file{ selected_scenario_file , desc });
				}
			}
			std::sort(scenario_files.begin(), scenario_files.end(), [](scenario_file const& a, scenario_file const& b) {
				return a.ident.count > b.ident.count;
			});
			scenario_key = game_state->scenario_checksum;
		} else {
#ifndef NDEBUG
			sys::write_scenario_file(*game_state, std::to_string(date_index) + NATIVE(".bin"), 0);
#endif
			game_state->scenario_checksum = scenario_key;
			sys::write_save_file(*game_state, sys::save_type::bookmark, bookmark_context.bookmark_dates[date_index].name_);
		}
	}

	if(!err.accumulated_errors.empty() || !err.accumulated_warnings.empty()) {
		auto assembled_msg = std::string("You can still play the mod, but it might be unstable\r\nThe following problems were encountered while creating the scenario:\r\n\r\nErrors:\r\n") + err.accumulated_errors + "\r\n\r\nWarnings:\r\n" + err.accumulated_warnings;
		auto pdir = simple_fs::get_or_create_scenario_directory();
		auto fname = NATIVE("scenario_errors.log");
		simple_fs::write_file(pdir, NATIVE("scenario_errors.log"), assembled_msg.data(), uint32_t(assembled_msg.length()));
		auto fnamepath = simple_fs::get_full_name(pdir) + NATIVE("/") + NATIVE(fname);
		//system(xdg-open fnamepath);
		window::emit_error_message("Some errors were found during generation of scenario file. Please open " + NATIVE(fnamepath) + " to check the errors.\n", false);
	}
	window::emit_error_message("Scenario file had been generated and saved to " + native_string(getenv("HOME")) + "/.local/share/Alice/\n", false);
}

//The function below would better be implemented in a launcher
void find_scenario_file() {
	selected_scenario_file = NATIVE("");
	auto mod_path = produce_mod_path();
	for(auto& f : scenario_files) {
		if(f.ident.mod_path == mod_path){
			selected_scenario_file = f.file_name;
			break;
		}
	}
}

bool transitively_depends_on_internal(parsers::mod_file const& moda, parsers::mod_file const& modb, std::vector<bool>& seen_indices) {
	for(auto& dep : moda.dependent_mods) {
		if(dep == modb.name_)
			return true;

		for(int32_t i = 0; i < int32_t(mod_list.size()); ++i) {
			if(seen_indices[i] == false && mod_list[i].name_ == dep) {
				seen_indices[i] = true;
				if(transitively_depends_on_internal(mod_list[i], modb, seen_indices))
					return true;
			}
		}
	}
	return false;
}

bool transitively_depends_on(parsers::mod_file const& moda, parsers::mod_file const& modb) {
	std::vector<bool> seen_indices;
	seen_indices.resize(mod_list.size());

	return transitively_depends_on_internal(moda, modb, seen_indices);
}

void enforce_list_order() {
	std::stable_sort(mod_list.begin(), mod_list.end(), [&](parsers::mod_file const& a, parsers::mod_file const& b) {
		if(a.mod_selected && b.mod_selected) {
			return transitively_depends_on(b, a);
		} else if(a.mod_selected && !b.mod_selected) {
			return true;
		} else if(!a.mod_selected && b.mod_selected) {
			return false;
		} else {
			return a.name_ < b.name_;
		}
	});
}

int main (int argc, char *argv[]) {
	add_root(game_state.common_fs, NATIVE("."));
	check_mods_folder();
	check_scenario_folder();
	if ( argc >= 2) {
		//TODO: Get all argv as vector and process them before running any codes
		//for (int args = 1; args < argc; ++args){
		//	arguments.push_back(args);
		//}
		for ( int i = 1; i < argc; ++i ) {
			if (native_string(argv[i]) == NATIVE("--mod")) {
				//Use closest to standard v2game.exe args structure, i.e. --mod mod/modname.mod
				//We don't need powerful do-it-all entry point, scenario generation is enough
				//Always assume that user will run this in the standard way of usage
				if ( i + 1 < argc ){
					//Beginning of seemingly unnecessary code
					std::string fullpath = argv[ i + 1];
					int idx = fullpath.rfind("/");
					std::string dir = fullpath.substr(0, idx);
					if(dir != NATIVE("mod")) {
						window::emit_error_message("Usage: --mod mod/modname.mod\n", true);
					}
					std::string filename = fullpath.substr(idx + 1);
					//Ending of seemingly unnecessary code
					simple_fs::file_system fs_root;
					simple_fs::add_root(fs_root, ".");
					auto root = get_root(fs_root);
					auto mod_dir = simple_fs::open_directory(root, NATIVE(dir));
					//Implement check of passed argv in mod_list vector, if found, set true to mod_selected
					//Currently, the code is crude
					auto of = simple_fs::open_file(mod_dir, NATIVE(filename));
					parsers::error_handler err{""};
					std::vector<parsers::mod_file> selected_mod;
					if(of){
						auto content = view_contents(*of);
						parsers::token_generator gen(content.data, content.data + content.file_size);
						selected_mod.push_back(parsers::parse_mod_file(gen, err, parsers::mod_file_context{}));
					}
					//This is to iterate through the content of mod_list
					for(int32_t modindex = 0; modindex < int32_t(mod_list.size()); ++modindex){
						if(mod_list[modindex].name_ == selected_mod[0].name_) {
							mod_list[modindex].mod_selected = true;
						}
					}
					i++;
				}
			} else if ( native_string(argv[i]) == NATIVE("--test")) {
				if (sys::try_read_scenario_and_save_file(game_state, "development_test_file.bin")){
					selected_scenario_file = "development_test_file.bin";
				} else {
					window::emit_error_message("Development test file not found. Proceeding to generate file, this process may take a few minutes to complete.\n", false);
					parsers::error_handler err{ "" };
					game_state.load_scenario_data(err, sys::year_month_day{ 1836, 1, 1});
					if(!err.accumulated_errors.empty() || !err.accumulated_warnings.empty()) {
						auto assembled_msg = std::string("You can still play the mod, but it might be unstable\r\nThe following problems were encountered while creating the scenario:\r\n\r\nErrors:\r\n") + err.accumulated_errors + "\r\n\r\nWarnings:\r\n" + err.accumulated_warnings;
						window::emit_error_message(assembled_msg, false);
					}
					sys::write_scenario_file(game_state, NATIVE("development_test_file.bin"), 0);
					selected_scenario_file = "development_test_file.bin";
				}
				break;
			} else if(native_string(argv[i]) == NATIVE("-host")) {
				game_state.network_mode = sys::network_mode_type::host;
			} else if(native_string(argv[i]) == NATIVE("-join")) {
				game_state.network_mode = sys::network_mode_type::client;
				game_state.network_state.ip_address = "127.0.0.1";
				if(i + 1 < argc) {
					game_state.network_state.ip_address = simple_fs::native_to_utf8(native_string(argv[i + 1]));
					i++;
				}
			} else if(native_string(argv[i]) == NATIVE("-name")) {
				if(i + 1 < argc) {
					std::string nickname = simple_fs::native_to_utf8(native_string(argv[i + 1]));
					memcpy(&game_state.network_state.nickname.data, nickname.c_str(), std::min<size_t>(nickname.length(), 8));
					i++;
				}
			} else if(native_string(argv[i]) == NATIVE("-password")) {
				if(i + 1 < argc) {
					auto str = simple_fs::native_to_utf8(native_string(argv[i + 1]));
					std::memset(game_state.network_state.lobby_password, '\0', sizeof(game_state.network_state.lobby_password));
					std::memcpy(game_state.network_state.lobby_password, str.c_str(), std::min(sizeof(game_state.network_state.lobby_password), str.length()));
					i++;
				}
			}
			else if (native_string(argv[i]) == NATIVE("-player_password")) {
				if (i + 1 < argc) {
					std::string password = simple_fs::native_to_utf8(native_string(argv[i + 1]));
					memcpy(&game_state.network_state.player_password.data, password.c_str(), std::min<size_t>(password.length(), 8));
					i++;
				}
			}
			else if(native_string(argv[i]) == NATIVE("-v6")) {
				game_state.network_state.as_v6 = true;
			} else if(native_string(argv[i]) == NATIVE("-v4")) {
				game_state.network_state.as_v6 = false;
			}
		}
		enforce_list_order();
		for (int32_t modindex = 0; modindex < int32_t(mod_list.size()); ++modindex){
			if(mod_list[modindex].mod_selected){
				window::emit_error_message("Selected mod: " + NATIVE(mod_list[modindex].name_) + "\n", false);
			}
		}
		//Trying to find the corresponding scenario file before attempting to build
		//Sacred scenario file = "development_test_file.bin"
		//Some parts below felt redundant, may not fix
		if (selected_scenario_file == NATIVE("development_test_file.bin")){
		} else {
			//Checking the scenario folder
			find_scenario_file();
			//If any file fits the criteria, then don't build
			if (!selected_scenario_file.empty()) {
				window::emit_error_message("Selected scenario file: " + NATIVE(selected_scenario_file) + "\n", false);
			//If no file fits the critertia, then build
			} else {
				window::emit_error_message("Scenario file with selected mods cannot be found. Proceeding to build.\nThis process may take a few minutes to finish.\n", false);
				build_scenario_file();
			}
		}
	} else {
		//Falling back to vanilla
		find_scenario_file();
		if (!selected_scenario_file.empty()) {
			window::emit_error_message("Selected scenario file: " + NATIVE(selected_scenario_file) + "\n", false);
		} else {
			window::emit_error_message("Building the vanilla scenario file. This process may take a few minutes to complete.\n", false);
			build_scenario_file();
		}
	}

	if (sys::try_read_scenario_and_save_file(game_state, selected_scenario_file)) {
			auto msg = "Running scenario file " + simple_fs::native_to_utf8(selected_scenario_file) + "\n";
			window::emit_error_message(msg, false);
			game_state.loaded_scenario_file = NATIVE(selected_scenario_file);
			game_state.fill_unsaved_data();
	} else {
		window::emit_error_message("Scenario file could not be read.", true);
	}

	network::init(game_state);
	game_state.load_user_settings();
	ui::populate_definitions_map(game_state);
	std::thread update_thread([&]() { game_state.game_loop(); });
	window::emit_error_message("Starting the game.\n", false);
	window::create_window(game_state, window::creation_parameters{1024, 780, window::window_state::maximized, game_state.user_settings.prefer_fullscreen});
	game_state.quit_signaled.store(true, std::memory_order_release);
	update_thread.join();
	network::finish(game_state, true);

	return EXIT_SUCCESS;
}
