#pragma once

#include <memory>
#include <stdint.h>
#include <atomic>
#include <chrono>

#include "window.hpp"
#include "constants.hpp"
#include "dcon_generated.hpp"
#include "gui_graphics.hpp"
#include "game_scene.hpp"
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
#include "network.hpp"
#include "fif.hpp"

// this header will eventually contain the highest-level objects
// that represent the overall state of the program
// it will also include the game state itself eventually as a member

namespace sys {

enum class gui_modes : uint8_t { faithful = 0, nouveau = 1, dummycabooseval = 2 };
enum class projection_mode : uint8_t { globe_ortho = 0, flat = 1, globe_perpect = 2, num_of_modes = 3};

struct user_settings_s {
	float ui_scale = 1.0f;
	float master_volume = 0.5f;
	float music_volume = 1.0f;
	float effects_volume = 1.0f;
	float interface_volume = 1.0f;
	bool prefer_fullscreen = false;
	projection_mode map_is_globe = projection_mode::globe_ortho;
	autosave_frequency autosaves = autosave_frequency::yearly;
	bool bind_tooltip_mouse = true;
	bool use_classic_fonts = false;
	bool left_mouse_click_hold_and_release = false;
	bool outliner_views[14] = {true, true, true, true, true, true, true, true, true, true, true, true, true, true};
	uint8_t self_message_settings[int32_t(sys::message_setting_type::count)] = {
		message_response::standard_pause,//revolt = 0,
		message_response::standard_pause,//war_on_nation = 1,
		message_response::standard_log,//war_by_nation = 2,
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
		message_response::standard_log,//peace_accepted_by_nation = 16,
		message_response::standard_log,//peace_rejected_by_nation = 17,
		message_response::standard_popup,//mobilization_start = 18,
		message_response::standard_popup,//mobilization_end = 19,
		message_response::standard_log,//factory_complete = 20,
		message_response::standard_log,//rr_complete = 21,
		message_response::standard_log,//fort_complete = 22,
		message_response::standard_log,//naval_base_complete = 23,
		message_response::standard_log,//province_event = 24,
		message_response::standard_log,//national_event = 25,
		message_response::standard_log,//major_event = 26,
		message_response::standard_popup,//invention = 27,
		message_response::standard_popup,//tech = 28,
		message_response::standard_log,//leader_dies = 29,
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
		message_response::standard_log,//army_built = 98,
		message_response::standard_log,//navy_built = 99,
		message_response::standard_popup,//bankruptcy = 100,
		message_response::standard_popup,//entered_automatic_alliance = 101,
		message_response::standard_log,//chat_message = 102,
	};
	uint8_t interesting_message_settings[int32_t(sys::message_setting_type::count)] = {
		message_response::standard_log,//revolt = 0,
		message_response::standard_log,//war_on_nation = 1,
		message_response::standard_log,//war_by_nation = 2,
		message_response::standard_log,//wargoal_added = 3,
		message_response::ignore,//siegeover_by_nation = 4,
		message_response::ignore,//siegeover_on_nation = 5,
		message_response::standard_log,//colony_finished = 6,
		message_response::ignore,//reform_gained = 7,
		message_response::ignore,//reform_lost = 8,
		message_response::ignore,//ruling_party_change = 9,
		message_response::standard_log,//upperhouse = 10,
		message_response::standard_log,//electionstart = 11,
		message_response::standard_log,//electiondone = 12,
		message_response::standard_log,//breakcountry = 13,
		message_response::ignore,//peace_accepted_from_nation = 14,
		message_response::standard_log,//peace_rejected_from_nation = 15,
		message_response::ignore,//peace_accepted_by_nation = 16,
		message_response::ignore,//peace_rejected_by_nation = 17,
		message_response::standard_popup,//mobilization_start = 18,
		message_response::standard_popup,//mobilization_end = 19,
		message_response::ignore,//factory_complete = 20,
		message_response::ignore,//rr_complete = 21,
		message_response::ignore,//fort_complete = 22,
		message_response::ignore,//naval_base_complete = 23,
		message_response::standard_log,//province_event = 24,
		message_response::standard_log,//national_event = 25,
		message_response::standard_popup,//major_event = 26,
		message_response::standard_log,//invention = 27,
		message_response::standard_log,//tech = 28,
		message_response::ignore,//leader_dies = 29,
		message_response::ignore,//land_combat_starts_on_nation = 30,
		message_response::ignore,//naval_combat_starts_on_nation = 31,
		message_response::ignore,//land_combat_starts_by_nation = 32,
		message_response::ignore,//naval_combat_starts_by_nation = 33,
		message_response::ignore,//movement_finishes = 34,
		message_response::ignore,//decision = 35,
		message_response::standard_log,//lose_great_power = 36,
		message_response::standard_log,//become_great_power = 37,
		message_response::standard_log,//war_subsidies_start_by_nation = 38,
		message_response::standard_log,//war_subsidies_start_on_nation = 39,
		message_response::standard_log,//war_subsidies_end_by_nation = 40,
		message_response::standard_log,//war_subsidies_end_on_nation = 41,
		message_response::standard_log,//reparations_start_by_nation = 42,
		message_response::standard_log,//reparations_start_on_nation = 43,
		message_response::standard_log,//reparations_end_by_nation = 44,
		message_response::standard_log,//reparations_end_on_nation = 45,
		message_response::ignore,//mil_access_start_by_nation = 46,
		message_response::ignore,//mil_access_start_on_nation = 47,
		message_response::ignore,//mil_access_end_by_nation = 48,
		message_response::ignore,//mil_access_end_on_nation = 49,
		message_response::ignore,//mil_access_declined_by_nation = 50,
		message_response::ignore,//mil_access_declined_on_nation = 51,
		message_response::standard_log,//alliance_starts = 52,
		message_response::standard_log,//alliance_ends = 53,
		message_response::ignore,//alliance_declined_by_nation = 54,
		message_response::ignore,//alliance_declined_on_nation = 55,
		message_response::ignore,//ally_called_accepted_by_nation = 56,
		message_response::ignore,//ally_called_declined_by_nation = 57,
		message_response::ignore,//discredit_by_nation = 58,
		message_response::standard_log,//ban_by_nation = 59,
		message_response::standard_log,//expell_by_nation = 60,
		message_response::standard_log,//discredit_on_nation = 61,
		message_response::ignore,//ban_on_nation = 62,
		message_response::ignore,//expell_on_nation = 63,
		message_response::ignore,//increase_opinion = 64,
		message_response::standard_log,//decrease_opinion_by_nation = 65,
		message_response::standard_log,//decrease_opinion_on_nation = 66,
		message_response::standard_log,//rem_sphere_by_nation = 67,
		message_response::standard_log,//rem_sphere_on_nation = 68,
		message_response::standard_log,//removed_from_sphere = 69,
		message_response::standard_log,//add_sphere = 70,
		message_response::standard_log,//added_to_sphere = 71,
		message_response::standard_log,//increase_relation_by_nation = 72,
		message_response::standard_log,//increase_relation_on_nation = 73,
		message_response::standard_log,//decrease_relation_by_nation = 74,
		message_response::standard_log,//decrease_relation_on_nation = 75,
		message_response::standard_log,//join_war_by_nation = 76,
		message_response::standard_log,//join_war_on_nation = 77,
		message_response::ignore,//gw_unlocked = 78,
		message_response::ignore,//war_becomes_great = 79,
		message_response::ignore,//cb_detected_on_nation = 80,
		message_response::standard_popup,//cb_detected_by_nation = 81,
		message_response::standard_log,//crisis_join_offer_accepted_by_nation = 82,
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
		message_response::standard_log,//war_join_by = 93,
		message_response::ignore,//war_join_on = 94,
		message_response::ignore,//cb_fab_finished = 95,
		message_response::ignore,//cb_fab_cancelled = 96,
		message_response::standard_popup,//crisis_voluntary_join_on = 97,
		message_response::ignore,//army_built = 98,
		message_response::ignore,//navy_built = 99,
		message_response::standard_popup,//bankruptcy = 100,
		message_response::ignore,//entered_automatic_alliance = 101,
		message_response::standard_log,//chat_message = 102,
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
		message_response::standard_popup,//mobilization_start = 18,
		message_response::standard_popup,//mobilization_end = 19,
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
		message_response::standard_log,//cb_detected_by_nation = 81,
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
		message_response::ignore,//army_built = 98,
		message_response::ignore,//navy_built = 99,
		message_response::standard_popup,//bankruptcy = 100,
		message_response::ignore,//entered_automatic_alliance = 101,
		message_response::standard_log,//chat_message = 102,
	};
	bool fow_enabled = false;
	map_label_mode map_label = map_label_mode::quadratic;
	uint8_t antialias_level = 4;
	float gaussianblur_level = 1.f;
	float gamma = 1.f;
	bool railroads_enabled = true;
	bool rivers_enabled = true;
	map_zoom_mode zoom_mode = map_zoom_mode::panning;
	map_vassal_color_mode vassal_color = map_vassal_color_mode::inherit;
	bool render_models = false;
	bool mouse_edge_scrolling = false;
	bool black_map_font = true;
	bool spoilers = true;
	float zoom_speed = 20.f;
	bool mute_on_focus_lost = true;
	bool diplomatic_message_popup = false;
	bool wasd_for_map_movement = false;
	bool notify_rebels_defeat = true;
	sys::color_blind_mode color_blind_mode = sys::color_blind_mode::none;
	uint32_t UNUSED_UINT32_T = 0;
	char locale[16] = "en-US";
};

struct global_scenario_data_s { // this struct holds miscellaneous global properties of the scenario
};

struct cheat_data_s {
	bool always_allow_wargoals = false;
	bool always_allow_reforms = false;
	bool always_accept_deals = false;
	bool show_province_id_tooltip = false;
	bool instant_army = false;
	bool instant_industry = false;
	std::vector<dcon::nation_id> instant_research_nations;
	bool daily_oos_check = false;
	bool province_names = false;

	bool ecodump = false;

	std::string national_economy_dump_buffer;
	std::string prices_dump_buffer;
	std::string demand_dump_buffer;
	std::string supply_dump_buffer;

	bool instant_navy = false;
	bool always_allow_decisions = false;
	bool always_potential_decisions = false;
};

struct crisis_member_def {
	dcon::nation_id id;

	bool supports_attacker = false;
	bool merely_interested = false;
};
static_assert(sizeof(crisis_member_def) ==
	sizeof(crisis_member_def::id)
	+ sizeof(crisis_member_def::supports_attacker)
	+ sizeof(crisis_member_def::merely_interested));

enum class crisis_type : uint32_t { none = 0, claim = 1, liberation = 2, colonial = 3, influence = 4, wargoals = 5 };
enum class crisis_state : uint32_t { inactive = 0, finding_attacker = 1, finding_defender = 2, heating_up = 3 };

struct great_nation {
	sys::date last_greatness = sys::date(0);
	dcon::nation_id nation;

	great_nation(sys::date last_greatness, dcon::nation_id nation) : last_greatness(last_greatness), nation(nation) { }
	great_nation() = default;
};
static_assert(sizeof(great_nation) ==
	sizeof(great_nation::last_greatness)
	+ sizeof(great_nation::nation));

// used by state selector visually
struct state_selection_data {
	bool single_state_select = false;
	std::vector<dcon::state_definition_id> selectable_states;
	std::function<void(sys::state&, dcon::state_definition_id)> on_select;
	std::function<void(sys::state&)> on_cancel;
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

	std::vector<char> key_data;
	std::vector<char> locale_text_data;
	ankerl::unordered_dense::set<dcon::text_key, text::vector_backed_ci_hash, text::vector_backed_ci_eq> untrans_key_to_text_sequence;
	ankerl::unordered_dense::map<dcon::text_key, uint32_t, text::vector_backed_ci_hash, text::vector_backed_ci_eq> locale_key_to_text_sequence;

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
	int32_t autosave_counter = 0; // which autosave file is next
	sys::checksum_key scenario_checksum;// for checksum for savefiles
	sys::checksum_key session_host_checksum;// for checking that the client can join a session
	native_string loaded_scenario_file;
	native_string loaded_save_file;

#ifdef USE_LLVM
	std::unique_ptr<fif::environment> jit_environment;
#endif

	//
	// Crisis data
	//

	dcon::nation_id crisis_attacker;
	dcon::nation_id crisis_defender;
	// For legacy reasons (compatibility with mods) save state_instance_id separately. It's only filled for Liberation crises.
	dcon::state_instance_id crisis_state_instance;
	// For legacy triggers save crisis type separately
	crisis_type current_crisis = crisis_type::none;
	std::vector<crisis_member_def> crisis_participants;
	std::vector<sys::full_wg> crisis_attacker_wargoals;
	std::vector<sys::full_wg> crisis_defender_wargoals;
	float crisis_temperature = 0;
	dcon::nation_id primary_crisis_attacker;
	dcon::nation_id primary_crisis_defender;
	crisis_state current_crisis_state = crisis_state::inactive;
	uint32_t crisis_last_checked_gp = 0;
	dcon::war_id crisis_war;
	sys::date last_crisis_end_date{0}; // initial grace period

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

	std::vector<event::pending_human_n_event> future_n_event;
	std::vector<event::pending_human_p_event> future_p_event;

	std::vector<int32_t> unit_names_indices; // indices for the names
	std::vector<char> unit_names;
	// a second text buffer, this time for just the unit names
	// why a second text buffer? Partly because unit names don't need the extra redirection possibilities of
	// ordinary game text, partly because I envision the possibility that we may stick dynamic names into
	// this We also may push this into the save game if we handle unit renaming using this

	ui::definitions ui_defs; // definitions for graphics and ui

	std::vector<uint8_t> flag_type_map;   // flag_type remapper for saving space while also allowing mods to add flags not present in vanilla
	std::vector<culture::flag_type> flag_types; // List of unique flag types

	//
	// persistent user settings
	//

	user_settings_s user_settings;

	//
	// current program / ui state
	//

	network_mode_type network_mode = network_mode_type::single_player;
	dcon::nation_id local_player_nation;
	sys::date current_date = sys::date{0};
	sys::date ui_date = sys::date{0};
	uint32_t game_seed = 0; // do *not* alter this value, ever
	float inflation = 1.0f;
	player_data player_data_cache;
	std::vector<dcon::army_id> selected_armies;
	std::vector<dcon::regiment_id> selected_regiments; // selected regiments inside the army

	std::vector<dcon::navy_id> selected_navies;
	std::vector<dcon::ship_id> selected_ships; // selected ships inside the navy

	dcon::commodity_id selected_trade_good;
	std::mutex ugly_ui_game_interaction_hack;

	//control groups
	std::array<std::vector<dcon::army_id>, 10> ctrl_armies;
	std::array<std::vector<dcon::navy_id>, 10> ctrl_navies;

	//army group
	dcon::automated_army_group_id selected_army_group{};

	army_group_order selected_army_group_order = army_group_order::none;

	//current ui
	game_scene::scene_properties current_scene;

	std::optional<state_selection_data> state_selection;
	map_mode::mode stored_map_mode = map_mode::mode::political;

	simple_fs::file_system common_fs;                                // file system for looking up graphics assets, etc
	std::unique_ptr<window::window_data_impl> win_ptr = nullptr;     // platform-dependent window information
	std::unique_ptr<sound::sound_impl> sound_ptr = nullptr;          // platform-dependent sound information
	ui::state ui_state;                                              // transient information for the state of the ui
	text::font_manager font_collection;

	// synchronization data (between main update logic and ui thread)
	std::atomic<bool> game_state_updated = false;                    // game state -> ui signal
	std::atomic<bool> province_ownership_changed = true;                    // game state -> ui signal
	std::atomic<bool> save_list_updated = false;                     // game state -> ui signal
	std::atomic<bool> quit_signaled = false;                         // ui -> game state signal
	std::atomic<int32_t> actual_game_speed = 0;                      // ui -> game state message
	rigtorp::SPSCQueue<command::payload> incoming_commands;          // ui or network -> local gamestate
	std::atomic<bool> ui_pause = false;                              // force pause by an important message being open
	std::atomic<bool> railroad_built = true; // game state -> map
	std::atomic<bool> update_trade_flow = true;

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
	
	// graphics data
	ogl::data open_gl;

#ifdef DIRECTX_11
	directx::data directx;
#endif

	// cheat data
	cheat_data_s cheat_data;

	// network data
	network::network_state network_state;

	// console interpreter
	std::mutex lock_console_strings;
	std::string console_command_pending;
	std::string console_command_result;
	std::string console_command_error;
	std::unique_ptr<fif::environment> fif_environment;
	int32_t type_text_key = -1;
	int32_t type_localized_key = -1;

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
	void on_text(char32_t c); // c is a win1250 codepage value
	void render(); // called to render the frame may (and should) delay returning until the frame is rendered, including waiting
	               // for vsync

	void single_game_tick();
	// this function runs the internal logic of the game. It will return *only* after a quit notification is sent to it
	void game_loop();
	sys::checksum_key get_save_checksum();
	void debug_save_oos_dump();
	void debug_scenario_oos_dump();

	void start_state_selection(state_selection_data& data);
	void state_select(dcon::state_definition_id sdef);

	// the following function are for interacting with the string pool

	std::string_view to_string_view(dcon::text_key tag) const;
	std::string_view locale_string_view(uint32_t tag) const;
	bool key_is_localized(dcon::text_key tag) const;
	bool key_is_localized(std::string_view key) const;
	dcon::text_key lookup_key(std::string_view text) const;

	void reset_locale_pool();
	void load_locale_strings(std::string_view locale_name);

	dcon::text_key add_key_win1252(std::string const& text);
	dcon::text_key add_key_win1252(std::string_view text);
	dcon::text_key add_key_utf8(std::string const& text);
	dcon::text_key add_key_utf8(std::string_view text);
	uint32_t add_locale_data_win1252(std::string const& text);
	uint32_t add_locale_data_win1252(std::string_view text);
	uint32_t add_locale_data_utf8(std::string const& text);
	uint32_t add_locale_data_utf8(std::string_view text);

	dcon::unit_name_id add_unit_name(std::string_view text);       // returns the newly added text
	std::string_view to_string_view(dcon::unit_name_id tag) const; // takes a stored tag and give you the text

	dcon::trigger_key commit_trigger_data(std::vector<uint16_t> data);
	dcon::effect_key commit_effect_data(std::vector<uint16_t> data);

	state() : untrans_key_to_text_sequence(0, text::vector_backed_ci_hash(key_data), text::vector_backed_ci_eq(key_data)), locale_key_to_text_sequence(0, text::vector_backed_ci_hash(key_data), text::vector_backed_ci_eq(key_data)), current_scene(game_scene::nation_picker()), incoming_commands(1024), new_n_event(1024), new_f_n_event(1024), new_p_event(1024), new_f_p_event(1024), new_requests(256), new_messages(2048), naval_battle_reports(256), land_battle_reports(256) {

		key_data.push_back(0);
	}

	~state() = default;

	void save_user_settings() const;
	void load_user_settings();
	void update_ui_scale(float new_scale);

	void load_scenario_data(parsers::error_handler& err, sys::year_month_day bookmark_date);   // loads all scenario files other than map data
	void fill_unsaved_data();    // reconstructs derived values that are not directly saved after a save has been loaded
	void on_scenario_load(); // called when the scenario file is loaded (not when saves are loaded)
	void preload(); // clears data that will be later reconstructed from saved values

	void console_log(std::string_view message);
	void log_player_nations();

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

	void new_army_group(dcon::province_id hq);
	void delete_army_group(dcon::automated_army_group_id group);
	void toggle_designated_port(dcon::automated_army_group_id group, dcon::province_id position);
	void toggle_defensive_position(dcon::automated_army_group_id group, dcon::province_id position);
	void toggle_enforce_control_position(dcon::automated_army_group_id group, dcon::province_id position);
	void update_armies_and_fleets(dcon::automated_army_group_id group);
	void regiment_reset_order(dcon::regiment_automation_data_id regiment);
	bool army_group_recalculate_distribution(dcon::automated_army_group_id group, std::vector<float>& regiments_distribution);
	void army_group_update_tasks(dcon::automated_army_group_id group);
	dcon::province_id get_port_for_landing(dcon::automated_army_group_id group, dcon::province_id target);
	void army_group_distribute_tasks(dcon::automated_army_group_id group);
	float army_group_available_supply(dcon::automated_army_group_id group, dcon::province_id province);
	dcon::province_id find_available_ferry_origin(dcon::automated_army_group_id group, dcon::regiment_automation_data_id regiment);
	bool move_to_available_port(dcon::automated_army_group_id group, dcon::regiment_automation_data_id regiment);
	void remove_navy_from_army_group(dcon::automated_army_group_id selected_group, dcon::navy_id navy_to_delete);
	void army_group_update_regiment_status(dcon::automated_army_group_id group);
	void army_group_add_regiment(dcon::automated_army_group_id group, dcon::regiment_id id);
	void remove_regiment_from_army_group(dcon::automated_army_group_id selected_group, dcon::regiment_id selected_regiment);
	void remove_regiment_from_all_army_groups(dcon::regiment_id regiment_to_delete);
	void remove_army_army_group_clean(dcon::automated_army_group_id group, dcon::army_id army_to_delete);
	void add_army_to_army_group(dcon::automated_army_group_id selected_group, dcon::army_id selected_army);
	void add_navy_to_army_group(dcon::automated_army_group_id selected_group, dcon::navy_id selected_navy);
	void smart_select_army_group(dcon::automated_army_group_id selected_group);
	void select_army_group(dcon::automated_army_group_id selected_group);
	void deselect_army_group();
	dcon::regiment_automation_data_id fill_province_up_to_supply_limit(
		dcon::automated_army_group_id group_id,
		dcon::province_id target,
		std::vector<float>& regiments_distribution,
		float overestimate_supply_limit,
		bool ignore_enemy_regiments_in_supply_calculations
	);
	dcon::regiment_automation_data_id fill_province(
		dcon::automated_army_group_id group_id,
		dcon::province_id target,
		std::vector<float>& regiments_expectation_ideal
	);

	void fill_vector_of_connected_provinces(
		dcon::province_id p1,
		bool is_land,
		std::vector<dcon::province_id> & provinces
	);

	void build_up_to_template_land(
		macro_builder_template const& target_template,
		dcon::province_id target_province,
		std::vector<dcon::province_id>& available_provinces,
		std::array<uint8_t, sys::macro_builder_template::max_types>& current_distribution
	);
};

constexpr inline size_t num_selected_units = 1000;

void selected_regiments_add(sys::state& state, dcon::regiment_id reg);
void selected_regiments_clear(sys::state& state);

void selected_ships_add(sys::state& state, dcon::ship_id sh);
void selected_ships_clear(sys::state& state);
} // namespace sys
