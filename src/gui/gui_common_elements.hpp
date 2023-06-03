#pragma once

#include "dcon_generated.hpp"
#include "demographics.hpp"
#include "economy.hpp"
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

typedef std::variant<
    std::monostate,
    dcon::nation_id,
    dcon::state_instance_id,
    dcon::province_id>
    pop_list_filter;

void trigger_description(sys::state& state, text::layout_base& layout, dcon::trigger_key k, int32_t primary_slot = -1, int32_t this_slot = -1, int32_t from_slot = -1);
void multiplicative_value_modifier_description(sys::state& state, text::layout_base& layout, dcon::value_modifier_key modifier, int32_t primary, int32_t this_slot, int32_t from_slot);
void additive_value_modifier_description(sys::state& state, text::layout_base& layout, dcon::value_modifier_key modifier, int32_t primary, int32_t this_slot, int32_t from_slot);
void modifier_description(sys::state& state, text::layout_base& layout, dcon::modifier_id mid, int32_t indentation = 0);
void acting_modifiers_description(sys::state& state, text::layout_base& layout, dcon::nation_id n, int32_t identation, dcon::national_modifier_value nmid, dcon::provincial_modifier_value pmid);
void effect_description(sys::state& state, text::layout_base& layout, dcon::effect_key k, int32_t primary_slot, int32_t this_slot, int32_t from_slot, uint32_t r_lo, uint32_t r_hi);

// Filters used on both production and diplomacy tabs for the country lists
enum class country_list_filter : uint8_t {
	all,
	neighbors,
	sphere,
	enemies,
	allies,
	deselect_all, // Used only by message filter window
	best_guess,   // Used only by message filter window
	continent
};
class button_press_notification { };

template<class T, class K>
class generic_settable_element : public T {
public:
	K content{};

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<K>()) {
			content = any_cast<K>(payload);
			T::impl_on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

template<class T>
class generic_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = T{};
			parent->impl_get(state, payload);
			T content = any_cast<T>(payload);
			auto fat_id = dcon::fatten(state.world, content);
			simple_text_element_base::set_text(state, text::get_name_as_string(state, fat_id));
		}
	}
};

template<class T>
class generic_multiline_name_text : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = T{};
			parent->impl_get(state, payload);
			T content = any_cast<T>(payload);
			auto color = multiline_text_element_base::black_text ? text::text_color::black : text::text_color::white;
			auto container = text::create_endless_layout(
			    multiline_text_element_base::internal_layout,
			    text::layout_parameters{0, 0, multiline_text_element_base::base_data.size.x, multiline_text_element_base::base_data.size.y, multiline_text_element_base::base_data.data.text.font_handle, 0, text::alignment::left, color});
			auto fat_id = dcon::fatten(state.world, content);
			auto box = text::open_layout_box(container);
			text::add_to_layout_box(container, state, box, fat_id.get_name(), text::substitution_map{});
			text::close_layout_box(container, box);
		}
	}
};

template<typename T>
class generic_simple_text : public simple_text_element_base {
public:
	virtual std::string get_text(sys::state& state, T content) noexcept {
		return "";
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = T{};
			parent->impl_get(state, payload);
			T content = any_cast<T>(payload);
			simple_text_element_base::set_text(state, get_text(state, content));
		}
	}
};

template<typename T>
class generic_multiline_text : public multiline_text_element_base {
public:
	virtual void populate_layout(sys::state& state, text::endless_layout& contents, T id) noexcept { }

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = T{};
			parent->impl_get(state, payload);
			auto content = any_cast<T>(payload);
			auto color = black_text ? text::text_color::black : text::text_color::white;
			auto container = text::create_endless_layout(
			    internal_layout,
			    text::layout_parameters{0, 0, base_data.size.x, base_data.size.y, base_data.data.text.font_handle, 0, text::alignment::left, color});
			populate_layout(state, container, content);
		}
	}
};

class state_name_text : public generic_simple_text<dcon::state_instance_id> {
public:
	std::string get_text(sys::state& state, dcon::state_instance_id content) noexcept override {
		return text::get_dynamic_state_name(state, content);
	}
};

class state_factory_count_text : public generic_simple_text<dcon::state_instance_id> {
public:
	std::string get_text(sys::state& state, dcon::state_instance_id content) noexcept override {
		uint8_t count = 0;
		province::for_each_province_in_state_instance(state, content, [&](dcon::province_id pid) {
			auto fat_id = dcon::fatten(state.world, pid);
			fat_id.for_each_factory_location_as_province([&](dcon::factory_location_id flid) {
				++count;
			});
		});
		return std::to_string(count) + "/" + std::to_string(int32_t(state.defines.factories_per_state));
	}
};

class state_admin_efficiency_text : public generic_simple_text<dcon::state_instance_id> {
public:
	std::string get_text(sys::state& state, dcon::state_instance_id content) noexcept override {
		return text::format_percentage(province::state_admin_efficiency(state, content), 1);
	}
};

class state_aristocrat_presence_text : public generic_simple_text<dcon::state_instance_id> {
public:
	std::string get_text(sys::state& state, dcon::state_instance_id content) noexcept override {
		auto total_pop = state.world.state_instance_get_demographics(content, demographics::total);
		auto aristocrat_key = demographics::to_key(state, state.culture_definitions.aristocrat);
		auto aristocrat_amount = state.world.state_instance_get_demographics(content, aristocrat_key);
		return text::format_percentage(total_pop > 0 ? aristocrat_amount / total_pop : 0.0f, 1);
	}
};

class state_population_text : public generic_simple_text<dcon::state_instance_id> {
public:
	std::string get_text(sys::state& state, dcon::state_instance_id content) noexcept override {
		auto total_pop = state.world.state_instance_get_demographics(content, demographics::total);
		return text::prettify(int32_t(total_pop));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("provinceview_totalpop"), text::substitution_map{});
		text::close_layout_box(contents, box);
	}
};

class standard_province_progress_bar : public progress_bar {
public:
	virtual float get_progress(sys::state& state, dcon::province_id prov_id) noexcept {
		return 0.f;
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto prov_id = any_cast<dcon::province_id>(payload);
			progress = get_progress(state, prov_id);
		}
	}
};

class province_liferating_progress_bar : public standard_province_progress_bar {
public:
	float get_progress(sys::state& state, dcon::province_id prov_id) noexcept override {
		return state.world.province_get_life_rating(prov_id) / 100.f;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto prov_id = any_cast<dcon::province_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::substitution_map lr_sub;
			text::add_to_substitution_map(lr_sub, text::variable_type::value, text::fp_one_place{float(state.world.province_get_life_rating(prov_id))});
			text::localised_format_box(state, contents, box, std::string_view("provinceview_liferating"), lr_sub);
			text::close_layout_box(contents, box);
		}
	}
};

class standard_province_icon : public opaque_element_base {
public:
	virtual int32_t get_icon_frame(sys::state& state, dcon::province_id prov_id) noexcept {
		return 0;
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto prov_id = any_cast<dcon::province_id>(payload);

			frame = get_icon_frame(state, prov_id);
		}
	}
};

class province_rgo_employment_progress_icon : public standard_province_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::province_id prov_id) noexcept override {
		auto employment_ratio = state.world.province_get_rgo_employment(prov_id);
		return int32_t(10.f * employment_ratio);
	}
};

class province_rgo_icon : public standard_province_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::province_id prov_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, prov_id);
		return fat_id.get_rgo().get_icon();
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto prov_id = any_cast<dcon::province_id>(payload);

			auto rgo_good = state.world.province_get_rgo(prov_id);
			if(rgo_good) {
				auto box = text::open_layout_box(contents, 0);
				text::add_to_layout_box(contents, state, box, state.world.commodity_get_name(rgo_good), text::substitution_map{});
				text::close_layout_box(contents, box);
			}
		}
	}
};

class province_crime_icon : public standard_province_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::province_id prov_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, prov_id);
		return fat_id.get_crime().index();
	}
};

class standard_movement_text : public simple_text_element_base {
public:
	virtual std::string get_text(sys::state& state, dcon::movement_id movement_id) noexcept {
		return "";
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::movement_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::movement_id>(payload);
			set_text(state, get_text(state, content));
		}
	}
};

class movement_size_text : public standard_movement_text {
public:
	std::string get_text(sys::state& state, dcon::movement_id movement_id) noexcept override {
		auto size = state.world.movement_get_pop_support(movement_id);
		return text::prettify(int64_t(size));
	}
};

class movement_radicalism_text : public standard_movement_text {
public:
	std::string get_text(sys::state& state, dcon::movement_id movement_id) noexcept override {
		auto radicalism = state.world.movement_get_radicalism(movement_id);
		return text::format_float(radicalism, 1);
	}
};

class movement_issue_name_text : public standard_movement_text {
public:
	std::string get_text(sys::state& state, dcon::movement_id movement_id) noexcept override {
		auto issue = state.world.movement_get_associated_issue_option(movement_id);
		return text::produce_simple_string(state, issue.get_movement_name());
	}
};

class standard_movement_multiline_text : public multiline_text_element_base {
public:
	virtual void populate_layout(sys::state& state, text::endless_layout& contents, dcon::movement_id movement_id) noexcept { }

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::movement_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::movement_id>(payload);

			auto color = black_text ? text::text_color::black : text::text_color::white;
			auto container = text::create_endless_layout(
			    internal_layout,
			    text::layout_parameters{0, 0, base_data.size.x, base_data.size.y, base_data.data.text.font_handle, 0, text::alignment::left, color});
			populate_layout(state, container, content);
		}
	}
};

class movement_nationalist_name_text : public standard_movement_multiline_text {
public:
	void populate_layout(sys::state& state, text::endless_layout& contents, dcon::movement_id movement_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, movement_id);
		auto independence_target = fat_id.get_associated_independence();
		auto box = text::open_layout_box(contents);
		text::substitution_map sub;
		std::string movementAdj;
		if(independence_target.get_cultural_union_of().id) {
			auto k = state.key_to_text_sequence.find(std::string_view("nationalist_union_movement"));
			movementAdj = text::get_adjective_as_string(state, independence_target);
			text::add_to_substitution_map(sub, text::variable_type::country_adj, std::string_view(movementAdj));
			text::add_to_layout_box(contents, state, box, k->second, sub);
		} else {
			auto k = state.key_to_text_sequence.find(std::string_view("nationalist_liberation_movement"));
			movementAdj = text::get_adjective_as_string(state, independence_target);
			text::add_to_substitution_map(sub, text::variable_type::country, std::string_view(movementAdj));
			text::add_to_layout_box(contents, state, box, k->second, sub);
		}
		text::close_layout_box(contents, box);
	}
};

class standard_rebel_faction_text : public simple_text_element_base {
public:
	virtual std::string get_text(sys::state& state, dcon::rebel_faction_id rebel_faction_id) noexcept {
		return "";
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::rebel_faction_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::rebel_faction_id>(payload);
			set_text(state, get_text(state, content));
		}
	}
};

class standard_province_text : public simple_text_element_base {
public:
	virtual std::string get_text(sys::state& state, dcon::province_id province_id) noexcept {
		return "";
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::province_id>(payload);
			set_text(state, get_text(state, content));
		}
	}
};

class province_population_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		auto total_pop = state.world.province_get_demographics(province_id, demographics::total);
		return text::prettify(int32_t(total_pop));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("provinceview_totalpop"));
		text::close_layout_box(contents, box);
	}
};

class province_militancy_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		auto militancy = state.world.province_get_demographics(province_id, demographics::militancy);
		auto total_pop = state.world.province_get_demographics(province_id, demographics::total);
		return text::format_float(militancy / total_pop);
	}
};

class province_consciousness_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		auto consciousness = state.world.province_get_demographics(province_id, demographics::consciousness);
		auto total_pop = state.world.province_get_demographics(province_id, demographics::total);
		return text::format_float(consciousness / total_pop);
	}
};

class province_literacy_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		auto literacy = state.world.province_get_demographics(province_id, demographics::literacy);
		auto total_pop = state.world.province_get_demographics(province_id, demographics::total);
		return text::format_percentage(literacy / total_pop, 1);
	}
};

class province_dominant_culture_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		dcon::culture_id last_culture{};
		float last_culture_amount = 0.f;
		state.world.for_each_culture([&](dcon::culture_id id) {
			auto amount = state.world.province_get_demographics(province_id, demographics::to_key(state, id));
			if(amount > last_culture_amount) {
				last_culture_amount = amount;
				last_culture = id;
			}
		});
		return text::get_name_as_string(state, dcon::fatten(state.world, last_culture));
	}
};
class province_dominant_religion_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		dcon::religion_id last_religion{};
		float last_religion_amount = 0.f;
		state.world.for_each_religion([&](dcon::religion_id id) {
			auto amount = state.world.province_get_demographics(province_id, demographics::to_key(state, id));
			if(amount > last_religion_amount) {
				last_religion_amount = amount;
				last_religion = id;
			}
		});
		return text::get_name_as_string(state, dcon::fatten(state.world, last_religion));
	}
};
class province_dominant_issue_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		dcon::issue_option_id last_issue{};
		float last_issue_amount = 0.f;
		state.world.for_each_issue_option([&](dcon::issue_option_id id) {
			auto amount = state.world.province_get_demographics(province_id, demographics::to_key(state, id));
			if(amount > last_issue_amount) {
				last_issue_amount = amount;
				last_issue = id;
			}
		});
		return text::get_name_as_string(state, dcon::fatten(state.world, last_issue));
	}
};
class province_dominant_ideology_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		dcon::ideology_id last_ideology{};
		float last_ideology_amount = 0.f;
		state.world.for_each_ideology([&](dcon::ideology_id id) {
			auto amount = state.world.province_get_demographics(province_id, demographics::to_key(state, id));
			if(amount > last_ideology_amount) {
				last_ideology_amount = amount;
				last_ideology = id;
			}
		});
		return text::get_name_as_string(state, dcon::fatten(state.world, last_ideology));
	}
};

class province_supply_limit_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		auto supply = military::supply_limit_in_province(state, state.local_player_nation, province_id);
		return std::to_string(supply);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("provinceview_supply_limit"));
		text::close_layout_box(contents, box);
	}
};

class province_crime_name_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
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
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		return text::format_percentage(province::crime_fighting_efficiency(state, province_id), 1);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::province_id>(payload);
			auto box = text::open_layout_box(contents, 0);
			text::localised_single_sub_box(state, contents, box, std::string_view("provinceview_crimefight"), text::variable_type::value, text::fp_one_place{province::crime_fighting_efficiency(state, content) * 100});
			text::close_layout_box(contents, box);
		}
	}
};

class province_rebel_percent_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		return text::format_float(province::revolt_risk(state, province_id), 2);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::province_id>(payload);
			// Not sure if this is the right key, but looking through the CSV files, this is the only one with a value you can substitute.
			auto box = text::open_layout_box(contents, 0);
			text::localised_single_sub_box(state, contents, box, std::string_view("avg_mil_on_map"), text::variable_type::value, text::fp_one_place{province::revolt_risk(state, content) * 100});
			text::close_layout_box(contents, box);
		}
	}
};

class province_rgo_workers_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		return text::prettify(int32_t(province::rgo_employment(state, province_id)));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("provinceview_employment"));
		text::close_layout_box(contents, box);
	}
};

class province_rgo_employment_percent_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		return text::format_percentage(state.world.province_get_rgo_employment(province_id), 1);
	}
};

class province_rgo_name_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		return text::get_name_as_string(state, state.world.province_get_rgo(province_id));
	}
};

class province_player_rgo_size_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		return text::format_float(economy::rgo_effective_size(state, state.local_player_nation, province_id), 2);
	}
};

class province_goods_produced_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		return text::format_float(province::rgo_production_quantity(state, province_id), 3);
	}
};

class province_income_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		return text::format_money(province::rgo_income(state, province_id));
	}
};

class province_migration_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		auto internal = province::monthly_net_pop_internal_migration(state, province_id);
		auto external = province::monthly_net_pop_external_migration(state, province_id);
		return text::prettify(int32_t(internal + external));
	}
};

class province_pop_growth_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		return text::prettify(int32_t(province::monthly_net_pop_growth(state, province_id)));
	}
};

class province_army_size_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		auto built = military::regiments_created_from_province(state, province_id);
		auto max_possible = military::regiments_max_possible_from_province(state, province_id);
		return text::format_ratio(built, max_possible);
	}
};

class province_state_name_text : public standard_province_text {
public:
	std::string get_text(sys::state& state, dcon::province_id province_id) noexcept override {
		return text::get_province_state_name(state, province_id);
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
		    text::layout_parameters{0, 0, base_data.size.x, base_data.size.y, base_data.data.text.font_handle, 0, text::alignment::left, color});
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

class national_identity_vassal_type_text : public generic_simple_text<dcon::national_identity_id> {
public:
	std::string get_text(sys::state& state, dcon::national_identity_id content) noexcept override {
		// TODO: figure out if it should be a substate
		return text::produce_simple_string(state, "satellite");
	}
};

class standard_nation_text : public simple_text_element_base {
public:
	virtual std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept {
		return "";
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);
			set_text(state, get_text(state, content));
		}
	}
};

class nation_gp_opinion_text : public standard_nation_text {
public:
	uint16_t rank = 0;
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		const auto great_power_id = nations::get_nth_great_power(state, rank);
		if(!bool(great_power_id))
			return "0";
		auto great_power_rel = state.world.get_gp_relationship_by_gp_influence_pair(nation_id, great_power_id);
		auto fat_id = dcon::fatten(state.world, great_power_rel);
		auto influence = fat_id.get_influence();
		return std::to_string(int32_t(influence));
	}
};

class nation_player_investment_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto uni_rel = state.world.get_unilateral_relationship_by_unilateral_pair(nation_id, state.local_player_nation);
		auto fat_id = dcon::fatten(state.world, uni_rel);
		return text::format_money(fat_id.get_foreign_investment());
	}
};

class nation_gp_investment_text : public standard_nation_text {
public:
	uint16_t rank = 0;
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		const auto great_power_id = nations::get_nth_great_power(state, rank);
		if(!bool(great_power_id))
			return text::format_money(0.f);
		auto uni_rel = state.world.get_unilateral_relationship_by_unilateral_pair(nation_id, great_power_id);
		auto fat_id = dcon::fatten(state.world, uni_rel);
		return text::format_money(fat_id.get_foreign_investment());
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

class nation_actual_stockpile_spending_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		float amount = state.world.nation_get_effective_construction_spending(nation_id);
		amount += state.world.nation_get_effective_land_spending(nation_id);
		amount += state.world.nation_get_effective_naval_spending(nation_id);
		return text::format_money(amount);
	}
};

class nation_tariff_percentage_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return text::format_percentage(state.world.nation_get_tariffs(nation_id));
	}
};

class nation_gold_income_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return text::format_money(economy::estimate_gold_income(state, nation_id));
	}
};

class nation_loan_spending_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return text::format_money(economy::estimate_loan_payments(state, nation_id));
	}
};

class nation_diplomatic_balance_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return text::format_money(economy::estimate_diplomatic_balance(state, nation_id));
	}
};

class nation_subsidy_spending_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return text::format_money(economy::estimate_subsidy_spending(state, nation_id));
	}
};

class nation_administrative_efficiency_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return text::format_percentage(state.world.nation_get_administrative_efficiency(nation_id));
	}
};

class nation_prestige_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return std::to_string(int32_t(nations::prestige_score(state, nation_id)));
	}
};

class nation_industry_score_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(int32_t(fat_id.get_industrial_score()));
	}
};

class nation_military_score_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(int32_t(fat_id.get_military_score()));
	}
};

class nation_total_score_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(int32_t(nations::prestige_score(state, nation_id) + fat_id.get_industrial_score() + fat_id.get_military_score()));
	}
};

class nation_prestige_rank_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(fat_id.get_prestige_rank());
	}
};

class nation_industry_rank_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(fat_id.get_industrial_rank());
	}
};

class nation_military_rank_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(fat_id.get_military_rank());
	}
};

class nation_rank_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(fat_id.get_rank());
	}
};

class nation_status_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
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

class nation_ruling_party_ideology_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto ruling_party = state.world.nation_get_ruling_party(nation_id);
		auto ideology = state.world.political_party_get_ideology(ruling_party);
		return text::get_name_as_string(state, ideology);
	}
};

class nation_ruling_party_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return text::get_name_as_string(state, fat_id.get_ruling_party());
	}
};

class nation_government_type_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		auto gov_type_id = fat_id.get_government_type();
		auto gov_name_seq = state.culture_definitions.governments[gov_type_id].name;
		return text::produce_simple_string(state, gov_name_seq);
	}
};

class nation_sphere_list_label : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
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
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
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
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto gp_rel_id = state.world.get_gp_relationship_by_gp_influence_pair(nation_id, state.local_player_nation);
		if(bool(gp_rel_id)) {
			const auto status = state.world.gp_relationship_get_status(gp_rel_id);
			return text::get_influence_level_name(state, status);
		}
		return text::produce_simple_string(state, "rel_neutral");
	}
};

class nation_industries_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		size_t num_factories = 0;
		for(auto si : state.world.nation_get_state_ownership(nation_id))
			province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id p) {
				for(auto f : state.world.province_get_factory_location(p))
					++num_factories;
			});
		return std::to_string(num_factories);
	}
};

class nation_provinces_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		size_t num_provinces = 0;
		for(auto si : state.world.nation_get_state_ownership(nation_id))
			province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id p) {
				++num_provinces;
			});
		return std::to_string(num_provinces);
	}
};

class nation_player_relations_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(nation_id, state.local_player_nation);
		auto fat_rel = dcon::fatten(state.world, rel);
		return std::to_string(int32_t(fat_rel.get_value()));
	}
};

class nation_militancy_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto militancy = state.world.nation_get_demographics(nation_id, demographics::militancy);
		auto total_pop = state.world.nation_get_demographics(nation_id, demographics::total);
		return text::format_float(militancy / total_pop);
	}
};

class nation_consciousness_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto consciousness = state.world.nation_get_demographics(nation_id, demographics::consciousness);
		auto total_pop = state.world.nation_get_demographics(nation_id, demographics::total);
		return text::format_float(consciousness / total_pop);
	}
};

class nation_colonial_power_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fcp = nations::free_colonial_points(state, nation_id);
		auto mcp = nations::max_colonial_points(state, nation_id);
		return text::format_ratio(fcp, mcp);
	}
};

class nation_budget_funds_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto budget = nations::get_treasury(state, nation_id);
		return text::format_money(budget);
	}
};

class nation_budget_bank_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto budget = nations::get_bank_funds(state, nation_id);
		return text::format_money(budget);
	}
};

class nation_budget_debt_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto budget = nations::get_debt(state, nation_id);
		return text::format_money(budget);
	}
};

class nation_literacy_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto literacy = state.world.nation_get_demographics(nation_id, demographics::literacy);
		auto total_pop = state.world.nation_get_demographics(nation_id, demographics::total);
		return text::format_percentage(literacy / total_pop, 1);
	}
};

class nation_infamy_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return text::format_float(fat_id.get_infamy(), 2);
	}
};

class nation_war_exhaustion_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return text::format_float(fat_id.get_war_exhaustion(), 2);
	}
};

class nation_population_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto total_pop = state.world.nation_get_demographics(nation_id, demographics::total);
		return text::prettify(int32_t(total_pop));
	}
};

class nation_current_research_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
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
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto points = nations::daily_research_points(state, nation_id);
		return text::format_float(points, 2);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			float sum_from_pops = 0;
			float total_clergy = 0;
			state.world.for_each_pop_type([&](dcon::pop_type_id t) {
				auto rp = state.world.pop_type_get_research_points(t);
				if(rp > 0) {
					total_clergy += state.world.nation_get_demographics(nation_id, demographics::to_key(state, t));
					sum_from_pops += rp * std::min(1.0f, state.world.nation_get_demographics(nation_id, demographics::to_key(state, t)) / (state.world.nation_get_demographics(nation_id, demographics::total) * state.world.pop_type_get_research_optimum(t)));
				}
			});

			total_clergy = (total_clergy / state.world.nation_get_demographics(nation_id, demographics::total)) * 100; // TODO - there is most certainly a better way to do this, it also fails to replicate vic2 closely enough, atleast in my opinion -breizh

			auto box = text::open_layout_box(contents, 0);
			bool clergy_passed = false; // Ugly fix but it should work -breizh

			if(ceil(state.world.nation_get_demographics(nation_id, demographics::to_key(state, state.culture_definitions.clergy))) > 1.0f) {
				text::substitution_map sub1;
				clergy_passed = true;
				text::add_to_substitution_map(sub1, text::variable_type::poptype, state.world.pop_type_get_name(state.culture_definitions.clergy));
				text::add_to_substitution_map(sub1, text::variable_type::value, std::string_view("PLACEHOLDER"));
				text::add_to_substitution_map(sub1, text::variable_type::fraction, text::fp_two_places{(state.world.nation_get_demographics(nation_id, demographics::to_key(state, state.culture_definitions.clergy)) / state.world.nation_get_demographics(nation_id, demographics::total)) * 100});
				text::add_to_substitution_map(sub1, text::variable_type::optimal, text::fp_two_places{(state.world.pop_type_get_research_optimum(state.culture_definitions.clergy) * 100)});
				text::localised_format_box(state, contents, box, std::string_view("tech_daily_researchpoints_tooltip"), sub1);
			}

			if(ceil(state.world.nation_get_demographics(nation_id, demographics::to_key(state, state.culture_definitions.secondary_factory_worker))) > 1.0f) {
				text::substitution_map sub2;
				if(clergy_passed) {
					text::add_line_break_to_layout_box(contents, state, box);
				}
				text::add_to_substitution_map(sub2, text::variable_type::poptype, state.world.pop_type_get_name(state.culture_definitions.secondary_factory_worker));
				text::add_to_substitution_map(sub2, text::variable_type::value, std::string_view("PLACEHOLDER"));
				text::add_to_substitution_map(sub2, text::variable_type::fraction, text::fp_two_places{(state.world.nation_get_demographics(nation_id, demographics::to_key(state, state.culture_definitions.secondary_factory_worker)) / state.world.nation_get_demographics(nation_id, demographics::total)) * 100});
				text::add_to_substitution_map(sub2, text::variable_type::optimal, text::fp_two_places{(state.world.pop_type_get_research_optimum(state.culture_definitions.secondary_factory_worker) * 100)});
				text::localised_format_box(state, contents, box, std::string_view("tech_daily_researchpoints_tooltip"), sub2);
			}
			text::add_divider_to_layout_box(state, contents, box);
			text::close_layout_box(contents, box);

			acting_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::research_points, dcon::provincial_modifier_value{});
		}
	}
};

class nation_research_points_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto points = state.world.nation_get_research_points(nation_id);
		return text::format_float(points, 1);
	}
};

class nation_suppression_points_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto points = nations::suppression_points(state, nation_id);
		return text::format_float(points, 1);
	}
};

class nation_focus_allocation_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto available = nations::max_national_focuses(state, nation_id);
		auto in_use = nations::national_focuses_in_use(state, nation_id);
		return text::format_ratio(in_use, available);
	}
};

class nation_diplomatic_points_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto points = nations::diplomatic_points(state, nation_id);
		return text::format_float(points, 0);
	}
};

class nation_brigades_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return std::to_string(fat_id.get_active_regiments());
	}
};

class nation_ships_text : public standard_nation_text {
protected:
	int32_t get_ship_count(sys::state& state, dcon::nation_id nation_id) {
		auto fat_id = dcon::fatten(state.world, nation_id);
		int32_t total = 0;
		for(auto nv : fat_id.get_navy_control())
			for(auto shp : nv.get_navy().get_navy_membership())
				++total;
		return total;
	}

public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return std::to_string(get_ship_count(state, nation_id));
	}
};

class nation_brigade_allocation_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto available = state.world.nation_get_recruitable_regiments(nation_id);
		auto in_use = state.world.nation_get_active_regiments(nation_id);
		return text::format_ratio(in_use, available);
	}
};

class nation_mobilization_size_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
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
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto available = military::naval_supply_points(state, nation_id);
		auto in_use = military::naval_supply_points_used(state, nation_id);
		return text::format_ratio(in_use, available);
	}
};

class nation_leadership_points_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto points = nations::leadership_points(state, nation_id);
		return text::format_float(points, 1);
	}
};

class nation_plurality_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto plurality = state.world.nation_get_plurality(nation_id);
		return std::to_string(int32_t(plurality)) + '%';
	}
};

class nation_revanchism_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto revanchism = state.world.nation_get_revanchism(nation_id);
		return std::to_string(int32_t(revanchism)) + '%';
	}
};

class nation_can_do_social_reform_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
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
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		if(nations::has_political_reform_available(state, nation_id)) {
			// yes, the original localisation says "refroms"
			return text::produce_simple_string(state, "politics_can_do_political_refroms");
		} else {
			return text::produce_simple_string(state, "politics_can_not_do_political_refroms");
		}
	}
};

class nation_national_value_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto fat_id = dcon::fatten(state.world, nation_id);
		return text::produce_simple_string(state, fat_id.get_national_value().get_name());
	}
};

class nation_technology_admin_type_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto mod_id = state.world.nation_get_tech_school(nation_id);
		if(bool(mod_id)) {
			return text::produce_simple_string(state, state.world.modifier_get_name(mod_id));
		} else {
			return text::produce_simple_string(state, "traditional_academic");
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto fat_id = dcon::fatten(state.world, nation_id);
			auto name = fat_id.get_name();
			if(bool(name)) {
				auto box = text::open_layout_box(contents, 0);
				text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, name), text::text_color::yellow);
				text::close_layout_box(contents, box);
			}
			auto mod_id = fat_id.get_tech_school().id;
			if(bool(mod_id)) {
				modifier_description(state, contents, mod_id);
			}
		}
	}
};

class standard_nation_icon : public image_element_base {
public:
	virtual int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept {
		return 0;
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);
			frame = get_icon_frame(state, nation_id);
		}
	}
};

class standard_nation_progress_bar : public progress_bar {
public:
	virtual float get_progress(sys::state& state, dcon::nation_id nation_id) noexcept {
		return 0.f;
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);
			progress = get_progress(state, nation_id);
		}
	}
};

class nation_westernization_progress_bar : public standard_nation_progress_bar {
public:
	float get_progress(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return state.world.nation_get_modifier_values(nation_id, sys::national_mod_offsets::civilization_progress_modifier);
	}
};

class nation_technology_research_progress : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto tech_id = nations::current_research(state, state.local_player_nation);
		if(bool(tech_id)) {
			progress = state.world.nation_get_research_points(state.local_player_nation) / state.world.technology_get_cost(tech_id);
		} else {
			progress = 0.f;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		auto tech_id = nations::current_research(state, state.local_player_nation);
		if(tech_id) {
			text::substitution_map sub_map;
			text::add_to_substitution_map(sub_map, text::variable_type::tech, dcon::fatten(state.world, tech_id).get_name());
			text::localised_format_box(state, contents, box, "technologyview_under_research_tooltip", sub_map);
		} else {
			text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "technologyview_no_research"), text::text_color::white);
		}
		text::close_layout_box(contents, box);
	}
};

class standard_state_instance_button : public button_element_base {
public:
	virtual int32_t get_icon_frame(sys::state& state, dcon::state_instance_id state_instance_id) noexcept {
		return 0;
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			auto state_instance_id = any_cast<dcon::state_instance_id>(payload);
			frame = get_icon_frame(state, state_instance_id);
		}
	}
};

class standard_nation_button : public button_element_base {
public:
	virtual int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept {
		return 0;
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);
			frame = get_icon_frame(state, nation_id);
		}
	}
};

class nation_flag_frame : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto status = nations::get_status(state, nation_id);
		return std::min(3, int32_t(status));
	}
};

class nation_national_value_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto nat_val = state.world.nation_get_national_value(nation_id);
		return nat_val.get_icon();
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto fat_id = dcon::fatten(state.world, nation_id);
			auto name = fat_id.get_name();
			if(bool(name)) {
				auto box = text::open_layout_box(contents, 0);
				text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, name), text::text_color::yellow);
				text::close_layout_box(contents, box);
			}
			auto mod_id = fat_id.get_national_value().id;
			if(bool(mod_id)) {
				modifier_description(state, contents, mod_id);
			}
		}
	}
};

class nation_can_do_social_reform_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return int32_t(!nations::has_social_reform_available(state, nation_id));
	}
};

class nation_can_do_political_reform_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return int32_t(!nations::has_political_reform_available(state, nation_id));
	}
};

class nation_military_reform_multiplier_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return int32_t(politics::get_military_reform_multiplier(state, nation_id) <= 0.f);
	}
};

class nation_economic_reform_multiplier_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return int32_t(politics::get_economic_reform_multiplier(state, nation_id) <= 0.f);
	}
};

class nation_ruling_party_ideology_plupp : public tinted_image_element_base {
public:
	uint32_t get_tint_color(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);
			auto ruling_party = state.world.nation_get_ruling_party(nation_id);
			auto ideology = state.world.political_party_get_ideology(ruling_party);
			return state.world.ideology_get_color(ideology);
		}
		return 0;
	}
};

class nation_player_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, state.local_player_nation);
		return fat_id.get_identity_from_identity_holder();
	}
};

class nation_gp_flag : public flag_button {
public:
	uint16_t rank = 0;
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		const auto nat_id = nations::get_nth_great_power(state, rank);
		if(!bool(nat_id))
			return dcon::national_identity_id{};
		auto fat_id = dcon::fatten(state.world, nat_id);
		return fat_id.get_identity_from_identity_holder();
	}
};

class ideology_plupp : public tinted_image_element_base {
public:
	uint32_t get_tint_color(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::ideology_id{};
			parent->impl_get(state, payload);
			auto ideology_id = any_cast<dcon::ideology_id>(payload);
			return state.world.ideology_get_color(ideology_id);
		}
		return 0;
	}
};

class fixed_pop_type_icon : public opaque_element_base {
public:
	dcon::pop_type_id type{};

	void set_type(sys::state& state, dcon::pop_type_id t) {
		type = t;
		frame = int32_t(state.world.pop_type_get_sprite(t) - 1);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto name = state.world.pop_type_get_name(type);
		if(bool(name)) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(contents, state, box, name);
			text::close_layout_box(contents, box);
		}
	}
};

class pop_type_icon : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::pop_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::pop_type_id>(payload);

			auto fat_id = dcon::fatten(state.world, content);
			frame = int32_t(fat_id.get_sprite() - 1);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::pop_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::pop_type_id>(payload);

			auto name = state.world.pop_type_get_name(content);
			if(bool(name)) {
				auto box = text::open_layout_box(contents, 0);
				text::add_to_layout_box(contents, state, box, name);
				text::close_layout_box(contents, box);
			}
		}
	}
};
class religion_type_icon : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::religion_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::religion_id>(payload);

			auto fat_id = dcon::fatten(state.world, content);
			frame = int32_t(fat_id.get_icon() - 1);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::religion_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::religion_id>(payload);

			auto name = state.world.religion_get_name(content);
			if(bool(name)) {
				auto box = text::open_layout_box(contents, 0);
				text::add_to_layout_box(contents, state, box, name);
				text::close_layout_box(contents, box);
			}
		}
	}
};

class nation_ideology_percentage_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			Cyto::Any i_payload = dcon::ideology_id{};
			parent->impl_get(state, i_payload);
			auto ideology_id = any_cast<dcon::ideology_id>(i_payload);
			if(nation_id && ideology_id) {
				auto percentage = .01f * state.world.nation_get_upper_house(nation_id, ideology_id);
				set_text(state, text::format_percentage(percentage, 1));
			}
		}
	}
};

class upper_house_piechart : public piechart<dcon::ideology_id> {
protected:
	std::unordered_map<dcon::ideology_id::value_base_t, float> get_distribution(sys::state& state) noexcept override {
		std::unordered_map<dcon::ideology_id::value_base_t, float> distrib = {};
		Cyto::Any nat_id_payload = dcon::nation_id{};
		if(parent) {
			parent->impl_get(state, nat_id_payload);
			if(nat_id_payload.holds_type<dcon::nation_id>()) {
				auto nat_id = any_cast<dcon::nation_id>(nat_id_payload);
				state.world.for_each_ideology([&](dcon::ideology_id ideo_id) {
					auto weight = .01f * state.world.nation_get_upper_house(nat_id, ideo_id);
					distrib[dcon::ideology_id::value_base_t(ideo_id.index())] = weight;
				});
			}
		}
		return distrib;
	}
};

class voter_ideology_piechart : public piechart<dcon::ideology_id> {
protected:
	std::unordered_map<dcon::ideology_id::value_base_t, float> get_distribution(sys::state& state) noexcept override {
		std::unordered_map<dcon::ideology_id::value_base_t, float> distrib = {};
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
					distrib[dcon::ideology_id::value_base_t(i)] = ideo_pool[i] / total;
				}
			}
		}
		return distrib;
	}
};

class commodity_factory_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::commodity_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::commodity_id>(payload);
			frame = int32_t(state.world.commodity_get_icon(content));
		}
	}
};

class commodity_price_text : public generic_simple_text<dcon::commodity_id> {
public:
	std::string get_text(sys::state& state, dcon::commodity_id commodity_id) noexcept override {
		return text::format_money(state.world.commodity_get_current_price(commodity_id));
	}
};

class commodity_player_availability_text : public generic_simple_text<dcon::commodity_id> {
public:
	std::string get_text(sys::state& state, dcon::commodity_id commodity_id) noexcept override {
		if(commodity_id)
			return text::format_money(state.world.nation_get_demand_satisfaction(state.local_player_nation, commodity_id));
		else
			return "";
	}
};

class commodity_player_real_demand_text : public generic_simple_text<dcon::commodity_id> {
public:
	std::string get_text(sys::state& state, dcon::commodity_id commodity_id) noexcept override {
		if(commodity_id)
			return text::format_float(state.world.nation_get_real_demand(state.local_player_nation, commodity_id), 1);
		else
			return "";
	}
};

class commodity_player_production_text : public generic_simple_text<dcon::commodity_id> {
public:
	std::string get_text(sys::state& state, dcon::commodity_id commodity_id) noexcept override {
		if(commodity_id)
			return text::format_float(state.world.nation_get_domestic_market_pool(state.local_player_nation, commodity_id), 1);
		else
			return "";
	}
};

class commodity_national_player_stockpile_text : public generic_simple_text<dcon::commodity_id> {
public:
	std::string get_text(sys::state& state, dcon::commodity_id commodity_id) noexcept override {
		if(commodity_id) {
			float stockpile = state.world.nation_get_stockpiles(state.local_player_nation, commodity_id);
			return text::format_float(stockpile, 2);
		} else {
			return "";
		}
	}
};

class commodity_player_stockpile_increase_text : public generic_simple_text<dcon::commodity_id> {
public:
	std::string get_text(sys::state& state, dcon::commodity_id commodity_id) noexcept override {
		float amount = economy::stockpile_commodity_daily_increase(state, commodity_id, state.local_player_nation);
		return std::string(amount >= 0.f ? "+" : "") + text::format_float(amount, 2);
	}
};

class commodity_market_increase_text : public generic_simple_text<dcon::commodity_id> {
public:
	std::string get_text(sys::state& state, dcon::commodity_id commodity_id) noexcept override {
		float amount = economy::global_market_commodity_daily_increase(state, commodity_id);
		return std::string("(") + text::format_float(amount, 0) + ")";
	}
};

class commodity_global_market_text : public generic_simple_text<dcon::commodity_id> {
public:
	std::string get_text(sys::state& state, dcon::commodity_id commodity_id) noexcept override {
		float amount = state.world.commodity_get_global_market_pool(commodity_id);
		return text::format_float(amount, 2);
	}
};

class commodity_player_domestic_market_text : public generic_simple_text<dcon::commodity_id> {
public:
	std::string get_text(sys::state& state, dcon::commodity_id commodity_id) noexcept override {
		float amount = commodity_id ? state.world.nation_get_domestic_market_pool(state.local_player_nation, commodity_id) : 0.0f;
		return text::format_float(amount, 1);
	}
};

class commodity_player_factory_needs_text : public generic_simple_text<dcon::commodity_id> {
public:
	std::string get_text(sys::state& state, dcon::commodity_id commodity_id) noexcept override {
		auto amount = commodity_id ? economy::nation_factory_consumption(state, state.local_player_nation, commodity_id) : 0.0f;
		return text::format_float(amount, 3);
	}
};

class commodity_player_pop_needs_text : public generic_simple_text<dcon::commodity_id> {
public:
	std::string get_text(sys::state& state, dcon::commodity_id commodity_id) noexcept override {
		auto amount = commodity_id ? economy::nation_pop_consumption(state, state.local_player_nation, commodity_id) : 0.0f;
		return text::format_float(amount, 3);
	}
};

class commodity_player_government_needs_text : public generic_simple_text<dcon::commodity_id> {
public:
	std::string get_text(sys::state& state, dcon::commodity_id commodity_id) noexcept override {
		auto amount = commodity_id ? economy::government_consumption(state, state.local_player_nation, commodity_id) : 0.0f;
		return text::format_float(amount, 3);
	}
};

class standard_factory_text : public simple_text_element_base {
public:
	virtual std::string get_text(sys::state& state, dcon::factory_id factory_id) noexcept {
		return "";
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::factory_id>(payload);
			set_text(state, get_text(state, content));
		}
	}
};
class factory_state_name_text : public standard_factory_text {
public:
	std::string get_text(sys::state& state, dcon::factory_id factory_id) noexcept override {
		auto flid = state.world.factory_get_factory_location_as_factory(factory_id);
		auto pid = state.world.factory_location_get_province(flid);
		auto sdef = state.world.province_get_state_from_abstract_state_membership(pid);
		dcon::state_instance_id sid{};
		state.world.for_each_state_instance([&](dcon::state_instance_id id) {
			if(state.world.state_instance_get_definition(id) == sdef)
				sid = id;
		});
		return text::get_dynamic_state_name(state, sid);
	}
};
class factory_output_name_text : public standard_factory_text {
public:
	std::string get_text(sys::state& state, dcon::factory_id factory_id) noexcept override {
		auto cid = state.world.factory_get_building_type(factory_id).get_output();
		return text::get_name_as_string(state, cid);
	}
};
class factory_produced_text : public standard_factory_text {
public:
	std::string get_text(sys::state& state, dcon::factory_id factory_id) noexcept override {
		return text::format_float(state.world.factory_get_actual_production(factory_id), 2);
	}
};
class factory_income_text : public standard_factory_text {
public:
	std::string get_text(sys::state& state, dcon::factory_id factory_id) noexcept override {
		return text::format_float(state.world.factory_get_full_profit(factory_id), 2);
	}
};
class factory_workers_text : public standard_factory_text {
public:
	std::string get_text(sys::state& state, dcon::factory_id factory_id) noexcept override {
		return text::format_float(economy::factory_total_employment(state, factory_id), 2);
	}
};
class factory_level_text : public standard_factory_text {
public:
	std::string get_text(sys::state& state, dcon::factory_id factory_id) noexcept override {
		return std::to_string(uint32_t(state.world.factory_get_level(factory_id)));
	}
};
class factory_profit_text : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::factory_id>(payload);

			auto profit = state.world.factory_get_full_profit(content);
			bool is_positive = profit >= 0.f;
			auto text = (is_positive ? "+" : "") + text::format_float(profit, 2);
			// Create colour
			auto contents = text::create_endless_layout(
			    internal_layout,
			    text::layout_parameters{0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::left, text::text_color::black});
			auto box = text::open_layout_box(contents);
			text::add_to_layout_box(contents, state, box, text, is_positive ? text::text_color::dark_green : text::text_color::dark_red);
			text::close_layout_box(contents, box);
		}
	}
};
class factory_priority_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::factory_id>(payload);
			frame = economy::factory_priority(state, content);
		}
	}
};

// -----------------------------------------------------------------------------
// National focuses
class national_focus_icon : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::national_focus_id{};
			parent->impl_get(state, payload);
			auto nfid = any_cast<dcon::national_focus_id>(payload);

			Cyto::Any s_payload = dcon::state_instance_id{};
			parent->impl_get(state, s_payload);
			auto sid = any_cast<dcon::state_instance_id>(s_payload);

			disabled = !command::can_set_national_focus(state, state.local_player_nation, sid, nfid);
			frame = state.world.national_focus_get_icon(nfid) - 1;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::national_focus_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::national_focus_id>(payload);
			if(bool(content)) {
				auto box = text::open_layout_box(contents, 0);
				text::add_to_layout_box(contents, state, box, state.world.national_focus_get_name(content), text::substitution_map{});
				text::close_layout_box(contents, box);
			}
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::state_instance_id>(payload);

			Cyto::Any nf_payload = dcon::national_focus_id{};
			parent->impl_get(state, nf_payload);
			auto nat_focus = any_cast<dcon::national_focus_id>(nf_payload);
			command::set_national_focus(state, state.local_player_nation, content, nat_focus);
		}
	}
};

class national_focus_item : public listbox_row_element_base<dcon::national_focus_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "focus_icon") {
			return make_element_by_type<national_focus_icon>(state, id);
		} else {
			return nullptr;
		}
	}
};

class national_focus_category_list : public overlapping_listbox_element_base<national_focus_item, dcon::national_focus_id> {
public:
	std::string_view get_row_element_name() override {
		return "focus_item";
	}
};

class national_focus_category : public window_element_base {
private:
	simple_text_element_base* category_label = nullptr;
	national_focus_category_list* focus_list = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			category_label = ptr.get();
			return ptr;
		} else if(name == "focus_icons") {
			auto ptr = make_element_by_type<national_focus_category_list>(state, id);
			focus_list = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<nations::focus_type>()) {
			auto category = any_cast<nations::focus_type>(payload);
			category_label->set_text(state, text::get_focus_category_name(state, category));

			focus_list->row_contents.clear();
			state.world.for_each_national_focus([&](dcon::national_focus_id focus_id) {
				auto fat_id = dcon::fatten(state.world, focus_id);
				if(fat_id.get_type() == uint8_t(category))
					focus_list->row_contents.push_back(focus_id);
			});
			focus_list->update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class national_focus_remove_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::state_instance_id>(payload);
			command::set_national_focus(state, state.local_player_nation, content, dcon::national_focus_id{});
		}
	}
};

class national_focus_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		auto start = make_element_by_type<window_element_base>(state, "focuscategory_start");
		auto current_pos = start->base_data.position;
		auto step = make_element_by_type<window_element_base>(state, "focuscategory_step");
		auto step_y = step->base_data.position.y;

		for(uint8_t i = 1; i <= uint8_t(nations::focus_type::party_loyalty_focus); i++) {
			auto ptr = make_element_by_type<national_focus_category>(state, "focus_category");
			ptr->base_data.position = current_pos;
			current_pos = xy_pair{current_pos.x, int16_t(current_pos.y + step_y)};

			Cyto::Any foc_type_payload = nations::focus_type(i);
			ptr->impl_set(state, foc_type_payload);

			add_child_to_front(std::move(ptr));
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "background") {
			return make_element_by_type<draggable_target>(state, id);
		} else if(name == "cancel_button") {
			return make_element_by_type<national_focus_remove_button>(state, id);
		} else {
			return nullptr;
		}
	}
};
// -----------------------------------------------------------------------------

class war_name_text : public generic_multiline_text<dcon::war_id> {
	void populate_layout(sys::state& state, text::endless_layout& contents, dcon::war_id id) noexcept override {
		auto war = dcon::fatten(state.world, id);
		dcon::nation_id primary_attacker = state.world.war_get_primary_attacker(war);
		dcon::nation_id primary_defender = state.world.war_get_primary_defender(war);

		for(auto wg : state.world.war_get_wargoals_attached(war)) {
			if(wg.get_wargoal().get_added_by() == primary_attacker && wg.get_wargoal().get_target_nation() == primary_defender) {
				auto box = text::open_layout_box(contents);
				text::substitution_map sub{};
				auto pa_adj = state.world.nation_get_adjective(primary_attacker);
				text::add_to_substitution_map(sub, text::variable_type::first, pa_adj);
				auto sdef = wg.get_wargoal().get_associated_state();
				auto bits = state.world.cb_type_get_type_bits(wg.get_wargoal().get_type());
				if(dcon::fatten(state.world, sdef).is_valid()) {
					auto name = state.world.state_definition_get_name(sdef);
					text::add_to_substitution_map(sub, text::variable_type::second, name);
				} else if((bits & (military::cb_flag::po_annex | military::cb_flag::po_make_puppet | military::cb_flag::po_gunboat)) != 0) {
					text::add_to_substitution_map(sub, text::variable_type::second, primary_defender);
				} else if((bits & (military::cb_flag::po_liberate | military::cb_flag::po_take_from_sphere)) != 0) {
					auto niid = wg.get_wargoal().get_associated_tag();
					auto adj = state.world.national_identity_get_adjective(niid);
					text::add_to_substitution_map(sub, text::variable_type::second, adj);
				} else {
					auto adj = state.world.nation_get_adjective(primary_defender);
					text::add_to_substitution_map(sub, text::variable_type::second, adj);
				}

				// TODO: ordinal numbering, 1st, 2nd, 3rd, 4th, etc...
				text::add_to_substitution_map(sub, text::variable_type::order, std::string_view(""));

				// First, start by using the base name of the wargoal
				std::string full_key = "normal_" + text::produce_simple_string(state, state.world.cb_type_get_war_name(wg.get_wargoal().get_type()));
				if(auto k1 = state.key_to_text_sequence.find(std::string_view(full_key.c_str())); k1 != state.key_to_text_sequence.end()) {
					// Check if an specialization exists, for example:
					// normal_take_name_eng_tur would apply if ENG is vs. TUR
					auto att_tag = nations::int_to_tag(state.world.national_identity_get_identifying_int(state.world.nation_get_identity_from_identity_holder(primary_attacker)));
					std::transform(att_tag.begin(), att_tag.end(), att_tag.begin(), [](auto c) {
						return char(tolower(char(c)));
					});
					full_key += "_" + att_tag;
					if(auto k2 = state.key_to_text_sequence.find(std::string_view(full_key.c_str())); k2 != state.key_to_text_sequence.end()) {
						auto def_tag = nations::int_to_tag(state.world.national_identity_get_identifying_int(state.world.nation_get_identity_from_identity_holder(primary_defender)));
						std::transform(def_tag.begin(), def_tag.end(), def_tag.begin(), [](auto c) {
							return char(tolower(char(c)));
						});
						full_key += "_" + def_tag;
						if(auto k3 = state.key_to_text_sequence.find(std::string_view(full_key.c_str())); k3 != state.key_to_text_sequence.end()) {
							text::add_to_layout_box(contents, state, box, k3->second, sub);
						} else {
							text::add_to_layout_box(contents, state, box, k2->second, sub);
						}
					} else {
						text::add_to_layout_box(contents, state, box, k1->second, sub);
					}
				}
				text::close_layout_box(contents, box);
				break;
			}
		}
	}
};

} // namespace ui
