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

enum class leader_type : uint8_t {
	european = 0,
	southamerican,
	russian,
	arab,
	asian,
	indian,
	nativeamerican,
	african,
	polar_bear
};
enum class commodity_group : uint8_t { military_goods = 0, raw_material_goods, industrial_goods, consumer_goods, count };

#define MSG_SETTING_LIST                                                                                                         \
	MSG_SETTING_ITEM(gaincb, "wegaincb")                                                                                         \
	MSG_SETTING_ITEM(gaincb_target, "wegaincb_target")                                                                           \
	MSG_SETTING_ITEM_R(cb_justify_no_longer_valid)                                                                               \
	MSG_SETTING_ITEM_R(losecb_target)                                                                                            \
	MSG_SETTING_ITEM(revoltalliance_cause, "revoltalliance")                                                                     \
	MSG_SETTING_ITEM(revoltalliance_target, "revoltalliance")                                                                    \
	MSG_SETTING_ITEM(war_cause, "war_they_accept")                                                                               \
	MSG_SETTING_ITEM(war_target, "war_we_accept")                                                                                \
	MSG_SETTING_ITEM(wargoal_cause, "wargoal_they_accept")                                                                       \
	MSG_SETTING_ITEM(wargoal_target, "wargoal_we_accept")                                                                        \
	MSG_SETTING_ITEM_R(socreform)                                                                                                \
	MSG_SETTING_ITEM_R(socreform_bad)                                                                                            \
	MSG_SETTING_ITEM_R(polreform)                                                                                                \
	MSG_SETTING_ITEM_R(polreform_bad)                                                                                            \
	MSG_SETTING_ITEM_R(economicreform)                                                                                           \
	MSG_SETTING_ITEM_R(economicreform_bad)                                                                                       \
	MSG_SETTING_ITEM_R(militaryreform)                                                                                           \
	MSG_SETTING_ITEM_R(militaryreform_bad)                                                                                       \
	MSG_SETTING_ITEM_R(setparty)                                                                                                 \
	MSG_SETTING_ITEM_R(upperhouse)                                                                                               \
	MSG_SETTING_ITEM_R(electionstart)                                                                                            \
	MSG_SETTING_ITEM_R(electiondone)                                                                                             \
	MSG_SETTING_ITEM_R(breakcountry)                                                                                             \
	MSG_SETTING_ITEM(rebels, "partisans")                                                                                        \
	MSG_SETTING_ITEM(annex_cause, "annex")                                                                                       \
	MSG_SETTING_ITEM(annex_target, "annex")                                                                                      \
	MSG_SETTING_ITEM(peace_accept_cause, "peace_they_accept")                                                                    \
	MSG_SETTING_ITEM(peace_accept_target, "peace_we_accept")                                                                     \
	MSG_SETTING_ITEM(peace_accept_other, "peace_other_accept")                                                                   \
	MSG_SETTING_ITEM(peace_decline_cause, "peace_they_decline")                                                                  \
	MSG_SETTING_ITEM(peace_decline_target, "peace_we_decline")                                                                   \
	MSG_SETTING_ITEM(peace_decline_other, "peace_other_decline")                                                                 \
	MSG_SETTING_ITEM(mobilize, "we_mobilize")                                                                                    \
	MSG_SETTING_ITEM(demobilize, "we_demobilize")                                                                                \
	MSG_SETTING_ITEM_R(building_done)                                                                                            \
	MSG_SETTING_ITEM_R(eventhappenother)                                                                                         \
	MSG_SETTING_ITEM_R(eventhappenotheroption)                                                                                   \
	MSG_SETTING_ITEM_R(majoreventhappenother)                                                                                    \
	MSG_SETTING_ITEM_R(majoreventhappenotheroption)                                                                              \
	MSG_SETTING_ITEM_R(invention)                                                                                                \
	MSG_SETTING_ITEM(tech, "tech_once")                                                                                          \
	MSG_SETTING_ITEM_R(leaderdied)                                                                                               \
	MSG_SETTING_ITEM_R(landbattleover)                                                                                           \
	MSG_SETTING_ITEM_R(navalbattleover)                                                                                          \
	MSG_SETTING_ITEM_R(decisionother)                                                                                            \
	MSG_SETTING_ITEM(no_longer_great_power, "we_no_longer_great_power")                                                          \
	MSG_SETTING_ITEM(became_great_power, "we_became_great_power")                                                                \
	MSG_SETTING_ITEM(alliance_accept_cause, "alliance_they_accept")                                                              \
	MSG_SETTING_ITEM(alliance_accept_target, "alliance_we_accept")                                                               \
	MSG_SETTING_ITEM(alliance_decline_cause, "alliance_they_decline")                                                            \
	MSG_SETTING_ITEM(alliance_decline_target, "alliance_we_decline")                                                             \
	MSG_SETTING_ITEM(cancelalliance_cause, "cancelalliance_they_accept")                                                         \
	MSG_SETTING_ITEM(cancelalliance_target, "cancelalliance_we_accept")                                                          \
	MSG_SETTING_ITEM(increaseopinion_cause, "increaseopinion_they_accept")                                                       \
	MSG_SETTING_ITEM(increaseopinion_target, "increaseopinion_we_accept")                                                        \
	MSG_SETTING_ITEM(addtosphere_cause, "addtosphere_they_accept")                                                               \
	MSG_SETTING_ITEM(addtosphere_target, "addtosphere_we_accept")                                                                \
	MSG_SETTING_ITEM(removefromsphere_cause, "removefromsphere_they_accept")                                                     \
	MSG_SETTING_ITEM(removefromsphere_target, "removefromsphere_we_accept")                                                      \
	MSG_SETTING_ITEM(removefromsphere_other_target, "removefromsphere_other_accept")                                             \
	MSG_SETTING_ITEM(increaserelation_cause, "increaserelation_they_accept")                                                     \
	MSG_SETTING_ITEM(increaserelation_target, "increaserelation_we_accept")                                                      \
	MSG_SETTING_ITEM(decreaserelation_cause, "decreaserelation_they_accept")                                                     \
	MSG_SETTING_ITEM(decreaserelation_target, "decreaserelation_we_accept")                                                      \
	MSG_SETTING_ITEM(intervention_same_side, "intervention_our_side")                                                            \
	MSG_SETTING_ITEM(intervention_other_side, "intervention_their_side")                                                         \
	MSG_SETTING_ITEM(intervention, "intervention_other_side")                                                                    \
	MSG_SETTING_ITEM_R(bankruptcy)                                                                                               \
	MSG_SETTING_ITEM_R(new_party_avail)                                                                                          \
	MSG_SETTING_ITEM_R(party_unavail)                                                                                            \
	MSG_SETTING_ITEM(cb_detected_cause, "others_cb_detected")                                                                    \
	MSG_SETTING_ITEM(cb_towards_us_detected, "cb_towards_us_detected")                                                           \
	MSG_SETTING_ITEM(cb_detected_target, "our_cb_detected")                                                                      \
	MSG_SETTING_ITEM(join_crisis_offer_cause, "crisis_offer_they_accept")                                                        \
	MSG_SETTING_ITEM(join_crisis_offer_target, "crisis_offer_we_accept")                                                         \
	MSG_SETTING_ITEM(join_crisis_offer_declind_cause, "crisis_offer_they_decline")                                               \
	MSG_SETTING_ITEM(join_crisis_offer_decline_target, "crisis_offer_we_decline")                                                \
	MSG_SETTING_ITEM(join_crisis_cause, "crisis_offer_they_accept")                                                              \
	MSG_SETTING_ITEM(join_crisis_target, "crisis_offer_we_accept")                                                               \
	MSG_SETTING_ITEM(crisis_offer_accept_cause, "crisis_offer_they_accept")                                                      \
	MSG_SETTING_ITEM(crisis_offer_accept_target, "crisis_offer_we_accept")                                                       \
	MSG_SETTING_ITEM(crisis_offer_accept_other, "crisis_offer_other_accept")                                                     \
	MSG_SETTING_ITEM(crisis_offer_decline_cause, "crisis_offer_they_decline")                                                    \
	MSG_SETTING_ITEM(crisis_offer_decline_target, "crisis_offer_we_decline")                                                     \
	MSG_SETTING_ITEM(crisis_offer_decline_other, "crisis_offer_other_decline")                                                   \
	MSG_SETTING_ITEM(crisis_resolved_cause, "crisis_back_down_they_accept")                                                      \
	MSG_SETTING_ITEM(crisis_resolved_target, "crisis_back_down_we_accept")                                                       \
	MSG_SETTING_ITEM_R(crisis_started)                                                                                           \
	MSG_SETTING_ITEM(crisis_became_war_target, "crisis_became_war")
#define MSG_SETTING_ITEM_R(name) MSG_SETTING_ITEM(name, #name)

enum class message_setting_type : uint8_t {
#define MSG_SETTING_ITEM(name, locale_name) name,
	MSG_SETTING_LIST count
#undef MSG_SETTING_ITEM
};

} // namespace sys

namespace culture {
inline constexpr int32_t max_issue_options = 6;
}

namespace province {
namespace border {
constexpr inline uint8_t state_bit = 0x01;
constexpr inline uint8_t national_bit = 0x02;
constexpr inline uint8_t coastal_bit = 0x04;
constexpr inline uint8_t impassible_bit = 0x08;
constexpr inline uint8_t non_adjacent_bit = 0x10;
constexpr inline uint8_t river_crossing_bit = 0x20;
} // namespace border
} // namespace province
