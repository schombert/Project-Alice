#pragma once
#include "parsers.hpp"
#include "script_constants.hpp"
#include "trigger_parsing.hpp"
#include "text.hpp"

namespace economy {
dcon::modifier_id get_province_selector_modifier(sys::state& state);
dcon::modifier_id get_province_immigrator_modifier(sys::state& state);
}

namespace parsers {

struct effect_building_context {
	scenario_building_context& outer_context;
	std::vector<uint16_t> compiled_effect;
	size_t limit_position = 0;

	trigger::slot_contents main_slot = trigger::slot_contents::empty;
	trigger::slot_contents this_slot = trigger::slot_contents::empty;
	trigger::slot_contents from_slot = trigger::slot_contents::empty;

	effect_building_context(scenario_building_context& outer_context, trigger::slot_contents main_slot,
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

		compiled_effect.push_back(pack_float.v.low);
		compiled_effect.push_back(pack_float.v.high);
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

		compiled_effect.push_back(pack_int.v.low);
		compiled_effect.push_back(pack_int.v.high);
	}
};

struct ef_trigger_revolt {
	dcon::culture_id culture_;
	dcon::religion_id religion_;
	dcon::ideology_id ideology_;
	dcon::rebel_type_id type_;
	void culture(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_culture_names.find(std::string(value));
				it != context.outer_context.map_of_culture_names.end()) {
			culture_ = it->second;
		} else {
			err.accumulated_errors +=
					"Invalid culture " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void religion(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_religion_names.find(std::string(value));
				it != context.outer_context.map_of_religion_names.end()) {
			religion_ = it->second;
		} else {
			err.accumulated_errors +=
					"Invalid religion " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void ideology(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_ideologies.find(std::string(value));
				it != context.outer_context.map_of_ideologies.end()) {
			ideology_ = it->second.id;
		} else {
			err.accumulated_errors +=
					"Invalid ideology " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void type(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_rebeltypes.find(std::string(value));
				it != context.outer_context.map_of_rebeltypes.end()) {
			type_ = it->second.id;
		} else {
			err.accumulated_errors +=
					"Invalid rebel type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(effect_building_context&) { }
};

struct ef_diplomatic_influence {
	std::string_view who;
	int32_t value = 0;
	void finish(effect_building_context&) { }
};
struct ef_relation {
	std::string_view who;
	int32_t value = 0;
	void finish(effect_building_context&) { }
};
struct ef_add_province_modifier {
	dcon::modifier_id name_;
	int32_t duration = 0;
	void name(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_modifiers.find(std::string(value));
				it != context.outer_context.map_of_modifiers.end()) {
			name_ = it->second;
		} else {
			err.accumulated_errors +=
					"Invalid modifier " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(effect_building_context&) { }
};
struct ef_add_country_modifier {
	dcon::modifier_id name_;
	int32_t duration = 0;
	void name(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_modifiers.find(std::string(value));
				it != context.outer_context.map_of_modifiers.end()) {
			name_ = it->second;
		} else {
			err.accumulated_errors +=
					"Invalid modifier " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(effect_building_context&) { }
};
struct ef_casus_belli {
	dcon::cb_type_id type_;
	std::string_view target;
	int32_t months = 0;
	void type(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_cb_types.find(std::string(value));
				it != context.outer_context.map_of_cb_types.end()) {
			type_ = it->second.id;
		} else {
			err.accumulated_errors +=
					"Invalid cb type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(effect_building_context&) { }
};
struct ef_add_casus_belli {
	dcon::cb_type_id type_;
	std::string_view target;
	int32_t months = 0;
	void type(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_cb_types.find(std::string(value));
				it != context.outer_context.map_of_cb_types.end()) {
			type_ = it->second.id;
		} else {
			err.accumulated_errors +=
					"Invalid cb type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(effect_building_context&) { }
};
struct ef_remove_casus_belli {
	dcon::cb_type_id type_;
	std::string_view target;
	void type(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_cb_types.find(std::string(value));
				it != context.outer_context.map_of_cb_types.end()) {
			type_ = it->second.id;
		} else {
			err.accumulated_errors +=
					"Invalid cb type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(effect_building_context&) { }
};
struct ef_this_remove_casus_belli {
	dcon::cb_type_id type_;
	std::string_view target;
	void type(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_cb_types.find(std::string(value));
				it != context.outer_context.map_of_cb_types.end()) {
			type_ = it->second.id;
		} else {
			err.accumulated_errors +=
					"Invalid cb type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(effect_building_context&) { }
};
struct ef_wargoal {
	dcon::cb_type_id casus_belli_;
	dcon::national_identity_id country_;
	bool target_country_is_this = false;
	bool target_country_is_from = false;
	dcon::province_id state_province_id_;
	void country(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(is_this(value)) {
			target_country_is_this = true;
		} else if(is_from(value)) {
			target_country_is_from = true;
		} else if(value.length() == 3) {
			if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
					it != context.outer_context.map_of_ident_names.end()) {
				country_ = it->second;
			} else {
				err.accumulated_errors += "wargoal given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "wargoal given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void casus_belli(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(auto it = context.outer_context.map_of_cb_types.find(std::string(value));
				it != context.outer_context.map_of_cb_types.end()) {
			casus_belli_ = it->second.id;
		} else {
			err.accumulated_errors +=
					"Invalid cb type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void state_province_id(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context) {
		if(0 <= value && size_t(value) < context.outer_context.original_id_to_prov_id_map.size()) {
			state_province_id_ = context.outer_context.original_id_to_prov_id_map[value];
		} else {
			err.accumulated_errors +=
					"wargoal given an invalid province id (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}
	void finish(effect_building_context&) { }
};
struct ef_war {
	bool call_ally = true;
	std::string_view target;
	ef_wargoal defender_goal;
	ef_wargoal attacker_goal;
	void finish(effect_building_context&) { }
};
struct ef_country_event {
	int32_t days = -1;
	dcon::national_event_id id_;
	void id(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context);
	void finish(effect_building_context&) { }
};
struct ef_province_event {
	int32_t days = -1;
	dcon::provincial_event_id id_;
	void id(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context);
	void finish(effect_building_context&) { }
};
struct ef_sub_unit {
	std::string_view value;
	dcon::unit_type_id type_;
	void type(association_type t, std::string_view v, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_unit_types.find(std::string(v));
				it != context.outer_context.map_of_unit_types.end()) {
			type_ = it->second;
		} else {
			err.accumulated_errors +=
					"Invalid unit type " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(effect_building_context&) { }
};
struct ef_set_variable {
	float value = 0.0f;
	dcon::national_variable_id which_;
	void which(association_type t, std::string_view v, error_handler& err, int32_t line, effect_building_context& context) {
		which_ = context.outer_context.get_national_variable(std::string(v));
	}
	void finish(effect_building_context&) { }
};
struct ef_change_variable {
	float value = 0.0f;
	dcon::national_variable_id which_;
	void which(association_type t, std::string_view v, error_handler& err, int32_t line, effect_building_context& context) {
		which_ = context.outer_context.get_national_variable(std::string(v));
	}
	void finish(effect_building_context&) { }
};
struct ef_ideology {
	float factor = 0.0f;
	dcon::ideology_id value_;
	void value(association_type t, std::string_view v, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_ideologies.find(std::string(v));
				it != context.outer_context.map_of_ideologies.end()) {
			value_ = it->second.id;
		} else {
			err.accumulated_errors +=
					"Invalid ideology " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(effect_building_context&) { }
};
struct ef_dominant_issue {
	float factor = 0.0f;
	dcon::issue_option_id value_;
	void value(association_type t, std::string_view v, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_ioptions.find(std::string(v)); it != context.outer_context.map_of_ioptions.end()) {
			value_ = it->second.id;
		} else {
			err.accumulated_errors +=
					"Invalid issue option " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(effect_building_context&) { }
};
struct ef_upper_house {
	float value = 0.0f;
	dcon::ideology_id ideology_;
	void ideology(association_type t, std::string_view v, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_ideologies.find(std::string(v));
				it != context.outer_context.map_of_ideologies.end()) {
			ideology_ = it->second.id;
		} else {
			err.accumulated_errors +=
					"Invalid ideology " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(effect_building_context&) { }
};
struct ef_scaled_militancy {
	float factor = 0.0f;
	float unemployment = 0.0f;
	dcon::ideology_id ideology_;
	dcon::issue_option_id issue_;
	void ideology(association_type t, std::string_view v, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_ideologies.find(std::string(v));
				it != context.outer_context.map_of_ideologies.end()) {
			ideology_ = it->second.id;
		} else {
			err.accumulated_errors +=
					"Invalid ideology " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void issue(association_type t, std::string_view v, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_ioptions.find(std::string(v)); it != context.outer_context.map_of_ioptions.end()) {
			issue_ = it->second.id;
		} else {
			err.accumulated_errors +=
					"Invalid issue option " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(effect_building_context&) { }
};
struct ef_scaled_consciousness {
	float factor = 0.0f;
	float unemployment = 0.0f;
	dcon::ideology_id ideology_;
	dcon::issue_option_id issue_;
	void ideology(association_type t, std::string_view v, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_ideologies.find(std::string(v));
				it != context.outer_context.map_of_ideologies.end()) {
			ideology_ = it->second.id;
		} else {
			err.accumulated_errors +=
					"Invalid ideology " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void issue(association_type t, std::string_view v, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_ioptions.find(std::string(v)); it != context.outer_context.map_of_ioptions.end()) {
			issue_ = it->second.id;
		} else {
			err.accumulated_errors +=
					"Invalid issue option " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(effect_building_context&) { }
};
struct ef_define_general {
	std::string_view name;
	dcon::leader_trait_id background_;
	dcon::leader_trait_id personality_;
	void background(association_type t, std::string_view v, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_leader_traits.find(std::string(v));
				it != context.outer_context.map_of_leader_traits.end()) {
			background_ = it->second;
		} else {
			err.accumulated_errors +=
					"Invalid leader trait " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void personality(association_type t, std::string_view v, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_leader_traits.find(std::string(v));
				it != context.outer_context.map_of_leader_traits.end()) {
			personality_ = it->second;
		} else {
			err.accumulated_errors +=
					"Invalid leader trait " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(effect_building_context&) { }
};
struct ef_define_admiral {
	std::string_view name;
	dcon::leader_trait_id background_;
	dcon::leader_trait_id personality_;
	void background(association_type t, std::string_view v, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_leader_traits.find(std::string(v));
				it != context.outer_context.map_of_leader_traits.end()) {
			background_ = it->second;
		} else {
			err.accumulated_errors +=
					"Invalid leader trait " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void personality(association_type t, std::string_view v, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_leader_traits.find(std::string(v));
				it != context.outer_context.map_of_leader_traits.end()) {
			personality_ = it->second;
		} else {
			err.accumulated_errors +=
					"Invalid leader trait " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(effect_building_context&) { }
};
struct ef_add_war_goal {
	dcon::cb_type_id casus_belli_;
	void casus_belli(association_type t, std::string_view v, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_cb_types.find(std::string(v)); it != context.outer_context.map_of_cb_types.end()) {
			casus_belli_ = it->second.id;
		} else {
			err.accumulated_errors +=
					"Invalid cb type " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(effect_building_context&) { }
};
struct ef_move_issue_percentage {
	float value = 0.0f;
	dcon::issue_option_id from_;
	dcon::issue_option_id to_;
	void from(association_type t, std::string_view v, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_ioptions.find(std::string(v)); it != context.outer_context.map_of_ioptions.end()) {
			from_ = it->second.id;
		} else {
			err.accumulated_errors +=
					"Invalid issue option " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void to(association_type t, std::string_view v, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_ioptions.find(std::string(v)); it != context.outer_context.map_of_ioptions.end()) {
			to_ = it->second.id;
		} else {
			err.accumulated_errors +=
					"Invalid issue option " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(effect_building_context&) { }
};
struct ef_party_loyalty {
	float loyalty_value = 0.0f;
	dcon::province_id province_id_;
	dcon::ideology_id ideology_;
	void ideology(association_type t, std::string_view v, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_ideologies.find(std::string(v));
				it != context.outer_context.map_of_ideologies.end()) {
			ideology_ = it->second.id;
		} else {
			err.accumulated_errors +=
					"Invalid ideology " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void province_id(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context) {
		if(0 <= value && size_t(value) < context.outer_context.original_id_to_prov_id_map.size()) {
			province_id_ = context.outer_context.original_id_to_prov_id_map[value];
		} else {
			err.accumulated_errors +=
					"party_loyalty given an invalid province id (" + err.file_name + ", line " + std::to_string(line) + ")\n";
		}
	}
	void finish(effect_building_context&) { }
};
struct ef_build_railway_in_capital {
	bool limit_to_world_greatest_level = false;
	bool in_whole_capital_state = false;
	void finish(effect_building_context&) { }
};
struct ef_build_fort_in_capital {
	bool limit_to_world_greatest_level = false;
	bool in_whole_capital_state = false;
	void finish(effect_building_context&) { }
};
struct ef_build_bank_in_capital {
	bool limit_to_world_greatest_level = false;
	bool in_whole_capital_state = false;
	void finish(effect_building_context&) { }
};
struct ef_build_university_in_capital {
	bool limit_to_world_greatest_level = false;
	bool in_whole_capital_state = false;
	void finish(effect_building_context&) { }
};

struct ef_random_list {
	int32_t chances_sum = 0;
	void any_group(std::string_view label, int32_t value, error_handler& err, int32_t line, effect_building_context& context) {
		chances_sum += value;
	}
	void finish(effect_building_context&) { }
};

struct effect_body {
	int32_t chance = 0;
	void finish(effect_building_context&) { }
	void capital(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::capital));
		} else {
			err.accumulated_errors +=
					"capital effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		if(0 <= value && size_t(value) < context.outer_context.original_id_to_prov_id_map.size()) {
			context.compiled_effect.push_back(trigger::payload(context.outer_context.original_id_to_prov_id_map[value]).value);
		} else {
			err.accumulated_errors +=
					"capital effect given an invalid province id (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			context.compiled_effect.push_back(trigger::payload(dcon::province_id()).value);
		}
	}
	void add_core(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(is_integer(value.data(), value.data() + value.length())) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_effect.push_back(uint16_t(effect::add_core_int));
				auto ivalue = parse_int(value, line, err);
				if(0 <= ivalue && size_t(ivalue) < context.outer_context.original_id_to_prov_id_map.size()) {
					context.compiled_effect.push_back(trigger::payload(context.outer_context.original_id_to_prov_id_map[ivalue]).value);
				} else {
					err.accumulated_errors +=
							"add_core = int effect given an invalid province id (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					context.compiled_effect.push_back(trigger::payload(dcon::province_id()).value);
				}
			} else {
				err.accumulated_errors +=
						"add_core = int effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			if(context.main_slot == trigger::slot_contents::province) {
				if(is_reb(value)) {
					if(context.from_slot == trigger::slot_contents::rebel)
						context.compiled_effect.push_back(uint16_t(effect::add_core_reb | effect::no_payload));
					else {
						err.accumulated_errors += "add_core = reb effect used in an incorrect scope type (" + err.file_name + ", line " +
																			std::to_string(line) + ")\n";
						return;
					}
				} else if(is_from(value)) {
					if(context.from_slot == trigger::slot_contents::nation)
						context.compiled_effect.push_back(uint16_t(effect::add_core_from_nation | effect::no_payload));
					else if(context.from_slot == trigger::slot_contents::province)
						context.compiled_effect.push_back(uint16_t(effect::add_core_from_province | effect::no_payload));
					else {
						err.accumulated_errors += "add_core = from effect used in an incorrect scope type (" + err.file_name + ", line " +
																			std::to_string(line) + ")\n";
						return;
					}
				} else if(is_this(value)) {
					if(context.this_slot == trigger::slot_contents::nation)
						context.compiled_effect.push_back(uint16_t(effect::add_core_this_nation | effect::no_payload));
					else if(context.this_slot == trigger::slot_contents::province)
						context.compiled_effect.push_back(uint16_t(effect::add_core_this_province | effect::no_payload));
					else if(context.this_slot == trigger::slot_contents::state)
						context.compiled_effect.push_back(uint16_t(effect::add_core_this_state | effect::no_payload));
					else if(context.this_slot == trigger::slot_contents::pop)
						context.compiled_effect.push_back(uint16_t(effect::add_core_this_pop | effect::no_payload));
					else {
						err.accumulated_errors += "add_core = this effect used in an incorrect scope type (" + err.file_name + ", line " +
																			std::to_string(line) + ")\n";
						return;
					}
				} else if(value.length() == 3) {
					if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
							it != context.outer_context.map_of_ident_names.end()) {
						context.compiled_effect.push_back(uint16_t(effect::add_core_tag));
						context.compiled_effect.push_back(trigger::payload(it->second).value);
					} else {
						err.accumulated_errors +=
								"add_core effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
						return;
					}
				} else {
					err.accumulated_errors +=
							"add_core effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else if(context.main_slot == trigger::slot_contents::state) {
				if(is_reb(value)) {
					if(context.from_slot == trigger::slot_contents::rebel)
						context.compiled_effect.push_back(uint16_t(effect::add_core_state_reb | effect::no_payload));
					else {
						err.accumulated_errors += "add_core = reb effect used in an incorrect scope type (" + err.file_name + ", line " +
							std::to_string(line) + ")\n";
						return;
					}
				} else if(is_from(value)) {
					if(context.from_slot == trigger::slot_contents::nation)
						context.compiled_effect.push_back(uint16_t(effect::add_core_state_from_nation | effect::no_payload));
					else if(context.from_slot == trigger::slot_contents::province)
						context.compiled_effect.push_back(uint16_t(effect::add_core_state_from_province | effect::no_payload));
					else {
						err.accumulated_errors += "add_core = from effect used in an incorrect scope type (" + err.file_name + ", line " +
							std::to_string(line) + ")\n";
						return;
					}
				} else if(is_this(value)) {
					if(context.this_slot == trigger::slot_contents::nation)
						context.compiled_effect.push_back(uint16_t(effect::add_core_state_this_nation | effect::no_payload));
					else if(context.this_slot == trigger::slot_contents::province)
						context.compiled_effect.push_back(uint16_t(effect::add_core_state_this_province | effect::no_payload));
					else if(context.this_slot == trigger::slot_contents::state)
						context.compiled_effect.push_back(uint16_t(effect::add_core_state_this_state | effect::no_payload));
					else if(context.this_slot == trigger::slot_contents::pop)
						context.compiled_effect.push_back(uint16_t(effect::add_core_state_this_pop | effect::no_payload));
					else {
						err.accumulated_errors += "add_core = this effect used in an incorrect scope type (" + err.file_name + ", line " +
							std::to_string(line) + ")\n";
						return;
					}
				} else if(value.length() == 3) {
					if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
							it != context.outer_context.map_of_ident_names.end()) {
						context.compiled_effect.push_back(uint16_t(effect::add_core_tag_state));
						context.compiled_effect.push_back(trigger::payload(it->second).value);
					} else {
						err.accumulated_errors +=
								"add_core effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
						return;
					}
				} else {
					err.accumulated_errors +=
							"add_core effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors +=
						"add_core effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		}
	}
	void remove_core(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(is_integer(value.data(), value.data() + value.length())) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_effect.push_back(uint16_t(effect::remove_core_int));
				auto ivalue = parse_int(value, line, err);
				if(0 <= ivalue && size_t(ivalue) < context.outer_context.original_id_to_prov_id_map.size()) {
					context.compiled_effect.push_back(trigger::payload(context.outer_context.original_id_to_prov_id_map[ivalue]).value);
				} else {
					err.accumulated_errors += "remove_core = int effect given an invalid province id (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					context.compiled_effect.push_back(trigger::payload(dcon::province_id()).value);
				}
			} else {
				err.accumulated_errors += "remove_core = int effect used in an incorrect scope type (" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else {
			if(context.main_slot == trigger::slot_contents::province) {
				if(is_reb(value)) {
					if(context.from_slot == trigger::slot_contents::rebel)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_reb | effect::no_payload));
					else {
						err.accumulated_errors += "remove_core = reb effect used in an incorrect scope type (" + err.file_name + ", line " +
																			std::to_string(line) + ")\n";
						return;
					}
				} else if(is_from(value)) {
					if(context.from_slot == trigger::slot_contents::nation)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_from_nation | effect::no_payload));
					else if(context.from_slot == trigger::slot_contents::province)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_from_province | effect::no_payload));
					else {
						err.accumulated_errors += "remove_core = from effect used in an incorrect scope type (" + err.file_name + ", line " +
																			std::to_string(line) + ")\n";
						return;
					}
				} else if(is_this(value)) {
					if(context.this_slot == trigger::slot_contents::nation)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_this_nation | effect::no_payload));
					else if(context.this_slot == trigger::slot_contents::province)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_this_province | effect::no_payload));
					else if(context.this_slot == trigger::slot_contents::state)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_this_state | effect::no_payload));
					else if(context.this_slot == trigger::slot_contents::pop)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_this_pop | effect::no_payload));
					else {
						err.accumulated_errors += "remove_core = this effect used in an incorrect scope type (" + err.file_name + ", line " +
																			std::to_string(line) + ")\n";
						return;
					}
				} else if(value.length() == 3) {
					if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
							it != context.outer_context.map_of_ident_names.end()) {
						context.compiled_effect.push_back(uint16_t(effect::remove_core_tag));
						context.compiled_effect.push_back(trigger::payload(it->second).value);
					} else {
						err.accumulated_errors +=
								"remove_core effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
						return;
					}
				} else {
					err.accumulated_errors +=
							"remove_core effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else if(context.main_slot == trigger::slot_contents::state) {
				if(is_reb(value)) {
					if(context.from_slot == trigger::slot_contents::rebel)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_state_reb | effect::no_payload));
					else {
						err.accumulated_errors += "remove_core = reb effect used in an incorrect scope type (" + err.file_name + ", line " +
							std::to_string(line) + ")\n";
						return;
					}
				} else if(is_from(value)) {
					if(context.from_slot == trigger::slot_contents::nation)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_state_from_nation | effect::no_payload));
					else if(context.from_slot == trigger::slot_contents::province)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_state_from_province | effect::no_payload));
					else {
						err.accumulated_errors += "remove_core = from effect used in an incorrect scope type (" + err.file_name + ", line " +
							std::to_string(line) + ")\n";
						return;
					}
				} else if(is_this(value)) {
					if(context.this_slot == trigger::slot_contents::nation)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_state_this_nation | effect::no_payload));
					else if(context.this_slot == trigger::slot_contents::province)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_state_this_province | effect::no_payload));
					else if(context.this_slot == trigger::slot_contents::state)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_state_this_state | effect::no_payload));
					else if(context.this_slot == trigger::slot_contents::pop)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_state_this_pop | effect::no_payload));
					else {
						err.accumulated_errors += "remove_core = this effect used in an incorrect scope type (" + err.file_name + ", line " +
							std::to_string(line) + ")\n";
						return;
					}
				} else if(value.length() == 3) {
					if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
							it != context.outer_context.map_of_ident_names.end()) {
						context.compiled_effect.push_back(uint16_t(effect::remove_core_tag_state));
						context.compiled_effect.push_back(trigger::payload(it->second).value);
					} else {
						err.accumulated_errors +=
								"remove_core effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
						return;
					}
				} else {
					err.accumulated_errors +=
							"remove_core effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else if(context.main_slot == trigger::slot_contents::nation) {
				if(is_reb(value)) {
					if(context.from_slot == trigger::slot_contents::rebel)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_nation_reb | effect::no_payload));
					else {
						err.accumulated_errors += "remove_core = reb effect used in an incorrect scope type (" + err.file_name + ", line " +
							std::to_string(line) + ")\n";
						return;
					}
				} else if(is_from(value)) {
					if(context.from_slot == trigger::slot_contents::nation)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_nation_from_nation | effect::no_payload));
					else if(context.from_slot == trigger::slot_contents::province)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_nation_from_province | effect::no_payload));
					else {
						err.accumulated_errors += "remove_core = from effect used in an incorrect scope type (" + err.file_name + ", line " +
							std::to_string(line) + ")\n";
						return;
					}
				} else if(is_this(value)) {
					if(context.this_slot == trigger::slot_contents::nation)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_nation_this_nation | effect::no_payload));
					else if(context.this_slot == trigger::slot_contents::province)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_nation_this_province | effect::no_payload));
					else if(context.this_slot == trigger::slot_contents::state)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_nation_this_state | effect::no_payload));
					else if(context.this_slot == trigger::slot_contents::pop)
						context.compiled_effect.push_back(uint16_t(effect::remove_core_nation_this_pop | effect::no_payload));
					else {
						err.accumulated_errors += "remove_core = this effect used in an incorrect scope type (" + err.file_name + ", line " +
							std::to_string(line) + ")\n";
						return;
					}
				} else if(value.length() == 3) {
					if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
							it != context.outer_context.map_of_ident_names.end()) {
						context.compiled_effect.push_back(uint16_t(effect::remove_core_tag_nation));
						context.compiled_effect.push_back(trigger::payload(it->second).value);
					} else {
						err.accumulated_errors +=
							"remove_core effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
						return;
					}
				} else {
					err.accumulated_errors +=
						"remove_core effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors +=
						"remove_core effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		}
	}
	void change_region_name(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_effect.push_back(uint16_t(effect::change_region_name_state));
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_effect.push_back(uint16_t(effect::change_region_name_province));
		} else {
			err.accumulated_errors += "change_region_name effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		auto name = text::find_or_add_key(context.outer_context.state, value);
		context.add_int32_t_to_payload(name.index());
	}
	void trade_goods(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(auto it = context.outer_context.map_of_commodity_names.find(std::string(value));
				it != context.outer_context.map_of_commodity_names.end()) {
			if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_effect.push_back(uint16_t(effect::trade_goods));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors +=
						"trade_goods effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "trade_goods effect supplied with invalid commodity name " + std::string(value) + " (" +
																err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void add_accepted_culture(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_fixed_token_ci(value.data(), value.data() + value.length(), "union")) {
				context.compiled_effect.push_back(uint16_t(effect::add_accepted_culture_union | effect::no_payload));
			} else if(auto it = context.outer_context.map_of_culture_names.find(std::string(value));
								it != context.outer_context.map_of_culture_names.end()) {
				context.compiled_effect.push_back(uint16_t(effect::add_accepted_culture));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors += "add_accepted_culture effect supplied with invalid culture name " + std::string(value) + " (" +
																	err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "add_accepted_culture effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void primary_culture(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::primary_culture_from_nation | effect::no_payload));
				else {
					err.accumulated_errors += "primary_culture = from effect used in an incorrect scope type (" + err.file_name +
																		", line " + std::to_string(line) + ")\n";
					return;
				}
			} else if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::primary_culture_this_nation | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_effect.push_back(uint16_t(effect::primary_culture_this_state | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::primary_culture_this_province | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_effect.push_back(uint16_t(effect::primary_culture_this_pop | effect::no_payload));
				else {
					err.accumulated_errors += "primary_culture = this effect used in an incorrect scope type (" + err.file_name +
																		", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				if(auto it = context.outer_context.map_of_culture_names.find(std::string(value));
						it != context.outer_context.map_of_culture_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::primary_culture));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors += "primary_culture effect supplied with invalid culture name " + std::string(value) + " (" +
																		err.file_name + ", line " + std::to_string(line) + ")\n";
				}
			}
		} else {
			err.accumulated_errors +=
					"primary_culture effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void remove_accepted_culture(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(auto it = context.outer_context.map_of_culture_names.find(std::string(value));
					it != context.outer_context.map_of_culture_names.end()) {
				context.compiled_effect.push_back(uint16_t(effect::remove_accepted_culture));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors += "remove_accepted_culture effect supplied with invalid culture name " + std::string(value) +
																	" (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "remove_accepted_culture effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void life_rating(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_effect.push_back(uint16_t(effect::life_rating));
			context.compiled_effect.push_back(trigger::payload(int16_t(value)).value);
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_effect.push_back(uint16_t(effect::life_rating_state));
			context.compiled_effect.push_back(trigger::payload(int16_t(value)).value);
		} else {
			err.accumulated_errors +=
					"life_rating effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void religion(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(auto it = context.outer_context.map_of_religion_names.find(std::string(value));
					it != context.outer_context.map_of_religion_names.end()) {
				context.compiled_effect.push_back(uint16_t(effect::religion));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors += "religion effect supplied with invalid religion name " + std::string(value) + " (" +
																	err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors +=
					"religion effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void is_slave(association_type t, bool value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::state) {
			if(value)
				context.compiled_effect.push_back(uint16_t(effect::is_slave_state_yes | effect::no_payload));
			else
				context.compiled_effect.push_back(uint16_t(effect::is_slave_state_no | effect::no_payload));
		} else if(context.main_slot == trigger::slot_contents::province) {
			if(value)
				context.compiled_effect.push_back(uint16_t(effect::is_slave_province_yes | effect::no_payload));
			else
				context.compiled_effect.push_back(uint16_t(effect::is_slave_province_no | effect::no_payload));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			if(value)
				context.compiled_effect.push_back(uint16_t(effect::is_slave_pop_yes | effect::no_payload));
			else
				context.compiled_effect.push_back(uint16_t(effect::is_slave_pop_no | effect::no_payload));
		} else {
			err.accumulated_errors +=
					"is_slave effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void research_points(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::research_points));
			context.compiled_effect.push_back(trigger::payload(int16_t(value)).value);
		} else {
			err.accumulated_errors +=
					"research_points effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void tech_school(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(auto it = context.outer_context.map_of_modifiers.find(std::string(value));
					it != context.outer_context.map_of_modifiers.end()) {
				context.compiled_effect.push_back(uint16_t(effect::tech_school));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors += "tech_school effect supplied with invalid modifier name " + std::string(value) + " (" +
																	err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors +=
					"tech_school effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void government(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_reb(value)) {
				if(context.from_slot == trigger::slot_contents::rebel)
					context.compiled_effect.push_back(uint16_t(effect::government_reb | effect::no_payload));
				else {
					err.accumulated_errors += "government = reb effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(auto it = context.outer_context.map_of_governments.find(std::string(value));
								it != context.outer_context.map_of_governments.end()) {
				context.compiled_effect.push_back(uint16_t(effect::government));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors += "government effect supplied with invalid government name " + std::string(value) + " (" +
																	err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors +=
					"government effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void treasury(association_type t, float value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::treasury));
			context.add_float_to_payload(value);
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_effect.push_back(uint16_t(effect::treasury_province));
			context.add_float_to_payload(value);
		} else {
			err.accumulated_errors +=
					"treasury effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void war_exhaustion(association_type t, float value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::war_exhaustion));
			context.add_float_to_payload(value);
		} else {
			err.accumulated_errors +=
					"war_exhaustion effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void prestige(association_type t, float value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::prestige));
			context.add_float_to_payload(value);
		} else {
			err.accumulated_errors +=
					"prestige effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void change_tag(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_fixed_token_ci(value.data(), value.data() + value.length(), "culture")) {
				context.compiled_effect.push_back(uint16_t(effect::change_tag_culture | effect::no_payload));
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::change_tag));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"change_tag effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors +=
						"change_tag effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}

		} else {
			err.accumulated_errors +=
					"change_tag effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void change_tag_no_core_switch(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_fixed_token_ci(value.data(), value.data() + value.length(), "culture")) {
				context.compiled_effect.push_back(uint16_t(effect::change_tag_no_core_switch_culture | effect::no_payload));
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::change_tag_no_core_switch));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors += "change_tag_no_core_switch effect given an invalid tag (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors +=
						"change_tag_no_core_switch effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}

		} else {
			err.accumulated_errors += "change_tag_no_core_switch effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void set_country_flag(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::set_country_flag));
			context.compiled_effect.push_back(trigger::payload(context.outer_context.get_national_flag(std::string(value))).value);
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_effect.push_back(uint16_t(effect::set_country_flag_province));
			context.compiled_effect.push_back(trigger::payload(context.outer_context.get_national_flag(std::string(value))).value);
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_effect.push_back(uint16_t(effect::set_country_flag_pop));
			context.compiled_effect.push_back(trigger::payload(context.outer_context.get_national_flag(std::string(value))).value);
		} else {
			err.accumulated_errors +=
					"set_country_flag effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void clr_country_flag(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::clr_country_flag));
			context.compiled_effect.push_back(trigger::payload(context.outer_context.get_national_flag(std::string(value))).value);
		} else {
			err.accumulated_errors +=
					"clr_country_flag effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void country_event(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context);
	void province_event(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context);
	void military_access(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::military_access_this_nation | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::military_access_this_province | effect::no_payload));
				else {
					err.accumulated_errors += "military_access = this effect used in an incorrect scope type (" + err.file_name +
																		", line " + std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::military_access_from_nation | effect::no_payload));
				else if(context.from_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::military_access_from_province | effect::no_payload));
				else {
					err.accumulated_errors += "military_access = from effect used in an incorrect scope type (" + err.file_name +
																		", line " + std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::military_access));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"military_access effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors +=
						"military_access effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}

		} else {
			err.accumulated_errors +=
					"military_access effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void badboy(association_type t, float value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::badboy));
			context.add_float_to_payload(value);
		} else {
			err.accumulated_errors +=
					"badboy effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void secede_province(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::secede_province_this_nation | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::secede_province_this_province | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_effect.push_back(uint16_t(effect::secede_province_this_state | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_effect.push_back(uint16_t(effect::secede_province_this_pop | effect::no_payload));
				else {
					err.accumulated_errors += "secede_province = this effect used in an incorrect scope type (" + err.file_name +
																		", line " + std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::secede_province_from_nation | effect::no_payload));
				else if(context.from_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::secede_province_from_province | effect::no_payload));
				else {
					err.accumulated_errors += "secede_province = from effect used in an incorrect scope type (" + err.file_name +
																		", line " + std::to_string(line) + ")\n";
					return;
				}
			} else if(is_reb(value)) {
				if(context.from_slot == trigger::slot_contents::rebel)
					context.compiled_effect.push_back(uint16_t(effect::secede_province_reb | effect::no_payload));
				else {
					err.accumulated_errors += "secede_province = reb effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::secede_province));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"secede_province effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors +=
						"secede_province effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(context.main_slot == trigger::slot_contents::state) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::secede_province_state_this_nation | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::secede_province_state_this_province | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_effect.push_back(uint16_t(effect::secede_province_state_this_state | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_effect.push_back(uint16_t(effect::secede_province_state_this_pop | effect::no_payload));
				else {
					err.accumulated_errors += "secede_province = this effect used in an incorrect scope type (" + err.file_name +
						", line " + std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::secede_province_state_from_nation | effect::no_payload));
				else if(context.from_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::secede_province_state_from_province | effect::no_payload));
				else {
					err.accumulated_errors += "secede_province = from effect used in an incorrect scope type (" + err.file_name +
						", line " + std::to_string(line) + ")\n";
					return;
				}
			} else if(is_reb(value)) {
				if(context.from_slot == trigger::slot_contents::rebel)
					context.compiled_effect.push_back(uint16_t(effect::secede_province_state_reb | effect::no_payload));
				else {
					err.accumulated_errors += "secede_province = reb effect used in an incorrect scope type (" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::secede_province_state));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"secede_province effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors +=
						"secede_province effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors +=
					"secede_province effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void inherit(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::inherit_this_nation | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::inherit_this_province | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_effect.push_back(uint16_t(effect::inherit_this_state | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_effect.push_back(uint16_t(effect::inherit_this_pop | effect::no_payload));
				else {
					err.accumulated_errors += "inherit = this effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::inherit_from_nation | effect::no_payload));
				else if(context.from_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::inherit_from_province | effect::no_payload));
				else {
					err.accumulated_errors += "inherit = from effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::inherit));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"inherit effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors +=
						"inherit effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}

		} else {
			err.accumulated_errors +=
					"inherit effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void annex_to(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::annex_to_this_nation | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::annex_to_this_province | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_effect.push_back(uint16_t(effect::annex_to_this_state | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_effect.push_back(uint16_t(effect::annex_to_this_pop | effect::no_payload));
				else {
					err.accumulated_errors += "annex_to = this effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::annex_to_from_nation | effect::no_payload));
				else if(context.from_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::annex_to_from_province | effect::no_payload));
				else {
					err.accumulated_errors += "annex_to = from effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::annex_to));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"annex_to effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors +=
						"annex_to effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}

		} else {
			err.accumulated_errors +=
					"annex_to effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void release(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::release_this_nation | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::release_this_province | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_effect.push_back(uint16_t(effect::release_this_state | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_effect.push_back(uint16_t(effect::release_this_pop | effect::no_payload));
				else {
					err.accumulated_errors += "release = this effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::release_from_nation | effect::no_payload));
				else if(context.from_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::release_from_province | effect::no_payload));
				else {
					err.accumulated_errors += "release = from effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::release));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"release effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors +=
						"release effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}

		} else {
			err.accumulated_errors +=
					"release effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void change_controller(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::change_controller_this_nation | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::change_controller_this_province | effect::no_payload));
				else {
					err.accumulated_errors += "change_controller = this effect used in an incorrect scope type (" + err.file_name +
						", line " + std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::change_controller_from_nation | effect::no_payload));
				else if(context.from_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::change_controller_from_province | effect::no_payload));
				else {
					err.accumulated_errors += "change_controller = from effect used in an incorrect scope type (" + err.file_name +
						", line " + std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::change_controller));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
						"change_controller effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors +=
					"change_controller effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(context.main_slot == trigger::slot_contents::state) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::change_controller_state_this_nation | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::change_controller_state_this_province | effect::no_payload));
				else {
					err.accumulated_errors += "change_controller = this effect used in an incorrect scope type (" + err.file_name +
						", line " + std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::change_controller_state_from_nation | effect::no_payload));
				else if(context.from_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::change_controller_state_from_province | effect::no_payload));
				else {
					err.accumulated_errors += "change_controller = from effect used in an incorrect scope type (" + err.file_name +
						", line " + std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::change_controller_state));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
						"change_controller effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors +=
					"change_controller effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors +=
					"change_controller effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void infrastructure(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_effect.push_back(uint16_t(effect::infrastructure));
			context.compiled_effect.push_back(trigger::payload(int16_t(value)).value);
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_effect.push_back(uint16_t(effect::infrastructure_state));
			context.compiled_effect.push_back(trigger::payload(int16_t(value)).value);
		} else {
			err.accumulated_errors +=
					"infrastructure effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void fort(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_effect.push_back(uint16_t(effect::fort));
			context.compiled_effect.push_back(trigger::payload(int16_t(value)).value);
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_effect.push_back(uint16_t(effect::fort_state));
			context.compiled_effect.push_back(trigger::payload(int16_t(value)).value);
		} else {
			err.accumulated_errors +=
					"fort effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void naval_base(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_effect.push_back(uint16_t(effect::naval_base));
			context.compiled_effect.push_back(trigger::payload(int16_t(value)).value);
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_effect.push_back(uint16_t(effect::naval_base_state));
			context.compiled_effect.push_back(trigger::payload(int16_t(value)).value);
		} else {
			err.accumulated_errors +=
					"naval_base effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void bank(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_effect.push_back(uint16_t(effect::bank));
			context.compiled_effect.push_back(trigger::payload(int16_t(value)).value);
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_effect.push_back(uint16_t(effect::bank_state));
			context.compiled_effect.push_back(trigger::payload(int16_t(value)).value);
		} else {
			err.accumulated_errors +=
				"bank effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void university(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_effect.push_back(uint16_t(effect::university));
			context.compiled_effect.push_back(trigger::payload(int16_t(value)).value);
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_effect.push_back(uint16_t(effect::university_state));
			context.compiled_effect.push_back(trigger::payload(int16_t(value)).value);
		} else {
			err.accumulated_errors +=
				"university effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void province_selector(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			if(value == 1) {
				context.compiled_effect.push_back(uint16_t(effect::add_province_modifier_no_duration));
				context.compiled_effect.push_back(trigger::payload(economy::get_province_selector_modifier(context.outer_context.state)).value);
			} else if(value == -1) {
				context.compiled_effect.push_back(uint16_t(effect::remove_province_modifier));
				context.compiled_effect.push_back(trigger::payload(economy::get_province_selector_modifier(context.outer_context.state)).value);
			} else {
				err.accumulated_errors +=
					"province_selector effect with invalid value " + std::to_string(value) + " (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			}
		} else {
			err.accumulated_errors +=
				"province_selector effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void province_immigrator(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			if(value == 1) {
				context.compiled_effect.push_back(uint16_t(effect::add_province_modifier_no_duration));
				context.compiled_effect.push_back(trigger::payload(economy::get_province_immigrator_modifier(context.outer_context.state)).value);
			} else if(value == -1) {
				context.compiled_effect.push_back(uint16_t(effect::remove_province_modifier));
				context.compiled_effect.push_back(trigger::payload(economy::get_province_immigrator_modifier(context.outer_context.state)).value);
			} else {
				err.accumulated_errors +=
					"province_immigrator effect with invalid value " + std::to_string(value) + " (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			}
		} else {
			err.accumulated_errors +=
				"province_immigrator effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void money(association_type t, float value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::treasury));
			context.add_float_to_payload(value);
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_effect.push_back(uint16_t(effect::treasury_province));
			context.add_float_to_payload(value);
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_effect.push_back(uint16_t(effect::money));
			context.add_float_to_payload(value);
		} else {
			err.accumulated_errors +=
					"money effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void leadership(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::leadership));
			context.compiled_effect.push_back(trigger::payload(int16_t(value)).value);
		} else {
			err.accumulated_errors +=
					"leadership effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void create_vassal(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::create_vassal_this_nation | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::create_vassal_this_province | effect::no_payload));
				else {
					err.accumulated_errors += "create_vassal = this effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::create_vassal_from_nation | effect::no_payload));
				else if(context.from_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::create_vassal_from_province | effect::no_payload));
				else {
					err.accumulated_errors += "create_vassal = from effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::create_vassal));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"create_vassal effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors +=
						"create_vassal effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}

		} else {
			err.accumulated_errors +=
					"create_vassal effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void end_military_access(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::end_military_access_this_nation | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::end_military_access_this_province | effect::no_payload));
				else {
					err.accumulated_errors += "end_military_access = this effect used in an incorrect scope type (" + err.file_name +
																		", line " + std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::end_military_access_from_nation | effect::no_payload));
				else if(context.from_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::end_military_access_from_province | effect::no_payload));
				else {
					err.accumulated_errors += "end_military_access = from effect used in an incorrect scope type (" + err.file_name +
																		", line " + std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::end_military_access));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"end_military_access effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors +=
						"end_military_access effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}

		} else {
			err.accumulated_errors += "end_military_access effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void leave_alliance(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::leave_alliance_this_nation | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::leave_alliance_this_province | effect::no_payload));
				else {
					err.accumulated_errors += "leave_alliance = this effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::leave_alliance_from_nation | effect::no_payload));
				else if(context.from_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::leave_alliance_from_province | effect::no_payload));
				else {
					err.accumulated_errors += "leave_alliance = from effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::leave_alliance));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"leave_alliance effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors +=
						"leave_alliance effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}

		} else {
			err.accumulated_errors += "end_military_access effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void end_war(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::end_war_this_nation | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::end_war_this_province | effect::no_payload));
				else {
					err.accumulated_errors += "end_war = this effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::end_war_from_nation | effect::no_payload));
				else if(context.from_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::end_war_from_province | effect::no_payload));
				else {
					err.accumulated_errors += "end_war = from effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::end_war));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"end_war effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors +=
						"end_war effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}

		} else {
			err.accumulated_errors +=
					"end_war effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void enable_ideology(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(auto it = context.outer_context.map_of_ideologies.find(std::string(value));
				it != context.outer_context.map_of_ideologies.end()) {
			context.compiled_effect.push_back(uint16_t(effect::enable_ideology));
			context.compiled_effect.push_back(trigger::payload(it->second.id).value);
		} else {
			err.accumulated_errors += "enable_ideology effect supplied with invalid ideology name " + std::string(value) + " (" +
																err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void ruling_party_ideology(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(auto it = context.outer_context.map_of_ideologies.find(std::string(value));
					it != context.outer_context.map_of_ideologies.end()) {
				context.compiled_effect.push_back(uint16_t(effect::ruling_party_ideology));
				context.compiled_effect.push_back(trigger::payload(it->second.id).value);
			} else {
				err.accumulated_errors += "ruling_party_ideology effect supplied with invalid ideology name " + std::string(value) +
																	" (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "ruling_party_ideology effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void plurality(association_type t, float value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::plurality));
			context.add_float_to_payload(value);
		} else {
			err.accumulated_errors +=
					"plurality effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void remove_province_modifier(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			if(auto it = context.outer_context.map_of_modifiers.find(std::string(value));
					it != context.outer_context.map_of_modifiers.end()) {
				context.compiled_effect.push_back(uint16_t(effect::remove_province_modifier));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors += "remove_province_modifier effect supplied with invalid modifier name " + std::string(value) +
					" (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(context.main_slot == trigger::slot_contents::state) {
			if(auto it = context.outer_context.map_of_modifiers.find(std::string(value));
					it != context.outer_context.map_of_modifiers.end()) {
				context.compiled_effect.push_back(uint16_t(effect::remove_province_modifier_state));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors += "remove_province_modifier effect supplied with invalid modifier name " + std::string(value) +
					" (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "remove_province_modifier effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void remove_country_modifier(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(auto it = context.outer_context.map_of_modifiers.find(std::string(value));
					it != context.outer_context.map_of_modifiers.end()) {
				context.compiled_effect.push_back(uint16_t(effect::remove_country_modifier));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors += "remove_country_modifier effect supplied with invalid modifier name " + std::string(value) +
																	" (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "remove_country_modifier effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void create_alliance(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::create_alliance_this_nation | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::create_alliance_this_province | effect::no_payload));
				else {
					err.accumulated_errors += "create_alliance = this effect used in an incorrect scope type (" + err.file_name +
																		", line " + std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::create_alliance_from_nation | effect::no_payload));
				else if(context.from_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::create_alliance_from_province | effect::no_payload));
				else {
					err.accumulated_errors += "create_alliance = from effect used in an incorrect scope type (" + err.file_name +
																		", line " + std::to_string(line) + ")\n";
					return;
				}
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::create_alliance));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors +=
							"create_alliance effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors +=
						"create_alliance effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}

		} else {
			err.accumulated_errors +=
					"create_alliance effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void release_vassal(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::release_vassal_this_nation | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::release_vassal_this_province | effect::no_payload));
				else {
					err.accumulated_errors += "release_vassal = this effect used in an incorrect scope type (" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::release_vassal_from_nation | effect::no_payload));
				else if(context.from_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::release_vassal_from_province | effect::no_payload));
				else {
					err.accumulated_errors += "release_vassal = from effect used in an incorrect scope type (" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
			} else if(is_reb(value)) {
				if(context.from_slot == trigger::slot_contents::rebel)
					context.compiled_effect.push_back(uint16_t(effect::release_vassal_reb | effect::no_payload));
				else {
					err.accumulated_errors += "release_vassal = reb effect used in an incorrect scope type (" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
			} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "random")) {
				context.compiled_effect.push_back(uint16_t(effect::release_vassal_random | effect::no_payload));
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::release_vassal));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors += "release_vassal effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors += "release_vassal effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(context.main_slot == trigger::slot_contents::province) {
			if(is_this(value)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::release_vassal_province_this_nation | effect::no_payload));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::release_vassal_province_this_province | effect::no_payload));
				else {
					err.accumulated_errors += "release_vassal = this effect used in an incorrect scope type (" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
			} else if(is_from(value)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::release_vassal_province_from_nation | effect::no_payload));
				else if(context.from_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::release_vassal_province_from_province | effect::no_payload));
				else {
					err.accumulated_errors += "release_vassal = from effect used in an incorrect scope type (" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
			} else if(is_reb(value)) {
				if(context.from_slot == trigger::slot_contents::rebel)
					context.compiled_effect.push_back(uint16_t(effect::release_vassal_province_reb | effect::no_payload));
				else {
					err.accumulated_errors += "release_vassal = reb effect used in an incorrect scope type (" + err.file_name + ", line " +
						std::to_string(line) + ")\n";
					return;
				}
			} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "random")) {
				context.compiled_effect.push_back(uint16_t(effect::release_vassal_province_random | effect::no_payload));
			} else if(value.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value[0], value[1], value[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::release_vassal_province));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
				} else {
					err.accumulated_errors += "release_vassal effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors += "release_vassal effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "release_vassal effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void change_province_name(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_effect.push_back(uint16_t(effect::change_region_name_province));
		} else {
			err.accumulated_errors += "change_province_name effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		auto name = text::find_or_add_key(context.outer_context.state, value);
		context.add_int32_t_to_payload(name.index());
	}
	void enable_canal(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context);
	void set_global_flag(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		context.compiled_effect.push_back(uint16_t(effect::set_global_flag));
		context.compiled_effect.push_back(trigger::payload(context.outer_context.get_global_flag(std::string(value))).value);
	}
	void clr_global_flag(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		context.compiled_effect.push_back(uint16_t(effect::clr_global_flag));
		context.compiled_effect.push_back(trigger::payload(context.outer_context.get_global_flag(std::string(value))).value);
	}
	void nationalvalue(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(auto it = context.outer_context.map_of_modifiers.find(std::string(value));
				it != context.outer_context.map_of_modifiers.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_effect.push_back(uint16_t(effect::nationalvalue_nation));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_effect.push_back(uint16_t(effect::nationalvalue_province));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors +=
						"nationalvalue effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "nationalvalue effect supplied with invalid modifier name " + std::string(value) + " (" +
																err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void civilized(association_type t, bool value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(value)
				context.compiled_effect.push_back(uint16_t(effect::civilized_yes | effect::no_payload));
			else
				context.compiled_effect.push_back(uint16_t(effect::civilized_no | effect::no_payload));
		} else {
			err.accumulated_errors +=
					"civilized effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void election(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::election | effect::no_payload));
		} else {
			err.accumulated_errors +=
					"election effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void social_reform(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(auto it = context.outer_context.map_of_ioptions.find(std::string(value));
				it != context.outer_context.map_of_ioptions.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_effect.push_back(uint16_t(effect::social_reform));
				context.compiled_effect.push_back(trigger::payload(it->second.id).value);
			} else if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_effect.push_back(uint16_t(effect::social_reform_province));
				context.compiled_effect.push_back(trigger::payload(it->second.id).value);
			} else {
				err.accumulated_errors +=
						"social_reform effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "social_reform effect supplied with invalid issue option name " + std::string(value) + " (" +
																err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void political_reform(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(auto it = context.outer_context.map_of_ioptions.find(std::string(value));
				it != context.outer_context.map_of_ioptions.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_effect.push_back(uint16_t(effect::political_reform));
				context.compiled_effect.push_back(trigger::payload(it->second.id).value);
			} else if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_effect.push_back(uint16_t(effect::political_reform_province));
				context.compiled_effect.push_back(trigger::payload(it->second.id).value);
			} else {
				err.accumulated_errors += "political_reform effect used in an incorrect scope type (" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "political_reform effect supplied with invalid issue option name " + std::string(value) + " (" +
																err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void add_tax_relative_income(association_type t, float value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::add_tax_relative_income));
			context.add_float_to_payload(value);
		} else {
			err.accumulated_errors += "add_tax_relative_income effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void neutrality(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::neutrality | effect::no_payload));
		} else {
			err.accumulated_errors +=
					"neutrality effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void reduce_pop(association_type t, float value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_effect.push_back(uint16_t(effect::reduce_pop));
			context.add_float_to_payload(value);
		} else if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::reduce_pop_nation));
			context.add_float_to_payload(value);
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_effect.push_back(uint16_t(effect::reduce_pop_province));
			context.add_float_to_payload(value);
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_effect.push_back(uint16_t(effect::reduce_pop_state));
			context.add_float_to_payload(value);
		} else {
			err.accumulated_errors +=
					"reduce_pop effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void move_pop(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::pop) {
			if(0 <= value && size_t(value) < context.outer_context.original_id_to_prov_id_map.size()) {
				context.compiled_effect.push_back(uint16_t(effect::move_pop));
				context.compiled_effect.push_back(trigger::payload(context.outer_context.original_id_to_prov_id_map[value]).value);
			} else {
				err.accumulated_errors +=
						"move_pop effect given an invalid province id (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors +=
					"move_pop effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void pop_type(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_poptypes.find(std::string(value));
				it != context.outer_context.map_of_poptypes.end()) {
			if(context.main_slot == trigger::slot_contents::pop) {
				context.compiled_effect.push_back(uint16_t(effect::pop_type));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors +=
						"pop_type effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "pop_type effect supplied with invalid pop type " + std::string(value) + " (" + err.file_name +
																", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void years_of_research(association_type t, float value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::years_of_research));
			context.add_float_to_payload(value);
		} else {
			err.accumulated_errors +=
					"years_of_research effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void prestige_factor(association_type t, float value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(value >= 0.0f)
				context.compiled_effect.push_back(uint16_t(effect::prestige_factor_positive));
			else
				context.compiled_effect.push_back(uint16_t(effect::prestige_factor_negative));
			context.add_float_to_payload(value);
		} else {
			err.accumulated_errors +=
					"prestige_factor effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void military_reform(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(auto it = context.outer_context.map_of_roptions.find(std::string(value));
				it != context.outer_context.map_of_roptions.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_effect.push_back(uint16_t(effect::military_reform));
				context.compiled_effect.push_back(trigger::payload(it->second.id).value);
			} else {
				err.accumulated_errors +=
						"military_reform effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "military_reform effect supplied with invalid issue option name " + std::string(value) + " (" +
																err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void economic_reform(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(auto it = context.outer_context.map_of_roptions.find(std::string(value));
				it != context.outer_context.map_of_roptions.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_effect.push_back(uint16_t(effect::economic_reform));
				context.compiled_effect.push_back(trigger::payload(it->second.id).value);
			} else {
				err.accumulated_errors +=
						"economic_reform effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "economic_reform effect supplied with invalid issue option name " + std::string(value) + " (" +
																err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void remove_random_military_reforms(association_type t, int32_t value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::remove_random_military_reforms));
			context.compiled_effect.push_back(trigger::payload(int16_t(value)).value);
		} else {
			err.accumulated_errors += "remove_random_military_reforms effect used in an incorrect scope type (" + err.file_name +
																", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void remove_random_economic_reforms(association_type t, int32_t value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::remove_random_economic_reforms));
			context.compiled_effect.push_back(trigger::payload(int16_t(value)).value);
		} else {
			err.accumulated_errors += "remove_random_economic_reforms effect used in an incorrect scope type (" + err.file_name +
																", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void add_crime(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		if(auto it = context.outer_context.map_of_crimes.find(std::string(value)); it != context.outer_context.map_of_crimes.end()) {
			if(context.main_slot == trigger::slot_contents::province) {
				context.compiled_effect.push_back(uint16_t(effect::add_crime));
				context.compiled_effect.push_back(trigger::payload(it->second.id).value);
			} else {
				err.accumulated_errors +=
						"add_crime effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "none")) {
			context.compiled_effect.push_back(uint16_t(effect::add_crime_none) | effect::no_payload);
		} else {
			err.accumulated_errors += "add_crime effect supplied with invalid modifier name " + std::string(value) + " (" +
																err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void nationalize(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::nationalize | effect::no_payload));
		} else {
			err.accumulated_errors +=
					"nationalize effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void build_factory_in_capital_state(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(auto it = context.outer_context.map_of_factory_names.find(std::string(value));
				it != context.outer_context.map_of_factory_names.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_effect.push_back(uint16_t(effect::build_factory_in_capital_state));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors += "build_factory_in_capital_state effect used in an incorrect scope type (" + err.file_name +
																	", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "build_factory_in_capital_state effect supplied with invalid factory name " + std::string(value) +
																" (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void activate_technology(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(auto it = context.outer_context.map_of_technologies.find(std::string(value));
				it != context.outer_context.map_of_technologies.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_effect.push_back(uint16_t(effect::activate_technology));
				context.compiled_effect.push_back(trigger::payload(it->second.id).value);
			} else {
				err.accumulated_errors += "activate_technology effect used in an incorrect scope type (" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(auto itb = context.outer_context.map_of_inventions.find(std::string(value));
							itb != context.outer_context.map_of_inventions.end()) {
			if(context.main_slot == trigger::slot_contents::nation) {
				context.compiled_effect.push_back(uint16_t(effect::activate_invention));
				context.compiled_effect.push_back(trigger::payload(itb->second.id).value);
			} else {
				err.accumulated_errors += "activate_technology effect used in an incorrect scope type (" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "activate_technology effect supplied with invalid technology/invention name " +
																std::string(value) + " (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void great_wars_enabled(association_type t, bool value, error_handler& err, int32_t line, effect_building_context& context) {
		if(value)
			context.compiled_effect.push_back(uint16_t(effect::great_wars_enabled_yes | effect::no_payload));
		else
			context.compiled_effect.push_back(uint16_t(effect::great_wars_enabled_no | effect::no_payload));
	}
	void world_wars_enabled(association_type t, bool value, error_handler& err, int32_t line, effect_building_context& context) {
		if(value)
			context.compiled_effect.push_back(uint16_t(effect::world_wars_enabled_yes | effect::no_payload));
		else
			context.compiled_effect.push_back(uint16_t(effect::world_wars_enabled_no | effect::no_payload));
	}
	void assimilate(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_effect.push_back(uint16_t(effect::assimilate_province | effect::no_payload));
		} else if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_effect.push_back(uint16_t(effect::assimilate_pop | effect::no_payload));
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_effect.push_back(uint16_t(effect::assimilate_state | effect::no_payload));
		} else {
			err.accumulated_errors +=
					"assimilate effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void literacy(association_type t, float value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_effect.push_back(uint16_t(effect::literacy));
			context.add_float_to_payload(value);
		} else {
			err.accumulated_errors +=
					"literacy effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void add_crisis_interest(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::add_crisis_interest | effect::no_payload));
		} else {
			err.accumulated_errors += "add_crisis_interest effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void flashpoint_tension(association_type t, float value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_effect.push_back(uint16_t(effect::flashpoint_tension));
			context.add_float_to_payload(value);
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_effect.push_back(uint16_t(effect::flashpoint_tension_province));
			context.add_float_to_payload(value);
		} else {
			err.accumulated_errors += "flashpoint_tension effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void add_crisis_temperature(association_type t, float value, error_handler& err, int32_t line,
			effect_building_context& context) {
		context.compiled_effect.push_back(uint16_t(effect::add_crisis_temperature));
		context.add_float_to_payload(value);
	}
	void consciousness(association_type t, float value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_effect.push_back(uint16_t(effect::consciousness));
			context.add_float_to_payload(value);
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_effect.push_back(uint16_t(effect::consciousness_province));
			context.add_float_to_payload(value);
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_effect.push_back(uint16_t(effect::consciousness_state));
			context.add_float_to_payload(value);
		} else if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::consciousness_nation));
			context.add_float_to_payload(value);
		} else {
			err.accumulated_errors +=
					"consciousness effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void militancy(association_type t, float value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_effect.push_back(uint16_t(effect::militancy));
			context.add_float_to_payload(value);
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_effect.push_back(uint16_t(effect::militancy_province));
			context.add_float_to_payload(value);
		} else if(context.main_slot == trigger::slot_contents::state) {
			context.compiled_effect.push_back(uint16_t(effect::militancy_state));
			context.add_float_to_payload(value);
		} else if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(uint16_t(effect::militancy_nation));
			context.add_float_to_payload(value);
		} else {
			err.accumulated_errors +=
					"militancy effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void rgo_size(association_type t, int32_t value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_effect.push_back(uint16_t(effect::rgo_size));
			context.compiled_effect.push_back(trigger::payload(int16_t(value)).value);
		} else {
			err.accumulated_errors +=
					"rgo_size effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void add_province_modifier(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			if(auto it = context.outer_context.map_of_modifiers.find(std::string(value));
					it != context.outer_context.map_of_modifiers.end()) {
				context.compiled_effect.push_back(uint16_t(effect::add_province_modifier_no_duration));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors += "add_province_modifier effect supplied with invalid modifier name " + std::string(value) +
					" (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(context.main_slot == trigger::slot_contents::state) {
			if(auto it = context.outer_context.map_of_modifiers.find(std::string(value));
					it != context.outer_context.map_of_modifiers.end()) {
				context.compiled_effect.push_back(uint16_t(effect::add_province_modifier_state_no_duration));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors += "add_province_modifier effect supplied with invalid modifier name " + std::string(value) +
					" (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "add_province_modifier effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void add_country_modifier(association_type t, std::string_view value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(auto it = context.outer_context.map_of_modifiers.find(std::string(value));
					it != context.outer_context.map_of_modifiers.end()) {
				context.compiled_effect.push_back(uint16_t(effect::add_country_modifier_no_duration));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors += "add_country_modifier effect supplied with invalid modifier name " + std::string(value) + " (" +
																	err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors += "add_country_modifier effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void trigger_revolt(ef_trigger_revolt const& value, error_handler& err, int32_t line, effect_building_context& context) {
		if(!value.type_) {
			err.accumulated_errors +=
					"trigger_revolt must have a valid rebel type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		if(context.main_slot == trigger::slot_contents::nation)
			context.compiled_effect.push_back(effect::trigger_revolt_nation);
		else if(context.main_slot == trigger::slot_contents::state)
			context.compiled_effect.push_back(effect::trigger_revolt_state);
		else if(context.main_slot == trigger::slot_contents::province)
			context.compiled_effect.push_back(effect::trigger_revolt_province);
		else {
			err.accumulated_errors +=
					"trigger_revolt effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		context.compiled_effect.push_back(trigger::payload(value.type_).value);
		context.compiled_effect.push_back(trigger::payload(value.culture_).value);
		context.compiled_effect.push_back(trigger::payload(value.religion_).value);
		context.compiled_effect.push_back(trigger::payload(value.ideology_).value);
	}
	void diplomatic_influence(ef_diplomatic_influence const& value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot != trigger::slot_contents::nation && context.main_slot != trigger::slot_contents::province) {
			err.accumulated_errors += "diplomatic_influence effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		if(is_from(value.who)) {
			if(context.from_slot == trigger::slot_contents::nation) {
				if(context.main_slot == trigger::slot_contents::province) {
					context.compiled_effect.push_back(uint16_t(effect::owner_scope_province | effect::scope_has_limit));
					context.compiled_effect.push_back(uint16_t(2 + 2));
					auto payload_size_offset = context.compiled_effect.size() - 1;
					context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);
				}
				context.compiled_effect.push_back(effect::diplomatic_influence_from_nation);
			} else if(context.from_slot == trigger::slot_contents::province) {
				if(context.main_slot == trigger::slot_contents::province) {
					context.compiled_effect.push_back(uint16_t(effect::owner_scope_province | effect::scope_has_limit));
					context.compiled_effect.push_back(uint16_t(2 + 2));
					auto payload_size_offset = context.compiled_effect.size() - 1;
					context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);
				}
				context.compiled_effect.push_back(effect::diplomatic_influence_from_province);
			} else {
				err.accumulated_errors += "diplomatic_influence = from effect used in an incorrect scope type (" + err.file_name +
																	", line " + std::to_string(line) + ")\n";
				return;
			}
			context.compiled_effect.push_back(trigger::payload(int16_t(value.value)).value);
		} else if(is_this(value.who)) {
			if(context.this_slot == trigger::slot_contents::nation) {
				if(context.main_slot == trigger::slot_contents::province) {
					context.compiled_effect.push_back(uint16_t(effect::owner_scope_province | effect::scope_has_limit));
					context.compiled_effect.push_back(uint16_t(2 + 2));
					auto payload_size_offset = context.compiled_effect.size() - 1;
					context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);
				}
				context.compiled_effect.push_back(effect::diplomatic_influence_this_nation);
			} else if(context.this_slot == trigger::slot_contents::province) {
				if(context.main_slot == trigger::slot_contents::province) {
					context.compiled_effect.push_back(uint16_t(effect::owner_scope_province | effect::scope_has_limit));
					context.compiled_effect.push_back(uint16_t(2 + 2));
					auto payload_size_offset = context.compiled_effect.size() - 1;
					context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);
				}
				context.compiled_effect.push_back(effect::diplomatic_influence_this_province);
			}  else {
				err.accumulated_errors += "diplomatic_influence = this effect used in an incorrect scope type (" + err.file_name +
																	", line " + std::to_string(line) + ")\n";
				return;
			}
			context.compiled_effect.push_back(trigger::payload(int16_t(value.value)).value);
		} else if(value.who.length() == 3) {
			
			if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value.who[0], value.who[1], value.who[2]));
					it != context.outer_context.map_of_ident_names.end()) {

				if(context.main_slot == trigger::slot_contents::province) {
					context.compiled_effect.push_back(uint16_t(effect::owner_scope_province | effect::scope_has_limit));
					context.compiled_effect.push_back(uint16_t(2 + 3));
					auto payload_size_offset = context.compiled_effect.size() - 1;
					context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);
				}

				context.compiled_effect.push_back(uint16_t(effect::diplomatic_influence));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
				context.compiled_effect.push_back(trigger::payload(int16_t(value.value)).value);
			} else {
				err.accumulated_errors +=
						"diplomatic_influence effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors +=
					"diplomatic_influence effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void relation(ef_relation const& value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(is_from(value.who)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(effect::relation_from_nation);
				else if(context.from_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(effect::relation_from_province);
				else {
					err.accumulated_errors += "relation = from effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
				context.compiled_effect.push_back(trigger::payload(int16_t(value.value)).value);
			} else if(is_this(value.who)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(effect::relation_this_nation);
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(effect::relation_this_province);
				else {
					err.accumulated_errors += "relation = this effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
				context.compiled_effect.push_back(trigger::payload(int16_t(value.value)).value);
			} else if(is_reb(value.who)) {
				if(context.from_slot == trigger::slot_contents::rebel)
					context.compiled_effect.push_back(effect::relation_reb);
				else {
					err.accumulated_errors += "relation = reb effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
				context.compiled_effect.push_back(trigger::payload(int16_t(value.value)).value);
			} else if(value.who.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value.who[0], value.who[1], value.who[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::relation));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
					context.compiled_effect.push_back(trigger::payload(int16_t(value.value)).value);
				} else {
					err.accumulated_errors +=
							"relation effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors +=
						"relation effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(context.main_slot == trigger::slot_contents::province) {
			if(is_from(value.who)) {
				if(context.from_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(effect::relation_province_from_nation);
				else if(context.from_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(effect::relation_province_from_province);
				else {
					err.accumulated_errors += "relation = from effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
				context.compiled_effect.push_back(trigger::payload(int16_t(value.value)).value);
			} else if(is_this(value.who)) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(effect::relation_province_this_nation);
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(effect::relation_province_this_province);
				else {
					err.accumulated_errors += "relation = this effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
				context.compiled_effect.push_back(trigger::payload(int16_t(value.value)).value);
			} else if(is_reb(value.who)) {
				if(context.from_slot == trigger::slot_contents::rebel)
					context.compiled_effect.push_back(effect::relation_province_reb);
				else {
					err.accumulated_errors += "relation = reb effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
				context.compiled_effect.push_back(trigger::payload(int16_t(value.value)).value);
			} else if(value.who.length() == 3) {
				if(auto it = context.outer_context.map_of_ident_names.find(nations::tag_to_int(value.who[0], value.who[1], value.who[2]));
						it != context.outer_context.map_of_ident_names.end()) {
					context.compiled_effect.push_back(uint16_t(effect::relation_province));
					context.compiled_effect.push_back(trigger::payload(it->second).value);
					context.compiled_effect.push_back(trigger::payload(int16_t(value.value)).value);
				} else {
					err.accumulated_errors +=
							"relation effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
			} else {
				err.accumulated_errors +=
						"relation effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors +=
					"relation effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void add_province_modifier(ef_add_province_modifier const& value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			if(value.duration <= 0) {
				context.compiled_effect.push_back(effect::add_province_modifier_no_duration);
				context.compiled_effect.push_back(trigger::payload(value.name_).value);
			} else {
				context.compiled_effect.push_back(effect::add_province_modifier);
				context.compiled_effect.push_back(trigger::payload(value.name_).value);
				context.compiled_effect.push_back(trigger::payload(int16_t(value.duration)).value);
			}
		} else if(context.main_slot == trigger::slot_contents::state) {
			if(value.duration <= 0) {
				context.compiled_effect.push_back(effect::add_province_modifier_state_no_duration);
				context.compiled_effect.push_back(trigger::payload(value.name_).value);
			} else {
				context.compiled_effect.push_back(effect::add_province_modifier_state);
				context.compiled_effect.push_back(trigger::payload(value.name_).value);
				context.compiled_effect.push_back(trigger::payload(int16_t(value.duration)).value);
			}
		} else {
			err.accumulated_errors += "add_province_modifier effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void add_country_modifier(ef_add_country_modifier const& value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			if(value.duration <= 0) {
				context.compiled_effect.push_back(effect::add_country_modifier_province_no_duration);
				context.compiled_effect.push_back(trigger::payload(value.name_).value);
			} else {
				context.compiled_effect.push_back(effect::add_country_modifier_province);
				context.compiled_effect.push_back(trigger::payload(value.name_).value);
				context.compiled_effect.push_back(trigger::payload(int16_t(value.duration)).value);
			}
		} else if(context.main_slot == trigger::slot_contents::nation) {
			if(value.duration <= 0) {
				context.compiled_effect.push_back(effect::add_country_modifier_no_duration);
				context.compiled_effect.push_back(trigger::payload(value.name_).value);
			} else {
				context.compiled_effect.push_back(effect::add_country_modifier);
				context.compiled_effect.push_back(trigger::payload(value.name_).value);
				context.compiled_effect.push_back(trigger::payload(int16_t(value.duration)).value);
			}
		} else {
			err.accumulated_errors += "add_country_modifier effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void casus_belli(ef_casus_belli const& value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot != trigger::slot_contents::nation) {
			err.accumulated_errors +=
					"casus_belli effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		if(is_from(value.target)) {
			if(context.from_slot == trigger::slot_contents::nation)
				context.compiled_effect.push_back(effect::casus_belli_from_nation);
			else if(context.from_slot == trigger::slot_contents::province)
				context.compiled_effect.push_back(effect::casus_belli_from_province);
			else {
				err.accumulated_errors += "casus_belli = from effect used in an incorrect scope type (" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_effect.push_back(trigger::payload(value.type_).value);
			context.compiled_effect.push_back(uint16_t(value.months));
		} else if(is_this(value.target)) {
			if(context.this_slot == trigger::slot_contents::nation)
				context.compiled_effect.push_back(effect::casus_belli_this_nation);
			else if(context.this_slot == trigger::slot_contents::province)
				context.compiled_effect.push_back(effect::casus_belli_this_province);
			else if(context.this_slot == trigger::slot_contents::state)
				context.compiled_effect.push_back(effect::casus_belli_this_state);
			else if(context.this_slot == trigger::slot_contents::pop)
				context.compiled_effect.push_back(effect::casus_belli_this_pop);
			else {
				err.accumulated_errors += "casus_belli = this effect used in an incorrect scope type (" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_effect.push_back(trigger::payload(value.type_).value);
			context.compiled_effect.push_back(uint16_t(value.months));
		} else if(is_integer(value.target.data(), value.target.data() + value.target.length())) {
			auto ivalue = parse_int(value.target, line, err);
			if(0 <= ivalue && size_t(ivalue) < context.outer_context.original_id_to_prov_id_map.size()) {
				context.compiled_effect.push_back(effect::casus_belli_int);
				context.compiled_effect.push_back(trigger::payload(value.type_).value);
				context.compiled_effect.push_back(uint16_t(value.months));
				context.compiled_effect.push_back(trigger::payload(context.outer_context.original_id_to_prov_id_map[ivalue]).value);
			} else {
				err.accumulated_errors +=
						"casus_belli effect given an invalid province id (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(value.target.length() == 3) {
			if(auto it = context.outer_context.map_of_ident_names.find(
						 nations::tag_to_int(value.target[0], value.target[1], value.target[2]));
					it != context.outer_context.map_of_ident_names.end()) {
				context.compiled_effect.push_back(uint16_t(effect::casus_belli_tag));
				context.compiled_effect.push_back(trigger::payload(value.type_).value);
				context.compiled_effect.push_back(uint16_t(value.months));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors +=
						"casus_belli effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors +=
					"casus_belli effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void add_casus_belli(ef_add_casus_belli const& value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot != trigger::slot_contents::nation) {
			err.accumulated_errors +=
					"add_casus_belli effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		if(is_from(value.target)) {
			if(context.from_slot == trigger::slot_contents::nation)
				context.compiled_effect.push_back(effect::add_casus_belli_from_nation);
			else if(context.from_slot == trigger::slot_contents::province)
				context.compiled_effect.push_back(effect::add_casus_belli_from_province);
			else {
				err.accumulated_errors += "add_casus_belli = from effect used in an incorrect scope type (" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_effect.push_back(trigger::payload(value.type_).value);
			context.compiled_effect.push_back(uint16_t(value.months));
		} else if(is_this(value.target)) {
			if(context.this_slot == trigger::slot_contents::nation)
				context.compiled_effect.push_back(effect::add_casus_belli_this_nation);
			else if(context.this_slot == trigger::slot_contents::province)
				context.compiled_effect.push_back(effect::add_casus_belli_this_province);
			else if(context.this_slot == trigger::slot_contents::state)
				context.compiled_effect.push_back(effect::add_casus_belli_this_state);
			else if(context.this_slot == trigger::slot_contents::pop)
				context.compiled_effect.push_back(effect::add_casus_belli_this_pop);
			else {
				err.accumulated_errors += "add_casus_belli = this effect used in an incorrect scope type (" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
			context.compiled_effect.push_back(trigger::payload(value.type_).value);
			context.compiled_effect.push_back(uint16_t(value.months));
		} else if(is_integer(value.target.data(), value.target.data() + value.target.length())) {
			auto ivalue = parse_int(value.target, line, err);
			if(0 <= ivalue && size_t(ivalue) < context.outer_context.original_id_to_prov_id_map.size()) {
				context.compiled_effect.push_back(effect::add_casus_belli_int);
				context.compiled_effect.push_back(trigger::payload(value.type_).value);
				context.compiled_effect.push_back(uint16_t(value.months));
				context.compiled_effect.push_back(trigger::payload(context.outer_context.original_id_to_prov_id_map[ivalue]).value);
			} else {
				err.accumulated_errors +=
						"add_casus_belli effect given an invalid province id (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(value.target.length() == 3) {
			if(auto it = context.outer_context.map_of_ident_names.find(
						 nations::tag_to_int(value.target[0], value.target[1], value.target[2]));
					it != context.outer_context.map_of_ident_names.end()) {
				context.compiled_effect.push_back(uint16_t(effect::add_casus_belli_tag));
				context.compiled_effect.push_back(trigger::payload(value.type_).value);
				context.compiled_effect.push_back(uint16_t(value.months));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors +=
						"add_casus_belli effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors +=
					"add_casus_belli effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void remove_casus_belli(ef_remove_casus_belli const& value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot != trigger::slot_contents::nation) {
			err.accumulated_errors += "remove_casus_belli effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		if(is_from(value.target)) {
			if(context.from_slot == trigger::slot_contents::nation)
				context.compiled_effect.push_back(effect::remove_casus_belli_from_nation);
			else if(context.from_slot == trigger::slot_contents::province)
				context.compiled_effect.push_back(effect::remove_casus_belli_from_province);
			else {
				err.accumulated_errors += "remove_casus_belli = from effect used in an incorrect scope type (" + err.file_name +
																	", line " + std::to_string(line) + ")\n";
				return;
			}
			context.compiled_effect.push_back(trigger::payload(value.type_).value);
		} else if(is_this(value.target)) {
			if(context.this_slot == trigger::slot_contents::nation)
				context.compiled_effect.push_back(effect::remove_casus_belli_this_nation);
			else if(context.this_slot == trigger::slot_contents::province)
				context.compiled_effect.push_back(effect::remove_casus_belli_this_province);
			else if(context.this_slot == trigger::slot_contents::state)
				context.compiled_effect.push_back(effect::remove_casus_belli_this_state);
			else if(context.this_slot == trigger::slot_contents::pop)
				context.compiled_effect.push_back(effect::remove_casus_belli_this_pop);
			else {
				err.accumulated_errors += "remove_casus_belli = this effect used in an incorrect scope type (" + err.file_name +
																	", line " + std::to_string(line) + ")\n";
				return;
			}
			context.compiled_effect.push_back(trigger::payload(value.type_).value);
		} else if(is_integer(value.target.data(), value.target.data() + value.target.length())) {
			auto ivalue = parse_int(value.target, line, err);
			if(0 <= ivalue && size_t(ivalue) < context.outer_context.original_id_to_prov_id_map.size()) {
				context.compiled_effect.push_back(effect::remove_casus_belli_int);
				context.compiled_effect.push_back(trigger::payload(value.type_).value);
				context.compiled_effect.push_back(trigger::payload(context.outer_context.original_id_to_prov_id_map[ivalue]).value);
			} else {
				err.accumulated_errors +=
						"remove_casus_belli effect given an invalid province id (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(value.target.length() == 3) {
			if(auto it = context.outer_context.map_of_ident_names.find(
						 nations::tag_to_int(value.target[0], value.target[1], value.target[2]));
					it != context.outer_context.map_of_ident_names.end()) {
				context.compiled_effect.push_back(uint16_t(effect::remove_casus_belli_tag));
				context.compiled_effect.push_back(trigger::payload(value.type_).value);
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors +=
						"remove_casus_belli effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors +=
					"remove_casus_belli effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void this_remove_casus_belli(ef_this_remove_casus_belli const& value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot != trigger::slot_contents::nation) {
			err.accumulated_errors += "remove_casus_belli effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		if(is_from(value.target)) {
			if(context.from_slot == trigger::slot_contents::nation)
				context.compiled_effect.push_back(effect::this_remove_casus_belli_from_nation);
			else if(context.from_slot == trigger::slot_contents::province)
				context.compiled_effect.push_back(effect::this_remove_casus_belli_from_province);
			else {
				err.accumulated_errors += "this_remove_casus_belli = from effect used in an incorrect scope type (" + err.file_name +
																	", line " + std::to_string(line) + ")\n";
				return;
			}
			context.compiled_effect.push_back(trigger::payload(value.type_).value);
		} else if(is_this(value.target)) {
			if(context.this_slot == trigger::slot_contents::nation)
				context.compiled_effect.push_back(effect::this_remove_casus_belli_this_nation);
			else if(context.this_slot == trigger::slot_contents::province)
				context.compiled_effect.push_back(effect::this_remove_casus_belli_this_province);
			else if(context.this_slot == trigger::slot_contents::state)
				context.compiled_effect.push_back(effect::this_remove_casus_belli_this_state);
			else if(context.this_slot == trigger::slot_contents::pop)
				context.compiled_effect.push_back(effect::this_remove_casus_belli_this_pop);
			else {
				err.accumulated_errors += "this_remove_casus_belli = this effect used in an incorrect scope type (" + err.file_name +
																	", line " + std::to_string(line) + ")\n";
				return;
			}
			context.compiled_effect.push_back(trigger::payload(value.type_).value);
		} else if(is_integer(value.target.data(), value.target.data() + value.target.length())) {
			auto ivalue = parse_int(value.target, line, err);
			if(0 <= ivalue && size_t(ivalue) < context.outer_context.original_id_to_prov_id_map.size()) {
				context.compiled_effect.push_back(effect::this_remove_casus_belli_int);
				context.compiled_effect.push_back(trigger::payload(value.type_).value);
				context.compiled_effect.push_back(trigger::payload(context.outer_context.original_id_to_prov_id_map[ivalue]).value);
			} else {
				err.accumulated_errors += "this_remove_casus_belli effect given an invalid province id (" + err.file_name + ", line " +
																	std::to_string(line) + ")\n";
				return;
			}
		} else if(value.target.length() == 3) {
			if(auto it = context.outer_context.map_of_ident_names.find(
						 nations::tag_to_int(value.target[0], value.target[1], value.target[2]));
					it != context.outer_context.map_of_ident_names.end()) {
				context.compiled_effect.push_back(uint16_t(effect::this_remove_casus_belli_tag));
				context.compiled_effect.push_back(trigger::payload(value.type_).value);
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors +=
						"this_remove_casus_belli effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else {
			err.accumulated_errors +=
					"this_remove_casus_belli effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void war(ef_war const& value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot != trigger::slot_contents::nation) {
			err.accumulated_errors +=
					"war effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		if(is_from(value.target)) {
			if(context.from_slot == trigger::slot_contents::nation)
				context.compiled_effect.push_back(value.call_ally ? effect::war_from_nation : effect::war_no_ally_from_nation);
			else if(context.from_slot == trigger::slot_contents::province)
				context.compiled_effect.push_back(value.call_ally ? effect::war_from_province : effect::war_no_ally_from_province);
			else {
				err.accumulated_errors +=
						"war = from effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(is_this(value.target)) {
			if(context.this_slot == trigger::slot_contents::nation)
				context.compiled_effect.push_back(value.call_ally ? effect::war_this_nation : effect::war_no_ally_this_nation);
			else if(context.this_slot == trigger::slot_contents::province)
				context.compiled_effect.push_back(value.call_ally ? effect::war_this_province : effect::war_no_ally_this_province);
			else if(context.this_slot == trigger::slot_contents::state)
				context.compiled_effect.push_back(value.call_ally ? effect::war_this_state : effect::war_no_ally_this_state);
			else if(context.this_slot == trigger::slot_contents::province)
				context.compiled_effect.push_back(value.call_ally ? effect::war_this_pop : effect::war_no_ally_this_pop);
			else {
				err.accumulated_errors +=
						"war = this effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(value.target.length() == 3) {
			if(auto it = context.outer_context.map_of_ident_names.find(
						 nations::tag_to_int(value.target[0], value.target[1], value.target[2]));
					it != context.outer_context.map_of_ident_names.end()) {
				context.compiled_effect.push_back(uint16_t(value.call_ally ? effect::war_tag : effect::war_no_ally_tag));
				context.compiled_effect.push_back(trigger::payload(it->second).value);
			} else {
				err.accumulated_errors +=
						"war = effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		} else if(value.target.length() == 0) { // some mods use this as a call allies effect. So far, we just discard it
			return;
		} else {
			err.accumulated_errors +=
					"war = effect given an invalid tag (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		context.compiled_effect.push_back(trigger::payload(value.defender_goal.casus_belli_).value);
		context.compiled_effect.push_back(trigger::payload(value.defender_goal.state_province_id_).value);
		context.compiled_effect.push_back(trigger::payload(value.defender_goal.country_).value);
		context.compiled_effect.push_back(trigger::payload(value.attacker_goal.casus_belli_).value);
		context.compiled_effect.push_back(trigger::payload(value.attacker_goal.state_province_id_).value);
		context.compiled_effect.push_back(trigger::payload(value.attacker_goal.country_).value);
	}
	void country_event(ef_country_event const& value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation) {
			if(value.days <= 0) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::country_event_immediate_this_nation));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::country_event_immediate_this_province));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_effect.push_back(uint16_t(effect::country_event_immediate_this_state));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_effect.push_back(uint16_t(effect::country_event_immediate_this_pop));
				else {
					err.accumulated_errors +=
							"country_event effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
				context.compiled_effect.push_back(trigger::payload(value.id_).value);
			} else {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::country_event_this_nation));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::country_event_this_province));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_effect.push_back(uint16_t(effect::country_event_this_state));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_effect.push_back(uint16_t(effect::country_event_this_pop));
				else {
					err.accumulated_errors +=
							"country_event effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
				context.compiled_effect.push_back(trigger::payload(value.id_).value);
				context.compiled_effect.push_back(uint16_t(value.days));
			}
		} else if(context.main_slot == trigger::slot_contents::province) {
			if(value.days <= 0) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::country_event_immediate_province_this_nation));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::country_event_immediate_province_this_province));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_effect.push_back(uint16_t(effect::country_event_immediate_province_this_state));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_effect.push_back(uint16_t(effect::country_event_immediate_province_this_pop));
				else {
					err.accumulated_errors +=
							"country_event effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
				context.compiled_effect.push_back(trigger::payload(value.id_).value);
			} else {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::country_event_province_this_nation));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::country_event_province_this_province));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_effect.push_back(uint16_t(effect::country_event_province_this_state));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_effect.push_back(uint16_t(effect::country_event_province_this_pop));
				else {
					err.accumulated_errors +=
							"country_event effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
					return;
				}
				context.compiled_effect.push_back(trigger::payload(value.id_).value);
				context.compiled_effect.push_back(uint16_t(value.days));
			}
		} else {
			err.accumulated_errors +=
					"country_event effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void province_event(ef_province_event const& value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::province) {
			if(value.days <= 0) {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::province_event_immediate_this_nation));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::province_event_immediate_this_province));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_effect.push_back(uint16_t(effect::province_event_immediate_this_state));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_effect.push_back(uint16_t(effect::province_event_immediate_this_pop));
				else {
					err.accumulated_errors += "province_event effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
				context.compiled_effect.push_back(trigger::payload(value.id_).value);
			} else {
				if(context.this_slot == trigger::slot_contents::nation)
					context.compiled_effect.push_back(uint16_t(effect::province_event_this_nation));
				else if(context.this_slot == trigger::slot_contents::province)
					context.compiled_effect.push_back(uint16_t(effect::province_event_this_province));
				else if(context.this_slot == trigger::slot_contents::state)
					context.compiled_effect.push_back(uint16_t(effect::province_event_this_state));
				else if(context.this_slot == trigger::slot_contents::pop)
					context.compiled_effect.push_back(uint16_t(effect::province_event_this_pop));
				else {
					err.accumulated_errors += "province_event effect used in an incorrect scope type (" + err.file_name + ", line " +
																		std::to_string(line) + ")\n";
					return;
				}
				context.compiled_effect.push_back(trigger::payload(value.id_).value);
				context.compiled_effect.push_back(uint16_t(value.days));
			}
		} else {
			err.accumulated_errors +=
					"province_event effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}

	void sub_unit(ef_sub_unit const& value, error_handler& err, int32_t line, effect_building_context& context) {
		if(is_this(value.value)) {
			if(context.main_slot != trigger::slot_contents::nation || context.this_slot != trigger::slot_contents::province) {
				err.accumulated_errors +=
						"sub_unit effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
			context.compiled_effect.push_back(effect::sub_unit_this);
			context.compiled_effect.push_back(trigger::payload(value.type_).value);
		} else if(is_from(value.value)) {
			if(context.main_slot != trigger::slot_contents::nation || context.from_slot != trigger::slot_contents::province) {
				err.accumulated_errors +=
						"sub_unit effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
			context.compiled_effect.push_back(effect::sub_unit_from);
			context.compiled_effect.push_back(trigger::payload(value.type_).value);
		} else if(is_fixed_token_ci(value.value.data(), value.value.data() + value.value.length(), "current")) {
			if(context.main_slot != trigger::slot_contents::province) {
				err.accumulated_errors +=
						"sub_unit effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
			context.compiled_effect.push_back(effect::sub_unit_current);
			context.compiled_effect.push_back(trigger::payload(value.type_).value);
		} else {
			auto ivalue = parse_int(value.value, line, err);
			if(0 <= ivalue && size_t(ivalue) < context.outer_context.original_id_to_prov_id_map.size()) {
				auto prov_id = context.outer_context.original_id_to_prov_id_map[ivalue];
				context.compiled_effect.push_back(effect::sub_unit_int);
				context.compiled_effect.push_back(trigger::payload(value.type_).value);
				context.compiled_effect.push_back(trigger::payload(prov_id).value);
			} else {
				err.accumulated_errors +=
						"sub_unit effect given an invalid province id (" + err.file_name + ", line " + std::to_string(line) + ")\n";
				return;
			}
		}
	}
	void set_variable(ef_set_variable const& value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot != trigger::slot_contents::nation) {
			err.accumulated_errors +=
					"set_variable effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		context.compiled_effect.push_back(effect::set_variable);
		context.compiled_effect.push_back(trigger::payload(value.which_).value);
		context.add_float_to_payload(value.value);
	}
	void change_variable(ef_change_variable const& value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot != trigger::slot_contents::nation) {
			err.accumulated_errors +=
					"change_variable effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		context.compiled_effect.push_back(effect::change_variable);
		context.compiled_effect.push_back(trigger::payload(value.which_).value);
		context.add_float_to_payload(value.value);
	}
	void ideology(ef_ideology const& value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot != trigger::slot_contents::pop) {
			err.accumulated_errors +=
					"ideology effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		context.compiled_effect.push_back(effect::ideology);
		context.compiled_effect.push_back(trigger::payload(value.value_).value);
		context.add_float_to_payload(value.factor);
	}
	void dominant_issue(ef_dominant_issue const& value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::pop) {
			context.compiled_effect.push_back(effect::dominant_issue);
			context.compiled_effect.push_back(trigger::payload(value.value_).value);
			context.add_float_to_payload(value.factor);
		} else if(context.main_slot == trigger::slot_contents::nation) {
			context.compiled_effect.push_back(effect::dominant_issue_nation);
			context.compiled_effect.push_back(trigger::payload(value.value_).value);
			context.add_float_to_payload(value.factor);
		} else {
			err.accumulated_errors +=
					"dominant_issue effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void upper_house(ef_upper_house const& value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot != trigger::slot_contents::nation) {
			err.accumulated_errors +=
					"upper_house effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		context.compiled_effect.push_back(effect::upper_house);
		context.compiled_effect.push_back(trigger::payload(value.ideology_).value);
		context.add_float_to_payload(value.value);
	}
	void scaled_militancy(ef_scaled_militancy const& value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::pop) {
			if(bool(value.ideology_)) {
				context.compiled_effect.push_back(effect::scaled_militancy_ideology);
				context.compiled_effect.push_back(trigger::payload(value.ideology_).value);
				context.add_float_to_payload(value.factor);
			} else if(bool(value.issue_)) {
				context.compiled_effect.push_back(effect::scaled_militancy_issue);
				context.compiled_effect.push_back(trigger::payload(value.issue_).value);
				context.add_float_to_payload(value.factor);
			} else {
				context.compiled_effect.push_back(effect::scaled_militancy_unemployment);
				context.add_float_to_payload(value.factor * value.unemployment);
			}
		} else if(context.main_slot == trigger::slot_contents::nation) {
			if(bool(value.ideology_)) {
				context.compiled_effect.push_back(effect::scaled_militancy_nation_ideology);
				context.compiled_effect.push_back(trigger::payload(value.ideology_).value);
				context.add_float_to_payload(value.factor);
			} else if(bool(value.issue_)) {
				context.compiled_effect.push_back(effect::scaled_militancy_nation_issue);
				context.compiled_effect.push_back(trigger::payload(value.issue_).value);
				context.add_float_to_payload(value.factor);
			} else {
				context.compiled_effect.push_back(effect::scaled_militancy_nation_unemployment);
				context.add_float_to_payload(value.factor * value.unemployment);
			}
		} else if(context.main_slot == trigger::slot_contents::state) {
			if(bool(value.ideology_)) {
				context.compiled_effect.push_back(effect::scaled_militancy_state_ideology);
				context.compiled_effect.push_back(trigger::payload(value.ideology_).value);
				context.add_float_to_payload(value.factor);
			} else if(bool(value.issue_)) {
				context.compiled_effect.push_back(effect::scaled_militancy_state_issue);
				context.compiled_effect.push_back(trigger::payload(value.issue_).value);
				context.add_float_to_payload(value.factor);
			} else {
				context.compiled_effect.push_back(effect::scaled_militancy_state_unemployment);
				context.add_float_to_payload(value.factor * value.unemployment);
			}
		} else if(context.main_slot == trigger::slot_contents::province) {
			if(bool(value.ideology_)) {
				context.compiled_effect.push_back(effect::scaled_militancy_province_ideology);
				context.compiled_effect.push_back(trigger::payload(value.ideology_).value);
				context.add_float_to_payload(value.factor);
			} else if(bool(value.issue_)) {
				context.compiled_effect.push_back(effect::scaled_militancy_province_issue);
				context.compiled_effect.push_back(trigger::payload(value.issue_).value);
				context.add_float_to_payload(value.factor);
			} else {
				context.compiled_effect.push_back(effect::scaled_militancy_province_unemployment);
				context.add_float_to_payload(value.factor * value.unemployment);
			}
		} else {
			err.accumulated_errors +=
					"scaled_militancy effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void scaled_consciousness(ef_scaled_consciousness const& value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::pop) {
			if(bool(value.ideology_)) {
				context.compiled_effect.push_back(effect::scaled_consciousness_ideology);
				context.compiled_effect.push_back(trigger::payload(value.ideology_).value);
				context.add_float_to_payload(value.factor);
			} else if(bool(value.issue_)) {
				context.compiled_effect.push_back(effect::scaled_consciousness_issue);
				context.compiled_effect.push_back(trigger::payload(value.issue_).value);
				context.add_float_to_payload(value.factor);
			} else {
				context.compiled_effect.push_back(effect::scaled_consciousness_unemployment);
				context.add_float_to_payload(value.factor * value.unemployment);
			}
		} else if(context.main_slot == trigger::slot_contents::nation) {
			if(bool(value.ideology_)) {
				context.compiled_effect.push_back(effect::scaled_consciousness_nation_ideology);
				context.compiled_effect.push_back(trigger::payload(value.ideology_).value);
				context.add_float_to_payload(value.factor);
			} else if(bool(value.issue_)) {
				context.compiled_effect.push_back(effect::scaled_consciousness_nation_issue);
				context.compiled_effect.push_back(trigger::payload(value.issue_).value);
				context.add_float_to_payload(value.factor);
			} else {
				context.compiled_effect.push_back(effect::scaled_consciousness_nation_unemployment);
				context.add_float_to_payload(value.factor * value.unemployment);
			}
		} else if(context.main_slot == trigger::slot_contents::state) {
			if(bool(value.ideology_)) {
				context.compiled_effect.push_back(effect::scaled_consciousness_state_ideology);
				context.compiled_effect.push_back(trigger::payload(value.ideology_).value);
				context.add_float_to_payload(value.factor);
			} else if(bool(value.issue_)) {
				context.compiled_effect.push_back(effect::scaled_consciousness_state_issue);
				context.compiled_effect.push_back(trigger::payload(value.issue_).value);
				context.add_float_to_payload(value.factor);
			} else {
				context.compiled_effect.push_back(effect::scaled_consciousness_state_unemployment);
				context.add_float_to_payload(value.factor * value.unemployment);
			}
		} else if(context.main_slot == trigger::slot_contents::province) {
			if(bool(value.ideology_)) {
				context.compiled_effect.push_back(effect::scaled_consciousness_province_ideology);
				context.compiled_effect.push_back(trigger::payload(value.ideology_).value);
				context.add_float_to_payload(value.factor);
			} else if(bool(value.issue_)) {
				context.compiled_effect.push_back(effect::scaled_consciousness_province_issue);
				context.compiled_effect.push_back(trigger::payload(value.issue_).value);
				context.add_float_to_payload(value.factor);
			} else {
				context.compiled_effect.push_back(effect::scaled_consciousness_province_unemployment);
				context.add_float_to_payload(value.factor * value.unemployment);
			}
		} else {
			err.accumulated_errors += "scaled_consciousness effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
	}
	void define_general(ef_define_general const& value, error_handler& err, int32_t line, effect_building_context& context);
	void define_admiral(ef_define_admiral const& value, error_handler& err, int32_t line, effect_building_context& context);
	void kill_leader(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context);
	void add_war_goal(ef_add_war_goal const& value, error_handler& err, int32_t line, effect_building_context& context) {
		if(context.main_slot != trigger::slot_contents::nation || context.from_slot != trigger::slot_contents::nation) {
			err.accumulated_errors +=
					"add_war_goal effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
		context.compiled_effect.push_back(effect::add_war_goal);
		context.compiled_effect.push_back(trigger::payload(value.casus_belli_).value);
	}
	void move_issue_percentage(ef_move_issue_percentage const& value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot == trigger::slot_contents::nation)
			context.compiled_effect.push_back(effect::move_issue_percentage_nation);
		else if(context.main_slot == trigger::slot_contents::state)
			context.compiled_effect.push_back(effect::move_issue_percentage_state);
		else if(context.main_slot == trigger::slot_contents::province)
			context.compiled_effect.push_back(effect::move_issue_percentage_province);
		else if(context.main_slot == trigger::slot_contents::pop)
			context.compiled_effect.push_back(effect::move_issue_percentage_pop);
		else {
			err.accumulated_errors += "move_issue_percentage effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		context.compiled_effect.push_back(trigger::payload(value.from_).value);
		context.compiled_effect.push_back(trigger::payload(value.to_).value);
		context.add_float_to_payload(value.value);
	}
	void party_loyalty(ef_party_loyalty const& value, error_handler& err, int32_t line, effect_building_context& context) {
		if(bool(value.province_id_)) {
			context.compiled_effect.push_back(effect::party_loyalty);
			context.compiled_effect.push_back(trigger::payload(value.province_id_).value);
			context.compiled_effect.push_back(trigger::payload(value.ideology_).value);
			context.compiled_effect.push_back(trigger::payload(int16_t(value.loyalty_value)).value);
		} else if(context.main_slot == trigger::slot_contents::province) {
			context.compiled_effect.push_back(effect::party_loyalty_province);
			context.compiled_effect.push_back(trigger::payload(value.ideology_).value);
			context.compiled_effect.push_back(trigger::payload(int16_t(value.loyalty_value)).value);
		} else {
			err.accumulated_errors +=
					"party_loyalty effect used in an incorrect scope type (" + err.file_name + ", line " + std::to_string(line) + ")\n";
			return;
		}
	}
	void build_railway_in_capital(ef_build_railway_in_capital const& value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot != trigger::slot_contents::nation) {
			err.accumulated_errors += "build_railway_in_capital effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		if(value.in_whole_capital_state && value.limit_to_world_greatest_level)
			context.compiled_effect.push_back(
					uint16_t(effect::build_railway_in_capital_yes_whole_state_yes_limit | effect::no_payload));
		else if(value.in_whole_capital_state && !value.limit_to_world_greatest_level)
			context.compiled_effect.push_back(uint16_t(effect::build_railway_in_capital_yes_whole_state_no_limit | effect::no_payload));
		else if(!value.in_whole_capital_state && value.limit_to_world_greatest_level)
			context.compiled_effect.push_back(uint16_t(effect::build_railway_in_capital_no_whole_state_yes_limit | effect::no_payload));
		else
			context.compiled_effect.push_back(uint16_t(effect::build_railway_in_capital_no_whole_state_no_limit | effect::no_payload));
	}
	void build_bank_in_capital(ef_build_bank_in_capital const& value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot != trigger::slot_contents::nation) {
			err.accumulated_errors += "build_bank_in_capital effect used in an incorrect scope type (" + err.file_name + ", line " +
				std::to_string(line) + ")\n";
			return;
		}
		if(value.in_whole_capital_state && value.limit_to_world_greatest_level)
			context.compiled_effect.push_back(
					uint16_t(effect::build_bank_in_capital_yes_whole_state_yes_limit | effect::no_payload));
		else if(value.in_whole_capital_state && !value.limit_to_world_greatest_level)
			context.compiled_effect.push_back(uint16_t(effect::build_bank_in_capital_yes_whole_state_no_limit | effect::no_payload));
		else if(!value.in_whole_capital_state && value.limit_to_world_greatest_level)
			context.compiled_effect.push_back(uint16_t(effect::build_bank_in_capital_no_whole_state_yes_limit | effect::no_payload));
		else
			context.compiled_effect.push_back(uint16_t(effect::build_bank_in_capital_no_whole_state_no_limit | effect::no_payload));
	}
	void build_university_in_capital(ef_build_university_in_capital const& value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot != trigger::slot_contents::nation) {
			err.accumulated_errors += "build_university_in_capital effect used in an incorrect scope type (" + err.file_name + ", line " +
				std::to_string(line) + ")\n";
			return;
		}
		if(value.in_whole_capital_state && value.limit_to_world_greatest_level)
			context.compiled_effect.push_back(
					uint16_t(effect::build_university_in_capital_yes_whole_state_yes_limit | effect::no_payload));
		else if(value.in_whole_capital_state && !value.limit_to_world_greatest_level)
			context.compiled_effect.push_back(uint16_t(effect::build_university_in_capital_yes_whole_state_no_limit | effect::no_payload));
		else if(!value.in_whole_capital_state && value.limit_to_world_greatest_level)
			context.compiled_effect.push_back(uint16_t(effect::build_university_in_capital_no_whole_state_yes_limit | effect::no_payload));
		else
			context.compiled_effect.push_back(uint16_t(effect::build_university_in_capital_no_whole_state_no_limit | effect::no_payload));
	}
	void build_fort_in_capital(ef_build_fort_in_capital const& value, error_handler& err, int32_t line,
			effect_building_context& context) {
		if(context.main_slot != trigger::slot_contents::nation) {
			err.accumulated_errors += "build_fort_in_capital effect used in an incorrect scope type (" + err.file_name + ", line " +
																std::to_string(line) + ")\n";
			return;
		}
		if(value.in_whole_capital_state && value.limit_to_world_greatest_level)
			context.compiled_effect.push_back(uint16_t(effect::build_fort_in_capital_yes_whole_state_yes_limit | effect::no_payload));
		else if(value.in_whole_capital_state && !value.limit_to_world_greatest_level)
			context.compiled_effect.push_back(uint16_t(effect::build_fort_in_capital_yes_whole_state_no_limit | effect::no_payload));
		else if(!value.in_whole_capital_state && value.limit_to_world_greatest_level)
			context.compiled_effect.push_back(uint16_t(effect::build_fort_in_capital_no_whole_state_yes_limit | effect::no_payload));
		else
			context.compiled_effect.push_back(uint16_t(effect::build_fort_in_capital_no_whole_state_no_limit | effect::no_payload));
	}
	void any_value(std::string_view label, association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context);
	void limit(dcon::trigger_key lim, error_handler& err, int32_t line, effect_building_context& context) {
		context.compiled_effect[context.limit_position] = trigger::payload(lim).value;
	}
};

struct event_option : public effect_body {
	dcon::value_modifier_key ai_chance;
	dcon::text_sequence_id name_;

	void name(association_type t, std::string_view value, error_handler& err, int32_t line, effect_building_context& context) {
		name_ = text::find_or_add_key(context.outer_context.state, value);
	}
};

dcon::value_modifier_key make_option_ai_chance(token_generator& gen, error_handler& err, effect_building_context& context);

dcon::trigger_key ef_limit(token_generator& gen, error_handler& err, effect_building_context& context);

void ef_scope_hidden_tooltip(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_any_neighbor_province(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_any_neighbor_country(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_any_country(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_random_country(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_random_neighbor_province(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_random_empty_neighbor_province(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_any_greater_power(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_poor_strata(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_middle_strata(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_rich_strata(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_random_pop(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_random_owned(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_any_owned(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_all_core(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_any_state(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_random_state(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_any_pop(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_owner(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_controller(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_location(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_country(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_capital_scope(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_this(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_from(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_sea_zone(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_cultural_union(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_overlord(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_sphere_owner(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_independence(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_flashpoint_tag_scope(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_crisis_state_scope(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_state_scope(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_random(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_random_list(token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_variable(std::string_view label, token_generator& gen, error_handler& err, effect_building_context& context);
void ef_scope_any_substate(token_generator& gen, error_handler& err, effect_building_context& context);
int32_t add_to_random_list(std::string_view label, token_generator& gen, error_handler& err, effect_building_context& context);

dcon::effect_key make_effect(token_generator& gen, error_handler& err, effect_building_context& context);
int32_t simplify_effect(uint16_t* source);

} // namespace parsers
