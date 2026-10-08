#include "catch.hpp"
#include "military.hpp"

// This is commented out because the supply reinforcement logic is split up now

//TEST_CASE("update_supply_reinforcement_profile", "[military_profiling]") {
//
//	gamestate = load_testing_scenario_file_with_save(sys::network_mode_type::host);
//
//	{
//		std::chrono::steady_clock::time_point time_start = std::chrono::steady_clock::now();
//		for(int i = 0; i < 1000000; i++) {
//			military::update_regiment_supply_reinforcement_satisfaction(*gamestate);
//		}
//		std::chrono::steady_clock::time_point time_end = std::chrono::steady_clock::now();
//		auto net_time = time_end - time_start;
//		gamestate->console_log("Parallel ns: " + std::to_string(net_time.count()));
//	}
//
//	{
//		std::chrono::steady_clock::time_point time_start = std::chrono::steady_clock::now();
//		for(int i = 0; i < 1000000; i++) {
//			military::update_regiment_supply_reinforcement_satisfaction_serial(*gamestate);
//		}
//		std::chrono::steady_clock::time_point time_end = std::chrono::steady_clock::now();
//		auto net_time = time_end - time_start;
//		gamestate->console_log("Serial ns: " + std::to_string(net_time.count()));
//	}
//
//	std::this_thread::sleep_for(std::chrono::seconds{ 1 });
//}
