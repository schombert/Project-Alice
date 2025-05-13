#include "triggers.hpp"
#include "system_state.hpp"
#include "demographics.hpp"
#include "military_templates.hpp"
#include "nations_templates.hpp"
#include "prng.hpp"
#include "province_templates.hpp"
#include "ve_scalar_extensions.hpp"
#include "script_constants.hpp"
#include "fif.hpp"
#include "fif_common.hpp"

namespace fif_trigger {
using namespace fif;

#ifdef WIN32
#define CALLTYPE __vectorcall
#else
#define CALLTYPE
#endif

[[nodiscard]] std::string compare_values(uint16_t trigger_code) {
	switch(trigger_code & trigger::association_mask) {
	case trigger::association_eq:
		return " = ";
	case trigger::association_gt:
		return " > ";
	case trigger::association_lt:
		return " < ";
	case trigger::association_le:
		return " <= ";
	case trigger::association_ne:
		return " <> ";
	case trigger::association_ge:
		return " >= ";
	default:
		return " >= ";
	}
}

[[nodiscard]] std::string compare_values_eq(uint16_t trigger_code) {
	switch(trigger_code & trigger::association_mask) {
	case trigger::association_eq:
		return " = ";
	case trigger::association_gt:
		return " <> ";
	case trigger::association_lt:
		return " <> ";
	case trigger::association_le:
		return " = ";
	case trigger::association_ne:
		return " <> ";
	case trigger::association_ge:
		return " = ";
	default:
		return " = ";
	}
}
[[nodiscard]] std::string truth_inversion(uint16_t trigger_code) {
	switch(trigger_code & trigger::association_mask) {
	case trigger::association_eq:
		return "";
	case trigger::association_gt:
		return "not ";
	case trigger::association_lt:
		return "not ";
	case trigger::association_le:
		return "";
	case trigger::association_ne:
		return "not ";
	case trigger::association_ge:
		return "";
	default:
		return "";
	}
}

float read_float_from_payload(uint16_t const* data) {
	union {
		struct {
			uint16_t low;
			uint16_t high;
		} v;
		float f;
	} pack_float;

	pack_float.v.low = data[0];
	pack_float.v.high = data[1];

	return pack_float.f;
}
int32_t read_int32_t_from_payload(uint16_t const* data) {
	union {
		struct {
			uint16_t low;
			uint16_t high;
		} v;
		int32_t f;
	} pack_float;

	pack_float.v.low = data[0];
	pack_float.v.high = data[1];

	return pack_float.f;
}

std::string CALLTYPE test_trigger_generic(uint16_t const* tval, sys::state& ws);
#define TRIGGER_FUNCTION(function_name) std::string CALLTYPE function_name(uint16_t const* tval, sys::state& ws)

TRIGGER_FUNCTION(apply_disjuctively) {
	auto const source_size = 1 + trigger::get_trigger_scope_payload_size(tval);
	auto sub_units_start = tval + 2 + trigger::trigger_scope_data_payload(tval[0]);

	std::string result = "false >r ";
	while(sub_units_start < tval + source_size) {
		result += test_trigger_generic(sub_units_start, ws);
		result += "r> or >r ";
		sub_units_start += 1 + trigger::get_trigger_payload_size(sub_units_start);
	}
	result += "r> ";
	return result;
}

TRIGGER_FUNCTION(apply_conjuctively) {
	auto const source_size = 1 + trigger::get_trigger_scope_payload_size(tval);
	auto sub_units_start = tval + 2 + trigger::trigger_scope_data_payload(tval[0]);

	std::string result = "true >r ";
	while(sub_units_start < tval + source_size) {
		result += test_trigger_generic(sub_units_start, ws);
		result += "r> and >r ";
		sub_units_start += 1 + trigger::get_trigger_payload_size(sub_units_start);
	}
	result += "r> ";
	return result;
}

TRIGGER_FUNCTION(apply_subtriggers) {
	if((*tval & trigger::is_disjunctive_scope) != 0)
		return apply_disjuctively(tval, ws);
	else
		return apply_conjuctively(tval, ws);
}

TRIGGER_FUNCTION(tf_none) {
	return "true ";
}
TRIGGER_FUNCTION(tf_unused_1) {
	return "true ";
}

TRIGGER_FUNCTION(tf_generic_scope) {
	return apply_subtriggers(tval, ws);
}

TRIGGER_FUNCTION(tf_x_neighbor_province_scope) {
	if(*tval & trigger::is_existence_scope) {
		return "dup >r false >r make-ap-it "
			"while more? r@ not and "
			"loop "
				"next " // ap on top
				"if "
					"swap >r " // hide it                        
					+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
					"r> drop >r "                                               // replace bool value with new bool
					"swap drop "
				"else "
					"drop "
				"end-if "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
			;
	} else {
		return "dup >r true >r make-ap-it "
			"while more? r@ and "
			"loop "
				"next " // ap on top
				"if "
					"swap >r " // hide it                        
					+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
					"r> drop >r "                                               // replace bool value with new bool
					"swap drop "
				"else "
					"drop "
				"end-if "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
			;
	}
}
TRIGGER_FUNCTION(tf_x_neighbor_province_scope_state) {
	if(*tval & trigger::is_existence_scope) {
		return "dup >r false >r make-sp-it "
			"while more? r@ not and "
			"loop "
				"next " // it ap on top
				"if " // [it ap] [SI bool]
					"swap r> swap >r >r " // [p] [si it bool]
					"make-ap-it " // [apit] [si it bool]
					"while more? r@ not and "
					"loop "
						"next " // [apit p2] [si it bool]
						"dup state_membership @ r> r> .parent@ swap >r swap >r = not "
						"if "
							"swap >r " // hide it
							+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
							"r> drop >r "                                               // replace bool value with new bool
							"swap drop "
						"else "
							"drop "
						"end-if "
					"end-while "
					"drop r> r> swap >r "   // drop pop-iterator [bool IT], [IT, bool}, {IT], [bool]
				"else "
					"drop "
				"end-if "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
			;
	} else {
		return "dup >r true >r make-sp-it "
			"while more? r@ and "
			"loop "
				"next " // it ap on top
				"if " // [it ap] [SI bool]
					"swap r> swap >r >r " // [p] [si it bool]
					"make-ap-it " // [apit] [si it bool]
					"while more? r@ and "
					"loop "
						"next " // [apit p2] [si it bool]
						"dup state_membership @ r> r> .parent@ swap >r swap >r = not "
						"if "
							"swap >r " // hide it
							+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
							"r> drop >r "                                               // replace bool value with new bool
							"swap drop "
						"else "
							"drop "
						"end-if "
					"end-while "
					"drop r> r> swap >r "   // drop pop-iterator [bool IT], [IT, bool}, {IT], [bool]
				"else "
					"drop "
				"end-if "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
			;
	}
}
TRIGGER_FUNCTION(tf_x_neighbor_country_scope_nation) {
	if(*tval & trigger::is_existence_scope) {
		return "dup >r false >r make-an-it "
			"while more? r@ not and "
			"loop "
				"next " // ap on top
				"swap >r " // hide it                        
				+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
				"r> drop >r "                                               // replace bool value with new bool
				"swap drop "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
			;
	} else {
		return "dup >r true >r make-an-it "
			"while more? r@ and "
			"loop "
				"next " // ap on top
				"swap >r " // hide it                        
				+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
				"r> drop >r "                                               // replace bool value with new bool
				"swap drop "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
			;
	}
}
TRIGGER_FUNCTION(tf_x_neighbor_country_scope_pop) {
	if(*tval & trigger::is_existence_scope) {
		return "dup >r " + pop_to_owner() + "false >r make-an-it "
			"while more? r@ not and "
			"loop "
				"next " // ap on top
				"swap >r " // hide it                        
				+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
				"r> drop >r "                                               // replace bool value with new bool
				"swap drop "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
			;
	} else {
		return "dup >r " + pop_to_owner() + "true >r make-an-it "
			"while more? r@ and "
			"loop "
				"next " // ap on top
				"swap >r " // hide it                        
				+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
				"r> drop >r "                                               // replace bool value with new bool
				"swap drop "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
			;
	}
}
TRIGGER_FUNCTION(tf_x_war_countries_scope_nation) {
	if(*tval & trigger::is_existence_scope) {
		return "false >r dup >r " // note : removes nation from stack entirely
			"nation_adjacency-connected_nations dup size "  // R bool, nation
			"while 1 - dup 0 >= r> swap r@ not and swap >r " //war_participant_id
			"loop "
				"2dup index @ dup is_attacker @ >r " //war_participant_id on stack, R bool, nation, is attacker
				"war @ war_participant-war dup size"
				"while 1 - dup 0 >= r> swap r> swap r@ not and swap >r swap >r "
				"loop "
					"2dup index @ dup is_attacker @ r@ <> "
					"if "
						"nation @ "
						"swap >r swap >r swap >r swap >r "                              // hide array and index (x2)
						+ apply_subtriggers(tval, ws) + " r> swap r> swap r> swap r> swap" // bool result on top
						"r> swap r> swap r> drop >r >r >r "                              // replace bool value with new bool
					"else "
						"drop "
					"end-if "
				"end-while "
				"drop drop r> drop " // drop array, index, is attacker
			"end-while "
			"drop drop r> r> " // drop array, index, move stored self nation to stack,  move bool to stack
			;
	} else {
		return "true >r dup >r " // note : removes nation from stack entirely
			"nation_adjacency-connected_nations dup size "  // R bool, nation
			"while 1 - dup 0 >= r> swap r@ and swap >r " //war_participant_id
			"loop "
				"2dup index @ dup is_attacker @ >r " //war_participant_id on stack, R bool, nation, is attacker
				"war @ war_participant-war dup size"
				"while 1 - dup 0 >= r> swap r> swap r@ and swap >r swap >r "
				"loop "
					"2dup index @ dup is_attacker @ r@ <> "
					"if "
						"nation @ "
						"swap >r swap >r swap >r swap >r "                              // hide array and index (x2)
						+ apply_subtriggers(tval, ws) + " r> swap r> swap r> swap r> swap" // bool result on top
						"r> swap r> swap r> drop >r >r >r "                              // replace bool value with new bool
					"else "
						"drop "
					"end-if "
				"end-while "
				"drop drop r> drop " // drop array, index, is attacker
			"end-while "
			"drop drop r> r> " // drop array, index, move stored self nation to stack,  move bool to stack
			;
	}
}
TRIGGER_FUNCTION(tf_x_war_countries_scope_pop) {
	if(*tval & trigger::is_existence_scope) {
		return ">r r@ " + pop_to_owner() + "false >r dup >r " // note : removes nation from stack entirely
			"nation_adjacency-connected_nations dup size "  // R bool, nation
			"while 1 - dup 0 >= r> swap r@ not and swap >r " //war_participant_id
			"loop "
				"2dup index @ dup is_attacker @ >r " //war_participant_id on stack, R bool, nation, is attacker
				"war @ war_participant-war dup size"
				"while 1 - dup 0 >= r> swap r> swap r@ not and swap >r swap >r "
				"loop "
					"2dup index @ dup is_attacker @ r@ <> "
					"if "
						"nation @ "
						"swap >r swap >r swap >r swap >r "                              // hide array and index (x2)
						+ apply_subtriggers(tval, ws) + " r> swap r> swap r> swap r> swap" // bool result on top
						"r> swap r> swap r> drop >r >r >r "                              // replace bool value with new bool
					"else "
						"drop "
					"end-if "
				"end-while "
				"drop drop r> drop " // drop array, index, is attacker
			"end-while "
			"drop drop r> drop r> r> swap "// drop array, index, move stored self nation to stack,  move bool to stack
			;
	} else {
		return ">r r@ " + pop_to_owner() + "true >r dup >r " // note : removes nation from stack entirely
			"nation_adjacency-connected_nations dup size "  // R bool, nation
			"while 1 - dup 0 >= r> swap r@ and swap >r " //war_participant_id
			"loop "
				"2dup index @ dup is_attacker @ >r " //war_participant_id on stack, R bool, nation, is attacker
				"war @ war_participant-war dup size"
				"while 1 - dup 0 >= r> swap r> swap r@ and swap >r swap >r "
				"loop "
					"2dup index @ dup is_attacker @ r@ <> "
					"if "
						"nation @ "
						"swap >r swap >r swap >r swap >r "                              // hide array and index (x2)
						+ apply_subtriggers(tval, ws) + " r> swap r> swap r> swap r> swap" // bool result on top
						"r> swap r> swap r> drop >r >r >r "                              // replace bool value with new bool
					"else "
						"drop "
					"end-if "
				"end-while "
				"drop drop r> drop " // drop array, index, is attacker
			"end-while "
			"drop drop r> drop r> r> swap " // drop array, index, move stored self nation to stack,  move bool to stack
			;
	}
}
TRIGGER_FUNCTION(tf_x_country_scope) {
	if(*tval & trigger::is_existence_scope) {
		return ">r false >r nation-size @ " // note : removes nation from stack entirely
			"while 1 - dup 0 >= r@ not and "
			"loop "
				"dup >nation_id " // adjacent nation on top
				"swap >r "                                                            // hide index
				+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
				"r> drop >r "                                               // replace bool value with new bool
				"swap drop "
			"end-while "
			"drop r> r> swap " // drop  index, move stored self nation to stack,  move bool to stack
			;
	} else {
		return ">r true >r nation-size @ " // note : removes nation from stack entirely
			"while 1 - dup 0 >= r@ and "
			"loop "
				"dup >nation_id " // adjacent nation on top
				"swap >r "                                                            // hide index
				+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
				"r> drop >r "                                               // replace bool value with new bool
				"swap drop "
			"end-while "
			"drop r> r> swap " // drop  index, move stored self nation to stack,  move bool to stack
			;
	}
}
TRIGGER_FUNCTION(tf_x_greater_power_scope) {
	if(*tval & trigger::is_existence_scope) {
		return ">r false >r make-gp-it "
			"while more? r@ not and "
			"loop "
				"next " // nation on top
				"dup is_great_power @ "
				"if "
					"swap inc-gp >r " // hide it                        
					+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
					"r> drop >r "                                               // replace bool value with new bool
					"swap drop "
				"else "
					"drop "
				"end-if "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
			;
	} else {
		return ">r true >r make-gp-it "
			"while more? r@ and "
			"loop "
				"next " // nation on top
				"dup is_great_power @ "
				"if "
					"swap inc-gp >r " // hide it                        
					+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
					"r> drop >r "                                               // replace bool value with new bool
					"swap drop "
				"else "
					"drop "
				"end-if "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
			;
	}
}
TRIGGER_FUNCTION(tf_x_owned_province_scope_state) {
	if(*tval & trigger::is_existence_scope) {
		return "dup >r false >r make-sp-it "
			"while more? r@ not and "
			"loop "
				"next " // prov + properly in state on top
				"if "
					"swap >r " // hide it                        
					+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
					"r> drop >r "                                               // replace bool value with new bool
					"swap drop "
				"else "
					"drop "
				"end-if "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
			;
	} else {
		return "dup >r true >r make-sp-it "
			"while more? r@ and "
			"loop "
				"next " // prov + properly in state on top
				"if "
					"swap >r " // hide it                        
					+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
					"r> drop >r "                                               // replace bool value with new bool
					"swap drop "
				"else "
					"drop "
				"end-if "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
			;
	}
}
TRIGGER_FUNCTION(tf_x_owned_province_scope_nation) {
	if(*tval & trigger::is_existence_scope) {
		return "dup >r false >r make-n-it "
			"while more? r@ not and "
			"loop "
				"next " // prov on top
				"swap >r " // hide it                        
				+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
				"r> drop >r "                                               // replace bool value with new bool
				"swap drop "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
			;
	} else {
		return "dup >r true >r make-n-it "
			"while more? r@ and "
			"loop "
				"next " // prov on top
				"swap >r " // hide it                        
				+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
				"r> drop >r "                                               // replace bool value with new bool
				"swap drop "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
			;
	}
}
TRIGGER_FUNCTION(tf_x_core_scope_province) {
	if(*tval & trigger::is_existence_scope) {
		return "dup >r false >r make-pc-it "
			"while more? r@ not and "
			"loop "
				"next " // n on top
				"swap >r " // hide it
				"dup valid? if "
					+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
					"r> drop >r "                                               // replace bool value with new bool
					"swap drop "
				"else "
					"drop r> "
				"end-if "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
			;
	} else {
		return "dup >r true >r make-pc-it "
			"while more? r@ and "
			"loop "
				"next " // n on top
				"swap >r " // hide it
				"dup valid? if "
					+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
					"r> drop >r "                                               // replace bool value with new bool
					"swap drop "
				"else "
					"drop r> "
				"end-if "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
			;
	}
}
TRIGGER_FUNCTION(tf_x_core_scope_nation) {
	if(*tval & trigger::is_existence_scope) {
		return "dup >r false >r make-nc-it "
			"while more? r@ not and "
			"loop "
				"next " // n on top
				"swap >r " // hide it                        
				+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
				"r> drop >r "                                               // replace bool value with new bool
				"swap drop "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
			;
	} else {
		return "dup >r true >r make-nc-it "
			"while more? r@ and "
			"loop "
				"next " // n on top
				"swap >r " // hide it                        
				+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
				"r> drop >r "                                               // replace bool value with new bool
				"swap drop "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
			;
	}
}

TRIGGER_FUNCTION(tf_x_state_scope) {
	if(*tval & trigger::is_existence_scope) {
		return "dup >r false >r make-ns-it "
			"while more? r@ not and "
			"loop "
				"next " // n on top
				"swap >r " // hide it                        
				+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
				"r> drop >r "                                               // replace bool value with new bool
				"swap drop "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
		;
	} else {
		return "dup >r true >r make-ns-it "
			"while more? r@ and "
			"loop "
				"next " // n on top
				"swap >r " // hide it                        
				+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
				"r> drop >r "                                               // replace bool value with new bool
				"swap drop "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
		;
	}
}
TRIGGER_FUNCTION(tf_x_substate_scope) {
	if(*tval & trigger::is_existence_scope) {
		return "dup >r false >r make-nv-it "
			"while more? r@ not and "
			"loop "
				"next " // n on top
				"dup is_substate @ "
				"if "
					"swap >r " // hide it                        
					+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
					"r> drop >r "                                               // replace bool value with new bool
					"swap drop "
				"else "
					"drop "
				"end-if "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
		;
	} else {
		return "dup >r true >r make-nv-it "
			"while more? r@ and "
			"loop "
				"next " // n on top
				"dup is_substate @ "
				"if "
					"swap >r " // hide it                        
					+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
					"r> drop >r "                                               // replace bool value with new bool
					"swap drop "
				"else "
					"drop "
				"end-if "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
		;
	}
}
TRIGGER_FUNCTION(tf_x_sphere_member_scope) {
	if(*tval & trigger::is_existence_scope) {
		return "dup >r false >r make-ngp-it "
			"while more? r@ not and "
			"loop "
				"next " // n on top
				"if "
					"swap >r " // hide it                        
					+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
					"r> drop >r "                                               // replace bool value with new bool
					"swap drop "
				"else "
					"drop "
				"end-if "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
		;
	} else {
		return "dup >r true >r make-ngp-it "
			"while more? r@ and "
			"loop "
				"next " // n on top
				"if "
					"swap >r " // hide it                        
					+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
					"r> drop >r "                                               // replace bool value with new bool
					"swap drop "
				"else "
					"drop "
				"end-if "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
		;
	}
}
TRIGGER_FUNCTION(tf_x_pop_scope_province) {
	if(*tval & trigger::is_existence_scope) {
		return "dup >r false >r make-pp-it "
			"while more? r@ not and "
			"loop "
				"next " // n on top
				"swap >r " // hide it                        
				+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
				"r> drop >r "                                               // replace bool value with new bool
				"swap drop "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
		;
	} else {
		return "dup >r true >r make-pp-it "
			"while more? r@ and "
			"loop "
				"next " // n on top
				"swap >r " // hide it                        
				+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
				"r> drop >r "                                               // replace bool value with new bool
				"swap drop "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
		;
	}
}
TRIGGER_FUNCTION(tf_x_pop_scope_state) {
	if(*tval & trigger::is_existence_scope) {
		return "dup >r false >r make-sp-it "
			"while more? r@ not and "
			"loop "
				"next " // v on top
				"if "
					"swap r> swap >r >r " // hide it [IT bool]
					"make-pp-it " // state -> it
					"while more? r@ not and "
					"loop "
						"next " // p on top
						"swap >r " // hide it
						+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
						"r> drop >r "                                               // replace bool value with new bool
						"swap drop "
					"end-while "
					"drop r> r> swap >r "   // drop pop-iterator [bool IT], [IT, bool}, {IT], [bool] 
				"else "
					"drop "
				"end-if "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
		;
	} else {
		return "dup >r true >r make-sp-it "
			"while more? r@ and "
			"loop "
				"next " // v on top
				"if "
					"swap r> swap >r >r " // hide it [IT bool]
					"make-pp-it " // state -> it
					"while more? r@ and "
					"loop "
						"next " // p on top
						"swap >r " // hide it
						+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
						"r> drop >r "                                               // replace bool value with new bool
						"swap drop "
					"end-while "
					"drop r> r> swap >r "   // drop pop-iterator [bool IT], [IT, bool}, {IT], [bool] 
				"else "
					"drop "
				"end-if "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
		;
	}
}
TRIGGER_FUNCTION(tf_x_pop_scope_nation) {
	if(*tval & trigger::is_existence_scope) {
		return "dup >r false >r make-n-it "
			"while more? r@ not and "
			"loop "
				"next " // v on top
				"if "
					"swap r> swap >r >r " // hide it [IT bool]
					"make-pp-it " // state -> it
					"while more? r@ not and "
					"loop "
						"next " // p on top
						"swap >r " // hide it
						+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
						"r> drop >r "                                               // replace bool value with new bool
						"swap drop "
					"end-while "
					"drop r> r> swap >r "   // drop pop-iterator [bool IT], [IT, bool}, {IT], [bool] 
				"else "
					"drop "
				"end-if "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
		;
	} else {
		return "dup >r true >r make-n-it "
			"while more? r@ and "
			"loop "
				"next " // v on top
				"if "
					"swap r> swap >r >r " // hide it [IT bool]
					"make-pp-it " // state -> it
					"while more? r@ and "
					"loop "
						"next " // p on top
						"swap >r " // hide it
						+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
						"r> drop >r "                                               // replace bool value with new bool
				"swap drop "
					"end-while "
					"drop r> r> swap >r "   // drop pop-iterator [bool IT], [IT, bool}, {IT], [bool] 
				"else "
					"drop "
				"end-if "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
		;
	}
}
TRIGGER_FUNCTION(tf_x_provinces_in_variable_region) {
	if(*tval & trigger::is_existence_scope) {
		return ">r false >r " + std::to_string(trigger::payload(*(tval + 2)).state_id.index()) + " >state_definition_id make-asp-it "
			"while more? r@ not and "
			"loop "
				"next " // n on top
				"swap >r " // hide it                        
				+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
				"r> drop >r "                                               // replace bool value with new bool
				"swap drop "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
		;
	} else {
		return ">r true >r " + std::to_string(trigger::payload(*(tval + 2)).state_id.index()) + " >state_definition_id make-asp-it "
			"while more? r@ and "
			"loop "
				"next " // n on top
				"swap >r " // hide it                        
				+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
				"r> drop >r "                                               // replace bool value with new bool
				"swap drop "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
		;
	}
}
TRIGGER_FUNCTION(tf_x_provinces_in_variable_region_proper) {
	if(*tval & trigger::is_existence_scope) {
		return ">r false >r " + std::to_string(trigger::payload(*(tval + 2)).reg_id.index()) + " >region_id make-arp-it "
			"while more? r@ not and "
			"loop "
				"next " // n on top
				"swap >r " // hide it                        
				+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
				"r> drop >r "                                               // replace bool value with new bool
				"swap drop "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
		;
	} else {
		return ">r true >r " + std::to_string(trigger::payload(*(tval + 2)).reg_id.index()) + " >region_id make-arp-it "
			"while more? r@ and "
			"loop "
				"next " // n on top
				"swap >r " // hide it                        
				+ apply_subtriggers(tval, ws) + " r> swap " // bool result on top
				"r> drop >r "                                               // replace bool value with new bool
				"swap drop "
			"end-while "
			"drop r> r> swap " // drop  it, move stored to stack,  move bool to stack
		;
	}
}


TRIGGER_FUNCTION(tf_owner_scope_state) {
	std::string result;
	result += "dup >r " + state_to_owner();
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap ";
	return result;
}
TRIGGER_FUNCTION(tf_owner_scope_province) {
	std::string result;
	result += "dup >r " + province_to_owner();
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap ";
	return result;
}
TRIGGER_FUNCTION(tf_controller_scope) {
	std::string result;
	result += "dup >r " + province_to_controller();
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap ";
	return result;
}

TRIGGER_FUNCTION(tf_location_scope) {
	std::string result;
	result += "dup >r " + pop_to_location();
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap ";
	return result;
}
TRIGGER_FUNCTION(tf_country_scope_state) {
	std::string result;
	result += "dup >r " + state_to_owner();
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap ";
	return result;
}
TRIGGER_FUNCTION(tf_country_scope_pop) {
	std::string result;
	result += "dup >r " + pop_to_owner();
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap ";
	return result;
}
TRIGGER_FUNCTION(tf_capital_scope) {
	std::string result;
	result += "dup >r capital @ ";
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap ";
	return result;
}
TRIGGER_FUNCTION(tf_capital_scope_province) {
	std::string result;
	result += "dup >r " + province_to_owner() + "capital @ ";
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap ";
	return result;
}
TRIGGER_FUNCTION(tf_capital_scope_pop) {
	std::string result;
	result += "dup >r " + pop_to_owner() + "capital @ ";
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap ";
	return result;
}
TRIGGER_FUNCTION(tf_this_scope) {
	std::string result;
	result += ">r dup ";
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap ";
	return result;
}
TRIGGER_FUNCTION(tf_from_scope) {
	std::string result;
	result += ">r >r dup r> swap ";
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap ";
	return result;
}

TRIGGER_FUNCTION(tf_sea_zone_scope) {
	std::string result;
	result += "dup >r port_to @ "; // F T port -- source province on R
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap "; // put original province back under result value
	return result;
}

std::string culture_to_union_holder() {
	return "culture_group_membership-member group @ "
		"cultural_union_of-culture_group identity @ identity_holder-identity nation @ ";
}
std::string nation_to_union_holder() {
	return "primary_culture @ " + culture_to_union_holder();
}


TRIGGER_FUNCTION(tf_cultural_union_scope) {
	std::string result;
	result += "dup >r " + nation_to_union_holder();
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap "; // put original under result value
	return result;
}
std::string overlord_from_subject() {
	return "overlord-subject ruler @ ";
}
TRIGGER_FUNCTION(tf_overlord_scope) {
	std::string result;
	result += "dup >r " + overlord_from_subject();
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap "; // put original under result value
	return result;
}
TRIGGER_FUNCTION(tf_sphere_owner_scope) {
	std::string result;
	result += "dup >r in_sphere_of @ ";
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap "; // put original under result value
	return result;
}
TRIGGER_FUNCTION(tf_independence_scope) {
	std::string result;
	result += ">r >r dup defection_target @  identity_holder-identity nation @ r> swap ";
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap "; // put original under result value
	return result;
}
TRIGGER_FUNCTION(tf_flashpoint_tag_scope) {
	std::string result;
	result += "dup >r flashpoint_tag @  identity_holder-identity nation @ ";
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap "; // put original under result value
	return result;
}
TRIGGER_FUNCTION(tf_crisis_state_scope) {
	std::string result;
	result += ">r crisis-state @ ";
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap "; // put original under result value
	return result;
}
TRIGGER_FUNCTION(tf_state_scope_province) {
	std::string result;
	result += "dup >r state_membership @ ";
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap "; // put original under result value
	return result;
}
TRIGGER_FUNCTION(tf_state_scope_pop) {
	std::string result;
	result += "dup >r " + pop_to_location() + "state_membership @ ";
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap "; // put original under result value
	return result;
}
TRIGGER_FUNCTION(tf_tag_scope) {
	auto tag = trigger::payload(tval[2]).tag_id;

	std::string result;
	result += ">r " + std::to_string(tag.index()) + " >national_identity_id identity_holder-identity nation @ ";
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap "; // put original under result value
	return result;
}
TRIGGER_FUNCTION(tf_integer_scope) {
	auto wprov = trigger::payload(tval[2]).prov_id;

	std::string result;
	result += ">r " + std::to_string(wprov.index()) + " >province_id ";
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap "; // put original under result value
	return result;
}
TRIGGER_FUNCTION(tf_country_scope_nation) {
	return apply_subtriggers(tval, ws);
}
TRIGGER_FUNCTION(tf_country_scope_province) {
	std::string result;
	result += "dup >r " + province_to_owner();
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap "; // put original under result value
	return result;
}
TRIGGER_FUNCTION(tf_cultural_union_scope_pop) {
	std::string result;
	result += "dup >r culture @ " + culture_to_union_holder();
	result += apply_subtriggers(tval, ws);
	result += "swap drop r> swap "; // put original under result value
	return result;
}

//
// non scope trigger functions
//

TRIGGER_FUNCTION(tf_year) {
	return "start-date @ current-date @ >i64 -1 >i64 + + current-year " + std::to_string(int32_t(tval[1])) + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_month) {
	return "start-date @ current-date @ >i64 -1 >i64 + + current-month " + std::to_string(int32_t(tval[1])) + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_port) {
	return "dup is_coast @ ";
}
TRIGGER_FUNCTION(tf_rank) {
	// note: comparison reversed since rank 1 is "greater" than rank 1 + N
	return "dup get_rank @ >i32 " + std::to_string(int32_t(tval[1])) + " swap " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_technology) {
	return "dup " + std::to_string(trigger::payload(tval[1]).tech_id.index()) + " >technology_id active_technologies @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_technology_province) {
	return "dup " + province_to_owner() + std::to_string(trigger::payload(tval[1]).tech_id.index()) + " >technology_id active_technologies @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_technology_pop) {
	return "dup " + pop_to_owner() + std::to_string(trigger::payload(tval[1]).tech_id.index()) + " >technology_id active_technologies @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_strata_rich) {
	return "dup poptype @ strata @ " + std::to_string(int32_t(culture::pop_strata::rich)) + " >u8 " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_strata_middle) {
	return "dup poptype @ strata @ " + std::to_string(int32_t(culture::pop_strata::middle)) + " >u8 " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_strata_poor) {
	return "dup poptype @ strata @ " + std::to_string(int32_t(culture::pop_strata::poor)) + " >u8 " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_life_rating_province) {
	return "dup life_rating @ " + std::to_string(trigger::payload(tval[1]).signed_value) + " >u8 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_life_rating_state) {
	return "dup capital @ life_rating @ " + std::to_string(trigger::payload(tval[1]).signed_value) + " >u8 " + compare_values(tval[0]);
}

TRIGGER_FUNCTION(tf_has_empty_adjacent_state_province) {
	return "dup >index state-ptr @ empty-a-province " + truth_inversion(tval[0]);
	/*
	return "dup false >r >r province_adjacency-connected_provinces dup size "
		"while 1 - dup 0 >= loop "
			"2dup nth-adjacent dup sea-province? not swap " + province_to_owner() + "valid? not and "
			"r> swap r> or >r >r "
		"end-while "
		"drop drop "
		" r> r> " + truth_inversion(tval[0]);
		;
	*/
}

TRIGGER_FUNCTION(tf_has_empty_adjacent_state_state) {
	return "dup >index state-ptr @ empty-a-state " + truth_inversion(tval[0]);
	/*
	return
		"dup dup dup " + state_to_owner() + ">r false >r definition @ abstract_state_membership-state dup size "
		"while 1 - dup 0 >= loop "
			"2dup index @ province " // now have a province in scope, just need to iterate over it ...
			"dup " + province_to_owner() + "r> swap r@ = swap >r " //
			" if "
				"dup >r province_adjacency-connected_provinces dup size "
				"while 1 - dup 0 >= loop " // R: owner BOOL SOURCE-PROV
					"2dup nth-adjacent dup sea-province? not swap " + province_to_owner() + "valid? not and " // proxy - index  - bool
					"r> swap r> or >r >r "
				"end-while "
				"drop drop " // drop proxy and index
				"r> drop " // discard saved province
			" else "
				" drop " // drop province
			" end-if "
		"end-while "
		"drop drop " // drop proxy and index
		" r> r> swap drop " + truth_inversion(tval[0]);
	;
	*/
}
TRIGGER_FUNCTION(tf_state_id_province) {
	return "dup abstract_state_membership-province state @ "
		+ std::to_string(trigger::payload(tval[1]).prov_id.index()) + " >province_id abstract_state_membership-province state @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_state_id_state) {
	return "dup definition @ "
		+ std::to_string(trigger::payload(tval[1]).prov_id.index()) + " >province_id abstract_state_membership-province state @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_cash_reserves) {
	return "true " + truth_inversion(tval[0]);
	// commented out because desired_needs_spending is not implemented
	//auto ratio = read_float_from_payload(tval + 1);
	//auto target = economy::desired_needs_spending(ws, to_pop(primary_slot)); 
	//auto savings_qnty = ws.world.pop_get_savings(to_pop(primary_slot));
	//return compare_values(tval[0], ve::select(target != 0.0f, savings_qnty * 100.0f / target, 100.0f), ratio);
}


TRIGGER_FUNCTION(tf_unemployment_nation) {
	return "dup " + std::to_string(demographics::employable.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::employed.index()) + " >demographics_key demographics @ 1.0 swap r@ / - 0.0 0.0 r> >= select "  + std::to_string(read_float_from_payload(tval + 1))  + compare_values(tval[0]);
	//auto total_employable = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::employable);
	//auto total_employed = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::employed);
	//return compare_values(tval[0], ve::select(total_employable > 0.0f, 1.0f - (total_employed / total_employable), 0.0f),
	//		read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_unemployment_state) {
	return "dup " + std::to_string(demographics::employable.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::employed.index()) + " >demographics_key demographics @ 1.0 swap r@ / - 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_unemployment_province) {
	return "dup " + std::to_string(demographics::employable.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::employed.index()) + " >demographics_key demographics @ 1.0 swap r@ / - 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_unemployment_pop) {
	return "dup poptype @ has_unemployment @ not >r "
		"dup uemployment @ >f32 " + std::to_string(1.0f / float(std::numeric_limits<uint8_t>::max())) + " * 1.0 swap - 0.0 r> select "
		+ std::to_string(read_float_from_payload(tval + 1)) + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_is_slave_nation) {
	return "dup combined_issue_rules @ >i32 " + std::to_string(issue_rule::slavery_allowed) + " and 0 <> " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_slave_state) {
	return "dup capital @ is_slave @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_slave_province) {
	return "dup is_slave @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_slave_pop) {
	return "dup poptype @ >index " + std::to_string(ws.culture_definitions.slaves.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_is_independant) {
	return "dup overlord-subject ruler @ valid? not " + truth_inversion(tval[0]);
}

std::string demo_culture_key(sys::state& ws) {
	return ">index " + std::to_string(demographics::count_special_keys + ws.world.pop_type_size() * 2) + " + >demographics_key ";
}
TRIGGER_FUNCTION(tf_has_national_minority_province) {
	return "dup dup " + province_to_owner() + " primary_culture @ " + demo_culture_key(ws) + "demographics @ >r "
		"dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ r> <> " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_national_minority_state) {
	return "dup dup " + state_to_owner() + " primary_culture @ " + demo_culture_key(ws) + "demographics @ >r "
		"dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ r> <> " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_national_minority_nation) {
	return "dup dup primary_culture @ " + demo_culture_key(ws) + "demographics @ >r "
		"dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ r> <> " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_government_nation) {
	return "dup government_type @ >index " + std::to_string(trigger::payload(tval[1]).gov_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_government_pop) {
	return "dup " + pop_to_owner() + "government_type @ >index " + std::to_string(trigger::payload(tval[1]).gov_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_capital) {
	return "dup capital @ >index " + std::to_string(trigger::payload(tval[1]).prov_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_tech_school) {
	return "dup tech_school @ >index " + std::to_string(trigger::payload(tval[1]).mod_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_primary_culture) {
	return "dup primary_culture @ >index " + std::to_string(trigger::payload(tval[1]).cul_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_primary_culture_pop) {
	return "dup " + pop_to_owner()  + "primary_culture @ >index " + std::to_string(trigger::payload(tval[1]).cul_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_accepted_culture) {
	return "dup " + std::to_string(trigger::payload(tval[1]).cul_id.index()) + " >culture_id accepted_cultures @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_pop) {
	return "dup culture @ >index " + std::to_string(trigger::payload(tval[1]).cul_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_state) {
	return "dup dominant_culture @ >index " + std::to_string(trigger::payload(tval[1]).cul_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_province) {
	return "dup dominant_culture @ >index " + std::to_string(trigger::payload(tval[1]).cul_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_nation) {
	return "dup primary_culture @ >index " + std::to_string(trigger::payload(tval[1]).cul_id.index()) + " = >r "
		"dup " + std::to_string(trigger::payload(tval[1]).cul_id.index()) + " >culture_id accepted_cultures @ r> or " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_pop_reb) {
	return ">r >r dup primary_culture @ >index r> swap r> swap >r dup culture @ >index r> " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_state_reb) {
	return ">r >r dup primary_culture @ >index r> swap r> swap >r dup dominant_culture @ >index r> " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_province_reb) {
	return ">r >r dup primary_culture @ >index r> swap r> swap >r dup dominant_culture @ >index r> " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_nation_reb) {
	return ">r >r dup primary_culture @  r> swap r> swap >r dup primary_culture @ r@ = >r "
		"dup r> r> swap >r  accepted_cultures @ r> or " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_this_nation) {
	return "dup culture @  swap >r >r dup primary_culture @  r@ = >r " // R[pop C bool]
		"dup r> r> swap >r accepted_cultures @ r> or " + truth_inversion(tval[0]) + "r> swap ";
}
TRIGGER_FUNCTION(tf_culture_from_nation) {
	return "dup culture @   swap >r swap >r >r dup primary_culture @  r@ = >r "
		"dup r> r> swap >r  accepted_cultures @ r> or " + truth_inversion(tval[0]) + "r> swap r> swap ";
}
TRIGGER_FUNCTION(tf_culture_this_state) {
	return "dup culture @   swap >r >r dup " + state_to_owner() + "dup primary_culture @  r@ = >r "
		"r> r> swap >r  accepted_cultures @ r> or " + truth_inversion(tval[0]) + "r> swap ";
}
TRIGGER_FUNCTION(tf_culture_this_pop) {
	return "dup culture @   swap >r >r dup " + pop_to_owner() + "dup primary_culture @  r@ = >r "
		"r> r> swap >r  accepted_cultures @ r> or " + truth_inversion(tval[0]) + "r> swap ";
}
TRIGGER_FUNCTION(tf_culture_this_province) {
	return "dup culture @   swap >r >r dup " + province_to_owner() + "dup primary_culture @  r@ = >r "
		"r> r> swap >r  accepted_cultures @ r> or " + truth_inversion(tval[0]) + "r> swap ";
}
TRIGGER_FUNCTION(tf_culture_group_nation) {
	return "dup primary_culture @ culture_group_membership-member group @ >index " + std::to_string(trigger::payload(tval[1]).culgrp_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_group_pop) {
	return "dup culture @ culture_group_membership-member group @ >index " + std::to_string(trigger::payload(tval[1]).culgrp_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_group_province) {
	return "dup dominant_culture @ culture_group_membership-member group @ >index " + std::to_string(trigger::payload(tval[1]).culgrp_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_group_state) {
	return "dup dominant_culture @ culture_group_membership-member group @ >index " + std::to_string(trigger::payload(tval[1]).culgrp_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_group_reb_nation) {
	return "dup primary_culture @ culture_group_membership-member group @ >r >r >r dup primary_culture @ culture_group_membership-member group @ r> swap r> swap r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_group_reb_pop) {
	return "dup culture @ culture_group_membership-member group @ >r >r >r dup primary_culture @ culture_group_membership-member group @ r> swap r> swap r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_group_nation_this_nation) {
	return "dup primary_culture @ culture_group_membership-member group @ >r >r dup primary_culture @ culture_group_membership-member group @ r> swap r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_group_pop_this_nation) {
	return "dup culture @ culture_group_membership-member group @ >r >r dup primary_culture @ culture_group_membership-member group @ r> swap r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_group_nation_from_nation) {
	return "dup primary_culture @ culture_group_membership-member group @ >r >r >r dup primary_culture @ culture_group_membership-member group @ r> swap r> swap r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_group_pop_from_nation) {
	return "dup culture @ culture_group_membership-member group @ >r >r >r dup primary_culture @ culture_group_membership-member group @ r> swap r> swap r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_group_nation_this_province) {
	return "dup primary_culture @ culture_group_membership-member group @ >r >r dup " + province_to_owner() + "primary_culture @ culture_group_membership-member group @ r> swap r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_group_pop_this_province) {
	return "dup culture @ culture_group_membership-member group @ >r >r dup " + province_to_owner() + "primary_culture @ culture_group_membership-member group @ r> swap r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_group_nation_this_state) {
	return "dup primary_culture @ culture_group_membership-member group @ >r >r dup " + state_to_owner() + "primary_culture @ culture_group_membership-member group @ r> swap r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_group_pop_this_state) {
	return "dup culture @ culture_group_membership-member group @ >r >r dup " + state_to_owner() + "primary_culture @ culture_group_membership-member group @ r> swap r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_group_nation_this_pop) {
	return "dup primary_culture @ culture_group_membership-member group @ >r >r dup " + pop_to_owner() + "primary_culture @ culture_group_membership-member group @ r> swap r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_group_pop_this_pop) {
	return "dup culture @ culture_group_membership-member group @ >r >r dup " + pop_to_owner() + "primary_culture @ culture_group_membership-member group @ r> swap r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_religion) {
	return "dup religion @ >index " + std::to_string(trigger::payload(tval[1]).rel_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_religion_reb) {
	return ">r >r dup religion @ >index r> swap r> swap >r dup religion @ >index r> " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_religion_from_nation) {
	return ">r >r dup religion @ >index r> swap r> swap >r dup religion @ >index r> " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_religion_this_nation) {
	return ">r dup religion @ >index r> swap >r dup religion @ >index r> " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_religion_this_state) {
	return ">r dup " + state_to_owner()  + "religion @ >index r> swap >r dup religion @ >index r> " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_religion_this_province) {
	return ">r dup " + province_to_owner() + "religion @ >index r> swap >r dup religion @ >index r> " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_religion_this_pop) {
	return ">r dup " + pop_to_owner() + "religion @ >index r> swap >r dup religion @ >index r> " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_terrain_province) {
	return "dup terrain @ >index " + std::to_string(trigger::payload(tval[1]).mod_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_terrain_pop) {
	return "dup " + pop_to_location()  + "terrain @ >index " + std::to_string(trigger::payload(tval[1]).mod_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_trade_goods) {
	return "dup rgo @ >index " + std::to_string(trigger::payload(tval[1]).com_id.index()) + " " + compare_values_eq(tval[0]);
}

TRIGGER_FUNCTION(tf_is_secondary_power_nation) {
	return "dup rank @ >i32 " + std::to_string(int32_t(ws.defines.colonial_rank)) + " <= " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_secondary_power_pop) {
	return "dup " + pop_to_owner() + "rank @ >i32 " + std::to_string(int32_t(ws.defines.colonial_rank)) + " <= " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_faction_nation) {
	return "false >r dup rebellion_within-ruler dup size "
		"while 1 - dup 0 >= loop "
			"2dup index @ rebels type @ >index " + std::to_string(trigger::payload(tval[1]).reb_id.index()) + " = r> or >r "
		"end-while "
		"drop drop "
		"r> " + truth_inversion(tval[0]);
	;
}
TRIGGER_FUNCTION(tf_has_faction_pop) {
	return "dup pop_rebellion_membership-pop type @ >index " + std::to_string(trigger::payload(tval[1]).reb_id.index()) + " " + compare_values_eq(tval[0]);
}

TRIGGER_FUNCTION(tf_has_unclaimed_cores) {
	return "dup >index state-ptr @ unowned-core " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_have_core_in_nation_tag) {
	return "dup identity_holder-nation @ identity >index " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id identity_holder-identity nation @ >index state-ptr @ core-in-nation " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_have_core_in_nation_this) {
	return "dup identity_holder-nation @ identity >index >r >r dup >index r> swap r> swap state-ptr @ core-in-nation " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_have_core_in_nation_from) {
	return "dup identity_holder-nation @ identity >index >r >r >r dup >index r> swap r> swap r> swap state-ptr @ core-in-nation " + truth_inversion(tval[0]);
}

TRIGGER_FUNCTION(tf_is_cultural_union_bool) {
	return "dup identity_holder-nation @ identity cultural_union_of-identity @ valid? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_cultural_union_this_self_pop) {
	return "dup culture @ culture_group_membership-member group @ >r >r "
		"dup identity_holder-nation @ identity cultural_union_of-identity @ culture_group r> swap r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_cultural_union_pop_this_pop) {
	return "dup culture @ culture_group_membership-member group @ >r >r "
		"dup " + pop_to_owner() + "identity_holder-nation @ identity cultural_union_of-identity @ culture_group r> swap r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_cultural_union_this_nation) {
	return "dup identity_holder-nation @ identity cultural_union_of-identity @ culture_group >r >r "
		"dup identity_holder-nation @ identity cultural_union_of-identity @ culture_group r> swap r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_cultural_union_this_pop) {
	return "dup identity_holder-nation @ identity cultural_union_of-identity @ culture_group >r >r "
		"dup " + pop_to_owner() + "identity_holder-nation @ identity cultural_union_of-identity @ culture_group r> swap r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_cultural_union_this_state) {
	return "dup identity_holder-nation @ identity cultural_union_of-identity @ culture_group >r >r "
		"dup " + state_to_owner() + "identity_holder-nation @ identity cultural_union_of-identity @ culture_group r> swap r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_cultural_union_this_province) {
	return "dup identity_holder-nation @ identity cultural_union_of-identity @ culture_group >r >r "
		"dup " + province_to_owner() + "identity_holder-nation @ identity cultural_union_of-identity @ culture_group r> swap r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_cultural_union_this_rebel) {
	return "dup identity_holder-nation @ identity cultural_union_of-identity @ culture_group >r >r >r "
		"dup primary_culture @ culture_group_membership-member group @ r> swap r> swap r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_cultural_union_tag_nation) {
	return "dup primary_culture @ culture_group_membership-member group @ cultural_union_of-culture_group identity @ >index "
		+ std::to_string(trigger::payload(tval[1]).tag_id.index()) + " = >r "
		"dup identity_holder-nation @ identity cultural_union_of-identity @ culture_group "
		+ std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id cultural_union_of-identity @ culture_group = "
		"r> or " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_cultural_union_tag_this_pop) {
	return ">r dup culture @ culture_group_membership-member group @ cultural_union_of-culture_group identity @ >index "
		+ std::to_string(trigger::payload(tval[1]).tag_id.index()) + " = r> swap " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_cultural_union_tag_this_state) {
	return ">r dup " + state_to_owner()  + "primary_culture @ culture_group_membership-member group @ cultural_union_of-culture_group identity @ >index "
		+ std::to_string(trigger::payload(tval[1]).tag_id.index()) + " = >r "
		"dup " + state_to_owner() + "identity_holder-nation @ identity cultural_union_of-identity @ culture_group "
		+ std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id cultural_union_of-identity @ culture_group = "
		"r> or r> swap" + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_cultural_union_tag_this_province) {
	return ">r dup " + state_to_owner() + "primary_culture @ culture_group_membership-member group @ cultural_union_of-culture_group identity @ >index "
		+ std::to_string(trigger::payload(tval[1]).tag_id.index()) + " = >r "
		"dup " + state_to_owner() + "identity_holder-nation @ identity cultural_union_of-identity @ culture_group "
		+ std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id cultural_union_of-identity @ culture_group = "
		"r> or r> swap" + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_cultural_union_tag_this_nation) {
	return ">r dup primary_culture @ culture_group_membership-member group @ cultural_union_of-culture_group identity @ >index "
		+ std::to_string(trigger::payload(tval[1]).tag_id.index()) + " = >r "
		"dup identity_holder-nation @ identity cultural_union_of-identity @ culture_group "
		+ std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id cultural_union_of-identity @ culture_group = "
		"r> or r> swap" + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_factory_nation) {
	return "dup combined_issue_rules @ >i32 " + std::to_string(issue_rule::pop_build_factory) + " and 0 <> " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_factory_province) {
	return "dup " + province_to_owner() + "combined_issue_rules @ >i32 " + std::to_string(issue_rule::pop_build_factory) + " and 0 <> " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_factory_pop) {
	return "dup " + pop_to_owner() + "combined_issue_rules @ >i32 " + std::to_string(issue_rule::pop_build_factory) + " and 0 <> " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_war_nation) {
	return "dup is_at_war @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_war_pop) {
	return "dup " + pop_to_owner() + "is_at_war @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_war_exhaustion_nation) {
	return "dup war_exhaustion @ " + std::to_string(read_float_from_payload(tval + 1))  + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_war_exhaustion_province) {
	return "dup " + province_to_owner() + "war_exhaustion @ " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_war_exhaustion_pop) {
	return "dup " + pop_to_owner() + "war_exhaustion @ " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_blockade) {
	return "dup central_ports @ >f32 >r dup central_blockaded @ >f32 r@ / 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_diplo_points) {
	return "dup diplomatic_points @ " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_owns) {
	return "dup " + std::to_string(trigger::payload(tval[1]).prov_id.index()) + " >province_id " + province_to_owner() + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_owns_province) {
	return "dup " + province_to_owner() + std::to_string(trigger::payload(tval[1]).prov_id.index()) + " >province_id " + province_to_owner() + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_controls) {
	return "dup " + std::to_string(trigger::payload(tval[1]).prov_id.index()) + " >province_id " + province_to_controller() + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_core_integer) {
	return "dup " + nation_to_tag() + ">index " + std::to_string(trigger::payload(tval[1]).prov_id.index()) + " state-ptr @ core-in-province " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_core_this_nation) {
	return "dup >index >r >r dup " + nation_to_tag() + ">index r> swap r> state-ptr @ core-in-province " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_core_this_state) {
	return "dup >index >r >r dup " + state_to_owner() + nation_to_tag() + ">index r> swap r> state-ptr @ core-in-province " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_core_this_province) {
	return "dup >index >r >r dup " + province_to_owner() + nation_to_tag() + ">index r> swap r> state-ptr @ core-in-province " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_core_this_pop) {
	return "dup >index >r >r dup " + pop_to_owner() + nation_to_tag() + ">index r> swap r> state-ptr @ core-in-province " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_core_from_nation) {
	return "dup >index >r >r >r dup " + nation_to_tag() + ">index r> swap r> swap r> state-ptr @ core-in-province " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_core_reb) {
	return "dup >index >r >r >r dup defection_target @ >index r> swap r> swap r> state-ptr @ core-in-province " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_core_state_from_nation) {
	return "dup >index >r >r >r dup " + nation_to_tag() + ">index r> swap r> swap r> state-ptr @ core-in-state " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_core_state_this_nation) {
	return "dup >index >r >r dup " + nation_to_tag() + ">index r> swap r> state-ptr @ core-in-state " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_core_state_this_province) {
	return "dup >index >r >r dup " + province_to_owner() + nation_to_tag() + ">index r> swap r> state-ptr @ core-in-state " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_core_state_this_pop) {
	return "dup >index >r >r dup " + pop_to_owner() + nation_to_tag() + ">index r> swap r> state-ptr @ core-in-state " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_core_boolean) {
	return "dup is_owner_core @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_core_tag) {
	return "dup >index " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " swap state-ptr @ core-in-province " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_core_pop_tag) {
	return "dup " + pop_to_location() + ">index " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " swap state-ptr @ core-in-province " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_core_state_tag) {
	return "dup >index " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " swap state-ptr @ core-fully-state " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_num_of_revolts) {
	return "dup rebel_controlled_count @ >i32 " + std::to_string(tval[1]) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_revolt_percentage) {
	return "dup central_province_count @ >f32 >r dup central_rebel_controlled @ >f32 r@ / 0.0 r> 0.0 <= select "
		+ std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_num_of_cities_int) {
	return "dup owned_province_count @ >i32 " + std::to_string(tval[1]) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_num_of_cities_from_nation) {
	return ">r >r dup owned_province_count @ r> swap r> swap >r dup owned_province_count @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_num_of_cities_this_nation) {
	return ">r dup owned_province_count @ r> swap >r dup owned_province_count @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_num_of_cities_this_state) {
	return ">r dup " + state_to_owner()  + "owned_province_count @ r> swap >r dup owned_province_count @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_num_of_cities_this_province) {
	return ">r dup " + province_to_owner() + "owned_province_count @ r> swap >r dup owned_province_count @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_num_of_cities_this_pop) {
	return ">r dup " + pop_to_owner() + "owned_province_count @ r> swap >r dup owned_province_count @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_num_of_ports) {
	return "dup central_ports @ >i32 " + std::to_string(tval[1]) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_num_of_allies) {
	return "dup allies_count @ >i32 " + std::to_string(tval[1]) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_num_of_vassals) {
	return "dup vassals_count @ >i32 " + std::to_string(tval[1]) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_owned_by_tag) {
	return "dup " + province_to_owner() + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + "= " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_owned_by_this_nation) {
	return ">r dup r> swap >r dup " + province_to_owner() + " r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_owned_by_from_nation) {
	return ">r >r dup r> swap r> swap >r dup " + province_to_owner() + " r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_owned_by_this_province) {
	return ">r dup " + province_to_owner() + "r> swap >r dup " + province_to_owner() + " r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_owned_by_this_state) {
	return ">r dup " + state_to_owner() + "r> swap >r dup " + province_to_owner() + " r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_owned_by_this_pop) {
	return ">r dup " + pop_to_owner() + "r> swap >r dup " + province_to_owner() + " r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_owned_by_state_tag) {
	return "dup " + state_to_owner() + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + "= " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_owned_by_state_this_nation) {
	return ">r dup r> swap >r dup " + state_to_owner() + " r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_owned_by_state_from_nation) {
	return ">r >r dup r> swap r> swap >r dup " + state_to_owner() + " r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_owned_by_state_this_province) {
	return ">r dup " + province_to_owner() + "r> swap >r dup " + state_to_owner() + " r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_owned_by_state_this_state) {
	return ">r dup " + state_to_owner() + "r> swap >r dup " + state_to_owner() + " r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_owned_by_state_this_pop) {
	return ">r dup " + pop_to_owner() + "r> swap >r dup " + state_to_owner() + " r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_exists_bool) {
	return "dup owned_province_count @ >i32 0 <> " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_exists_tag) {
	return std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + "owned_province_count @ >i32 0 <> " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_country_flag) {
	return "dup " + std::to_string(trigger::payload(tval[1]).natf_id.index()) + " >national_flag_id flag_variables @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_country_flag_pop) {
	return "dup " + pop_to_owner() + std::to_string(trigger::payload(tval[1]).natf_id.index()) + " >national_flag_id flag_variables @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_country_flag_province) {
	return "dup " + province_to_owner() + std::to_string(trigger::payload(tval[1]).natf_id.index()) + " >national_flag_id flag_variables @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_country_flag_state) {
	return "dup " + state_to_owner() + std::to_string(trigger::payload(tval[1]).natf_id.index()) + " >national_flag_id flag_variables @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_continent_province) {
	return "dup continent @ >index " + std::to_string(trigger::payload(tval[1]).mod_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_continent_state) {
	return "dup capital @ continent @ >index " + std::to_string(trigger::payload(tval[1]).mod_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_continent_nation) {
	return "dup capital @ continent @ >index " + std::to_string(trigger::payload(tval[1]).mod_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_continent_pop) {
	return "dup " + pop_to_location() + "continent @ >index " + std::to_string(trigger::payload(tval[1]).mod_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_continent_nation_this) {
	return ">r dup capital @ continent @ r> swap >r dup capital @ continent @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_continent_state_this) {
	return ">r dup capital @ continent @ r> swap >r dup capital @ continent @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_continent_province_this) {
	return ">r dup capital @ continent @ r> swap >r dup continent @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_continent_pop_this) {
	return ">r dup capital @ continent @ r> swap >r dup " + pop_to_location() + "continent @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_continent_nation_from) {
	return ">r >r dup capital @ continent @ r> swap r> swap >r dup capital @ continent @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_continent_state_from) {
	return ">r >r dup capital @ continent @ r> swap r> swap >r dup capital @ continent @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_continent_province_from) {
	return ">r >r dup capital @ continent @ r> swap r> swap >r dup continent @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_continent_pop_from) {
	return ">r >r dup capital @ continent @ r> swap r> swap >r dup " + pop_to_location() + "continent @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_casus_belli_tag) {
	return "dup >index " +  std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + ">index state-ptr @ has-cb-against " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_casus_belli_from) {
	return ">r >r dup r> swap r> swap >r dup >index r> >index state-ptr @ has-cb-against " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_casus_belli_this_nation) {
	return ">r >dup r> swap >r dup >index r> >index state-ptr @ has-cb-against " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_casus_belli_this_state) {
	return ">r >dup " + state_to_owner() + "r> swap >r dup >index r> >index state-ptr @ has-cb-against " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_casus_belli_this_province) {
	return ">r >dup " + province_to_owner() + "r> swap >r dup >index r> >index state-ptr @ has-cb-against " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_casus_belli_this_pop) {
	return ">r >dup " + pop_to_owner() + "r> swap >r dup >index r> >index state-ptr @ has-cb-against " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_military_access_tag) {
	return "dup >index " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + ">index state-ptr @ has-access-with " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_military_access_from) {
	return ">r >r dup >index r> swap r> swap >r dup >index r> state-ptr @ has-access-with " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_military_access_this_nation) {
	return ">r dup >index r> swap >r dup >index r> state-ptr @ has-access-with " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_military_access_this_state) {
	return ">r dup " + state_to_owner() + ">index r> swap >r dup >index r> state-ptr @ has-access-with " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_military_access_this_province) {
	return ">r dup " + province_to_owner() + ">index r> swap >r dup >index r> state-ptr @ has-access-with " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_military_access_this_pop) {
	return ">r dup " + pop_to_owner() + ">index r> swap >r dup >index r> state-ptr @ has-access-with " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_prestige_value) {
	return "dup prestige @ " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_prestige_from) {
	return ">r >r dup prestige @ r> swap r> swap >r dup prestige @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_prestige_this_nation) {
	return ">r dup prestige @ r> swap >r dup prestige @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_prestige_this_state) {
	return ">r dup " + state_to_owner()  + "prestige @ r> swap >r dup prestige @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_prestige_this_province) {
	return ">r dup " + province_to_owner() + "prestige @ r> swap >r dup prestige @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_prestige_this_pop) {
	return ">r dup " + pop_to_owner() + "prestige @ r> swap >r dup prestige @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_badboy) {
	return "dup infamy @ " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_has_building_fort) {
	return "dup " + std::to_string(int32_t(economy::province_building_type::fort)) + " >ui8 building_level 0 <> " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_building_railroad) {
	return "dup " + std::to_string(int32_t(economy::province_building_type::railroad)) + " >ui8 building_level 0 <> " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_building_naval_base) {
	return "dup " + std::to_string(int32_t(economy::province_building_type::naval_base)) + " >ui8 building_level 0 <> " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_building_factory) {
	return "dup >index state-ptr @ has-any-factory " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_building_state) {
	return "dup >index " + std::to_string(trigger::payload(tval[1]).fac_id.index()) + " state-ptr @ has-factory " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_building_state_from_province) {
	return "dup state_membership @ >index " + std::to_string(trigger::payload(tval[1]).fac_id.index()) + " state-ptr @ has-factory " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_building_factory_from_province) {
	return "dup state_membership @ >index state-ptr @ has-any-factory " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_empty) {
	return "dup " + province_to_owner() + "make nation_id = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_empty_state) {
	return "dup make state_instance_id = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_blockaded) {
	return "dup is_blockaded @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_country_modifier) {
	return "dup >index " + std::to_string(trigger::payload(tval[1]).mod_id.index()) + " state-ptr @ has-nmod " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_country_modifier_province) {
	return "dup " + province_to_owner() + ">index " + std::to_string(trigger::payload(tval[1]).mod_id.index()) + " state-ptr @ has-nmod " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_province_modifier) {
	return "dup >index " + std::to_string(trigger::payload(tval[1]).mod_id.index()) + " state-ptr @ has-pmod " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_region) {
	return "dup abstract_state_membership-province state @ >index " + std::to_string(trigger::payload(tval[1]).state_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_region_state) {
	return "dup definition @ >index " + std::to_string(trigger::payload(tval[1]).state_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_region_pop) {
	return "dup " + pop_to_location() + "abstract_state_membership-province state @ >index " + std::to_string(trigger::payload(tval[1]).state_id.index()) + " " + compare_values_eq(tval[0]);
}

TRIGGER_FUNCTION(tf_region_proper) {
	return "dup >index " + std::to_string(trigger::payload(tval[1]).reg_id.index()) + " state-ptr @ in-region " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_region_proper_state) {
	return "dup capital @ >index " + std::to_string(trigger::payload(tval[1]).reg_id.index()) + " state-ptr @ in-region " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_region_proper_pop) {
	return "dup " + pop_to_location()  + ">index " + std::to_string(trigger::payload(tval[1]).reg_id.index()) + " state-ptr @ in-region " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_owns_region_proper) {
	return "dup >index " + std::to_string(trigger::payload(tval[1]).reg_id.index()) + " state-ptr @ owns-region " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_owns_region) {
	return "dup >index " + std::to_string(trigger::payload(tval[1]).state_id.index()) + " state-ptr @ owns-state " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_tag_tag) {
	return "dup " + nation_to_tag() + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_tag_this_nation) {
	return "2dup = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_tag_this_province) {
	return ">r dup " + province_to_owner() + " r> swap >r dup r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_tag_from_nation) {
	return ">r >r dup r> swap r> swap >r dup r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_tag_from_province) {
	return ">r >r dup " + province_to_owner() + " r> swap r> swap >r dup r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_tag_pop) {
	return "dup " + pop_to_owner() + nation_to_tag() + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_stronger_army_than_tag) {
	return "dup active_regiments @ " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + "active_regiments @ " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_stronger_army_than_this_nation) {
	return ">r dup active_regiments @ r> swap >r dup active_regiments @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_stronger_army_than_this_state) {
	return ">r dup " + state_to_owner() + "active_regiments @ r> swap >r dup active_regiments @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_stronger_army_than_this_province) {
	return ">r dup " + province_to_owner() + "active_regiments @ r> swap >r dup active_regiments @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_stronger_army_than_this_pop) {
	return ">r dup " + pop_to_owner() + "active_regiments @ r> swap >r dup active_regiments @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_stronger_army_than_from_nation) {
	return ">r >r dup active_regiments @ r> swap r> swap >r dup active_regiments @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_stronger_army_than_from_province) {
	return ">r >r dup " + province_to_owner() + "active_regiments @ r> swap r> swap >r dup active_regiments @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_neighbour_tag) {
	return "dup >index " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + " >index state-ptr @ nation-a " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_neighbour_this) {
	return ">r dup >index r> swap >r dup >index r> state-ptr @ nation-a " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_neighbour_from) {
	return ">r >r dup >index r> swap r> swap >r dup >index r> state-ptr @ nation-a " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_neighbour_this_province) {
	return ">r dup " + province_to_owner() + ">index r> swap >r dup >index r> state-ptr @ nation-a " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_neighbour_from_province) {
	return ">r >r dup " + province_to_owner() + ">index r> swap r> swap >r dup >index r> state-ptr @ nation-a " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_country_units_in_state_from) {
	return ">r >r dup >index r> swap r> swap >r dup >index r> swap state-ptr @ units-in-state " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_country_units_in_state_this_nation) {
	return ">r dup >index r> swap >r dup >index r> swap state-ptr @ units-in-state " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_country_units_in_state_this_province) {
	return ">r dup " + province_to_owner() + ">index r> swap >r dup >index r> swap state-ptr @ units-in-state " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_country_units_in_state_this_state) {
	return ">r dup " + state_to_owner() + ">index r> swap >r dup >index r> swap state-ptr @ units-in-state " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_country_units_in_state_this_pop) {
	return ">r dup " + pop_to_owner() + ">index r> swap >r dup >index r> swap state-ptr @ units-in-state " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_country_units_in_state_tag) {
	return std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + ">index >r dup >index r> swap state-ptr @ units-in-state " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_units_in_province_value) {
	return "dup >index state-ptr @ count-p-units " + std::to_string(int32_t(tval[1])) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_units_in_province_tag) {
	return "dup >index >r " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + ">index r> state-ptr @ units-in-province " + truth_inversion(tval[0]);
}

TRIGGER_FUNCTION(tf_units_in_province_from) {
	return "dup >index >r >r >r dup >index r> swap r> swap r> state-ptr @ units-in-province " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_units_in_province_this_nation) {
	return "dup >index >r >r dup >index r> swap r> state-ptr @ units-in-province " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_units_in_province_this_province) {
	return "dup >index >r >r dup " + province_to_owner() + ">index r> swap r> state-ptr @ units-in-province " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_units_in_province_this_state) {
	return "dup >index >r >r dup " + state_to_owner() + ">index r> swap r> state-ptr @ units-in-province " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_units_in_province_this_pop) {
	return "dup >index >r >r dup " + pop_to_owner() + ">index r> swap r> state-ptr @ units-in-province " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_war_with_tag) {
	return "dup >index " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation()  + ">index state-ptr @ at-war? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_war_with_from) {
	return ">r >r dup >index r> swap r> swap >r dup >index r> state-ptr @ at-war? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_war_with_this_nation) {
	return ">r dup >index r> swap >r dup >index r> state-ptr @ at-war? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_war_with_this_province) {
	return ">r dup " + province_to_owner() + ">index r> swap >r dup >index r> state-ptr @ at-war? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_war_with_this_state) {
	return ">r dup " + state_to_owner() + ">index r> swap >r dup >index r> state-ptr @ at-war? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_war_with_this_pop) {
	return ">r dup " + pop_to_owner() + ">index r> swap >r dup >index r> state-ptr @ at-war? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_unit_in_battle) {
	return "dup land_battle_location-location size 0 <> " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_unit_has_leader) {
	return "dup >index state-ptr @ general-in-province? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_national_focus_state) {
	return "dup owner_focus @ >index " + std::to_string(trigger::payload(tval[1]).nf_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_has_national_focus_province) {
	return "dup state_membership @ owner_focus @ >index " + std::to_string(trigger::payload(tval[1]).nf_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_total_amount_of_divisions) {
	return "dup active_regiments @ >i32 " + std::to_string(int32_t(tval[1])) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_money) {
	return "dup 0 >commodity_id @ "  + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_money_province) {
	return "dup " + province_to_owner() + "0 >commodity_id @ " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_suppression_points) {
	return "dup suppression @ " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_lost_national) {
	return "dup revanchism @ 1.0 swap - " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_is_vassal) {
	return "dup overlord-subject ruler @ valid? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_ruling_party_ideology_nation) {
	return "dup ruling_party @ ideology @ >index " + std::to_string(trigger::payload(tval[1]).ideo_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_ruling_party_ideology_pop) {
	return "dup " + pop_to_owner() + "ruling_party @ ideology @ >index " + std::to_string(trigger::payload(tval[1]).ideo_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_ruling_party_ideology_province) {
	return "dup " + province_to_owner() + "ruling_party @ ideology @ >index " + std::to_string(trigger::payload(tval[1]).ideo_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_ruling_party) {
	return "dup ruling_party @ name @ >index " + std::to_string(read_int32_t_from_payload(tval + 1)) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_is_ideology_enabled) {
	return std::to_string(trigger::payload(tval[1]).ideo_id.index()) + " >ideology_id enabled @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_political_reform_want_nation) {
	return "dup dup >r " + std::to_string(demographics::political_reform_desire.index()) + " >demographics_key demographics @ " + std::to_string(ws.defines.movement_support_uh_factor) + " * r> non_colonial_population @ / " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_political_reform_want_pop) {
	return "dup upolitical_reform_desire @  >f32 " + std::to_string(1.0f / float(std::numeric_limits<uint8_t>::max())) + " * " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_social_reform_want_nation) {
	return "dup dup >r " + std::to_string(demographics::social_reform_desire.index()) + " >demographics_key demographics @ " + std::to_string(ws.defines.movement_support_uh_factor) + " * r> non_colonial_population @ / " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_social_reform_want_pop) {
	return "dup usocial_reform_desire @  >f32 " + std::to_string(1.0f / float(std::numeric_limits<uint8_t>::max())) + " * " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_total_amount_of_ships) {
	return "dup >index state-ptr @ count-ships " + std::to_string(int32_t(tval[1])) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_plurality) {
	return "dup plurality @ " + std::to_string(read_float_from_payload(tval + 1)) + " 100.0 / " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_plurality_pop) {
	return "dup " + pop_to_owner() + "plurality @ " + std::to_string(read_float_from_payload(tval + 1)) + " 100.0 / " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_corruption) {
	return "dup central_province_count @ >f32 >r dup central_crime_count @ >f32 r@ / 0.0 r> 0.0 <= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_is_state_religion_pop) {
	return " dup >r r@ " + pop_to_owner() + "religion @ r> religion @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_state_religion_province) {
	return " dup >r r@ " + province_to_owner() + "religion @ r> dominant_religion @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_state_religion_state) {
	return " dup >r r@ " + state_to_owner() + "religion @ r> dominant_religion @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_primary_culture_pop) {
	return " dup >r r@ " + pop_to_owner() + "primary_culture @ r> culture @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_primary_culture_province) {
	return " dup >r r@ " + province_to_owner() + "primary_culture @ r> dominant_culture @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_primary_culture_state) {
	return " dup >r r@ " + state_to_owner() + "primary_culture @ r> dominant_culture @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_primary_culture_from_nation) {
	return ">r >r dup r> swap r> swap >r dup primary_culture @ r> primary_culture @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_primary_culture_from_province) {
	return ">r >r dup " + province_to_owner() + "r> swap r> swap >r dup primary_culture @ r> primary_culture @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_primary_culture_nation_this_nation) {
	return ">r dup r> swap >r dup primary_culture @ r> primary_culture @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_primary_culture_nation_this_pop) {
	return ">r dup culture @ r> swap >r dup primary_culture @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_primary_culture_nation_this_state) {
	return ">r dup " + state_to_owner() + "r> swap >r dup primary_culture @ r> primary_culture @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_primary_culture_nation_this_province) {
	return ">r dup " + province_to_owner() + "r> swap >r dup primary_culture @ r> primary_culture @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_primary_culture_state_this_nation) {
	return ">r dup r> swap >r dup dominant_culture @ r> primary_culture @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_primary_culture_state_this_pop) {
	return ">r dup " + pop_to_owner() + "r> swap >r dup dominant_culture @ r> primary_culture @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_primary_culture_state_this_state) {
	return ">r dup " + state_to_owner() + "r> swap >r dup dominant_culture @ r> primary_culture @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_primary_culture_state_this_province) {
	return ">r dup " + province_to_owner() + "r> swap >r dup dominant_culture @ r> primary_culture @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_primary_culture_province_this_nation) {
	return ">r dup r> swap >r dup dominant_culture @ r> primary_culture @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_primary_culture_province_this_pop) {
	return ">r dup " + pop_to_owner() + "r> swap >r dup dominant_culture @ r> primary_culture @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_primary_culture_province_this_state) {
	return ">r dup " + state_to_owner() + "r> swap >r dup dominant_culture @ r> primary_culture @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_primary_culture_province_this_province) {
	return ">r dup " + province_to_owner() + "r> swap >r dup dominant_culture @ r> primary_culture @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_primary_culture_pop_this_nation) {
	return ">r dup r> swap >r dup culture @ r> primary_culture @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_primary_culture_pop_this_pop) {
	return ">r dup " + pop_to_owner()  + "r> swap >r dup culture @ r> primary_culture @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_primary_culture_pop_this_state) {
	return ">r dup " + state_to_owner() + "r> swap >r dup culture @ r> primary_culture @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_primary_culture_pop_this_province) {
	return ">r dup " + province_to_owner() + "r> swap >r dup culture @ r> primary_culture @ = " + truth_inversion(tval[0]);
}

TRIGGER_FUNCTION(tf_is_accepted_culture_nation_this_nation) {
	return ">r dup primary_culture @ r> swap >r dup r> accepted_cultures @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_accepted_culture_nation_this_pop) {
	return ">r dup culture @ r> swap >r dup r> accepted_cultures @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_accepted_culture_nation_this_state) {
	return ">r dup " + state_to_owner() + "primary_culture @ r> swap >r dup r> accepted_cultures @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_accepted_culture_nation_this_province) {
	return ">r dup " + province_to_owner() + "primary_culture @ r> swap >r dup r> accepted_cultures @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_accepted_culture_state_this_nation) {
	return ">r dup primary_culture @ r> swap >r dup " + state_to_owner()  + "r> accepted_cultures @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_accepted_culture_state_this_pop) {
	return ">r dup " + pop_to_owner() + "primary_culture @ r> swap >r dup " + state_to_owner() + "r> accepted_cultures @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_accepted_culture_state_this_state) {
	return ">r dup " + state_to_owner() + "primary_culture @ r> swap >r dup " + state_to_owner() + "r> accepted_cultures @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_accepted_culture_state_this_province) {
	return ">r dup " + province_to_owner() + "primary_culture @ r> swap >r dup " + state_to_owner() + "r> accepted_cultures @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_accepted_culture_province_this_nation) {
	return ">r dup primary_culture @ r> swap >r dup " + province_to_owner() + "r> accepted_cultures @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_accepted_culture_province_this_pop) {
	return ">r dup " + pop_to_owner() + "primary_culture @ r> swap >r dup " + province_to_owner() + "r> accepted_cultures @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_accepted_culture_province_this_state) {
	return ">r dup " + state_to_owner() + "primary_culture @ r> swap >r dup " + province_to_owner() + "r> accepted_cultures @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_accepted_culture_province_this_province) {
	return ">r dup " + province_to_owner() + "primary_culture @ r> swap >r dup " + province_to_owner() + "r> accepted_cultures @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_accepted_culture_pop_this_nation) {
	return ">r dup r> swap >r dup culture r> swap accepted_cultures @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_accepted_culture_pop_this_pop) {
	return ">r dup " + pop_to_owner() + "r> swap >r dup culture r> swap accepted_cultures @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_accepted_culture_pop_this_state) {
	return ">r dup " + state_to_owner() + "r> swap >r dup culture r> swap accepted_cultures @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_accepted_culture_pop_this_province) {
	return ">r dup " + province_to_owner() + "r> swap >r dup culture r> swap accepted_cultures @ " + truth_inversion(tval[0]);
}

TRIGGER_FUNCTION(tf_is_accepted_culture_pop) {
	return "dup dup culture @ >r " + pop_to_owner() + " r> accepted_cultures @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_accepted_culture_province) {
	return "dup dup dominant_culture @ >r " + province_to_owner() + " r> accepted_cultures @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_accepted_culture_state) {
	return "dup dup dominant_culture @ >r " + state_to_owner() + " r> accepted_cultures @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_coastal_province) {
	return "dup is_coast @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_coastal_state) {
	return "dup >index state-ptr @ coastal-s? @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_in_sphere_tag) {
	return "dup in_sphere_of @ " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + "= " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_in_sphere_from) {
	return ">r >r dup r> swap r> swap >r dup in_sphere_of @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_in_sphere_this_nation) {
	return ">r dup r> swap >r dup in_sphere_of @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_in_sphere_this_province) {
	return ">r dup " + province_to_owner() + "r> swap >r dup in_sphere_of @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_in_sphere_this_state) {
	return ">r dup " + state_to_owner() + "r> swap >r dup in_sphere_of @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_in_sphere_this_pop) {
	return ">r dup " + pop_to_owner() + "r> swap >r dup in_sphere_of @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_produces_nation) {
	// TODO: adjust to local economy
	return "dup " + std::to_string(trigger::payload(tval[1]).com_id.index()) + " >commodity_id domestic_market_pool 0.0 > " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_produces_province) {
	return "dup rgo @ >index " + std::to_string(trigger::payload(tval[1]).com_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_produces_state) {
	return "dup >index " + std::to_string(trigger::payload(tval[1]).com_id.index()) + " state-ptr @ state-produces? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_produces_pop) {
	return "false " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_average_militancy_nation) {
	return "dup dup >r " + std::to_string(demographics::militancy.index()) + " >demographics_key demographics @ r> " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ dup >r / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_average_militancy_state) {
	return "dup dup >r " + std::to_string(demographics::militancy.index()) + " >demographics_key demographics @ r> " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ dup >r / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_average_militancy_province) {
	return "dup dup >r " + std::to_string(demographics::militancy.index()) + " >demographics_key demographics @ r> " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ dup >r / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_average_consciousness_nation) {
	return "dup dup >r " + std::to_string(demographics::consciousness.index()) + " >demographics_key demographics @ r> " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ dup >r / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_average_consciousness_state) {
	return "dup dup >r " + std::to_string(demographics::consciousness.index()) + " >demographics_key demographics @ r> " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ dup >r / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_average_consciousness_province) {
	return "dup dup >r " + std::to_string(demographics::consciousness.index()) + " >demographics_key demographics @ r> " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ dup >r / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_is_next_reform_nation) {
	return "dup " + std::to_string(trigger::payload(tval[1]).opt_id.index())  + " >issue_option_id parent_issue @ issues @ >index " + std::to_string(trigger::payload(tval[1]).opt_id.index() - 1) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_is_next_reform_pop) {
	return "dup " + pop_to_owner() + std::to_string(trigger::payload(tval[1]).opt_id.index()) + " >issue_option_id parent_issue @ issues @ >index " + std::to_string(trigger::payload(tval[1]).opt_id.index() - 1) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_is_next_rreform_nation) {
	return "dup " + std::to_string(trigger::payload(tval[1]).ropt_id.index()) + " >reform_option_id parent_reform @ reforms @ >index " + std::to_string(trigger::payload(tval[1]).ropt_id.index() - 1) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_is_next_rreform_pop) {
	return "dup " + pop_to_owner() + std::to_string(trigger::payload(tval[1]).ropt_id.index()) + " >reform_option_id parent_reform @ reforms @ >index " + std::to_string(trigger::payload(tval[1]).ropt_id.index() - 1) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_rebel_power_fraction) {
	return "false " + truth_inversion(tval[0]);
	// note: virtually unused
}
TRIGGER_FUNCTION(tf_recruited_percentage_nation) {
	return "dup dup >r active_regiments @ >f32 r> recruitable_regiments @ >f32 dup >r / 1.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_recruited_percentage_pop) {
	return "dup " + pop_to_owner() + "dup >r active_regiments @ >f32 r> recruitable_regiments @ >f32 dup >r / 1.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_has_culture_core) {
	return "dup dup >r " + pop_to_location() + ">index r> culture >index state-ptr @ has-culture-core? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_culture_core_province_this_pop) {
	return ">r dup culture @ >index r> swap >r dup >index r> state-ptr @ has-culture-core? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_nationalism) {
	return "dup nationalism @ " + std::to_string(float(tval[1])) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_is_overseas) {
	return "dup dup >r " + province_to_owner() + "capital @ dup continent @ r@ continent @ = not r> swap >r >r "
		"connected_region_id @ r> connected_region_id @ <> r> and " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_overseas_pop) {
	return "dup " + pop_to_location() + "dup >r " + province_to_owner() + "capital @ dup continent @ r@ continent @ = not r> swap >r >r "
		"connected_region_id @ r> connected_region_id @ <> r> and " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_overseas_state) {
	return "dup capital @ dup >r " + province_to_owner() + "capital @ dup continent @ r@ continent @ = not r> swap >r >r "
		"connected_region_id @ r> connected_region_id @ <> r> and " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_controlled_by_rebels) {
	return "dup province_rebel_control-province rebel_faction @ valid? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_controlled_by_tag) {
	return "dup " + province_to_controller() + nation_to_tag() + ">index " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_controlled_by_from) {
	return ">r >r dup r> swap r> swap >r dup " + province_to_controller() + "r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_controlled_by_this_nation) {
	return ">r dup r> swap >r dup " + province_to_controller() + "r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_controlled_by_this_province) {
	return ">r dup " + province_to_owner() + "r> swap >r dup " + province_to_controller() + "r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_controlled_by_this_state) {
	return ">r dup " + state_to_owner() + "r> swap >r dup " + province_to_controller() + "r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_controlled_by_this_pop) {
	return ">r dup " + pop_to_owner() + "r> swap >r dup " + province_to_controller() + "r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_controlled_by_owner) {
	return  "dup dup >r " + province_to_owner() + "r> " + province_to_controller() + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_controlled_by_reb) {
	return ">r >r dup r> swap r> swap >r dup province_rebel_control-province rebel_faction @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_canal_enabled) {
	return std::to_string(tval[1] - 1) + " state-ptr @ canal-enabled? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_state_capital) {
	return "dup dup >r state_membership @ capital @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_state_capital_pop) {
	return "dup " + pop_to_location() + "dup >r state_membership @ capital @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_truce_with_tag) {
	return "dup >index " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + ">index state-ptr @ truce-with? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_truce_with_from) {
	return ">r >r dup r> swap r> swap >r dup >index r> >index state-ptr @ truce-with? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_truce_with_this_nation) {
	return ">r dup r> swap >r dup >index r> >index state-ptr @ truce-with? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_truce_with_this_province) {
	return ">r dup " + province_to_owner()  + "r> swap >r dup >index r> >index state-ptr @ truce-with? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_truce_with_this_state) {
	return ">r dup " + state_to_owner() + "r> swap >r dup >index r> >index state-ptr @ truce-with? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_truce_with_this_pop) {
	return ">r dup " + pop_to_owner() + "r> swap >r dup >index r> >index state-ptr @ truce-with? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_total_pops_nation) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_total_pops_state) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_total_pops_province) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_total_pops_pop) {
	return "dup " + pop_to_location() + std::to_string(demographics::total.index()) + " >demographics_key demographics @ " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_type_nation) {
	return "dup " + std::to_string(demographics::count_special_keys + trigger::payload(tval[1]).popt_id.index()) + " >demographics_key demographics @ 0.0 > " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_type_state) {
	return "dup " + std::to_string(demographics::count_special_keys + trigger::payload(tval[1]).popt_id.index()) + " >demographics_key demographics @ 0.0 > " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_type_province) {
	return "dup " + std::to_string(demographics::count_special_keys + trigger::payload(tval[1]).popt_id.index()) + " >demographics_key demographics @ 0.0 > " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_type_pop) {
	return "dup poptype @ >index " + std::to_string(trigger::payload(tval[1]).popt_id.index()) + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_empty_adjacent_province) {
	return "dup >index state-ptr @ empty-a-province " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_leader) {
	return "dup >index " + std::to_string(read_int32_t_from_payload(tval + 1)) + " state-ptr @ has-named-leader? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_ai) {
	return "dup is_player_controlled @ not " + truth_inversion(tval[0]);
}
//can-release?
//national_identity-size
TRIGGER_FUNCTION(tf_can_create_vassals) {
	return "dup >index >r false 0"
		"while "
			">r dup not r> swap >r "
			"dup national_identity-size < r> and "
		"loop "
			"dup r@ swap state-ptr @ can-release? "
			">r swap drop r> swap "
			"1 + "
		"end-while "
		"r> drop drop "
		+ truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_possible_vassal) {
	return "dup >index " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " state-ptr @ can-release? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_province_id) {
	return "dup >index " + std::to_string(trigger::payload(tval[1]).prov_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_vassal_of_tag) {
	return "dup overlord-subject ruler @ " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + "= " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_vassal_of_from) {
	return ">r >r dup r> swap r> swap >r dup overlord-subject ruler @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_vassal_of_this_nation) {
	return ">r dup r> swap >r dup overlord-subject ruler @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_vassal_of_this_province) {
	return ">r dup " + province_to_owner() + "r> swap >r dup overlord-subject ruler @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_vassal_of_this_state) {
	return ">r dup " + state_to_owner() + "r> swap >r dup overlord-subject ruler @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_vassal_of_this_pop) {
	return ">r dup " + pop_to_owner() + "r> swap >r dup overlord-subject ruler @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_vassal_of_province_tag) {
	return "dup " + province_to_owner() + "overlord-subject ruler @ " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + "= " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_vassal_of_province_from) {
	return ">r >r dup r> swap r> swap >r dup " + province_to_owner() + "overlord-subject ruler @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_vassal_of_province_this_nation) {
	return ">r dup r> swap >r dup " + province_to_owner() + "overlord-subject ruler @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_vassal_of_province_this_province) {
	return ">r dup " + province_to_owner() + "r> swap >r dup " + province_to_owner() + "overlord-subject ruler @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_vassal_of_province_this_state) {
	return ">r dup " + state_to_owner() + "r> swap >r dup " + province_to_owner() + "overlord-subject ruler @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_vassal_of_province_this_pop) {
	return ">r dup " + pop_to_owner() + "r> swap >r dup " + province_to_owner() + "overlord-subject ruler @ = " + truth_inversion(tval[0]);
}

TRIGGER_FUNCTION(tf_substate_of_tag) {
	return "dup overlord-subject ruler @ " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + "= "
		">r dup is_substate @ r> and " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_substate_of_from) {
	return ">r >r dup r> swap r> swap >r dup overlord-subject ruler @ = >r dup is_substate @ r> and " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_substate_of_this_nation) {
	return ">r dup r> swap >r dup overlord-subject ruler @ = >r dup is_substate @ r> and " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_substate_of_this_province) {
	return ">r dup " + province_to_owner() + "r> swap >r dup overlord-subject ruler @ = >r dup is_substate @ r> and " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_substate_of_this_state) {
	return ">r dup " + state_to_owner() + "r> swap >r dup overlord-subject ruler @ = >r dup is_substate @ r> and " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_substate_of_this_pop) {
	return ">r dup " + pop_to_owner() + "r> swap >r dup overlord-subject ruler @ = >r dup is_substate @ r> and " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_alliance_with_tag) {
	return "dup >index " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity " + tag_to_nation() + "state-ptr @ are-allied? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_alliance_with_from) {
	return ">r >r dup >index r> swap r> swap >r dup >index r> state-ptr @ are-allied? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_alliance_with_this_nation) {
	return ">r dup >index r> swap >r dup >index r> state-ptr @ are-allied? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_alliance_with_this_province) {
	return ">r dup " + province_to_owner() + ">index r> swap >r dup >index r> state-ptr @ are-allied? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_alliance_with_this_state) {
	return ">r dup " + state_to_owner() + ">index r> swap >r dup >index r> state-ptr @ are-allied? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_alliance_with_this_pop) {
	return ">r dup " + pop_to_owner() + ">index r> swap >r dup >index r> state-ptr @ are-allied? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_recently_lost_war) {
	return "dup last_war_loss ptr-cast ptr(u16) @ >i32 dup 0 <> >r 365 5 * + current-date @ >i32 >= r> and " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_recently_lost_war_pop) {
	return "dup " + pop_to_owner() + "last_war_loss ptr-cast ptr(u16) @ >i32 dup 0 <> >r 365 5 * + current-date @ >i32 >= r> and " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_mobilised) {
	return "dup is_mobilized @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_mobilisation_size) {
	return "dup " + std::to_string(sys::national_mod_offsets::mobilization_size.index()) + " >national_modifier_value modifier_values @ " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_crime_higher_than_education_nation) {
	return "dup dup >r administrative_spending @ r> education_spending @ >= " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_crime_higher_than_education_state) {
	return "dup " + state_to_owner() + "dup >r administrative_spending @ r> education_spending @ >= " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_crime_higher_than_education_province) {
	return "dup " + province_to_owner() + "dup >r administrative_spending @ r> education_spending @ >= " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_crime_higher_than_education_pop) {
	return "dup " + pop_to_owner() + "dup >r administrative_spending @ r> education_spending @ >= " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_agree_with_ruling_party) {
	return "dup dup >r " + pop_to_owner() + "ruling_party @ ideology @ dup valid? if >index " + std::to_string(pop_demographics::count_special_keys) + " + >pop_demographics_key  "
		"r> dup size @ >r swap udemographics @ >f32 " + std::to_string(1.0f / float(std::numeric_limits<uint8_t>::max())) + " * r@ / 0.0 0.0 r> >= select else drop r> drop 0.0 end-if " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_is_colonial_state) {
	return "dup capital @ is_colonial @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_colonial_province) {
	return "dup is_colonial @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_colonial_pop) {
	return "dup " + pop_to_location() + "is_colonial @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_factories_state) {
	return "dup >index state-ptr @ has-any-factory " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_factories_nation) {
	return "dup >index state-ptr @ nation-has-any-factory " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_in_default_bool) {
	return "dup is_bankrupt @ " + truth_inversion(tval[0]);
}
//bankrupt-debtor-to?
TRIGGER_FUNCTION(tf_in_default_tag) {
	return "dup >index " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + "state-ptr @ bankrupt-debtor-to? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_in_default_from) {
	return ">r >r dup >index r> swap r> swap >r dup >index r> state-ptr @ bankrupt-debtor-to? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_in_default_this_nation) {
	return ">r dup >index r> swap >r dup >index r> state-ptr @ bankrupt-debtor-to? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_in_default_this_province) {
	return ">r dup " + province_to_owner() + ">index r> swap >r dup >index r> state-ptr @ bankrupt-debtor-to? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_in_default_this_state) {
	return ">r dup " + state_to_owner() + ">index r> swap >r dup >index r> state-ptr @ bankrupt-debtor-to? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_in_default_this_pop) {
	return ">r dup " + pop_to_owner() + ">index r> swap >r dup >index r> state-ptr @ bankrupt-debtor-to? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_total_num_of_ports) {
	return "dup total_ports @ >i32 " + std::to_string(tval[1]) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_always) {
	return "true " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_election) {
	return "dup election_ends ptr-cast ptr(u16) @ >i32 dup 0 <> swap current-date @ >i32 > and " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_global_flag) {
	return std::to_string(trigger::payload(tval[1]).glob_id.index()) + " state-ptr @ global-flag-set? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_capital) {
	return "dup dup " + province_to_owner() + "capital @ = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_nationalvalue_nation) {
	return "dup national_value @ >index " + std::to_string(trigger::payload(tval[1]).mod_id.index()) + " " + compare_values_eq(tval[0]) ;
}
TRIGGER_FUNCTION(tf_nationalvalue_pop) {
	return "dup " + pop_to_owner() + "national_value @ >index " + std::to_string(trigger::payload(tval[1]).mod_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_nationalvalue_province) {
	return "dup " + province_to_owner() + "national_value @ >index " + std::to_string(trigger::payload(tval[1]).mod_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_industrial_score_value) {
	return "dup industrial_score @ >i32 " + std::to_string(tval[1]) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_industrial_score_tag) {
	return "dup industrial_score @ " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + "industrial_score @ " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_industrial_score_from_nation) {
	return ">r >r dup industrial_score @ r> swap r> swap >r dup industrial_score @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_industrial_score_this_nation) {
	return ">r dup industrial_score @ r> swap >r dup industrial_score @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_industrial_score_this_pop) {
	return ">r dup " + pop_to_owner() + "industrial_score @ r> swap >r dup industrial_score @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_industrial_score_this_state) {
	return ">r dup " + state_to_owner() + "industrial_score @ r> swap >r dup industrial_score @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_industrial_score_this_province) {
	return ">r dup " +province_to_owner() + "industrial_score @ r> swap >r dup industrial_score @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_military_score_value) {
	return "dup military_score @ >i32 " + std::to_string(tval[1]) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_military_score_tag) {
	return "dup military_score @ " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + "military_score @ " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_military_score_from_nation) {
	return ">r >r dup military_score @ r> swap r> swap >r dup military_score @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_military_score_this_nation) {
	return ">r dup military_score @ r> swap >r dup military_score @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_military_score_this_pop) {
	return ">r dup " + pop_to_owner() + "military_score @ r> swap >r dup military_score @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_military_score_this_state) {
	return ">r dup " + state_to_owner() + "military_score @ r> swap >r dup military_score @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_military_score_this_province) {
	return ">r dup " + province_to_owner() + "military_score @ r> swap >r dup military_score @ r> " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_civilized_nation) {
	return "dup is_civilized @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_civilized_pop) {
	return "dup " + pop_to_owner() + "is_civilized @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_civilized_province) {
	return "dup " + province_to_owner() + "is_civilized @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_national_provinces_occupied) {
	return "dup owned_province_count @ >f32 >r dup occupied_count @ >f32 r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1))  + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_is_greater_power_nation) {
	return "dup is_great_power @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_greater_power_pop) {
	return "dup " + pop_to_owner()  + "is_great_power @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_greater_power_province) {
	return "dup " + province_to_owner() + "is_great_power @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_rich_tax) {
	return "dup rich_tax @ >i32 " + std::to_string(trigger::payload(tval[1]).signed_value) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_middle_tax) {
	return "dup middle_tax @ >i32 " + std::to_string(trigger::payload(tval[1]).signed_value) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_poor_tax) {
	return "dup poor_tax @ >i32 " + std::to_string(trigger::payload(tval[1]).signed_value) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_rich_tax_pop) {
	return "dup " + pop_to_owner() + "rich_tax @ >i32 " + std::to_string(trigger::payload(tval[1]).signed_value) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_middle_tax_pop) {
	return "dup " + pop_to_owner() + "middle_tax @ >i32 " + std::to_string(trigger::payload(tval[1]).signed_value) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_poor_tax_pop) {
	return "dup " + pop_to_owner() + "poor_tax @ >i32 " + std::to_string(trigger::payload(tval[1]).signed_value) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_social_spending_nation) {
	return "dup dup >r social_spending @ >f32 r> spending_level @ * " + std::to_string(trigger::payload(tval[1]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_social_spending_pop) {
	return "dup " + pop_to_owner() + "dup >r social_spending @ >f32 r> spending_level @ * " + std::to_string(trigger::payload(tval[1]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_social_spending_province) {
	return "dup " + province_to_owner() + "dup >r social_spending @ >f32 r> spending_level @ * " + std::to_string(trigger::payload(tval[1]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_military_spending_nation) {
	return "dup dup >r military_spending @ >f32 r> spending_level @ * " + std::to_string(trigger::payload(tval[1]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_military_spending_pop) {
	return "dup " + pop_to_owner() + "dup >r military_spending @ >f32 r> spending_level @ * " + std::to_string(trigger::payload(tval[1]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_military_spending_province) {
	return "dup " + province_to_owner() + "dup >r military_spending @ >f32 r> spending_level @ * " + std::to_string(trigger::payload(tval[1]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_military_spending_state) {
	return "dup " + state_to_owner() + "dup >r military_spending @ >f32 r> spending_level @ * " + std::to_string(trigger::payload(tval[1]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_administration_spending_nation) {
	return "dup dup >r administrative_spending @ >f32 r> spending_level @ * " + std::to_string(trigger::payload(tval[1]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_administration_spending_pop) {
	return "dup " + pop_to_owner() + "dup >r administrative_spending @ >f32 r> spending_level @ * " + std::to_string(trigger::payload(tval[1]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_administration_spending_province) {
	return "dup " + province_to_owner() + "dup >r administrative_spending @ >f32 r> spending_level @ * " + std::to_string(trigger::payload(tval[1]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_administration_spending_state) {
	return "dup " + state_to_owner() + "dup >r administrative_spending @ >f32 r> spending_level @ * " + std::to_string(trigger::payload(tval[1]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_education_spending_nation) {
	return "dup dup >r education_spending @ >f32 r> spending_level @ * " + std::to_string(trigger::payload(tval[1]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_education_spending_pop) {
	return "dup " + pop_to_owner() + "dup >r education_spending @ >f32 r> spending_level @ * " + std::to_string(trigger::payload(tval[1]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_education_spending_province) {
	return "dup " + province_to_owner() + "dup >r education_spending @ >f32 r> spending_level @ * " + std::to_string(trigger::payload(tval[1]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_education_spending_state) {
	return "dup " + state_to_owner() + "dup >r education_spending @ >f32 r> spending_level @ * " + std::to_string(trigger::payload(tval[1]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_colonial_nation) {
	return "dup is_colonial_nation @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_majority_religion_nation) {
	return "dup dominant_religion @ >index " + std::to_string(trigger::payload(tval[1]).rel_id.index()) + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_majority_religion_nation_this_nation) {
	return ">r dup religion @ r> swap >r dup dominant_religion @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_majority_religion_state) {
	return "dup dominant_religion @ >index " + std::to_string(trigger::payload(tval[1]).rel_id.index()) + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_majority_religion_province) {
	return "dup dominant_religion @ >index " + std::to_string(trigger::payload(tval[1]).rel_id.index()) + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_majority_culture_nation) {
	return "dup dominant_culture @ >index " + std::to_string(trigger::payload(tval[1]).cul_id.index()) + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_majority_culture_state) {
	return "dup dominant_culture @ >index " + std::to_string(trigger::payload(tval[1]).cul_id.index()) + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_majority_culture_province) {
	return "dup dominant_culture @ >index " + std::to_string(trigger::payload(tval[1]).cul_id.index()) + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_majority_issue_nation) {
	return "dup dominant_issue_option @ >index " + std::to_string(trigger::payload(tval[1]).opt_id.index()) + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_majority_issue_state) {
	return "dup dominant_issue_option @ >index " + std::to_string(trigger::payload(tval[1]).opt_id.index()) + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_majority_issue_province) {
	return "dup dominant_issue_option @ >index " + std::to_string(trigger::payload(tval[1]).opt_id.index()) + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_majority_issue_pop) {
	return "dup dominant_issue_option @ >index " + std::to_string(trigger::payload(tval[1]).opt_id.index()) + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_majority_ideology_nation) {
	return "dup dominant_ideology @ >index " + std::to_string(trigger::payload(tval[1]).ideo_id.index()) + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_majority_ideology_state) {
	return "dup dominant_ideology @ >index " + std::to_string(trigger::payload(tval[1]).ideo_id.index()) + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_majority_ideology_province) {
	return "dup dominant_ideology @ >index " + std::to_string(trigger::payload(tval[1]).ideo_id.index()) + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_majority_ideology_pop) {
	return "dup dominant_ideology @ >index " + std::to_string(trigger::payload(tval[1]).ideo_id.index()) + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_poor_strata_militancy_nation) {
	return "dup " + std::to_string(demographics::poor_total.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::poor_militancy.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select "
		+ std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_poor_strata_militancy_state) {
	return "dup " + std::to_string(demographics::poor_total.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::poor_militancy.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select "
		+ std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_poor_strata_militancy_province) {
	return "dup " + std::to_string(demographics::poor_total.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::poor_militancy.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select "
		+ std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_poor_strata_militancy_pop) {
	return "dup poptype @ strata @ >i32 " + std::to_string(uint8_t(culture::pop_strata::poor)) + " <> >r "
		"dup umilitancy @ >f32 " + std::to_string(10.0f / float(std::numeric_limits<uint16_t>::max())) + " * 0.0 r> select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_middle_strata_militancy_nation) {
	return "dup " + std::to_string(demographics::middle_total.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::middle_militancy.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select "
		+ std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_middle_strata_militancy_state) {
	return "dup " + std::to_string(demographics::middle_total.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::middle_militancy.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select "
		+ std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_middle_strata_militancy_province) {
	return "dup " + std::to_string(demographics::middle_total.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::middle_militancy.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select "
		+ std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_middle_strata_militancy_pop) {
	return "dup poptype @ strata @ >i32 " + std::to_string(uint8_t(culture::pop_strata::middle)) + " <> >r "
		"dup umilitancy @ >f32 " + std::to_string(10.0f / float(std::numeric_limits<uint16_t>::max())) + " * 0.0 r> select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_rich_strata_militancy_nation) {
	return "dup " + std::to_string(demographics::rich_total.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::rich_militancy.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select "
		+ std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_rich_strata_militancy_state) {
	return "dup " + std::to_string(demographics::rich_total.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::rich_militancy.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select "
		+ std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_rich_strata_militancy_province) {
	return "dup " + std::to_string(demographics::rich_total.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::rich_militancy.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select "
		+ std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_rich_strata_militancy_pop) {
	return "dup poptype @ strata @ >i32 " + std::to_string(uint8_t(culture::pop_strata::rich)) + " <> >r "
		"dup umilitancy @ >f32 " + std::to_string(10.0f / float(std::numeric_limits<uint16_t>::max())) + " * 0.0 r> select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_rich_tax_above_poor) {
	return "dup dup >r rich_tax @ r> poor_tax @ > " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_has_union_tag_pop) {
	return "dup culture @ culture_group_membership-member group @ cultural_union_of-culture_group identity @ valid? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_culture_has_union_tag_nation) {
	return "dup primary_culture @ culture_group_membership-member group @ cultural_union_of-culture_group identity @ valid? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_this_culture_union_tag) {
	return "dup primary_culture @ culture_group_membership-member group @ cultural_union_of-culture_group identity @ >index " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_this_culture_union_from) {
	return ">r >r dup " + nation_to_tag() + "r> swap r> swap >r dup primary_culture @ culture_group_membership-member group @ cultural_union_of-culture_group identity @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_this_culture_union_this_nation) {
	return ">r dup " + nation_to_tag() + "r> swap >r dup primary_culture @ culture_group_membership-member group @ cultural_union_of-culture_group identity @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_this_culture_union_this_province) {
	return ">r dup " + province_to_owner() + nation_to_tag() + "r> swap >r dup primary_culture @ culture_group_membership-member group @ cultural_union_of-culture_group identity @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_this_culture_union_this_state) {
	return ">r dup " + state_to_owner() + nation_to_tag() + "r> swap >r dup primary_culture @ culture_group_membership-member group @ cultural_union_of-culture_group identity @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_this_culture_union_this_pop) {
	return ">r dup " + pop_to_owner() + nation_to_tag() + "r> swap >r dup primary_culture @ culture_group_membership-member group @ cultural_union_of-culture_group identity @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_this_culture_union_this_union_nation) {
	return ">r dup primary_culture @ culture_group_membership-member group @ r> swap >r dup primary_culture @ culture_group_membership-member group @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_this_culture_union_this_union_province) {
	return ">r dup " + province_to_owner() + "primary_culture @ culture_group_membership-member group @ r> swap >r dup primary_culture @ culture_group_membership-member group @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_this_culture_union_this_union_state) {
	return ">r dup " + state_to_owner() + "primary_culture @ culture_group_membership-member group @ r> swap >r dup primary_culture @ culture_group_membership-member group @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_this_culture_union_this_union_pop) {
	return ">r dup " + pop_to_owner() + "primary_culture @ culture_group_membership-member group @ r> swap >r dup primary_culture @ culture_group_membership-member group @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_minorities_nation) {
	return "dup dup >r " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ "
		"r@ " + std::to_string(demographics::count_special_keys + ws.world.pop_type_size() * 2) + " r@ primary_culture @ >index + >demographics_key demographics @ - "
		"0 "
		"while "
			"dup culture-size < "
		"loop "
			"dup >culture_id r@ swap accepted_cultures @ "
			"if "
				"dup r@ swap >r "
				+ std::to_string(demographics::count_special_keys + ws.world.pop_type_size() * 2) + " r> + >demographics_key demographics @ "
				">r swap r> - swap "
			"then "
			"1 + "
		"end-while "
		"drop r> drop 0.0 > "
		+ truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_minorities_state) {
	return "dup dup >r " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ "
		"r@ " + std::to_string(demographics::count_special_keys + ws.world.pop_type_size() * 2) + " r@ " + state_to_owner() + "primary_culture @ >index + >demographics_key demographics @ - "
		"0 "
		"while "
			"dup culture-size < "
		"loop "
			"dup >culture_id r@ " + state_to_owner() + "swap accepted_cultures @ "
			"if "
				"dup r@ swap >r "
				+ std::to_string(demographics::count_special_keys + ws.world.pop_type_size() * 2) + " r> + >demographics_key demographics @ "
				">r swap r> - swap "
			"then "
			"1 + "
		"end-while "
		"drop r> drop 0.0 > "
		+ truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_minorities_province) {
	return "dup dup >r " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ "
		"r@ " + std::to_string(demographics::count_special_keys + ws.world.pop_type_size() * 2) + " r@ " + province_to_owner() + "primary_culture @ >index + >demographics_key demographics @ - "
		"0 "
		"while "
			"dup culture-size < "
		"loop "
			"dup >culture_id r@ " + province_to_owner() + "swap accepted_cultures @ "
			"if "
				"dup r@ swap >r "
				+ std::to_string(demographics::count_special_keys + ws.world.pop_type_size() * 2) + " r> + >demographics_key demographics @ "
				">r swap r> - swap "
			"then "
			"1 + "
		"end-while "
		"drop r> drop 0.0 > "
		+ truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_revanchism_nation) {
	return "dup revanchism @ " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_revanchism_pop) {
	return "dup " + pop_to_owner() + "revanchism @ " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_has_crime) {
	return "dup crime @ valid? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_num_of_substates) {
	return "dup substates_count @ >i32 " + std::to_string(tval[1]) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_num_of_vassals_no_substates) {
	return "dup dup >r vassals_count @ >i32 r> substates_count @ >i32 - " + std::to_string(tval[1]) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_brigades_compare_this) {
	return ">r dup active_regiments >f32 r> swap >r dup active_regiments >f32 r@ / 1000000.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_brigades_compare_from) {
	return ">r >r dup active_regiments >f32 r> swap r> swap >r dup active_regiments >f32 r@ / 1000000.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_brigades_compare_province_this) {
	return ">r dup active_regiments >f32 r> swap >r dup " + province_to_owner() + "active_regiments >f32 r@ / 1000000.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_brigades_compare_province_from) {
	return ">r >r dup active_regiments >f32 r> swap r> swap >r dup " + province_to_owner() + "active_regiments >f32 r@ / 1000000.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_constructing_cb_tag) {
	return "dup constructing_cb_target @ " + std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_constructing_cb_from) {
	return ">r >r dup r> swap r> swap >r dup constructing_cb_target @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_constructing_cb_this_nation) {
	return ">r dup r> swap >r dup constructing_cb_target @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_constructing_cb_this_province) {
	return ">r dup " + province_to_owner() + "r> swap >r dup constructing_cb_target @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_constructing_cb_this_state) {
	return ">r dup " + state_to_owner() + "r> swap >r dup constructing_cb_target @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_constructing_cb_this_pop) {
	return ">r dup " + pop_to_owner() + "r> swap >r dup constructing_cb_target @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_constructing_cb_discovered) {
	return "dup constructing_cb_is_discovered @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_constructing_cb_progress) {
	return "dup constructing_cb_progress @ " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_civilization_progress) {
	return "dup " + std::to_string(sys::national_mod_offsets::civilization_progress_modifier.index()) + " >national_modifier_value modifier_values @ " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_constructing_cb_type) {
	return "dup constructing_cb_type @ >index " + std::to_string(trigger::payload(tval[1]).cb_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_is_our_vassal_tag) {
	return std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + "overlord-subject ruler @ >r dup r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_our_vassal_from) {
	return ">r >r dup overlord-subject ruler @ r> swap r> swap >r dup r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_our_vassal_this_nation) {
	return ">r dup overlord-subject ruler @ r> swap >r dup r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_our_vassal_this_province) {
	return ">r dup " + province_to_owner() + "overlord-subject ruler @ r> swap >r dup r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_our_vassal_this_state) {
	return ">r dup " + state_to_owner() + "overlord-subject ruler @ r> swap >r dup r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_our_vassal_this_pop) {
	return ">r dup " + pop_to_owner() + "overlord-subject ruler @ r> swap >r dup r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_our_vassal_province_tag) {
	return ">r >r dup overlord-subject ruler @ r> swap r> swap >r dup " + province_to_owner() + "r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_our_vassal_province_from) {
	return ">r >r dup overlord-subject ruler @ r> swap r> swap >r dup " + province_to_owner() + "r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_our_vassal_province_this_nation) {
	return ">r dup overlord-subject ruler @ r> swap >r dup " + province_to_owner() + "r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_our_vassal_province_this_province) {
	return ">r dup " + province_to_owner() + "overlord-subject ruler @ r> swap >r dup " + province_to_owner() + "r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_our_vassal_province_this_state) {
	return ">r dup " + state_to_owner() + "overlord-subject ruler @ r> swap >r dup " + province_to_owner() + "r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_our_vassal_province_this_pop) {
	return ">r dup " + pop_to_owner() + "overlord-subject ruler @ r> swap >r dup " + province_to_owner() + "r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_substate) {
	return "dup is_substate @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_great_wars_enabled) {
	return std::to_string(offsetof(sys::state, military_definitions) + offsetof(military::global_military_state, great_wars_enabled)) + " state-ptr @ buf-add ptr-cast ptr(i8) @ 0 >i8 <> " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_nationalize) {
	return "dup >index state-ptr @ can-nationalize? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_part_of_sphere) {
	return "dup in_sphere_of @ valid? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_sphere_leader_of_tag) {
	return std::to_string(trigger::payload(tval[1]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + "in_sphere_of @ >r dup r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_sphere_leader_of_from) {
	return ">r >r dup in_sphere_of @ r> swap r> swap >r dup r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_sphere_leader_of_this_nation) {
	return ">r dup in_sphere_of @ r> swap >r dup r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_sphere_leader_of_this_province) {
	return ">r " + province_to_owner() + "dup in_sphere_of @ r> swap >r dup r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_sphere_leader_of_this_state) {
	return ">r " + state_to_owner() + "dup in_sphere_of @ r> swap >r dup r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_sphere_leader_of_this_pop) {
	return ">r " + pop_to_owner() + "dup in_sphere_of @ r> swap >r dup r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_number_of_states) {
	return "dup owned_state_count @ >i32 " + std::to_string(tval[1]) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_war_score) {
	return "dup >index state-ptr @ avg-warscore " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_is_releasable_vassal_from) {
	return ">r >r dup " + nation_to_tag() + "is_not_releasable @ not r> swap r> swap" + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_releasable_vassal_other) {
	return ">r >r dup " + nation_to_tag() + "is_not_releasable @ not r> swap r> swap" + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_recent_imigration) {
	return "dup last_immigration ptr-cast ptr(u16) @ >i32 " + std::to_string(tval[1]) + " + current-date @ >i32 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_province_control_days) {
	return "dup last_control_change ptr-cast ptr(u16) @ >i32 " + std::to_string(tval[1]) + " + current-date @ >i32 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_is_disarmed) {
	return "dup disarmed_until ptr-cast ptr(u16) @ >i32 dup 0 <> swap current-date @ >i32 > and " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_disarmed_pop) {
	return "dup " + pop_to_owner() + "disarmed_until ptr-cast ptr(u16) @ >i32 dup 0 <> swap current-date @ >i32 > and " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_big_producer) {
	return "false " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_someone_can_form_union_tag_from) {
	return "false " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_someone_can_form_union_tag_other) {
	return "false " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_social_movement_strength) {
	return "dup >index true state-ptr @ movement-str " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_political_movement_strength) {
	return "dup >index false state-ptr @ movement-str " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_factory_in_capital_state) {
	return "true " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_social_movement) {
	return "dup pop_movement_membership-pop movement @ get_associated_issue_option @ dup  >r  valid? r> parent_issue @ issue_type @ >i32 " + std::to_string(int32_t(culture::issue_type::social)) + " = and " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_political_movement) {
	return "dup pop_movement_membership-pop movement @ get_associated_issue_option @ dup  >r  valid? r> parent_issue @ issue_type @ >i32 " + std::to_string(int32_t(culture::issue_type::political)) + " = and " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_political_movement_from_reb) {
	// returns false because I'm not sure exactly what it is supposed to do (applies to the below as well)
	// it is called in nation scope while modifying chances for a rebel faction to spawn
	// perhaps reducing the chance if there is an "appropriate" movement (because there is almost always some movement)
	// however, the logic from working backwards from rebel factions to movements is not clear to me
	return "false " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_social_movement_from_reb) {
	return "false " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_cultural_sphere) {
	return "dup >index state-ptr @ has-union-sphere? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_world_wars_enabled) {
	return std::to_string(offsetof(sys::state, military_definitions) + offsetof(military::global_military_state, world_wars_enabled)) + " state-ptr @ buf-add ptr-cast ptr(i8) @ 0 >i8 <> " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_culture_pop_this_pop) {
	return ">r dup culture @ r> swap >r dup culture r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_culture_state_this_pop) {
	return ">r dup culture @ >index r> swap >r dup r> " + std::to_string(demographics::count_special_keys + ws.world.pop_type_size() * 2) + " + >demographics_key demographics @ 0.0 > " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_culture_province_this_pop) {
	return ">r dup culture @ >index r> swap >r dup r> " + std::to_string(demographics::count_special_keys + ws.world.pop_type_size() * 2) + " + >demographics_key demographics @ 0.0 > " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_culture_nation_this_pop) {
	return ">r dup culture @ >index r> swap >r dup r> " + std::to_string(demographics::count_special_keys + ws.world.pop_type_size() * 2) + " + >demographics_key demographics @ 0.0 > " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_culture_pop) {
	return "dup culture @ >index " + std::to_string(trigger::payload(tval[1]).cul_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_culture_state) {
	return "dup " + std::to_string(trigger::payload(tval[1]).cul_id.index() + demographics::count_special_keys + ws.world.pop_type_size() * 2) + " >demographics_key demographics @ 0.0 > " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_culture_province) {
	return "dup " + std::to_string(trigger::payload(tval[1]).cul_id.index() + demographics::count_special_keys + ws.world.pop_type_size() * 2) + " >demographics_key demographics @ 0.0 > " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_culture_nation) {
	return "dup " + std::to_string(trigger::payload(tval[1]).cul_id.index() + demographics::count_special_keys + ws.world.pop_type_size() * 2) + " >demographics_key demographics @ 0.0 > " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_religion_pop_this_pop) {
	return ">r dup religion @ r> swap >r dup religion r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_religion_state_this_pop) {
	return ">r dup religion @ >index r> swap >r dup r> " + std::to_string(demographics::count_special_keys + ws.world.pop_type_size() * 2 + ws.world.culture_size() + ws.world.ideology_size() + ws.world.issue_option_size()) + " + >demographics_key demographics @ 0.0 > " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_religion_province_this_pop) {
	return ">r dup religion @ >index r> swap >r dup r> " + std::to_string(demographics::count_special_keys + ws.world.pop_type_size() * 2 + ws.world.culture_size() + ws.world.ideology_size() + ws.world.issue_option_size()) + " + >demographics_key demographics @ 0.0 > " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_religion_nation_this_pop) {
	return ">r dup religion @ >index r> swap >r dup r> " + std::to_string(demographics::count_special_keys + ws.world.pop_type_size() * 2 + ws.world.culture_size() + ws.world.ideology_size() + ws.world.issue_option_size()) + " + >demographics_key demographics @ 0.0 > " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_religion_pop) {
	return "dup religion @ >index " + std::to_string(trigger::payload(tval[1]).rel_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_religion_state) {
	return "dup " + std::to_string(demographics::to_key(ws, trigger::payload(tval[1]).rel_id).index()) + " >demographics_key demographics @ 0.0 > " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_religion_province) {
	return "dup " + std::to_string(demographics::to_key(ws, trigger::payload(tval[1]).rel_id).index()) + " >demographics_key demographics @ 0.0 > " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_pop_religion_nation) {
	return "dup " + std::to_string(demographics::to_key(ws, trigger::payload(tval[1]).rel_id).index()) + " >demographics_key demographics @ 0.0 > " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_life_needs) {
	return "dup ulife_needs_satisfaction @ >f32 " + std::to_string(1.0f / float(std::numeric_limits<uint8_t>::max())) + " * " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_everyday_needs) {
	return "dup ueveryday_needs_satisfaction @ >f32 " + std::to_string(1.0f / float(std::numeric_limits<uint8_t>::max())) + " * " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_luxury_needs) {
	return "dup uluxury_needs_satisfaction @ >f32 " + std::to_string(1.0f / float(std::numeric_limits<uint8_t>::max())) + " * " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_consciousness_pop) {
	return "dup uconsciousness @ >f32 " + std::to_string(10.0f / float(std::numeric_limits<uint16_t>::max())) + " * " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_consciousness_province) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::consciousness.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_consciousness_state) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::consciousness.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_consciousness_nation) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::consciousness.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_literacy_pop) {
	return "dup uliteracy @ >f32 " + std::to_string(1.0f / float(std::numeric_limits<uint16_t>::max())) + " * " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_literacy_province) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::literacy.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_literacy_state) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::literacy.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_literacy_nation) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::literacy.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_militancy_pop) {
	return "dup umilitancy @ >f32 " + std::to_string(10.0f / float(std::numeric_limits<uint16_t>::max())) + " * " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_militancy_province) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::militancy.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_militancy_state) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::militancy.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_militancy_nation) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::militancy.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_trade_goods_in_state_state) {
	return "dup >index " + std::to_string(trigger::payload(tval[1]).com_id.index()) + " state-ptr @ state-has-rgo? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_trade_goods_in_state_province) {
	return "dup state_membership @ >index " + std::to_string(trigger::payload(tval[1]).com_id.index()) + " state-ptr @ state-has-rgo? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_flashpoint) {
	return "dup flashpoint_tag @ valid? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_flashpoint_tension) {
	return "dup flashpoint_tension @ " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_flashpoint_tension_province) {
	return "dup state_membership @ flashpoint_tension @ " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_crisis_exist) {
	return std::to_string(offsetof(sys::state, current_crisis_state)) + " state-ptr @ buf-add ptr-cast ptr(i32) @ " + std::to_string(int32_t(sys::crisis_state::inactive)) + " = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_liberation_crisis) {
	return "state-ptr @ liberation-crisis? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_is_claim_crisis) {
	return " false " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_crisis_temperature) {
	return std::to_string(offsetof(sys::state, crisis_temperature)) + " state-ptr @ buf-add ptr-cast ptr(f32) @ " + std::to_string(read_float_from_payload(tval + 1)) + "  " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_involved_in_crisis_nation) {
	return "dup >index state-ptr @ in-crisis? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_involved_in_crisis_pop) {
	return "dup " + pop_to_owner() + ">index state-ptr @ in-crisis? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_rich_strata_life_needs_nation) {
	return "dup " + std::to_string(demographics::rich_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::rich_life_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_rich_strata_life_needs_state) {
	return "dup " + std::to_string(demographics::rich_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::rich_life_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_rich_strata_life_needs_province) {
	return "dup " + std::to_string(demographics::rich_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::rich_life_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_rich_strata_life_needs_pop) {
	return "dup " + pop_to_location() + "state_membership @ dup " + std::to_string(demographics::rich_total.index()) + " >demographics_key demographics @ >r " + std::to_string(demographics::rich_life_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_rich_strata_everyday_needs_nation) {
	return "dup " + std::to_string(demographics::rich_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::rich_everyday_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_rich_strata_everyday_needs_state) {
	return "dup " + std::to_string(demographics::rich_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::rich_everyday_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_rich_strata_everyday_needs_province) {
	return "dup " + std::to_string(demographics::rich_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::rich_everyday_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_rich_strata_everyday_needs_pop) {
	return "dup " + pop_to_location() + "state_membership @ dup " + std::to_string(demographics::rich_total.index()) + " >demographics_key demographics @ >r " + std::to_string(demographics::rich_everyday_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_rich_strata_luxury_needs_nation) {
	return "dup " + std::to_string(demographics::rich_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::rich_luxury_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_rich_strata_luxury_needs_state) {
	return "dup " + std::to_string(demographics::rich_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::rich_luxury_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_rich_strata_luxury_needs_province) {
	return "dup " + std::to_string(demographics::rich_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::rich_luxury_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_rich_strata_luxury_needs_pop) {
	return "dup " + pop_to_location() + "state_membership @ dup " + std::to_string(demographics::rich_total.index()) + " >demographics_key demographics @ >r " + std::to_string(demographics::rich_luxury_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_middle_strata_life_needs_nation) {
	return "dup " + std::to_string(demographics::middle_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::middle_life_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_middle_strata_life_needs_state) {
	return "dup " + std::to_string(demographics::middle_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::middle_life_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_middle_strata_life_needs_province) {
	return "dup " + std::to_string(demographics::middle_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::middle_life_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_middle_strata_life_needs_pop) {
	return "dup " + pop_to_location() + "state_membership @ dup " + std::to_string(demographics::middle_total.index()) + " >demographics_key demographics @ >r " + std::to_string(demographics::middle_life_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_middle_strata_everyday_needs_nation) {
	return "dup " + std::to_string(demographics::middle_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::middle_everyday_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_middle_strata_everyday_needs_state) {
	return "dup " + std::to_string(demographics::middle_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::middle_everyday_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_middle_strata_everyday_needs_province) {
	return "dup " + std::to_string(demographics::middle_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::middle_everyday_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_middle_strata_everyday_needs_pop) {
	return "dup " + pop_to_location() + "state_membership @ dup " + std::to_string(demographics::middle_total.index()) + " >demographics_key demographics @ >r " + std::to_string(demographics::middle_everyday_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_middle_strata_luxury_needs_nation) {
	return "dup " + std::to_string(demographics::middle_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::middle_luxury_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_middle_strata_luxury_needs_state) {
	return "dup " + std::to_string(demographics::middle_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::middle_luxury_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_middle_strata_luxury_needs_province) {
	return "dup " + std::to_string(demographics::middle_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::middle_luxury_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_middle_strata_luxury_needs_pop) {
	return "dup " + pop_to_location() + "state_membership @ dup " + std::to_string(demographics::middle_total.index()) + " >demographics_key demographics @ >r " + std::to_string(demographics::middle_luxury_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_poor_strata_life_needs_nation) {
	return "dup " + std::to_string(demographics::poor_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::poor_life_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_poor_strata_life_needs_state) {
	return "dup " + std::to_string(demographics::poor_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::poor_life_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_poor_strata_life_needs_province) {
	return "dup " + std::to_string(demographics::poor_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::poor_life_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_poor_strata_life_needs_pop) {
	return "dup " + pop_to_location() + "state_membership @ dup " + std::to_string(demographics::poor_total.index()) + " >demographics_key demographics @ >r " + std::to_string(demographics::poor_life_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_poor_strata_everyday_needs_nation) {
	return "dup " + std::to_string(demographics::poor_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::poor_everyday_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_poor_strata_everyday_needs_state) {
	return "dup " + std::to_string(demographics::poor_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::poor_everyday_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_poor_strata_everyday_needs_province) {
	return "dup " + std::to_string(demographics::poor_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::poor_everyday_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_poor_strata_everyday_needs_pop) {
	return "dup " + pop_to_location() + "state_membership @ dup " + std::to_string(demographics::poor_total.index()) + " >demographics_key demographics @ >r " + std::to_string(demographics::poor_everyday_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_poor_strata_luxury_needs_nation) {
	return "dup " + std::to_string(demographics::poor_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::poor_luxury_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_poor_strata_luxury_needs_state) {
	return "dup " + std::to_string(demographics::poor_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::poor_luxury_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_poor_strata_luxury_needs_province) {
	return "dup " + std::to_string(demographics::poor_total.index()) + " >demographics_key demographics @ >r dup " + std::to_string(demographics::poor_luxury_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_poor_strata_luxury_needs_pop) {
	return "dup " + pop_to_location() + "state_membership @ dup " + std::to_string(demographics::poor_total.index()) + " >demographics_key demographics @ >r " + std::to_string(demographics::poor_luxury_needs.index()) + " >demographics_key demographics @ r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_diplomatic_influence_tag) {
	return "dup >index " + std::to_string(trigger::payload(tval[2]).tag_id.index()) +" >national_identity_id " + tag_to_nation() + ">index state-ptr @ influence-on " + std::to_string(tval[1]) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_diplomatic_influence_this_nation) {
	return ">r dup r> swap >r dup >index r> >index state-ptr @ influence-on " + std::to_string(tval[1]) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_diplomatic_influence_this_province) {
	return ">r dup " + province_to_owner() + "r> swap >r dup >index r> >index state-ptr @ influence-on " + std::to_string(tval[1]) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_diplomatic_influence_from_nation) {
	return ">r >r dup r> swap r> swap >r dup >index r> >index state-ptr @ influence-on " + std::to_string(tval[1]) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_diplomatic_influence_from_province) {
	return ">r >r dup " + province_to_owner() + "r> swap r> swap >r dup >index r> >index state-ptr @ influence-on " + std::to_string(tval[1]) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_unemployment_nation) {
	return "dup " + std::to_string(demographics::count_special_keys + trigger::payload(tval[3]).popt_id.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::count_special_keys + ws.world.pop_type_size() + trigger::payload(tval[3]).popt_id.index()) + " >demographics_key demographics @ "
		"r@ / 1.0 swap - 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_unemployment_state) {
	return "dup " + std::to_string(demographics::count_special_keys + trigger::payload(tval[3]).popt_id.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::count_special_keys + ws.world.pop_type_size() + trigger::payload(tval[3]).popt_id.index()) + " >demographics_key demographics @ "
		"r@ / 1.0 swap - 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_unemployment_province) {
	return "dup " + std::to_string(demographics::count_special_keys + trigger::payload(tval[3]).popt_id.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::count_special_keys + ws.world.pop_type_size() + trigger::payload(tval[3]).popt_id.index()) + " >demographics_key demographics @ "
		"r@ / 1.0 swap - 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_unemployment_pop) {
	return "dup uemployment @ >f32 " + std::to_string(1.0f / float(std::numeric_limits<uint8_t>::max())) + " * 1.0 swap - " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_unemployment_nation_this_pop) {
	return
		">r dup poptype @ >index r> swap >r "
		"dup " + std::to_string(demographics::count_special_keys) + " r@ + >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::count_special_keys + ws.world.pop_type_size()) + " r> r> swap >r + >demographics_key demographics @ "
		"r@ / 1.0 swap - 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_unemployment_state_this_pop) {
	return
		">r dup poptype @ >index r> swap >r "
		"dup " + std::to_string(demographics::count_special_keys) + " r@ + >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::count_special_keys + ws.world.pop_type_size()) + " r> r> swap >r + >demographics_key demographics @ "
		"r@ / 1.0 swap - 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_pop_unemployment_province_this_pop) {
	return
		">r dup poptype @ >index r> swap >r "
		"dup " + std::to_string(demographics::count_special_keys) + " r@ + >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::count_special_keys + ws.world.pop_type_size()) + " r> r> swap >r + >demographics_key demographics @ "
		"r@ / 1.0 swap - 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_relation_tag) {
	return "dup >index " +std::to_string(trigger::payload(tval[2]).tag_id.index()) + " >national_identity_id " + tag_to_nation() + ">index state-ptr @ relations " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_relation_this_nation) {
	return ">r dup >index r> swap >r dup >index r> state-ptr @ relations " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_relation_this_province) {
	return ">r dup " + province_to_owner() + ">index r> swap >r dup >index r> state-ptr @ relations " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_relation_this_pop) {
	return ">r dup " + pop_to_owner() + ">index r> swap >r dup >index r> state-ptr @ relations " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_relation_from_nation) {
	return ">r >r dup >index r> swap r> swap >r dup >index r> state-ptr @ relations " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_relation_from_province) {
	return ">r >r dup " + province_to_owner() + ">index r> swap r> swap >r dup >index r> state-ptr @ relations " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_check_variable) {
	return "dup " + std::to_string(trigger::payload(tval[3]).natv_id.index()) + " >national_variable_id variables @ " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_upper_house) {
	return "dup " + std::to_string(trigger::payload(tval[3]).ideo_id.index()) + " >ideology_id upper_house @ " + std::to_string(100.0f * read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_unemployment_by_type_nation) {
	return "dup " + std::to_string(demographics::count_special_keys + trigger::payload(tval[3]).popt_id.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::count_special_keys + ws.world.pop_type_size() + trigger::payload(tval[3]).popt_id.index()) + " >demographics_key demographics @ "
		"r@ / 1.0 swap - 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_unemployment_by_type_state) {
	return "dup " + std::to_string(demographics::count_special_keys + trigger::payload(tval[3]).popt_id.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::count_special_keys + ws.world.pop_type_size() + trigger::payload(tval[3]).popt_id.index()) + " >demographics_key demographics @ "
		"r@ / 1.0 swap - 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_unemployment_by_type_province) {
	return "dup " + std::to_string(demographics::count_special_keys + trigger::payload(tval[3]).popt_id.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::count_special_keys + ws.world.pop_type_size() + trigger::payload(tval[3]).popt_id.index()) + " >demographics_key demographics @ "
		"r@ / 1.0 swap - 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_unemployment_by_type_pop) {
	return "dup " + pop_to_location() + "state_membership @ dup "
		+ std::to_string(demographics::count_special_keys + trigger::payload(tval[3]).popt_id.index()) + " >demographics_key demographics @ >r "
		+ std::to_string(demographics::count_special_keys + ws.world.pop_type_size() + trigger::payload(tval[3]).popt_id.index()) + " >demographics_key demographics @ "
		"r@ / 1.0 swap - 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 1)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_party_loyalty_nation_province_id) {
	return std::to_string(trigger::payload(tval[1]).prov_id.index()) + " >province_id " + std::to_string(trigger::payload(tval[3]).ideo_id.index()) + " >ideology_id party_loyalty @" + std::to_string(trigger::payload(tval[2]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_party_loyalty_from_nation_province_id) {
	return std::to_string(trigger::payload(tval[1]).prov_id.index()) + " >province_id " + std::to_string(trigger::payload(tval[3]).ideo_id.index()) + " >ideology_id party_loyalty @" + std::to_string(trigger::payload(tval[2]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_party_loyalty_province_province_id) {
	return std::to_string(trigger::payload(tval[1]).prov_id.index()) + " >province_id " + std::to_string(trigger::payload(tval[3]).ideo_id.index()) + " >ideology_id party_loyalty @" + std::to_string(trigger::payload(tval[2]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_party_loyalty_from_province_province_id) {
	return std::to_string(trigger::payload(tval[1]).prov_id.index()) + " >province_id " + std::to_string(trigger::payload(tval[3]).ideo_id.index()) + " >ideology_id party_loyalty @" + std::to_string(trigger::payload(tval[2]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_party_loyalty_nation_from_province) {
	return ">r >r dup r> swap r> swap " + std::to_string(trigger::payload(tval[2]).ideo_id.index()) + " >ideology_id party_loyalty @" + std::to_string(trigger::payload(tval[1]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_party_loyalty_generic) {
	return "dup " + std::to_string(trigger::payload(tval[2]).ideo_id.index()) + " >ideology_id party_loyalty @" + std::to_string(trigger::payload(tval[1]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_party_loyalty_from_nation_scope_province) {
	return "dup " + std::to_string(trigger::payload(tval[2]).ideo_id.index()) + " >ideology_id party_loyalty @" + std::to_string(trigger::payload(tval[1]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_party_loyalty_from_province_scope_province) {
	return "dup " + std::to_string(trigger::payload(tval[2]).ideo_id.index()) + " >ideology_id party_loyalty @" + std::to_string(trigger::payload(tval[1]).signed_value) + ".0 " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_in_province_railroad_no_limit_from_nation) {
	return ">r >r dup " + std::to_string(uint8_t(economy::province_building_type::railroad)) + " >u8 max_building_level @ >f32 r> swap r> swap >r "
		"dup " + std::to_string(uint8_t(economy::province_building_type::railroad)) + " >u8 building_level @ >f32 >r "
		"dup " + std::to_string(sys::provincial_mod_offsets::min_build_railroad.index()) + " >provincial_modifier_value modifier_values @ r> + r> < " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_in_province_railroad_yes_limit_from_nation) {
	return ">r >r dup " + std::to_string(uint8_t(economy::province_building_type::railroad)) + " >u8 max_building_level @ >f32 r> swap r> swap >r "
		"dup " + std::to_string(uint8_t(economy::province_building_type::railroad)) + " >u8 building_level @ >f32 >r "
		"dup " + std::to_string(sys::provincial_mod_offsets::min_build_railroad.index()) + " >provincial_modifier_value modifier_values @ r> + r> < " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_in_province_railroad_no_limit_this_nation) {
	return ">r dup " + std::to_string(uint8_t(economy::province_building_type::railroad)) + " >u8 max_building_level @ >f32 r> swap >r "
		"dup " + std::to_string(uint8_t(economy::province_building_type::railroad)) + " >u8 building_level @ >f32 >r "
		"dup " + std::to_string(sys::provincial_mod_offsets::min_build_railroad.index()) + " >provincial_modifier_value modifier_values @ r> + r> < " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_in_province_railroad_yes_limit_this_nation) {
	return ">r dup " + std::to_string(uint8_t(economy::province_building_type::railroad)) + " >u8 max_building_level @ >f32 r> swap >r "
		"dup " + std::to_string(uint8_t(economy::province_building_type::railroad)) + " >u8 building_level @ >f32 >r "
		"dup " + std::to_string(sys::provincial_mod_offsets::min_build_railroad.index()) + " >provincial_modifier_value modifier_values @ r> + r> < " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_in_province_fort_no_limit_from_nation) {
	return ">r >r dup " + std::to_string(uint8_t(economy::province_building_type::fort)) + " >u8 max_building_level @ >f32 r> swap r> swap >r "
		"dup " + std::to_string(uint8_t(economy::province_building_type::fort)) + " >u8 building_level @ >f32 >r "
		"dup " + std::to_string(sys::provincial_mod_offsets::min_build_fort.index()) + " >provincial_modifier_value modifier_values @ r> + r> < " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_in_province_fort_yes_limit_from_nation) {
	return ">r >r dup " + std::to_string(uint8_t(economy::province_building_type::fort)) + " >u8 max_building_level @ >f32 r> swap r> swap >r "
		"dup " + std::to_string(uint8_t(economy::province_building_type::fort)) + " >u8 building_level @ >f32 >r "
		"dup " + std::to_string(sys::provincial_mod_offsets::min_build_fort.index()) + " >provincial_modifier_value modifier_values @ r> + r> < " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_in_province_fort_no_limit_this_nation) {
	return ">r dup " + std::to_string(uint8_t(economy::province_building_type::fort)) + " >u8 max_building_level @ >f32 r> swap >r "
		"dup " + std::to_string(uint8_t(economy::province_building_type::fort)) + " >u8 building_level @ >f32 >r "
		"dup " + std::to_string(sys::provincial_mod_offsets::min_build_fort.index()) + " >provincial_modifier_value modifier_values @ r> + r> < " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_in_province_fort_yes_limit_this_nation) {
	return ">r dup " + std::to_string(uint8_t(economy::province_building_type::fort)) + " >u8 max_building_level @ >f32 r> swap >r "
		"dup " + std::to_string(uint8_t(economy::province_building_type::fort)) + " >u8 building_level @ >f32 >r "
		"dup " + std::to_string(sys::provincial_mod_offsets::min_build_fort.index()) + " >provincial_modifier_value modifier_values @ r> + r> < " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_in_province_naval_base_no_limit_from_nation) {
	return ">r >r dup >index r> swap r> swap >r dup >index r> state-ptr @ can-build-naval-base? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_in_province_naval_base_yes_limit_from_nation) {
	return ">r >r dup >index r> swap r> swap >r dup >index r> state-ptr @ can-build-naval-base? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_in_province_naval_base_no_limit_this_nation) {
	return ">r dup >index r> swap >r dup >index r> state-ptr @ can-build-naval-base? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_in_province_naval_base_yes_limit_this_nation) {
	return ">r dup >index r> swap >r dup >index r> state-ptr @ can-build-naval-base? " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_railway_in_capital_yes_whole_state_yes_limit) {
	return "true " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_railway_in_capital_yes_whole_state_no_limit) {
	return "true " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_railway_in_capital_no_whole_state_yes_limit) {
	return "true " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_railway_in_capital_no_whole_state_no_limit) {
	return "true " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_fort_in_capital_yes_whole_state_yes_limit) {
	return "true " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_fort_in_capital_yes_whole_state_no_limit) {
	return "true " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_fort_in_capital_no_whole_state_yes_limit) {
	return "true " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_can_build_fort_in_capital_no_whole_state_no_limit) {
	return "true " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_work_available_nation) {
	return "dup " + std::to_string(demographics::to_key(ws, trigger::payload(tval[1]).popt_id).index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::to_employment_key(ws, trigger::payload(tval[1]).popt_id).index()) + " >demographics_key demographics @ "
		" r> 0.4 * >= " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_work_available_state) {
	return "dup " + std::to_string(demographics::to_key(ws, trigger::payload(tval[1]).popt_id).index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::to_employment_key(ws, trigger::payload(tval[1]).popt_id).index()) + " >demographics_key demographics @ "
		" r> 0.4 * >= " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_work_available_province) {
	return "dup " + std::to_string(demographics::to_key(ws, trigger::payload(tval[1]).popt_id).index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::to_employment_key(ws, trigger::payload(tval[1]).popt_id).index()) + " >demographics_key demographics @ "
		" r> 0.4 * >= " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_ideology_name_nation) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::to_key(ws, trigger::payload(tval[1]).ideo_id).index()) + " >demographics_key demographics @ "
		" r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 2)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_ideology_name_state) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::to_key(ws, trigger::payload(tval[1]).ideo_id).index()) + " >demographics_key demographics @ "
		" r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 2)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_ideology_name_province) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::to_key(ws, trigger::payload(tval[1]).ideo_id).index()) + " >demographics_key demographics @ "
		" r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 2)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_ideology_name_pop) {
	return "dup " + std::to_string(demographics::to_key(ws, trigger::payload(tval[1]).ideo_id).index()) + " >pop_demographics_key udemographics @ "
		" >f32 " + std::to_string(1.0f / float(std::numeric_limits<uint8_t>::max())) + " * " + std::to_string(read_float_from_payload(tval + 2)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_issue_name_nation) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::to_key(ws, trigger::payload(tval[1]).opt_id).index()) + " >demographics_key demographics @ "
		" r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 2)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_issue_name_state) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::to_key(ws, trigger::payload(tval[1]).opt_id).index()) + " >demographics_key demographics @ "
		" r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 2)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_issue_name_province) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::to_key(ws, trigger::payload(tval[1]).opt_id).index()) + " >demographics_key demographics @ "
		" r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 2)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_issue_name_pop) {
	return "dup " + std::to_string(demographics::to_key(ws, trigger::payload(tval[1]).opt_id).index()) + " >pop_demographics_key udemographics @ "
		" >f32 " + std::to_string(1.0f / float(std::numeric_limits<uint8_t>::max())) + " * " + std::to_string(read_float_from_payload(tval + 2)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_issue_group_name_nation) {
	return "dup " + std::to_string(trigger::payload(tval[1]).iss_id.index()) + " >issue_id issues @ >index " + std::to_string(trigger::payload(tval[2]).opt_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_issue_group_name_state) {
	return "dup " + state_to_owner() + std::to_string(trigger::payload(tval[1]).iss_id.index()) + " >issue_id issues @ >index " + std::to_string(trigger::payload(tval[2]).opt_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_issue_group_name_province) {
	return "dup " + province_to_owner() + std::to_string(trigger::payload(tval[1]).iss_id.index()) + " >issue_id issues @ >index " + std::to_string(trigger::payload(tval[2]).opt_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_issue_group_name_pop) {
	return "dup " + pop_to_owner() + std::to_string(trigger::payload(tval[1]).iss_id.index()) + " >issue_id issues @ >index " + std::to_string(trigger::payload(tval[2]).opt_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_reform_group_name_nation) {
	return "dup " + std::to_string(trigger::payload(tval[1]).ref_id.index()) + " >reform_id issues @ >index " + std::to_string(trigger::payload(tval[2]).ropt_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_reform_group_name_state) {
	return "dup " + state_to_owner() + std::to_string(trigger::payload(tval[1]).ref_id.index()) + " >reform_id issues @ >index " + std::to_string(trigger::payload(tval[2]).ropt_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_reform_group_name_province) {
	return "dup " + province_to_owner() + std::to_string(trigger::payload(tval[1]).ref_id.index()) + " >reform_id issues @ >index " + std::to_string(trigger::payload(tval[2]).ropt_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_reform_group_name_pop) {
	return "dup " + pop_to_owner() + std::to_string(trigger::payload(tval[1]).ref_id.index()) + " >reform_id issues @ >index " + std::to_string(trigger::payload(tval[2]).ropt_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_pop_type_name_nation) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::to_key(ws, trigger::payload(tval[1]).popt_id).index()) + " >demographics_key demographics @ "
		" r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 2)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_pop_type_name_state) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::to_key(ws, trigger::payload(tval[1]).popt_id).index()) + " >demographics_key demographics @ "
		" r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 2)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_pop_type_name_province) {
	return "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ >r "
		"dup " + std::to_string(demographics::to_key(ws, trigger::payload(tval[1]).popt_id).index()) + " >demographics_key demographics @ "
		" r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 2)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_pop_type_name_pop) {
	return "dup " + pop_to_location() + "dup " + std::to_string(demographics::total.index()) + " >demographics_key demographics @ >r "
		+ std::to_string(demographics::to_key(ws, trigger::payload(tval[1]).popt_id).index()) + " >demographics_key demographics @ "
		" r@ / 0.0 0.0 r> >= select " + std::to_string(read_float_from_payload(tval + 2)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_variable_good_name) {
	return "dup " + std::to_string(trigger::payload(tval[1]).com_id.index()) + " >commodity_id stockpiles @ " + std::to_string(read_float_from_payload(tval + 2)) + " " + compare_values(tval[0]);
}
TRIGGER_FUNCTION(tf_religion_nation) {
	return "dup religion @ >index " + std::to_string(trigger::payload(tval[1]).rel_id.index()) + " " + compare_values_eq(tval[0]);
}
TRIGGER_FUNCTION(tf_religion_nation_reb) {
	return ">r >r dup religion @ r> swap r> swap >r dup religion @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_religion_nation_from_nation) {
	return ">r >r dup religion @ r> swap r> swap >r dup religion @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_religion_nation_this_nation) {
	return ">r dup religion @ r> swap >r dup religion @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_religion_nation_this_state) {
	return ">r dup " + state_to_owner() + "religion @ r> swap >r dup religion @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_religion_nation_this_province) {
	return ">r dup " + province_to_owner() + "religion @ r> swap >r dup religion @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_religion_nation_this_pop) {
	return ">r dup " + pop_to_owner() + "religion @ r> swap >r dup religion @ r> = " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_invention) {
	return "dup " + std::to_string(trigger::payload(tval[1]).invt_id.index()) + " >invention_id active_inventions @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_invention_province) {
	return "dup " + province_to_owner() + std::to_string(trigger::payload(tval[1]).invt_id.index()) + " >invention_id active_inventions @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_invention_pop) {
	return "dup " + pop_to_owner() + std::to_string(trigger::payload(tval[1]).invt_id.index()) + " >invention_id active_inventions @ " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_test) {
	auto sid = trigger::payload(tval[1]).str_id;
	auto tid = ws.world.stored_trigger_get_function(sid);
	return test_trigger_generic(ws.trigger_data.data() + ws.trigger_data_indices[tid.index() + 1], ws) + truth_inversion(tval[0]);
}

TRIGGER_FUNCTION(tf_has_building_bank) {
	return "dup " + std::to_string(int32_t(economy::province_building_type::bank)) + " >ui8 building_level 0 <> " + truth_inversion(tval[0]);
}
TRIGGER_FUNCTION(tf_has_building_university) {
	return "dup " + std::to_string(int32_t(economy::province_building_type::university)) + " >ui8 building_level 0 <> " + truth_inversion(tval[0]);
}

TRIGGER_FUNCTION(tf_party_name) {
	auto ideo = trigger::payload(tval[1]).ideo_id;
	dcon::text_key new_name{ dcon::text_key::value_base_t(trigger::read_int32_t_from_payload(tval + 2)) };

	if(ideo) {
		return "dup >index >r " + std::to_string(new_name.index()) + " " + std::to_string(ideo.index()) + " r> state-ptr @ has-named-party? " + truth_inversion(tval[0]);
	} else {
		return "dup ruling_party @ name @ " + std::to_string(new_name.index()) + " >text_key = " + truth_inversion(tval[0]);
	}
}

TRIGGER_FUNCTION(tf_party_position) {
	auto ideo = trigger::payload(tval[1]).ideo_id;
	dcon::issue_option_id new_opt = trigger::payload(tval[2]).opt_id;

	if(ideo) {
		return "dup >index >r " + std::to_string(new_opt.index()) + " " + std::to_string(ideo.index()) + " r> state-ptr @ has-positioned-party? " + truth_inversion(tval[0]);
	} else {
		auto popt = ws.world.issue_option_get_parent_issue(new_opt);
		return "dup ruling_party @ " + std::to_string(popt.id.index())  + " >issue_id party_issues @ " + std::to_string(new_opt.index()) + " >issue_option_id = " + truth_inversion(tval[0]);
	}
}

struct trigger_container {
	constexpr static std::string (
			CALLTYPE* trigger_functions[])(uint16_t const* tval, sys::state& ws) = {
			tf_none,
#define TRIGGER_BYTECODE_ELEMENT(code, name, arg) tf_##name ,
			TRIGGER_BYTECODE_LIST
#undef TRIGGER_BYTECODE_ELEMENT
			//
			// scopes
			//
			tf_generic_scope, // constexpr uint16_t generic_scope = 0x0000; // or & and
			tf_x_neighbor_province_scope, // constexpr uint16_t x_neighbor_province_scope = 0x0001;
			tf_x_neighbor_country_scope_nation, // constexpr uint16_t x_neighbor_country_scope_nation = 0x0002;
			tf_x_neighbor_country_scope_pop, // constexpr uint16_t x_neighbor_country_scope_pop = 0x0003;
			tf_x_war_countries_scope_nation, // constexpr uint16_t x_war_countries_scope_nation = 0x0004;
			tf_x_war_countries_scope_pop, // constexpr uint16_t x_war_countries_scope_pop = 0x0005;
			tf_x_greater_power_scope, // constexpr uint16_t x_greater_power_scope = 0x0006;
			tf_x_owned_province_scope_state, // constexpr uint16_t x_owned_province_scope_state = 0x0007;
			tf_x_owned_province_scope_nation, // constexpr uint16_t x_owned_province_scope_nation = 0x0008;
			tf_x_core_scope_province, // constexpr uint16_t x_core_scope_province = 0x0009;
			tf_x_core_scope_nation,	 // constexpr uint16_t x_core_scope_nation = 0x000A;
			tf_x_state_scope,		// constexpr uint16_t x_state_scope = 0x000B;
			tf_x_substate_scope, // constexpr uint16_t x_substate_scope = 0x000C;
			tf_x_sphere_member_scope, // constexpr uint16_t x_sphere_member_scope = 0x000D;
			tf_x_pop_scope_province, // constexpr uint16_t x_pop_scope_province = 0x000E;
			tf_x_pop_scope_state,	// constexpr uint16_t x_pop_scope_state = 0x000F;
			tf_x_pop_scope_nation, // constexpr uint16_t x_pop_scope_nation = 0x0010;
			tf_x_provinces_in_variable_region, // constexpr uint16_t x_provinces_in_variable_region = 0x0011; // variable name
			tf_owner_scope_state, // constexpr uint16_t owner_scope_state = 0x0012;
			tf_owner_scope_province,  // constexpr uint16_t owner_scope_province = 0x0013;
			tf_controller_scope, // constexpr uint16_t controller_scope = 0x0014;
			tf_location_scope, // constexpr uint16_t location_scope = 0x0015;
			tf_country_scope_state, // constexpr uint16_t country_scope_state = 0x0016;
			tf_country_scope_pop, // constexpr uint16_t country_scope_pop = 0x0017;
			tf_capital_scope,		 // constexpr uint16_t capital_scope = 0x0018;
			tf_this_scope,				 // constexpr uint16_t this_scope_pop = 0x0019;
			tf_this_scope,				 // constexpr uint16_t this_scope_nation = 0x001A;
			tf_this_scope,				 // constexpr uint16_t this_scope_state = 0x001B;
			tf_this_scope,				 // constexpr uint16_t this_scope_province = 0x001C;
			tf_from_scope,				 // constexpr uint16_t from_scope_pop = 0x001D;
			tf_from_scope,				 // constexpr uint16_t from_scope_nation = 0x001E;
			tf_from_scope,				 // constexpr uint16_t from_scope_state = 0x001F;
			tf_from_scope,				 // constexpr uint16_t from_scope_province = 0x0020;
			tf_sea_zone_scope,		 // constexpr uint16_t sea_zone_scope = 0x0021;
			tf_cultural_union_scope,  // constexpr uint16_t cultural_union_scope = 0x0022;
			tf_overlord_scope,			// constexpr uint16_t overlord_scope = 0x0023;
			tf_sphere_owner_scope, // constexpr uint16_t sphere_owner_scope = 0x0024;
			tf_independence_scope, // constexpr uint16_t independence_scope = 0x0025;
			tf_flashpoint_tag_scope,	 // constexpr uint16_t flashpoint_tag_scope = 0x0026;
			tf_crisis_state_scope, // constexpr uint16_t crisis_state_scope = 0x0027;
			tf_state_scope_pop,		// constexpr uint16_t state_scope_pop = 0x0028;
			tf_state_scope_province,	 // constexpr uint16_t state_scope_province = 0x0029;
			tf_tag_scope, // constexpr uint16_t tag_scope = 0x002A; // variable name
			tf_integer_scope, // constexpr uint16_t integer_scope = 0x002B; // variable name
			tf_country_scope_nation, // constexpr uint16_t country_scope_nation = 0x002C;
			tf_country_scope_province, // constexpr uint16_t country_scope_province = 0x002D;
			tf_cultural_union_scope_pop, // constexpr uint16_t cultural_union_scope_pop = 0x002E;
			tf_capital_scope_province, // constexpr uint16_t capital_scope_province = 0x002F;
			tf_capital_scope_pop, //constexpr inline uint16_t capital_scope_pop = first_scope_code + 0x0030;
			tf_x_country_scope, //constexpr inline uint16_t x_country_scope = first_scope_code + 0x0031;
			tf_x_neighbor_province_scope_state, //constexpr inline uint16_t x_neighbor_province_scope_state = first_scope_code + 0x0032;
			tf_x_provinces_in_variable_region_proper, //constexpr inline uint16_t x_provinces_in_variable_region_proper = first_scope_code + 0x0033;
	};
};


std::string CALLTYPE test_trigger_generic(uint16_t const* tval, sys::state& ws) {
	return trigger_container::trigger_functions[*tval & trigger::code_mask](tval, ws);
}

std::string multiplicative_modifier(sys::state& state, dcon::value_modifier_key modifier) {
	auto base = state.value_modifiers[modifier];
	std::string result;
	result += std::to_string(base.factor) + " >r ";

	for(uint32_t i = 0; i < base.segments_count; ++i) {
		auto seg = state.value_modifier_segments[base.first_segment_offset + i];
		if(seg.condition) {
			result += test_trigger_generic(state.trigger_data.data() + state.trigger_data_indices[seg.condition.index() + 1], state);
			result += "1.0 swap " + std::to_string(seg.factor) + " swap select r> * >r "; // multiply by either 1.0 or the segement factor depending on bool result
		}
	}
	return result;
}
std::string additive_modifier(sys::state& state, dcon::value_modifier_key modifier) {
	auto base = state.value_modifiers[modifier];
	std::string result;
	result += std::to_string(base.base) + " >r ";

	for(uint32_t i = 0; i < base.segments_count; ++i) {
		auto seg = state.value_modifier_segments[base.first_segment_offset + i];
		if(seg.condition) {
			result += test_trigger_generic(state.trigger_data.data() + state.trigger_data_indices[seg.condition.index() + 1], state);
			result += "0.0 swap " + std::to_string(seg.factor) + " swap select r> + >r "; // multiply by either 1.0 or the segement factor depending on bool result
		}
	}
	result += "r> " + std::to_string(base.factor) + " * >r ";
	return result;
}

std::string evaluate(sys::state& state, dcon::trigger_key key) {
	return test_trigger_generic(state.trigger_data.data() + state.trigger_data_indices[key.index() + 1], state);
}

#undef CALLTYPE
#undef TRIGGER_FUNCTION

} // namespace trigger
