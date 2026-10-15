#include <cstring>
#include "catch.hpp"
#include "parsers_declarations.hpp"
#include "dcon_generated.hpp"
#include "nations.hpp"
#include "container_types.hpp"
#include "system_state.hpp"
#include "serialization.hpp"
#include "prng.hpp"
#include "math_fns.hpp"

TEST_CASE("prng_simple", "[determinism]") {
	std::unique_ptr<sys::state> game_state = std::make_unique<sys::state>(); // too big for the stack
	game_state->game_seed = 64273;
	game_state->current_date.value = 49963;
	auto r1 = rng::get_random(*game_state, ~uint32_t(0x6a3f));
	auto r2 = rng::get_random(*game_state, ~uint32_t(0x6a3f));
	REQUIRE(r1 == r2);
}

TEST_CASE("prng_low_entropy", "[determinism]") {
	std::unique_ptr<sys::state> game_state = std::make_unique<sys::state>(); // too big for the stack
	game_state->game_seed = 1;
	game_state->current_date.value = 1;
	auto r1 = rng::get_random(*game_state, 1);
	auto r2 = rng::get_random(*game_state, 1);
	REQUIRE(r1 == r2);
}

#define UNOPTIMIZABLE_FLOAT(name, value) \
	char name##_storage[sizeof(float)]; \
	new (&name##_storage) float(value); \
	const float *name##_p = std::launder(reinterpret_cast<const float*>(&name##_storage)); \
	const float name = *name##_p;

TEST_CASE("fp_mul", "[determinism]") {
	UNOPTIMIZABLE_FLOAT(f1, 2.f);
	UNOPTIMIZABLE_FLOAT(f2, 4.f);
	REQUIRE(f1 * f2 == 8.f);
	REQUIRE(f2 / f1 == 2.f);
}
TEST_CASE("fp_fmadd", "[determinism]") {
	UNOPTIMIZABLE_FLOAT(f1, 1.f);
	UNOPTIMIZABLE_FLOAT(f2, 2.f);
	UNOPTIMIZABLE_FLOAT(f3, 3.f);
	UNOPTIMIZABLE_FLOAT(f4, 4.f);

	float fmadd_1 = f1 * f2 + f3 * f4; //equation
	float fmadd_2 = f3 * f4 + f1 * f2; //reverse products
	float fmadd_3 = f1 * (f2 + (f3 * f4) / f1); //factorization by f1
	float fmadd_4 = f2 * (f1 + (f3 * f4) / f2); //factorization by f2
	float fmadd_5 = f3 * ((f1 * f2) / f3 + f4); //factorization by f3
	float fmadd_6 = f4 * ((f1 * f2) / f4 + f3); //factorization by f4

	REQUIRE(fmadd_1 == Approx(fmadd_2));
	REQUIRE(fmadd_1 == Approx(fmadd_3));
	REQUIRE(fmadd_1 == Approx(fmadd_4));
	REQUIRE(fmadd_1 == Approx(fmadd_5));
	REQUIRE(fmadd_1 == Approx(fmadd_6));
}

TEST_CASE("fp_fprec", "[determinism]") {
	UNOPTIMIZABLE_FLOAT(f1, 2.f);
	UNOPTIMIZABLE_FLOAT(f2, 4.f);
	UNOPTIMIZABLE_FLOAT(f3, 3.f);

	float fprec_1 = f1 * f2 / f3; //equation
	float fprec_2 = (f1 * f2) / f3; //same
	float fprec_3 = f1 * (f2 / f3); //same
	float fprec_4 = (f1 / f3) * f2; //same

	REQUIRE(fprec_1 == Approx(fprec_2));
	REQUIRE(fprec_1 == Approx(fprec_3));
	REQUIRE(fprec_1 == Approx(fprec_4));
}

TEST_CASE("math_fns", "[determinism]") {
	for(float k = 1.f; k <= 128.f; k += 1.f) {
		UNOPTIMIZABLE_FLOAT(f1, k);
		REQUIRE(math::sqrt(f1) == Approx(std::sqrt(f1)).margin(0.01f));
	}
	for(float k = -1.f; k <= 1.f; k += 0.01f) {
		UNOPTIMIZABLE_FLOAT(f1, k);
		REQUIRE(math::acos(f1) == Approx(std::acos(f1)).margin(0.018f));
	}
	for(float k = -math::pi; k <= math::pi; k += 0.01f) {
		UNOPTIMIZABLE_FLOAT(f1, k);
		REQUIRE(math::sin(f1) == Approx(std::sin(f1)).margin(0.01f));
		REQUIRE(math::cos(f1) == Approx(std::cos(f1)).margin(0.01f));
	}
}

#undef UNOPTIMIZABLE_FLOAT

void compare_game_states(sys::state& ws1, sys::state& ws2) {
	auto ymd = ws1.current_date.to_ymd(ws1.start_date);
	INFO(ymd.year << "." << ymd.month << "." << ymd.day);
	sys::checksum_key ws1_checksum;
	sys::checksum_key ws2_checksum;
	concurrency::parallel_invoke(
		[&]() {
			ws1_checksum = ws1.get_mp_state_checksum();
		},
		[&]() {
			ws2_checksum = ws2.get_mp_state_checksum();
		}
	);
	if(!ws1_checksum.is_equal(ws2_checksum)) {
		std::string oos_report = network::generate_full_oos_report(ws1, ws2);
		simple_fs::write_file(simple_fs::get_or_create_oos_directory(), native_string_view{NATIVE("OOS_TEST.log")}, oos_report.data(), uint32_t(oos_report.length()));
		auto current_date1 = ws1.current_date.to_string(ws1.start_date);
		auto current_date2 = ws2.current_date.to_string(ws2.start_date);
		// Write a save for each seperate state to inspect
		sys::write_save_file(ws1, sys::save_type::normal, current_date1 + "_OOS_SAVE_STATE1");
		sys::write_save_file(ws2, sys::save_type::normal, current_date2 + "_OOS_SAVE_STATE2");

		REQUIRE(false);
	}
}


void write_test_save(sys::state& state) {
	auto current_date = state.current_date.to_string(state.start_date);
	sys::write_save_file(state, sys::save_type::normal, current_date + "_TEST_SAVE", current_date + "_TEST_SAVE");
}


void test_load_save(sys::state& state, uint8_t* ptr_in, uint32_t length) {

	std::vector<dcon::nation_id> players;
	for(const auto n : state.world.in_nation)
		if(state.world.nation_get_is_player_controlled(n))
			players.push_back(n);
	dcon::nation_id old_local_player_nation = state.local_player_nation;
	state.local_player_nation = dcon::nation_id{ };
	// Then reload from network
	state.clear_unsaved_data();
	read_save_section(ptr_in, ptr_in + length, state);
	network::set_no_ai_nations_after_reload(state, players);
	state.local_player_nation = old_local_player_nation;
	state.fill_unsaved_data();
}

constexpr uint32_t test_game_seed = 808080;



// Compares and tests scenarios to see if they are equal after loading. Requires the scenarios to be created and put in the scenario folder before running this
TEST_CASE("compare_scenarios", "[determinism]") {
	std::unique_ptr<sys::state> game_state_1 = std::make_unique<sys::state>();
	game_state_1->network_mode = sys::network_mode_type::host;
	std::unique_ptr<sys::state> game_state_2 = std::make_unique<sys::state>();
	game_state_2->network_mode = sys::network_mode_type::host;
	if(!sys::try_read_scenario_and_save_file(*game_state_1, NATIVE("scenario_1.bin"))) {
		std::abort();
	}
	else {
		game_state_1->game_seed = test_game_seed;
		game_state_1->fill_unsaved_data();
	}
	if(!sys::try_read_scenario_and_save_file(*game_state_2, NATIVE("scenario_2.bin"))) {
		std::abort();
	}
	else {
		game_state_2->game_seed = test_game_seed;
		game_state_2->fill_unsaved_data();
	}

	compare_game_states(*game_state_1, *game_state_2);

}

void do_sim_game_test(uint32_t days_advance, const native_string& savefile = native_string{ }) {
	std::unique_ptr<sys::state> game_state_1;
	std::unique_ptr<sys::state> game_state_2;

	if(savefile.empty()) {
		game_state_1 = load_testing_scenario_file_with_save(sys::network_mode_type::host);
		game_state_2 = load_testing_scenario_file_with_save(sys::network_mode_type::host);
	}
	else {
		game_state_1 = load_testing_scenario_file(sys::network_mode_type::host);
		game_state_2 = load_testing_scenario_file(sys::network_mode_type::host);
		if(!sys::try_read_save_file(*game_state_1, savefile)) {
			assert(false);
			std::abort();
		} else {
			game_state_1->fill_unsaved_data();
		}
		if(!sys::try_read_save_file(*game_state_2, savefile)) {
			assert(false);
			std::abort();
		} else {
			game_state_2->fill_unsaved_data();
		}
	}

	game_state_1->current_scene.game_in_progress = true;
	game_state_2->current_scene.game_in_progress = true;
	game_state_2->game_seed = game_state_1->game_seed = test_game_seed;

	compare_game_states(*game_state_1, *game_state_2);
	for(uint32_t i = 0; i <= days_advance; i++) {
		if(!sys::is_playable_date(game_state_1->current_date, game_state_1->start_date, game_state_1->end_date)) {
			break;
		}
		std::string idx_str = std::to_string(i);
		game_state_1->console_log(idx_str);
		game_state_1->single_game_tick();
		game_state_2->single_game_tick();
		compare_game_states(*game_state_1, *game_state_2);
		sys::write_save_file(*game_state_1, sys::save_type::normal, "OOS_checkpoint_save", "OOS_checkpoint_save"); // Write a checkpoint save if the last OOS check passed sucessfully
	}
}

void do_sim_game_solo_test(const native_string& savefile = native_string{ }) {
	std::unique_ptr<sys::state> game_state_1;
	if(savefile.empty()) {
		game_state_1 = load_testing_scenario_file_with_save(sys::network_mode_type::host);
	}
	else {
		game_state_1 = load_testing_scenario_file(sys::network_mode_type::host);

		if(!sys::try_read_save_file(*game_state_1, savefile)) {
			assert(false);
			std::abort();
		} else {
			game_state_1->fill_unsaved_data();
		}
	}
	game_state_1->current_scene.game_in_progress = true;
	game_state_1->game_seed = test_game_seed;

	for(int i = 0; i <= 3653; i++) {
		game_state_1->console_log(std::to_string(i));
		game_state_1->single_game_tick();
	}
}

void do_fill_unsaved_values_test(uint32_t days_advance, const native_string& savefile = native_string{ }) {
	std::unique_ptr<sys::state> game_state_1;
	if(savefile.empty()) {
		game_state_1 = load_testing_scenario_file_with_save(sys::network_mode_type::host);
	}
	else {
		game_state_1 = load_testing_scenario_file(sys::network_mode_type::host);

		if(!sys::try_read_save_file(*game_state_1, savefile)) {
			assert(false);
			std::abort();
		} else {
			game_state_1->fill_unsaved_data();
		}
	}
	game_state_1->current_scene.game_in_progress = true;
	game_state_1->game_seed = test_game_seed;


	for(uint32_t i = 0; i <= days_advance; i++) {
		if(!sys::is_playable_date(game_state_1->current_date, game_state_1->start_date, game_state_1->end_date)) {
			break;
		}
		std::string idx_str = std::to_string(i);

		game_state_1->console_log(idx_str);
		game_state_1->single_game_tick();

		auto before_key1 = game_state_1->get_save_checksum();

		game_state_1->fill_unsaved_data();


		// make sure that the fill_unsaved_data does not change saved data at all
		REQUIRE(before_key1.to_string() == game_state_1->get_save_checksum().to_string());
	}
}

void do_save_game_with_saveload(uint32_t days_before_reload, uint32_t days_after_reload,const native_string& savefile = native_string{ }) {
	std::unique_ptr<sys::state> game_state_1;
	std::unique_ptr<sys::state> game_state_2;
	if(savefile.empty()) {
		game_state_1 = load_testing_scenario_file_with_save(sys::network_mode_type::host);
		game_state_2 = load_testing_scenario_file_with_save(sys::network_mode_type::host);
	}
	else {
		game_state_1 = load_testing_scenario_file(sys::network_mode_type::host);
		game_state_2 = load_testing_scenario_file(sys::network_mode_type::host);


		if(!sys::try_read_save_file(*game_state_1, savefile)) {
			assert(false);
			std::abort();
		} else {
			game_state_1->fill_unsaved_data();
		}
		if(!sys::try_read_save_file(*game_state_2, savefile)) {
			assert(false);
			std::abort();
		} else {
			game_state_2->fill_unsaved_data();
		}

	}
	game_state_1->current_scene.game_in_progress = true;
	game_state_2->current_scene.game_in_progress = true;

	game_state_2->game_seed = game_state_1->game_seed = test_game_seed;

	compare_game_states(*game_state_1, *game_state_2);

	// run the first gamestate for about five years
	for(uint32_t i = 0; i <= days_before_reload; i++) {
		if(!sys::is_playable_date(game_state_1->current_date, game_state_1->start_date, game_state_1->end_date)) {
			break;
		}
		auto i_str = std::to_string(i);
		game_state_1->console_log(i_str);
		game_state_1->single_game_tick();
	}
	// serialize the save and get both gamestates to reload it
	auto length = sys::sizeof_save_section(*game_state_1);
	auto buffer = std::unique_ptr<uint8_t[]>(new uint8_t[length]);
	auto buffer_position = sys::write_save_section(buffer.get(), *game_state_1);

	test_load_save(*game_state_1, buffer.get(), uint32_t(length));
	test_load_save(*game_state_2, buffer.get(), uint32_t(length));
	compare_game_states(*game_state_1, *game_state_2);

	// run both gamestates for another 5 years
	for(uint32_t i = 0; i <= days_after_reload; i++) {
		if(!sys::is_playable_date(game_state_1->current_date, game_state_1->start_date, game_state_1->end_date)) {
			break;
		}
		auto i_str = std::to_string(i);
		game_state_1->console_log(i_str);
		game_state_1->single_game_tick();
		game_state_2->single_game_tick();
		compare_game_states(*game_state_1, *game_state_2);
	}




}




TEST_CASE("sim_none", "[determinism]") {
	// Test that the game states are equal AFTER loading
	std::unique_ptr<sys::state> game_state_1 = load_testing_scenario_file(sys::network_mode_type::host);
	std::unique_ptr<sys::state> game_state_2 = load_testing_scenario_file(sys::network_mode_type::host);
	game_state_2->game_seed = game_state_1->game_seed = test_game_seed;
	compare_game_states(*game_state_1, *game_state_2);
}




// this test repopulates all the test saves which is used for the tests below. Each test uses a save and runs in a 10-year incremement from the start of that save-
TEST_CASE("populate_test_saves", "[determinism]") {

	std::unique_ptr<sys::state> game_state_1 = load_testing_scenario_file_with_save(sys::network_mode_type::host);

	game_state_1->current_scene.game_in_progress = true;


	game_state_1->game_seed = test_game_seed;

	for(int i = 0; i <= 36530; i++) {
		auto i_str = std::to_string(i);
		game_state_1->console_log(i_str);
		game_state_1->single_game_tick();
		auto current_date = game_state_1->current_date.to_ymd(game_state_1->start_date);
		if(current_date == sys::year_month_day{ 1846, 1, 1 }) {
			write_test_save(*game_state_1);
		}
		else if(current_date == sys::year_month_day{ 1856, 1, 1 }) {
			write_test_save(*game_state_1);
		}
		else if(current_date == sys::year_month_day{ 1866, 1, 1 }) {
			write_test_save(*game_state_1);
		}
		else if(current_date == sys::year_month_day{ 1876, 1, 1 }) {
			write_test_save(*game_state_1);
		}
		else if(current_date == sys::year_month_day{ 1886, 1, 1 }) {
			write_test_save(*game_state_1);
		}
		else if(current_date == sys::year_month_day{ 1896, 1, 1 }) {
			write_test_save(*game_state_1);
		}
		else if(current_date == sys::year_month_day{ 1906, 1, 1 }) {
			write_test_save(*game_state_1);
		}
		else if(current_date == sys::year_month_day{ 1916, 1, 1 }) {
			write_test_save(*game_state_1);
		}
		else if(current_date == sys::year_month_day{ 1926, 1, 1 }) {
			write_test_save(*game_state_1);
			break;
		}

	}
}
//All of the following tests from first to tenth is running the entire game in 10 year intervals with a save for each to test for asserts in debug builds

TEST_CASE("sim_game_solo_1", "[determinism][sim_solo_tests]") {
	do_sim_game_solo_test();
}

TEST_CASE("sim_game_solo_2", "[determinism][sim_solo_tests]") {
	do_sim_game_solo_test(NATIVE("184611_TEST_SAVE.bin"));
}
TEST_CASE("sim_game_solo_3", "[determinism][sim_solo_tests]") {
	do_sim_game_solo_test(NATIVE("185611_TEST_SAVE.bin"));
}
TEST_CASE("sim_game_solo_4", "[determinism][sim_solo_tests]") {
	do_sim_game_solo_test(NATIVE("186611_TEST_SAVE.bin"));
}
TEST_CASE("sim_game_solo_5", "[determinism][sim_solo_tests]") {
	do_sim_game_solo_test(NATIVE("187611_TEST_SAVE.bin"));
}

TEST_CASE("sim_game_solo_6", "[determinism][sim_solo_tests]") {
	do_sim_game_solo_test(NATIVE("188611_TEST_SAVE.bin"));
}
TEST_CASE("sim_game_solo_7", "[determinism][sim_solo_tests]") {
	do_sim_game_solo_test(NATIVE("189611_TEST_SAVE.bin"));
}
TEST_CASE("sim_game_solo_8", "[determinism][sim_solo_tests]") {
	do_sim_game_solo_test(NATIVE("190611_TEST_SAVE.bin"));
}
TEST_CASE("sim_game_solo_9", "[determinism][sim_solo_tests]") {
	do_sim_game_solo_test(NATIVE("191611_TEST_SAVE.bin"));
}
TEST_CASE("sim_game_solo_10", "[determinism][sim_solo_tests]") {
	do_sim_game_solo_test(NATIVE("192611_TEST_SAVE.bin"));
}


//All of the following tests from first to tenth is running the entire game in 10 year intervals with a save for each to test for desync's.

TEST_CASE("sim_game_from_last_checkpoint", "[determinism]") {
	do_sim_game_test(36530, NATIVE("OOS_checkpoint_save.bin"));
}

TEST_CASE("sim_game_1", "[determinism][sim_game_tests]") {
	do_sim_game_test(3653);
}

TEST_CASE("sim_game_2", "[determinism][sim_game_tests]") {
	do_sim_game_test(3653, NATIVE("184611_TEST_SAVE.bin"));
}

TEST_CASE("sim_game_3", "[determinism][sim_game_tests]") {
	do_sim_game_test(3653, NATIVE("185611_TEST_SAVE.bin"));
}
TEST_CASE("sim_game_4", "[determinism][sim_game_tests]") {
	do_sim_game_test(3653, NATIVE("186611_TEST_SAVE.bin"));
}
TEST_CASE("sim_game_5", "[determinism][sim_game_tests]") {
	do_sim_game_test(3653, NATIVE("187611_TEST_SAVE.bin"));
}
TEST_CASE("sim_game_6", "[determinism][sim_game_tests]") {
	do_sim_game_test(3653, NATIVE("188611_TEST_SAVE.bin"));
}
TEST_CASE("sim_game_7", "[determinism][sim_game_tests]") {
	do_sim_game_test(3653, NATIVE("189611_TEST_SAVE.bin"));
}
TEST_CASE("sim_game_8", "[determinism][sim_game_tests]") {
	do_sim_game_test(3653, NATIVE("190611_TEST_SAVE.bin"));
}
TEST_CASE("sim_game_9", "[determinism][sim_game_tests]") {
	do_sim_game_test(3653, NATIVE("191611_TEST_SAVE.bin"));
}
TEST_CASE("sim_game_10", "[determinism][sim_game_tests]") {
	do_sim_game_test(3653, NATIVE("192611_TEST_SAVE.bin"));
}

//All of the following tests from first to tenth is running the entire game in 10 year intervals with a save for each to test if the save checksum changes when fill_unsaved_values is called. It should not change, or something is wrong.

TEST_CASE("fill_unsaved_values_determinism_1", "[determinism][fill_unsaved_tests]") {
	do_fill_unsaved_values_test(3653);
}


TEST_CASE("fill_unsaved_values_determinism_2", "[determinism][fill_unsaved_tests]") {
	do_fill_unsaved_values_test(3653, NATIVE("184611_TEST_SAVE.bin"));
}


TEST_CASE("fill_unsaved_values_determinism_3", "[determinism][fill_unsaved_tests]") {
	do_fill_unsaved_values_test(3653, NATIVE("185611_TEST_SAVE.bin"));
}

TEST_CASE("fill_unsaved_values_determinism_4", "[determinism][fill_unsaved_tests]") {
	do_fill_unsaved_values_test(3653, NATIVE("186611_TEST_SAVE.bin"));
}

TEST_CASE("fill_unsaved_values_determinism_5", "[determinism][fill_unsaved_tests]") {
	do_fill_unsaved_values_test(3653, NATIVE("187611_TEST_SAVE.bin"));
}

TEST_CASE("fill_unsaved_values_determinism_6", "[determinism][fill_unsaved_tests]") {
	do_fill_unsaved_values_test(3653, NATIVE("188611_TEST_SAVE.bin"));
}


TEST_CASE("fill_unsaved_values_determinism_7", "[determinism][fill_unsaved_tests]") {
	do_fill_unsaved_values_test(3653, NATIVE("189611_TEST_SAVE.bin"));
}

TEST_CASE("fill_unsaved_values_determinism_8", "[determinism][fill_unsaved_tests]") {
	do_fill_unsaved_values_test(3653, NATIVE("190611_TEST_SAVE.bin"));
}

TEST_CASE("fill_unsaved_values_determinism_9", "[determinism][fill_unsaved_tests]") {
	do_fill_unsaved_values_test(3653, NATIVE("191611_TEST_SAVE.bin"));
}

TEST_CASE("fill_unsaved_values_determinism_10", "[determinism][fill_unsaved_tests]") {
	do_fill_unsaved_values_test(3653, NATIVE("192611_TEST_SAVE.bin"));
}



TEST_CASE("sim_game_with_saveload_1", "[determinism][sim_with_saveload_tests]") {
	do_save_game_with_saveload(1826, 1826);
}


TEST_CASE("sim_game_with_saveload_2", "[determinism][sim_with_saveload_tests]") {
	do_save_game_with_saveload(1826, 1826, NATIVE("184611_TEST_SAVE.bin"));


}

TEST_CASE("sim_game_with_saveload_3", "[determinism][sim_with_saveload_tests]") {


	do_save_game_with_saveload(1826, 1826, NATIVE("185611_TEST_SAVE.bin"));

}

TEST_CASE("sim_game_with_saveload_4", "[determinism][sim_with_saveload_tests]") {


	do_save_game_with_saveload(1826, 1826, NATIVE("186611_TEST_SAVE.bin"));

}

TEST_CASE("sim_game_with_saveload_5", "[determinism][sim_with_saveload_tests]") {


	do_save_game_with_saveload(1826, 1826, NATIVE("187611_TEST_SAVE.bin"));
}

TEST_CASE("sim_game_with_saveload_6", "[determinism][sim_with_saveload_tests]") {


	do_save_game_with_saveload(1826, 1826, NATIVE("188611_TEST_SAVE.bin"));

}

TEST_CASE("sim_game_with_saveload_7", "[determinism][sim_with_saveload_tests]") {


	do_save_game_with_saveload(1826, 1826, NATIVE("189611_TEST_SAVE.bin"));

}

TEST_CASE("sim_game_with_saveload_8", "[determinism][sim_with_saveload_tests]") {


	do_save_game_with_saveload(1826, 1826, NATIVE("190611_TEST_SAVE.bin"));

}

TEST_CASE("sim_game_with_saveload_9", "[determinism][sim_with_saveload_tests]") {


	do_save_game_with_saveload(1826, 1826, NATIVE("191611_TEST_SAVE.bin"));

}

TEST_CASE("sim_game_with_saveload_10", "[determinism][sim_with_saveload_tests]") {


	do_save_game_with_saveload(1826, 1826, NATIVE("192611_TEST_SAVE.bin"));

}
