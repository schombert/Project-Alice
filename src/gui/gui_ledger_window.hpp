#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"

namespace ui {

class ledger_page_number {
public:
	int8_t value;
};

enum class ledger_sort_type {
	country_name,
	country_status,
	military_score,
	industrial_score,
	prestige,
	total_score,
	total_pop,
	provinces,
	factories,
	leadership,
	literacy,
	brigades,
	ships,
	government_type,
	ruling_party,
	ruling_ideology,
	national_value,
	militancy,
	conciousness,
	religion,
	culture,
	dom_issue,
	dom_ideology,
	province_name,
	state_name,
	commodity_type,
	output_amount,
	profit,
	employment,
	rgo_size,
	factory_level
};

struct ledger_sort {
	std::variant<std::monostate, ledger_sort_type, dcon::issue_id, dcon::pop_type_id> type = std::monostate{};
	bool reversed = false;
};

class ledger_generic_sort_button : public button_element_base {
public:
	std::variant<std::monostate, ledger_sort_type, dcon::issue_id, dcon::pop_type_id> type;

	ledger_generic_sort_button(std::variant<std::monostate, ledger_sort_type, dcon::issue_id, dcon::pop_type_id> type) : type(type) { }

	void button_action(sys::state& state) noexcept override {
		element_selection_wrapper<ledger_sort> current_sort;
		current_sort.data = retrieve< ledger_sort>(state, parent);
		if(current_sort.data.type == type) {
			current_sort.data.reversed = !current_sort.data.reversed;
		} else {
			current_sort.data.type = type;
			current_sort.data.reversed = false;
		}
		send(state, parent, current_sort);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return std::holds_alternative<dcon::pop_type_id>(type) ? tooltip_behavior::tooltip : tooltip_behavior::no_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		if(std::holds_alternative<dcon::pop_type_id>(type))
			text::add_line(state, contents, state.world.pop_type_get_name(std::get<dcon::pop_type_id>(type)));
	}
};

class ledger_prev_button : public generic_settable_element<button_element_base, ledger_page_number> {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			auto num = int8_t(content.value - 1);
			if(num <= 0)
				num = 11;
			Cyto::Any new_payload = ledger_page_number{num};
			parent->impl_set(state, new_payload);
		}
	}
};

class ledger_next_button : public generic_settable_element<button_element_base, ledger_page_number> {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			auto num = int8_t(content.value + 1);
			if(num > 11)
				num = 1;
			Cyto::Any new_payload = ledger_page_number{num};
			parent->impl_set(state, new_payload);
		}
	}
};

//
// Nation ranking
//

class ledger_nation_ranking_buttons : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		auto button_def = state.ui_state.defs_by_name.find("ledger_default_button")->second.definition;

		xy_pair cell_offset{ int16_t(24), 0 };
		auto cell_width = (972 - cell_offset.x) / 6;
		auto apply_offset = [&](auto& ptr) {
			ptr->base_data.position = cell_offset;
			ptr->base_data.size.x = int16_t(cell_width);
			cell_offset.x += ptr->base_data.size.x;
			};

		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::country_name);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_name"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::country_status);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_status"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::military_score);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_milscore"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::industrial_score);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_indscore"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::prestige);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_prestige"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::total_score);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_totalscore"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
	}
};

class ledger_nation_ranking_entry : public listbox_row_element_base<dcon::nation_id> {
	flag_button* country_flag = nullptr;

public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base::on_create(state);
		// Country flag
		{
			auto ptr =
					make_element_by_type<flag_button>(state, state.ui_state.defs_by_name.find("ledger_default_flag")->second.definition);
			country_flag = ptr.get();
			add_child_to_front(std::move(ptr));
		}
		xy_pair cell_offset{int16_t(country_flag->base_data.position.x + country_flag->base_data.size.x), 0};
		auto cell_width = (972 - cell_offset.x) / 6;
		auto apply_offset = [&](auto& ptr) {
			ptr->base_data.position = cell_offset;
			ptr->base_data.size.x = int16_t(cell_width);
			cell_offset.x += ptr->base_data.size.x;
		};
		// Country name
		{
			auto ptr = make_element_by_type<generic_name_text<dcon::nation_id>>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Country status
		{
			auto ptr = make_element_by_type<nation_status_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Military score
		{
			auto ptr = make_element_by_type<nation_military_score_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Industrial score
		{
			auto ptr = make_element_by_type<nation_industry_score_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Prestige
		{
			auto ptr = make_element_by_type<nation_prestige_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Total score
		{
			auto ptr = make_element_by_type<nation_total_score_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(content);
			return message_result::consumed;
		}
		return listbox_row_element_base::get(state, payload);
	}

	void on_update(sys::state& state) noexcept override {
		Cyto::Any payload = content;
		impl_set(state, payload);
	}
};
class ledger_nation_ranking_listbox : public listbox_element_base<ledger_nation_ranking_entry, dcon::nation_id> {
protected:
	std::string_view get_row_element_name() override {
		return "default_listbox_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_nation([&](dcon::nation_id id) {
			if(state.world.nation_get_owned_province_count(id) != 0)
				row_contents.push_back(id);
		});
		auto lsort = retrieve<ledger_sort>(state, parent);
		ledger_sort_type st = std::holds_alternative<ledger_sort_type>(lsort.type) ? std::get<ledger_sort_type>(lsort.type) : ledger_sort_type::country_name;
		switch(st) {
			case ledger_sort_type::country_status:
				std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
					if(lsort.reversed) {
						return int32_t(nations::get_status(state, a)) >  int32_t(nations::get_status(state, b));
					} else {
						return int32_t(nations::get_status(state, a)) < int32_t(nations::get_status(state, b));
					}
				});
				break;
			case ledger_sort_type::military_score:
				std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
					if(lsort.reversed) {
						return state.world.nation_get_military_score(a) < state.world.nation_get_military_score(b);
					} else {
						return state.world.nation_get_military_score(a) > state.world.nation_get_military_score(b);
					}
				});
				break;
			case ledger_sort_type::industrial_score:
				std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
					if(lsort.reversed) {
						return state.world.nation_get_industrial_score(a) < state.world.nation_get_industrial_score(b);
					} else {
						return state.world.nation_get_industrial_score(a) > state.world.nation_get_industrial_score(b);
					}
				});
				break;
			case ledger_sort_type::prestige:
				std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
					if(lsort.reversed) {
						return nations::prestige_score(state, a) < nations::prestige_score(state, b);
					} else {
						return nations::prestige_score(state, a) > nations::prestige_score(state, b);
					}
				});
				break;
			case ledger_sort_type::total_score:
				std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
					if(lsort.reversed) {
						return state.world.nation_get_military_score(a) + state.world.nation_get_industrial_score(a) + nations::prestige_score(state, a) < state.world.nation_get_military_score(b) + state.world.nation_get_industrial_score(b) + nations::prestige_score(state, b);
					} else {
						return state.world.nation_get_military_score(a) + state.world.nation_get_industrial_score(a) + nations::prestige_score(state, a) > state.world.nation_get_military_score(b) + state.world.nation_get_industrial_score(b) + nations::prestige_score(state, b);
					}
				});
				break;
			default:
				std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
					if(lsort.reversed) {
						return text::produce_simple_string(state, state.world.nation_get_name(a)) > text::produce_simple_string(state, state.world.nation_get_name(b));
					} else {
						return text::produce_simple_string(state, state.world.nation_get_name(a)) < text::produce_simple_string(state, state.world.nation_get_name(b));
					}
				});
				break;
		}
		update(state);
	}
};

//
// Nation comparison
//

class ledger_nation_comparison_buttons : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		auto button_def = state.ui_state.defs_by_name.find("ledger_default_button")->second.definition;

		xy_pair cell_offset{ int16_t(24), 0 };
		auto cell_width = (972 - cell_offset.x) / 8;
		auto apply_offset = [&](auto& ptr) {
			ptr->base_data.position = cell_offset;
			ptr->base_data.size.x = int16_t(cell_width);
			cell_offset.x += ptr->base_data.size.x;
		};

		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::country_name);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_name"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::total_pop);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_total_pop"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::provinces);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_provcount"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::factories);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_factorycount"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::literacy);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_literacy"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::leadership);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_leadership"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::brigades);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_divisions"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::ships);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_ships"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
	}
};

class ledger_nation_comparison_entry : public listbox_row_element_base<dcon::nation_id> {
	flag_button* country_flag = nullptr;

public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base::on_create(state);
		// Country flag
		{
			auto ptr =
					make_element_by_type<flag_button>(state, state.ui_state.defs_by_name.find("ledger_default_flag")->second.definition);
			country_flag = ptr.get();
			add_child_to_front(std::move(ptr));
		}
		xy_pair cell_offset{int16_t(country_flag->base_data.position.x + country_flag->base_data.size.x), 0};
		auto cell_width = (972 - cell_offset.x) / 8;
		auto apply_offset = [&](auto& ptr) {
			ptr->base_data.position = cell_offset;
			ptr->base_data.size.x = int16_t(cell_width);
			cell_offset.x += ptr->base_data.size.x;
		};
		// Country name
		{
			auto ptr = make_element_by_type<generic_name_text<dcon::nation_id>>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Total population
		{
			auto ptr = make_element_by_type<nation_population_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Provinces count
		{
			auto ptr = make_element_by_type<nation_provinces_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Factories count
		{
			auto ptr = make_element_by_type<nation_industries_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Literacy
		{
			auto ptr = make_element_by_type<nation_literacy_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Leadership
		{
			auto ptr = make_element_by_type<nation_leadership_points_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Brigades
		{
			auto ptr = make_element_by_type<nation_num_regiments>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Ships
		{
			auto ptr = make_element_by_type<nation_num_ships>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(content);
			return message_result::consumed;
		}
		return listbox_row_element_base::get(state, payload);
	}
};
class ledger_nation_comparison_listbox : public listbox_element_base<ledger_nation_comparison_entry, dcon::nation_id> {
protected:
	std::string_view get_row_element_name() override {
		return "default_listbox_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_nation([&](dcon::nation_id id) {
			if(state.world.nation_get_owned_province_count(id) != 0)
				row_contents.push_back(id);
		});

		auto lsort = retrieve<ledger_sort>(state, parent);
		ledger_sort_type st = std::holds_alternative<ledger_sort_type>(lsort.type) ? std::get<ledger_sort_type>(lsort.type) : ledger_sort_type::country_name;
		switch(st) {
		case ledger_sort_type::total_pop:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				if(lsort.reversed) {
					return state.world.nation_get_demographics(a, demographics::total) < state.world.nation_get_demographics(b, demographics::total);
				} else {
					return state.world.nation_get_demographics(a, demographics::total) > state.world.nation_get_demographics(b, demographics::total);
				}
			});
			break;
		case ledger_sort_type::provinces:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				if(lsort.reversed) {
					return state.world.nation_get_owned_province_count(a) < state.world.nation_get_owned_province_count(b);
				} else {
					return state.world.nation_get_owned_province_count(a) > state.world.nation_get_owned_province_count(b);
				}
			});
			break;
		case ledger_sort_type::factories:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				uint32_t anum_factories = 0;
				for(auto si : state.world.nation_get_state_ownership(a)) {
					province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id p) {
						anum_factories += uint32_t(state.world.province_get_factory_location(p).end() - state.world.province_get_factory_location(p).begin());
					});
				}
				uint32_t bnum_factories = 0;
				for(auto si : state.world.nation_get_state_ownership(b)) {
					province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id p) {
						bnum_factories += uint32_t(state.world.province_get_factory_location(p).end() - state.world.province_get_factory_location(p).begin());
					});
				}
				if(lsort.reversed) {
					return anum_factories < bnum_factories;
				} else {
					return anum_factories > bnum_factories;
				}
			});
			break;
		case ledger_sort_type::literacy:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				auto aliteracy = state.world.nation_get_demographics(a, demographics::literacy) / std::max(1.0f, state.world.nation_get_demographics(a, demographics::total));
				auto bliteracy = state.world.nation_get_demographics(b, demographics::literacy) / std::max(1.0f, state.world.nation_get_demographics(b, demographics::total));
				if(lsort.reversed) {
					return aliteracy < bliteracy;
				} else {
					return aliteracy > bliteracy;
				}
			});
			break;
		case ledger_sort_type::leadership:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				if(lsort.reversed) {
					return state.world.nation_get_leadership_points(a) < state.world.nation_get_leadership_points(b);
				} else {
					return state.world.nation_get_leadership_points(a) > state.world.nation_get_leadership_points(b);
				}
			});
			break;
		case ledger_sort_type::brigades:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				if(lsort.reversed) {
					return state.world.nation_get_active_regiments(a) < state.world.nation_get_active_regiments(b);
				} else {
					return state.world.nation_get_active_regiments(a) > state.world.nation_get_active_regiments(b);
				}
			});
			break;
		case ledger_sort_type::ships:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				if(lsort.reversed) {
					return military::total_ships(state, a) < military::total_ships(state, b);
				} else {
					return military::total_ships(state, a) > military::total_ships(state, b);
				}
			});
			break;
		default:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				if(lsort.reversed) {
					return text::produce_simple_string(state, state.world.nation_get_name(a)) > text::produce_simple_string(state, state.world.nation_get_name(b));
				} else {
					return text::produce_simple_string(state, state.world.nation_get_name(a)) < text::produce_simple_string(state, state.world.nation_get_name(b));
				}
			});
			break;
		}

		update(state);
	}
};

//
// Nation political systems
//

class ledger_nation_political_systems_buttons : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		auto button_def = state.ui_state.defs_by_name.find("ledger_default_button")->second.definition;

		xy_pair cell_offset{ int16_t(24), 0 };
		auto cell_width = (972 - cell_offset.x) / 5;
		auto apply_offset = [&](auto& ptr) {
			ptr->base_data.position = cell_offset;
			ptr->base_data.size.x = int16_t(cell_width);
			cell_offset.x += ptr->base_data.size.x;
			};

		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::country_name);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_name"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::government_type);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_government"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::national_value);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_nationalvalue"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::ruling_party);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_rulingparty"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::ruling_ideology);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_rulingideology"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
	}
};

class ledger_nation_political_systems_entry : public listbox_row_element_base<dcon::nation_id> {
	flag_button* country_flag = nullptr;

public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base::on_create(state);
		// Country flag
		{
			auto ptr =
					make_element_by_type<flag_button>(state, state.ui_state.defs_by_name.find("ledger_default_flag")->second.definition);
			country_flag = ptr.get();
			add_child_to_front(std::move(ptr));
		}
		xy_pair cell_offset{int16_t(country_flag->base_data.position.x + country_flag->base_data.size.x), 0};
		auto cell_width = (972 - cell_offset.x) / 5;
		auto apply_offset = [&](auto& ptr) {
			ptr->base_data.position = cell_offset;
			ptr->base_data.size.x = int16_t(cell_width);
			cell_offset.x += ptr->base_data.size.x;
		};
		// Country name
		{
			auto ptr = make_element_by_type<generic_name_text<dcon::nation_id>>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Government
		{
			auto ptr = make_element_by_type<nation_government_type_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// National value
		{
			auto ptr = make_element_by_type<nation_national_value_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Ruling party
		{
			auto ptr = make_element_by_type<nation_ruling_party_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Party ideology
		{
			auto ptr = make_element_by_type<nation_ruling_party_ideology_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(content);
			return message_result::consumed;
		}
		return listbox_row_element_base::get(state, payload);
	}
};
class ledger_nation_political_systems_listbox
		: public listbox_element_base<ledger_nation_political_systems_entry, dcon::nation_id> {
protected:
	std::string_view get_row_element_name() override {
		return "default_listbox_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_nation([&](dcon::nation_id id) {
			if(state.world.nation_get_owned_province_count(id) != 0)
				row_contents.push_back(id);
		});

		auto lsort = retrieve<ledger_sort>(state, parent);
		ledger_sort_type st = std::holds_alternative<ledger_sort_type>(lsort.type) ? std::get<ledger_sort_type>(lsort.type) : ledger_sort_type::country_name;
		switch(st) {
		case ledger_sort_type::government_type:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				if(lsort.reversed) {
					return state.world.nation_get_government_type(a).id.index() < state.world.nation_get_government_type(b).id.index();
				} else {
					return state.world.nation_get_government_type(a).id.index() > state.world.nation_get_government_type(b).id.index();
				}
			});
			break;
		case ledger_sort_type::ruling_party:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				if(lsort.reversed) {
					return text::produce_simple_string(state, state.world.nation_get_ruling_party(a).get_name()) < text::produce_simple_string(state, state.world.nation_get_ruling_party(b).get_name());
				} else {
					return text::produce_simple_string(state, state.world.nation_get_ruling_party(a).get_name()) > text::produce_simple_string(state, state.world.nation_get_ruling_party(b).get_name());
				}
			});
			break;
		case ledger_sort_type::national_value:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				if(lsort.reversed) {
					return state.world.nation_get_national_value(a).id.index() < state.world.nation_get_national_value(b).id.index();
				} else {
					return state.world.nation_get_national_value(a).id.index() > state.world.nation_get_national_value(b).id.index();
				}
			});
			break;
		case ledger_sort_type::ruling_ideology:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				if(lsort.reversed) {
					return state.world.nation_get_ruling_party(a).get_ideology().id.index() < state.world.nation_get_ruling_party(b).get_ideology().id.index();
				} else {
					return state.world.nation_get_ruling_party(a).get_ideology().id.index() > state.world.nation_get_ruling_party(b).get_ideology().id.index();
				}
			});
			break;
		default:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				if(lsort.reversed) {
					return text::produce_simple_string(state, state.world.nation_get_name(a)) > text::produce_simple_string(state, state.world.nation_get_name(b));
				} else {
					return text::produce_simple_string(state, state.world.nation_get_name(a)) < text::produce_simple_string(state, state.world.nation_get_name(b));
				}
			});
			break;
		}

		update(state);
	}
};

//
// Social & Political reforms
//
class nation_selected_issue_text : public standard_nation_text {
public:
	dcon::issue_id issue_id{};
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		// Obtain the active issue option for this issue id
		auto active_issue_option = state.world.nation_get_issues(nation_id, issue_id);
		return text::produce_simple_string(state, active_issue_option.get_name());
	}
};

template<bool IsPolitical>
class ledger_nation_reforms_buttons : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		auto button_def = state.ui_state.defs_by_name.find("ledger_default_button")->second.definition;

		xy_pair cell_offset{ int16_t(24), 0 };

		auto num_issues = 0;
		state.world.for_each_issue([&](dcon::issue_id id) {
			if(state.world.issue_get_issue_type(id) == uint8_t(culture::issue_type::party) || (state.world.issue_get_issue_type(id) == uint8_t(culture::issue_type::political)) != IsPolitical)
				return;
			++num_issues;
		});

		// 1 for the country name
		auto cell_width = (972 - cell_offset.x) / (1 + num_issues);
		auto apply_offset = [&](auto& ptr) {
			ptr->base_data.position = cell_offset;
			ptr->base_data.size.x = int16_t(cell_width);
			cell_offset.x += ptr->base_data.size.x;
		};

		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::country_name);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_name"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}

		state.world.for_each_issue([&](dcon::issue_id id) {
			if(state.world.issue_get_issue_type(id) == uint8_t(culture::issue_type::party) || (state.world.issue_get_issue_type(id) == uint8_t(culture::issue_type::political)) != IsPolitical)
				return;


			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, id);
			ptr->set_button_text(state, text::produce_simple_string(state, state.world.issue_get_name(id)));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		});
	}
};

template<bool IsPolitical>
class ledger_nation_reforms_entry : public listbox_row_element_base<dcon::nation_id> {
	flag_button* country_flag = nullptr;

public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base::on_create(state);
		// Country flag
		{
			auto ptr =
					make_element_by_type<flag_button>(state, state.ui_state.defs_by_name.find("ledger_default_flag")->second.definition);
			country_flag = ptr.get();
			add_child_to_front(std::move(ptr));
		}
		xy_pair cell_offset{int16_t(country_flag->base_data.position.x + country_flag->base_data.size.x), 0};

		auto num_issues = 0;
		state.world.for_each_issue([&](dcon::issue_id id) {
			if(state.world.issue_get_issue_type(id) == uint8_t(culture::issue_type::party) || (state.world.issue_get_issue_type(id) == uint8_t(culture::issue_type::political)) != IsPolitical)
				return;
			++num_issues;
		});

		// 1 for the country name
		auto cell_width = (972 - cell_offset.x) / (1 + num_issues);
		auto apply_offset = [&](auto& ptr) {
			ptr->base_data.position = cell_offset;
			ptr->base_data.size.x = int16_t(cell_width);
			cell_offset.x += ptr->base_data.size.x;
		};
		// Country name
		{
			auto ptr = make_element_by_type<generic_name_text<dcon::nation_id>>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// For each (political) issue generate
		state.world.for_each_issue([&](dcon::issue_id id) {
			if(state.world.issue_get_issue_type(id) == uint8_t(culture::issue_type::party) || (state.world.issue_get_issue_type(id) == uint8_t(culture::issue_type::political)) != IsPolitical)
				return;

			auto ptr = make_element_by_type<nation_selected_issue_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			ptr->issue_id = id;
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		});
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(content);
			return message_result::consumed;
		}
		return listbox_row_element_base::get(state, payload);
	}
};
class ledger_nation_political_reforms_listbox : public listbox_element_base<ledger_nation_reforms_entry<true>, dcon::nation_id> {
protected:
	std::string_view get_row_element_name() override {
		return "default_listbox_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_nation([&](dcon::nation_id id) {
			if(state.world.nation_get_owned_province_count(id) != 0)
				row_contents.push_back(id);
		});

		auto lsort = retrieve<ledger_sort>(state, parent);
		if(std::holds_alternative<dcon::issue_id>(lsort.type)) {
			auto iss = std::get<dcon::issue_id>(lsort.type);
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				if(lsort.reversed) {
					return state.world.nation_get_issues(a, iss).id.index() < state.world.nation_get_issues(b, iss).id.index();
				} else {
					return state.world.nation_get_issues(a, iss).id.index() > state.world.nation_get_issues(b, iss).id.index();
				}
			});
		} else {
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				if(lsort.reversed) {
					return text::produce_simple_string(state, state.world.nation_get_name(a)) > text::produce_simple_string(state, state.world.nation_get_name(b));
				} else {
					return text::produce_simple_string(state, state.world.nation_get_name(a)) < text::produce_simple_string(state, state.world.nation_get_name(b));
				}
			});
		}

		update(state);
	}
};
class ledger_nation_social_reforms_listbox : public listbox_element_base<ledger_nation_reforms_entry<false>, dcon::nation_id> {
protected:
	std::string_view get_row_element_name() override {
		return "default_listbox_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_nation([&](dcon::nation_id id) {
			if(state.world.nation_get_owned_province_count(id) != 0)
				row_contents.push_back(id);
		});

		auto lsort = retrieve<ledger_sort>(state, parent);
		if(std::holds_alternative<dcon::issue_id>(lsort.type)) {
			auto iss = std::get<dcon::issue_id>(lsort.type);
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				if(lsort.reversed) {
					return state.world.nation_get_issues(a, iss).id.index() < state.world.nation_get_issues(b, iss).id.index();
				} else {
					return state.world.nation_get_issues(a, iss).id.index() > state.world.nation_get_issues(b, iss).id.index();
				}
			});
		} else {
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				if(lsort.reversed) {
					return text::produce_simple_string(state, state.world.nation_get_name(a)) > text::produce_simple_string(state, state.world.nation_get_name(b));
				} else {
					return text::produce_simple_string(state, state.world.nation_get_name(a)) < text::produce_simple_string(state, state.world.nation_get_name(b));
				}
			});
		}

		update(state);
	}
};

//
// Nation population per strata
//
class nation_population_per_pop_type_text : public standard_nation_text {
public:
	dcon::pop_type_id pop_type_id{};
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto total_pop = state.world.nation_get_demographics(nation_id, demographics::to_key(state, pop_type_id));
		return text::prettify(int32_t(total_pop));
	}
};

class ledger_nation_population_buttons : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		auto button_def = state.ui_state.defs_by_name.find("ledger_default_button")->second.definition;
		auto ibutton_def = state.ui_state.defs_by_name.find("ledger_pop_icon")->second.definition;

		xy_pair cell_offset{ int16_t(24), 0 };

		auto num_pop_types = 0;
		state.world.for_each_pop_type([&](dcon::pop_type_id id) { ++num_pop_types; });

		// 1 for the country name
		auto cell_width = (972 - cell_offset.x) / (1 + num_pop_types);
		auto apply_offset = [&](auto& ptr) {
			ptr->base_data.position = cell_offset;
			cell_offset.x += int16_t(cell_width);
		};

		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::country_name);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_country_name"));
			apply_offset(ptr);
			ptr->base_data.size.x = int16_t(cell_width);
			add_child_to_front(std::move(ptr));
		}


		state.world.for_each_pop_type([&](dcon::pop_type_id id) {
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, ibutton_def, id);
			ptr->frame = int32_t(state.world.pop_type_get_sprite(id) - 1);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		});
	}
};

class ledger_nation_population_entry : public listbox_row_element_base<dcon::nation_id> {
	flag_button* country_flag = nullptr;

public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base::on_create(state);
		// Country flag
		{
			auto ptr =
					make_element_by_type<flag_button>(state, state.ui_state.defs_by_name.find("ledger_default_flag")->second.definition);
			country_flag = ptr.get();
			add_child_to_front(std::move(ptr));
		}
		xy_pair cell_offset{int16_t(country_flag->base_data.position.x + country_flag->base_data.size.x), 0};

		auto num_pop_types = 0;
		state.world.for_each_pop_type([&](dcon::pop_type_id id) { ++num_pop_types; });

		// 1 for the country name
		auto cell_width = (972 - cell_offset.x) / (1 + num_pop_types);
		auto apply_offset = [&](auto& ptr) {
			ptr->base_data.position = cell_offset;
			ptr->base_data.size.x = int16_t(cell_width);
			cell_offset.x += ptr->base_data.size.x;
		};
		// Country name
		{
			auto ptr = make_element_by_type<generic_name_text<dcon::nation_id>>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// For each pop type generate
		state.world.for_each_pop_type([&](dcon::pop_type_id id) {
			auto ptr = make_element_by_type<nation_population_per_pop_type_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			ptr->pop_type_id = id;
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		});
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(content);
			return message_result::consumed;
		}
		return listbox_row_element_base::get(state, payload);
	}
};
class ledger_nation_population_listbox : public listbox_element_base<ledger_nation_population_entry, dcon::nation_id> {
protected:
	std::string_view get_row_element_name() override {
		return "default_listbox_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_nation([&](dcon::nation_id id) {
			if(state.world.nation_get_owned_province_count(id) != 0)
				row_contents.push_back(id);
		});

		auto lsort = retrieve<ledger_sort>(state, parent);
		if(std::holds_alternative<dcon::pop_type_id>(lsort.type)) {
			auto pt = std::get<dcon::pop_type_id>(lsort.type);
			auto dkey = demographics::to_key(state, pt);
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				if(lsort.reversed) {
					return state.world.nation_get_demographics(a, dkey) < state.world.nation_get_demographics(b, dkey);
				} else {
					return state.world.nation_get_demographics(a, dkey) > state.world.nation_get_demographics(b, dkey);
				}
			});
		} else {
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				if(lsort.reversed) {
					return text::produce_simple_string(state, state.world.nation_get_name(a)) > text::produce_simple_string(state, state.world.nation_get_name(b));
				} else {
					return text::produce_simple_string(state, state.world.nation_get_name(a)) < text::produce_simple_string(state, state.world.nation_get_name(b));
				}
			});
		}

		update(state);
	}
};

//
// Provinces
//

class ledger_province_buttons : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		auto button_def = state.ui_state.defs_by_name.find("ledger_default_button")->second.definition;

		xy_pair cell_offset{ 0, 0 };

		auto cell_width = (972 - cell_offset.x) / 9;
		auto apply_offset = [&](auto& ptr) {
			ptr->base_data.position = cell_offset;
			ptr->base_data.size.x = int16_t(cell_width);
			cell_offset.x += ptr->base_data.size.x;
		};

		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::province_name);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_province_name"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::total_pop);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_province_totalpop"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::militancy);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_province_avgmil"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::conciousness);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_province_avgcon"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::literacy);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_province_avglit"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::religion);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_province_religion"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::culture);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_province_culture"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::dom_issue);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_province_issue"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::dom_ideology);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_province_ideology"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
	}
};

class ledger_province_entry : public listbox_row_element_base<dcon::province_id> {
public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base::on_create(state);

		xy_pair cell_offset{0, 0};

		auto cell_width = (972 - cell_offset.x) / 9;
		auto apply_offset = [&](auto& ptr) {
			ptr->base_data.position = cell_offset;
			ptr->base_data.size.x = int16_t(cell_width);
			cell_offset.x += ptr->base_data.size.x;
		};
		// Province name
		{
			auto ptr = make_element_by_type<generic_name_text<dcon::province_id>>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Total population
		{
			auto ptr = make_element_by_type<province_population_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Average militancy
		{
			auto ptr = make_element_by_type<province_militancy_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Average consciousness
		{
			auto ptr = make_element_by_type<province_consciousness_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Average literacy
		{
			auto ptr = make_element_by_type<province_literacy_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Religion
		{
			auto ptr = make_element_by_type<province_dominant_religion_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Culture
		{
			auto ptr = make_element_by_type<province_dominant_culture_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Issues
		{
			auto ptr = make_element_by_type<province_dominant_issue_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Ideology
		{
			auto ptr = make_element_by_type<province_dominant_ideology_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(content);
			return message_result::consumed;
		}
		return listbox_row_element_base::get(state, payload);
	}
};
class ledger_province_listbox : public listbox_element_base<ledger_province_entry, dcon::province_id> {
protected:
	std::string_view get_row_element_name() override {
		return "default_listbox_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		for(auto si : state.world.nation_get_state_ownership(state.local_player_nation))
			province::for_each_province_in_state_instance(state, si.get_state(),
					[&](dcon::province_id p) { row_contents.push_back(p); });

		auto lsort = retrieve<ledger_sort>(state, parent);
		ledger_sort_type st = std::holds_alternative<ledger_sort_type>(lsort.type) ? std::get<ledger_sort_type>(lsort.type) : ledger_sort_type::country_name;
		switch(st) {
		case ledger_sort_type::total_pop:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::province_id a, dcon::province_id b) {
				if(lsort.reversed) {
					return state.world.province_get_demographics(a, demographics::total) < state.world.province_get_demographics(b, demographics::total);
				} else {
					return state.world.province_get_demographics(a, demographics::total) > state.world.province_get_demographics(b, demographics::total);
				}
			});
			break;
		case ledger_sort_type::militancy:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::province_id a, dcon::province_id b) {
				if(lsort.reversed) {
					return state.world.province_get_demographics(a, demographics::militancy) / std::max(1.0f, state.world.province_get_demographics(a, demographics::total)) < state.world.province_get_demographics(b, demographics::militancy) / std::max(1.0f, state.world.province_get_demographics(b, demographics::total));
				} else {
					return state.world.province_get_demographics(a, demographics::militancy) / std::max(1.0f, state.world.province_get_demographics(a, demographics::total)) > state.world.province_get_demographics(b, demographics::militancy) / std::max(1.0f, state.world.province_get_demographics(b, demographics::total));
				}
			});
			break;
		case ledger_sort_type::conciousness:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::province_id a, dcon::province_id b) {
				if(lsort.reversed) {
					return state.world.province_get_demographics(a, demographics::consciousness) / std::max(1.0f, state.world.province_get_demographics(a, demographics::total)) < state.world.province_get_demographics(b, demographics::consciousness) / std::max(1.0f, state.world.province_get_demographics(b, demographics::total));
				} else {
					return state.world.province_get_demographics(a, demographics::consciousness) / std::max(1.0f, state.world.province_get_demographics(a, demographics::total)) > state.world.province_get_demographics(b, demographics::consciousness) / std::max(1.0f, state.world.province_get_demographics(b, demographics::total));
				}
			});
			break;
		case ledger_sort_type::literacy:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::province_id a, dcon::province_id b) {
				if(lsort.reversed) {
					return state.world.province_get_demographics(a, demographics::literacy) / std::max(1.0f, state.world.province_get_demographics(a, demographics::total)) < state.world.province_get_demographics(b, demographics::literacy) / std::max(1.0f, state.world.province_get_demographics(b, demographics::total));
				} else {
					return state.world.province_get_demographics(a, demographics::literacy) / std::max(1.0f, state.world.province_get_demographics(a, demographics::total)) > state.world.province_get_demographics(b, demographics::literacy) / std::max(1.0f, state.world.province_get_demographics(b, demographics::total));
				}
			});
			break;
		case ledger_sort_type::culture:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::province_id a, dcon::province_id b) {
				if(lsort.reversed) {
					return text::produce_simple_string(state, state.world.culture_get_name(state.world.province_get_dominant_culture(a))) > text::produce_simple_string(state, state.world.culture_get_name(state.world.province_get_dominant_culture(b)));
				} else {
					return text::produce_simple_string(state, state.world.culture_get_name(state.world.province_get_dominant_culture(a))) < text::produce_simple_string(state, state.world.culture_get_name(state.world.province_get_dominant_culture(b)));
				}
			});
			break;
		case ledger_sort_type::religion:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::province_id a, dcon::province_id b) {
				if(lsort.reversed) {
					return state.world.province_get_dominant_religion(a).id.index() > state.world.province_get_dominant_religion(b).id.index();
				} else {
					return state.world.province_get_dominant_religion(a).id.index() < state.world.province_get_dominant_religion(b).id.index();
				}
			});
			break;
		case ledger_sort_type::dom_ideology:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::province_id a, dcon::province_id b) {
				if(lsort.reversed) {
					return state.world.province_get_dominant_ideology(a).id.index() > state.world.province_get_dominant_ideology(b).id.index();
				} else {
					return state.world.province_get_dominant_ideology(a).id.index() < state.world.province_get_dominant_ideology(b).id.index();
				}
			});
			break;
		case ledger_sort_type::dom_issue:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::province_id a, dcon::province_id b) {
				if(lsort.reversed) {
					return state.world.province_get_dominant_issue_option(a).id.index() > state.world.province_get_dominant_issue_option(b).id.index();
				} else {
					return state.world.province_get_dominant_issue_option(a).id.index() < state.world.province_get_dominant_issue_option(b).id.index();
				}
			});
			break;
		default:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::province_id a, dcon::province_id b) {
				if(lsort.reversed) {
					return text::produce_simple_string(state, state.world.province_get_name(a)) > text::produce_simple_string(state, state.world.province_get_name(b));
				} else {
					return text::produce_simple_string(state, state.world.province_get_name(a)) < text::produce_simple_string(state, state.world.province_get_name(b));
				}
			});
			break;
		}

		update(state);
	}
};

//
// Province population per strata
//
class province_population_per_pop_type_text : public simple_text_element_base {
public:
	dcon::pop_type_id pop_type_id;

	province_population_per_pop_type_text(dcon::pop_type_id pop_type_id) : pop_type_id(pop_type_id) { }

	void on_update(sys::state& state) noexcept override {
		auto province_id = retrieve<dcon::province_id>(state, parent);
		auto total_pop = state.world.province_get_demographics(province_id, demographics::to_key(state, pop_type_id));
		set_text(state, text::prettify(int32_t(total_pop)));
	}
};

class ledger_province_population_buttons : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		auto button_def = state.ui_state.defs_by_name.find("ledger_default_button")->second.definition;
		auto ibutton_def = state.ui_state.defs_by_name.find("ledger_pop_icon")->second.definition;

		xy_pair cell_offset{ 0, 0 };

		auto num_pop_types = 0;
		state.world.for_each_pop_type([&](dcon::pop_type_id id) { ++num_pop_types; });

		// 1 for the country name
		auto cell_width = (972 - cell_offset.x) / (1 + num_pop_types);
		auto apply_offset = [&](auto& ptr) {
			ptr->base_data.position = cell_offset;
			cell_offset.x += int16_t(cell_width);
			};

		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::province_name);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_province_name"));
			ptr->base_data.size.x = int16_t(cell_width);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}


		state.world.for_each_pop_type([&](dcon::pop_type_id id) {
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, ibutton_def, id);
			ptr->frame = int32_t(state.world.pop_type_get_sprite(id) - 1);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		});
	}
};

class ledger_province_population_entry : public listbox_row_element_base<dcon::province_id> {
public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base::on_create(state);

		xy_pair cell_offset{0, 0};

		auto num_pop_types = 0;
		state.world.for_each_pop_type([&](dcon::pop_type_id id) { ++num_pop_types; });

		auto cell_width = (972 - cell_offset.x) / (1 + num_pop_types);
		auto apply_offset = [&](auto& ptr) {
			ptr->base_data.position = cell_offset;
			ptr->base_data.size.x = int16_t(cell_width);
			cell_offset.x += ptr->base_data.size.x;
		};
		// Province name
		{
			auto ptr = make_element_by_type<generic_name_text<dcon::province_id>>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// For each pop type generate
		state.world.for_each_pop_type([&](dcon::pop_type_id id) {
			auto ptr = make_element_by_type<province_population_per_pop_type_text>(state, state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition, id);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		});
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(content);
			return message_result::consumed;
		}
		return listbox_row_element_base::get(state, payload);
	}
};
class ledger_province_population_listbox : public listbox_element_base<ledger_province_population_entry, dcon::province_id> {
protected:
	std::string_view get_row_element_name() override {
		return "default_listbox_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		for(auto si : state.world.nation_get_state_ownership(state.local_player_nation))
			province::for_each_province_in_state_instance(state, si.get_state(),
					[&](dcon::province_id p) { row_contents.push_back(p); });

		auto lsort = retrieve<ledger_sort>(state, parent);
		if(std::holds_alternative<dcon::pop_type_id>(lsort.type)) {
			auto pt = std::get<dcon::pop_type_id>(lsort.type);
			auto dkey = demographics::to_key(state, pt);
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::province_id a, dcon::province_id b) {
				if(lsort.reversed) {
					return state.world.province_get_demographics(a, dkey) < state.world.province_get_demographics(b, dkey);
				} else {
					return state.world.province_get_demographics(a, dkey) > state.world.province_get_demographics(b, dkey);
				}
			});
		} else {
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::province_id a, dcon::province_id b) {
				if(lsort.reversed) {
					return text::produce_simple_string(state, state.world.province_get_name(a)) > text::produce_simple_string(state, state.world.province_get_name(b));
				} else {
					return text::produce_simple_string(state, state.world.province_get_name(a)) < text::produce_simple_string(state, state.world.province_get_name(b));
				}
			});
		}

		update(state);
	}
};

//
// Provinces production
//

class ledger_province_production_buttons : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		auto button_def = state.ui_state.defs_by_name.find("ledger_default_button")->second.definition;

		xy_pair cell_offset{ 0, 0 };

		auto cell_width = (972 - cell_offset.x) / 7;
		auto apply_offset = [&](auto& ptr) {
			ptr->base_data.position = cell_offset;
			ptr->base_data.size.x = int16_t(cell_width);
			cell_offset.x += ptr->base_data.size.x;
			};

		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::province_name);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_province_name"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::state_name);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_state_name"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::commodity_type);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_goods"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::output_amount);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_output"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::profit);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_income"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::employment);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_employed"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::rgo_size);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_level"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
	}
};

class ledger_provinces_production_entry : public listbox_row_element_base<dcon::province_id> {
public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base::on_create(state);

		xy_pair cell_offset{0, 0};

		auto cell_width = (972 - cell_offset.x) / 7;
		auto apply_offset = [&](auto& ptr) {
			ptr->base_data.position = cell_offset;
			ptr->base_data.size.x = int16_t(cell_width);
			cell_offset.x += ptr->base_data.size.x;
		};
		// Province name
		{
			auto ptr = make_element_by_type<generic_name_text<dcon::province_id>>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// State name
		{
			auto ptr = make_element_by_type<province_state_name_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Goods
		{
			auto ptr = make_element_by_type<province_rgo_name_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Output
		{
			auto ptr = make_element_by_type<province_goods_produced_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Income
		{
			auto ptr = make_element_by_type<province_income_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Employed
		{
			auto ptr = make_element_by_type<province_rgo_workers_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Level
		{
			auto ptr = make_element_by_type<province_rgo_size_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(content);
			return message_result::consumed;
		}
		return listbox_row_element_base::get(state, payload);
	}
};
class ledger_provinces_production_listbox : public listbox_element_base<ledger_provinces_production_entry, dcon::province_id> {
protected:
	std::string_view get_row_element_name() override {
		return "default_listbox_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		for(auto si : state.world.nation_get_state_ownership(state.local_player_nation))
			province::for_each_province_in_state_instance(state, si.get_state(),
					[&](dcon::province_id p) { row_contents.push_back(p); });

		auto lsort = retrieve<ledger_sort>(state, parent);
		ledger_sort_type st = std::holds_alternative<ledger_sort_type>(lsort.type) ? std::get<ledger_sort_type>(lsort.type) : ledger_sort_type::country_name;
		switch(st) {
		case ledger_sort_type::state_name:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::province_id a, dcon::province_id b) {
				if(lsort.reversed) {
					return text::get_province_state_name(state, a) > text::get_province_state_name(state, b);
				} else {
					return text::get_province_state_name(state, a) < text::get_province_state_name(state, b);
				}
			});
			break;
		case ledger_sort_type::commodity_type:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::province_id a, dcon::province_id b) {
				if(lsort.reversed) {
					return state.world.province_get_rgo(a).id.index() < state.world.province_get_rgo(b).id.index();
				} else {
					return state.world.province_get_rgo(a).id.index() > state.world.province_get_rgo(b).id.index();
				}
			});
			break;
		case ledger_sort_type::output_amount:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::province_id a, dcon::province_id b) {
				if(lsort.reversed) {
					return province::rgo_production_quantity(state, a) < province::rgo_production_quantity(state, b);
				} else {
					return province::rgo_production_quantity(state, a) > province::rgo_production_quantity(state, b);
				}
			});
			break;
		case ledger_sort_type::profit:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::province_id a, dcon::province_id b) {
				if(lsort.reversed) {
					return province::rgo_income(state, a) < province::rgo_income(state, b);
				} else {
					return province::rgo_income(state, a) > province::rgo_income(state, b);
				}
			});
			break;
		case ledger_sort_type::employment:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::province_id a, dcon::province_id b) {
				if(lsort.reversed) {
					return province::rgo_employment(state, a) < province::rgo_employment(state, b);
				} else {
					return province::rgo_employment(state, a) > province::rgo_employment(state, b);
				}
			});
			break;
		case ledger_sort_type::rgo_size:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::province_id a, dcon::province_id b) {
				if(lsort.reversed) {
					return economy::rgo_effective_size(state, state.world.province_get_nation_from_province_ownership(a), a) < economy::rgo_effective_size(state, state.world.province_get_nation_from_province_ownership(b), b);
				} else {
					return economy::rgo_effective_size(state, state.world.province_get_nation_from_province_ownership(a), a) > economy::rgo_effective_size(state, state.world.province_get_nation_from_province_ownership(b), b);
				}
			});
			break;
		default:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::province_id a, dcon::province_id b) {
				if(lsort.reversed) {
					return text::produce_simple_string(state, state.world.province_get_name(a)) > text::produce_simple_string(state, state.world.province_get_name(b));
				} else {
					return text::produce_simple_string(state, state.world.province_get_name(a)) < text::produce_simple_string(state, state.world.province_get_name(b));
				}
			});
			break;
		}

		update(state);
	}
};

//
// Factory production
//

class ledger_factories_production_buttons : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		auto button_def = state.ui_state.defs_by_name.find("ledger_default_button")->second.definition;

		xy_pair cell_offset{ 0, 0 };

		auto cell_width = (972 - cell_offset.x) / 6;
		auto apply_offset = [&](auto& ptr) {
			ptr->base_data.position = cell_offset;
			ptr->base_data.size.x = int16_t(cell_width);
			cell_offset.x += ptr->base_data.size.x;
			};

		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::state_name);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_state_name"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::commodity_type);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_goods"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::output_amount);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_output"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::profit);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_income"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::employment);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_employed"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_generic_sort_button>(state, button_def, ledger_sort_type::factory_level);
			ptr->set_button_text(state, text::produce_simple_string(state, "ledger_level"));
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
	}
};

class ledger_factories_production_entry : public listbox_row_element_base<dcon::factory_id> {
public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base::on_create(state);

		xy_pair cell_offset{0, 0};

		auto cell_width = (972 - cell_offset.x) / 6;
		auto apply_offset = [&](auto& ptr) {
			ptr->base_data.position = cell_offset;
			ptr->base_data.size.x = int16_t(cell_width);
			cell_offset.x += ptr->base_data.size.x;
		};
		// State name
		{
			auto ptr = make_element_by_type<factory_state_name_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Goods
		{
			auto ptr = make_element_by_type<factory_output_name_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Output
		{
			auto ptr = make_element_by_type<factory_produced_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Income
		{
			auto ptr = make_element_by_type<factory_income_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Employed
		{
			auto ptr = make_element_by_type<factory_workers_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
		// Level
		{
			auto ptr = make_element_by_type<factory_level_text>(state,
					state.ui_state.defs_by_name.find("ledger_default_textbox")->second.definition);
			apply_offset(ptr);
			add_child_to_front(std::move(ptr));
		}
	}
};
class ledger_factories_production_listbox : public listbox_element_base<ledger_factories_production_entry, dcon::factory_id> {
protected:
	std::string_view get_row_element_name() override {
		return "default_listbox_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		for(auto const fat_smemb_id : state.world.nation_get_state_ownership(state.local_player_nation))
			province::for_each_province_in_state_instance(state, fat_smemb_id.get_state(), [&](dcon::province_id pid) {
				auto fat_id = dcon::fatten(state.world, pid);
				fat_id.for_each_factory_location_as_province(
						[&](dcon::factory_location_id flid) { row_contents.push_back(state.world.factory_location_get_factory(flid)); });
			});

		auto lsort = retrieve<ledger_sort>(state, parent);
		ledger_sort_type st = std::holds_alternative<ledger_sort_type>(lsort.type) ? std::get<ledger_sort_type>(lsort.type) : ledger_sort_type::country_name;
		switch(st) {
		case ledger_sort_type::commodity_type:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::factory_id a, dcon::factory_id b) {
				if(lsort.reversed) {
					return state.world.factory_get_building_type(a).get_output().id.index() < state.world.factory_get_building_type(b).get_output().id.index();
				} else {
					return state.world.factory_get_building_type(a).get_output().id.index() > state.world.factory_get_building_type(b).get_output().id.index();
				}
			});
			break;
		case ledger_sort_type::output_amount:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::factory_id a, dcon::factory_id b) {
				if(lsort.reversed) {
					return state.world.factory_get_actual_production(a) < state.world.factory_get_actual_production(b);
				} else {
					return state.world.factory_get_actual_production(a) > state.world.factory_get_actual_production(b);
				}
			});
			break;
		case ledger_sort_type::profit:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::factory_id a, dcon::factory_id b) {
				if(lsort.reversed) {
					return state.world.factory_get_full_profit(a) < state.world.factory_get_full_profit(b);
				} else {
					return state.world.factory_get_full_profit(a) > state.world.factory_get_full_profit(b);
				}
			});
			break;
		case ledger_sort_type::employment:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::factory_id a, dcon::factory_id b) {
				if(lsort.reversed) {
					return economy::factory_total_employment(state, a) < economy::factory_total_employment(state, b);
				} else {
					return economy::factory_total_employment(state, a) > economy::factory_total_employment(state, b);
				}
			});
			break;
		case ledger_sort_type::factory_level:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::factory_id a, dcon::factory_id b) {
				if(lsort.reversed) {
					return state.world.factory_get_level(a) < state.world.factory_get_level(b);
				} else {
					return state.world.factory_get_level(a) > state.world.factory_get_level(b);
				}
			});
			break;
		default:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::factory_id a, dcon::factory_id b) {
				if(lsort.reversed) {
					return text::get_province_state_name(state, state.world.factory_get_province_from_factory_location(a)) > text::get_province_state_name(state, state.world.factory_get_province_from_factory_location(b));
				} else {
					return text::get_province_state_name(state, state.world.factory_get_province_from_factory_location(a)) < text::get_province_state_name(state, state.world.factory_get_province_from_factory_location(b));
				}
			});
			break;
		}

		update(state);
	}
};

class ledger_commodity_plupp : public tinted_image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::commodity_id>(state, parent);
		color = state.world.commodity_get_color(content);
	}
};

//
// Commodity price
//
class ledger_commodity_price_entry : public listbox_row_element_base<dcon::commodity_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "ledger_default_textbox") {
			return make_element_by_type<generic_name_text<dcon::commodity_id>>(state, id);
		} else if(name == "ledger_legend_plupp") {
			return make_element_by_type<ledger_commodity_plupp>(state, id);
		} else {
			return nullptr;
		}
	}
};
class ledger_commodity_price_listbox : public listbox_element_base<ledger_commodity_price_entry, dcon::commodity_id> {
protected:
	std::string_view get_row_element_name() override {
		return "ledger_legend_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_commodity([&](dcon::commodity_id id) { row_contents.push_back(id); });
		update(state);
	}
};
class commodity_linegraph_legend_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "legend_list") {
			return make_element_by_type<ledger_commodity_price_listbox>(state, id);
		} else {
			return nullptr;
		}
	}
};

class ledger_window : public window_element_base {
	dcon::gui_def_id listbox_def_id{};
	dcon::gui_def_id sort_buttons_window_id{};

	window_element_base* commodity_linegraph = nullptr;
	commodity_linegraph_legend_window* commodity_linegraph_legend = nullptr;
	image_element_base* commodity_linegraph_image = nullptr;
	simple_text_element_base* page_number_text = nullptr;
	simple_text_element_base* ledger_header_text = nullptr;
	ledger_page_number page_num{int8_t(1)};

	ledger_nation_ranking_listbox* nation_ranking_listbox = nullptr;
	ledger_nation_comparison_listbox* nation_compare_listbox = nullptr;
	ledger_nation_political_systems_listbox* nation_party_listbox = nullptr;
	ledger_nation_political_reforms_listbox* nation_political_reforms_listbox = nullptr;
	ledger_nation_social_reforms_listbox* nation_social_reforms_listbox = nullptr;
	ledger_nation_population_listbox* nation_pops_listbox = nullptr;
	ledger_province_listbox* provinces_listbox = nullptr;
	ledger_province_population_listbox* provinces_pops_listbox = nullptr;
	ledger_provinces_production_listbox* provinces_production_listbox = nullptr;
	ledger_factories_production_listbox* factory_production_listbox = nullptr;

	ledger_sort current_sorting;

	void hide_sub_windows(sys::state& state) noexcept {
		nation_ranking_listbox->set_visible(state, false);
		nation_compare_listbox->set_visible(state, false);
		nation_party_listbox->set_visible(state, false);
		nation_political_reforms_listbox->set_visible(state, false);
		nation_social_reforms_listbox->set_visible(state, false);
		nation_pops_listbox->set_visible(state, false);
		provinces_listbox->set_visible(state, false);
		provinces_pops_listbox->set_visible(state, false);
		provinces_production_listbox->set_visible(state, false);
		factory_production_listbox->set_visible(state, false);

		// Linegraphs
		commodity_linegraph->set_visible(state, false);
		commodity_linegraph_legend->set_visible(state, false);
		commodity_linegraph_image->set_visible(state, false);
	}

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		{
			auto ptr = make_element_by_type<ledger_nation_ranking_listbox>(state, listbox_def_id);
			nation_ranking_listbox = ptr.get();

			auto ptrb = make_element_by_type<ledger_nation_ranking_buttons>(state, sort_buttons_window_id);
			ptrb->base_data.position.x -= ptr->base_data.position.x;
			ptrb->base_data.position.y -= ptr->base_data.position.y;
			ptr->add_child_to_back(std::move(ptrb));

			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_nation_comparison_listbox>(state, listbox_def_id);
			nation_compare_listbox = ptr.get();

			auto ptrb = make_element_by_type<ledger_nation_comparison_buttons>(state, sort_buttons_window_id);
			ptrb->base_data.position.x -= ptr->base_data.position.x;
			ptrb->base_data.position.y -= ptr->base_data.position.y;
			ptr->add_child_to_back(std::move(ptrb));

			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_nation_political_systems_listbox>(state, listbox_def_id);
			nation_party_listbox = ptr.get();

			auto ptrb = make_element_by_type<ledger_nation_political_systems_buttons>(state, sort_buttons_window_id);
			ptrb->base_data.position.x -= ptr->base_data.position.x;
			ptrb->base_data.position.y -= ptr->base_data.position.y;
			ptr->add_child_to_back(std::move(ptrb));

			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_nation_political_reforms_listbox>(state, listbox_def_id);
			nation_political_reforms_listbox = ptr.get();

			auto ptrb = make_element_by_type<ledger_nation_reforms_buttons<true>>(state, sort_buttons_window_id);
			ptrb->base_data.position.x -= ptr->base_data.position.x;
			ptrb->base_data.position.y -= ptr->base_data.position.y;
			ptr->add_child_to_back(std::move(ptrb));

			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_nation_social_reforms_listbox>(state, listbox_def_id);
			nation_social_reforms_listbox = ptr.get();

			auto ptrb = make_element_by_type<ledger_nation_reforms_buttons<false>>(state, sort_buttons_window_id);
			ptrb->base_data.position.x -= ptr->base_data.position.x;
			ptrb->base_data.position.y -= ptr->base_data.position.y;
			ptr->add_child_to_back(std::move(ptrb));

			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_nation_population_listbox>(state, listbox_def_id);
			nation_pops_listbox = ptr.get();

			auto ptrb = make_element_by_type<ledger_nation_population_buttons>(state, sort_buttons_window_id);
			ptrb->base_data.position.x -= ptr->base_data.position.x;
			ptrb->base_data.position.y -= ptr->base_data.position.y;
			ptr->add_child_to_back(std::move(ptrb));

			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_province_listbox>(state, listbox_def_id);
			provinces_listbox = ptr.get();

			auto ptrb = make_element_by_type<ledger_province_buttons>(state, sort_buttons_window_id);
			ptrb->base_data.position.x -= ptr->base_data.position.x;
			ptrb->base_data.position.y -= ptr->base_data.position.y;
			ptr->add_child_to_back(std::move(ptrb));

			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_province_population_listbox>(state, listbox_def_id);
			provinces_pops_listbox = ptr.get();

			auto ptrb = make_element_by_type<ledger_province_population_buttons>(state, sort_buttons_window_id);
			ptrb->base_data.position.x -= ptr->base_data.position.x;
			ptrb->base_data.position.y -= ptr->base_data.position.y;
			ptr->add_child_to_back(std::move(ptrb));

			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_provinces_production_listbox>(state, listbox_def_id);
			provinces_production_listbox = ptr.get();

			auto ptrb = make_element_by_type<ledger_province_production_buttons>(state, sort_buttons_window_id);
			ptrb->base_data.position.x -= ptr->base_data.position.x;
			ptrb->base_data.position.y -= ptr->base_data.position.y;
			ptr->add_child_to_back(std::move(ptrb));

			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<ledger_factories_production_listbox>(state, listbox_def_id);
			factory_production_listbox = ptr.get();

			auto ptrb = make_element_by_type<ledger_factories_production_buttons>(state, sort_buttons_window_id);
			ptrb->base_data.position.x -= ptr->base_data.position.x;
			ptrb->base_data.position.y -= ptr->base_data.position.y;
			ptr->add_child_to_back(std::move(ptrb));

			add_child_to_front(std::move(ptr));
		}

		Cyto::Any payload = page_num;
		impl_set(state, payload);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close") {
			auto ptr = make_element_by_type<generic_close_button>(state, id);
			ptr->set_button_text(state, "");
			return ptr;
		} else if(name == "ledger_bg") {
			return make_element_by_type<draggable_target>(state, id);
		} else if(name == "ledger_linegraphs") {
			auto ptr = make_element_by_type<window_element_base>(state, id);
			commodity_linegraph = ptr.get();
			return ptr;
		} else if(name == "ledger_linegraph_legend") {
			auto ptr = make_element_by_type<commodity_linegraph_legend_window>(state, id);
			commodity_linegraph_legend = ptr.get();
			return ptr;
		} else if(name == "ledger_linegraph_bg") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			commodity_linegraph_image = ptr.get();
			return ptr;
		} else if(name == "page_number") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			page_number_text = ptr.get();
			return ptr;
		} else if(name == "ledger_header") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ledger_header_text = ptr.get();
			return ptr;
		} else if(name == "prev") {
			return make_element_by_type<ledger_prev_button>(state, id);
		} else if(name == "next") {
			return make_element_by_type<ledger_next_button>(state, id);
		} else if(name == "ledger_sort_buttons") {
			sort_buttons_window_id = id;
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "default_listbox") {
			listbox_def_id = id;
			return make_element_by_type<invisible_element>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		page_number_text->set_text(state, std::to_string(page_num.value));

		hide_sub_windows(state);
		switch(page_num.value) {
		case 1:
			ledger_header_text->set_text(state, text::produce_simple_string(state, "ledger_header_rank"));
			nation_ranking_listbox->set_visible(state, true);
			break;
		case 2:
			ledger_header_text->set_text(state, text::produce_simple_string(state, "ledger_header_countrycompare"));
			nation_compare_listbox->set_visible(state, true);
			break;
		case 3:
			ledger_header_text->set_text(state, text::produce_simple_string(state, "ledger_header_countryparty"));
			nation_party_listbox->set_visible(state, true);
			break;
		case 4:
			ledger_header_text->set_text(state, text::produce_simple_string(state, "ledger_header_countrypoliticalreforms"));
			nation_political_reforms_listbox->set_visible(state, true);
			break;
		case 5:
			ledger_header_text->set_text(state, text::produce_simple_string(state, "ledger_header_countrysocialreforms"));
			nation_social_reforms_listbox->set_visible(state, true);
			break;
		case 6:
			ledger_header_text->set_text(state, text::produce_simple_string(state, "ledger_header_country_pops"));
			nation_pops_listbox->set_visible(state, true);
			break;
		case 7:
			ledger_header_text->set_text(state, text::produce_simple_string(state, "ledger_header_provinces"));
			provinces_listbox->set_visible(state, true);
			break;
		case 8:
			ledger_header_text->set_text(state, text::produce_simple_string(state, "ledger_header_province_pops"));
			provinces_pops_listbox->set_visible(state, true);
			break;
		case 9:
			ledger_header_text->set_text(state, text::produce_simple_string(state, "ledger_header_provinceproduction"));
			provinces_production_listbox->set_visible(state, true);
			break;
		case 10:
			ledger_header_text->set_text(state, text::produce_simple_string(state, "ledger_header_factoryproduction"));
			factory_production_listbox->set_visible(state, true);
			break;
		case 11:
			ledger_header_text->set_text(state, text::produce_simple_string(state, "ledger_header_goods_pricehistory"));
			commodity_linegraph->set_visible(state, true);
			commodity_linegraph_legend->set_visible(state, true);
			commodity_linegraph_image->set_visible(state, true);
			break;
		default:
			break;
		}
	}
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<ledger_sort>()) {
			payload.emplace<ledger_sort>(current_sorting);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<ledger_sort>>()) {
			current_sorting = any_cast<element_selection_wrapper<ledger_sort>>(payload).data;
			impl_on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<ledger_page_number>()) {
			page_num = any_cast<ledger_page_number>(payload);
			for(auto& child : children)
				child->impl_set(state, payload);
			impl_on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};
} // namespace ui
