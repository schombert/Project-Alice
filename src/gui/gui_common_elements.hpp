#pragma once

#include "dcon_generated.hpp"
#include "demographics.hpp"
#include "gui_graphics.hpp"
#include "gui_element_types.hpp"
#include "military.hpp"
#include "nations.hpp"
#include "politics.hpp"
#include "province.hpp"
#include "rebels.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include <unordered_map>
#include <vector>

namespace ui {

// Filters used on both production and diplomacy tabs for the country lists
enum class country_list_filter : uint8_t {
	all,
	neighbors,
	sphere,
	enemies,
	allies,
	continent
};
class button_press_notification{};

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

template<class T>
class generic_multiline_name_text : public multiline_text_element_base {
protected:
	T obj_id{};

public:
	void on_update(sys::state& state) noexcept override {
		auto color = black_text ? text::text_color::black : text::text_color::white;
		auto container = text::create_endless_layout(
			internal_layout,
			text::layout_parameters{ 0, 0, base_data.size.x, base_data.size.y, base_data.data.text.font_handle, 0, text::alignment::left, color }
		);
		auto fat_id = dcon::fatten(state.world, obj_id);
		auto box = text::open_layout_box(container);
		text::add_to_layout_box(container, state, box, fat_id.get_name(), text::substitution_map{ });
		text::close_layout_box(container, box);
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

class state_name_text : public standard_state_instance_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		return text::get_dynamic_state_name(state, state_id);
	}
};

class state_admin_efficiency_text : public standard_state_instance_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		return text::format_percentage(province::state_admin_efficiency(state, state_id), 1);
	}
};

class state_aristocrat_presence_text : public standard_state_instance_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto total_pop = state.world.state_instance_get_demographics(state_id, demographics::total);
		auto aristocrat_key = demographics::to_key(state, state.culture_definitions.aristocrat);
		auto aristocrat_amount = state.world.state_instance_get_demographics(state_id, aristocrat_key);
		return text::format_percentage(aristocrat_amount / total_pop, 1);
	}
};

class standard_province_progress_bar : public progress_bar {
protected:
	dcon::province_id prov_id{};

public:
	virtual float get_progress(sys::state& state) noexcept {
		return 0.f;
	}

	void on_update(sys::state& state) noexcept override {
		progress = get_progress(state);
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

class province_liferating_progress_bar : public standard_province_progress_bar {
public:
	float get_progress(sys::state& state) noexcept override {
		return state.world.province_get_life_rating(prov_id) / 100.f;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(auto k = state.key_to_text_sequence.find(std::string_view("provinceview_liferating")); k != state.key_to_text_sequence.end()) {
			auto box = text::open_layout_box(contents, 0);

			text::substitution_map lr_sub;
			text::add_to_substitution_map(lr_sub, text::variable_type::value, text::fp_one_place{ float(state.world.province_get_life_rating(prov_id)) });

			text::add_to_layout_box(contents, state, box, k->second, lr_sub);
			text::close_layout_box(contents, box);
		}
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

class province_rgo_employment_progress_icon : public standard_province_icon {
public:
	int32_t get_icon_frame(sys::state& state) noexcept override {
		auto employment_ratio = state.world.province_get_rgo_employment(prov_id);
		return int32_t(10.f * employment_ratio);
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

class standard_rebel_type_icon : public opaque_element_base {
protected:
	dcon::rebel_type_id rebel_type_id{};

public:
	virtual int32_t get_icon_frame(sys::state& state) noexcept {
		return 0;
	}

	void on_update(sys::state& state) noexcept override {
		frame = get_icon_frame(state);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::rebel_type_id>()) {
			rebel_type_id = any_cast<dcon::rebel_type_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class rebel_type_icon : public standard_rebel_type_icon {
public:
	int32_t get_icon_frame(sys::state& state) noexcept override {
		return int32_t(state.world.rebel_type_get_icon(rebel_type_id) - 1);
	}
};

class standard_rebel_faction_text : public simple_text_element_base {
protected:
	dcon::rebel_faction_id rebel_faction_id{};

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
		if(payload.holds_type<dcon::rebel_faction_id>()) {
			rebel_faction_id = any_cast<dcon::rebel_faction_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class rebel_faction_size_text : public standard_rebel_faction_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		float total = 0.f;
		for(auto member : state.world.rebel_faction_get_pop_rebellion_membership(rebel_faction_id)) {
			total += member.get_pop().get_size();
		}
		return text::prettify(int64_t(total));
	}
};

class rebel_faction_active_brigade_count_text : public standard_rebel_faction_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto count = rebel::get_faction_brigades_active(state, rebel_faction_id);
		return text::prettify(count);
	}
};

class rebel_faction_ready_brigade_count_text : public standard_rebel_faction_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto count = rebel::get_faction_brigades_ready(state, rebel_faction_id);
		return text::prettify(count);
	}
};

class rebel_faction_organization_text : public standard_rebel_faction_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto org = rebel::get_faction_organization(state, rebel_faction_id);
		return text::format_percentage(org, 1);
	}
};

class rebel_faction_revolt_risk_text : public standard_rebel_faction_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto risk = rebel::get_faction_revolt_risk(state, rebel_faction_id);
		return text::format_percentage(risk, 1);
	}
};

class standard_rebel_faction_multiline_text : public multiline_text_element_base {
protected:
	dcon::rebel_faction_id rebel_faction_id{};

public:
	virtual void populate_layout(sys::state& state, text::endless_layout& contents) noexcept { }

	void on_update(sys::state& state) noexcept override {
		auto color = black_text ? text::text_color::black : text::text_color::white;
		auto container = text::create_endless_layout(
			internal_layout,
			text::layout_parameters{ 0, 0, base_data.size.x, base_data.size.y, base_data.data.text.font_handle, 0, text::alignment::left, color }
		);
		populate_layout(state, container);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::rebel_faction_id>()) {
			rebel_faction_id = any_cast<dcon::rebel_faction_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class rebel_faction_name_text : public standard_rebel_faction_multiline_text {
public:
	void populate_layout(sys::state& state, text::endless_layout& contents) noexcept override {
			auto fat_id = dcon::fatten(state.world, rebel_faction_id);
			auto box = text::open_layout_box(contents);
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::country, text::get_adjective_as_string(state, fat_id.get_ruler_from_rebellion_within()));
			auto culture = fat_id.get_primary_culture();
			auto defection_target = fat_id.get_defection_target();
			if(culture.id) {
				text::add_to_substitution_map(sub, text::variable_type::culture, text::get_name_as_string(state, culture));
			} else if(defection_target.id) {
				auto adjective = text::get_adjective_as_string(state, defection_target);
				text::add_to_substitution_map(sub, text::variable_type::indep, adjective);
				text::add_to_substitution_map(sub, text::variable_type::union_adj, adjective);
			}
			text::add_to_layout_box(contents, state, box, fat_id.get_type().get_name(), sub);
			text::close_layout_box(contents, box);
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
		return text::format_percentage(province::crime_fighting_efficiency(state, province_id), 1);
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
		return text::format_percentage(state.world.province_get_rgo_employment(province_id), 1);
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

class standard_nation_issue_option_text : public simple_text_element_base {
protected:
	dcon::nation_id nation_id{};
	dcon::issue_option_id issue_option_id{};

public:
	virtual std::string get_text(sys::state& state) noexcept {
		return "";
	}

	void on_update(sys::state& state) noexcept override {
		set_text(state, get_text(state));
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::issue_option_id>()) {
			issue_option_id = any_cast<dcon::issue_option_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::nation_id>()) {
			nation_id = any_cast<dcon::nation_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class issue_option_popular_support : public standard_nation_issue_option_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		return text::format_percentage(politics::get_popular_support(state, nation_id, issue_option_id), 1);
	}
};

class issue_option_voter_support : public standard_nation_issue_option_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		return text::format_percentage(politics::get_voter_support(state, nation_id, issue_option_id), 1);
	}
};

class standard_nation_reform_option_icon : public image_element_base {
protected:
	dcon::nation_id nation_id{};
	dcon::reform_option_id reform_option_id{};

public:
	virtual int32_t get_icon_frame(sys::state& state) noexcept {
		return 0;
	}

	void on_update(sys::state& state) noexcept override {
		frame = get_icon_frame(state);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::reform_option_id>()) {
			reform_option_id = any_cast<dcon::reform_option_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::nation_id>()) {
			nation_id = any_cast<dcon::nation_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class reform_selected_icon : public standard_nation_reform_option_icon {
public:
	void on_update(sys::state& state) noexcept override {
		standard_nation_reform_option_icon::on_update(state);
		set_visible(state, politics::reform_is_selected(state, nation_id, reform_option_id));
	}
};

class standard_nation_issue_option_icon : public image_element_base {
protected:
	dcon::nation_id nation_id{};
	dcon::issue_option_id issue_option_id{};

public:
	virtual int32_t get_icon_frame(sys::state& state) noexcept {
		return 0;
	}

	void on_update(sys::state& state) noexcept override {
		frame = get_icon_frame(state);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::issue_option_id>()) {
			issue_option_id = any_cast<dcon::issue_option_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::nation_id>()) {
			nation_id = any_cast<dcon::nation_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class issue_selected_icon : public standard_nation_issue_option_icon {
public:
	void on_update(sys::state& state) noexcept override {
		standard_nation_issue_option_icon::on_update(state);
		set_visible(state, politics::issue_is_selected(state, nation_id, issue_option_id));
	}
};

class standard_nation_reform_option_button : public button_element_base {
protected:
	dcon::nation_id nation_id{};
	dcon::reform_option_id reform_option_id{};

public:
	virtual int32_t get_icon_frame(sys::state& state) noexcept {
		return 0;
	}

	void on_update(sys::state& state) noexcept override {
		frame = get_icon_frame(state);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::reform_option_id>()) {
			reform_option_id = any_cast<dcon::reform_option_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::nation_id>()) {
			nation_id = any_cast<dcon::nation_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class standard_nation_issue_option_button : public button_element_base {
protected:
	dcon::nation_id nation_id{};
	dcon::issue_option_id issue_option_id{};

public:
	virtual int32_t get_icon_frame(sys::state& state) noexcept {
		return 0;
	}

	void on_update(sys::state& state) noexcept override {
		frame = get_icon_frame(state);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::issue_option_id>()) {
			issue_option_id = any_cast<dcon::issue_option_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::nation_id>()) {
			nation_id = any_cast<dcon::nation_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class standard_nation_multiline_text : public multiline_text_element_base {
protected:
	dcon::nation_id nation_id{};

public:
	virtual void populate_layout(sys::state& state, text::endless_layout& contents) noexcept { }

	void on_update(sys::state& state) noexcept override {
		auto color = black_text ? text::text_color::black : text::text_color::white;
		auto container = text::create_endless_layout(
			internal_layout,
			text::layout_parameters{ 0, 0, base_data.size.x, base_data.size.y, base_data.data.text.font_handle, 0, text::alignment::left, color }
		);
		populate_layout(state, container);
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

class standard_nation_national_identity_multiline_text : public multiline_text_element_base {
protected:
	dcon::nation_id nation_id{};
	dcon::national_identity_id national_identity_id{};

public:
	virtual void populate_layout(sys::state& state, text::endless_layout& contents) noexcept { }

	void on_update(sys::state& state) noexcept override {
		auto color = black_text ? text::text_color::black : text::text_color::white;
		auto container = text::create_endless_layout(
			internal_layout,
			text::layout_parameters{ 0, 0, base_data.size.x, base_data.size.y, base_data.data.text.font_handle, 0, text::alignment::left, color }
		);
		populate_layout(state, container);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::national_identity_id>()) {
			national_identity_id = any_cast<dcon::national_identity_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::nation_id>()) {
			nation_id = any_cast<dcon::nation_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class standard_national_identity_text : public simple_text_element_base {
protected:
	dcon::national_identity_id national_identity_id{};

public:
	virtual std::string get_text(sys::state& state) noexcept {
		return "";
	}

	void on_update(sys::state& state) noexcept override {
		set_text(state, get_text(state));
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::national_identity_id>()) {
			national_identity_id = any_cast<dcon::national_identity_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class national_identity_vassal_type_text : public standard_national_identity_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		// TODO: figure out if it should be a substate
		return text::produce_simple_string(state, "satellite");
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

template<uint16_t Rank>
class nation_gp_opinion_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		const auto great_power_id = nations::get_nth_great_power(state, Rank);
		if(!bool(great_power_id))
			return "-";
		auto great_power_rel = state.world.get_gp_relationship_by_gp_influence_pair(nation_id, great_power_id);
		auto fat_id = dcon::fatten(state.world, great_power_rel);
		auto influence = fat_id.get_influence();
		return std::to_string(int32_t(influence));
	}
};

class nation_player_investment_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto uni_rel = state.world.get_unilateral_relationship_by_unilateral_pair(nation_id, state.local_player_nation);
		auto fat_id = dcon::fatten(state.world, uni_rel);
		return text::prettify(fat_id.get_foreign_investment());
	}
};

template<uint16_t Rank>
class nation_gp_investment_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		const auto great_power_id = nations::get_nth_great_power(state, Rank);
		if(!bool(great_power_id))
			return "-";
		auto uni_rel = state.world.get_unilateral_relationship_by_unilateral_pair(nation_id, great_power_id);
		auto fat_id = dcon::fatten(state.world, uni_rel);
		return text::prettify(int64_t(fat_id.get_foreign_investment()));
	}
};

class nation_overlord_flag : public flag_button {
	dcon::nation_id sphereling_id{};
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto ovr_id = state.world.nation_get_in_sphere_of(sphereling_id);
		if(bool(ovr_id)) {
			auto fat_id = dcon::fatten(state.world, ovr_id);
			return fat_id.get_identity_from_identity_holder();
		}
		return dcon::national_identity_id{};
	}

	void on_update(sys::state& state) noexcept override {
		// Only show if there is any overlord
		set_visible(state, bool(get_current_nation(state)));
		set_current_nation(state, get_current_nation(state));
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			sphereling_id = any_cast<dcon::nation_id>(payload);
			on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
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

class nation_player_opinion_text : public standard_nation_text {
	std::string_view get_level_str(uint8_t v) {
		switch(v & nations::influence::level_mask) {
		case nations::influence::level_neutral:
			return "REL_NEUTRAL";
		case nations::influence::level_opposed:
			return "REL_OPPOSED";
		case nations::influence::level_hostile:
			return "REL_HOSTILE";
		case nations::influence::level_cordial:
			return "REL_CORDIAL";
		case nations::influence::level_friendly:
			return "REL_FRIENDLY";
		case nations::influence::level_in_sphere:
			return "REL_SPHERE_OF_INFLUENCE";
		default:
			return "?";
		}
	}
public:
	std::string get_text(sys::state& state) noexcept override {
		auto gp_rel_id = state.world.get_gp_relationship_by_gp_influence_pair(nation_id, state.local_player_nation);
		if(bool(gp_rel_id)) {
			const auto status = state.world.gp_relationship_get_status(gp_rel_id);
			return text::produce_simple_string(state, get_level_str(status));
		}
		return "-";
	}
};

class nation_industries_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		size_t num_factories = 0;
		for(auto si : state.world.nation_get_state_ownership(nation_id))
			province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id p) {
				for(auto f : state.world.province_get_factory_location(p))
					++num_factories;
			});
		return std::to_string(num_factories);
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

class nation_colonial_power_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto fcp = std::to_string(nations::free_colonial_points(state, nation_id));
		auto mcp = std::to_string(nations::max_colonial_points(state, nation_id));
		return fcp + "/" + mcp;
	}
};

class nation_budget_funds_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto budget = nations::get_treasury(state, nation_id);
		return text::format_money(budget);
	}
};

class nation_literacy_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto literacy = state.world.nation_get_demographics(nation_id, demographics::literacy);
		auto total_pop = state.world.nation_get_demographics(nation_id, demographics::total);
		return text::format_percentage(literacy / total_pop, 1);
	}
};

class nation_infamy_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return text::format_float(fat_id.get_infamy(), 2);
	}
};

class nation_war_exhaustion_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return text::format_float(fat_id.get_war_exhaustion(), 2);
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

class nation_daily_research_points_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto points = nations::daily_research_points(state, nation_id);
		return text::format_float(points, 2);
	}
};

class nation_research_points_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto points = state.world.nation_get_research_points(nation_id);
		return text::format_float(points, 1);
	}
};

class nation_suppression_points_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto points = nations::suppression_points(state, nation_id);
		return text::format_float(points, 2);
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
		return text::format_float(points, 0);
	}
};

class nation_brigades_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(fat_id.get_active_regiments());
	}
};

class nation_ships_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);

		int32_t total = 0;
		for(auto nv : fat_id.get_navy_control())
			for(auto shp : nv.get_navy().get_navy_membership())
				total++;
		return std::to_string(total);
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
		return text::format_float(points, 1);
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

class nation_technology_admin_type_text : public standard_nation_text {
public:
	void on_update(sys::state& state) noexcept override {
		auto mod_id = state.world.nation_get_tech_school(nation_id);
		if(mod_id) {
			auto name = text::produce_simple_string(state, state.world.modifier_get_name(mod_id));
			set_text(state, name);
		} else {
			set_text(state, text::produce_simple_string(state, "traditional_academic"));
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

class standard_nation_progress_bar : public progress_bar {
protected:
	dcon::nation_id nation_id{};

public:
	virtual float get_progress(sys::state& state) noexcept {
		return 0.f;
	}

	void on_update(sys::state& state) noexcept override {
		progress = get_progress(state);
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

class nation_westernization_progress_bar : public standard_nation_progress_bar {
public:
	float get_progress(sys::state& state) noexcept override {
		return state.world.nation_get_modifier_values(nation_id, sys::national_mod_offsets::civilization_progress_modifier);
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

class nation_player_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, state.local_player_nation);
		return fat_id.get_identity_from_identity_holder();
	}
};

template<uint16_t Rank>
class nation_gp_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		const auto nat_id = nations::get_nth_great_power(state, Rank);
		if(!bool(nat_id))
			return dcon::national_identity_id{};
		auto fat_id = dcon::fatten(state.world, nat_id);
		return fat_id.get_identity_from_identity_holder();
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

class standard_party_button : public button_element_base {
protected:
	dcon::political_party_id political_party_id{};

public:
	virtual int32_t get_icon_frame(sys::state& state) noexcept {
		return 0;
	}

	void on_update(sys::state& state) noexcept override {
		frame = get_icon_frame(state);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::political_party_id>()) {
			political_party_id = any_cast<dcon::political_party_id>(payload);
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
class religion_type_icon : public button_element_base {
protected:
    dcon::religion_id religion_id{};

public:
    void update(sys::state& state) noexcept {
        auto fat_id = dcon::fatten(state.world, religion_id);
        frame = int32_t(fat_id.get_icon() - 1);
    }

    message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
        if(payload.holds_type<dcon::religion_id>()) {
            religion_id = any_cast<dcon::religion_id>(payload);
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
			set_text(state, text::format_percentage(percentage, 1));
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

class upper_house_piechart : public piechart<dcon::ideology_id> {
protected:
	std::unordered_map<uint8_t, float> get_distribution(sys::state& state) noexcept override {
		std::unordered_map<uint8_t, float> distrib = {};
		Cyto::Any nat_id_payload = dcon::nation_id{};
		if(parent) {
			parent->impl_get(state, nat_id_payload);
			if(nat_id_payload.holds_type<dcon::nation_id>()) {
				auto nat_id = any_cast<dcon::nation_id>(nat_id_payload);
				state.world.for_each_ideology([&](dcon::ideology_id ideo_id) {
					auto weight = .01f * state.world.nation_get_upper_house(nat_id, ideo_id);
					distrib[uint8_t(ideo_id.index())] = weight;
				});
			}
		}
		return distrib;
	}
};

class voter_ideology_piechart : public piechart<dcon::ideology_id> {
protected:
	std::unordered_map<uint8_t, float> get_distribution(sys::state& state) noexcept override {
		std::unordered_map<uint8_t, float> distrib = {};
		Cyto::Any nat_id_payload = dcon::nation_id{};
		if(parent) {
			parent->impl_get(state, nat_id_payload);
			if(nat_id_payload.holds_type<dcon::nation_id>()) {
				auto nat_id = any_cast<dcon::nation_id>(nat_id_payload);
				auto total = politics::vote_total(state, nat_id);
				if(total <= 0.f) {
					enabled = false;
					return distrib;
				} else {
					enabled = true;
				}
				auto ideo_pool = std::vector<float>(state.world.ideology_size());
				state.world.for_each_province([&](dcon::province_id province) {
					if(nat_id == state.world.province_get_nation_from_province_ownership(province)) {
						for(auto pop_loc : state.world.province_get_pop_location(province)) {
							auto pop_id = pop_loc.get_pop();
							auto vote_size = politics::get_weighted_vote_size(state, nat_id, pop_id.id);
							state.world.for_each_ideology([&](dcon::ideology_id iid) {
								auto dkey = pop_demographics::to_key(state, iid);
								ideo_pool[iid.index()] += state.world.pop_get_demographics(pop_id.id, dkey);
							});
						}
					}
				});
				for(size_t i = 0; i < ideo_pool.size(); i++) {
					distrib[uint8_t(i)] = ideo_pool[i] / total;
				}
			}
		}
		return distrib;
	}
};

void trigger_description(sys::state& state, text::columnar_layout& layout, dcon::trigger_key k, int32_t primary_slot = -1, int32_t this_slot = -1, int32_t from_slot = -1);

}
