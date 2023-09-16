#pragma once

#include <memory>
#include <stdint.h>
#include <atomic>
#include <chrono>


#include "window.hpp"
#include "constants.hpp"
#include "dcon_generated.hpp"
#include "gui_graphics.hpp"
#include "simple_fs.hpp"
#include "text.hpp"
#include "opengl_wrapper.hpp"
#include "fonts.hpp"
#include "sound.hpp"
#include "map_state.hpp"
#include "economy.hpp"
#include "culture.hpp"
#include "military.hpp"
#include "nations.hpp"
#include "date_interface.hpp"
#include "defines.hpp"
#include "province.hpp"
#include "events.hpp"
#include "SPSCQueue.h"
#include "commands.hpp"
#include "diplomatic_messages.hpp"
#include "events.hpp"
#include "notifications.hpp"

// this header will eventually contain the highest-level objects
// that represent the overall state of the program
// it will also include the game state itself eventually as a member

namespace sys {

enum class gui_modes : uint8_t { faithful = 0, nouveau = 1, dummycabooseval = 2 };

struct user_settings_s {
	float ui_scale = 1.0f;
	float master_volume = 0.5f;
	float music_volume = 1.0f;
	float effects_volume = 1.0f;
	float interface_volume = 1.0f;
	bool prefer_fullscreen = false;
	bool map_is_globe = true;
	autosave_frequency autosaves = autosave_frequency::yearly;
	bool bind_tooltip_mouse = true;
	bool use_classic_fonts = false;
	bool outliner_views[14] = {true, true, true, true, true, true, true, true, true, true, true, true, true, true};
	uint8_t self_message_settings[int32_t(sys::message_setting_type::count)] = {
		message_response::standard_pause,//revolt = 0,
		message_response::standard_pause,//war_on_nation = 1,
		message_response::ignore,//war_by_nation = 2,
		message_response::standard_popup,//wargoal_added = 3,
		message_response::ignore,//siegeover_by_nation = 4,
		message_response::ignore,//siegeover_on_nation = 5,
		message_response::standard_popup,//colony_finished = 6,
		message_response::ignore,//reform_gained = 7,
		message_response::ignore,//reform_lost = 8,
		message_response::ignore,//ruling_party_change = 9,
		message_response::standard_popup,//upperhouse = 10,
		message_response::standard_popup,//electionstart = 11,
		message_response::standard_popup,//electiondone = 12,
		message_response::standard_popup,//breakcountry = 13,
		message_response::ignore,//peace_accepted_from_nation = 14,
		message_response::standard_popup,//peace_rejected_from_nation = 15,
		message_response::ignore,//peace_accepted_by_nation = 16,
		message_response::ignore,//peace_rejected_by_nation = 17,
		message_response::ignore,//mobilization_start = 18,
		message_response::ignore,//mobilization_end = 19,
		message_response::log,//factory_complete = 20,
		message_response::log,//rr_complete = 21,
		message_response::log,//fort_complete = 22,
		message_response::log,//naval_base_complete = 23,
		message_response::log,//province_event = 24,
		message_response::log,//national_event = 25,
		message_response::log,//major_event = 26,
		message_response::standard_popup,//invention = 27,
		message_response::standard_popup,//tech = 28,
		message_response::log,//leader_dies = 29,
		message_response::ignore,//land_combat_starts_on_nation = 30,
		message_response::ignore,//naval_combat_starts_on_nation = 31,
		message_response::ignore,//land_combat_starts_by_nation = 32,
		message_response::ignore,//naval_combat_starts_by_nation = 33,
		message_response::ignore,//movement_finishes = 34,
		message_response::ignore,//decision = 35,
		message_response::standard_popup,//lose_great_power = 36,
		message_response::standard_popup,//become_great_power = 37,
		message_response::ignore,//war_subsidies_start_by_nation = 38,
		message_response::standard_popup,//war_subsidies_start_on_nation = 39,
		message_response::standard_popup,//war_subsidies_end_by_nation = 40,
		message_response::standard_popup,//war_subsidies_end_on_nation = 41,
		message_response::ignore,//reparations_start_by_nation = 42,
		message_response::ignore,//reparations_start_on_nation = 43,
		message_response::standard_popup,//reparations_end_by_nation = 44,
		message_response::standard_popup,//reparations_end_on_nation = 45,
		message_response::ignore,//mil_access_start_by_nation = 46,
		message_response::ignore,//mil_access_start_on_nation = 47,
		message_response::ignore,//mil_access_end_by_nation = 48,
		message_response::standard_popup,//mil_access_end_on_nation = 49,
		message_response::ignore,//mil_access_declined_by_nation = 50,
		message_response::standard_popup,//mil_access_declined_on_nation = 51,
		message_response::ignore,//alliance_starts = 52,
		message_response::standard_popup,//alliance_ends = 53,
		message_response::ignore,//alliance_declined_by_nation = 54,
		message_response::standard_popup,//alliance_declined_on_nation = 55,
		message_response::ignore,//ally_called_accepted_by_nation = 56,
		message_response::standard_popup,//ally_called_declined_by_nation = 57,
		message_response::ignore,//discredit_by_nation = 58,
		message_response::ignore,//ban_by_nation = 59,
		message_response::ignore,//expell_by_nation = 60,
		message_response::standard_popup,//discredit_on_nation = 61,
		message_response::standard_popup,//ban_on_nation = 62,
		message_response::standard_popup,//expell_on_nation = 63,
		message_response::ignore,//increase_opinion = 64,
		message_response::ignore,//decrease_opinion_by_nation = 65,
		message_response::standard_popup,//decrease_opinion_on_nation = 66,
		message_response::ignore,//rem_sphere_by_nation = 67,
		message_response::standard_popup,//rem_sphere_on_nation = 68,
		message_response::standard_popup,//removed_from_sphere = 69,
		message_response::ignore,//add_sphere = 70,
		message_response::standard_popup,//added_to_sphere = 71,
		message_response::ignore,//increase_relation_by_nation = 72,
		message_response::standard_popup,//increase_relation_on_nation = 73,
		message_response::ignore,//decrease_relation_by_nation = 74,
		message_response::standard_popup,//decrease_relation_on_nation = 75,
		message_response::ignore,//join_war_by_nation = 76,
		message_response::standard_popup,//join_war_on_nation = 77,
		message_response::standard_popup,//gw_unlocked = 78,
		message_response::standard_popup,//war_becomes_great = 79,
		message_response::ignore,//cb_detected_on_nation = 80,
		message_response::standard_popup,//cb_detected_by_nation = 81,
		message_response::ignore,//crisis_join_offer_accepted_by_nation = 82,
		message_response::ignore,//crisis_join_offer_declined_by_nation = 83,
		message_response::ignore,//crisis_join_offer_accepted_from_nation = 84,
		message_response::standard_popup,//crisis_join_offer_declined_from_nation = 85,
		message_response::standard_popup,//crisis_resolution_accepted = 86,
		message_response::standard_popup,//crisis_becomes_war = 87,
		message_response::standard_popup,//crisis_resolution_declined_from_nation = 88,
		message_response::standard_popup,//crisis_starts = 89,
		message_response::ignore,//crisis_attacker_backer = 90,
		message_response::ignore,//crisis_defender_backer = 91,
		message_response::standard_popup,//crisis_fizzle = 92,
		message_response::ignore,//war_join_by = 93,
		message_response::standard_popup,//war_join_on = 94,
		message_response::standard_popup,//cb_fab_finished = 95,
		message_response::standard_popup,//cb_fab_cancelled = 96,
		message_response::ignore,//crisis_voluntary_joi_on = 97,
	};
	uint8_t interesting_message_settings[int32_t(sys::message_setting_type::count)] = {
		message_response::log,//revolt = 0,
		message_response::log,//war_on_nation = 1,
		message_response::ignore,//war_by_nation = 2,
		message_response::log,//wargoal_added = 3,
		message_response::ignore,//siegeover_by_nation = 4,
		message_response::ignore,//siegeover_on_nation = 5,
		message_response::log,//colony_finished = 6,
		message_response::ignore,//reform_gained = 7,
		message_response::ignore,//reform_lost = 8,
		message_response::ignore,//ruling_party_change = 9,
		message_response::log,//upperhouse = 10,
		message_response::log,//electionstart = 11,
		message_response::log,//electiondone = 12,
		message_response::log,//breakcountry = 13,
		message_response::ignore,//peace_accepted_from_nation = 14,
		message_response::log,//peace_rejected_from_nation = 15,
		message_response::ignore,//peace_accepted_by_nation = 16,
		message_response::ignore,//peace_rejected_by_nation = 17,
		message_response::ignore,//mobilization_start = 18,
		message_response::ignore,//mobilization_end = 19,
		message_response::ignore,//factory_complete = 20,
		message_response::ignore,//rr_complete = 21,
		message_response::ignore,//fort_complete = 22,
		message_response::ignore,//naval_base_complete = 23,
		message_response::log,//province_event = 24,
		message_response::log,//national_event = 25,
		message_response::log,//major_event = 26,
		message_response::log,//invention = 27,
		message_response::log,//tech = 28,
		message_response::ignore,//leader_dies = 29,
		message_response::ignore,//land_combat_starts_on_nation = 30,
		message_response::ignore,//naval_combat_starts_on_nation = 31,
		message_response::ignore,//land_combat_starts_by_nation = 32,
		message_response::ignore,//naval_combat_starts_by_nation = 33,
		message_response::ignore,//movement_finishes = 34,
		message_response::ignore,//decision = 35,
		message_response::log,//lose_great_power = 36,
		message_response::log,//become_great_power = 37,
		message_response::log,//war_subsidies_start_by_nation = 38,
		message_response::log,//war_subsidies_start_on_nation = 39,
		message_response::log,//war_subsidies_end_by_nation = 40,
		message_response::log,//war_subsidies_end_on_nation = 41,
		message_response::log,//reparations_start_by_nation = 42,
		message_response::log,//reparations_start_on_nation = 43,
		message_response::log,//reparations_end_by_nation = 44,
		message_response::log,//reparations_end_on_nation = 45,
		message_response::ignore,//mil_access_start_by_nation = 46,
		message_response::ignore,//mil_access_start_on_nation = 47,
		message_response::ignore,//mil_access_end_by_nation = 48,
		message_response::ignore,//mil_access_end_on_nation = 49,
		message_response::ignore,//mil_access_declined_by_nation = 50,
		message_response::ignore,//mil_access_declined_on_nation = 51,
		message_response::log,//alliance_starts = 52,
		message_response::log,//alliance_ends = 53,
		message_response::ignore,//alliance_declined_by_nation = 54,
		message_response::ignore,//alliance_declined_on_nation = 55,
		message_response::ignore,//ally_called_accepted_by_nation = 56,
		message_response::ignore,//ally_called_declined_by_nation = 57,
		message_response::ignore,//discredit_by_nation = 58,
		message_response::log,//ban_by_nation = 59,
		message_response::log,//expell_by_nation = 60,
		message_response::log,//discredit_on_nation = 61,
		message_response::ignore,//ban_on_nation = 62,
		message_response::ignore,//expell_on_nation = 63,
		message_response::ignore,//increase_opinion = 64,
		message_response::log,//decrease_opinion_by_nation = 65,
		message_response::log,//decrease_opinion_on_nation = 66,
		message_response::log,//rem_sphere_by_nation = 67,
		message_response::log,//rem_sphere_on_nation = 68,
		message_response::log,//removed_from_sphere = 69,
		message_response::log,//add_sphere = 70,
		message_response::log,//added_to_sphere = 71,
		message_response::log,//increase_relation_by_nation = 72,
		message_response::log,//increase_relation_on_nation = 73,
		message_response::log,//decrease_relation_by_nation = 74,
		message_response::log,//decrease_relation_on_nation = 75,
		message_response::log,//join_war_by_nation = 76,
		message_response::log,//join_war_on_nation = 77,
		message_response::ignore,//gw_unlocked = 78,
		message_response::ignore,//war_becomes_great = 79,
		message_response::ignore,//cb_detected_on_nation = 80,
		message_response::standard_popup,//cb_detected_by_nation = 81,
		message_response::log,//crisis_join_offer_accepted_by_nation = 82,
		message_response::ignore,//crisis_join_offer_declined_by_nation = 83,
		message_response::ignore,//crisis_join_offer_accepted_from_nation = 84,
		message_response::ignore,//crisis_join_offer_declined_from_nation = 85,
		message_response::ignore,//crisis_resolution_accepted = 86,
		message_response::ignore,//crisis_becomes_war = 87,
		message_response::ignore,//crisis_resolution_declined_from_nation = 88,
		message_response::ignore,//crisis_starts = 89,
		message_response::standard_popup,//crisis_attacker_backer = 90,
		message_response::standard_popup,//crisis_defender_backer = 91,
		message_response::ignore,//crisis_fizzle = 92,
		message_response::log,//war_join_by = 93,
		message_response::ignore,//war_join_on = 94,
		message_response::ignore,//cb_fab_finished = 95,
		message_response::ignore,//cb_fab_cancelled = 96,
		message_response::standard_popup,//crisis_voluntary_join_on = 97,
	};
	uint8_t other_message_settings[int32_t(sys::message_setting_type::count)] = {
		message_response::ignore,//revolt = 0,
		message_response::ignore,//war_on_nation = 1,
		message_response::ignore,//war_by_nation = 2,
		message_response::ignore,//wargoal_added = 3,
		message_response::ignore,//siegeover_by_nation = 4,
		message_response::ignore,//siegeover_on_nation = 5,
		message_response::ignore,//colony_finished = 6,
		message_response::ignore,//reform_gained = 7,
		message_response::ignore,//reform_lost = 8,
		message_response::ignore,//ruling_party_change = 9,
		message_response::ignore,//upperhouse = 10,
		message_response::ignore,//electionstart = 11,
		message_response::ignore,//electiondone = 12,
		message_response::ignore,//breakcountry = 13,
		message_response::ignore,//peace_accepted_from_nation = 14,
		message_response::ignore,//peace_rejected_from_nation = 15,
		message_response::ignore,//peace_accepted_by_nation = 16,
		message_response::ignore,//peace_rejected_by_nation = 17,
		message_response::ignore,//mobilization_start = 18,
		message_response::ignore,//mobilization_end = 19,
		message_response::ignore,//factory_complete = 20,
		message_response::ignore,//rr_complete = 21,
		message_response::ignore,//fort_complete = 22,
		message_response::ignore,//naval_base_complete = 23,
		message_response::ignore,//province_event = 24,
		message_response::ignore,//national_event = 25,
		message_response::standard_popup,//major_event = 26,
		message_response::ignore,//invention = 27,
		message_response::ignore,//tech = 28,
		message_response::ignore,//leader_dies = 29,
		message_response::ignore,//land_combat_starts_on_nation = 30,
		message_response::ignore,//naval_combat_starts_on_nation = 31,
		message_response::ignore,//land_combat_starts_by_nation = 32,
		message_response::ignore,//naval_combat_starts_by_nation = 33,
		message_response::ignore,//movement_finishes = 34,
		message_response::ignore,//decision = 35,
		message_response::ignore,//lose_great_power = 36,
		message_response::standard_popup,//become_great_power = 37,
		message_response::ignore,//war_subsidies_start_by_nation = 38,
		message_response::ignore,//war_subsidies_start_on_nation = 39,
		message_response::ignore,//war_subsidies_end_by_nation = 40,
		message_response::ignore,//war_subsidies_end_on_nation = 41,
		message_response::ignore,//reparations_start_by_nation = 42,
		message_response::ignore,//reparations_start_on_nation = 43,
		message_response::ignore,//reparations_end_by_nation = 44,
		message_response::ignore,//reparations_end_on_nation = 45,
		message_response::ignore,//mil_access_start_by_nation = 46,
		message_response::ignore,//mil_access_start_on_nation = 47,
		message_response::ignore,//mil_access_end_by_nation = 48,
		message_response::ignore,//mil_access_end_on_nation = 49,
		message_response::ignore,//mil_access_declined_by_nation = 50,
		message_response::ignore,//mil_access_declined_on_nation = 51,
		message_response::ignore,//alliance_starts = 52,
		message_response::ignore,//alliance_ends = 53,
		message_response::ignore,//alliance_declined_by_nation = 54,
		message_response::ignore,//alliance_declined_on_nation = 55,
		message_response::ignore,//ally_called_accepted_by_nation = 56,
		message_response::ignore,//ally_called_declined_by_nation = 57,
		message_response::ignore,//discredit_by_nation = 58,
		message_response::ignore,//ban_by_nation = 59,
		message_response::ignore,//expell_by_nation = 60,
		message_response::ignore,//discredit_on_nation = 61,
		message_response::ignore,//ban_on_nation = 62,
		message_response::ignore,//expell_on_nation = 63,
		message_response::ignore,//increase_opinion = 64,
		message_response::ignore,//decrease_opinion_by_nation = 65,
		message_response::ignore,//decrease_opinion_on_nation = 66,
		message_response::ignore,//rem_sphere_by_nation = 67,
		message_response::ignore,//rem_sphere_on_nation = 68,
		message_response::ignore,//removed_from_sphere = 69,
		message_response::ignore,//add_sphere = 70,
		message_response::ignore,//added_to_sphere = 71,
		message_response::ignore,//increase_relation_by_nation = 72,
		message_response::ignore,//increase_relation_on_nation = 73,
		message_response::ignore,//decrease_relation_by_nation = 74,
		message_response::ignore,//decrease_relation_on_nation = 75,
		message_response::ignore,//join_war_by_nation = 76,
		message_response::ignore,//join_war_on_nation = 77,
		message_response::ignore,//gw_unlocked = 78,
		message_response::ignore,//war_becomes_great = 79,
		message_response::ignore,//cb_detected_on_nation = 80,
		message_response::standard_popup,//cb_detected_by_nation = 81,
		message_response::ignore,//crisis_join_offer_accepted_by_nation = 82,
		message_response::ignore,//crisis_join_offer_declined_by_nation = 83,
		message_response::ignore,//crisis_join_offer_accepted_from_nation = 84,
		message_response::ignore,//crisis_join_offer_declined_from_nation = 85,
		message_response::ignore,//crisis_resolution_accepted = 86,
		message_response::ignore,//crisis_becomes_war = 87,
		message_response::ignore,//crisis_resolution_declined_from_nation = 88,
		message_response::ignore,//crisis_starts = 89,
		message_response::standard_popup,//crisis_attacker_backer = 90,
		message_response::standard_popup, //crisis_defender_backer = 91,
		message_response::ignore, //crisis_fizzle = 92,
		message_response::ignore, //war_join_by = 93,
		message_response::ignore,//war_join_on = 94,
		message_response::ignore,//cb_fab_finished = 95,
		message_response::ignore,//cb_fab_cancelled = 96,
		message_response::standard_popup,//crisis_voluntary_join_on = 97,
	};
};

struct global_scenario_data_s { // this struct holds miscellaneous global properties of the scenario
};

struct crisis_member_def {
	dcon::nation_id id;

	crisis_join_offer joined_with_offer;

	bool supports_attacker = false;
	bool merely_interested = false;
};
enum class crisis_type : uint32_t { none = 0, claim = 1, liberation = 2, colonial = 3, influence = 4 };
enum class crisis_mode : uint32_t { inactive = 0, finding_attacker = 1, finding_defender = 2, heating_up = 3 };

struct great_nation {
	sys::date last_greatness = sys::date(0);
	dcon::nation_id nation;

	great_nation(sys::date last_greatness, dcon::nation_id nation) : last_greatness(last_greatness), nation(nation) { }
	great_nation() = default;
};

struct player_data { // currently this data is serialized via memcpy, to make sure no pointers end up in here
	std::array<float, 32> treasury_record = {0.0f}; // current day's value = date.value & 31
	std::array<float, 32> population_record = { 0.0f }; // current day's value = date.value & 31
};

// the state struct will eventually include (at least pointers to)
// the state of the sound system, the state of the windowing system,
// and the game data / state itself

// ultimately it is the system struct that gets passed around everywhere
// so that bits of the ui, for example, can control the overall state of
// the game

struct alignas(64) state {
	dcon::data_container world;

	// scenario data

	parsing::defines defines;

	economy::global_economy_state economy_definitions;
	culture::global_cultural_state culture_definitions;
	military::global_military_state military_definitions;
	nations::global_national_state national_definitions;
	province::global_provincial_state province_definitions;

	absolute_time_point start_date;
	absolute_time_point end_date;

	std::vector<uint16_t> trigger_data;
	std::vector<int32_t> trigger_data_indices;
	std::vector<uint16_t> effect_data;
	std::vector<int32_t> effect_data_indices;
	std::vector<value_modifier_segment> value_modifier_segments;
	tagged_vector<value_modifier_description, dcon::value_modifier_key> value_modifiers;

	std::vector<char> text_data; // stores string data in the win1250 codepage
	std::vector<text::text_component> text_components;
	tagged_vector<text::text_sequence, dcon::text_sequence_id> text_sequences;
	ankerl::unordered_dense::map<dcon::text_key, dcon::text_sequence_id, text::vector_backed_hash, text::vector_backed_eq>
			key_to_text_sequence;

	bool adjacency_data_out_of_date = true;
	bool national_cached_values_out_of_date = false;
	bool diplomatic_cached_values_out_of_date = false;
	std::vector<dcon::nation_id> nations_by_rank;
	std::vector<dcon::nation_id> nations_by_industrial_score;
	std::vector<dcon::nation_id> nations_by_military_score;
	std::vector<dcon::nation_id> nations_by_prestige_score;
	std::vector<great_nation> great_nations;

	uint64_t scenario_time_stamp = 0;	// for identifying the scenario file
	uint32_t scenario_counter = 0;		// as above
	sys::checksum_key scenario_checksum;// for checksum for savefiles
	sys::checksum_key session_host_checksum;// for checking that the client can join a session
	native_string loaded_scenario_file;
	native_string loaded_save_file;

	//
	// Crisis data
	//

	dcon::state_instance_id crisis_state;
	std::vector<crisis_member_def> crisis_participants;
	crisis_type current_crisis = crisis_type::none;
	float crisis_temperature = 0;
	dcon::nation_id primary_crisis_attacker;
	dcon::nation_id primary_crisis_defender;
	crisis_mode current_crisis_mode;
	uint32_t crisis_last_checked_gp = 0;
	dcon::war_id crisis_war;
	sys::date last_crisis_end_date{0}; // initial grace period
	dcon::national_identity_id crisis_liberation_tag;
	dcon::state_definition_id crisis_colony;

	//
	// Messages
	//

	std::array<diplomatic_message::message, 128> pending_messages = {};

	//
	// Event data
	//

	std::vector<event::pending_human_n_event> pending_n_event;
	std::vector<event::pending_human_f_n_event> pending_f_n_event;
	std::vector<event::pending_human_p_event> pending_p_event;
	std::vector<event::pending_human_f_p_event> pending_f_p_event;

	std::vector<int32_t> unit_names_indices; // indices for the names
	std::vector<char> unit_names;
	// a second text buffer, this time for just the unit names
	// why a second text buffer? Partly because unit names don't need the extra redirection possibilities of
	// ordinary game text, partly because I envision the possibility that we may stick dynamic names into
	// this We also may push this into the save game if we handle unit renaming using this

	ui::definitions ui_defs; // definitions for graphics and ui

	std::vector<uint8_t> flag_type_map;   // flag_type remapper for saving space while also allowing
	                                      // mods to add flags not present in vanilla
	std::vector<culture::flag_type> flag_types; // List of unique flag types

	//
	// persistent user settings
	//

	user_settings_s user_settings;

	//
	// current program / ui state
	//

	game_mode_type mode = game_mode_type::pick_nation;
	network_mode_type network_mode = network_mode_type::single_player;
	dcon::nation_id local_player_nation;
	sys::date current_date = sys::date{0};
	sys::date ui_date = sys::date{0};
	uint32_t game_seed = 0; // do *not* alter this value, ever
	float inflation = 1.0f;
	player_data player_data_cache;
	std::vector<dcon::army_id> selected_armies;
	std::vector<dcon::navy_id> selected_navies;

	simple_fs::file_system common_fs;                                // file system for looking up graphics assets, etc
	std::unique_ptr<window::window_data_impl> win_ptr = nullptr;     // platform-dependent window information
	std::unique_ptr<sound::sound_impl> sound_ptr = nullptr;          // platform-dependent sound information
	ui::state ui_state;                                              // transient information for the state of the ui
	text::font_manager font_collection;

	// synchronization data (between main update logic and ui thread)
	std::atomic<bool> game_state_updated = false;                    // game state -> ui signal
	std::atomic<bool> quit_signaled = false;                         // ui -> game state signal
	std::atomic<int32_t> actual_game_speed = 0;                      // ui -> game state message
	rigtorp::SPSCQueue<command::payload> incoming_commands;          // ui or network -> local gamestate
	std::atomic<bool> ui_pause = false;                              // force pause by an important message being open

	// synchronization: notifications from the gamestate to ui
	rigtorp::SPSCQueue<event::pending_human_n_event> new_n_event;
	rigtorp::SPSCQueue<event::pending_human_f_n_event> new_f_n_event;
	rigtorp::SPSCQueue<event::pending_human_p_event> new_p_event;
	rigtorp::SPSCQueue<event::pending_human_f_p_event> new_f_p_event;
	rigtorp::SPSCQueue<diplomatic_message::message> new_requests;
	rigtorp::SPSCQueue<notification::message> new_messages;
	rigtorp::SPSCQueue<military::naval_battle_report> naval_battle_reports;
	rigtorp::SPSCQueue<military::land_battle_report> land_battle_reports;

	// internal game timer / update logic
	std::chrono::time_point<std::chrono::steady_clock> last_update = std::chrono::steady_clock::now();
	bool internally_paused = false; // should NOT be set from the ui context (but may be read)

	// common data for the window
	int32_t x_size = 0;
	int32_t y_size = 0;
	bool drag_selecting = false;
	int32_t mouse_x_position = 0;
	int32_t mouse_y_position = 0;
	bool is_dragging = false;
	int32_t x_drag_start = 0;
	int32_t y_drag_start = 0;
	std::chrono::time_point<std::chrono::steady_clock> tooltip_timer = std::chrono::steady_clock::now();

	// map data
	map::map_state map_state;
	dcon::gfx_object_id bg_gfx_id{};

	// graphics data
	ogl::data open_gl;

	// the following functions will be invoked by the window subsystem

	void on_create(); // called once after the window is created and opengl is ready
	void on_rbutton_down(int32_t x, int32_t y, key_modifiers mod);
	void on_mbutton_down(int32_t x, int32_t y, key_modifiers mod);
	void on_lbutton_down(int32_t x, int32_t y, key_modifiers mod);
	void on_rbutton_up(int32_t x, int32_t y, key_modifiers mod);
	void on_mbutton_up(int32_t x, int32_t y, key_modifiers mod);
	void on_lbutton_up(int32_t x, int32_t y, key_modifiers mod);
	void on_mouse_move(int32_t x, int32_t y, key_modifiers mod);
	void on_mouse_drag(int32_t x, int32_t y, key_modifiers mod); // called when the left button is held down
	void on_drag_finished(int32_t x, int32_t y, key_modifiers mod); // called when the left button is released after one or more drag events
	void on_resize(int32_t x, int32_t y, window::window_state win_state);
	void on_mouse_wheel(int32_t x, int32_t y, key_modifiers mod, float amount); // an amount of 1.0 is one "click" of the wheel
	void on_key_down(virtual_key keycode, key_modifiers mod);
	void on_key_up(virtual_key keycode, key_modifiers mod);
	void on_text(char c); // c is a win1250 codepage value
	void render(); // called to render the frame may (and should) delay returning until the frame is rendered, including waiting
	               // for vsync

	void single_game_tick();
	// this function runs the internal logic of the game. It will return *only* after a quit notification is sent to it
	void game_loop();
	sys::checksum_key get_save_checksum();
	sys::checksum_key get_scenario_checksum();

	// the following function are for interacting with the string pool

	std::string_view to_string_view(dcon::text_key tag) const; // takes a stored tag and give you the text

	dcon::text_key add_to_pool(std::string const& text); // returns the newly added text
	dcon::text_key add_to_pool(std::string_view text);
	dcon::text_key add_to_pool_lowercase(std::string const& text); // these functions are as above, but force the text into lower case
	dcon::text_key add_to_pool_lowercase(std::string_view text);

	// searches the string pool for any existing string, appends if it is new
	// use this function sparingly; i.e. only when you think it is likely that
	// the text has already been added. Searching *all* the text may not be cheap
	dcon::text_key add_unique_to_pool(std::string const& text);

	dcon::unit_name_id add_unit_name(std::string_view text);       // returns the newly added text
	std::string_view to_string_view(dcon::unit_name_id tag) const; // takes a stored tag and give you the text

	dcon::trigger_key commit_trigger_data(std::vector<uint16_t> data);
	dcon::effect_key commit_effect_data(std::vector<uint16_t> data);

	state() : key_to_text_sequence(0, text::vector_backed_hash(text_data), text::vector_backed_eq(text_data)), incoming_commands(1024), new_n_event(1024), new_f_n_event(1024), new_p_event(1024), new_f_p_event(1024), new_requests(256), new_messages(1024), naval_battle_reports(256), land_battle_reports(256) { }

	~state();

	void save_user_settings() const;
	void load_user_settings();
	void update_ui_scale(float new_scale);

	void load_scenario_data(parsers::error_handler& err);   // loads all scenario files other than map data
	void fill_unsaved_data();    // reconstructs derived values that are not directly saved after a save has been loaded
	void preload(); // clears data that will be later reconstructed from saved values

	void console_log(ui::element_base* base, std::string message, bool open_console = true);

	void open_diplomacy(dcon::nation_id target); // Open the diplomacy window with target selected

	bool is_selected(dcon::army_id a) {
		return std::find(selected_armies.begin(), selected_armies.end(), a) != selected_armies.end();
	}
	bool is_selected(dcon::navy_id a) {
		return std::find(selected_navies.begin(), selected_navies.end(), a) != selected_navies.end();
	}
	void select(dcon::army_id a) {
		if(!is_selected(a)) {
			selected_armies.push_back(a);
			game_state_updated.store(true, std::memory_order_release);
		}
	}
	void select(dcon::navy_id a) {
		if(!is_selected(a)) {
			selected_navies.push_back(a);
			game_state_updated.store(true, std::memory_order_release);
		}
	}
	void deselect(dcon::army_id a) {
		for(size_t i = selected_armies.size(); i-- > 0;) {
			if(selected_armies[i] == a) {
				selected_armies[i] = selected_armies.back();
				selected_armies.pop_back();
				game_state_updated.store(true, std::memory_order_release);
				return;
			}
		}
	}
	void deselect(dcon::navy_id a) {
		for(size_t i = selected_navies.size(); i-- > 0;) {
			if(selected_navies[i] == a) {
				selected_navies[i] = selected_navies.back();
				selected_navies.pop_back();
				game_state_updated.store(true, std::memory_order_release);
				return;
			}
		}
	}
};
} // namespace sys
