#include "gui_common_elements.hpp"
#include "gui_listbox_templates.hpp"
#include "gui_templates.hpp"
#include "ai_alliances.hpp"
#include "economy.hpp"
#include "economy_production.hpp"
#include "economy_government.hpp"
#include "construction.hpp"
#include "economy_stats.hpp"
#include "economy_factory_view.hpp"

namespace ui {
bool country_category_filter_check(sys::state& state, country_list_filter filt, dcon::nation_id a, dcon::nation_id b) {
	switch(filt) {
	case country_list_filter::all:
		return true;
	case country_list_filter::allies:
		return nations::are_allied(state, a, b);
	case country_list_filter::enemies:
		return military::are_at_war(state, a, b);
	case country_list_filter::sphere:
		return state.world.nation_get_in_sphere_of(b) == a;
	case country_list_filter::neighbors:
		return bool(state.world.get_nation_adjacency_by_nation_adjacency_pair(a, b));
	case country_list_filter::find_allies:
		return ai::ai_will_accept_alliance(state, b, a)
			&& command::can_ask_for_alliance(state, a, b, false);
	case country_list_filter::influenced:
		return (state.world.gp_relationship_get_status(state.world.get_gp_relationship_by_gp_influence_pair(b, a))
			& nations::influence::priority_mask) != nations::influence::priority_zero
			&& state.world.nation_get_in_sphere_of(b) != a;
	case country_list_filter::neighbors_no_vassals:
		for(const auto sub : state.world.nation_get_overlord_as_ruler(b)) {
			if(state.world.get_nation_adjacency_by_nation_adjacency_pair(a, sub.get_subject()))
				return true;
		}
		return !state.world.overlord_get_ruler(state.world.nation_get_overlord_as_subject(b))
			&& state.world.get_nation_adjacency_by_nation_adjacency_pair(a, b);
	default:
		return true;
	}
}

void sort_countries(sys::state& state, std::vector<dcon::nation_id>& list, country_list_sort sort, bool sort_ascend) {
	switch(sort) {
	case country_list_sort::country: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			auto a_name = text::produce_simple_string(state, text::get_name(state, a));
			auto b_name = text::produce_simple_string(state, text::get_name(state, b));
			return a_name < b_name;
		};
		std::stable_sort(list.begin(), list.end(), f);
	}
		break;
	case country_list_sort::economic_rank: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			return state.world.nation_get_industrial_rank(a) < state.world.nation_get_industrial_rank(b);
		};
		std::stable_sort(list.begin(), list.end(), f);
	}
		break;
	case country_list_sort::military_rank: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			return state.world.nation_get_military_rank(a) < state.world.nation_get_military_rank(b);
		};
		std::stable_sort(list.begin(), list.end(), f);
	}
		break;
	case country_list_sort::prestige_rank: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			return state.world.nation_get_prestige_rank(a) < state.world.nation_get_prestige_rank(b);
		};
		std::stable_sort(list.begin(), list.end(), f);
	}
		break;
	case country_list_sort::total_rank: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			return state.world.nation_get_rank(a) < state.world.nation_get_rank(b);
		};
		std::stable_sort(list.begin(), list.end(), f);
	}
		break;
	case country_list_sort::relation: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			auto rid_a = state.world.get_diplomatic_relation_by_diplomatic_pair(state.local_player_nation, a);
			auto va = bool(rid_a) ? state.world.diplomatic_relation_get_value(rid_a) : 0;
			auto rid_b = state.world.get_diplomatic_relation_by_diplomatic_pair(state.local_player_nation, b);
			auto vb = bool(rid_b) ? state.world.diplomatic_relation_get_value(rid_b) : 0;
			return va > vb;
		};
		std::stable_sort(list.begin(), list.end(), f);
	}
		break;
	case country_list_sort::opinion: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			auto grid_a = state.world.get_gp_relationship_by_gp_influence_pair(a, state.local_player_nation);
			auto va = bool(grid_a) ? state.world.gp_relationship_get_status(grid_a) : 0;
			auto grid_b = state.world.get_gp_relationship_by_gp_influence_pair(b, state.local_player_nation);
			auto vb = bool(grid_b) ? state.world.gp_relationship_get_status(grid_b) : 0;
			return nations::influence::is_influence_level_greater(va & nations::influence::level_mask,
					vb & nations::influence::level_mask);
		};
		std::stable_sort(list.begin(), list.end(), f);
	}
		break;
	case country_list_sort::priority: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			auto rel_a = state.world.get_gp_relationship_by_gp_influence_pair(a, state.local_player_nation);
			uint8_t rel_flags_a = bool(rel_a) ? state.world.gp_relationship_get_status(rel_a) : 0;
			auto rel_b = state.world.get_gp_relationship_by_gp_influence_pair(b, state.local_player_nation);
			uint8_t rel_flags_b = bool(rel_b) ? state.world.gp_relationship_get_status(rel_b) : 0;
			return (rel_flags_a & nations::influence::priority_mask) > (rel_flags_b & nations::influence::priority_mask);
		};
		std::stable_sort(list.begin(), list.end(), f);
	}
		break;
	case country_list_sort::boss: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			return state.world.nation_get_in_sphere_of(a).id.index() > state.world.nation_get_in_sphere_of(b).id.index();
		};
		std::stable_sort(list.begin(), list.end(), f);
	}
		break;
	case country_list_sort::player_influence: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			auto rel_a = state.world.get_gp_relationship_by_gp_influence_pair(a, state.local_player_nation);
			auto rel_b = state.world.get_gp_relationship_by_gp_influence_pair(b, state.local_player_nation);
			return state.world.gp_relationship_get_influence(rel_a) > state.world.gp_relationship_get_influence(rel_b);
		};
		std::stable_sort(list.begin(), list.end(), f);
	} break;
	case country_list_sort::player_investment: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			auto urel_a = state.world.get_unilateral_relationship_by_unilateral_pair(a, state.local_player_nation);
			auto urel_b = state.world.get_unilateral_relationship_by_unilateral_pair(b, state.local_player_nation);
			return state.world.unilateral_relationship_get_foreign_investment(urel_a) >
						 state.world.unilateral_relationship_get_foreign_investment(urel_b);
		};
		std::stable_sort(list.begin(), list.end(), f);
	} break;
	case country_list_sort::factories: {
		auto num_factories = [&](dcon::nation_id n) {
			int32_t total = 0;
			for(auto p : state.world.nation_get_province_ownership(n)) {
				auto fac = p.get_province().get_factory_location();
				total += int32_t(fac.end() - fac.begin());
			}
			return total;
		};
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			return num_factories(a) > num_factories(b);
		};
		std::stable_sort(list.begin(), list.end(), f);
	} break;
	default:
		uint8_t rank = uint8_t(sort) & 0x3F;
		dcon::nation_id gp = nations::get_nth_great_power(state, rank);

		if((uint8_t(sort) & uint8_t(country_list_sort::gp_influence)) != 0) {
			auto f = [&](dcon::nation_id a, dcon::nation_id b) {
				auto rel_a = state.world.get_gp_relationship_by_gp_influence_pair(a, gp);
				auto rel_b = state.world.get_gp_relationship_by_gp_influence_pair(b, gp);
				return state.world.gp_relationship_get_influence(rel_a) > state.world.gp_relationship_get_influence(rel_b);
			};
			std::sort(list.begin(), list.end(), f);
		} else if((uint8_t(sort) & uint8_t(country_list_sort::gp_investment)) != 0) {
			auto f = [&](dcon::nation_id a, dcon::nation_id b) {
				auto urel_a = state.world.get_unilateral_relationship_by_unilateral_pair(a, gp);
				auto urel_b = state.world.get_unilateral_relationship_by_unilateral_pair(b, gp);
				return state.world.unilateral_relationship_get_foreign_investment(urel_a) >
							 state.world.unilateral_relationship_get_foreign_investment(urel_b);
			};
			std::stable_sort(list.begin(), list.end(), f);
		} else {
			assert(0);
		}
		break;
	}

	if(!sort_ascend) {
		std::reverse(list.begin(), list.end());
	}

}
std::string get_status_text(sys::state& state, dcon::nation_id nation_id) {
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

std::unique_ptr<element_base> national_focus_category::make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept {
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

message_result national_focus_category::set(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<nations::focus_type>()) {
		auto category = any_cast<nations::focus_type>(payload);
		if(category_label)
			category_label->set_text(state, text::get_focus_category_name(state, category));

		if(focus_list) {
			focus_list->row_contents.clear();
			state.world.for_each_national_focus([&](dcon::national_focus_id focus_id) {
				auto fat_id = dcon::fatten(state.world, focus_id);
				if(fat_id.get_type() == uint8_t(category))
					focus_list->row_contents.push_back(focus_id);
			});
			focus_list->update(state);
		}
		return message_result::consumed;
	} else {
		return message_result::unseen;
	}
}

void state_name_text::on_update(sys::state& state) noexcept {
	auto content = retrieve<dcon::state_instance_id>(state, parent);
	set_text(state, text::get_dynamic_state_name(state, content));
}

void province_name_text::on_update(sys::state& state) noexcept {
	auto content = retrieve<dcon::province_id>(state, parent);
	set_text(state, text::produce_simple_string(state, state.world.province_get_name(content)));
}

void province_factory_count_text::on_update(sys::state& state) noexcept {
	auto content = retrieve<dcon::province_id>(state, parent);
	int32_t count = economy::province_factory_count(state, content);
	auto txt = std::to_string(count) + "/" + std::to_string(int32_t(state.defines.factories_per_state));
	set_text(state, txt);
}

std::string standard_nation_text::get_text(sys::state& state, dcon::nation_id nation_id) noexcept {
	return "";
}

void standard_nation_text::on_update(sys::state& state) noexcept {
	auto content = retrieve<dcon::nation_id>(state, parent);
	set_text(state, get_text(state, content));
}

void province_goods_produced_text::on_update(sys::state& state) noexcept {
	auto province_id = retrieve<dcon::province_id>(state, parent);
	set_text(state, text::format_float(economy::rgo_output(state, state.world.province_get_rgo(province_id), province_id), 3));
}

void province_goods_produced_text::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto p = retrieve<dcon::province_id>(state, parent);

	auto n = state.world.province_get_nation_from_province_ownership(p);

	state.world.for_each_commodity([&](dcon::commodity_id c) {
		auto production = economy::rgo_output(state, c, p);

		if(production < 0.0001f) {
			return;
		}

		auto base_box = text::open_layout_box(contents);
		auto name_box = base_box;
		name_box.x_size = 75;
		auto production_box = base_box;
		production_box.x_position += 120.f;

		text::add_to_layout_box(state, contents, name_box, text::get_name_as_string(state, dcon::fatten(state.world, c)));
		text::add_to_layout_box(state, contents, production_box, text::format_money(production));
		text::add_to_layout_box(state, contents, base_box, std::string(" "));
		text::close_layout_box(contents, base_box);
	});
}

void province_income_text::on_update(sys::state& state) noexcept {
	auto province_id = retrieve<dcon::province_id>(state, parent);
	set_text(state, text::format_money(economy::rgo_income(state, province_id)));
}

tooltip_behavior province_income_text::has_tooltip(sys::state& state) noexcept {
	return tooltip_behavior::variable_tooltip;
}

void province_income_text::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto p = retrieve<dcon::province_id>(state, parent);

	auto n = state.world.province_get_nation_from_province_ownership(p);

	state.world.for_each_commodity([&](dcon::commodity_id c) {
		auto profit = economy::rgo_income(state, c, p);

		if(profit < 0.0001f) {
			return;
		}

		auto base_box = text::open_layout_box(contents);
		auto name_box = base_box;
		name_box.x_size = 75;
		auto profit_box = base_box;
		profit_box.x_position += 120.f;

		text::add_to_layout_box(state, contents, name_box, text::get_name_as_string(state, dcon::fatten(state.world, c)));
		text::add_to_layout_box(state, contents, profit_box, text::format_money(profit));
		text::add_to_layout_box(state, contents, base_box, std::string(" "));
		text::close_layout_box(contents, base_box);
	});
}

void factory_level_text::on_create(sys::state& state) noexcept {
	base_data.size.x += int16_t(20);
}
void factory_level_text::on_update(sys::state& state) noexcept {
	auto factory_id = retrieve<dcon::factory_id>(state, parent);
	set_text(state, text::format_float(economy::get_factory_level(state, factory_id)));
}

void province_rgo_workers_text::on_update(sys::state& state) noexcept {
	auto province_id = retrieve<dcon::province_id>(state, parent);
	set_text(state, text::prettify(int32_t(economy::rgo_employment(state, province_id))));
}

tooltip_behavior province_rgo_workers_text::has_tooltip(sys::state& state) noexcept {
	return tooltip_behavior::variable_tooltip;
}

void province_rgo_workers_text::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto p = retrieve<dcon::province_id>(state, parent);

	auto n = state.world.province_get_nation_from_province_ownership(p);
	auto s = state.world.province_get_state_membership(p);
	auto m = state.world.state_instance_get_market_from_local_market(s);

	auto row_1 = text::open_layout_box(contents);
	auto col_1 = row_1;
	col_1.x_size = 75;
	auto col_2 = row_1;
	col_2.x_position += 90.f;
	auto col_3 = row_1;
	col_3.x_position += 180.f;
	auto col_4 = row_1;
	col_4.x_position += 250.f;
	text::add_to_layout_box(state, contents, col_1, std::string_view("Good"));
	text::add_to_layout_box(state, contents, col_2, std::string_view("Employed"));
	text::add_to_layout_box(state, contents, col_3, std::string_view("Max"));
	text::add_to_layout_box(state, contents, col_4, std::string_view("Profit"));
	text::add_to_layout_box(state, contents, row_1, std::string_view(" "));
	text::close_layout_box(contents, row_1);

	state.world.for_each_commodity([&](dcon::commodity_id c) {
		auto current_employment = int64_t(economy::rgo_employment(state, c, p));
		auto max_employment = int64_t(economy::rgo_max_employment(state, c, p));

		if(max_employment < 1.f) {
			return;
		}
		auto base_box = text::open_layout_box(contents);
		auto name_box = base_box;
		name_box.x_size = 75;
		auto employment_box = base_box;
		employment_box.x_position += 120.f;
		auto max_employment_box = base_box;
		max_employment_box.x_position += 180.f;

		text::add_to_layout_box(state, contents, name_box, text::get_name_as_string(state, dcon::fatten(state.world, c)));


		text::add_to_layout_box(state, contents, employment_box, current_employment);
		text::add_to_layout_box(state, contents, max_employment_box, max_employment);

		text::add_to_layout_box(state, contents, base_box, std::string(" "));
		text::close_layout_box(contents, base_box);
	});

	auto rgo_employment = state.world.province_get_subsistence_employment(p);
	auto current_employment = int64_t(rgo_employment);
	auto max_employment = int64_t(economy::subsistence_max_pseudoemployment(state, p));
	auto expected_profit = 0.f;

	auto base_box = text::open_layout_box(contents);
	auto name_box = base_box;
	name_box.x_size = 75;
	auto employment_box = base_box;
	employment_box.x_position += 120.f;
	auto max_employment_box = base_box;
	max_employment_box.x_position += 180.f;
	auto expected_profit_box = base_box;
	expected_profit_box.x_position += 250.f;

	text::add_to_layout_box(state, contents, name_box, std::string_view("Subsistence"));

	text::add_to_layout_box(state, contents, employment_box, current_employment);
	text::add_to_layout_box(state, contents, max_employment_box, max_employment);
	text::add_to_layout_box(state, contents, expected_profit_box, text::format_money(expected_profit));

	text::add_to_layout_box(state, contents, base_box, std::string(" "));
	text::close_layout_box(contents, base_box);

	active_modifiers_description(state, contents, p, 15, sys::provincial_mod_offsets::mine_rgo_size, false);
	if(auto owner = state.world.province_get_nation_from_province_ownership(p); owner)
		active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::mine_rgo_size, false);
	active_modifiers_description(state, contents, p, 15, sys::provincial_mod_offsets::farm_rgo_size, false);
	if(auto owner = state.world.province_get_nation_from_province_ownership(p); owner)
		active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::farm_rgo_size, false);
}
void factory_profit_text::on_update(sys::state& state) noexcept {
	auto content = retrieve<dcon::factory_id>(state, parent);

	auto profit = economy::explain_last_factory_profit(state, content).profit;
	bool is_positive = profit >= 0.f;
	auto text = (is_positive ? "+" : "") + text::format_float(profit, 2);
	// Create colour
	auto contents = text::create_endless_layout(state, internal_layout,
			text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y),
					base_data.data.text.font_handle, 0, text::alignment::left, text::text_color::black, true });
	auto box = text::open_layout_box(contents);
	text::add_to_layout_box(state, contents, box, text,
			is_positive ? text::text_color::dark_green : text::text_color::dark_red);
	text::close_layout_box(contents, box);
}
void factory_income_image::on_update(sys::state& state) noexcept {
	auto content = retrieve<dcon::factory_id>(state, parent);
	float profit = economy::explain_last_factory_profit(state, content).profit;

	if(profit > 0.f) {
		frame = 0;
	} else if(profit < 0.f) {
		frame = 1;
	} else {
		frame = 2; //empty frame
	}
}
void factory_priority_image::on_update(sys::state& state) noexcept {
	auto content = retrieve<dcon::factory_id>(state, parent);
	frame = economy::factory_priority(state, content);
}

void commodity_image::on_update(sys::state& state) noexcept {
	frame = int32_t(state.world.commodity_get_icon(retrieve<dcon::commodity_id>(state, parent)));
}
tooltip_behavior commodity_image::has_tooltip(sys::state& state) noexcept {
	return tooltip_behavior::variable_tooltip;
}

void commodity_image::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto com = retrieve<dcon::commodity_id>(state, parent);
	if(!com)
		return;

	auto n = retrieve<dcon::nation_id>(state, parent);
	auto p = retrieve<dcon::province_id>(state, parent);

	commodity_tooltip(state, contents, com);


	// Nation modifiers
	if(bool(n)) {
		text::add_line_break_to_layout(state, contents);
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view{ "national_modifiers" });
		text::close_layout_box(contents, box);

		auto commodity_mod_description = [&](float value, std::string_view locale_base_name, std::string_view locale_farm_base_name) {
			if(value == 0.f)
				return;
			auto box = text::open_layout_box(contents, 15);
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, state.world.commodity_get_name(com)), text::text_color::white);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, state.world.commodity_get_is_mine(com) ? locale_base_name : locale_farm_base_name), text::text_color::white);
			text::add_to_layout_box(state, contents, box, std::string{ ":" }, text::text_color::white);
			text::add_space_to_layout_box(state, contents, box);
			auto color = value > 0.f ? text::text_color::green : text::text_color::red;
			text::add_to_layout_box(state, contents, box, (value > 0.f ? "+" : "") + text::format_percentage(value, 1), color);
			text::close_layout_box(contents, box);
			};
		commodity_mod_description(state.world.nation_get_factory_goods_output(n, com), "tech_output", "tech_output");
		commodity_mod_description(state.world.nation_get_rgo_goods_output(n, com), "tech_mine_output", "tech_farm_output");
		commodity_mod_description(state.world.nation_get_rgo_size(n, com), "tech_mine_size", "tech_farm_size");
	}
	if(economy::commodity_get_factory_types_as_output(state, com).size() > 0) {
		if(bool(n)) {
			active_modifiers_description(state, contents, n, 15, sys::national_mod_offsets::factory_output, true);
		}
		if(bool(p)) {
			active_modifiers_description(state, contents, p, 15, sys::provincial_mod_offsets::local_factory_output, true);
		}
		if(bool(n)) {
			active_modifiers_description(state, contents, n, 15, sys::national_mod_offsets::factory_throughput, true);
		}
		if(bool(p)) {
			active_modifiers_description(state, contents, p, 15, sys::provincial_mod_offsets::local_factory_throughput, true);
		}
	} else {
		if(state.world.commodity_get_is_mine(com)) {
			if(bool(n)) {
				active_modifiers_description(state, contents, n, 15, sys::national_mod_offsets::mine_rgo_eff, true);
			}
			if(bool(p)) {
				active_modifiers_description(state, contents, p, 15, sys::provincial_mod_offsets::mine_rgo_eff, true);
			}
			if(bool(n)) {
				active_modifiers_description(state, contents, n, 15, sys::national_mod_offsets::mine_rgo_size, true);
			}
			if(bool(p)) {
				active_modifiers_description(state, contents, p, 15, sys::provincial_mod_offsets::mine_rgo_size, true);
			}
		} else {
			if(bool(n)) {
				active_modifiers_description(state, contents, n, 15, sys::national_mod_offsets::farm_rgo_eff, true);
			}
			if(bool(p)) {
				active_modifiers_description(state, contents, p, 15, sys::provincial_mod_offsets::farm_rgo_eff, true);
			}
			if(bool(n)) {
				active_modifiers_description(state, contents, n, 15, sys::national_mod_offsets::farm_rgo_size, true);
			}
			if(bool(p)) {
				active_modifiers_description(state, contents, p, 15, sys::provincial_mod_offsets::farm_rgo_size, true);
			}
		}
	}

	for(const auto tid : state.world.in_technology) {
		if(state.world.nation_get_active_technologies(n, tid)) {
			bool have_header = false;
			auto commodity_technology_mod_description = [&](auto const& list, std::string_view locale_base_name, std::string_view locale_farm_base_name) {
				for(const auto mod : list) {
					if(mod.type != com || mod.amount == 0.f)
						return;
					auto box = text::open_layout_box(contents, 15);
					if(!have_header) {
						text::add_to_layout_box(state, contents, box, state.world.technology_get_name(tid), text::text_color::yellow);
						text::add_line_break_to_layout_box(state, contents, box);
						have_header = true;
					}
					auto name = state.world.commodity_get_name(mod.type);
					text::add_to_layout_box(state, contents, box, name);
					text::add_space_to_layout_box(state, contents, box);
					text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, state.world.commodity_get_is_mine(mod.type) ? locale_base_name : locale_farm_base_name), text::text_color::white);
					text::add_to_layout_box(state, contents, box, std::string{ ":" }, text::text_color::white);
					text::add_space_to_layout_box(state, contents, box);
					auto color = mod.amount > 0.f ? text::text_color::green : text::text_color::red;
					text::add_to_layout_box(state, contents, box, (mod.amount > 0.f ? "+" : "") + text::format_percentage(mod.amount, 1), color);
					text::close_layout_box(contents, box);
				}
				};
			commodity_technology_mod_description(state.world.technology_get_factory_goods_output(tid), "tech_output", "tech_output");
			commodity_technology_mod_description(state.world.technology_get_rgo_goods_output(tid), "tech_mine_output", "tech_farm_output");
			commodity_technology_mod_description(state.world.technology_get_rgo_size(tid), "tech_mine_size", "tech_farm_size");
		}
	}
	for(const auto iid : state.world.in_invention) {
		if(state.world.nation_get_active_inventions(n, iid)) {
			bool have_header = false;
			auto commodity_invention_mod_description = [&](auto const& list, std::string_view locale_base_name, std::string_view locale_farm_base_name) {
				for(const auto mod : list) {
					if(mod.type != com || mod.amount == 0.f)
						return;
					auto box = text::open_layout_box(contents, 15);
					if(!have_header) {
						text::add_to_layout_box(state, contents, box, state.world.invention_get_name(iid), text::text_color::yellow);
						text::add_line_break_to_layout_box(state, contents, box);
						have_header = true;
					}
					auto name = state.world.commodity_get_name(mod.type);
					text::add_to_layout_box(state, contents, box, name);
					text::add_space_to_layout_box(state, contents, box);
					text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, state.world.commodity_get_is_mine(mod.type) ? locale_base_name : locale_farm_base_name), text::text_color::white);
					text::add_to_layout_box(state, contents, box, std::string{ ":" }, text::text_color::white);
					text::add_space_to_layout_box(state, contents, box);
					auto color = mod.amount > 0.f ? text::text_color::green : text::text_color::red;
					text::add_to_layout_box(state, contents, box, (mod.amount > 0.f ? "+" : "") + text::format_percentage(mod.amount, 1), color);
					text::close_layout_box(contents, box);
				}
				};
			commodity_invention_mod_description(state.world.invention_get_factory_goods_output(iid), "tech_output", "tech_output");
			commodity_invention_mod_description(state.world.invention_get_rgo_goods_output(iid), "tech_mine_output", "tech_farm_output");
			commodity_invention_mod_description(state.world.invention_get_rgo_size(iid), "tech_mine_size", "tech_farm_size");
		}
	}
}

std::string nation_industry_score_text::get_text(sys::state& state, dcon::nation_id nation_id) noexcept {
	auto fat_id = dcon::fatten(state.world, nation_id);
	return std::to_string(int32_t(fat_id.get_industrial_score()));
}

tooltip_behavior nation_industry_score_text::has_tooltip(sys::state& state) noexcept {
	return tooltip_behavior::variable_tooltip;
}

void nation_industry_score_text::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto n = retrieve<dcon::nation_id>(state, parent);
	if(state.world.nation_get_owned_province_count(n) == 0)
		return;

	auto iweight = state.defines.investment_score_factor;
	for(auto si : state.world.nation_get_state_ownership(n)) {
		float score = 0.f;
		float workers = 0.f;
		province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id p) {
			for(auto f : state.world.province_get_factory_location(p)) {
				score += economy::factory_total_employment(state, f.get_factory())
					/ f.get_factory().get_building_type().get_base_workforce();
				workers += economy::factory_total_employment(state, f.get_factory());
			}
		});
		float per_state = 4.0f * score;
		if(per_state > 0.f) {
			auto box = text::open_layout_box(contents);
			text::layout_box name_entry = box;
			text::layout_box level_entry = box;
			text::layout_box workers_entry = box;
			text::layout_box max_workers_entry = box;
			text::layout_box score_box = box;

			name_entry.x_size /= 10;
			text::add_to_layout_box(state, contents, name_entry, text::get_short_state_name(state, si.get_state()).substr(0, 20), text::text_color::yellow);

			workers_entry.x_position += 150;
			text::add_to_layout_box(state, contents, workers_entry, text::int_wholenum{ int32_t(workers) });

			score_box.x_position += 250;
			text::add_to_layout_box(state, contents, score_box, text::fp_two_places{ per_state });

			text::add_to_layout_box(state, contents, box, std::string(" "));
			text::close_layout_box(contents, box);
		}
	}
	float total_invest = nations::get_foreign_investment(state, n);
	if(total_invest > 0.f) {
		text::add_line(state, contents, "industry_score_explain_2", text::variable_type::x, text::fp_four_places{ iweight });
		for(auto ur : state.world.nation_get_unilateral_relationship_as_source(n)) {
			if(ur.get_foreign_investment() > 0.f) {
				text::substitution_map sub{};
				text::add_to_substitution_map(sub, text::variable_type::x, ur.get_target());
				text::add_to_substitution_map(sub, text::variable_type::y, text::fp_currency{ ur.get_foreign_investment() });
				auto box = text::open_layout_box(contents);
				text::localised_format_box(state, contents, box, std::string_view("industry_score_explain_3"), sub);
				text::close_layout_box(contents, box);
			}
		}
	}
}

std::string nation_ppp_gdp_text::get_text(sys::state& state, dcon::nation_id nation_id) noexcept {
	return text::format_float(economy::gdp::value_nation_adjusted(state, nation_id));
}
std::string nation_ppp_gdp_per_capita_text::get_text(sys::state& state, dcon::nation_id nation_id) noexcept {
	float population = state.world.nation_get_demographics(nation_id, demographics::total);
	return text::format_float(economy::gdp::value_nation_adjusted(state, nation_id) / population * 1000000.f);
}

} // namespace ui
