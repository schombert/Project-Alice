#pragma once
#include "dcon_generated.hpp"
#include "common_types.hpp"

namespace command {

enum class command_type : uint8_t {
	invalid = 0,
	change_nat_focus = 1,
	start_research = 2,
	make_leader = 3,
	begin_province_building_construction = 4,
	increase_relations = 5,
	decrease_relations = 6,
	begin_factory_building_construction = 7,
	begin_unit_construction = 8,
	cancel_unit_construction = 9,
};

struct national_focus_data {
	dcon::state_instance_id target_state;
	dcon::national_focus_id focus;
};

struct start_research_data {
	dcon::technology_id tech;
};

struct make_leader_data {
	bool is_general;
};

struct province_building_data {
	dcon::province_id location;
	economy::province_building_type type;
};

struct factory_building_data {
	dcon::state_instance_id location;
	dcon::factory_type_id type;
	bool is_upgrade;
};

struct diplo_action_data {
	dcon::nation_id target;
};

struct unit_construction_data {
	dcon::province_id location;
	dcon::unit_type_id type;
};

struct payload {
	union dtype {
		national_focus_data nat_focus;
		start_research_data start_research;
		make_leader_data make_leader;
		province_building_data start_province_building;
		diplo_action_data diplo_action;
		factory_building_data start_factory_building;
		unit_construction_data unit_construction;

		dtype() {}
	} data;
	dcon::nation_id source;
	command_type type = command_type::invalid;

	payload() {}
};

void set_national_focus(sys::state& state, dcon::nation_id source, dcon::state_instance_id target_state, dcon::national_focus_id focus);
bool can_set_national_focus(sys::state& state, dcon::nation_id source, dcon::state_instance_id target_state, dcon::national_focus_id focus);

void start_research(sys::state& state, dcon::nation_id source, dcon::technology_id tech);
bool can_start_research(sys::state& state, dcon::nation_id source, dcon::technology_id tech);

void make_leader(sys::state& state, dcon::nation_id source, bool general);
bool can_make_leader(sys::state& state, dcon::nation_id source, bool general);

void decrease_relations(sys::state& state, dcon::nation_id source, dcon::nation_id target);
bool can_decrease_relations(sys::state& state, dcon::nation_id source, dcon::nation_id target);

void begin_province_building_construction(sys::state& state, dcon::nation_id source, dcon::province_id p, economy::province_building_type type);
bool can_begin_province_building_construction(sys::state& state, dcon::nation_id source, dcon::province_id p, economy::province_building_type type);

void begin_factory_building_construction(sys::state& state, dcon::nation_id source, dcon::state_instance_id location, dcon::factory_type_id type, bool is_upgrade);
bool can_begin_factory_building_construction(sys::state& state, dcon::nation_id source, dcon::state_instance_id location, dcon::factory_type_id type, bool is_upgrade);

void start_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type);
bool can_start_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type);

void cancel_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type);
bool can_cancel_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type);

void execute_pending_commands(sys::state& state);

}
