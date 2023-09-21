#pragma once

#include <stdint.h>
#include <optional>
#include <string_view>
#include <string>

#include "constants.hpp"
#include "parsers.hpp"
#include "script_constants.hpp"
#include "nations.hpp"
#include "container_types.hpp"

namespace parsers {

struct trigger_building_context {
	scenario_building_context& outer_context;
	std::vector<uint16_t> compiled_trigger;

	float factor = 0.0f;

	trigger::slot_contents main_slot = trigger::slot_contents::empty;
	trigger::slot_contents this_slot = trigger::slot_contents::empty;
	trigger::slot_contents from_slot = trigger::slot_contents::empty;

	trigger_building_context(scenario_building_context& outer_context, trigger::slot_contents main_slot,
			trigger::slot_contents this_slot, trigger::slot_contents from_slot)
			: outer_context(outer_context), main_slot(main_slot), this_slot(this_slot), from_slot(from_slot) { }

	void add_float_to_payload(float f) {
		union {
			struct {
				uint16_t low;
				uint16_t high;
			} v;
			float f;
		} pack_float;
		pack_float.f = f;

		compiled_trigger.push_back(pack_float.v.low);
		compiled_trigger.push_back(pack_float.v.high);
	}

	void add_int32_t_to_payload(int32_t i) {
		union {
			struct {
				uint16_t low;
				uint16_t high;
			} v;
			int32_t i;
		} pack_int;
		pack_int.i = i;

		compiled_trigger.push_back(pack_int.v.low);
		compiled_trigger.push_back(pack_int.v.high);
	}
};

inline std::string slot_contents_to_string(trigger::slot_contents v) {
	switch(v) {
	case trigger::slot_contents::empty:
		return "empty";
	case trigger::slot_contents::province:
		return "province";
	case trigger::slot_contents::state:
		return "state";
	case trigger::slot_contents::pop:
		return "pop";
	case trigger::slot_contents::nation:
		return "nation";
	case trigger::slot_contents::rebel:
		return "rebel";
	}
	return "unknown";
}

inline uint16_t association_to_trigger_code(association_type a) {
	switch(a) {
	case association_type::eq:
		return trigger::association_eq;
	case association_type::eq_default:
		return trigger::association_ge;
	case association_type::ge:
		return trigger::association_ge;
	case association_type::gt:
		return trigger::association_gt;
	case association_type::lt:
		return trigger::association_lt;
	case association_type::le:
		return trigger::association_le;
	case association_type::ne:
		return trigger::association_ne;
	case association_type::none:
		return trigger::association_ge;
	case association_type::list:
		return trigger::association_ge;
	default:
		return trigger::association_ge;
	}
}

inline uint16_t association_to_le_trigger_code(association_type a) {
	switch(a) {
	case association_type::eq:
		return trigger::association_eq;
	case association_type::eq_default:
		return trigger::association_le;
	case association_type::ge:
		return trigger::association_ge;
	case association_type::gt:
		return trigger::association_gt;
	case association_type::lt:
		return trigger::association_lt;
	case association_type::le:
		return trigger::association_le;
	case association_type::ne:
		return trigger::association_ne;
	case association_type::none:
		return trigger::association_le;
	case association_type::list:
		return trigger::association_le;
	default:
		return trigger::association_le;
	}
}

inline uint16_t association_to_bool_code(association_type a) {
	switch(a) {
	case association_type::eq:
		return trigger::association_eq;
	case association_type::eq_default:
		return trigger::association_eq;
	case association_type::ge:
		return trigger::association_eq;
	case association_type::gt:
		return trigger::association_ne;
	case association_type::lt:
		return trigger::association_ne;
	case association_type::le:
		return trigger::association_eq;
	case association_type::ne:
		return trigger::association_ne;
	case association_type::none:
		return trigger::association_ge;
	case association_type::list:
		return trigger::association_ge;
	default:
		return trigger::association_ge;
	}
}

inline uint16_t invert_association(uint16_t a) {
	return static_cast<uint16_t>(0x7000) - a;
}

inline uint16_t association_to_bool_code(association_type a, bool v) {
	if(v) {
		if((a == association_type::eq) || (a == association_type::eq_default))
			return trigger::association_eq;
		else
			return trigger::association_ne;
	} else {
		if((a == association_type::eq) || (a == association_type::eq_default))
			return trigger::association_ne;
		else
			return trigger::association_eq;
	}
}

constexpr bool scope_has_any_all(uint16_t code) {
	return (code >= trigger::x_neighbor_province_scope) && (code <= trigger::x_provinces_in_variable_region);
}

struct tr_diplomatic_influence {
	std::string_view who;
	int32_t value_ = 0;
	association_type a;
	void value(association_type t, int32_t v, error_handler& err, int32_t line, trigger_building_context& context) {
		a = t;
		value_ = v;
	}
	void finish(trigger_building_context&) { }
};

struct tr_party_loyalty {
	std::string_view ideology;
	float value_ = 0.0f;
	association_type a;
	int32_t province_id = 0;
	void value(association_type t, float v, error_handler& err, int32_t line, trigger_building_context& context) {
		a = t;
		value_ = v;
	}
	void finish(trigger_building_context&) { }
};

struct tr_unemployment_by_type {
	std::string_view type;
	float value_ = 0.0f;
	association_type a;
	void value(association_type t, float v, error_handler& err, int32_t line, trigger_building_context& context) {
		a = t;
		value_ = v;
	}
	void finish(trigger_building_context&) { }
};

struct tr_upper_house {
	std::string_view ideology;
	float value_ = 0.0f;
	association_type a;
	void value(association_type t, float v, error_handler& err, int32_t line, trigger_building_context& context) {
		a = t;
		value_ = v;
	}
	void finish(trigger_building_context&) { }
};

struct tr_check_variable {
	std::string_view which;
	float value_ = 0.0f;
	association_type a;
	void value(association_type t, float v, error_handler& err, int32_t line, trigger_building_context& context) {
		a = t;
		value_ = v;
	}
	void finish(trigger_building_context&) { }
};

struct tr_relation {
	std::string_view who;
	int32_t value_ = 0;
	association_type a;
	void value(association_type t, int32_t v, error_handler& err, int32_t line, trigger_building_context& context) {
		a = t;
		value_ = v;
	}
	void finish(trigger_building_context&) { }
};

struct tr_pop_unemployment {
	std::string_view type;
	float value_ = 0.0f;
	association_type a;
	void value(association_type t, float v, error_handler& err, int32_t line, trigger_building_context& context) {
		a = t;
		value_ = v;
	}
	void finish(trigger_building_context&) { }
};

struct tr_can_build_in_province {
	std::string_view building;
	bool limit_to_world_greatest_level = false;
	void finish(trigger_building_context&) { }
};

struct tr_can_build_fort_in_capital {
	bool in_whole_capital_state = false;
	bool limit_to_world_greatest_level = false;
	void finish(trigger_building_context&) { }
};

struct tr_can_build_railway_in_capital {
	bool in_whole_capital_state = false;
	bool limit_to_world_greatest_level = false;
	void finish(trigger_building_context&) { }
};

struct tr_work_available {
	std::vector<dcon::pop_type_id> pop_type_list;
	void worker(association_type, std::string_view value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_poptypes.find(std::string(value));
				it != context.outer_context.map_of_poptypes.end()) {
			pop_type_list.push_back(it->second);
		} else {
			err.accumulated_errors +=
					std::string(value) + " is not a valid pop type name (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(trigger_building_context&) { }
};

inline bool is_from(std::string_view value) {
	return is_fixed_token_ci(value.data(), value.data() + value.length(), "from");
}
inline bool is_this(std::string_view value) {
	return is_fixed_token_ci(value.data(), value.data() + value.length(), "this");
}
inline bool is_reb(std::string_view value) {
	return is_fixed_token_ci(value.data(), value.data() + value.length(), "reb");
}

struct trigger_body {
	void finish(trigger_building_context&) { }

	void factor(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		context.factor = value;
	}

	void ai(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::ai | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "ai trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) + "(" +
																err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}

	void year(association_type a, int32_t value, error_handler& err, int32_t line, trigger_building_context& context) {
		context.compiled_trigger.push_back(uint16_t(trigger::year | association_to_trigger_code(a)));
		context.compiled_trigger.push_back(trigger::payload(uint16_t(value)).value);
	}
	void is_canal_enabled(association_type a, int32_t value, error_handler& err, int32_t line, trigger_building_context& context);
	void month(association_type a, int32_t value, error_handler& err, int32_t line, trigger_building_context& context) {
		context.compiled_trigger.push_back(uint16_t(trigger::month | association_to_trigger_code(a)));
		context.compiled_trigger.push_back(trigger::payload(uint16_t(value)).value);
	}
	void great_wars_enabled(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		context.compiled_trigger.push_back(
				uint16_t(trigger::great_wars_enabled | trigger::no_payload | association_to_bool_code(a, value)));
	}
	void world_wars_enabled(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		context.compiled_trigger.push_back(
				uint16_t(trigger::world_wars_enabled | trigger::no_payload | association_to_bool_code(a, value)));
	}
	void crisis_exist(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		context.compiled_trigger.push_back(
				uint16_t(trigger::crisis_exist | trigger::no_payload | association_to_bool_code(a, value)));
	}
	void is_liberation_crisis(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		context.compiled_trigger.push_back(
				uint16_t(trigger::is_liberation_crisis | trigger::no_payload | association_to_bool_code(a, value)));
	}
	void is_claim_crisis(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		context.compiled_trigger.push_back(
				uint16_t(trigger::is_claim_crisis | trigger::no_payload | association_to_bool_code(a, value)));
	}

	void port(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::port | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "port trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}

	void involved_in_crisis(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::involved_in_crisis_pop | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::involved_in_crisis_nation | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "involved_in_crisis trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void has_cultural_sphere(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation && context.this_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::has_cultural_sphere | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "has_cultural_sphere trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}

	void social_movement(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::social_movement | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.from_slot == trigger::slot_contents::rebel) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::social_movement_from_reb | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "social_movement trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void political_movement(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::political_movement | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.from_slot == trigger::slot_contents::rebel) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::political_movement_from_reb | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "political_movement trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void rich_tax_above_poor(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::rich_tax_above_poor | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "rich_tax_above_poor trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}

	void is_substate(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_substate | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "is_substate trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void has_flashpoint(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::has_flashpoint | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "has_flashpoint trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void is_disarmed(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_disarmed | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_disarmed_pop | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "is_disarmed trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void can_nationalize(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::can_nationalize | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "can_nationalize trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void part_of_sphere(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::part_of_sphere | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "part_of_sphere trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void constructing_cb_discovered(association_type a, bool value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::constructing_cb_discovered | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "constructing_cb_discovered trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void colonial_nation(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::colonial_nation | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "colonial_nation trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void is_capital(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_capital | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "is_capital trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void election(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::election | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "election trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void always(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		context.compiled_trigger.push_back(uint16_t(trigger::always | trigger::no_payload | association_to_bool_code(a, value)));
	}
	void is_releasable_vassal(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(is_from(value)) {
			if(context.from_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(
						uint16_t(trigger::is_releasable_vassal_from | trigger::no_payload | association_to_bool_code(a)));
			else {
				err.accumulated_errors += "is_releasable_vassal trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else {
			if(context.from_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(uint16_t(trigger::is_releasable_vassal_other | trigger::no_payload |
																										association_to_bool_code(a, parse_bool(value, line, err))));
			else {
				err.accumulated_errors += "is_releasable_vassal trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		}
	}

	void someone_can_form_union_tag(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(is_from(value)) {
			if(context.from_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(
						uint16_t(trigger::someone_can_form_union_tag_from | trigger::no_payload | association_to_bool_code(a)));
			else {
				err.accumulated_errors += "someone_can_form_union_tag trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else {
			if(context.from_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(uint16_t(trigger::someone_can_form_union_tag_other | trigger::no_payload |
																										association_to_bool_code(a, parse_bool(value, line, err))));
			else {
				err.accumulated_errors += "someone_can_form_union_tag trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		}
	}
	void is_state_capital(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_state_capital | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "is_state_capital trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}

	void has_factories(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::has_factories | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "has_factories trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void has_empty_adjacent_province(association_type a, bool value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::has_empty_adjacent_province | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "has_empty_adjacent_province trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void minorities(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::minorities_nation | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::minorities_state | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::minorities_province | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "minorities trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void culture_has_union_tag(association_type a, bool value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::culture_has_union_tag_nation | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::culture_has_union_tag_pop | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "culture_has_union_tag trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void is_colonial(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_colonial_province | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_colonial_state | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::colonial_nation | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "is_colonial trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void is_greater_power(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_greater_power_pop | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_greater_power_nation | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_greater_power_province | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "is_greater_power trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void can_create_vassals(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::can_create_vassals | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "can_create_vassals trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void has_recently_lost_war(association_type a, bool value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::has_recently_lost_war | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "has_recently_lost_war trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void is_mobilised(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_mobilised | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "is_mobilised trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void crime_higher_than_education(association_type a, bool value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::crime_higher_than_education_nation | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::crime_higher_than_education_state | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::crime_higher_than_education_province | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::crime_higher_than_education_pop | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "crime_higher_than_education trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}

	void civilized(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::civilized_nation | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::civilized_province | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::civilized_pop | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "civilized trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}

	void rank(association_type a, int32_t value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::rank | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "rank trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(trigger::payload(uint16_t(value)).value);
	}

	void crisis_temperature(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		context.compiled_trigger.push_back(uint16_t(trigger::crisis_temperature | association_to_trigger_code(a)));
		context.add_float_to_payload(value);
	}

	void has_recent_imigration(association_type a, int32_t value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::has_recent_imigration | association_to_le_trigger_code(a)));
		} else {
			err.accumulated_errors += "has_recent_imigration trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(trigger::payload(uint16_t(value)).value);
	}

	void province_control_days(association_type a, int32_t value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::province_control_days | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "province_control_days trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(trigger::payload(uint16_t(value)).value);
	}
	void num_of_substates(association_type a, int32_t value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::num_of_substates | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "num_of_substates trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(trigger::payload(uint16_t(value)).value);
	}
	void num_of_vassals_no_substates(association_type a, int32_t value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::num_of_vassals_no_substates | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "num_of_vassals_no_substates trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(trigger::payload(uint16_t(value)).value);
	}
	void number_of_states(association_type a, int32_t value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::number_of_states | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "number_of_states trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(trigger::payload(uint16_t(value)).value);
	}

	void war_score(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::war_score | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "war_score trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}

	void flashpoint_tension(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::flashpoint_tension | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "flashpoint_tension trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}

	void life_needs(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::life_needs | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "life_needs trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}

	void everyday_needs(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::everyday_needs | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "everyday_needs trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}

	void luxury_needs(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::luxury_needs | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "luxury_needs trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void social_movement_strength(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::social_movement_strength | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "social_movement_strength trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void political_movement_strength(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::political_movement_strength | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "political_movement_strength trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}

	void total_num_of_ports(association_type a, int32_t value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::total_num_of_ports | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "total_num_of_ports trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(trigger::payload(uint16_t(value)).value);
	}
	void agree_with_ruling_party(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::agree_with_ruling_party | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "agree_with_ruling_party trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void constructing_cb_progress(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::constructing_cb_progress | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "constructing_cb_progress trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void civilization_progress(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::civilization_progress | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "civilization_progress trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}

	void rich_strata_life_needs(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::rich_strata_life_needs_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::rich_strata_life_needs_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::rich_strata_life_needs_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::rich_strata_life_needs_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "rich_strata_life_needs trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void rich_strata_everyday_needs(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::rich_strata_everyday_needs_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::rich_strata_everyday_needs_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::rich_strata_everyday_needs_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::rich_strata_everyday_needs_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "rich_strata_everyday_needs trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void rich_strata_luxury_needs(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::rich_strata_luxury_needs_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::rich_strata_luxury_needs_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::rich_strata_luxury_needs_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::rich_strata_luxury_needs_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "rich_strata_luxury_needs trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}

	void middle_strata_life_needs(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::middle_strata_life_needs_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::middle_strata_life_needs_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::middle_strata_life_needs_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::middle_strata_life_needs_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "middle_strata_life_needs trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void middle_strata_everyday_needs(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::middle_strata_everyday_needs_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::middle_strata_everyday_needs_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::middle_strata_everyday_needs_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::middle_strata_everyday_needs_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "middle_strata_everyday_needs trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void middle_strata_luxury_needs(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::middle_strata_luxury_needs_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::middle_strata_luxury_needs_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::middle_strata_luxury_needs_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::middle_strata_luxury_needs_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "middle_strata_luxury_needs trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}

	void poor_strata_life_needs(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::poor_strata_life_needs_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::poor_strata_life_needs_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::poor_strata_life_needs_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::poor_strata_life_needs_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "poor_strata_life_needs trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void poor_strata_everyday_needs(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::poor_strata_everyday_needs_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::poor_strata_everyday_needs_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::poor_strata_everyday_needs_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::poor_strata_everyday_needs_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "poor_strata_everyday_needs trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void poor_strata_luxury_needs(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::poor_strata_luxury_needs_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::poor_strata_luxury_needs_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::poor_strata_luxury_needs_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::poor_strata_luxury_needs_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "poor_strata_luxury_needs trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}

	void revanchism(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::revanchism_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::revanchism_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "revanchism trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}

	void poor_strata_militancy(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::poor_strata_militancy_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::poor_strata_militancy_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::poor_strata_militancy_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::poor_strata_militancy_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "poor_strata_militancy trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void middle_strata_militancy(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::middle_strata_militancy_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::middle_strata_militancy_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::middle_strata_militancy_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::middle_strata_militancy_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "middle_strata_militancy trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void rich_strata_militancy(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::rich_strata_militancy_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::rich_strata_militancy_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::rich_strata_militancy_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::rich_strata_militancy_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "rich_strata_militancy trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void consciousness(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::consciousness_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::consciousness_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::consciousness_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::consciousness_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "consciousness trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void literacy(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::literacy_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::literacy_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::literacy_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::literacy_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "literacy trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void militancy(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::militancy_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::militancy_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::militancy_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::militancy_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "militancy trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void military_spending(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::military_spending_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::military_spending_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::military_spending_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::military_spending_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "military_spending trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(uint16_t(value * 100.0f));
	}
	void administration_spending(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::administration_spending_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::administration_spending_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::administration_spending_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::administration_spending_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "administration_spending trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(uint16_t(value * 100.0f));
	}
	void education_spending(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::education_spending_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::education_spending_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::education_spending_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::education_spending_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "education_spending trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(uint16_t(value * 100.0f));
	}
	void national_provinces_occupied(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::national_provinces_occupied | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "national_provinces_occupied trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void social_spending(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::social_spending_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::social_spending_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::social_spending_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "social_spending trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(uint16_t(value * 100.0f));
	}
	void brigades_compare(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(context.this_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::brigades_compare_this | association_to_trigger_code(a)));
			} else if(context.from_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::brigades_compare_from | association_to_trigger_code(a)));
			} else {
				err.accumulated_errors += "brigades_compare trigger used in an incorrect scope type " +
					slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
					std::to_string(line) + ")\n";
				return;
			}
		} else if(context.main_slot == trigger::slot_contents::province) {
			if(context.this_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::brigades_compare_province_this | association_to_trigger_code(a)));
			} else if(context.from_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::brigades_compare_province_from | association_to_trigger_code(a)));
			} else {
				err.accumulated_errors += "brigades_compare trigger used in an incorrect scope type " +
					slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
					std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "brigades_compare trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void rich_tax(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::rich_tax | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "rich_tax trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(uint16_t(value * 100.0f));
	}
	void middle_tax(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::middle_tax | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "middle_tax trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(uint16_t(value * 100.0f));
	}
	void poor_tax(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::poor_tax | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "poor_tax trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(uint16_t(value * 100.0f));
	}
	void mobilisation_size(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::mobilisation_size | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "mobilisation_size trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void province_id(association_type a, int32_t value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::province_id | association_to_bool_code(a)));
		} else {
			err.accumulated_errors += "province_id trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		if(0 <= value && size_t(value) < context.outer_context.original_id_to_prov_id_map.size()) {
			context.compiled_trigger.push_back(trigger::payload(context.outer_context.original_id_to_prov_id_map[value]).value);
		} else {
			err.accumulated_errors +=
					"province_id trigger given an invalid province id (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			context.compiled_trigger.push_back(trigger::payload(dcon::province_id()).value);
		}
	}
	void invention(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {

		if(auto it = context.outer_context.map_of_technologies.find(std::string(value));
				it != context.outer_context.map_of_technologies.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::technology | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "invention trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}

			context.compiled_trigger.push_back(uint16_t(1 + 1)); // data size; if no payload add code | trigger_codes::no_payload
			context.compiled_trigger.push_back(trigger::payload(it->second.id).value);
		} else if(auto itb = context.outer_context.map_of_inventions.find(std::string(value));
							itb != context.outer_context.map_of_inventions.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::invention | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "invention trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}

			context.compiled_trigger.push_back(trigger::payload(itb->second.id).value);
		} else {
			err.accumulated_errors += "invention trigger supplied with neither a valid technology nor a valid invention (" +
																err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}
	void big_producer(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_commodity_names.find(std::string(value));
				it != context.outer_context.map_of_commodity_names.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::big_producer | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "big_producer trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}

			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors +=
					"big_producer trigger supplied with an invalid commodity (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}
	void strata(association_type a, std::string_view value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::pop) {
			if(is_fixed_token_ci(value.data(), value.data() + value.length(), "poor"))
				context.compiled_trigger.push_back(uint16_t(trigger::strata_poor | trigger::no_payload | association_to_bool_code(a)));
			else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "rich"))
				context.compiled_trigger.push_back(uint16_t(trigger::strata_rich | trigger::no_payload | association_to_bool_code(a)));
			else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "middle"))
				context.compiled_trigger.push_back(uint16_t(trigger::strata_middle | trigger::no_payload | association_to_bool_code(a)));
			else {
				err.accumulated_errors +=
						"strata trigger given unknown strata (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "strata trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}

	void life_rating(association_type a, int32_t value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::life_rating_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::life_rating_state | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "life_rating trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(trigger::payload(int16_t(value)).value);
	}

	void has_empty_adjacent_state(association_type a, bool value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::has_empty_adjacent_state_province | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::has_empty_adjacent_state_state | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "has_empty_adjacent_state trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}

	void state_id(association_type a, int32_t value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::state_id_province | association_to_bool_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::state_id_state | association_to_bool_code(a)));
		} else {
			err.accumulated_errors += "state_id trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		if(0 <= value && size_t(value) < context.outer_context.original_id_to_prov_id_map.size()) {
			context.compiled_trigger.push_back(trigger::payload(context.outer_context.original_id_to_prov_id_map[value]).value);
		} else {
			err.accumulated_errors +=
					"state_id trigger given an invalid province id (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			context.compiled_trigger.push_back(trigger::payload(dcon::province_id()).value);
		}
	}
	void cash_reserves(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::cash_reserves | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "cash_reserves trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void unemployment(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::unemployment_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::unemployment_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::unemployment_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::unemployment_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "unemployment trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}

	void is_slave(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_slave_nation | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_slave_state | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_slave_province | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_slave_pop | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "is_slave trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}

	void is_independant(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_independant | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "is_independant trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}

	void has_national_minority(association_type a, bool value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::has_national_minority_nation | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::has_national_minority_state | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::has_national_minority_province | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "has_national_minority trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}

	void government(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_governments.find(std::string(value));
				it != context.outer_context.map_of_governments.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::government_nation | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(uint16_t(trigger::government_pop | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "government trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}

			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors += "government trigger supplied with an invalid government type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
		}
	}

	void constructing_cb_type(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_cb_types.find(std::string(value));
				it != context.outer_context.map_of_cb_types.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::constructing_cb_type | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "constructing_cb_type trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}

			context.compiled_trigger.push_back(trigger::payload(it->second.id).value);
		} else {
			err.accumulated_errors += "constructing_cb_type trigger supplied with an invalid cb type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
		}
	}

	void can_build_factory_in_capital_state(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_factory_names.find(std::string(value));
				it != context.outer_context.map_of_factory_names.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::can_build_factory_in_capital_state | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "can_build_factory_in_capital_state trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}

			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors += "can_build_factory_in_capital_state trigger supplied with an government type (" + err.file_name +
																", line " + std::to_string(line) + ")\n";
		}
	}
	void capital(association_type a, int32_t value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::capital | association_to_bool_code(a)));
		} else {
			err.accumulated_errors += "capital trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		if(0 <= value && size_t(value) < context.outer_context.original_id_to_prov_id_map.size()) {
			context.compiled_trigger.push_back(trigger::payload(context.outer_context.original_id_to_prov_id_map[value]).value);
		} else {
			err.accumulated_errors +=
					"capital trigger given an invalid province id (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			context.compiled_trigger.push_back(trigger::payload(dcon::province_id()).value);
		}
	}
	void tech_school(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_modifiers.find(std::string(value));
				it != context.outer_context.map_of_modifiers.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::tech_school | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "tech_school trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}

			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors +=
					"tech_school trigger supplied with an invalid school (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}
	void primary_culture(association_type a, std::string_view value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(is_this(value)) {
			if(context.main_slot == trigger::slot_contents::nation) {
				if(context.this_slot == trigger::slot_contents::nation) {
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_nation_this_nation | trigger::no_payload | association_to_bool_code(a)));
				} else if(context.this_slot == trigger::slot_contents::state) {
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_nation_this_state | trigger::no_payload | association_to_bool_code(a)));
				} else if(context.this_slot == trigger::slot_contents::province) {
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_nation_this_province | trigger::no_payload | association_to_bool_code(a)));
				} else if(context.this_slot == trigger::slot_contents::pop) {
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_nation_this_pop | trigger::no_payload | association_to_bool_code(a)));
				} else {
					err.accumulated_errors += "primary_culture trigger used in an incorrect scope type " +
						slot_contents_to_string(context.this_slot) + "(" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors += "primary_culture = this trigger used in an incorrect scope type " +
					slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(is_from(value)) {
			if(context.main_slot == trigger::slot_contents::nation && context.from_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(
						uint16_t(trigger::primary_culture_from_nation | trigger::no_payload | association_to_bool_code(a)));
			else if(context.main_slot == trigger::slot_contents::nation && context.from_slot == trigger::slot_contents::province)
				context.compiled_trigger.push_back(
						uint16_t(trigger::primary_culture_from_province | trigger::no_payload | association_to_bool_code(a)));
			else {
				err.accumulated_errors += "primary_culture = from trigger used in an incorrect scope type " +
					slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
					std::to_string(line) + ")\n";
				return;
			}
		} else if(auto it = context.outer_context.map_of_culture_names.find(std::string(value)); it != context.outer_context.map_of_culture_names.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::primary_culture | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(uint16_t(trigger::primary_culture_pop | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "primary_culture trigger used in an incorrect scope type " +
					slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}

			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors +=
					"primary_culture trigger supplied with an invalid culture: " + std::string(value) + " (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}
	void total_sunk_by_us(association_type a, int32_t value, error_handler& err, int32_t line, trigger_building_context& context) {
		// this is a trigger we don't support currently, not even in trigger codes
		context.compiled_trigger.push_back(uint16_t(trigger::always | trigger::no_payload | association_to_trigger_code(a)));
	}
	void has_crime(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_crimes.find(std::string(value)); it != context.outer_context.map_of_crimes.end()) {
			if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_crime | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "has_crime trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}

			context.compiled_trigger.push_back(trigger::payload(it->second.id).value);
		} else {
			err.accumulated_errors +=
					"has_crime trigger supplied with an invalid crime (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}
	void accepted_culture(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_culture_names.find(std::string(value));
				it != context.outer_context.map_of_culture_names.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::accepted_culture | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "accepted_culture trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}

			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors += "accepted_culture trigger supplied with an invalid culture (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
		}
	}
	void pop_majority_religion(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_religion_names.find(std::string(value));
				it != context.outer_context.map_of_religion_names.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::pop_majority_religion_nation | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::state) {
				context.compiled_trigger.push_back(uint16_t(trigger::pop_majority_religion_state | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(uint16_t(trigger::pop_majority_religion_province | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "pop_majority_religion trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors += "pop_majority_religion trigger supplied with an invalid religion (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
		}
	}
	void pop_majority_culture(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_culture_names.find(std::string(value));
				it != context.outer_context.map_of_culture_names.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::pop_majority_culture_nation | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::state) {
				context.compiled_trigger.push_back(uint16_t(trigger::pop_majority_culture_state | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(uint16_t(trigger::pop_majority_culture_province | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "pop_majority_culture trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors += "pop_majority_culture trigger supplied with an invalid culture (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
		}
	}
	void pop_majority_issue(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_ioptions.find(std::string(value));
				it != context.outer_context.map_of_ioptions.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::pop_majority_issue_nation | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::state) {
				context.compiled_trigger.push_back(uint16_t(trigger::pop_majority_issue_state | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(uint16_t(trigger::pop_majority_issue_province | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(uint16_t(trigger::pop_majority_issue_pop | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "pop_majority_issue trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second.id).value);
		} else {
			err.accumulated_errors += "pop_majority_issue trigger supplied with an invalid option name (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
		}
	}
	void pop_majority_ideology(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_ideologies.find(std::string(value));
				it != context.outer_context.map_of_ideologies.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::pop_majority_ideology_nation | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::state) {
				context.compiled_trigger.push_back(uint16_t(trigger::pop_majority_ideology_state | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(uint16_t(trigger::pop_majority_ideology_province | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(uint16_t(trigger::pop_majority_ideology_pop | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "pop_majority_ideology trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second.id).value);
		} else {
			err.accumulated_errors += "pop_majority_ideology trigger supplied with an invalid ideology (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
		}
	}
	void trade_goods_in_state(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_commodity_names.find(std::string(value));
				it != context.outer_context.map_of_commodity_names.end()) {
			if(context.main_slot == trigger::slot_contents::state) {
				context.compiled_trigger.push_back(uint16_t(trigger::trade_goods_in_state_state | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(uint16_t(trigger::trade_goods_in_state_province | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "trade_goods_in_state trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors += "trade_goods_in_state trigger supplied with an invalid commodity (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
		}
	}

	void culture(association_type a, std::string_view value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(is_this(value)) {
			if(context.main_slot == trigger::slot_contents::pop) {
				if(context.this_slot == trigger::slot_contents::nation) {
					context.compiled_trigger.push_back(
							uint16_t(trigger::culture_this_nation | trigger::no_payload | association_to_bool_code(a)));
				} else if(context.this_slot == trigger::slot_contents::state) {
					context.compiled_trigger.push_back(
							uint16_t(trigger::culture_this_state | trigger::no_payload | association_to_bool_code(a)));
				} else if(context.this_slot == trigger::slot_contents::province) {
					context.compiled_trigger.push_back(
							uint16_t(trigger::culture_this_province | trigger::no_payload | association_to_bool_code(a)));
				} else if(context.this_slot == trigger::slot_contents::pop) {
					context.compiled_trigger.push_back(
							uint16_t(trigger::culture_this_pop | trigger::no_payload | association_to_bool_code(a)));
				} else {
					err.accumulated_errors += "culture trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.this_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors += "culture = this trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(is_from(value)) {
			if(context.main_slot == trigger::slot_contents::pop && context.from_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(
						uint16_t(trigger::culture_from_nation | trigger::no_payload | association_to_bool_code(a)));
			else {
				err.accumulated_errors += "culture = from trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(is_reb(value)) {
			if(context.from_slot != trigger::slot_contents::rebel) {
				err.accumulated_errors += "culture = reb trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			} else if(context.main_slot == trigger::slot_contents::pop)
				context.compiled_trigger.push_back(
						uint16_t(trigger::culture_pop_reb | trigger::no_payload | association_to_bool_code(a)));
			else if(context.main_slot == trigger::slot_contents::state)
				context.compiled_trigger.push_back(
						uint16_t(trigger::culture_state_reb | trigger::no_payload | association_to_bool_code(a)));
			else if(context.main_slot == trigger::slot_contents::province)
				context.compiled_trigger.push_back(
						uint16_t(trigger::culture_province_reb | trigger::no_payload | association_to_bool_code(a)));
			else if(context.main_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(
						uint16_t(trigger::culture_nation_reb | trigger::no_payload | association_to_bool_code(a)));
			else {
				err.accumulated_errors += "culture = reb trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(auto it = context.outer_context.map_of_culture_names.find(std::string(value)); it != context.outer_context.map_of_culture_names.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::culture_nation | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::state) {
				context.compiled_trigger.push_back(uint16_t(trigger::culture_state | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(uint16_t(trigger::culture_province | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(uint16_t(trigger::culture_pop | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "culture trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors +=
					"culture trigger supplied with an invalid culture (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}
	void has_pop_culture(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(is_this(value)) {
			if(context.this_slot == trigger::slot_contents::pop) {
				if(context.main_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::has_pop_culture_nation_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::has_pop_culture_pop_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::has_pop_culture_state_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::has_pop_culture_province_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "has_pop_culture = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors += "has_pop_culture = this trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(auto it = context.outer_context.map_of_culture_names.find(std::string(value));
							it != context.outer_context.map_of_culture_names.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_pop_culture_nation | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::state) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_pop_culture_state | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_pop_culture_province | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_pop_culture_pop | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "has_pop_culture trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors +=
					"has_pop_culture trigger supplied with an invalid culture (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}
	void has_pop_religion(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(is_this(value)) {
			if(context.this_slot == trigger::slot_contents::pop) {
				if(context.main_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::has_pop_religion_nation_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::has_pop_religion_pop_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::has_pop_religion_state_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::has_pop_religion_province_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "has_pop_religion = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors += "has_pop_religion = this trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(auto it = context.outer_context.map_of_religion_names.find(std::string(value));
							it != context.outer_context.map_of_religion_names.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_pop_religion_nation | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::state) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_pop_religion_state | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_pop_religion_province | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_pop_religion_pop | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "has_pop_religion trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors += "has_pop_religion trigger supplied with an invalid religion (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
		}
	}

	void culture_group(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(is_this(value)) {
			if(context.this_slot == trigger::slot_contents::nation) {
				if(context.main_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::culture_group_nation_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::culture_group_pop_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "culture_group = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(context.this_slot == trigger::slot_contents::state) {
				if(context.main_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::culture_group_nation_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::culture_group_pop_this_state | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "culture_group = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(context.this_slot == trigger::slot_contents::province) {
				if(context.main_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::culture_group_nation_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::culture_group_pop_this_province | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "culture_group = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(context.this_slot == trigger::slot_contents::pop) {
				if(context.main_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::culture_group_nation_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::culture_group_pop_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "culture_group = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors += "culture_group = this trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(is_from(value)) {
			if(context.from_slot == trigger::slot_contents::nation) {
				if(context.main_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::culture_group_nation_from_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::culture_group_pop_from_nation | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "culture_group = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors += "culture_group = from trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(is_reb(value)) {
			if(context.from_slot == trigger::slot_contents::rebel) {
				if(context.main_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::culture_group_reb_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::culture_group_reb_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "culture_group = reb trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors += "culture_group = reb trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(auto it = context.outer_context.map_of_culture_group_names.find(std::string(value));
							it != context.outer_context.map_of_culture_group_names.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::culture_group_nation | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(uint16_t(trigger::culture_group_pop | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(uint16_t(trigger::culture_group_province | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::state) {
				context.compiled_trigger.push_back(uint16_t(trigger::culture_group_state | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "culture_group trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors += "culture_group trigger supplied with an invalid culture group (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
		}
	}

	void religion(association_type a, std::string_view value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(is_this(value)) {
			if(context.this_slot == trigger::slot_contents::nation) {
				if(context.main_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::religion_nation_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::religion_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "religion = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(context.this_slot == trigger::slot_contents::state) {
				if(context.main_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::religion_nation_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::religion_this_state | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "religion = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(context.this_slot == trigger::slot_contents::province) {
				if(context.main_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::religion_nation_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::religion_this_province | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "religion = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(context.this_slot == trigger::slot_contents::pop) {
				if(context.main_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::religion_nation_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::religion_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "religion = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors += "religion = this trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(is_from(value)) {
			if(context.from_slot == trigger::slot_contents::nation) {
				if(context.main_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::religion_nation_from_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::religion_from_nation | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "religion = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors += "religion = from trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(is_reb(value)) {
			if(context.from_slot == trigger::slot_contents::rebel) {
				if(context.main_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::religion_nation_reb | trigger::no_payload | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(uint16_t(trigger::religion_reb | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "religion = reb trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors += "religion = reb trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(auto it = context.outer_context.map_of_religion_names.find(std::string(value));
							it != context.outer_context.map_of_religion_names.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::religion_nation | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(uint16_t(trigger::religion | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "religion trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors +=
					"religion trigger supplied with an invalid religion (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}
	void terrain(association_type a, std::string_view value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_terrain_types.find(std::string(value));
				it != context.outer_context.map_of_terrain_types.end()) {
			if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(uint16_t(trigger::terrain_pop | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(uint16_t(trigger::terrain_province | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "terrain trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second.id).value);
		} else {
			err.accumulated_errors +=
					"terrain trigger supplied with an invalid terrain (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}
	void trade_goods(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_commodity_names.find(std::string(value));
				it != context.outer_context.map_of_commodity_names.end()) {
			if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(uint16_t(trigger::trade_goods | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "trade_goods trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors +=
					"trade_goods trigger supplied with an invalid commodity (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}
	void is_secondary_power(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_secondary_power_nation | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_secondary_power_pop | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "is_secondary_power trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void has_faction(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_rebeltypes.find(std::string(value));
				it != context.outer_context.map_of_rebeltypes.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_faction_nation | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_faction_pop | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "has_faction trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second.id).value);
		} else {
			err.accumulated_errors +=
					"has_faction trigger supplied with an invalid rebel type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}
	void has_unclaimed_cores(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::has_unclaimed_cores | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "has_unclaimed_cores trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void have_core_in(association_type a, std::string_view value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(is_this(value)) {
			if(context.main_slot == trigger::slot_contents::nation && context.this_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::have_core_in_nation_this | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "have_core_in = this trigger used in an incorrect scope type " +
					slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
					std::to_string(line) + ")\n";
				return;
			}
		} else if(is_from(value)) {
			if(context.main_slot == trigger::slot_contents::nation && context.from_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::have_core_in_nation_from | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "have_core_in = from trigger used in an incorrect scope type " +
					slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
					std::to_string(line) + ")\n";
				return;
			}
		} else if(value.length() == 3) {
			if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
					it != context.outer_context.map_of_ident_names.end()) {

				if(context.main_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(uint16_t(trigger::have_core_in_nation_tag | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "have_core_in = tag trigger used in an incorrect scope type " +
						slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
				context.compiled_trigger.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors +=
					"have_core_in trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			}
		} else {
			err.accumulated_errors +=
				"have_core_in trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}
	void is_cultural_union(association_type a, std::string_view value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(is_fixed_token_ci(value.data(), value.data() + value.length(), "yes") ||
				is_fixed_token_ci(value.data(), value.data() + value.length(), "no")) {
			if(context.main_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(uint16_t(
						trigger::is_cultural_union_bool | trigger::no_payload | association_to_bool_code(a, parse_bool(value, line, err))));
			else {
				err.accumulated_errors += "is_cultural_union = bool trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(is_this(value)) {
			if(context.main_slot == trigger::slot_contents::pop && context.this_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(
						uint16_t(trigger::is_cultural_union_this_self_pop | trigger::no_payload | association_to_bool_code(a)));
			else if(context.main_slot == trigger::slot_contents::pop && context.this_slot == trigger::slot_contents::pop)
				context.compiled_trigger.push_back(
						uint16_t(trigger::is_cultural_union_pop_this_pop | trigger::no_payload | association_to_bool_code(a)));
			else if(context.main_slot == trigger::slot_contents::nation) {
				if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_cultural_union_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_cultural_union_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_cultural_union_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_cultural_union_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.from_slot == trigger::slot_contents::rebel)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_cultural_union_this_rebel | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "is_cultural_union = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors += "is_cultural_union = this trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(value.length() == 3) {
			if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
					it != context.outer_context.map_of_ident_names.end()) {

				if(context.main_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(uint16_t(trigger::is_cultural_union_tag_nation | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(uint16_t(trigger::is_cultural_union_tag_this_pop | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(uint16_t(trigger::is_cultural_union_tag_this_state | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_cultural_union_tag_this_province | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(uint16_t(trigger::is_cultural_union_tag_this_nation | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "is_cultural_union = tag trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
				context.compiled_trigger.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors +=
						"is_cultural_union trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			}
		} else {
			err.accumulated_errors +=
					"is_cultural_union trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}
	void can_build_factory(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::can_build_factory_nation | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::can_build_factory_province | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::can_build_factory_pop | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "can_build_factory trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void war(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::war_nation | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::war_pop | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "war trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void war_exhaustion(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::war_exhaustion_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::war_exhaustion_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::war_exhaustion_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "war_exhaustion trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value / 100.0f);
	}
	void blockade(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::blockade | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "blockade trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void owns(association_type a, int32_t value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::owns | association_to_bool_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::owns_province | association_to_bool_code(a)));
		} else {
			err.accumulated_errors += "owns trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		if(0 <= value && size_t(value) < context.outer_context.original_id_to_prov_id_map.size()) {
			context.compiled_trigger.push_back(trigger::payload(context.outer_context.original_id_to_prov_id_map[value]).value);
		} else {
			err.accumulated_errors +=
					"owns trigger given an invalid province id (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			context.compiled_trigger.push_back(trigger::payload(dcon::province_id()).value);
		}
	}
	void controls(association_type a, int32_t value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::controls | association_to_bool_code(a)));
		} else {
			err.accumulated_errors += "controls trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		if(0 <= value && size_t(value) < context.outer_context.original_id_to_prov_id_map.size()) {
			context.compiled_trigger.push_back(trigger::payload(context.outer_context.original_id_to_prov_id_map[value]).value);
		} else {
			err.accumulated_errors +=
					"controls trigger given an invalid province id (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			context.compiled_trigger.push_back(trigger::payload(dcon::province_id()).value);
		}
	}

	void is_core(association_type a, std::string_view value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(is_this(value) && context.main_slot == trigger::slot_contents::province) {
			if(context.this_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(
						uint16_t(trigger::is_core_this_nation | trigger::no_payload | association_to_bool_code(a)));
			} else if(context.this_slot == trigger::slot_contents::state) {
				context.compiled_trigger.push_back(
						uint16_t(trigger::is_core_this_state | trigger::no_payload | association_to_bool_code(a)));
			} else if(context.this_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(
						uint16_t(trigger::is_core_this_province | trigger::no_payload | association_to_bool_code(a)));
			} else if(context.this_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(
						uint16_t(trigger::is_core_this_pop | trigger::no_payload | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "is_core = this trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(is_from(value) && context.main_slot == trigger::slot_contents::province) {
			if(context.from_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(
						uint16_t(trigger::is_core_from_nation | trigger::no_payload | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "is_core = from trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(is_reb(value) && context.main_slot == trigger::slot_contents::province) {
			if(context.from_slot == trigger::slot_contents::rebel) {
				context.compiled_trigger.push_back(uint16_t(trigger::is_core_reb | trigger::no_payload | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "is_core = reb trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(is_integer(value.data(), value.data() + value.length())) {
			if(context.main_slot == trigger::slot_contents::nation) {
				auto int_value = parse_int(value, line, err);
				if(0 <= int_value && size_t(int_value) < context.outer_context.original_id_to_prov_id_map.size()) {
					context.compiled_trigger.push_back(uint16_t(trigger::is_core_integer | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(context.outer_context.original_id_to_prov_id_map[int_value]).value);
				} else {
					err.accumulated_errors +=
							"owns trigger given an invalid province id (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					context.compiled_trigger.push_back(trigger::payload(dcon::province_id()).value);
				}
			} else {
				err.accumulated_errors += "is_core trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(value.length() == 3 && context.main_slot == trigger::slot_contents::province) {
			if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
					it != context.outer_context.map_of_ident_names.end()) {
				context.compiled_trigger.push_back(uint16_t(trigger::is_core_tag | association_to_bool_code(a)));
				context.compiled_trigger.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors +=
						"is_core trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			}
		} else {
			err.accumulated_errors +=
					"is_core trigger supplied with an invalid argument (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}
	void num_of_revolts(association_type a, int32_t value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::num_of_revolts | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "num_of_revolts trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(trigger::payload(uint16_t(value)).value);
	}
	void revolt_percentage(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::revolt_percentage | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "revolt_percentage trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void num_of_cities(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(is_this(value) && context.main_slot == trigger::slot_contents::nation) {
			if(context.this_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(
						uint16_t(trigger::num_of_cities_this_nation | trigger::no_payload | association_to_trigger_code(a)));
			} else if(context.this_slot == trigger::slot_contents::state) {
				context.compiled_trigger.push_back(
						uint16_t(trigger::num_of_cities_this_province | trigger::no_payload | association_to_trigger_code(a)));
			} else if(context.this_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(
						uint16_t(trigger::num_of_cities_this_state | trigger::no_payload | association_to_trigger_code(a)));
			} else if(context.this_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(
						uint16_t(trigger::num_of_cities_this_pop | trigger::no_payload | association_to_trigger_code(a)));
			} else {
				err.accumulated_errors += "num_of_cities = this trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(is_from(value) && context.main_slot == trigger::slot_contents::nation) {
			if(context.from_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(
						uint16_t(trigger::num_of_cities_from_nation | trigger::no_payload | association_to_trigger_code(a)));
			} else {
				err.accumulated_errors += "num_of_cities = from trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::num_of_cities_int | association_to_trigger_code(a)));
			context.compiled_trigger.push_back(trigger::payload(uint16_t(parse_uint(value, line, err))).value);
		} else {
			err.accumulated_errors += "num_of_cities trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void num_of_ports(association_type a, int32_t value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::num_of_ports | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "num_of_ports trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(trigger::payload(uint16_t(value)).value);
	}
	void num_of_allies(association_type a, int32_t value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::num_of_allies | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "num_of_allies trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(trigger::payload(uint16_t(value)).value);
	}
	void num_of_vassals(association_type a, int32_t value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::num_of_vassals | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "num_of_vassals trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(trigger::payload(uint16_t(value)).value);
	}
	void owned_by(association_type a, std::string_view value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::owned_by_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::owned_by_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::owned_by_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::owned_by_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "owned_by = this trigger used in an incorrect scope type " +
						slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::owned_by_from_nation | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "owned_by = from trigger used in an incorrect scope type " +
						slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::owned_by_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
						"owned_by trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors +=
					"owned_by trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(context.main_slot == trigger::slot_contents::state) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::owned_by_state_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::owned_by_state_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::owned_by_state_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::owned_by_state_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "owned_by = this trigger used in an incorrect scope type " +
						slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::owned_by_state_from_nation | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "owned_by = from trigger used in an incorrect scope type " +
						slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::owned_by_state_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
						"owned_by trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors +=
					"owned_by trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "owned_by trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}

	void exists(association_type a, std::string_view value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(is_fixed_token_ci(value.data(), value.data() + value.length(), "yes") ||
				is_fixed_token_ci(value.data(), value.data() + value.length(), "no")) {
			if(context.main_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(
						uint16_t(trigger::exists_bool | trigger::no_payload | association_to_bool_code(a, parse_bool(value, line, err))));
			else {
				err.accumulated_errors += "exists = bool trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(value.length() == 3) {
			if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
					it != context.outer_context.map_of_ident_names.end()) {
				context.compiled_trigger.push_back(uint16_t(trigger::exists_tag | association_to_bool_code(a)));
				context.compiled_trigger.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors +=
						"exists trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			}

		} else {
			err.accumulated_errors +=
					"exists trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void has_country_flag(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::has_country_flag | association_to_bool_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::has_country_flag_pop | association_to_bool_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::has_country_flag_province | association_to_bool_code(a)));
		} else {
			err.accumulated_errors += "has_country_flag trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(trigger::payload(context.outer_context.get_national_flag(std::string(value))).value);
	}
	void has_global_flag(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		context.compiled_trigger.push_back(uint16_t(trigger::has_global_flag | association_to_bool_code(a)));
		context.compiled_trigger.push_back(trigger::payload(context.outer_context.get_global_flag(std::string(value))).value);
	}

	void continent(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation) {
					context.compiled_trigger.push_back(
							uint16_t(trigger::continent_nation_this | trigger::no_payload | association_to_bool_code(a)));
				} else {
					err.accumulated_errors += "continent = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation) {
					context.compiled_trigger.push_back(
							uint16_t(trigger::continent_nation_from | trigger::no_payload | association_to_bool_code(a)));
				} else {
					err.accumulated_errors += "continent = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(auto it = context.outer_context.map_of_modifiers.find(std::string(value));
								it != context.outer_context.map_of_modifiers.end()) {
				context.compiled_trigger.push_back(uint16_t(trigger::continent_nation | association_to_bool_code(a)));
				context.compiled_trigger.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors +=
						"continent trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(context.main_slot == trigger::slot_contents::state) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation) {
					context.compiled_trigger.push_back(
							uint16_t(trigger::continent_state_this | trigger::no_payload | association_to_bool_code(a)));
				} else {
					err.accumulated_errors += "continent = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation) {
					context.compiled_trigger.push_back(
							uint16_t(trigger::continent_state_from | trigger::no_payload | association_to_bool_code(a)));
				} else {
					err.accumulated_errors += "continent = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(auto it = context.outer_context.map_of_modifiers.find(std::string(value));
								it != context.outer_context.map_of_modifiers.end()) {
				context.compiled_trigger.push_back(uint16_t(trigger::continent_state | association_to_bool_code(a)));
				context.compiled_trigger.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors +=
						"continent trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(context.main_slot == trigger::slot_contents::province) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation) {
					context.compiled_trigger.push_back(
							uint16_t(trigger::continent_province_this | trigger::no_payload | association_to_bool_code(a)));
				} else {
					err.accumulated_errors += "continent = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation) {
					context.compiled_trigger.push_back(
							uint16_t(trigger::continent_province_from | trigger::no_payload | association_to_bool_code(a)));
				} else {
					err.accumulated_errors += "continent = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(auto it = context.outer_context.map_of_modifiers.find(std::string(value));
								it != context.outer_context.map_of_modifiers.end()) {
				context.compiled_trigger.push_back(uint16_t(trigger::continent_province | association_to_bool_code(a)));
				context.compiled_trigger.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors +=
						"continent trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(context.main_slot == trigger::slot_contents::pop) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation) {
					context.compiled_trigger.push_back(
							uint16_t(trigger::continent_pop_this | trigger::no_payload | association_to_bool_code(a)));
				} else {
					err.accumulated_errors += "continent = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation) {
					context.compiled_trigger.push_back(
							uint16_t(trigger::continent_pop_from | trigger::no_payload | association_to_bool_code(a)));
				} else {
					err.accumulated_errors += "continent = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(auto it = context.outer_context.map_of_modifiers.find(std::string(value));
								it != context.outer_context.map_of_modifiers.end()) {
				context.compiled_trigger.push_back(uint16_t(trigger::continent_pop | association_to_bool_code(a)));
				context.compiled_trigger.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors +=
						"continent trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "continent trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void casus_belli(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::casus_belli_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::casus_belli_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::casus_belli_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::casus_belli_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "casus_belli = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::casus_belli_from | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "casus_belli = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::casus_belli_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"casus_belli trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors +=
						"casus_belli trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "casus_belli trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}

	void military_access(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::military_access_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::military_access_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::military_access_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::military_access_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "military_access = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::military_access_from | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "military_access = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::military_access_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"military_access trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors +=
						"military_access trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "military_access trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}

	void prestige(association_type a, std::string_view value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::prestige_this_pop | trigger::no_payload | association_to_trigger_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::prestige_this_state | trigger::no_payload | association_to_trigger_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::prestige_this_province | trigger::no_payload | association_to_trigger_code(a)));
				else if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::prestige_this_nation | trigger::no_payload | association_to_trigger_code(a)));
				else {
					err.accumulated_errors += "prestige = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::prestige_from | trigger::no_payload | association_to_trigger_code(a)));
				else {
					err.accumulated_errors += "prestige = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else {
				float fvalue = parse_float(value, line, err);
				context.compiled_trigger.push_back(uint16_t(trigger::prestige_value | association_to_trigger_code(a)));
				context.add_float_to_payload(fvalue);
			}
		} else {
			err.accumulated_errors += "prestige trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void badboy(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context);
	void has_building(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::state) {
			if(is_fixed_token_ci(value.data(), value.data() + value.length(), "factory"))
				context.compiled_trigger.push_back(
						uint16_t(trigger::has_building_factory | trigger::no_payload | association_to_bool_code(a)));
			else if(auto it = context.outer_context.map_of_factory_names.find(std::string(value));
							it != context.outer_context.map_of_factory_names.end()) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_building_state | association_to_bool_code(a)));
				context.compiled_trigger.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors +=
						"has_building trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(context.main_slot == trigger::slot_contents::province) {
			if(is_fixed_token_ci(value.data(), value.data() + value.length(), "fort")) {
				context.compiled_trigger.push_back(
						uint16_t(trigger::has_building_fort | trigger::no_payload | association_to_bool_code(a)));
			} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "railroad")) {
				context.compiled_trigger.push_back(
						uint16_t(trigger::has_building_railroad | trigger::no_payload | association_to_bool_code(a)));
			} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "naval_base")) {
				context.compiled_trigger.push_back(
						uint16_t(trigger::has_building_naval_base | trigger::no_payload | association_to_bool_code(a)));
			} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "factory")) {
				context.compiled_trigger.push_back(
						uint16_t(trigger::has_building_factory_from_province | trigger::no_payload | association_to_bool_code(a)));
			} else if(auto it = context.outer_context.map_of_factory_names.find(std::string(value));
								it != context.outer_context.map_of_factory_names.end()) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_building_state_from_province | association_to_bool_code(a)));
				context.compiled_trigger.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors +=
						"has_building trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "has_building trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void empty(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::empty | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::empty_state | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "empty trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void is_blockaded(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_blockaded | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "is_blockaded trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}

	void has_country_modifier(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_modifiers.find(std::string(value));
				it != context.outer_context.map_of_modifiers.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_country_modifier | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_country_modifier_province | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "has_country_modifier trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors += "has_country_modifier trigger supplied with an invalid modifier (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
		}
	}
	void has_province_modifier(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_modifiers.find(std::string(value));
				it != context.outer_context.map_of_modifiers.end()) {
			if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_province_modifier | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "has_province_modifier trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors += "has_province_modifier trigger supplied with an invalid modifier (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
		}
	}
	void nationalvalue(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_modifiers.find(std::string(value));
				it != context.outer_context.map_of_modifiers.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::nationalvalue_nation | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(uint16_t(trigger::nationalvalue_province | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(uint16_t(trigger::nationalvalue_pop | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "nationalvalue trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors +=
					"nationalvalue trigger supplied with an invalid modifier (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}
	void region(association_type a, std::string_view value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_state_names.find(std::string(value));
				it != context.outer_context.map_of_state_names.end()) {
			if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(uint16_t(trigger::region | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "region trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																	"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors +=
					"region trigger supplied with an state name (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}

	void tag(association_type a, std::string_view value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::tag_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::tag_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "tag = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::tag_from_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.from_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::tag_from_province | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "tag = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::tag_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"tag trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors +=
						"tag trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(context.main_slot == trigger::slot_contents::pop) {
			if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::tag_pop | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"tag trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors +=
						"tag trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "tag trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																" (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void stronger_army_than(association_type a, std::string_view value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::stronger_army_than_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
						"stronger_army_than trigger supplied with an invalid tag " + std::string(value) + " (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors +=
					"stronger_army_than trigger supplied with an invalid value " + std::string(value) + " (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "stronger_army_than trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
				" (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void neighbour(association_type a, std::string_view value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::neighbour_this | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::neighbour_this_province | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "neighbour = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::neighbour_from | trigger::no_payload | association_to_bool_code(a)));
				else if(context.from_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::neighbour_from_province | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "neighbour = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::neighbour_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"neighbour trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors +=
						"neighbour trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "neighbour trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void units_in_province(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::units_in_province_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::units_in_province_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::units_in_province_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::units_in_province_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "units_in_province = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::units_in_province_from | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "units_in_province = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_integer(value.data(), value.data() + value.length())) {
				context.compiled_trigger.push_back(uint16_t(trigger::units_in_province_value | association_to_trigger_code(a)));
				context.compiled_trigger.push_back(trigger::payload(uint16_t(parse_uint(value, line, err))).value);
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::units_in_province_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
						"units_in_province trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			} else {

			}
		} else {
			err.accumulated_errors += "units_in_province trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void war_with(association_type a, std::string_view value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::war_with_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::war_with_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::war_with_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::war_with_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "war_with = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::war_with_from | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "war_with = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::war_with_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"war_with trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors +=
						"war_with trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "war_with trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void unit_in_battle(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::unit_in_battle | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "unit_in_battle trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void total_amount_of_divisions(association_type a, int32_t value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::total_amount_of_divisions | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "total_amount_of_divisions trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(trigger::payload(uint16_t(value)).value);
	}
	void money(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::money | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "money trigger used in an incorrect scope type " + slot_contents_to_string(context.main_slot) +
																"(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void lost_national(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::lost_national | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "lost_national trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void is_vassal(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::is_vassal | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "is_vassal trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void ruling_party_ideology(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_ideologies.find(std::string(value));
				it != context.outer_context.map_of_ideologies.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::ruling_party_ideology_nation | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(uint16_t(trigger::ruling_party_ideology_pop | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "ruling_party_ideology trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second.id).value);
		} else {
			err.accumulated_errors += "ruling_party_ideology trigger supplied with an invalid ideology (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
		}
	}
	void ruling_party(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context);
	void has_leader(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context);
	void is_ideology_enabled(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_ideologies.find(std::string(value));
				it != context.outer_context.map_of_ideologies.end()) {
			context.compiled_trigger.push_back(uint16_t(trigger::is_ideology_enabled | association_to_bool_code(a)));
			context.compiled_trigger.push_back(trigger::payload(it->second.id).value);
		} else {
			err.accumulated_errors += "is_ideology_enabled trigger supplied with an invalid ideology (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
		}
	}
	void political_reform_want(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::political_reform_want_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::political_reform_want_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "political_reform_want trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void social_reform_want(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::social_reform_want_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::social_reform_want_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "social_reform_want trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void total_amount_of_ships(association_type a, int32_t value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::total_amount_of_ships | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "total_amount_of_ships trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(trigger::payload(uint16_t(value)).value);
	}
	void plurality(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::plurality | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::plurality_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "plurality trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value * 100.0f);
	}
	void corruption(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::corruption | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "corruption trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void is_state_religion(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_state_religion_state | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_state_religion_province | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_state_religion_pop | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "is_state_religion trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void is_primary_culture(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(is_this(value)) {
			if(context.main_slot == trigger::slot_contents::nation) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_nation_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_nation_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_nation_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_nation_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "is_primary_culture = this trigger used in an incorrect scope type " +
						slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
			} else if(context.main_slot == trigger::slot_contents::state) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_state_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_state_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_state_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_state_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "is_primary_culture = this trigger used in an incorrect scope type " +
						slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
			} else if(context.main_slot == trigger::slot_contents::province) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_province_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_province_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_province_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_province_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "is_primary_culture = this trigger used in an incorrect scope type " +
						slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
			} else if(context.main_slot == trigger::slot_contents::pop) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_pop_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_pop_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_pop_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_primary_culture_pop_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "is_primary_culture = this trigger used in an incorrect scope type " +
						slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors += "is_primary_culture = this trigger used in an incorrect scope type " +
					slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
					std::to_string(line) + ")\n";
				return;
			}
		} else if(auto it = context.outer_context.map_of_culture_names.find(std::string(value)); it != context.outer_context.map_of_culture_names.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::primary_culture | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(uint16_t(trigger::culture_pop | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "is_primary_culture (treated as primary_culture or culture) trigger used in an incorrect scope type " +
					slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			bool v = parse_bool(value, line, err);
			if(context.main_slot == trigger::slot_contents::state)
				context.compiled_trigger.push_back(
						uint16_t(trigger::is_primary_culture_state | trigger::no_payload | association_to_bool_code(a, v)));
			else if(context.main_slot == trigger::slot_contents::province)
				context.compiled_trigger.push_back(
						uint16_t(trigger::is_primary_culture_province | trigger::no_payload | association_to_bool_code(a, v)));
			else if(context.main_slot == trigger::slot_contents::pop)
				context.compiled_trigger.push_back(
						uint16_t(trigger::is_primary_culture_pop | trigger::no_payload | association_to_bool_code(a, v)));
			else {
				err.accumulated_errors += "is_primary_culture trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		}
	}
	void is_accepted_culture(association_type a, std::string_view value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(is_this(value)) {
			if(context.main_slot == trigger::slot_contents::nation) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_accepted_culture_nation_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_accepted_culture_nation_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_accepted_culture_nation_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_accepted_culture_nation_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "is_accepted_culture = this trigger used in an incorrect scope type " +
						slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
			} else if(context.main_slot == trigger::slot_contents::state) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_accepted_culture_state_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_accepted_culture_state_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_accepted_culture_state_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_accepted_culture_state_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "is_accepted_culture = this trigger used in an incorrect scope type " +
						slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
			} else if(context.main_slot == trigger::slot_contents::province) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_accepted_culture_province_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_accepted_culture_province_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_accepted_culture_province_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_accepted_culture_province_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "is_accepted_culture = this trigger used in an incorrect scope type " +
						slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
			} else if(context.main_slot == trigger::slot_contents::pop) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_accepted_culture_pop_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_accepted_culture_pop_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_accepted_culture_pop_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_accepted_culture_pop_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "is_accepted_culture = this trigger used in an incorrect scope type " +
						slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors += "is_accepted_culture = this trigger used in an incorrect scope type " +
					slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
					std::to_string(line) + ")\n";
				return;
			}
		} else if(auto it = context.outer_context.map_of_culture_names.find(std::string(value)); it != context.outer_context.map_of_culture_names.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::accepted_culture | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "is_accepted_culture (treated as accepted_culture) trigger used in an incorrect scope type " +
					slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			bool v = parse_bool(value, line, err);
			if(context.main_slot == trigger::slot_contents::state) {
				context.compiled_trigger.push_back(
						uint16_t(trigger::is_accepted_culture_state | trigger::no_payload | association_to_bool_code(a, v)));
			} else if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(
						uint16_t(trigger::is_accepted_culture_province | trigger::no_payload | association_to_bool_code(a, v)));
			} else if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(
						uint16_t(trigger::is_accepted_culture_pop | trigger::no_payload | association_to_bool_code(a, v)));
			} else {
				err.accumulated_errors += "is_accepted_culture trigger used in an incorrect scope type " +
					slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
					std::to_string(line) + ")\n";
				return;
			}
		}
	}
	void is_coastal(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_coastal | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "is_coastal trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void in_sphere(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::in_sphere_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::in_sphere_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::in_sphere_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::in_sphere_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "in_sphere = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::in_sphere_from | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "in_sphere = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::in_sphere_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"in_sphere trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors +=
						"in_sphere trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "in_sphere trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void produces(association_type a, std::string_view value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_commodity_names.find(std::string(value));
				it != context.outer_context.map_of_commodity_names.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::produces_nation | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::state) {
				context.compiled_trigger.push_back(uint16_t(trigger::produces_state | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(uint16_t(trigger::produces_province | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(uint16_t(trigger::produces_pop | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "produces trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors +=
					"produces trigger supplied with an invalid commodity (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}
	void has_pop_type(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_poptypes.find(std::string(value));
				it != context.outer_context.map_of_poptypes.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_pop_type_nation | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::state) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_pop_type_state | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_pop_type_province | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(uint16_t(trigger::has_pop_type_pop | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "has_pop_type trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors +=
					"has_pop_type trigger supplied with an invalid commodity (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}
	void total_pops(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::total_pops_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::total_pops_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::total_pops_province | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::total_pops_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "total_pops trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void average_militancy(association_type a, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::average_militancy_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::average_militancy_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::average_militancy_province | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "average_militancy trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void average_consciousness(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::average_consciousness_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(uint16_t(trigger::average_consciousness_state | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::average_consciousness_province | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "average_consciousness trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void is_next_reform(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_ioptions.find(std::string(value));
				it != context.outer_context.map_of_ioptions.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::is_next_reform_nation | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(uint16_t(trigger::is_next_reform_pop | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "is_next_reform trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(it->second.id).value);
		} else if(auto itb = context.outer_context.map_of_roptions.find(std::string(value));
							itb != context.outer_context.map_of_roptions.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_trigger.push_back(uint16_t(trigger::is_next_rreform_nation | association_to_bool_code(a)));
			} else if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_trigger.push_back(uint16_t(trigger::is_next_rreform_pop | association_to_bool_code(a)));
			} else {
				err.accumulated_errors += "is_next_reform trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(itb->second.id).value);
		} else {
			err.accumulated_errors += "is_next_reform trigger supplied with an invalid issue/reform (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
		}
	}
	void rebel_power_fraction(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::rebel_power_fraction | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "rebel_power_fraction trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void recruited_percentage(association_type a, float value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_trigger.push_back(uint16_t(trigger::recruited_percentage_nation | association_to_trigger_code(a)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(uint16_t(trigger::recruited_percentage_pop | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "recruited_percentage trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.add_float_to_payload(value);
	}
	void has_culture_core(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::has_culture_core | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::province && context.this_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::has_culture_core_province_this_pop | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "has_culture_core trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void nationalism(association_type a, int32_t value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(uint16_t(trigger::nationalism | association_to_trigger_code(a)));
		} else {
			err.accumulated_errors += "nationalism trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(trigger::payload(uint16_t(value)).value);
	}
	void is_overseas(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_overseas | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_overseas_pop | trigger::no_payload | association_to_bool_code(a, value)));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::is_overseas_state | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "is_overseas trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void controlled_by_rebels(association_type a, bool value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_trigger.push_back(
					uint16_t(trigger::controlled_by_rebels | trigger::no_payload | association_to_bool_code(a, value)));
		} else {
			err.accumulated_errors += "controlled_by_rebels trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void controlled_by(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::controlled_by_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::controlled_by_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::controlled_by_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::controlled_by_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "controlled_by = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::controlled_by_from | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "controlled_by = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_reb(value)) {
				if(context.from_slot == trigger::slot_contents::rebel)
					context.compiled_trigger.push_back(
							uint16_t(trigger::controlled_by_reb | trigger::no_payload | association_to_bool_code(a)));
				else {
					context.compiled_trigger.push_back(
							uint16_t(trigger::controlled_by_rebels | trigger::no_payload | association_to_bool_code(a)));
				}
			} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "owner")) {
				context.compiled_trigger.push_back(
						uint16_t(trigger::controlled_by_owner | trigger::no_payload | association_to_bool_code(a)));
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::controlled_by_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"controlled_by trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors +=
						"controlled_by trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "controlled_by trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void truce_with(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::truce_with_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::truce_with_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::truce_with_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::truce_with_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "truce_with = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::truce_with_from | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "truce_with = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::truce_with_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"truce_with trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors +=
						"truce_with trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "truce_with trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void is_sphere_leader_of(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_sphere_leader_of_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_sphere_leader_of_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_sphere_leader_of_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_sphere_leader_of_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "is_sphere_leader_of = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_sphere_leader_of_from | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "is_sphere_leader_of = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::is_sphere_leader_of_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors += "is_sphere_leader_of trigger supplied with an invalid tag (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors += "is_sphere_leader_of trigger supplied with an invalid value (" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "is_sphere_leader_of trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void constructing_cb(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::constructing_cb_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::constructing_cb_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::constructing_cb_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::constructing_cb_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "constructing_cb = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::constructing_cb_from | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "constructing_cb = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::constructing_cb_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"constructing_cb trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors +=
						"constructing_cb trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "constructing_cb trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void vassal_of(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::vassal_of_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::vassal_of_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::vassal_of_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::vassal_of_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "vassal_of = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::vassal_of_from | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "vassal_of = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::vassal_of_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"vassal_of trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors +=
						"vassal_of trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "vassal_of trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void substate_of(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::substate_of_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::substate_of_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::substate_of_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::substate_of_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "substate_of = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::substate_of_from | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "substate_of = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::substate_of_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"substate_of trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors +=
						"substate_of trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "substate_of trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void is_our_vassal(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_our_vassal_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_our_vassal_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_our_vassal_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_our_vassal_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "is_our_vassal = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::is_our_vassal_from | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "is_our_vassal = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::is_our_vassal_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"is_our_vassal trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors +=
						"is_our_vassal trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "is_our_vassal trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void this_culture_union(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::this_culture_union_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::this_culture_union_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::this_culture_union_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::this_culture_union_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "this_culture_union = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "this_union")) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::this_culture_union_this_union_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::this_culture_union_this_union_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::this_culture_union_this_union_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::this_culture_union_this_union_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "this_culture_union = this_union trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::this_culture_union_from | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "this_culture_union = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::this_culture_union_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors += "this_culture_union trigger supplied with an invalid tag (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors += "this_culture_union trigger supplied with an invalid value (" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "this_culture_union trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void alliance_with(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::alliance_with_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::alliance_with_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::alliance_with_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::alliance_with_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "alliance_with = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::alliance_with_from | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "alliance_with = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::alliance_with_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"alliance_with trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors +=
						"alliance_with trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "alliance_with trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void in_default(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::in_default_this_nation | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::in_default_this_state | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::in_default_this_province | trigger::no_payload | association_to_bool_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::in_default_this_pop | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "in_default = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::in_default_from | trigger::no_payload | association_to_bool_code(a)));
				else {
					err.accumulated_errors += "in_default = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_trigger.push_back(uint16_t(trigger::in_default_tag | association_to_bool_code(a)));
					context.compiled_trigger.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"in_default trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			} else {
				err.accumulated_errors +=
						"in_default trigger supplied with an invalid value (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "in_default trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void industrial_score(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::industrial_score_this_nation | trigger::no_payload | association_to_trigger_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::industrial_score_this_state | trigger::no_payload | association_to_trigger_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::industrial_score_this_province | trigger::no_payload | association_to_trigger_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::industrial_score_this_pop | trigger::no_payload | association_to_trigger_code(a)));
				else {
					err.accumulated_errors += "industrial_score = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::industrial_score_from_nation | trigger::no_payload | association_to_trigger_code(a)));
				else {
					err.accumulated_errors += "industrial_score = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else {
				context.compiled_trigger.push_back(uint16_t(trigger::industrial_score_value | association_to_trigger_code(a)));
				context.compiled_trigger.push_back(trigger::payload(uint16_t(parse_uint(value, line, err))).value);
			}
		} else {
			err.accumulated_errors += "industrial_score trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void military_score(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::military_score_this_nation | trigger::no_payload | association_to_trigger_code(a)));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(
							uint16_t(trigger::military_score_this_state | trigger::no_payload | association_to_trigger_code(a)));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(
							uint16_t(trigger::military_score_this_province | trigger::no_payload | association_to_trigger_code(a)));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(
							uint16_t(trigger::military_score_this_pop | trigger::no_payload | association_to_trigger_code(a)));
				else {
					err.accumulated_errors += "military_score = this trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(
							uint16_t(trigger::military_score_from_nation | trigger::no_payload | association_to_trigger_code(a)));
				else {
					err.accumulated_errors += "military_score = from trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else {
				context.compiled_trigger.push_back(uint16_t(trigger::military_score_value | association_to_trigger_code(a)));
				context.compiled_trigger.push_back(trigger::payload(uint16_t(parse_uint(value, line, err))).value);
			}
		} else {
			err.accumulated_errors += "military_score trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void is_possible_vassal(association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(value.length() == 3) {
			if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
					it != context.outer_context.map_of_ident_names.end()) {
				if(context.main_slot == trigger::slot_contents::nation) {
					context.compiled_trigger.push_back(uint16_t(trigger::is_possible_vassal | association_to_bool_code(a)));
				} else {
					err.accumulated_errors += "is_possible_vassal trigger used in an incorrect scope type " +
																		slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
				context.compiled_trigger.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors += "is_possible_vassal trigger supplied with an invalid tag (" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
			}
		} else {
			err.accumulated_errors +=
					"is_possible_vassal trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}

	void diplomatic_influence(tr_diplomatic_influence const& value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot != trigger::slot_contents::nation) {
			err.accumulated_errors += "diplomatic_influence trigger used in an incorrect scope type " +
																slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		} else if(is_from(value.who)) {
			if(context.from_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(
						uint16_t(trigger::diplomatic_influence_from_nation | association_to_trigger_code(value.a)));
			else if(context.from_slot == trigger::slot_contents::province)
				context.compiled_trigger.push_back(
						uint16_t(trigger::diplomatic_influence_from_province | association_to_trigger_code(value.a)));
			else {
				err.accumulated_errors += "diplomatic_influence trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(uint16_t(value.value_)).value);
		} else if(is_this(value.who)) {
			if(context.this_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(
						uint16_t(trigger::diplomatic_influence_this_nation | association_to_trigger_code(value.a)));
			else if(context.this_slot == trigger::slot_contents::province)
				context.compiled_trigger.push_back(
						uint16_t(trigger::diplomatic_influence_this_province | association_to_trigger_code(value.a)));
			else {
				err.accumulated_errors += "diplomatic_influence trigger used in an incorrect scope type " +
																	slot_contents_to_string(context.main_slot) + "(" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(uint16_t(value.value_)).value);
		} else if(value.who.length() == 3) {
			if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value.who[0], value.who[1], value.who[2]));
					it != context.outer_context.map_of_ident_names.end()) {
				context.compiled_trigger.push_back(uint16_t(trigger::diplomatic_influence_tag | association_to_trigger_code(value.a)));
				context.compiled_trigger.push_back(trigger::payload(uint16_t(value.value_)).value);
				context.compiled_trigger.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors += "diplomatic_influence trigger supplied with an invalid tag (" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
			}
		} else {
			err.accumulated_errors += "diplomatic_influence trigger supplied with an invalid tag (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void pop_unemployment(tr_pop_unemployment const& value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(is_this(value.type)) {
			if(context.this_slot != trigger::slot_contents::pop) {
				err.accumulated_errors += "pop_unemployment = this trigger used in an invalid context (" + err.file_name + ", line " +
					std::to_string(line) + ")\n";
				return;
			} else if(context.main_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(
						uint16_t(trigger::pop_unemployment_nation_this_pop | association_to_trigger_code(value.a)));
			else if(context.main_slot == trigger::slot_contents::state)
				context.compiled_trigger.push_back(
						uint16_t(trigger::pop_unemployment_state_this_pop | association_to_trigger_code(value.a)));
			else if(context.main_slot == trigger::slot_contents::province)
				context.compiled_trigger.push_back(
						uint16_t(trigger::pop_unemployment_province_this_pop | association_to_trigger_code(value.a)));
			else {
				err.accumulated_errors += "pop_unemployment = this trigger used in an invalid context (" + err.file_name + ", line " +
					std::to_string(line) + ")\n";
				return;
			}
			context.add_float_to_payload(value.value_);
		} else if(is_from(value.type)) {
			if(context.this_slot != trigger::slot_contents::pop) {
				err.accumulated_errors += "pop_unemployment = this trigger used in an invalid context (" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			} else if(context.main_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(
						uint16_t(trigger::pop_unemployment_nation_this_pop | association_to_trigger_code(value.a)));
			else if(context.main_slot == trigger::slot_contents::state)
				context.compiled_trigger.push_back(
						uint16_t(trigger::pop_unemployment_state_this_pop | association_to_trigger_code(value.a)));
			else if(context.main_slot == trigger::slot_contents::province)
				context.compiled_trigger.push_back(
						uint16_t(trigger::pop_unemployment_province_this_pop | association_to_trigger_code(value.a)));
			else {
				err.accumulated_errors += "pop_unemployment = this trigger used in an invalid context (" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.add_float_to_payload(value.value_);
		} else if(auto it = context.outer_context.map_of_poptypes.find(std::string(value.type));
							it != context.outer_context.map_of_poptypes.end()) {
			if(context.main_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(uint16_t(trigger::pop_unemployment_nation | association_to_trigger_code(value.a)));
			else if(context.main_slot == trigger::slot_contents::state)
				context.compiled_trigger.push_back(uint16_t(trigger::pop_unemployment_state | association_to_trigger_code(value.a)));
			else if(context.main_slot == trigger::slot_contents::province)
				context.compiled_trigger.push_back(uint16_t(trigger::pop_unemployment_province | association_to_trigger_code(value.a)));
			else if(context.main_slot == trigger::slot_contents::pop)
				context.compiled_trigger.push_back(uint16_t(trigger::pop_unemployment_pop | association_to_trigger_code(value.a)));
			else {
				err.accumulated_errors +=
						"pop_unemployment trigger used in an invalid context (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}

			context.add_float_to_payload(value.value_);
			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors += "pop_unemployment trigger supplied with an invalid pop type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void relation(tr_relation const& value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(context.main_slot != trigger::slot_contents::nation) {
			err.accumulated_errors +=
					"relation trigger used in an invalid context (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		} else if(is_from(value.who)) {
			if(context.from_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(uint16_t(trigger::relation_from_nation | association_to_trigger_code(value.a)));
			else if(context.from_slot == trigger::slot_contents::province)
				context.compiled_trigger.push_back(uint16_t(trigger::relation_from_province | association_to_trigger_code(value.a)));
			else {
				err.accumulated_errors +=
						"relation = from trigger used in an invalid context (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(int16_t(value.value_)).value);
		} else if(is_this(value.who)) {
			if(context.this_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(uint16_t(trigger::relation_this_nation | association_to_trigger_code(value.a)));
			else if(context.this_slot == trigger::slot_contents::province)
				context.compiled_trigger.push_back(uint16_t(trigger::relation_this_province | association_to_trigger_code(value.a)));
			else {
				err.accumulated_errors +=
						"relation = this trigger used in an invalid context (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(int16_t(value.value_)).value);
		} else if(value.who.length() == 3) {
			if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value.who[0], value.who[1], value.who[2]));
					it != context.outer_context.map_of_ident_names.end()) {
				context.compiled_trigger.push_back(uint16_t(trigger::relation_tag | association_to_trigger_code(value.a)));
				context.compiled_trigger.push_back(trigger::payload(int16_t(value.value_)).value);
				context.compiled_trigger.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors +=
						"relation trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			}
		} else {
			err.accumulated_errors +=
					"relation trigger supplied with an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void check_variable(tr_check_variable const& value, error_handler& err, int32_t line, trigger_building_context& context) {
		context.compiled_trigger.push_back(uint16_t(trigger::check_variable | association_to_trigger_code(value.a)));
		context.add_float_to_payload(value.value_);
		context.compiled_trigger.push_back(
				trigger::payload(context.outer_context.get_national_variable(std::string(value.which))).value);
	}
	void upper_house(tr_upper_house const& value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_ideologies.find(std::string(value.ideology));
				it != context.outer_context.map_of_ideologies.end()) {
			if(context.main_slot != trigger::slot_contents::nation) {
				err.accumulated_errors +=
						"upper_house trigger used in an invalid context (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(uint16_t(trigger::upper_house | association_to_trigger_code(value.a)));
			context.add_float_to_payload(value.value_);
			context.compiled_trigger.push_back(trigger::payload(it->second.id).value);
		} else {
			err.accumulated_errors +=
					"upper_house trigger supplied with an invalid ideology (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void unemployment_by_type(tr_unemployment_by_type const& value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_poptypes.find(std::string(value.type));
				it != context.outer_context.map_of_poptypes.end()) {
			if(context.main_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(uint16_t(trigger::unemployment_by_type_nation | association_to_trigger_code(value.a)));
			else if(context.main_slot == trigger::slot_contents::state)
				context.compiled_trigger.push_back(uint16_t(trigger::unemployment_by_type_state | association_to_trigger_code(value.a)));
			else if(context.main_slot == trigger::slot_contents::province)
				context.compiled_trigger.push_back(
						uint16_t(trigger::unemployment_by_type_province | association_to_trigger_code(value.a)));
			else if(context.main_slot == trigger::slot_contents::pop)
				context.compiled_trigger.push_back(uint16_t(trigger::unemployment_by_type_pop | association_to_trigger_code(value.a)));
			else {
				err.accumulated_errors += "unemployment_by_type trigger used in an invalid context (" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.add_float_to_payload(value.value_);
			context.compiled_trigger.push_back(trigger::payload(it->second).value);
		} else {
			err.accumulated_errors += "unemployment_by_type trigger supplied with an invalid pop type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}

	void party_loyalty(tr_party_loyalty const& value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(auto it = context.outer_context.map_of_ideologies.find(std::string(value.ideology));
				it != context.outer_context.map_of_ideologies.end()) {
			if(value.province_id != 0) {
				if(0 <= value.province_id && size_t(value.province_id) < context.outer_context.original_id_to_prov_id_map.size()) {
					if(context.main_slot == trigger::slot_contents::nation)
						context.compiled_trigger.push_back(
								uint16_t(trigger::party_loyalty_nation_province_id | association_to_trigger_code(value.a)));
					else if(context.from_slot == trigger::slot_contents::nation)
						context.compiled_trigger.push_back(
								uint16_t(trigger::party_loyalty_from_nation_province_id | association_to_trigger_code(value.a)));
					else if(context.main_slot == trigger::slot_contents::province)
						context.compiled_trigger.push_back(
								uint16_t(trigger::party_loyalty_province_province_id | association_to_trigger_code(value.a)));
					else if(context.from_slot == trigger::slot_contents::province)
						context.compiled_trigger.push_back(
								uint16_t(trigger::party_loyalty_from_province_province_id | association_to_trigger_code(value.a)));
					else {
						err.accumulated_errors +=
								"party_loyalty trigger used in an invalid context (" + err.file_name + ", line " + std::to_string(line) + ")\n";
						return;
					}
					context.compiled_trigger.push_back(
							trigger::payload(context.outer_context.original_id_to_prov_id_map[value.province_id]).value);
					context.compiled_trigger.push_back(trigger::payload(int16_t(value.value_)).value);
					context.compiled_trigger.push_back(trigger::payload(it->second.id).value);
				} else {
					err.accumulated_errors += "party_loyalty trigger supplied with an invalid province id (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else {
				if(context.main_slot == trigger::slot_contents::province) {
					if(context.from_slot == trigger::slot_contents::nation)
						context.compiled_trigger.push_back(
								uint16_t(trigger::party_loyalty_from_nation_scope_province | association_to_trigger_code(value.a)));
					else if(context.from_slot == trigger::slot_contents::province)
						context.compiled_trigger.push_back(
								uint16_t(trigger::party_loyalty_from_province_scope_province | association_to_trigger_code(value.a)));
					else
						context.compiled_trigger.push_back(uint16_t(trigger::party_loyalty_generic | association_to_trigger_code(value.a)));
				} else {
					err.accumulated_errors +=
							"party_loyalty trigger used in an invalid context (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
				context.compiled_trigger.push_back(trigger::payload(int16_t(value.value_)).value);
				context.compiled_trigger.push_back(trigger::payload(it->second.id).value);
			}
		} else {
			err.accumulated_errors +=
					"party_loyalty trigger supplied with an invalid ideology (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}

	void can_build_in_province(tr_can_build_in_province const& value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot != trigger::slot_contents::province) {
			err.accumulated_errors +=
					"can_build_in_province trigger used in an invalid context (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		} else if(value.limit_to_world_greatest_level) {
			if(context.this_slot == trigger::slot_contents::nation) {
				if(is_fixed_token_ci(value.building.data(), value.building.data() + value.building.length(), "railroad"))
					context.compiled_trigger.push_back(uint16_t(
							trigger::can_build_in_province_railroad_yes_limit_this_nation | trigger::association_eq | trigger::no_payload));
				else if(is_fixed_token_ci(value.building.data(), value.building.data() + value.building.length(), "naval_base"))
					context.compiled_trigger.push_back(uint16_t(
							trigger::can_build_in_province_naval_base_yes_limit_this_nation | trigger::association_eq | trigger::no_payload));
				else if(is_fixed_token_ci(value.building.data(), value.building.data() + value.building.length(), "fort"))
					context.compiled_trigger.push_back(uint16_t(
							trigger::can_build_in_province_fort_yes_limit_this_nation | trigger::association_eq | trigger::no_payload));
			} else if(context.from_slot == trigger::slot_contents::nation) {
				if(is_fixed_token_ci(value.building.data(), value.building.data() + value.building.length(), "railroad"))
					context.compiled_trigger.push_back(uint16_t(
							trigger::can_build_in_province_railroad_yes_limit_from_nation | trigger::association_eq | trigger::no_payload));
				else if(is_fixed_token_ci(value.building.data(), value.building.data() + value.building.length(), "naval_base"))
					context.compiled_trigger.push_back(uint16_t(
							trigger::can_build_in_province_naval_base_yes_limit_from_nation | trigger::association_eq | trigger::no_payload));
				else if(is_fixed_token_ci(value.building.data(), value.building.data() + value.building.length(), "fort"))
					context.compiled_trigger.push_back(uint16_t(
							trigger::can_build_in_province_fort_yes_limit_from_nation | trigger::association_eq | trigger::no_payload));
			} else {
				err.accumulated_errors += "can_build_in_province trigger used in an invalid context (" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else {
			if(context.this_slot == trigger::slot_contents::nation) {
				if(is_fixed_token_ci(value.building.data(), value.building.data() + value.building.length(), "railroad"))
					context.compiled_trigger.push_back(uint16_t(
							trigger::can_build_in_province_railroad_no_limit_this_nation | trigger::association_eq | trigger::no_payload));
				else if(is_fixed_token_ci(value.building.data(), value.building.data() + value.building.length(), "naval_base"))
					context.compiled_trigger.push_back(uint16_t(
							trigger::can_build_in_province_naval_base_no_limit_this_nation | trigger::association_eq | trigger::no_payload));
				else if(is_fixed_token_ci(value.building.data(), value.building.data() + value.building.length(), "fort"))
					context.compiled_trigger.push_back(
							uint16_t(trigger::can_build_in_province_fort_no_limit_this_nation | trigger::association_eq | trigger::no_payload));
			} else if(context.from_slot == trigger::slot_contents::nation) {
				if(is_fixed_token_ci(value.building.data(), value.building.data() + value.building.length(), "railroad"))
					context.compiled_trigger.push_back(uint16_t(
							trigger::can_build_in_province_railroad_no_limit_from_nation | trigger::association_eq | trigger::no_payload));
				else if(is_fixed_token_ci(value.building.data(), value.building.data() + value.building.length(), "naval_base"))
					context.compiled_trigger.push_back(uint16_t(
							trigger::can_build_in_province_naval_base_no_limit_from_nation | trigger::association_eq | trigger::no_payload));
				else if(is_fixed_token_ci(value.building.data(), value.building.data() + value.building.length(), "fort"))
					context.compiled_trigger.push_back(
							uint16_t(trigger::can_build_in_province_fort_no_limit_from_nation | trigger::association_eq | trigger::no_payload));
			} else {
				err.accumulated_errors += "can_build_in_province trigger used in an invalid context (" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		}
	}
	void can_build_railway_in_capital(tr_can_build_railway_in_capital const& value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot != trigger::slot_contents::nation) {
			err.accumulated_errors += "can_build_railway_in_capital trigger used in an invalid context (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		} else if(value.in_whole_capital_state) {
			if(value.limit_to_world_greatest_level)
				context.compiled_trigger.push_back(uint16_t(
						trigger::can_build_railway_in_capital_yes_whole_state_yes_limit | trigger::association_eq | trigger::no_payload));
			else
				context.compiled_trigger.push_back(uint16_t(
						trigger::can_build_railway_in_capital_yes_whole_state_no_limit | trigger::association_eq | trigger::no_payload));
		} else {
			if(value.limit_to_world_greatest_level)
				context.compiled_trigger.push_back(uint16_t(
						trigger::can_build_railway_in_capital_no_whole_state_yes_limit | trigger::association_eq | trigger::no_payload));
			else
				context.compiled_trigger.push_back(uint16_t(
						trigger::can_build_railway_in_capital_no_whole_state_no_limit | trigger::association_eq | trigger::no_payload));
		}
	}
	void can_build_fort_in_capital(tr_can_build_fort_in_capital const& value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		if(context.main_slot != trigger::slot_contents::nation) {
			err.accumulated_errors += "can_build_fort_in_capital trigger used in an invalid context (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		} else if(value.in_whole_capital_state) {
			if(value.limit_to_world_greatest_level)
				context.compiled_trigger.push_back(uint16_t(
						trigger::can_build_fort_in_capital_yes_whole_state_yes_limit | trigger::association_eq | trigger::no_payload));
			else
				context.compiled_trigger.push_back(uint16_t(
						trigger::can_build_fort_in_capital_yes_whole_state_no_limit | trigger::association_eq | trigger::no_payload));
		} else {
			if(value.limit_to_world_greatest_level)
				context.compiled_trigger.push_back(uint16_t(
						trigger::can_build_fort_in_capital_no_whole_state_yes_limit | trigger::association_eq | trigger::no_payload));
			else
				context.compiled_trigger.push_back(
						uint16_t(trigger::can_build_fort_in_capital_no_whole_state_no_limit | trigger::association_eq | trigger::no_payload));
		}
	}

	void work_available(tr_work_available const& value, error_handler& err, int32_t line, trigger_building_context& context) {
		if(value.pop_type_list.size() > 1 || value.pop_type_list.size() == 0) {
			err.accumulated_errors += "work_available trigger used with an unsupported number of worker types (" + err.file_name +
																", line " + std::to_string(line) + ")\n";
			return;
		}

		if(context.main_slot == trigger::slot_contents::nation)
			context.compiled_trigger.push_back(uint16_t(trigger::work_available_nation | trigger::association_eq));
		else if(context.main_slot == trigger::slot_contents::state)
			context.compiled_trigger.push_back(uint16_t(trigger::work_available_state | trigger::association_eq));
		else if(context.main_slot == trigger::slot_contents::province)
			context.compiled_trigger.push_back(uint16_t(trigger::work_available_province | trigger::association_eq));
		else {
			err.accumulated_errors +=
					"work_available trigger used in an invalid context (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		context.compiled_trigger.push_back(trigger::payload(value.pop_type_list[0]).value);
	}

	void any_value(std::string_view label, association_type a, std::string_view value, error_handler& err, int32_t line,
			trigger_building_context& context) {
		std::string str_label{label};
		if(auto it = context.outer_context.map_of_technologies.find(str_label);
				it != context.outer_context.map_of_technologies.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				bool bvalue = parse_bool(value, line, err);
				context.compiled_trigger.push_back(uint16_t(trigger::technology | association_to_bool_code(a, bvalue)));
				context.compiled_trigger.push_back(trigger::payload(it->second.id).value);
			} else if(context.main_slot == trigger::slot_contents::province) {
				bool bvalue = parse_bool(value, line, err);
				context.compiled_trigger.push_back(uint16_t(trigger::technology_province | association_to_bool_code(a, bvalue)));
				context.compiled_trigger.push_back(trigger::payload(it->second.id).value);
			} else  {
				err.accumulated_errors += "named technology trigger used in an invalid context (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			} 
		} else if(auto itb = context.outer_context.map_of_inventions.find(str_label);
							itb != context.outer_context.map_of_inventions.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				bool bvalue = parse_bool(value, line, err);
				context.compiled_trigger.push_back(uint16_t(trigger::invention | association_to_bool_code(a, bvalue)));
				context.compiled_trigger.push_back(trigger::payload(itb->second.id).value);
			} else if(context.main_slot == trigger::slot_contents::province) {
				bool bvalue = parse_bool(value, line, err);
				context.compiled_trigger.push_back(uint16_t(trigger::invention_province | association_to_bool_code(a, bvalue)));
				context.compiled_trigger.push_back(trigger::payload(itb->second.id).value);
			} else {
				err.accumulated_errors += "named invention trigger used in an invalid context (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(auto itc = context.outer_context.map_of_ideologies.find(str_label);
							itc != context.outer_context.map_of_ideologies.end()) {
			if(context.main_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(uint16_t(trigger::variable_ideology_name_nation | association_to_trigger_code(a)));
			else if(context.main_slot == trigger::slot_contents::pop)
				context.compiled_trigger.push_back(uint16_t(trigger::variable_ideology_name_pop | association_to_trigger_code(a)));
			else if(context.main_slot == trigger::slot_contents::province)
				context.compiled_trigger.push_back(uint16_t(trigger::variable_ideology_name_province | association_to_trigger_code(a)));
			else if(context.main_slot == trigger::slot_contents::state)
				context.compiled_trigger.push_back(uint16_t(trigger::variable_ideology_name_state | association_to_trigger_code(a)));
			else {
				err.accumulated_errors +=
						"named ideology trigger used in an invalid context (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(itc->second.id).value);
			context.add_float_to_payload(parse_float(value, line, err));
		} else if(auto itd = context.outer_context.map_of_poptypes.find(str_label);
							itd != context.outer_context.map_of_poptypes.end()) {
			if(context.main_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(uint16_t(trigger::variable_pop_type_name_nation | association_to_trigger_code(a)));
			else if(context.main_slot == trigger::slot_contents::pop)
				context.compiled_trigger.push_back(uint16_t(trigger::variable_pop_type_name_pop | association_to_trigger_code(a)));
			else if(context.main_slot == trigger::slot_contents::province)
				context.compiled_trigger.push_back(uint16_t(trigger::variable_pop_type_name_province | association_to_trigger_code(a)));
			else if(context.main_slot == trigger::slot_contents::state)
				context.compiled_trigger.push_back(uint16_t(trigger::variable_pop_type_name_state | association_to_trigger_code(a)));
			else {
				err.accumulated_errors +=
						"named pop type trigger used in an invalid context (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(itd->second).value);
			context.add_float_to_payload(parse_float(value, line, err));
		} else if(auto ite = context.outer_context.map_of_commodity_names.find(str_label);
							ite != context.outer_context.map_of_commodity_names.end()) {
			if(context.main_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(uint16_t(trigger::variable_good_name | association_to_trigger_code(a)));
			else {
				err.accumulated_errors +=
						"named commodity trigger used in an invalid context (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(ite->second).value);
			context.add_float_to_payload(parse_float(value, line, err));
		} else if(auto itg = context.outer_context.map_of_ioptions.find(str_label); itg != context.outer_context.map_of_ioptions.end()) {
			if(context.main_slot == trigger::slot_contents::nation)
				context.compiled_trigger.push_back(uint16_t(trigger::variable_issue_name_nation | association_to_trigger_code(a)));
			else if(context.main_slot == trigger::slot_contents::pop)
				context.compiled_trigger.push_back(uint16_t(trigger::variable_issue_name_pop | association_to_trigger_code(a)));
			else if(context.main_slot == trigger::slot_contents::province)
				context.compiled_trigger.push_back(uint16_t(trigger::variable_issue_name_province | association_to_trigger_code(a)));
			else if(context.main_slot == trigger::slot_contents::state)
				context.compiled_trigger.push_back(uint16_t(trigger::variable_issue_name_state | association_to_trigger_code(a)));
			else {
				err.accumulated_errors +=
						"named issue option trigger used in an invalid context (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
			context.compiled_trigger.push_back(trigger::payload(itg->second.id).value);
			context.add_float_to_payload(parse_float(value, line, err) / 100.0f);
		} else if(auto itf = context.outer_context.map_of_iissues.find(str_label); itf != context.outer_context.map_of_iissues.end()) {
			if(auto itopt = context.outer_context.map_of_ioptions.find(std::string(value));
					itopt != context.outer_context.map_of_ioptions.end()) {
				if(context.main_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(uint16_t(trigger::variable_issue_group_name_nation | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(uint16_t(trigger::variable_issue_group_name_pop | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(uint16_t(trigger::variable_issue_group_name_province | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(uint16_t(trigger::variable_issue_group_name_state | association_to_bool_code(a)));
				else {
					err.accumulated_errors +=
							"named issue trigger used in an invalid context (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
				context.compiled_trigger.push_back(trigger::payload(itf->second).value);
				context.compiled_trigger.push_back(trigger::payload(itopt->second.id).value);
			} else {
				err.accumulated_errors +=
						"named issue trigger used with an invalid option name (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(auto ith = context.outer_context.map_of_reforms.find(str_label); ith != context.outer_context.map_of_reforms.end()) {
			if(auto itopt = context.outer_context.map_of_roptions.find(std::string(value));
					itopt != context.outer_context.map_of_roptions.end()) {
				if(context.main_slot == trigger::slot_contents::nation)
					context.compiled_trigger.push_back(uint16_t(trigger::variable_reform_group_name_nation | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::pop)
					context.compiled_trigger.push_back(uint16_t(trigger::variable_reform_group_name_pop | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::province)
					context.compiled_trigger.push_back(uint16_t(trigger::variable_reform_group_name_province | association_to_bool_code(a)));
				else if(context.main_slot == trigger::slot_contents::state)
					context.compiled_trigger.push_back(uint16_t(trigger::variable_reform_group_name_state | association_to_bool_code(a)));
				else {
					err.accumulated_errors +=
							"named reform trigger used in an invalid context (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
				context.compiled_trigger.push_back(trigger::payload(ith->second).value);
				context.compiled_trigger.push_back(trigger::payload(itopt->second.id).value);
			} else {
				err.accumulated_errors +=
						"named reform trigger used with an invalid option name (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors +=
					"unknown key: " + str_label + " found in trigger(" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
};

void tr_scope_and(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_or(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_not(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_any_neighbor_province(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_any_neighbor_country(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_war_countries(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_any_greater_power(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_any_owned_province(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_any_core(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_all_core(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_any_state(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_any_substate(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_any_sphere_member(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_any_pop(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_owner(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_controller(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_location(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_country(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_capital_scope(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_this(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_from(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_sea_zone(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_cultural_union(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_overlord(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_sphere_owner(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_independence(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_flashpoint_tag_scope(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_crisis_state_scope(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_state_scope(token_generator& gen, error_handler& err, trigger_building_context& context);
void tr_scope_variable(std::string_view name, token_generator& gen, error_handler& err, trigger_building_context& context);

dcon::trigger_key make_trigger(token_generator& gen, error_handler& err, trigger_building_context& context);

struct value_modifier_definition {
	std::optional<float> factor;
	float base = 0.0f;
	void months(association_type, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		factor = value * 30.0f;
	}
	void years(association_type, float value, error_handler& err, int32_t line, trigger_building_context& context) {
		factor = value * 365.0f;
	}
	void group(value_modifier_definition const& value, error_handler& err, int32_t line, trigger_building_context& context) { }
	void finish(trigger_building_context&) { }
};

void make_value_modifier_segment(token_generator& gen, error_handler& err, trigger_building_context& context);
dcon::value_modifier_key make_value_modifier(token_generator& gen, error_handler& err, trigger_building_context& context);

} // namespace parsers
