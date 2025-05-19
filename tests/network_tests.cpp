#include "catch.hpp"
#include "dcon_generated.hpp"
#include <chrono>
#include <thread>


TEST_CASE("run_fresh_lobby", "[determinism]") {
	std::unique_ptr<sys::state> host_game_state = load_testing_scenario_file();
	host_game_state->network_mode = sys::network_mode_type::host;
	// set name of host "A"
	//host_game_state->network_state.nickname.from_string_view("Host");
	host_game_state->actual_game_speed = 1;
	host_game_state->cheat_data.daily_oos_check = true;
	network::init(*host_game_state);
	std::thread update_thread_host([&]() { host_game_state->game_loop(); });
	command::notify_player_picks_nation(*host_game_state, host_game_state->local_player_nation, dcon::nation_id{ 1 });



	std::unique_ptr<sys::state> client_game_state = load_testing_scenario_file();
	client_game_state->network_mode = sys::network_mode_type::client;
	client_game_state->network_state.ip_address = "127.0.0.1";
	// set name of client "B"
	//client_game_state->network_state.nickname.from_string_view("Client");
	client_game_state->cheat_data.daily_oos_check = true;

	network::init(*client_game_state);
	std::thread update_thread_client([&]() { client_game_state->game_loop(); });
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	command::notify_player_picks_nation(*client_game_state, client_game_state->local_player_nation, dcon::nation_id{5 });
	command::notify_start_game(*host_game_state, host_game_state->local_player_nation);
	
	while(true) {
		std::cout << "Host date: " << host_game_state->current_date.to_string(host_game_state->start_date);
		std::cout << "Client date: " << client_game_state->current_date.to_string(client_game_state->start_date);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	update_thread_host.join();
	update_thread_client.join();
}
