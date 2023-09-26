#pragma once
#include <stdint.h>

namespace sys {
enum class virtual_key : uint8_t {
	NONE = 0x00,
	LBUTTON = 0x01,
	RBUTTON = 0x02,
	CANCEL = 0x03,
	MBUTTON = 0x04,
	XBUTTON_1 = 0x05,
	XBUTTON_2 = 0x06,
	BACK = 0x08,
	TAB = 0x09,
	CLEAR = 0x0C,
	RETURN = 0x0D,
	SHIFT = 0x10,
	CONTROL = 0x11,
	MENU = 0x12,
	PAUSE = 0x13,
	CAPITAL = 0x14,
	KANA = 0x15,
	JUNJA = 0x17,
	FINAL = 0x18,
	KANJI = 0x19,
	ESCAPE = 0x1B,
	CONVERT = 0x1C,
	NONCONVERT = 0x1D,
	ACCEPT = 0x1E,
	MODECHANGE = 0x1F,
	SPACE = 0x20,
	PRIOR = 0x21,
	NEXT = 0x22,
	END = 0x23,
	HOME = 0x24,
	LEFT = 0x25,
	UP = 0x26,
	RIGHT = 0x27,
	DOWN = 0x28,
	SELECT = 0x29,
	PRINT = 0x2A,
	EXECUTE = 0x2B,
	SNAPSHOT = 0x2C,
	INSERT = 0x2D,
	DELETE_KEY = 0x2E,
	HELP = 0x2F,
	NUM_0 = 0x30,
	NUM_1 = 0x31,
	NUM_2 = 0x32,
	NUM_3 = 0x33,
	NUM_4 = 0x34,
	NUM_5 = 0x35,
	NUM_6 = 0x36,
	NUM_7 = 0x37,
	NUM_8 = 0x38,
	NUM_9 = 0x39,
	A = 0x41,
	B = 0x42,
	C = 0x43,
	D = 0x44,
	E = 0x45,
	F = 0x46,
	G = 0x47,
	H = 0x48,
	I = 0x49,
	J = 0x4A,
	K = 0x4B,
	L = 0x4C,
	M = 0x4D,
	N = 0x4E,
	O = 0x4F,
	P = 0x50,
	Q = 0x51,
	R = 0x52,
	S = 0x53,
	T = 0x54,
	U = 0x55,
	V = 0x56,
	W = 0x57,
	X = 0x58,
	Y = 0x59,
	Z = 0x5A,
	LWIN = 0x5B,
	RWIN = 0x5C,
	APPS = 0x5D,
	SLEEP = 0x5F,
	NUMPAD0 = 0x60,
	NUMPAD1 = 0x61,
	NUMPAD2 = 0x62,
	NUMPAD3 = 0x63,
	NUMPAD4 = 0x64,
	NUMPAD5 = 0x65,
	NUMPAD6 = 0x66,
	NUMPAD7 = 0x67,
	NUMPAD8 = 0x68,
	NUMPAD9 = 0x69,
	MULTIPLY = 0x6A,
	ADD = 0x6B,
	SEPARATOR = 0x6C,
	SUBTRACT = 0x6D,
	DECIMAL = 0x6E,
	DIVIDE = 0x6F,
	F1 = 0x70,
	F2 = 0x71,
	F3 = 0x72,
	F4 = 0x73,
	F5 = 0x74,
	F6 = 0x75,
	F7 = 0x76,
	F8 = 0x77,
	F9 = 0x78,
	F10 = 0x79,
	F11 = 0x7A,
	F12 = 0x7B,
	F13 = 0x7C,
	F14 = 0x7D,
	F15 = 0x7E,
	F16 = 0x7F,
	F17 = 0x80,
	F18 = 0x81,
	F19 = 0x82,
	F20 = 0x83,
	F21 = 0x84,
	F22 = 0x85,
	F23 = 0x86,
	F24 = 0x87,
	NAVIGATION_VIEW = 0x88,
	NAVIGATION_MENU = 0x89,
	NAVIGATION_UP = 0x8A,
	NAVIGATION_DOWN = 0x8B,
	NAVIGATION_LEFT = 0x8C,
	NAVIGATION_RIGHT = 0x8D,
	NAVIGATION_ACCEPT = 0x8E,
	NAVIGATION_CANCEL = 0x8F,
	NUMLOCK = 0x90,
	SCROLL = 0x91,
	OEM_NEC_EQUAL = 0x92,
	LSHIFT = 0xA0,
	RSHIFT = 0xA1,
	LCONTROL = 0xA2,
	RCONTROL = 0xA3,
	LMENU = 0xA4,
	RMENU = 0xA5,
	SEMICOLON = 0xBA,
	PLUS = 0xBB,
	COMMA = 0xBC,
	MINUS = 0xBD,
	PERIOD = 0xBE,
	FORWARD_SLASH = 0xBF,
	TILDA = 0xC0,
	OPEN_BRACKET = 0xDB,
	BACK_SLASH = 0xDC,
	CLOSED_BRACKET = 0xDD,
	QUOTE = 0xDE
};
enum class key_modifiers : uint8_t {
	modifiers_none = 0x0,
	modifiers_alt = 0x4,
	modifiers_ctrl = 0x2,
	modifiers_shift = 0x1,
	modifiers_ctrl_shift = 0x3,
	modifiers_ctrl_alt = 0x6,
	modifiers_alt_shift = 0x5,
	modifiers_all = 0x7
};
constexpr inline float ui_scales[] = {0.75f, 1.0f, 1.25f, 1.5f, 1.75f, 2.0f, 2.5f, 3.0f};
constexpr inline uint32_t ui_scales_count = 8;

enum class autosave_frequency : uint8_t {
	none = 0,
	yearly = 1,
	monthly = 2,
	daily = 3,
};

enum class commodity_group : uint8_t { military_goods = 0, raw_material_goods, industrial_goods, consumer_goods, count };

enum class message_setting_type : uint8_t {
	revolt = 0, // added
	war_on_nation = 1, // added
	war_by_nation = 2, // added
	wargoal_added = 3, // added
	siegeover_by_nation = 4, // added
	siegeover_on_nation = 5, // added
	colony_finished = 6,
	reform_gained = 7,
	reform_lost = 8,
	ruling_party_change = 9,
	upperhouse = 10, // added
	electionstart = 11, // added
	electiondone = 12, // added
	breakcountry = 13, // added
	peace_accepted_from_nation = 14, // added
	peace_rejected_from_nation = 15, // added
	peace_accepted_by_nation = 16, // added
	peace_rejected_by_nation = 17, // added
	mobilization_start = 18,
	mobilization_end = 19,
	factory_complete = 20,
	rr_complete = 21,
	fort_complete = 22,
	naval_base_complete = 23,
	province_event = 24,
	national_event = 25,
	major_event = 26,
	invention = 27, // added
	tech = 28, // added
	leader_dies = 29, // added
	land_combat_starts_on_nation = 30,
	naval_combat_starts_on_nation = 31,
	land_combat_starts_by_nation = 32,
	naval_combat_starts_by_nation = 33,
	movement_finishes = 34,
	decision = 35, // added
	lose_great_power = 36, // added
	become_great_power = 37, // added
	war_subsidies_start_by_nation = 38, // added
	war_subsidies_start_on_nation = 39, // added
	war_subsidies_end_by_nation = 40, // added
	war_subsidies_end_on_nation = 41, // added
	reparations_start_by_nation = 42,
	reparations_start_on_nation = 43,
	reparations_end_by_nation = 44,
	reparations_end_on_nation = 45,
	mil_access_start_by_nation = 46, // added
	mil_access_start_on_nation = 47, // added
	mil_access_end_by_nation = 48, // added
	mil_access_end_on_nation = 49, // added
	mil_access_declined_by_nation = 50, // added
	mil_access_declined_on_nation = 51, // added
	alliance_starts = 52, // added
	alliance_ends = 53, // added
	alliance_declined_by_nation = 54, // added
	alliance_declined_on_nation = 55, // added
	ally_called_accepted_by_nation = 56, // added
	ally_called_declined_by_nation = 57, // added
	discredit_by_nation = 58, // added
	ban_by_nation = 59, // added
	expell_by_nation = 60, // added
	discredit_on_nation = 61, // added
	ban_on_nation = 62, // added
	expell_on_nation = 63, // added
	increase_opinion = 64, // added
	decrease_opinion_by_nation = 65, // added
	decrease_opinion_on_nation = 66, // added
	rem_sphere_by_nation = 67, // added
	rem_sphere_on_nation = 68, // added
	removed_from_sphere = 69, // added
	add_sphere = 70, // added
	added_to_sphere = 71, // added
	increase_relation_by_nation = 72, // added
	increase_relation_on_nation = 73, // added
	decrease_relation_by_nation = 74, // added
	decrease_relation_on_nation = 75, // added
	join_war_by_nation = 76, // added
	join_war_on_nation = 77, // added
	gw_unlocked = 78,
	war_becomes_great = 79, // added
	cb_detected_on_nation = 80, // added
	cb_detected_by_nation = 81, // added
	crisis_join_offer_accepted_by_nation = 82, // added
	crisis_join_offer_declined_by_nation = 83, // added
	crisis_join_offer_accepted_from_nation = 84, // added
	crisis_join_offer_declined_from_nation = 85, // added
	crisis_resolution_accepted = 86, // added
	crisis_becomes_war = 87, // added
	crisis_resolution_declined_from_nation = 88, // added
	crisis_starts = 89, // added
	crisis_attacker_backer = 90, // added
	crisis_defender_backer = 91, // added
	crisis_fizzle = 92, // added
	war_join_by = 93, // added
	war_join_on = 94, // added
	cb_fab_finished = 95, // added
	cb_fab_cancelled = 96, // added
	crisis_voluntary_join = 97, // added
	count = 98
};

namespace  message_response {

constexpr inline uint8_t ignore = 0x00;
constexpr inline uint8_t log = 0x01;
constexpr inline uint8_t popup = 0x02;
constexpr inline uint8_t pause = 0x04;

constexpr inline uint8_t standard_popup = log | popup;
constexpr inline uint8_t standard_pause = log | popup | pause;

}

enum class game_mode_type {
	pick_nation,
	in_game,
	end_screen
};

enum class network_mode_type {
	single_player,
	client,
	host
};


constexpr int32_t max_event_options = 8;

} // namespace sys

namespace culture {
inline constexpr int32_t max_issue_options = 6;
}

namespace economy {
enum class province_building_type : uint8_t { railroad, fort, naval_base, last };
constexpr inline int32_t max_building_types = 3;
}

namespace ui {

enum class production_sort_order { name, factories, primary_workers, secondary_workers, owners, infrastructure };
enum class production_window_tab : uint8_t { factories = 0x0, investments = 0x1, projects = 0x2, goods = 0x3 };
constexpr inline uint32_t max_chat_message_len = 24;

}

namespace province {
namespace border {
constexpr inline uint8_t state_bit = 0x01;
constexpr inline uint8_t national_bit = 0x02;
constexpr inline uint8_t coastal_bit = 0x04;
constexpr inline uint8_t impassible_bit = 0x08;
constexpr inline uint8_t non_adjacent_bit = 0x10;
constexpr inline uint8_t river_crossing_bit = 0x20;
constexpr inline uint8_t test_bit = 0x40;
} // namespace border
} // namespace province
