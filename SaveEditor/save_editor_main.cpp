#define ALICE_NO_ENTRY_POINT 1
#include "main.cpp"

namespace sys {
void write_uncompressed_save_file(sys::state& state, native_string name) {
	save_header header;
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
}

int main(int argc, char **argv) {
	std::unique_ptr<sys::state> game_state = std::make_unique<sys::state>(); // too big for the stack

	assert(
			std::string("NONE") !=
			GAME_DIR); // If this fails, then you have not created a local_user_settings.hpp (read the documentation for contributors)
	add_root(game_state->common_fs, NATIVE_M(GAME_DIR)); // game files directory is overlaid on top of that
	add_root(game_state->common_fs,
			NATIVE(".")); // will add the working directory as first root -- for the moment this lets us find the shader files

	if(argc <= 1) {
		std::printf("Please give an argument denoting how many ticks to run for.\n");
		std::printf("%s [number-of-ticks] [options]\n", argv[0]);
		std::printf("Options can be either of:\n");
		std::printf("\t'n' - Output a savefile for each tick.\n");
		std::printf("\t's' - Skip reading the savefile (if any).\n");
		std::printf("\t'f' - Do not overwrite original savefile.\n");
		std::printf("\t'q' - Do not do anything, simply exit without doing anything.\n");
		std::printf("\t'u' - Save uncompressed\n");
		std::printf("For example: '%s 10 sn'\n", argv[0]);
		return EXIT_FAILURE;
	}

	static bool output_for_each = false;
	static bool skip_savefile = false;
	static bool save_final = true;
	static bool save_compress = true;
	static int num_ticks = 1;
	if(argc >= 2) {
		num_ticks = atoi(argv[1]);
		if(argc >= 3) {
			const char *p = argv[2];
			for(; *p != '\0'; ++p) {
				if(*p == 'n')
					output_for_each = true;
				else if(*p == 's')
					skip_savefile = true;
				else if(*p == 'f')
					save_final = false;
				else if(*p == 'u')
					save_compress = false;
				else if(*p == 'q')
					return EXIT_SUCCESS;
			}
		}
	}

	std::printf("Reading scenario\n");
	if(!sys::try_read_scenario_and_save_file(*game_state, NATIVE("development_test_file.bin"))) {
		// scenario making functions
		game_state->load_scenario_data();
		sys::write_scenario_file(*game_state, NATIVE("development_test_file.bin"));
	} else {
		if(!skip_savefile) {
			std::printf("Reading savefile\n");
			sys::try_read_save_file(*game_state, NATIVE("development_test_save.bin"));
		}
		game_state->fill_unsaved_data();
	}

	{
		auto ymd = game_state->current_date.to_ymd(game_state->start_date);
		std::printf("%u.%u.%u\n", (unsigned int)ymd.year, (unsigned int)ymd.month, (unsigned int)ymd.day);
	}
	for(auto i = 0; i < num_ticks; i++) {
		game_state->single_game_tick();
		auto ymd = game_state->current_date.to_ymd(game_state->start_date);
		std::printf("%u.%u.%u\n", (unsigned int)ymd.year, (unsigned int)ymd.month, (unsigned int)ymd.day);
		if(output_for_each) {
			native_string s = NATIVE("development_test_save.");
			s += to_native_string(ymd.year) + NATIVE(".");
			s += to_native_string(ymd.month) + NATIVE(".");
			s += to_native_string(ymd.day);
			if(save_compress) {
				s += NATIVE(".bin");
				sys::write_save_file(*game_state, s);
			} else {
				s += NATIVE(".raw");
				sys::write_uncompressed_save_file(*game_state, s);
			}
			std::printf("Savefile written\n");
		}
	}
	if(save_final) {
		if(save_compress) {
			sys::write_save_file(*game_state, NATIVE("development_test_save.bin"));
		} else {
			sys::write_uncompressed_save_file(*game_state, NATIVE("development_test_save.raw"));
		}
		std::printf("Savefile written\n");
	}
	return EXIT_SUCCESS;
}
