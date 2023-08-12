#define ALICE_NO_ENTRY_POINT 1
#include "main.cpp"

int main(int argc, char **argv) {
	std::unique_ptr<sys::state> game_state = std::make_unique<sys::state>(); // too big for the stack

	assert(
			std::string("NONE") !=
			GAME_DIR); // If this fails, then you have not created a local_user_settings.hpp (read the documentation for contributors)
	add_root(game_state->common_fs, NATIVE_M(GAME_DIR)); // game files directory is overlaid on top of that
	add_root(game_state->common_fs,
			NATIVE(".")); // will add the working directory as first root -- for the moment this lets us find the shader files

	if(argc <= 1)
        std::abort();

	std::printf("Reading savefile\n");
	if(!sys::try_read_scenario_and_save_file(*game_state, NATIVE("development_test_file.bin"))) {
		// scenario making functions
		game_state->load_scenario_data();
		game_state->local_player_nation = dcon::nation_id{8};
		game_state->world.nation_set_is_player_controlled(game_state->local_player_nation, true);
		sys::write_scenario_file(*game_state, NATIVE("development_test_file.bin"));
	} else {
		sys::try_read_save_file(*game_state, NATIVE("development_test_save.bin"));
		game_state->fill_unsaved_data();
	}

	{
		auto ymd = game_state->current_date.to_ymd(game_state->start_date);
		std::printf("%u.%u.%u\n", (unsigned int)ymd.year, (unsigned int)ymd.month, (unsigned int)ymd.day);
	}
    game_state->single_game_tick();
	{
		auto ymd = game_state->current_date.to_ymd(game_state->start_date);
		std::printf("%u.%u.%u\n", (unsigned int)ymd.year, (unsigned int)ymd.month, (unsigned int)ymd.day);
	}
	sys::write_save_file(*game_state, NATIVE("development_test_save.bin"));
	std::printf("Savefile written\n");
	return EXIT_SUCCESS;
}

