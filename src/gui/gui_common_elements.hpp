#pragma once

#include "dcon_generated.hpp"
#include "demographics.hpp"
#include "gui_graphics.hpp"
#include "gui_element_types.hpp"
#include "military.hpp"
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

class standard_province_icon : public image_element_base {
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
};

class province_supply_limit_text : public standard_province_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto supply = military::supply_limit_in_province(state, state.local_player_nation, province_id);
		return std::to_string(supply);
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
};

class province_rebel_percent_text : public standard_province_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto militancy = state.world.province_get_demographics(province_id, demographics::militancy);
		return text::format_float(militancy, 2);
	}
};

class province_rgo_workers_text : public standard_province_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		return text::prettify(int32_t(province::rgo_employment(state, province_id)));
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
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(int32_t(fat_id.get_prestige()));
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
		return std::to_string(int32_t(fat_id.get_prestige() + fat_id.get_industrial_score() + fat_id.get_military_score()));
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

}
