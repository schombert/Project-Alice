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
#include "SHA512.hpp"

namespace network {

static int lastchange = 0;

inline void read_player_nations(sys::state& state, char const* start, char const* end) noexcept {
	char const* cpos = start;
	parsers::error_handler err("");

	while(cpos < end) {
		cpos = parsers::parse_fixed_amount_csv_values<4>(cpos, end, ';', [&](std::string_view const* values) {
			auto first_text = parsers::remove_surrounding_whitespace(values[0]);
			auto second_text = parsers::remove_surrounding_whitespace(values[1]);
			auto third_text = parsers::remove_surrounding_whitespace(values[2]);
			auto fourth_text = parsers::remove_surrounding_whitespace(values[3]);


			if(first_text.length() > 0 && second_text.length() > 0) {
				auto second_value = parsers::parse_int(second_text, 0, err);
				if(second_value == 0) {
					// dead line
				} else {
					auto name = sys::player_name{ }.from_string_view(first_text);
					auto source = dcon::nation_id{ uint16_t ( second_value ) };
					auto password_hash = sys::player_password_hash{ }.from_string_view(third_text);
					auto password_salt = sys::player_password_salt{ }.from_string_view(fourth_text);

					auto p = load_mp_player(state, name, password_hash, password_salt);

					state.world.nation_set_is_player_controlled(source, true);
					state.world.force_create_player_nation(source, p);

					ui::chat_message m{};
					m.source = source;
					text::substitution_map sub{};
					text::add_to_substitution_map(sub, text::variable_type::playername, name.to_string_view());
					auto msg = text::resolve_string_substitution(state, "chat_player_joins", sub);
					command::chat_message(state, source, msg, dcon::nation_id{});

#ifndef NDEBUG
					network::log_player_nations(state);
					state.console_log("persistent | type:read_player | nickname:" + name.to_string() + " | nation:" + std::to_string(source.index()) + "| password:" + password_hash.to_string() + " | salt: " + password_salt.to_string());
#endif
				}
			}
		});
	}
}

inline void load_player_nations(sys::state& state) noexcept {
	if(state.network_mode != sys::network_mode_type::host || state.host_settings.alice_persistent_server_mode != 1) {
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
	if(state.network_mode != sys::network_mode_type::host || state.host_settings.alice_persistent_server_mode != 1) {
		return;
	}

	std::string res = "Nickname;NationID;PasswordHash;PasswordSalt\n";

	for(auto pl : state.world.in_mp_player) {
		auto nickname = sys::player_name{ pl.get_nickname() };
		auto password_hash = sys::player_password_hash{ pl.get_password_hash() };
		auto password_salt = sys::player_password_salt{ pl.get_password_salt() };
		res += nickname.to_string() + ";" + std::to_string(pl.get_nation_from_player_nation().id.index()) + ";"
			+ password_hash.to_string() + ";" + password_salt.to_string() + "\n";
	}
	auto folder = simple_fs::get_or_create_data_dumps_directory();

	simple_fs::write_file(folder, NATIVE("players.csv"), res.data(), uint32_t(res.length()));
}

inline void every_tick_checks(sys::state& state) noexcept {
	if(state.network_mode != sys::network_mode_type::host || state.host_settings.alice_persistent_server_mode != 1) {
		return;
	}

	auto moment = std::chrono::system_clock::now();
	std::time_t curtime = std::chrono::system_clock::to_time_t(moment);
	int h = (curtime / 3600) % 24;
	tm local_tm = *localtime(&curtime);

	if(h >= state.host_settings.alice_persistent_server_pause && state.actual_game_speed != 0 && local_tm.tm_yday != lastchange) {
		pause_game(state);

		lastchange = local_tm.tm_yday;
	}

	if(h >= state.host_settings.alice_persistent_server_unpause && h < state.host_settings.alice_persistent_server_pause && state.actual_game_speed != state.ui_state.held_game_speed && local_tm.tm_yday != lastchange) {
		unpause_game(state);

		lastchange = local_tm.tm_yday;
	}
}
}

