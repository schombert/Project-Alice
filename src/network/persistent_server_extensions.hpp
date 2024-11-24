#pragma once

#include <array>
#include <string>
#include "container_types.hpp"
#include "commands.hpp"
#include <iostream>
#include <chrono>
#include <ctime>
#include "system_state.hpp"
#include "network.hpp"
#include "parsers.hpp"
#include "simple_fs.hpp"
#include "network.hpp"
#include "parsers.hpp"

namespace network {

int lastchange = 0;

inline void read_player_nations(sys::state& state, char const* start, char const* end) noexcept {
	char const* cpos = start;
	parsers::error_handler err("");

	while(cpos < end) {
		cpos = parsers::parse_fixed_amount_csv_values<2>(cpos, end, ';', [&](std::string_view const* values) {
			auto first_text = parsers::remove_surrounding_whitespace(values[0]);
			auto second_text = parsers::remove_surrounding_whitespace(values[1]);
			if(first_text.length() > 0 && second_text.length() > 0) {
				auto second_value = parsers::parse_int(first_text, 0, err);
				if(second_value == 0) {
					// dead line
				} else {
					auto p = sys::player_name{ }.from_string_view(first_text);
					auto nid = dcon::nation_id{ uint16_t ( second_value ) };

					/* Materialize it into a command we send to new clients who connect and have to replay everything... */
					/* Don't execute it immediately for game loading & UI reasons */
					command::payload c;
					memset(&c, 0, sizeof(c));
					c.type = command::command_type::notify_player_joins;
					c.source = nid;
					c.data.player_name = p;
					state.network_state.outgoing_commands.push(c);
				}
			}
		});
	}
}

inline void load_player_nations(sys::state& state) noexcept {
	if(state.network_mode != sys::network_mode_type::host || state.defines.alice_persistent_server_mode != 1) {
		return;
	}

	auto folder = simple_fs::get_or_create_data_dumps_directory();

	auto pl_csv_file = open_file(folder, NATIVE("players.csv"));
	if(pl_csv_file) {
		auto pl_content = view_contents(*pl_csv_file);
		read_player_nations(state, pl_content.data, pl_content.data + pl_content.file_size);
	}
}

inline void write_player_nations(sys::state& state) noexcept {
	if(state.network_mode != sys::network_mode_type::host || state.defines.alice_persistent_server_mode != 1) {
		return;
	}

	std::string res = "";

	for(auto pl : state.world.in_mp_player) {
		res += sys::player_name{ pl.get_nickname() }.to_string() + ";" + std::to_string(pl.get_nation_from_player_nation().id.index()) + "\n";
	}
	auto folder = simple_fs::get_or_create_data_dumps_directory();

	simple_fs::write_file(folder, NATIVE("players.csv"), res.data(), uint32_t(res.length()));
}

inline void every_tick_checks(sys::state& state) noexcept {
	if(state.network_mode != sys::network_mode_type::host || state.defines.alice_persistent_server_mode != 1) {
		return;
	}

	auto moment = std::chrono::system_clock::now();
	std::time_t curtime = std::chrono::system_clock::to_time_t(moment);
	int h = (curtime / 3600) % 24;
	tm local_tm = *localtime(&curtime);

	if(h >= state.defines.alice_persistent_server_pause && state.actual_game_speed != 0 && local_tm.tm_yday != lastchange) {
		pause_game(state);

		lastchange = local_tm.tm_yday;
	}

	if(h >= state.defines.alice_persistent_server_unpause && h < state.defines.alice_persistent_server_pause && state.actual_game_speed != state.ui_state.held_game_speed && local_tm.tm_yday != lastchange) {
		unpause_game(state);

		lastchange = local_tm.tm_yday;
	}
}
}

