#pragma once

#include "dcon_generated.hpp"
#include "demographics.hpp"
#include "gui_graphics.hpp"
#include "gui_element_types.hpp"
#include "military.hpp"
#include "nations.hpp"
#include "province.hpp"
#include "system_state.hpp"
#include "text.hpp"

namespace ui {

template<class T>
class generic_name_text : public simple_text_element_base {
protected:
	T obj_id{};

public:
	void on_update(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, obj_id);
		set_text(state, text::get_name_as_string(state, fat_id));
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<T>()) {
			obj_id = any_cast<T>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class standard_state_instance_text : public simple_text_element_base {
protected:
	dcon::state_instance_id state_id{};

public:
	virtual std::string get_text(sys::state& state) noexcept {
		return "";
	}

	void on_update(sys::state& state) noexcept override {
		set_text(state, get_text(state));
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::state_instance_id>()) {
			state_id = any_cast<dcon::state_instance_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class state_admin_efficiency_text : public standard_state_instance_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		return text::format_percentage(province::state_admin_efficiency(state, state_id), 3);
	}
};

class state_aristocrat_presence_text : public standard_state_instance_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto total_pop = state.world.state_instance_get_demographics(state_id, demographics::total);
		auto aristocrat_key = demographics::to_key(state, state.culture_definitions.aristocrat);
		auto aristocrat_amount = state.world.state_instance_get_demographics(state_id, aristocrat_key);
		return text::format_percentage(aristocrat_amount / total_pop, 3);
	}
};

class standard_province_icon : public opaque_element_base {
protected:
	dcon::province_id prov_id{};

public:
	virtual int32_t get_icon_frame(sys::state& state) noexcept {
		return 0;
	}

	void on_update(sys::state& state) noexcept override {
		frame = get_icon_frame(state);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			prov_id = any_cast<dcon::province_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class province_rgo_icon : public standard_province_icon {
public:
	int32_t get_icon_frame(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, prov_id);
		return fat_id.get_rgo().get_icon();
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};

class province_fort_icon : public standard_province_icon {
public:
	int32_t get_icon_frame(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, prov_id);
		return fat_id.get_fort_level();
	}
};

class province_naval_base_icon : public standard_province_icon {
public:
	int32_t get_icon_frame(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, prov_id);
		return fat_id.get_naval_base_level();
	}
};

class province_railroad_icon : public standard_province_icon {
public:
	int32_t get_icon_frame(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, prov_id);
		return fat_id.get_railroad_level();
	}
};

class province_crime_icon : public standard_province_icon {
public:
	int32_t get_icon_frame(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, prov_id);
		return fat_id.get_crime().index();
	}
};

class standard_province_text : public simple_text_element_base {
protected:
	dcon::province_id province_id{};

public:
	virtual std::string get_text(sys::state& state) noexcept {
		return "";
	}

	void on_update(sys::state& state) noexcept override {
		set_text(state, get_text(state));
	}

	message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
		return message_result::consumed;
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			province_id = any_cast<dcon::province_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class province_population_text : public standard_province_text {

public:
	std::string get_text(sys::state& state) noexcept override {
		auto total_pop = state.world.province_get_demographics(province_id, demographics::total);
		return text::prettify(int32_t(total_pop));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(auto k = state.key_to_text_sequence.find(std::string_view("provinceview_totalpop")); k != state.key_to_text_sequence.end()) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{ });
			text::close_layout_box(contents, box);
		}
	}
};

class province_supply_limit_text : public standard_province_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto supply = military::supply_limit_in_province(state, state.local_player_nation, province_id);
		return std::to_string(supply);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(auto k = state.key_to_text_sequence.find(std::string_view("provinceview_supply_limit")); k != state.key_to_text_sequence.end()) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{ });
			text::close_layout_box(contents, box);
		}
	}
};

class province_crime_name_text : public standard_province_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, province_id);
		auto crime_id = fat_id.get_crime();
		if(crime_id) {
			return text::produce_simple_string(state, state.culture_definitions.crimes[crime_id].name);
		} else {
			return "";
		}
	}
};

class province_crime_fighting_text : public standard_province_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		return text::format_percentage(province::crime_fighting_efficiency(state, province_id), 3);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(auto k = state.key_to_text_sequence.find(std::string_view("provinceview_crimefight")); k != state.key_to_text_sequence.end()) {
			auto box = text::open_layout_box(contents, 0);
			text::substitution_map cf_sub;
			text::add_to_substitution_map(cf_sub, text::variable_type::value, text::fp_one_place{ province::crime_fighting_efficiency(state, province_id) * 100 });
			text::add_to_layout_box(contents, state, box, k->second, cf_sub);
			text::close_layout_box(contents, box);
		}
	}
};

class province_rebel_percent_text : public standard_province_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		return text::format_float(province::revolt_risk(state, province_id),2);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		// Not sure if this is the right key, but looking through the CSV files, this is the only one with a value you can substitute.
		if(auto k = state.key_to_text_sequence.find(std::string_view("avg_mil_on_map")); k != state.key_to_text_sequence.end()) {
			auto box = text::open_layout_box(contents, 0);
			text::substitution_map cf_sub;
			text::add_to_substitution_map(cf_sub, text::variable_type::value, text::fp_one_place{ province::revolt_risk(state, province_id) * 100 });
			text::add_to_layout_box(contents, state, box, k->second, cf_sub);
			text::close_layout_box(contents, box);
		}
	}
};

class province_rgo_workers_text : public standard_province_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		return text::prettify(int32_t(province::rgo_employment(state, province_id)));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(auto k = state.key_to_text_sequence.find(std::string_view("provinceview_employment")); k != state.key_to_text_sequence.end()) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{ });
			text::close_layout_box(contents, box);
		}
	}
};

class province_rgo_employment_percent_text : public standard_province_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto employed = province::rgo_employment(state, province_id);
		auto max_employed = province::rgo_maximum_employment(state, province_id);
		if(max_employed > 0.f) {
			return text::format_percentage(employed / max_employed, 3);
		} else {
			return text::format_percentage(0.f, 3);
		}
	}
};

class province_goods_produced_text : public standard_province_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		return text::format_float(province::rgo_production_quantity(state, province_id), 3);
	}
};

class province_income_text : public standard_province_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		return text::format_money(province::rgo_income(state, province_id));
	}
};

class province_migration_text : public standard_province_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		return text::prettify(int32_t(province::monthly_net_pop_growth(state, province_id)));
	}
};

class province_pop_growth_text : public standard_province_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto internal = province::monthly_net_pop_internal_migration(state, province_id);
		auto external = province::monthly_net_pop_external_migration(state, province_id);
		return text::prettify(int32_t(internal + external));
	}
};

class province_army_size_text : public standard_province_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto built = military::regiments_created_from_province(state, province_id);
		auto max_possible = military::regiments_max_possible_from_province(state, province_id);
		return text::format_ratio(built, max_possible);
	}
};

class province_state_name_text : public standard_province_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		return text::get_province_state_name(state, province_id);
	}
};

class standard_nation_text : public simple_text_element_base {
protected:
	dcon::nation_id nation_id{};

public:
	virtual std::string get_text(sys::state& state) noexcept {
		return "";
	}

	void on_update(sys::state& state) noexcept override {
		set_text(state, get_text(state));
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			nation_id = any_cast<dcon::nation_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class nation_prestige_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		return std::to_string(int32_t(nations::prestige_score(state, nation_id)));
	}
};

class nation_industry_score_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(int32_t(fat_id.get_industrial_score()));
	}
};

class nation_military_score_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(int32_t(fat_id.get_military_score()));
	}
};

class nation_total_score_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(int32_t(nations::prestige_score(state, nation_id) + fat_id.get_industrial_score() + fat_id.get_military_score()));
	}
};

class nation_prestige_rank_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(fat_id.get_prestige_rank());
	}
};

class nation_industry_rank_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(fat_id.get_industrial_rank());
	}
};

class nation_military_rank_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(fat_id.get_military_rank());
	}
};

class nation_rank_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(fat_id.get_rank());
	}
};

class nation_status_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		switch(nations::get_status(state, nation_id)) {
			case nations::status::great_power:
				return text::produce_simple_string(state, "diplomacy_greatnation_status");
			case nations::status::secondary_power:
				return text::produce_simple_string(state, "diplomacy_colonialnation_status");
			case nations::status::civilized:
				return text::produce_simple_string(state, "diplomacy_civilizednation_status");
			case nations::status::westernizing:
				return text::produce_simple_string(state, "diplomacy_almost_western_nation_status");
			case nations::status::uncivilized:
				return text::produce_simple_string(state, "diplomacy_uncivilizednation_status");
			case nations::status::primitive:
				return text::produce_simple_string(state, "diplomacy_primitivenation_status");
			default:
				return text::produce_simple_string(state, "diplomacy_greatnation_status");
		}
	}
};

class nation_ruling_party_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return text::get_name_as_string(state, fat_id.get_ruling_party());
	}
};

class nation_government_type_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		auto gov_type_id = fat_id.get_government_type();
		auto gov_name_seq = state.culture_definitions.governments[gov_type_id].name;
		return text::produce_simple_string(state, gov_name_seq);
	}
};

class nation_sphere_list_label : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		if(bool(fat_id.get_in_sphere_of().id)) {
			return text::produce_simple_string(state, "rel_sphere_of_influence") + ":";
		} else {
			return text::produce_simple_string(state, "diplomacy_sphere_label");
		}
	}
};

class nation_puppet_list_label : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		auto puppets_iter = state.world.nation_get_overlord_as_ruler(nation_id);
		if(puppets_iter.begin() < puppets_iter.end()) {
			return text::produce_simple_string(state, "country_puppets_list");
		} else if(fat_id.get_is_substate()) {
			return text::produce_simple_string(state, "substate_of_nocolor") + ":";
		} else {
			return text::produce_simple_string(state, "satellite_of_nocolor") + ":";
		}
	}
};

class nation_player_relations_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(nation_id, state.local_player_nation);
		auto fat_rel = dcon::fatten(state.world, rel);
		return std::to_string(fat_rel.get_value());
	}
};

class nation_militancy_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto militancy = state.world.nation_get_demographics(nation_id, demographics::militancy);
		auto total_pop = state.world.nation_get_demographics(nation_id, demographics::total);
		return text::format_float(militancy / total_pop);
	}
};

class nation_consciousness_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto consciousness = state.world.nation_get_demographics(nation_id, demographics::consciousness);
		auto total_pop = state.world.nation_get_demographics(nation_id, demographics::total);
		return text::format_float(consciousness / total_pop);
	}
};

class nation_literacy_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto literacy = state.world.nation_get_demographics(nation_id, demographics::literacy);
		auto total_pop = state.world.nation_get_demographics(nation_id, demographics::total);
		return text::format_percentage(literacy / total_pop, 3);
	}
};

class nation_infamy_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return text::format_float(fat_id.get_infamy(), 3);
	}
};

class nation_population_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto total_pop = state.world.nation_get_demographics(nation_id, demographics::total);
		return text::prettify(int32_t(total_pop));
	}
};

class nation_current_research_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto tech_id = nations::current_research(state, nation_id);
		if(tech_id) {
			return text::get_name_as_string(state, dcon::fatten(state.world, tech_id));
		} else {
			return text::produce_simple_string(state, "tb_tech_no_current");
		}
	}
};

class nation_research_points_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto points = nations::daily_research_points(state, nation_id);
		return text::format_float(points, 3);
	}
};

class nation_suppression_points_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto points = nations::suppression_points(state, nation_id);
		return text::format_float(points, 3);
	}
};

class nation_focus_allocation_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto available = nations::max_national_focuses(state, nation_id);
		auto in_use = nations::national_focuses_in_use(state, nation_id);
		return text::format_ratio(in_use, available);
	}
};

class nation_diplomatic_points_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto points = nations::diplomatic_points(state, nation_id);
		return text::format_float(points, 1);
	}
};

class nation_brigade_allocation_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto available = state.world.nation_get_recruitable_regiments(nation_id);
		auto in_use = state.world.nation_get_active_regiments(nation_id);
		return text::format_ratio(in_use, available + in_use);
	}
};

class nation_mobilization_size_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		int32_t sum = 0;
		auto fat_id = dcon::fatten(state.world, nation_id);
		for(auto prov_own : fat_id.get_province_ownership_as_nation()) {
			auto prov = prov_own.get_province();
			sum += military::mobilized_regiments_possible_from_province(state, prov.id);
		}
		return std::to_string(sum);
	}
};

class nation_navy_allocation_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto available = military::naval_supply_points(state, nation_id);
		auto in_use = military::naval_supply_points_used(state, nation_id);
		return text::format_ratio(in_use, available);
	}
};

class nation_leadership_points_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto points = nations::leadership_points(state, nation_id);
		return text::format_float(points, 2);
	}
};

class nation_plurality_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto plurality = state.world.nation_get_plurality(nation_id);
		return std::to_string(int32_t(plurality)) + '%';
	}
};

class nation_revanchism_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto revanchism = state.world.nation_get_revanchism(nation_id);
		return std::to_string(int32_t(revanchism)) + '%';
	}
};

class nation_can_do_social_reform_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		if(nations::has_social_reform_available(state, nation_id)) {
			// yes, the original localisation says "refroms"
			return text::produce_simple_string(state, "politics_can_do_social_refroms");
		} else {
			return text::produce_simple_string(state, "politics_can_not_do_social_refroms");
		}
	}
};

class nation_can_do_political_reform_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		if(nations::has_political_reform_available(state, nation_id)) {
			// yes, the original localisation says "refroms"
			return text::produce_simple_string(state, "politics_can_do_political_refroms");
		} else {
			return text::produce_simple_string(state, "politics_can_not_do_political_refroms");
		}
	}
};

class standard_nation_icon : public image_element_base {
protected:
	dcon::nation_id nation_id{};

public:
	virtual int32_t get_icon_frame(sys::state& state) noexcept {
		return 0;
	}

	void on_update(sys::state& state) noexcept override {
		frame = get_icon_frame(state);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			nation_id = any_cast<dcon::nation_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class standard_nation_button : public button_element_base {
protected:
	dcon::nation_id nation_id{};

public:
	virtual int32_t get_icon_frame(sys::state& state) noexcept {
		return 0;
	}

	void on_update(sys::state& state) noexcept override {
		frame = get_icon_frame(state);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			nation_id = any_cast<dcon::nation_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class nation_flag_frame : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state) noexcept override {
		auto status = nations::get_status(state, nation_id);
		return std::min(3, int32_t(status));
	}
};

class nation_national_value_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state) noexcept override {
		auto nat_val = state.world.nation_get_national_value(nation_id);
		return nat_val.get_icon();
	}
};

class nation_can_do_social_reform_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state) noexcept override {
		return int32_t(!nations::has_social_reform_available(state, nation_id));
	}
};

class nation_can_do_political_reform_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state) noexcept override {
		return int32_t(!nations::has_political_reform_available(state, nation_id));
	}
};

class nation_ruling_party_ideology_plupp : public tinted_image_element_base {
protected:
	dcon::nation_id nation_id{};

public:
	uint32_t get_tint_color(sys::state& state) noexcept override {
		auto ruling_party = state.world.nation_get_ruling_party(nation_id);
		auto ideology = state.world.political_party_get_ideology(ruling_party);
		return state.world.ideology_get_color(ideology);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			nation_id = any_cast<dcon::nation_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class ideology_plupp : public tinted_image_element_base {
protected:
	dcon::ideology_id ideology_id{};

public:
	uint32_t get_tint_color(sys::state& state) noexcept override {
		return state.world.ideology_get_color(ideology_id);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::ideology_id>()) {
			ideology_id = any_cast<dcon::ideology_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class pop_type_icon : public button_element_base {
protected:
	dcon::pop_type_id pop_type_id{};

public:
	void update(sys::state& state) noexcept {
		auto fat_id = dcon::fatten(state.world, pop_type_id);
		frame = int32_t(fat_id.get_sprite() - 1);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::pop_type_id>()) {
			pop_type_id = any_cast<dcon::pop_type_id>(payload);
			update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class nation_ideology_percentage_text : public simple_text_element_base {
protected:
	dcon::nation_id nation_id{};
	dcon::ideology_id ideology_id{};

public:
	void on_update(sys::state& state) noexcept override {
		if(nation_id && ideology_id) {
			auto percentage = .01f * state.world.nation_get_upper_house(nation_id, ideology_id);
			set_text(state, text::format_percentage(percentage, 3));
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::ideology_id>()) {
			ideology_id = any_cast<dcon::ideology_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::nation_id>()) {
			nation_id = any_cast<dcon::nation_id>(payload);
			on_update(state);
			return message_result::consumed;
		}  else {
			return message_result::unseen;
		}
	}
};

class upper_house_piechart : public piechart_element_base {
protected:
	std::vector<uint8_t> get_colors(sys::state& state) noexcept override {
		std::vector<uint8_t> colors(resolution * channels);
		Cyto::Any nat_id_payload = dcon::nation_id{};
		size_t i = 0;
		if(parent) {
			parent->impl_get(state, nat_id_payload);
			if(nat_id_payload.holds_type<dcon::nation_id>()) {
				auto nat_id = any_cast<dcon::nation_id>(nat_id_payload);
				dcon::ideology_id last_ideology{};
				state.world.for_each_ideology([&](dcon::ideology_id ideo_id) {
					last_ideology = ideo_id;
					auto ideo_fat_id = dcon::fatten(state.world, ideo_id);
					auto weight = .01f * state.world.nation_get_upper_house(nat_id, ideo_id);
					auto slice_count = std::min(size_t(weight * resolution), i + resolution * channels);
					auto color = ideo_fat_id.get_color();
					for(size_t j = 0; j < slice_count * channels; j += channels) {
						colors[j + i] = uint8_t(color & 0xFF);
						colors[j + i + 1] = uint8_t(color >> 8 & 0xFF);
						colors[j + i + 2] = uint8_t(color >> 16 & 0xFF);
					}
					i += slice_count * channels;
				});
				auto fat_last_ideology = dcon::fatten(state.world, last_ideology);
				auto last_color = fat_last_ideology.get_color();
				for(; i < colors.size(); i += channels) {
					colors[i] = uint8_t(last_color & 0xFF);
					colors[i + 1] = uint8_t(last_color >> 8 & 0xFF);
					colors[i + 2] = uint8_t(last_color >> 16 & 0xFF);
				}
			}
		}
		return colors;
	}
};

}
