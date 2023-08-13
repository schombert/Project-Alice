#define ALICE_NO_ENTRY_POINT 1
#include "main.cpp"

static inline void write_uncompressed_save_file(sys::state& state, native_string name) {
	sys::save_header header;
	// this is an upper bound, since compacting the data may require less space
	size_t total_size = sizeof_save_header(header) + sizeof_save_section(state);
	uint8_t* buffer = new uint8_t[total_size];
	uint8_t* buffer_position = buffer;
	buffer_position = write_save_header(buffer_position, header);
	buffer_position = write_save_section(buffer_position, state);
	auto total_size_used = buffer_position - buffer;
	simple_fs::write_file(simple_fs::get_or_create_save_game_directory(), name, reinterpret_cast<char*>(buffer),
			uint32_t(total_size_used));
	delete[] buffer;
}

static inline bool compare_game_states(sys::state& ws1, sys::state& ws2) {
	auto ymd = ws1.current_date.to_ymd(ws1.start_date);
	auto tmp1 = std::unique_ptr<uint8_t[]>(new uint8_t[sizeof_save_section(ws1)]);
	write_save_section(tmp1.get(), ws1);
	auto tmp2 = std::unique_ptr<uint8_t[]>(new uint8_t[sizeof_save_section(ws2)]);
	write_save_section(tmp2.get(), ws1);

	if(sizeof_save_section(ws1) == sizeof_save_section(ws2)) {
		if(std::memcmp(tmp1.get(), tmp2.get(), sizeof_save_section(ws1)) == 0) {
			return true;
		}
	}
	return false;
}

static inline std::unique_ptr<sys::state> load_prepare_game_state(native_string scenario, native_string save) {
	std::unique_ptr<sys::state> game_state = std::make_unique<sys::state>(); // too big for the stack
	add_root(game_state->common_fs, NATIVE_M(GAME_DIR)); // game files directory is overlaid on top of that
	add_root(game_state->common_fs,
			NATIVE(".")); // will add the working directory as first root -- for the moment this lets us find the shader files
	if(!sys::try_read_scenario_and_save_file(*game_state, scenario)) {
		// scenario making functions
		game_state->load_scenario_data();
		sys::write_scenario_file(*game_state, scenario);
	} else {
		std::printf("Reading savefile\n");
		sys::try_read_save_file(*game_state, save);
		game_state->fill_unsaved_data();
	}
	return game_state;
}

static inline void compare_savefiles(native_string scenario, native_string save1, native_string save2) {
	std::unique_ptr<sys::state> game_state_1 = load_prepare_game_state(scenario, save1);
	std::unique_ptr<sys::state> game_state_2 = load_prepare_game_state(scenario, save2);
	bool b = compare_game_states(*game_state_1, *game_state_2);
	std::printf("%s\n", b ? "EQUAL" : "DIFFERENT");
}

int main(int argc, char **argv) {
	if(argc <= 1) {
		std::printf("Please give an argument denoting how many ticks to run for.\n");
		std::printf("%s [number-of-ticks] [options] [save1] [save2]\n", argv[0]);
		std::printf("Options can be either of:\n");
		std::printf("\t'n' - Output a savefile for each tick.\n");
		std::printf("\t's' - Skip reading the savefile (if any).\n");
		std::printf("\t'f' - Do not overwrite original savefile.\n");
		std::printf("\t'q' - Do not do anything, simply exit without doing anything.\n");
		std::printf("\t'u' - Save uncompressed\n");
		std::printf("\t'c' - Compare that two saves are equal\n");
		std::printf("For example: '%s 10 sn'\n", argv[0]);
		return EXIT_FAILURE;
	}

	static bool output_for_each = false;
	static bool skip_savefile = false;
	static bool save_final = true;
	static bool save_compress = true;
	static bool save_compare = false;
	static int num_ticks = 1;

	native_string save1 = NATIVE("in_test_save.bin");
	native_string save2 = NATIVE("out_test_save.bin");
	native_string scenario = NATIVE("development_scenario_file.bin");

	if(argc >= 2) {
		num_ticks = atoi(argv[1]);
		if(argc >= 3) {
			const char *p = argv[2];
			for(; *p != '\0'; ++p) {
				if(*p == 'n') {
					output_for_each = true;
				} else if(*p == 's') {
					skip_savefile = true;
				} else if(*p == 'f') {
					save_final = false;
				} else if(*p == 'u') {
					save_compress = false;
				} else if(*p == 'c') {
					save_compare = true;
				} else if(*p == 'q') {
					return EXIT_SUCCESS;
				}
			}
			if(argc >= 4) {
				save1 = simple_fs::utf8_to_native(std::string(argv[3]));
				if(argc >= 5) {
					save2 = simple_fs::utf8_to_native(std::string(argv[4]));
					if(argc >= 6) {
						scenario = simple_fs::utf8_to_native(std::string(argv[5]));
					}
				}
			}
		}
	}

	if(save_compare) {
		compare_savefiles(scenario, save1, save2);
		return EXIT_SUCCESS;
	}

	if(skip_savefile) {
		save1 = NATIVE("$invalid$$$.bin");
	}

	std::unique_ptr<sys::state> game_state = load_prepare_game_state(scenario, save1);
	{
		auto ymd = game_state->current_date.to_ymd(game_state->start_date);
		std::printf("%u.%u.%u\n", (unsigned int)ymd.year, (unsigned int)ymd.month, (unsigned int)ymd.day);
	}
	for(auto i = 0; i < num_ticks; i++) {
		game_state->single_game_tick();
		auto ymd = game_state->current_date.to_ymd(game_state->start_date);
		std::printf("%u.%u.%u\n", (unsigned int)ymd.year, (unsigned int)ymd.month, (unsigned int)ymd.day);
		if(output_for_each) {
			native_string s = save2;
			s += to_native_string(ymd.year) + NATIVE(".");
			s += to_native_string(ymd.month) + NATIVE(".");
			s += to_native_string(ymd.day);
			if(save_compress) {
				s += NATIVE(".bin");
				sys::write_save_file(*game_state, s);
			} else {
				s += NATIVE(".raw");
				write_uncompressed_save_file(*game_state, s);
			}
			std::printf("Savefile written\n");
		}
	}
	if(save_final) {
		if(save_compress) {
			sys::write_save_file(*game_state, save2);
		} else {
			write_uncompressed_save_file(*game_state, save2);
		}
		std::printf("Savefile written\n");
	}
	return EXIT_SUCCESS;
}

