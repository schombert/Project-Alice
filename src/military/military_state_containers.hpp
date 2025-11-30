#pragma once

#include "dcon_generated_ids.hpp"
#include "container_types_dcon.hpp"
#include "container_types.hpp"

namespace military {

struct unit_definition : public sys::unit_variable_stats {
	economy::commodity_set build_cost;
	economy::commodity_set supply_cost;

	int32_t colonial_points = 0;
	int32_t min_port_level = 0;
	int32_t supply_consumption_score = 0;

	int32_t icon = 0;
	int32_t naval_icon = 0;

	dcon::text_key name;

	bool is_land = true;
	bool capital = false;
	bool can_build_overseas = true;
	bool primary_culture = false;
	bool active = true;

	unit_type type = unit_type::infantry;
	uint16_t padding = 0;

	unit_definition() {
	}
};
static_assert(sizeof(unit_definition) ==
	sizeof(sys::unit_variable_stats)
	+ sizeof(unit_definition::build_cost)
	+ sizeof(unit_definition::supply_cost)
	+ sizeof(unit_definition::colonial_points)
	+ sizeof(unit_definition::min_port_level)
	+ sizeof(unit_definition::supply_consumption_score)
	+ sizeof(unit_definition::icon)
	+ sizeof(unit_definition::naval_icon)
	+ sizeof(unit_definition::name)
	+ sizeof(unit_definition::is_land)
	+ sizeof(unit_definition::capital)
	+ sizeof(unit_definition::can_build_overseas)
	+ sizeof(unit_definition::primary_culture)
	+ sizeof(unit_definition::active)
	+ sizeof(unit_definition::type)
	+ sizeof(unit_definition::padding));

struct global_military_state {
	tagged_vector<unit_definition, dcon::unit_type_id> unit_base_definitions;

	dcon::leader_trait_id first_background_trait;

	bool great_wars_enabled = false;
	bool world_wars_enabled = false;

	dcon::unit_type_id base_army_unit;
	dcon::unit_type_id base_naval_unit;

	dcon::cb_type_id standard_civil_war;
	dcon::cb_type_id standard_great_war;

	dcon::cb_type_id standard_status_quo;

	dcon::cb_type_id liberate;
	dcon::cb_type_id uninstall_communist_gov;

	// CB type used to resolve crisis over colonizing the same state. Both parties have this WG.
	dcon::cb_type_id crisis_colony;
	/*
	CB type used to liberate a tag from the target in the liberation crisis.
	In vanilla - free_peoples.
	po_transfer_provinces = yes
	*/
	dcon::cb_type_id crisis_liberate;
	/* This type of a wargoal will be used for annex nation crises (restore order cb for example) */
	dcon::cb_type_id crisis_annex;

	dcon::unit_type_id irregular;
	//dcon::unit_type_id infantry;
	dcon::unit_type_id artillery;

	bool pending_blackflag_update = false;
};

}
