#include "dcon_generated.hpp"
#include "system_state.hpp"
#include "serialization.hpp"
#include <random>

#define ZSTD_STATIC_LINKING_ONLY
#define XXH_NAMESPACE ZSTD_

#include "zstd.h"

namespace sys {

uint8_t const* read_scenario_header(uint8_t const* ptr_in, scenario_header& header_out) {
	uint32_t length = 0;
	memcpy(&length, ptr_in, sizeof(uint32_t));
	memcpy(&header_out, ptr_in + sizeof(uint32_t), std::min(length, uint32_t(sizeof(scenario_header))));
	return ptr_in + sizeof(uint32_t) + length;
}

uint8_t const* read_save_header(uint8_t const* ptr_in, save_header& header_out) {
	uint32_t length = 0;
	memcpy(&length, ptr_in, sizeof(uint32_t));
	memcpy(&header_out, ptr_in + sizeof(uint32_t), std::min(length, uint32_t(sizeof(save_header))));
	return ptr_in + sizeof(uint32_t) + length;
}

uint8_t* write_scenario_header(uint8_t* ptr_in, scenario_header const& header_in) {
	uint32_t length = uint32_t(sizeof(scenario_header));
	memcpy(ptr_in, &length, sizeof(uint32_t));
	memcpy(ptr_in + sizeof(uint32_t), &header_in, sizeof(scenario_header));
	return ptr_in + sizeof_scenario_header(header_in);
}
uint8_t* write_save_header(uint8_t* ptr_in, save_header const& header_in) {
	uint32_t length = uint32_t(sizeof(save_header));
	memcpy(ptr_in, &length, sizeof(uint32_t));
	memcpy(ptr_in + sizeof(uint32_t), &header_in, sizeof(save_header));
	return ptr_in + sizeof_save_header(header_in);
}

size_t sizeof_scenario_header(scenario_header const& header_in) {
	return sizeof(uint32_t) + sizeof(scenario_header);
}

size_t sizeof_save_header(save_header const& header_in) {
	return sizeof(uint32_t) + sizeof(save_header);
}

uint8_t* write_compressed_section(uint8_t* ptr_out, uint8_t const* ptr_in, uint32_t uncompressed_size) {
	uint32_t decompressed_length = uncompressed_size;

	uint32_t section_length = uint32_t(
		ZSTD_compress(ptr_out + sizeof(uint32_t) * 2, ZSTD_compressBound(uncompressed_size),
			ptr_in, uncompressed_size, 0)); // write compressed data

	memcpy(ptr_out, &section_length, sizeof(uint32_t));
	memcpy(ptr_out + sizeof(uint32_t), &decompressed_length, sizeof(uint32_t));

	return ptr_out + sizeof(uint32_t) * 2 + section_length;
}

template<typename T>
uint8_t const* with_decompressed_section(uint8_t const* ptr_in, T const& function) {
	uint32_t section_length = 0;
	uint32_t decompressed_length = 0;
	memcpy(&section_length, ptr_in, sizeof(uint32_t));
	memcpy(&decompressed_length, ptr_in + sizeof(uint32_t), sizeof(uint32_t));

	uint8_t* temp_buffer = new uint8_t[decompressed_length];
	//TODO: allocate memory for decompression and decompress into it

	ZSTD_decompress(temp_buffer, decompressed_length, ptr_in + sizeof(uint32_t) * 2, section_length);

	//function(ptr_in + sizeof(uint32_t) * 2, decompressed_length);
	function(temp_buffer, decompressed_length);

	delete[] temp_buffer;
	return ptr_in + sizeof(uint32_t) * 2 + section_length;
}

uint8_t const* read_scenario_section(uint8_t const* ptr_in, uint8_t const* section_end, sys::state& state) {
	// hand-written contribution
	{ // map
		ptr_in = memcpy_deserialize(ptr_in, state.map_display.size_x);
		ptr_in = memcpy_deserialize(ptr_in, state.map_display.size_y);
		ptr_in = deserialize(ptr_in, state.map_display.border_vertices);
		ptr_in = deserialize(ptr_in, state.map_display.borders);
		ptr_in = deserialize(ptr_in, state.map_display.terrain_id_map);
		ptr_in = deserialize(ptr_in, state.map_display.province_id_map);
	}
	{
		uint32_t length = 0;
		memcpy(&length, ptr_in, sizeof(uint32_t));
		ptr_in += sizeof(uint32_t);

		simple_fs::restore_state(state.common_fs, native_string_view(reinterpret_cast<native_char const*>(ptr_in), length));
		ptr_in += length * sizeof(native_char);
	}
	{
		memcpy(&(state.defines), ptr_in, sizeof(parsing::defines));
		ptr_in += sizeof(parsing::defines);
	}
	{
		memcpy(&(state.economy_definitions), ptr_in, sizeof(economy::global_economy_state));
		ptr_in += sizeof(economy::global_economy_state);
	}
	{ // culture definitions
		ptr_in = deserialize(ptr_in, state.culture_definitions.party_issues);
		ptr_in = deserialize(ptr_in, state.culture_definitions.political_issues);
		ptr_in = deserialize(ptr_in, state.culture_definitions.social_issues);
		ptr_in = deserialize(ptr_in, state.culture_definitions.military_issues);
		ptr_in = deserialize(ptr_in, state.culture_definitions.economic_issues);
		ptr_in = deserialize(ptr_in, state.culture_definitions.tech_folders);
		ptr_in = deserialize(ptr_in, state.culture_definitions.governments);
		ptr_in = deserialize(ptr_in, state.culture_definitions.crimes);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.artisans);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.capitalists);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.farmers);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.laborers);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.clergy);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.soldiers);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.officers);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.slaves);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.bureaucrat);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.aristocrat);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.primary_factory_worker);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.secondary_factory_worker);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.officer_leadership_points);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.bureaucrat_tax_efficiency);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.conservative);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.jingoism);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.promotion_chance);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.demotion_chance);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.migration_chance);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.colonialmigration_chance);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.emigration_chance);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.assimilation_chance);
		ptr_in = memcpy_deserialize(ptr_in, state.culture_definitions.conversion_chance);
	}
	{ // military definitions
		ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.first_background_trait);
		ptr_in = deserialize(ptr_in, state.military_definitions.unit_base_definitions);
		ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.base_army_unit);
		ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.base_naval_unit);
		ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.standard_civil_war);
		ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.standard_great_war);
	}
	{ // national definitions
		ptr_in = deserialize(ptr_in, state.national_definitions.flag_variable_names);
		ptr_in = deserialize(ptr_in, state.national_definitions.global_flag_variable_names);
		ptr_in = deserialize(ptr_in, state.national_definitions.variable_names);
		ptr_in = deserialize(ptr_in, state.national_definitions.triggered_modifiers);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.very_easy_player);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.easy_player);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.hard_player);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.very_hard_player);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.very_easy_ai);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.easy_ai);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.hard_ai);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.very_hard_ai);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.overseas);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.coastal);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.non_coastal);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.coastal_sea);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.sea_zone);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.land_province);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.blockaded);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.no_adjacent_controlled);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.core);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.has_siege);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.occupied);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.nationalism);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.infrastructure);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.base_values);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.war);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.peace);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.disarming);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.war_exhaustion);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.badboy);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.debt_default_to);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.bad_debter);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.great_power);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.second_power);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.civ_nation);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.unciv_nation);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.average_literacy);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.plurality);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.generalised_debt_default);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.total_occupation);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.total_blockaded);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.in_bankrupcy);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.num_allocated_national_variables);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.num_allocated_national_flags);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.num_allocated_global_flags);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.flashpoint_focus);
		ptr_in = memcpy_deserialize(ptr_in, state.national_definitions.flashpoint_amount);
		ptr_in = deserialize(ptr_in, state.national_definitions.on_yearly_pulse);
		ptr_in = deserialize(ptr_in, state.national_definitions.on_quarterly_pulse);
		ptr_in = deserialize(ptr_in, state.national_definitions.on_battle_won);
		ptr_in = deserialize(ptr_in, state.national_definitions.on_battle_lost);
		ptr_in = deserialize(ptr_in, state.national_definitions.on_surrender);
		ptr_in = deserialize(ptr_in, state.national_definitions.on_new_great_nation);
		ptr_in = deserialize(ptr_in, state.national_definitions.on_lost_great_nation);
		ptr_in = deserialize(ptr_in, state.national_definitions.on_election_tick);
		ptr_in = deserialize(ptr_in, state.national_definitions.on_colony_to_state);
		ptr_in = deserialize(ptr_in, state.national_definitions.on_state_conquest);
		ptr_in = deserialize(ptr_in, state.national_definitions.on_colony_to_state_free_slaves);
		ptr_in = deserialize(ptr_in, state.national_definitions.on_debtor_default);
		ptr_in = deserialize(ptr_in, state.national_definitions.on_debtor_default_small);
		ptr_in = deserialize(ptr_in, state.national_definitions.on_debtor_default_second);
		ptr_in = deserialize(ptr_in, state.national_definitions.on_civilize);
		ptr_in = deserialize(ptr_in, state.national_definitions.on_my_factories_nationalized);
		ptr_in = deserialize(ptr_in, state.national_definitions.on_crisis_declare_interest);
	}
	{ // provincial definitions
		ptr_in = deserialize(ptr_in, state.province_definitions.canals);
		ptr_in = deserialize(ptr_in, state.province_definitions.terrain_to_gfx_map);
		ptr_in = memcpy_deserialize(ptr_in, state.province_definitions.first_sea_province);
		ptr_in = memcpy_deserialize(ptr_in, state.province_definitions.europe);
		ptr_in = memcpy_deserialize(ptr_in, state.province_definitions.asia);
		ptr_in = memcpy_deserialize(ptr_in, state.province_definitions.africa);
		ptr_in = memcpy_deserialize(ptr_in, state.province_definitions.north_america);
		ptr_in = memcpy_deserialize(ptr_in, state.province_definitions.south_america);
		ptr_in = memcpy_deserialize(ptr_in, state.province_definitions.oceania);
	}
	ptr_in = memcpy_deserialize(ptr_in, state.start_date);
	ptr_in = memcpy_deserialize(ptr_in, state.end_date);
	ptr_in = deserialize(ptr_in, state.trigger_data);
	ptr_in = deserialize(ptr_in, state.effect_data);
	ptr_in = deserialize(ptr_in, state.value_modifier_segments);
	ptr_in = deserialize(ptr_in, state.value_modifiers);
	ptr_in = deserialize(ptr_in, state.text_data);
	ptr_in = deserialize(ptr_in, state.text_components);
	ptr_in = deserialize(ptr_in, state.text_sequences);
	ptr_in = deserialize(ptr_in, state.key_to_text_sequence);
	{ // ui definitions
		ptr_in = deserialize(ptr_in, state.ui_defs.gfx);
		ptr_in = deserialize(ptr_in, state.ui_defs.textures);
		ptr_in = deserialize(ptr_in, state.ui_defs.gui);
		ptr_in = deserialize(ptr_in, state.font_collection.font_names);
	}

	// data container

	dcon::load_record loaded;
	std::byte const* start = reinterpret_cast<std::byte const*>(ptr_in);
	state.world.deserialize(start, reinterpret_cast<std::byte const*>(section_end), loaded);

	return section_end;
}
uint8_t* write_scenario_section(uint8_t* ptr_in, sys::state& state) {
	// hand-written contribution
	{ // map
		ptr_in = memcpy_serialize(ptr_in, state.map_display.size_x);
		ptr_in = memcpy_serialize(ptr_in, state.map_display.size_y);
		ptr_in = serialize(ptr_in, state.map_display.border_vertices);
		ptr_in = serialize(ptr_in, state.map_display.borders);
		ptr_in = serialize(ptr_in, state.map_display.terrain_id_map);
		ptr_in = serialize(ptr_in, state.map_display.province_id_map);
	}
	{
		auto fs_str = simple_fs::extract_state(state.common_fs);
		uint32_t length = uint32_t(fs_str.length());
		memcpy(ptr_in, &length, sizeof(uint32_t));
		ptr_in += sizeof(uint32_t);
		memcpy(ptr_in, fs_str.c_str(), length * sizeof(native_char));
		ptr_in += length * sizeof(native_char);
	}
	{
		memcpy(ptr_in, &(state.defines), sizeof(parsing::defines));
		ptr_in += sizeof(parsing::defines);
	}
	{
		memcpy(ptr_in, &(state.economy_definitions), sizeof(economy::global_economy_state));
		ptr_in += sizeof(economy::global_economy_state);
	}
	{ // culture definitions
		ptr_in = serialize(ptr_in, state.culture_definitions.party_issues);
		ptr_in = serialize(ptr_in, state.culture_definitions.political_issues);
		ptr_in = serialize(ptr_in, state.culture_definitions.social_issues);
		ptr_in = serialize(ptr_in, state.culture_definitions.military_issues);
		ptr_in = serialize(ptr_in, state.culture_definitions.economic_issues);
		ptr_in = serialize(ptr_in, state.culture_definitions.tech_folders);
		ptr_in = serialize(ptr_in, state.culture_definitions.governments);
		ptr_in = serialize(ptr_in, state.culture_definitions.crimes);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.artisans);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.capitalists);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.farmers);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.laborers);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.clergy);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.soldiers);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.officers);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.slaves);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.bureaucrat);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.aristocrat);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.primary_factory_worker);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.secondary_factory_worker);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.officer_leadership_points);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.bureaucrat_tax_efficiency);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.conservative);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.jingoism);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.promotion_chance);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.demotion_chance);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.migration_chance);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.colonialmigration_chance);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.emigration_chance);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.assimilation_chance);
		ptr_in = memcpy_serialize(ptr_in, state.culture_definitions.conversion_chance);
	}
	{ // military definitions
		ptr_in = memcpy_serialize(ptr_in, state.military_definitions.first_background_trait);
		ptr_in = serialize(ptr_in, state.military_definitions.unit_base_definitions);
		ptr_in = memcpy_serialize(ptr_in, state.military_definitions.base_army_unit);
		ptr_in = memcpy_serialize(ptr_in, state.military_definitions.base_naval_unit);
		ptr_in = memcpy_serialize(ptr_in, state.military_definitions.standard_civil_war);
		ptr_in = memcpy_serialize(ptr_in, state.military_definitions.standard_great_war);
	}
	{ // national definitions
		ptr_in = serialize(ptr_in, state.national_definitions.flag_variable_names);
		ptr_in = serialize(ptr_in, state.national_definitions.global_flag_variable_names);
		ptr_in = serialize(ptr_in, state.national_definitions.variable_names);
		ptr_in = serialize(ptr_in, state.national_definitions.triggered_modifiers);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.very_easy_player);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.easy_player);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.hard_player);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.very_hard_player);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.very_easy_ai);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.easy_ai);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.hard_ai);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.very_hard_ai);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.overseas);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.coastal);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.non_coastal);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.coastal_sea);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.sea_zone);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.land_province);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.blockaded);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.no_adjacent_controlled);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.core);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.has_siege);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.occupied);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.nationalism);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.infrastructure);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.base_values);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.war);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.peace);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.disarming);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.war_exhaustion);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.badboy);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.debt_default_to);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.bad_debter);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.great_power);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.second_power);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.civ_nation);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.unciv_nation);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.average_literacy);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.plurality);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.generalised_debt_default);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.total_occupation);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.total_blockaded);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.in_bankrupcy);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.num_allocated_national_variables);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.num_allocated_national_flags);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.num_allocated_global_flags);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.flashpoint_focus);
		ptr_in = memcpy_serialize(ptr_in, state.national_definitions.flashpoint_amount);
		ptr_in = serialize(ptr_in, state.national_definitions.on_yearly_pulse);
		ptr_in = serialize(ptr_in, state.national_definitions.on_quarterly_pulse);
		ptr_in = serialize(ptr_in, state.national_definitions.on_battle_won);
		ptr_in = serialize(ptr_in, state.national_definitions.on_battle_lost);
		ptr_in = serialize(ptr_in, state.national_definitions.on_surrender);
		ptr_in = serialize(ptr_in, state.national_definitions.on_new_great_nation);
		ptr_in = serialize(ptr_in, state.national_definitions.on_lost_great_nation);
		ptr_in = serialize(ptr_in, state.national_definitions.on_election_tick);
		ptr_in = serialize(ptr_in, state.national_definitions.on_colony_to_state);
		ptr_in = serialize(ptr_in, state.national_definitions.on_state_conquest);
		ptr_in = serialize(ptr_in, state.national_definitions.on_colony_to_state_free_slaves);
		ptr_in = serialize(ptr_in, state.national_definitions.on_debtor_default);
		ptr_in = serialize(ptr_in, state.national_definitions.on_debtor_default_small);
		ptr_in = serialize(ptr_in, state.national_definitions.on_debtor_default_second);
		ptr_in = serialize(ptr_in, state.national_definitions.on_civilize);
		ptr_in = serialize(ptr_in, state.national_definitions.on_my_factories_nationalized);
		ptr_in = serialize(ptr_in, state.national_definitions.on_crisis_declare_interest);
	}
	{ // provincial definitions
		ptr_in = serialize(ptr_in, state.province_definitions.canals);
		ptr_in = serialize(ptr_in, state.province_definitions.terrain_to_gfx_map);
		ptr_in = memcpy_serialize(ptr_in, state.province_definitions.first_sea_province);
		ptr_in = memcpy_serialize(ptr_in, state.province_definitions.europe);
		ptr_in = memcpy_serialize(ptr_in, state.province_definitions.asia);
		ptr_in = memcpy_serialize(ptr_in, state.province_definitions.africa);
		ptr_in = memcpy_serialize(ptr_in, state.province_definitions.north_america);
		ptr_in = memcpy_serialize(ptr_in, state.province_definitions.south_america);
		ptr_in = memcpy_serialize(ptr_in, state.province_definitions.oceania);
	}
	ptr_in = memcpy_serialize(ptr_in, state.start_date);
	ptr_in = memcpy_serialize(ptr_in, state.end_date);
	ptr_in = serialize(ptr_in, state.trigger_data);
	ptr_in = serialize(ptr_in, state.effect_data);
	ptr_in = serialize(ptr_in, state.value_modifier_segments);
	ptr_in = serialize(ptr_in, state.value_modifiers);
	ptr_in = serialize(ptr_in, state.text_data);
	ptr_in = serialize(ptr_in, state.text_components);
	ptr_in = serialize(ptr_in, state.text_sequences);
	ptr_in = serialize(ptr_in, state.key_to_text_sequence);
	{ // ui definitions
		ptr_in = serialize(ptr_in, state.ui_defs.gfx);
		ptr_in = serialize(ptr_in, state.ui_defs.textures);
		ptr_in = serialize(ptr_in, state.ui_defs.gui);
		ptr_in = serialize(ptr_in, state.font_collection.font_names);
	}

	dcon::load_record result = state.world.make_serialize_record_store_scenario();
	std::byte* start = reinterpret_cast<std::byte*>(ptr_in);
	state.world.serialize(start, result);

	return reinterpret_cast<uint8_t*>(start);
}
size_t sizeof_scenario_section(sys::state& state) {
	size_t sz = 0;

	// hand-written contribution
	{ // map
		sz += sizeof(state.map_display.size_x);
		sz += sizeof(state.map_display.size_y);
		sz += serialize_size(state.map_display.border_vertices);
		sz += serialize_size(state.map_display.borders);
		sz += serialize_size(state.map_display.terrain_id_map);
		sz += serialize_size(state.map_display.province_id_map);
	}
	{
		auto fs_str = simple_fs::extract_state(state.common_fs);
		uint32_t length = uint32_t(fs_str.length());
		//memcpy(ptr_in, &length, sizeof(uint32_t));
		sz += sizeof(uint32_t);
		//memcpy(ptr_in, fs_str.c_str(), length * sizeof(native_char));
		sz += length * sizeof(native_char);
	}
	{
		sz += sizeof(parsing::defines);
	}
	{
		sz += sizeof(economy::global_economy_state);
	}
	{ // culture definitions
		sz += serialize_size(state.culture_definitions.party_issues);
		sz += serialize_size(state.culture_definitions.political_issues);
		sz += serialize_size(state.culture_definitions.social_issues);
		sz += serialize_size(state.culture_definitions.military_issues);
		sz += serialize_size(state.culture_definitions.economic_issues);
		sz += serialize_size(state.culture_definitions.tech_folders);
		sz += serialize_size(state.culture_definitions.governments);
		sz += serialize_size(state.culture_definitions.crimes);
		sz += sizeof(state.culture_definitions.artisans);
		sz += sizeof(state.culture_definitions.capitalists);
		sz += sizeof(state.culture_definitions.farmers);
		sz += sizeof(state.culture_definitions.laborers);
		sz += sizeof(state.culture_definitions.clergy);
		sz += sizeof(state.culture_definitions.soldiers);
		sz += sizeof(state.culture_definitions.officers);
		sz += sizeof(state.culture_definitions.slaves);
		sz += sizeof(state.culture_definitions.bureaucrat);
		sz += sizeof(state.culture_definitions.aristocrat);
		sz += sizeof(state.culture_definitions.primary_factory_worker);
		sz += sizeof(state.culture_definitions.secondary_factory_worker);
		sz += sizeof(state.culture_definitions.officer_leadership_points);
		sz += sizeof(state.culture_definitions.bureaucrat_tax_efficiency);
		sz += sizeof(state.culture_definitions.conservative);
		sz += sizeof(state.culture_definitions.jingoism);
		sz += sizeof(state.culture_definitions.promotion_chance);
		sz += sizeof(state.culture_definitions.demotion_chance);
		sz += sizeof(state.culture_definitions.migration_chance);
		sz += sizeof(state.culture_definitions.colonialmigration_chance);
		sz += sizeof(state.culture_definitions.emigration_chance);
		sz += sizeof(state.culture_definitions.assimilation_chance);
		sz += sizeof(state.culture_definitions.conversion_chance);
	}
	{ // military definitions
		sz += sizeof(state.military_definitions.first_background_trait);
		sz += serialize_size(state.military_definitions.unit_base_definitions);
		sz += sizeof(state.military_definitions.base_army_unit);
		sz += sizeof(state.military_definitions.base_naval_unit);
		sz += sizeof(state.military_definitions.standard_civil_war);
		sz += sizeof(state.military_definitions.standard_great_war);
	}
	{ // national definitions
		sz += serialize_size(state.national_definitions.flag_variable_names);
		sz += serialize_size(state.national_definitions.global_flag_variable_names);
		sz += serialize_size(state.national_definitions.variable_names);
		sz += serialize_size(state.national_definitions.triggered_modifiers);
		sz += sizeof(state.national_definitions.very_easy_player);
		sz += sizeof(state.national_definitions.easy_player);
		sz += sizeof(state.national_definitions.hard_player);
		sz += sizeof(state.national_definitions.very_hard_player);
		sz += sizeof(state.national_definitions.very_easy_ai);
		sz += sizeof(state.national_definitions.easy_ai);
		sz += sizeof(state.national_definitions.hard_ai);
		sz += sizeof(state.national_definitions.very_hard_ai);
		sz += sizeof(state.national_definitions.overseas);
		sz += sizeof(state.national_definitions.coastal);
		sz += sizeof(state.national_definitions.non_coastal);
		sz += sizeof(state.national_definitions.coastal_sea);
		sz += sizeof(state.national_definitions.sea_zone);
		sz += sizeof(state.national_definitions.land_province);
		sz += sizeof(state.national_definitions.blockaded);
		sz += sizeof(state.national_definitions.no_adjacent_controlled);
		sz += sizeof(state.national_definitions.core);
		sz += sizeof(state.national_definitions.has_siege);
		sz += sizeof(state.national_definitions.occupied);
		sz += sizeof(state.national_definitions.nationalism);
		sz += sizeof(state.national_definitions.infrastructure);
		sz += sizeof(state.national_definitions.base_values);
		sz += sizeof(state.national_definitions.war);
		sz += sizeof(state.national_definitions.peace);
		sz += sizeof(state.national_definitions.disarming);
		sz += sizeof(state.national_definitions.war_exhaustion);
		sz += sizeof(state.national_definitions.badboy);
		sz += sizeof(state.national_definitions.debt_default_to);
		sz += sizeof(state.national_definitions.bad_debter);
		sz += sizeof(state.national_definitions.great_power);
		sz += sizeof(state.national_definitions.second_power);
		sz += sizeof(state.national_definitions.civ_nation);
		sz += sizeof(state.national_definitions.unciv_nation);
		sz += sizeof(state.national_definitions.average_literacy);
		sz += sizeof(state.national_definitions.plurality);
		sz += sizeof(state.national_definitions.generalised_debt_default);
		sz += sizeof(state.national_definitions.total_occupation);
		sz += sizeof(state.national_definitions.total_blockaded);
		sz += sizeof(state.national_definitions.in_bankrupcy);
		sz += sizeof(state.national_definitions.num_allocated_national_variables);
		sz += sizeof(state.national_definitions.num_allocated_national_flags);
		sz += sizeof(state.national_definitions.num_allocated_global_flags);
		sz += sizeof(state.national_definitions.flashpoint_focus);
		sz += sizeof(state.national_definitions.flashpoint_amount);
		sz += serialize_size(state.national_definitions.on_yearly_pulse);
		sz += serialize_size(state.national_definitions.on_quarterly_pulse);
		sz += serialize_size(state.national_definitions.on_battle_won);
		sz += serialize_size(state.national_definitions.on_battle_lost);
		sz += serialize_size(state.national_definitions.on_surrender);
		sz += serialize_size(state.national_definitions.on_new_great_nation);
		sz += serialize_size(state.national_definitions.on_lost_great_nation);
		sz += serialize_size(state.national_definitions.on_election_tick);
		sz += serialize_size(state.national_definitions.on_colony_to_state);
		sz += serialize_size(state.national_definitions.on_state_conquest);
		sz += serialize_size(state.national_definitions.on_colony_to_state_free_slaves);
		sz += serialize_size(state.national_definitions.on_debtor_default);
		sz += serialize_size(state.national_definitions.on_debtor_default_small);
		sz += serialize_size(state.national_definitions.on_debtor_default_second);
		sz += serialize_size(state.national_definitions.on_civilize);
		sz += serialize_size(state.national_definitions.on_my_factories_nationalized);
		sz += serialize_size(state.national_definitions.on_crisis_declare_interest);
	}
	{ // provincial definitions
		sz += serialize_size(state.province_definitions.canals);
		sz += serialize_size(state.province_definitions.terrain_to_gfx_map);
		sz += sizeof(state.province_definitions.first_sea_province);
		sz += sizeof(state.province_definitions.europe);
		sz += sizeof(state.province_definitions.asia);
		sz += sizeof(state.province_definitions.africa);
		sz += sizeof(state.province_definitions.north_america);
		sz += sizeof(state.province_definitions.south_america);
		sz += sizeof(state.province_definitions.oceania);
	}
	sz += sizeof(state.start_date);
	sz += sizeof(state.end_date);
	sz += serialize_size(state.trigger_data);
	sz += serialize_size(state.effect_data);
	sz += serialize_size(state.value_modifier_segments);
	sz += serialize_size(state.value_modifiers);
	sz += serialize_size(state.text_data);
	sz += serialize_size(state.text_components);
	sz += serialize_size(state.text_sequences);
	sz += serialize_size(state.key_to_text_sequence);
	{ // ui definitions
		sz += serialize_size(state.ui_defs.gfx);
		sz += serialize_size(state.ui_defs.textures);
		sz += serialize_size(state.ui_defs.gui);
		sz += serialize_size(state.font_collection.font_names);
	}

	// data container contribution
	dcon::load_record loaded = state.world.make_serialize_record_store_scenario();
	//dcon::load_record loaded;
	sz += state.world.serialize_size(loaded);

	return sz;
}


uint8_t const* read_save_section(uint8_t const* ptr_in, uint8_t const* section_end, sys::state& state) {
	// hand-written contribution
	ptr_in = deserialize(ptr_in, state.unit_names);
	ptr_in = memcpy_deserialize(ptr_in, state.local_player_nation);
	ptr_in = memcpy_deserialize(ptr_in, state.current_date);
	ptr_in = memcpy_deserialize(ptr_in, state.game_seed);
	ptr_in = memcpy_deserialize(ptr_in, state.crisis_state);
	ptr_in = deserialize(ptr_in, state.crisis_participants);
	ptr_in = memcpy_deserialize(ptr_in, state.current_crisis);
	ptr_in = memcpy_deserialize(ptr_in, state.crisis_temperature);
	ptr_in = memcpy_deserialize(ptr_in, state.primary_crisis_attacker);
	ptr_in = memcpy_deserialize(ptr_in, state.primary_crisis_defender);

	{ // national definitions
		ptr_in = deserialize(ptr_in, state.national_definitions.global_flag_variables);
	}

	{ // military definitions
		ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.great_wars_enabled);
		ptr_in = memcpy_deserialize(ptr_in, state.military_definitions.world_wars_enabled);
	}

	// data container contribution

	dcon::load_record loaded;
	std::byte const* start = reinterpret_cast<std::byte const*>(ptr_in);
	state.world.deserialize(start, reinterpret_cast<std::byte const*>(section_end), loaded);

	return section_end;
}

uint8_t* write_save_section(uint8_t* ptr_in, sys::state& state) {
	// hand-written contribution
	ptr_in = serialize(ptr_in, state.unit_names);
	ptr_in = memcpy_serialize(ptr_in, state.local_player_nation);
	ptr_in = memcpy_serialize(ptr_in, state.current_date);
	ptr_in = memcpy_serialize(ptr_in, state.game_seed);
	ptr_in = memcpy_serialize(ptr_in, state.crisis_state);
	ptr_in = serialize(ptr_in, state.crisis_participants);
	ptr_in = memcpy_serialize(ptr_in, state.current_crisis);
	ptr_in = memcpy_serialize(ptr_in, state.crisis_temperature);
	ptr_in = memcpy_serialize(ptr_in, state.primary_crisis_attacker);
	ptr_in = memcpy_serialize(ptr_in, state.primary_crisis_defender);

	{ // national definitions
		ptr_in = serialize(ptr_in, state.national_definitions.global_flag_variables);
	}
	{ // military definitions
		ptr_in = memcpy_serialize(ptr_in, state.military_definitions.great_wars_enabled);
		ptr_in = memcpy_serialize(ptr_in, state.military_definitions.world_wars_enabled);
	}

	// data container contribution
	dcon::load_record loaded = state.world.make_serialize_record_store_save();
	std::byte* start = reinterpret_cast<std::byte*>(ptr_in);
	state.world.serialize(start, loaded);

	return reinterpret_cast<uint8_t*>(start);
}
size_t sizeof_save_section(sys::state& state) {
	size_t sz = 0;

	// hand-written contribution

	sz += serialize_size(state.unit_names);
	sz += sizeof(state.local_player_nation);
	sz += sizeof(state.current_date);
	sz += sizeof(state.game_seed);
	sz += sizeof(state.crisis_state);
	sz += serialize_size(state.crisis_participants);
	sz += sizeof(state.current_crisis);
	sz += sizeof(state.crisis_temperature);
	sz += sizeof(state.primary_crisis_attacker);
	sz += sizeof(state.primary_crisis_defender);

	{ // national definitions
		sz += serialize_size(state.national_definitions.global_flag_variables);
	}
	{ // military definitions
		sz += sizeof(state.military_definitions.great_wars_enabled);
		sz += sizeof(state.military_definitions.world_wars_enabled);
	}

	// data container contribution
	dcon::load_record loaded = state.world.make_serialize_record_store_save();
	sz += state.world.serialize_size(loaded);

	return sz;
}

void write_scenario_file(sys::state& state, native_string_view name) {
	scenario_header header;

	size_t scenario_space = sizeof_scenario_section(state);
	size_t save_space = sizeof_save_section(state);

	// this is an upper bound, since compacting the data may require less space
	size_t total_size = sizeof_scenario_header(header) + ZSTD_compressBound(scenario_space) + ZSTD_compressBound(save_space)
		+ sizeof(uint32_t) * 4;

	uint8_t* temp_buffer = new uint8_t[total_size];
	uint8_t* buffer_position = temp_buffer;

	buffer_position = write_scenario_header(buffer_position, header);


	uint8_t* temp_scenario_buffer = new uint8_t[scenario_space];
	auto last_written = write_scenario_section(temp_scenario_buffer, state);
	auto last_written_count = last_written - temp_scenario_buffer;
	assert(size_t(last_written_count) == scenario_space);
	buffer_position = write_compressed_section(buffer_position, temp_scenario_buffer, uint32_t(scenario_space));
	delete[] temp_scenario_buffer;

	uint8_t* temp_save_buffer = new uint8_t[save_space];
	auto last_save_written = write_save_section(temp_save_buffer, state);
	auto last_save_written_count = last_save_written - temp_save_buffer;
	assert(size_t(last_save_written_count) == save_space);
	buffer_position = write_compressed_section(buffer_position, temp_save_buffer, uint32_t(save_space));
	delete[] temp_save_buffer;

	auto total_size_used = buffer_position - temp_buffer;

	simple_fs::write_file(simple_fs::get_or_create_scenario_directory(), name, reinterpret_cast<char*>(temp_buffer), uint32_t(total_size_used));

	delete[] temp_buffer;
}
bool try_read_scenario_file(sys::state& state, native_string_view name) {
	auto dir = simple_fs::get_or_create_scenario_directory();
	auto save_file = open_file(dir, name);
	if(save_file) {
		scenario_header header;
		header.version = 0;

		auto contents = simple_fs::view_contents(*save_file);
		uint8_t const* buffer_pos = reinterpret_cast<uint8_t const*>(contents.data);
		auto file_end = buffer_pos + contents.file_size;

		if(contents.file_size > sizeof_scenario_header(header)) {
			buffer_pos = read_scenario_header(buffer_pos, header);
		}

		if(header.version != sys::scenario_file_version) {
			return false;
		}

		buffer_pos = with_decompressed_section(buffer_pos, [&](uint8_t const* ptr_in, uint32_t length) {
			read_scenario_section(ptr_in, ptr_in + length, state);
		});

		return true;
	} else {
		return false;
	}
}

bool try_read_scenario_and_save_file(sys::state& state, native_string_view name) {
	auto dir = simple_fs::get_or_create_scenario_directory();
	auto save_file = open_file(dir, name);
	if(save_file) {
		scenario_header header;
		header.version = 0;

		auto contents = simple_fs::view_contents(*save_file);
		uint8_t const* buffer_pos = reinterpret_cast<uint8_t const*>(contents.data);
		auto file_end = buffer_pos + contents.file_size;

		if(contents.file_size > sizeof_scenario_header(header)) {
			buffer_pos = read_scenario_header(buffer_pos, header);
		}

		if(header.version != sys::scenario_file_version) {
			return false;
		}

		buffer_pos = with_decompressed_section(buffer_pos, [&](uint8_t const* ptr_in, uint32_t length) {
			read_scenario_section(ptr_in, ptr_in + length, state);
		});
		buffer_pos = with_decompressed_section(buffer_pos, [&](uint8_t const* ptr_in, uint32_t length) {
			read_save_section(ptr_in, ptr_in + length, state);
		});

		state.game_seed = uint32_t(std::random_device()());

		return true;
	} else {
		return false;
	}
}

void write_save_file(sys::state& state, native_string_view name) {
	save_header header;

	size_t save_space = sizeof_save_section(state);

	// this is an upper bound, since compacting the data may require less space
	size_t total_size = sizeof_save_header(header) + ZSTD_compressBound(save_space)
		+ sizeof(uint32_t) * 2;

	uint8_t* temp_buffer = new uint8_t[total_size];
	uint8_t* buffer_position = temp_buffer;

	buffer_position = write_save_header(buffer_position, header);

	uint8_t* temp_save_buffer = new uint8_t[save_space];
	write_save_section(temp_save_buffer, state);
	buffer_position = write_compressed_section(buffer_position, temp_save_buffer, uint32_t(save_space));
	delete[] temp_save_buffer;

	auto total_size_used = buffer_position - temp_buffer;

	simple_fs::write_file(simple_fs::get_or_create_save_game_directory(), name, reinterpret_cast<char*>(temp_buffer), uint32_t(total_size_used));

	delete[] temp_buffer;
}
bool try_read_save_file(sys::state& state, native_string_view name) {
	auto dir = simple_fs::get_or_create_save_game_directory();
	auto save_file = open_file(dir, name);
	if(save_file) {
		save_header header;
		header.version = 0;

		auto contents = simple_fs::view_contents(*save_file);
		uint8_t const* buffer_pos = reinterpret_cast<uint8_t const*>(contents.data);
		auto file_end = buffer_pos + contents.file_size;

		if(contents.file_size > sizeof_save_header(header)) {
			buffer_pos = read_save_header(buffer_pos, header);
		}

		if(header.version != sys::save_file_version) {
			return false;
		}

		buffer_pos = with_decompressed_section(buffer_pos, [&](uint8_t const* ptr_in, uint32_t length) {
			read_save_section(ptr_in, ptr_in + length, state);
		});

		return true;
	} else {
		return false;
	}
}

}
