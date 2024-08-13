#pragma once

#include "dcon_generated.hpp"
#include "economy.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"

namespace ui {


class commodity_price_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);
		set_text(state, text::format_money(state.world.commodity_get_current_price(commodity_id)));
	}
};

class commodity_effective_price_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		auto c = retrieve<dcon::commodity_id>(state, parent);
		set_text(state, text::format_money(state.world.nation_get_effective_prices(n, c)));
	}
};

class commodity_player_availability_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);
		if(commodity_id)
			set_text(state, text::format_float(state.world.nation_get_demand_satisfaction(state.local_player_nation, commodity_id), 2));
	}
};

class commodity_player_real_demand_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);
		if(commodity_id)
			set_text(state, text::format_float(state.world.nation_get_real_demand(state.local_player_nation, commodity_id), 1));
	}
};

class commodity_national_player_stockpile_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);
		if(commodity_id)
			set_text(state, text::format_float(state.world.nation_get_stockpiles(state.local_player_nation, commodity_id), 2));
	}
};

class commodity_player_stockpile_increase_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);
		float amount = economy::stockpile_commodity_daily_increase(state, commodity_id, state.local_player_nation);
		auto txt = std::string(amount >= 0.f ? "+" : "") + text::format_float(amount, 2);
		set_text(state, txt);
	}
};

class commodity_market_increase_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);
		float amount = economy::global_market_commodity_daily_increase(state, commodity_id);
		auto txt = std::string("(") + text::format_float(amount, 0) + ")";
		set_text(state, txt);
	}
};

class commodity_global_market_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);
		set_text(state, text::format_float(state.world.commodity_get_global_market_pool(commodity_id), 2));
	}
};

class commodity_player_domestic_market_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);
		if(commodity_id)
			set_text(state, text::format_float(commodity_id ? state.world.nation_get_domestic_market_pool(state.local_player_nation, commodity_id) : 0.0f, 2));
	}
};

class commodity_player_factory_needs_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);
		if(commodity_id)
			set_text(state, text::format_float(commodity_id ? economy::nation_factory_consumption(state, state.local_player_nation, commodity_id) : 0.0f, 2));
	}
};

class commodity_player_pop_needs_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);
		if(commodity_id)
			set_text(state, text::format_float(commodity_id ? economy::nation_pop_consumption(state, state.local_player_nation, commodity_id) : 0.0f, 2));
	}
};

class commodity_player_government_needs_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);
		if(commodity_id)
			set_text(state, text::format_float(commodity_id ? economy::government_consumption(state, state.local_player_nation, commodity_id) : 0.0f, 2));
	}
};

enum class trade_sort : uint8_t {
	commodity,
	stockpile,
	stockpile_change,
	price,
	demand_satisfaction,
	needs,
	market,
	real_demand,
	domestic_market,
	global_market_pool,
};
enum class trade_sort_assoc : uint8_t {
	market_activity,
	stockpile,
	common_market,
	needs_government,
	needs_factories,
	needs_pops
};
struct trade_sort_data {
	trade_sort sort;
	trade_sort_assoc assoc;
};

template<trade_sort Sort, trade_sort_assoc Assoc>
class trade_sort_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		send(state, parent, trade_sort_data{ Sort, Assoc });
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		/*auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "sort_by");
		text::add_space_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "trait_speed");
		text::close_layout_box(contents, box);*/
	}
};

class trade_market_activity_entry : public listbox_row_element_base<dcon::commodity_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "goods_type") {
			return make_element_by_type<commodity_image>(state, id);
		} else if(name == "cost") {
			return make_element_by_type<commodity_price_text>(state, id);
		} else if(name == "activity") {
			return make_element_by_type<commodity_player_availability_text>(state, id);
		} else {
			return nullptr;
		}
	}
};
class trade_market_activity_listbox : public listbox_element_base<trade_market_activity_entry, dcon::commodity_id> {
protected:
	std::string_view get_row_element_name() override {
		return "market_activity_entry";
	}

public:
	trade_sort sort = trade_sort::commodity;
	bool sort_ascend = false;
	void on_create(sys::state& state) noexcept override {
		listbox_element_base::on_create(state);
		on_update(state);
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_commodity([&](dcon::commodity_id id) {
			if(id != economy::money) {
				row_contents.push_back(id);
			}
		});
		switch(sort) {
		case trade_sort::commodity:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::commodity_id a, dcon::commodity_id b) {
				return a.index() < b.index();
			});
			break;
		case trade_sort::price:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::commodity_id a, dcon::commodity_id b) {
				auto av = state.world.commodity_get_current_price(a);
				auto bv = state.world.commodity_get_current_price(b);
				if(av != bv)
					return av > bv;
				else
					return a.index() < b.index();
			});
			break;
		case trade_sort::demand_satisfaction:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::commodity_id a, dcon::commodity_id b) {
				auto av = state.world.nation_get_demand_satisfaction(state.local_player_nation, a);
				auto bv = state.world.nation_get_demand_satisfaction(state.local_player_nation, b);
				if(av != bv)
					return av > bv;
				else
					return a.index() < b.index();
			});
			break;
		default:
			break;
		}
		if(!sort_ascend) {
			std::reverse(row_contents.begin(), row_contents.end());
		}
		update(state);
	}
};

class trade_stockpile_entry : public listbox_row_element_base<dcon::commodity_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "goods_type") {
			return make_element_by_type<commodity_image>(state, id);
		} else if(name == "value") {
			return make_element_by_type<commodity_national_player_stockpile_text>(state, id);
		} else if(name == "change") {
			return make_element_by_type<commodity_player_stockpile_increase_text>(state, id);
		} else {
			return nullptr;
		}
	}
};
class trade_stockpile_listbox : public listbox_element_base<trade_stockpile_entry, dcon::commodity_id> {
protected:
	std::string_view get_row_element_name() override {
		return "stockpile_entry";
	}

public:
	trade_sort sort = trade_sort::commodity;
	bool sort_ascend = false;
	void on_create(sys::state& state) noexcept override {
		listbox_element_base::on_create(state);
		on_update(state);
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_commodity([&](dcon::commodity_id id) {
			if(id != economy::money && state.world.nation_get_stockpiles(state.local_player_nation, id) > 0.0f) {
				row_contents.push_back(id);
			}
		});
		switch(sort) {
		case trade_sort::commodity:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::commodity_id a, dcon::commodity_id b) {
				return a.index() < b.index();
			});
			break;
		case trade_sort::stockpile:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::commodity_id a, dcon::commodity_id b) {
				auto av = state.world.nation_get_stockpiles(state.local_player_nation, a);
				auto bv = state.world.nation_get_stockpiles(state.local_player_nation, b);
				if(av != bv)
					return av > bv;
				else
					return a.index() < b.index();
			});
			break;
		case trade_sort::stockpile_change:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::commodity_id a, dcon::commodity_id b) {
				auto av = economy::stockpile_commodity_daily_increase(state, a, state.local_player_nation);
				auto bv = economy::stockpile_commodity_daily_increase(state, b, state.local_player_nation);
				if(av != bv)
					return av > bv;
				else
					return a.index() < b.index();
			});
			break;
		default:
			break;
		}
		if(!sort_ascend) {
			std::reverse(row_contents.begin(), row_contents.end());
		}
		update(state);
	}
};

class trade_common_market_entry : public listbox_row_element_base<dcon::commodity_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "goods_type") {
			return make_element_by_type<commodity_image>(state, id);
		} else if(name == "total") {
			return make_element_by_type<commodity_global_market_text>(state, id);
		} else if(name == "produce_change") {
			return make_element_by_type<commodity_player_real_demand_text>(state, id);
		} else if(name == "exported") {
			return make_element_by_type<commodity_player_domestic_market_text>(state, id);
		} else {
			return nullptr;
		}
	}
};
class trade_common_market_listbox : public listbox_element_base<trade_common_market_entry, dcon::commodity_id> {
protected:
	std::string_view get_row_element_name() override {
		return "common_market_entry";
	}
public:
	trade_sort sort = trade_sort::commodity;
	bool sort_ascend = false;
	void on_create(sys::state& state) noexcept override {
		listbox_element_base::on_create(state);
		on_update(state);
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_commodity([&](dcon::commodity_id id) {
			if(id != economy::money) {
				row_contents.push_back(id);
			}
		});
		switch(sort) {
		case trade_sort::commodity:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::commodity_id a, dcon::commodity_id b) {
				return a.index() < b.index();
			});
			break;
		case trade_sort::global_market_pool:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::commodity_id a, dcon::commodity_id b) {
				auto av = state.world.commodity_get_global_market_pool(a);
				auto bv = state.world.commodity_get_global_market_pool(b);
				if(av != bv)
					return av > bv;
				else
					return a.index() < b.index();
			});
			break;
		case trade_sort::real_demand:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::commodity_id a, dcon::commodity_id b) {
				auto av = state.world.nation_get_real_demand(state.local_player_nation, a);
				auto bv = state.world.nation_get_real_demand(state.local_player_nation, b);
				if(av != bv)
					return av > bv;
				else
					return a.index() < b.index();
			});
			break;
		case trade_sort::domestic_market:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::commodity_id a, dcon::commodity_id b) {
				auto av = state.world.nation_get_domestic_market_pool(state.local_player_nation, a);
				auto bv = state.world.nation_get_domestic_market_pool(state.local_player_nation, b);
				if(av != bv)
					return av > bv;
				else
					return a.index() < b.index();
			});
			break;
		default:
			break;
		}
		if(!sort_ascend) {
			std::reverse(row_contents.begin(), row_contents.end());
		}
		update(state);
	}
};

template<class T>
class trade_goods_needs_entry : public listbox_row_element_base<dcon::commodity_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "goods_type") {
			return make_element_by_type<commodity_image>(state, id);
		} else if(name == "value") {
			return make_element_by_type<T>(state, id);
		} else {
			return nullptr;
		}
	}
};

class trade_government_needs_listbox : public listbox_element_base<trade_goods_needs_entry<commodity_player_government_needs_text>, dcon::commodity_id> {
protected:
	std::string_view get_row_element_name() override {
		return "goods_needs_entry";
	}
public:
	trade_sort sort = trade_sort::commodity;
	bool sort_ascend = false;
	void on_create(sys::state& state) noexcept override {
		listbox_element_base::on_create(state);
		on_update(state);
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_commodity([&](dcon::commodity_id id) {
			if(id != economy::money && economy::government_consumption(state, state.local_player_nation, id) > 0.f) {
				row_contents.push_back(id);
			}
		});
		switch(sort) {
		case trade_sort::commodity:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::commodity_id a, dcon::commodity_id b) {
				return a.index() < b.index();
			});
			break;
		case trade_sort::needs:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::commodity_id a, dcon::commodity_id b) {
				auto av = economy::government_consumption(state, state.local_player_nation, a);
				auto bv = economy::government_consumption(state, state.local_player_nation, b);
				if(av != bv)
					return av > bv;
				else
					return a.index() < b.index();
			});
			break;
		default:
			break;
		}
		if(!sort_ascend) {
			std::reverse(row_contents.begin(), row_contents.end());
		}
		update(state);
	}
};

class trade_factory_needs_listbox : public listbox_element_base<trade_goods_needs_entry<commodity_player_factory_needs_text>, dcon::commodity_id> {
protected:
	std::string_view get_row_element_name() override {
		return "goods_needs_entry";
	}
public:
	trade_sort sort = trade_sort::commodity;
	bool sort_ascend = false;
	void on_create(sys::state& state) noexcept override {
		listbox_element_base::on_create(state);
		on_update(state);
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_commodity([&](dcon::commodity_id id) {
			if(id != economy::money && economy::nation_factory_consumption(state, state.local_player_nation, id) > 0.f) {
				row_contents.push_back(id);
			}
		});
		switch(sort) {
		case trade_sort::commodity:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::commodity_id a, dcon::commodity_id b) {
				return a.index() < b.index();
			});
			break;
		case trade_sort::needs:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::commodity_id a, dcon::commodity_id b) {
				auto av = economy::nation_factory_consumption(state, state.local_player_nation, a);
				auto bv = economy::nation_factory_consumption(state, state.local_player_nation, b);
				if(av != bv)
					return av > bv;
				else
					return a.index() < b.index();
			});
			break;
		default:
			break;
		}
		if(!sort_ascend) {
			std::reverse(row_contents.begin(), row_contents.end());
		}
		update(state);
	}
};

class trade_pop_needs_listbox : public listbox_element_base<trade_goods_needs_entry<commodity_player_pop_needs_text>, dcon::commodity_id> {
protected:
	std::string_view get_row_element_name() override {
		return "goods_needs_entry";
	}
public:
	trade_sort sort = trade_sort::commodity;
	bool sort_ascend = false;
	void on_create(sys::state& state) noexcept override {
		listbox_element_base::on_create(state);
		on_update(state);
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_commodity([&](dcon::commodity_id id) {
			if(id != economy::money && economy::nation_pop_consumption(state, state.local_player_nation, id) > 0.f) {
				row_contents.push_back(id);
			}
		});
		switch(sort) {
		case trade_sort::commodity:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::commodity_id a, dcon::commodity_id b) {
				return a.index() < b.index();
			});
			break;
		case trade_sort::needs:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::commodity_id a, dcon::commodity_id b) {
				auto av = economy::nation_pop_consumption(state, state.local_player_nation, a);
				auto bv = economy::nation_pop_consumption(state, state.local_player_nation, b);
				if(av != bv)
					return av > bv;
				else
					return a.index() < b.index();
			});
			break;
		default:
			break;
		}
		if(!sort_ascend) {
			std::reverse(row_contents.begin(), row_contents.end());
		}
		update(state);
	}
};

struct trade_details_select_commodity {
	dcon::commodity_id commodity_id{};
};
struct trade_details_open_window {
	dcon::commodity_id commodity_id{};
};

class trade_commodity_entry_button : public tinted_right_click_button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		auto sat = state.world.nation_get_demand_satisfaction(state.local_player_nation, com);
		if(sat < 0.5f) { // shortage
			color = sys::pack_color(255, 196, 196);
			if(state.user_settings.color_blind_mode == sys::color_blind_mode::deutan || state.user_settings.color_blind_mode == sys::color_blind_mode::protan) {
				color = sys::pack_color(255, 100, 255); //remap to blue
			} else if(state.user_settings.color_blind_mode == sys::color_blind_mode::achroma) {
				color = sys::pack_color(196, 196, 196);
			}
		} else if(sat >= 1.f) { // full fulfillment
			color = sys::pack_color(196, 255, 196);
			if(state.user_settings.color_blind_mode == sys::color_blind_mode::deutan || state.user_settings.color_blind_mode == sys::color_blind_mode::protan) {
				color = sys::pack_color(114, 150, 77); //remap to yellow
			} else if(state.user_settings.color_blind_mode == sys::color_blind_mode::achroma) {
				color = sys::pack_color(128, 128, 128);
			}
		} else {
			color = sys::pack_color(255, 255, 255);
		}
	}
	
	void button_action(sys::state& state) noexcept override {
		trade_details_select_commodity payload{retrieve<dcon::commodity_id>(state, parent)};
		send<trade_details_select_commodity>(state, state.ui_state.trade_subwindow, payload);
	}

	void button_right_action(sys::state& state) noexcept override {
		trade_details_select_commodity payload{ retrieve<dcon::commodity_id>(state, parent) };
		send<trade_details_select_commodity>(state, state.ui_state.trade_subwindow, payload);
		Cyto::Any dt_payload = trade_details_open_window{ retrieve<dcon::commodity_id>(state, parent) };
		state.ui_state.trade_subwindow->impl_get(state, dt_payload);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		text::add_line(state, contents, state.world.commodity_get_name(com));
		if(com != economy::money) {
			auto sat = state.world.nation_get_demand_satisfaction(state.local_player_nation, com);
			if(sat < 0.5f) {
				text::add_line(state, contents, "commodity_shortage");
			} else if(sat >= 1.f) {
				text::add_line(state, contents, "commodity_surplus");
			}
			text::add_line(state, contents, "alice_commodity_cprice", text::variable_type::x, text::format_money(state.world.commodity_get_current_price(com)));
			text::add_line(state, contents, "alice_commodity_cost", text::variable_type::x, text::format_money(state.world.commodity_get_cost(com)));
			text::add_line(state, contents, "alice_commodity_eprice", text::variable_type::x, text::format_money(state.world.nation_get_effective_prices(state.local_player_nation, com)));
			text::add_line_break_to_layout(state, contents);
		}
		text::add_line(state, contents, "trade_commodity_report_1", text::variable_type::x, text::fp_one_place{ state.world.commodity_get_total_real_demand(com) });
		text::add_line(state, contents, "trade_commodity_report_2", text::variable_type::x, text::fp_one_place{ state.world.commodity_get_total_production(com) });
		text::add_line(state, contents, "trade_commodity_report_4", text::variable_type::x, text::fp_one_place{ state.world.commodity_get_global_market_pool(com) });
		text::add_line_break_to_layout(state, contents);

		struct tagged_value {
			float v = 0.0f;
			dcon::nation_id n;
		};
		static std::vector<tagged_value> producers;

		producers.clear();
		for(auto n : state.world.in_nation) {
			if(n.get_domestic_market_pool(com) >= 0.05f) {
				producers.push_back(tagged_value{ n.get_domestic_market_pool(com), n.id });
			}
		}
		if(producers.size() > 0) {
			std::sort(producers.begin(), producers.end(), [](auto const& a, auto const& b) { return a.v > b.v; });
			{
				auto box = text::open_layout_box(contents, 0);
				text::substitution_map sub{};
				text::add_to_substitution_map(sub, text::variable_type::value, std::min(int32_t(state.defines.great_nations_count), int32_t(producers.size())));
				text::localised_format_box(state, contents, box, "alice_trade_top_producers", sub);
				text::close_layout_box(contents, box);
			}
			for(uint32_t i = 0; i < producers.size() && i < state.defines.great_nations_count; ++i) {
				auto box = text::open_layout_box(contents, 15);
				auto ident = state.world.nation_get_identity_from_identity_holder(producers[i].n);
				text::add_to_layout_box(state, contents, box, text::embedded_flag{ ident ? ident : state.national_definitions.rebel_id });
				text::add_space_to_layout_box(state, contents, box);
				text::add_to_layout_box(state, contents, box, text::get_name(state, producers[i].n));
				text::add_space_to_layout_box(state, contents, box);
				text::add_to_layout_box(state, contents, box, text::fp_one_place{ producers[i].v });
				text::close_layout_box(contents, box);
			}
			text::add_line_break_to_layout(state, contents);
			float r_total = 0.0f;
			for(auto p : state.world.in_province) {
				if(p.get_nation_from_province_ownership()) {
					r_total += p.get_rgo_actual_production_per_good(com);
				}
			}
			float a_total = 0.0f;
			for(auto n : state.world.in_nation) {
				a_total += state.world.nation_get_artisan_actual_production(n, com);
			}
			float f_total = 0.0f;
			for(auto f : state.world.in_factory) {
				if(f.get_building_type().get_output() == com)
					f_total += f.get_actual_production();
			}
			float total = r_total + a_total + f_total;
			if(r_total > 0.f) {
				if(com == economy::money) {
					text::add_line(state, contents, "alice_rgo_trade_prod_3",
						text::variable_type::x, text::fp_one_place{ r_total },
						text::variable_type::y, text::fp_percentage{ r_total / total });
				} else if(state.world.commodity_get_is_mine(com)) {
					text::add_line(state, contents, "alice_rgo_trade_prod_2",
						text::variable_type::x, text::fp_one_place{ r_total },
						text::variable_type::y, text::fp_percentage{ r_total / total });
				} else {
					text::add_line(state, contents, "alice_rgo_trade_prod_1",
						text::variable_type::x, text::fp_one_place{ r_total },
						text::variable_type::y, text::fp_percentage{ r_total / total });
				}
			}
			if(a_total > 0.f) {
				text::add_line(state, contents, "alice_artisan_trade_prod",
					text::variable_type::x, text::fp_one_place{ a_total },
					text::variable_type::y, text::fp_percentage{ a_total / total });
				text::add_line(state, contents, "w_artisan_profit", text::variable_type::x, text::fp_one_place{ economy::base_artisan_profit(state, state.local_player_nation, com) * economy::artisan_scale_limit(state, state.local_player_nation, com) });
				text::add_line(state, contents, "w_artisan_distribution", text::variable_type::x, text::fp_one_place{ economy::get_artisan_distribution_slow(state, state.local_player_nation, com) * 100.f });
			}
			if(f_total > 0.f) {
				text::add_line(state, contents, "alice_factory_trade_prod",
					text::variable_type::x, text::fp_one_place{ f_total },
					text::variable_type::y, text::fp_percentage{ f_total / total });
			}
			text::add_line(state, contents, "alice_all_trade_prod", text::variable_type::x, text::fp_one_place{ total });
		} else {
			text::add_line(state, contents, "alice_trade_no_producers");
		}
	}
};

class commodity_stockpile_indicator : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		if(state.world.nation_get_drawing_on_stockpiles(state.local_player_nation, com)) {
			if(state.world.nation_get_stockpiles(state.local_player_nation, com) > 0)
				frame = 2;
			else
				frame = 0;
		} else if(state.world.nation_get_stockpiles(state.local_player_nation, com)  < state.world.nation_get_stockpile_targets(state.local_player_nation, com)) {
			frame = 1;
		} else {
			frame = 0;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		if(state.world.nation_get_drawing_on_stockpiles(state.local_player_nation, com)) {
			return tooltip_behavior::variable_tooltip;
		} else if(state.world.nation_get_stockpiles(state.local_player_nation, com)  < state.world.nation_get_stockpile_targets(state.local_player_nation, com)) {
			return tooltip_behavior::variable_tooltip;
		} else {
			return tooltip_behavior::no_tooltip;
		}
		
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		if(state.world.nation_get_drawing_on_stockpiles(state.local_player_nation, com)) {
			if(state.world.nation_get_stockpiles(state.local_player_nation, com) > 0)
				text::add_line(state, contents, "trade_setting_drawing");
		} else if(state.world.nation_get_stockpiles(state.local_player_nation, com) < state.world.nation_get_stockpile_targets(state.local_player_nation, com)) {
			text::add_line(state, contents, "trade_setting_filling");
		} else {

		}
	}
};

class commodity_price_trend : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		auto current_price = state.world.commodity_get_price_record(com, (state.ui_date.value >> 4) % economy::price_history_length);
		auto previous_price = state.world.commodity_get_price_record(com, ((state.ui_date.value >> 4) + economy::price_history_length - 1) % economy::price_history_length);
		if(current_price > previous_price) {
			frame = 0;
		} else if(current_price < previous_price) {
			frame = 2;
		} else {
			frame = 1;
		}
	}
};

class trade_commodity_entry : public window_element_base {
public:
	dcon::commodity_id commodity_id{};
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "entry_button") {
			return make_element_by_type<trade_commodity_entry_button>(state, id);
		} else if(name == "goods_type") {
			return make_element_by_type<commodity_image>(state, id);
		} else if(name == "price") {
			auto ptr = make_element_by_type<commodity_effective_price_text>(state, id);
			ptr->base_data.position.y += ptr->base_data.size.y / 2;
			add_child_to_front(std::move(ptr));
			return make_element_by_type<commodity_price_text>(state, id);
		} else if(name == "trend_indicator") {
			return make_element_by_type<commodity_price_trend>(state, id);
		} else if(name == "selling_indicator") {
			return make_element_by_type<commodity_stockpile_indicator>(state, id);
		} else if(name == "automation_indicator") {
			return make_element_by_type<invisible_element>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::commodity_id>()) {
			payload.emplace<dcon::commodity_id>(commodity_id);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class trade_flow_data {
public:
	dcon::commodity_id trade_good;
	enum class type : uint8_t {
		factory,
		province,
		pop,
		military_army,
		military_navy,
	} type{};
	enum class value_type : uint8_t { used_by, produced_by, may_be_used_by } value_type{};
	union {
		dcon::factory_id factory_id;			 // factory
		dcon::province_id province_id;		 // province
		dcon::province_id pop_province_id; // pop
		dcon::army_id army_id;						 // army
		dcon::navy_id navy_id;						 // navy
	} data{};

	bool operator==(trade_flow_data const& o) const {
		if(value_type != o.value_type)
			return false;

		switch(type) {
		case type::factory:
			if(o.type != type::factory)
				return false;
			return data.factory_id == o.data.factory_id;
		case type::province:
			if(o.type != type::province)
				return false;
			return data.province_id == o.data.province_id;
		case type::pop:
			if(o.type != type::pop)
				return false;
			return data.pop_province_id == o.data.pop_province_id;
		case type::military_army:
			if(o.type != type::military_army)
				return false;
			return data.army_id == o.data.army_id;
		case type::military_navy:
			if(o.type != type::military_navy)
				return false;
			return data.navy_id == o.data.navy_id;
		default:
			return true;
		}
	}
	bool operator!=(trade_flow_data const& o) const {
		return !(*this == o);
	}
};
class trade_flow_entry : public listbox_row_element_base<trade_flow_data> {
	image_element_base* icon = nullptr;
	simple_text_element_base* title = nullptr;
	simple_text_element_base* value = nullptr;
	image_element_base* output_icon = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			icon = ptr.get();
			return ptr;
		} else if(name == "title") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			title = ptr.get();
			return ptr;
		} else if(name == "value") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			value = ptr.get();
			return ptr;
		} else if(name == "output_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			output_icon = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);

		icon->frame = int32_t(content.type);
		if(content.type == trade_flow_data::type::military_navy)
			icon->frame = int32_t(trade_flow_data::type::military_army);

		output_icon->set_visible(state, content.value_type != trade_flow_data::value_type::produced_by);
		value->set_visible(state, content.value_type != trade_flow_data::value_type::may_be_used_by);
		float amount = 0.f;
		switch(content.type) {
		case trade_flow_data::type::factory: {
			auto fid = content.data.factory_id;
			auto ftid = state.world.factory_get_building_type(fid);
			switch(content.value_type) {
			case trade_flow_data::value_type::produced_by: {
				amount += state.world.factory_get_actual_production(fid);
			} break;
			case trade_flow_data::value_type::used_by: {
				auto& inputs = state.world.factory_type_get_inputs(ftid);
				for(uint32_t i = 0; i < inputs.set_size; ++i)
					if(inputs.commodity_type[i] == commodity_id)
						amount += inputs.commodity_amounts[i];
				output_icon->frame = state.world.commodity_get_icon(state.world.factory_type_get_output(ftid));
			} break;
			case trade_flow_data::value_type::may_be_used_by:
				output_icon->frame = state.world.commodity_get_icon(state.world.factory_type_get_output(ftid));
				break;
			default:
				break;
			}
			auto name = state.world.factory_type_get_name(ftid);
			title->set_text(state, text::produce_simple_string(state, name));
		} break;
		case trade_flow_data::type::province: {
			auto pid = content.data.province_id;
			switch(content.value_type) {
			case trade_flow_data::value_type::produced_by: {
				amount += state.world.province_get_rgo_actual_production_per_good(pid, content.trade_good);
			} break;
			case trade_flow_data::value_type::used_by:
			case trade_flow_data::value_type::may_be_used_by:
			default:
				break;
			}
			auto name = state.world.province_get_name(pid);
			title->set_text(state, text::produce_simple_string(state, name));
		} break;
		case trade_flow_data::type::pop:
			break;
		case trade_flow_data::type::military_army:
			break;
		case trade_flow_data::type::military_navy:
			break;
		default:
			break;
		}
		if(value->is_visible())
			value->set_text(state, text::format_float(amount, 2));
	}
};
class trade_flow_listbox_base : public listbox_element_base<trade_flow_entry, trade_flow_data> {
protected:
	std::string_view get_row_element_name() override {
		return "trade_flow_entry";
	}

	template<typename F>
	void populate_rows(sys::state& state, F&& factory_func, enum trade_flow_data::value_type vt) {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);
		for(auto const fat_stown_id : state.world.nation_get_state_ownership(state.local_player_nation)) {
			province::for_each_province_in_state_instance(state, fat_stown_id.get_state(), [&](dcon::province_id pid) {
				auto fat_id = dcon::fatten(state.world, pid);
				fat_id.for_each_factory_location_as_province([&](dcon::factory_location_id flid) {
					auto fid = state.world.factory_location_get_factory(flid);
					if(factory_func(fid)) {
						trade_flow_data td{};
						td.type = trade_flow_data::type::factory;
						td.value_type = vt;
						td.data.factory_id = fid;
						td.trade_good = commodity_id;
						row_contents.push_back(td);
					}
				});
				if(vt == trade_flow_data::value_type::produced_by)
					if(state.world.province_get_rgo_actual_production_per_good(pid, commodity_id) > 0.f) {
						trade_flow_data td{};
						td.type = trade_flow_data::type::province;
						td.value_type = vt;
						td.data.province_id = pid;
						td.trade_good = commodity_id;
						row_contents.push_back(td);
					}
			});
		}
	}
};

class trade_flow_produced_by_listbox : public trade_flow_listbox_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);

		row_contents.clear();
		populate_rows(
				state,
				[&](dcon::factory_id fid) -> bool {
					auto ftid = state.world.factory_get_building_type(fid);
					return state.world.factory_type_get_output(ftid) == commodity_id;
				},
				trade_flow_data::value_type::produced_by);
		update(state);
	}
};
class trade_flow_used_by_listbox : public trade_flow_listbox_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);

		row_contents.clear();
		populate_rows(
				state,
				[&](dcon::factory_id fid) -> bool {
					auto ftid = state.world.factory_get_building_type(fid);
					auto& inputs = state.world.factory_type_get_inputs(ftid);
					for(uint32_t i = 0; i < inputs.set_size; ++i)
						if(inputs.commodity_type[i] == commodity_id)
							return inputs.commodity_amounts[i] > 0.f; // Some inputs taken
					return false;
				},
				trade_flow_data::value_type::used_by);
		update(state);
	}
};
class trade_flow_may_be_used_by_listbox : public trade_flow_listbox_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);

		row_contents.clear();
		populate_rows(
				state,
				[&](dcon::factory_id fid) -> bool {
					auto ftid = state.world.factory_get_building_type(fid);
					auto& inputs = state.world.factory_type_get_inputs(ftid);
					for(uint32_t i = 0; i < inputs.set_size; ++i)
						if(inputs.commodity_type[i] == commodity_id)
							return inputs.commodity_amounts[i] == 0.f; // No inputs intaken
					return false;
				},
				trade_flow_data::value_type::may_be_used_by);
		update(state);
	}
};

class trade_flow_producers_piechart : public piechart<dcon::nation_id> {
public:
	void on_update(sys::state& state) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		distribution.clear();
		for(auto n : state.world.in_nation)
			if(n.get_owned_province_count() != 0)
				distribution.emplace_back(n.id, n.get_domestic_market_pool(com));
		update_chart(state);
	}
};

class trade_flow_consumers_piechart : public piechart<dcon::nation_id> {
public:
	void on_update(sys::state& state) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		distribution.clear();
		for(auto n : state.world.in_nation)
			if(n.get_owned_province_count() != 0)
				distribution.emplace_back(n.id, n.get_real_demand(com));
		update_chart(state);
	}
};


class trade_flow_workers_piechart : public piechart<dcon::pop_type_id> {
public:
	void on_update(sys::state& state) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		distribution.clear();
		float total = 0.f;
		{
			float amount = 0.f;
			for(const auto pc : state.world.nation_get_province_control(state.local_player_nation)) {
				for(const auto fl : pc.get_province().get_factory_location()) {
					if(fl.get_factory().get_building_type().get_output() == com)
						amount += fl.get_factory().get_actual_production();
				}
			}
			total += amount;
			distribution.emplace_back(state.culture_definitions.capitalists, amount);
		}
		{
			float amount = 0.f;
			for(const auto pc : state.world.nation_get_province_control(state.local_player_nation)) {
				amount += pc.get_province().get_rgo_actual_production_per_good(com);
			}
			total += amount;
			distribution.emplace_back(state.culture_definitions.aristocrat, amount);
		}
		{
			auto amount = state.world.nation_get_artisan_actual_production(state.local_player_nation, com);
			total += amount;
			distribution.emplace_back(state.culture_definitions.artisans, amount);
		}
		// remaining
		auto produced = state.world.nation_get_domestic_market_pool(state.local_player_nation, com);
		if(produced >= total) {
			distribution.emplace_back(state.culture_definitions.laborers, total - produced);
		}
		update_chart(state);
	}
};

class trade_flow_price_graph_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "current_price_label") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_text(state, text::produce_simple_string(state, "alice_trade_flow_label"));
			ptr->base_data.size.x *= 2; // Nudge
			return ptr;
		} else if(name == "current_price_value"
			|| name == "price_linechart"
			|| name == "price_chart_low"
			|| name == "price_chart_time") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "price_chart_high") {
			auto ptr = make_element_by_type<invisible_element>(state, id);
			{
				auto ov_elm = make_element_by_type<image_element_base>(state, "generic_piechart_overlay");
				ov_elm->base_data.position.x = ptr->base_data.position.x;
				ov_elm->base_data.position.y = ptr->base_data.position.y;
				auto pc_elm = make_element_by_type<trade_flow_producers_piechart>(state, "generic_piechart");
				pc_elm->base_data.position.x += ov_elm->base_data.position.x;
				pc_elm->base_data.position.y += ov_elm->base_data.position.y;
				auto lg_elm = make_element_by_type<simple_text_element_base>(state, id);
				lg_elm->set_text(state, text::produce_simple_string(state, "alice_trade_flow_piechart_producers"));
				lg_elm->base_data.position.x = ptr->base_data.position.x;
				lg_elm->base_data.position.y = ptr->base_data.position.y - 8;
				add_child_to_front(std::move(lg_elm));
				add_child_to_front(std::move(pc_elm));
				add_child_to_front(std::move(ov_elm));
			}
			{
				auto ov_elm = make_element_by_type<image_element_base>(state, "generic_piechart_overlay");
				ov_elm->base_data.position.x = ptr->base_data.position.x + (ov_elm->base_data.size.x + 20) * 1;
				ov_elm->base_data.position.y = ptr->base_data.position.y;
				auto pc_elm = make_element_by_type<trade_flow_consumers_piechart>(state, "generic_piechart");
				pc_elm->base_data.position.x += ov_elm->base_data.position.x;
				pc_elm->base_data.position.y += ov_elm->base_data.position.y;
				auto lg_elm = make_element_by_type<simple_text_element_base>(state, id);
				lg_elm->set_text(state, text::produce_simple_string(state, "alice_trade_flow_piechart_consumers"));
				lg_elm->base_data.position.x = ptr->base_data.position.x + (ov_elm->base_data.size.x + 20) * 1;
				lg_elm->base_data.position.y = ptr->base_data.position.y - 8;
				add_child_to_front(std::move(lg_elm));
				add_child_to_front(std::move(pc_elm));
				add_child_to_front(std::move(ov_elm));
			}
			{
				auto ov_elm = make_element_by_type<image_element_base>(state, "generic_piechart_overlay");
				ov_elm->base_data.position.x = ptr->base_data.position.x + (ov_elm->base_data.size.x + 20) * 2;
				ov_elm->base_data.position.y = ptr->base_data.position.y;
				auto pc_elm = make_element_by_type<trade_flow_workers_piechart>(state, "generic_piechart");
				pc_elm->base_data.position.x += ov_elm->base_data.position.x;
				pc_elm->base_data.position.y += ov_elm->base_data.position.y;
				auto lg_elm = make_element_by_type<simple_text_element_base>(state, id);
				lg_elm->set_text(state, text::produce_simple_string(state, "alice_trade_flow_piechart_workforce"));
				lg_elm->base_data.position.x = ptr->base_data.position.x + (ov_elm->base_data.size.x + 20) * 2;
				lg_elm->base_data.position.y = ptr->base_data.position.y - 8;
				add_child_to_front(std::move(lg_elm));
				add_child_to_front(std::move(pc_elm));
				add_child_to_front(std::move(ov_elm));
			}
			return ptr;
		} else {
			return nullptr;
		}
	}
};

class trade_flow_total_produced_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);

		auto amount = 0.f;
		for(auto const fat_stown_id : state.world.nation_get_state_ownership(state.local_player_nation)) {
			province::for_each_province_in_state_instance(state, fat_stown_id.get_state(), [&](dcon::province_id pid) {
				auto fat_id = dcon::fatten(state.world, pid);
				fat_id.for_each_factory_location_as_province([&](dcon::factory_location_id flid) {
					auto fid = state.world.factory_location_get_factory(flid);
					auto ftid = state.world.factory_get_building_type(fid);
					if(state.world.factory_type_get_output(ftid) == commodity_id)
						amount += state.world.factory_get_actual_production(fid);
				});
				if(state.world.province_get_rgo(pid) == commodity_id)
					amount += state.world.province_get_rgo_actual_production_per_good(pid, commodity_id);
			});
		}

		set_text(state, text::format_float(amount, 2));
	}
};
class trade_flow_total_used_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);
		set_text(state, text::format_float(economy::nation_factory_consumption(state, state.local_player_nation, commodity_id), 2));
	}
};

class trade_flow_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "trade_flow_bg") {
			return make_element_by_type<draggable_target>(state, id);
		} else if(name == "material_name") {
			return make_element_by_type<generic_name_text<dcon::commodity_id>>(state, id);
		} else if(name == "material_icon_big") {
			return make_element_by_type<commodity_image>(state, id);
		} else if(name == "header_produced_by") {
			auto ptr = make_element_by_type<single_multiline_text_element_base>(state, id);
			ptr->text_id = text::find_or_add_key(state, "alice_trade_flow_produced", true);
			return ptr;
		} else if(name == "header_used_by") {
			auto ptr = make_element_by_type<single_multiline_text_element_base>(state, id);
			ptr->text_id = text::find_or_add_key(state, "alice_trade_flow_consumed", true);
			return ptr;
		} else if(name == "header_may_be_used_by") {
			auto ptr = make_element_by_type<single_multiline_text_element_base>(state, id);
			ptr->text_id = text::find_or_add_key(state, "trade_flow_may_be_used", true);
			return ptr;
		} else if(name == "total_produced_text") {
			auto ptr = make_element_by_type<single_multiline_text_element_base>(state, id);
			ptr->text_id = text::find_or_add_key(state, "trade_flow_total_produced", true);
			ptr->base_data.position.x += 48; // Nudge
			return ptr;
		} else if(name == "total_used_text") {
			auto ptr = make_element_by_type<single_multiline_text_element_base>(state, id);
			ptr->text_id = text::find_or_add_key(state, "trade_flow_total_used", true);
			ptr->base_data.position.x += 48; // Nudge
			return ptr;
		} else if(name == "total_produced_value") {
			return make_element_by_type<trade_flow_total_produced_text>(state, id);
		} else if(name == "total_used_value") {
			return make_element_by_type<trade_flow_total_used_text>(state, id);
		} else if(name == "price_graph") {
			return make_element_by_type<trade_flow_price_graph_window>(state, id);
		} else if(name == "produced_by_listbox") {
			return make_element_by_type<trade_flow_produced_by_listbox>(state, id);
		} else if(name == "used_by_listbox") {
			return make_element_by_type<trade_flow_used_by_listbox>(state, id);
		} else if(name == "may_be_used_by_listbox") {
			return make_element_by_type<trade_flow_may_be_used_by_listbox>(state, id);
		} else {
			return nullptr;
		}
	}
};

template<sys::commodity_group Group>
class trade_commodity_group_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		xy_pair cell_size = state.ui_defs.gui[state.ui_state.defs_by_name.find(state.lookup_key("goods_entry_offset"))->second.definition].position;
		xy_pair offset{0, 0};
		state.world.for_each_commodity([&](dcon::commodity_id id) {
			if(sys::commodity_group(state.world.commodity_get_commodity_group(id)) != Group)
				return;
			auto ptr = make_element_by_type<trade_commodity_entry>(state, state.ui_state.defs_by_name.find(state.lookup_key("goods_entry"))->second.definition);
			ptr->commodity_id = id;
			ptr->base_data.position = offset;
			offset.x += cell_size.x;
			if(offset.x + cell_size.x - 1 >= base_data.size.x) {
				offset.x = 0;
				offset.y += cell_size.y;
				if(offset.y + cell_size.y >= base_data.size.y) {
					offset.x += cell_size.x;
					offset.y = 0;
				}
			}
			add_child_to_front(std::move(ptr));
		});
	}
};

class trade_details_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);
		Cyto::Any dt_payload = trade_details_open_window{commodity_id};
		state.ui_state.trade_subwindow->impl_get(state, dt_payload);
	}
};

class trade_confirm_trade_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		// TODO: change stockpile settings option?
		//float famount = retrieve<float>(state, parent);
		//auto cid = retrieve<dcon::commodity_id>(state, parent);
		//command::change_stockpile_settings(state, state.local_player_nation, cid, famount, false);
	}
};

class prices_line_graph : public line_graph {
public:
	prices_line_graph()
			: line_graph(32) { }

	void on_create(sys::state& state) noexcept override {
		line_graph::on_create(state);
	}

	void on_update(sys::state& state) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);

		assert(economy::price_history_length >= 32);

		std::vector<float> datapoints(32);
		auto newest_index = economy::most_recent_price_record_index(state);
		for(uint32_t i = 0; i < 32; ++i) {
			datapoints[i] = state.world.commodity_get_price_record(com, (newest_index + i + economy::price_history_length - 32) % economy::price_history_length);
		}
		set_data_points(state, datapoints);
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		line_graph::render(state, x, y);
	}
};

class price_chart_high : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		
		auto newest_index = economy::most_recent_price_record_index(state);
		float max_price = state.world.commodity_get_price_record(com, newest_index);

		for(int32_t i = 1; i < 32; ++i) {
			max_price = std::max(state.world.commodity_get_price_record(com, (newest_index + i + economy::price_history_length - 32) % economy::price_history_length), max_price);
		}
		set_text(state, text::format_money(max_price));
	}
};

class price_chart_low : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		
		auto newest_index = economy::most_recent_price_record_index(state);
		float min_price = state.world.commodity_get_price_record(com, newest_index);

		for(int32_t i = 1; i < 32; ++i) {
			min_price = std::min(state.world.commodity_get_price_record(com, (newest_index + i + economy::price_history_length - 32) % economy::price_history_length), min_price);
		}
		set_text(state, text::format_money(min_price));
	}
};

class stockpile_sell_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		if(state.world.nation_get_drawing_on_stockpiles(state.local_player_nation, com)) {
			frame = 1;
		} else {
			frame = 0;
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		button_element_base::render(state, x, y);
	}

	void button_action(sys::state& state) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		command::change_stockpile_settings(state, state.local_player_nation, com, state.world.nation_get_stockpile_targets(state.local_player_nation, com), !state.world.nation_get_drawing_on_stockpiles(state.local_player_nation, com));
	}
};

class stockpile_sell_label : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		set_text(state, text::produce_simple_string(state, state.world.nation_get_drawing_on_stockpiles(state.local_player_nation, com) ? "trade_use" : "trade_fill"));
	}
};

class stockpile_slider_label : public simple_text_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		set_text(state, text::produce_simple_string(state, "trade_stockpile_target"));
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		simple_text_element_base::render(state, x, y);
	}
};

class stockpile_amount_label : public simple_text_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		
	}
	void on_update(sys::state& state) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		set_text(state, text::produce_simple_string(state, "trade_stockpile_current") + text::format_float(state.world.nation_get_stockpiles(state.local_player_nation, com), 1));
	}
};

class detail_domestic_production : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::val, text::pretty_integer{int64_t(state.world.nation_get_domestic_market_pool(state.local_player_nation, com))});
		set_text(state, text::resolve_string_substitution(state, "produced_detail_remove", m));
	}
};

struct stockpile_target_change {
	float value;
};
struct get_stockpile_target {
	float value;
};

class trade_slider : public scrollbar {
public:
	void on_value_change(sys::state& state, int32_t v) noexcept final {
		float a = std::pow(10.0f, float(v) * (6.0f / 2000.0f)) - 1.0f;
		send(state, parent, stockpile_target_change{a});
		if(state.ui_state.drag_target != slider)
			commit_changes(state);
	}

	void on_update(sys::state& state) noexcept final {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		if(!com)
			return;

		if(state.ui_state.drag_target == slider) {
			
		} else {
			auto value = state.world.nation_get_stockpile_targets(state.local_player_nation, com);
			auto a = std::log10(value + 1.0f);
			auto b = a * (2000.0f / 6.0f);
			update_raw_value(state, int32_t(b));

			send(state, parent, stockpile_target_change{value});
		}
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		scrollbar::impl_render(state, x, y);
	}
	void on_drag_finish(sys::state& state) noexcept override {
		commit_changes(state);
	}
	void commit_changes(sys::state& state) noexcept {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		float v = std::pow(10.0f, float(raw_value()) * (6.0f / 2000.0f)) - 1.0f;
		command::change_stockpile_settings(state, state.local_player_nation, com, v, state.world.nation_get_drawing_on_stockpiles(state.local_player_nation, com));
	}
};

class trade_slider_amount : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto com = retrieve<dcon::commodity_id>(state, parent);
		auto val = retrieve<get_stockpile_target>(state, parent);
		set_text(state, text::prettify(int64_t(val.value)));
	}
};


class trade_details_window : public window_element_base {
	simple_text_element_base* slider_value_display = nullptr;

public:
	float trade_amount = 0.0f;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "trade_flow_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "goods_icon") {
			return make_element_by_type<commodity_image>(state, id);
		} else if(name == "goods_title") {
			return make_element_by_type<generic_name_text<dcon::commodity_id>>(state, id);
		} else if(name == "goods_price") {
			return make_element_by_type<commodity_price_text>(state, id);
		} else if(name == "automate_label") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "automate") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "price_linechart") {
			return make_element_by_type<prices_line_graph>(state, id);
		} else if(name == "price_chart_low") {
			return make_element_by_type<price_chart_low>(state, id);
		} else if(name == "price_chart_high") {
			return make_element_by_type<price_chart_high>(state, id);
		} else if(name == "price_chart_time") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "sell_stockpile") {
			return make_element_by_type<stockpile_sell_button>(state, id);
		} else if(name == "sell_stockpile_label") {
			return make_element_by_type<stockpile_sell_label>(state, id);
		} else if(name == "sell_slidier_desc") {
			return make_element_by_type<stockpile_slider_label>(state, id);
		} else if(name == "sell_slider") {
			return make_element_by_type<trade_slider>(state, id);
		} else if(name == "slider_value") {
			auto ptr = make_element_by_type<trade_slider_amount>(state, id);
			slider_value_display = ptr.get();
			return ptr;
		} else if(name == "confirm_trade") {
			return make_element_by_type<stockpile_amount_label>(state, id);
		} else if(name == "goods_details") {
			return make_element_by_type<trade_details_button>(state, id);
		} else if(name == "goods_need_gov_desc") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "goods_need_factory_desc") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "produced_detail_desc") {
			return make_element_by_type<detail_domestic_production>(state, id);
		} else if(name == "goods_need_pop_desc") {
			return make_element_by_type<invisible_element>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<get_stockpile_target>()) {
			payload = get_stockpile_target{trade_amount};
			return message_result::consumed;
		} else if(payload.holds_type<stockpile_target_change>()) {
			trade_amount = any_cast<stockpile_target_change>(payload).value;
			slider_value_display->impl_on_update(state);
			return message_result::consumed;
		}

		return message_result::unseen;
	}
};

class stockpile_buy_from_stockpile_hint : public button_element_base {
	uint8_t index = 0;
	uint8_t subindex = 0;
	uint8_t click_amount = 0;
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		subindex = 0;
		if(state.network_mode != sys::network_mode_type::single_player) {
			index = 3;
		} else {
			index = uint8_t(state.game_seed % 3);
		}
	}

	void on_update(sys::state& state) noexcept override {

	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(index == 1 && subindex == 2) {
			return; //no render
		}
		button_element_base::render(state, x, y);
	}

	void button_action(sys::state& state) noexcept override {
		if(index == 1) {
			click_amount++;
			if(click_amount >= 10)
				subindex = 1;
			if(click_amount >= 15)
				subindex = 2;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		std::string key = "alice_stockpile_button_" + std::to_string(index) + "_" + std::to_string(subindex);
		text::add_line(state, contents, key);
	}
};

class trade_window : public window_element_base {
	trade_flow_window* trade_flow_win = nullptr;
	trade_details_window* details_win = nullptr;
	dcon::commodity_id commodity_id{1};

	trade_market_activity_listbox* list_ma = nullptr;
	trade_stockpile_listbox* list_sp = nullptr;
	trade_common_market_listbox* list_cm = nullptr;
	trade_government_needs_listbox* list_gn = nullptr;
	trade_factory_needs_listbox* list_fn = nullptr;
	trade_pop_needs_listbox* list_pn = nullptr;

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		auto btn = make_element_by_type<stockpile_buy_from_stockpile_hint>(state, state.ui_state.defs_by_name.find(state.lookup_key("alice_buy_from_stockpile"))->second.definition);
		add_child_to_front(std::move(btn));

		auto ptr = make_element_by_type<trade_flow_window>(state, state.ui_state.defs_by_name.find(state.lookup_key("trade_flow"))->second.definition);
		trade_flow_win = ptr.get();
		add_child_to_front(std::move(ptr));

		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "main_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "bg_trade") {
			return make_element_by_type<opaque_element_base>(state, id);
		} else if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "market_activity_list") {
			auto ptr = make_element_by_type<trade_market_activity_listbox>(state, id);
			list_ma = ptr.get();
			return ptr;
		} else if(name == "common_market_list") {
			auto ptr = make_element_by_type<trade_common_market_listbox>(state, id);
			list_cm = ptr.get();
			return ptr;
		} else if(name == "stockpile_list") {
			auto ptr = make_element_by_type<trade_stockpile_listbox>(state, id);
			list_sp = ptr.get();
			return ptr;
		} else if(name == "group_raw_material_goods") {
			return make_element_by_type<trade_commodity_group_window<sys::commodity_group::raw_material_goods>>(state, id);
		} else if(name == "group_industrial_goods") {
			return make_element_by_type<trade_commodity_group_window<sys::commodity_group::industrial_goods>>(state, id);
		} else if(name == "group_consumer_goods") {
			return make_element_by_type<trade_commodity_group_window<sys::commodity_group::consumer_goods>>(state, id);
		} else if(name == "group_military_goods") {
			return make_element_by_type<trade_commodity_group_window<sys::commodity_group::military_goods>>(state, id);
		} else if(name == "government_needs_list") {
			auto ptr = make_element_by_type<trade_government_needs_listbox>(state, id);
			list_gn = ptr.get();
			return ptr;
		} else if(name == "factory_needs_list") {
			auto ptr = make_element_by_type<trade_factory_needs_listbox>(state, id);
			list_fn = ptr.get();
			return ptr;
		} else if(name == "pop_needs_list") {
			auto ptr = make_element_by_type<trade_pop_needs_listbox>(state, id);
			list_pn = ptr.get();
			return ptr;
		} else if(name == "trade_details") {
			auto ptr = make_element_by_type<trade_details_window>(state, id);
			details_win = ptr.get();
			return ptr;
		} else if(name == "market_activity_sort_by_goods") {
			return make_element_by_type<trade_sort_button<trade_sort::commodity, trade_sort_assoc::market_activity>>(state, id);
		} else if(name == "market_activity_sort_by_activity") {
			return make_element_by_type<trade_sort_button<trade_sort::demand_satisfaction, trade_sort_assoc::market_activity>>(state, id);
		} else if(name == "market_activity_sort_by_cost") {
			return make_element_by_type<trade_sort_button<trade_sort::price, trade_sort_assoc::market_activity>>(state, id);
		} else if(name == "stockpile_sort_by_goods") {
			return make_element_by_type<trade_sort_button<trade_sort::commodity, trade_sort_assoc::stockpile>>(state, id);
		} else if(name == "stockpile_sort_by_value") {
			return make_element_by_type<trade_sort_button<trade_sort::stockpile, trade_sort_assoc::stockpile>>(state, id);
		} else if(name == "stockpile_sort_by_change") {
			return make_element_by_type<trade_sort_button<trade_sort::stockpile_change, trade_sort_assoc::stockpile>>(state, id);
		} else if(name == "common_market_sort_by_goods") {
			return make_element_by_type<trade_sort_button<trade_sort::commodity, trade_sort_assoc::common_market>>(state, id);
		} else if(name == "common_market_sort_by_produced") {
			return make_element_by_type<trade_sort_button<trade_sort::global_market_pool, trade_sort_assoc::common_market>>(state, id);
		} else if(name == "common_market_sort_by_diff") {
			return make_element_by_type<trade_sort_button<trade_sort::real_demand, trade_sort_assoc::common_market>>(state, id);
		} else if(name == "common_market_sort_by_exported") {
			return make_element_by_type<trade_sort_button<trade_sort::domestic_market, trade_sort_assoc::common_market>>(state, id);
		} else if(name == "needs_government_sort_by_goods") {
			return make_element_by_type<trade_sort_button<trade_sort::commodity, trade_sort_assoc::needs_government>>(state, id);
		} else if(name == "needs_government_sort_by_value") {
			return make_element_by_type<trade_sort_button<trade_sort::needs, trade_sort_assoc::needs_government>>(state, id);
		} else if(name == "needs_factories_sort_by_goods") {
			return make_element_by_type<trade_sort_button<trade_sort::commodity, trade_sort_assoc::needs_factories>>(state, id);
		} else if(name == "needs_factories_sort_by_value") {
			return make_element_by_type<trade_sort_button<trade_sort::needs, trade_sort_assoc::needs_factories>>(state, id);
		} else if(name == "needs_pops_sort_by_goods") {
			return make_element_by_type<trade_sort_button<trade_sort::commodity, trade_sort_assoc::needs_pops>>(state, id);
		} else if(name == "needs_pops_sort_by_value") {
			return make_element_by_type<trade_sort_button<trade_sort::needs, trade_sort_assoc::needs_pops>>(state, id);
		// Non-vanila
		} else if(name == "group_industrial_and_consumer_goods") {
			return make_element_by_type<trade_commodity_group_window<sys::commodity_group::industrial_and_consumer_goods>>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		// Special message rebroadcasted by the details button from the hierarchy
		if(payload.holds_type<dcon::commodity_id>()) {
			payload.emplace<dcon::commodity_id>(commodity_id);
			return message_result::consumed;
		} else if(payload.holds_type<trade_details_open_window>()) {
			commodity_id = any_cast<trade_details_open_window>(payload).commodity_id;
			trade_flow_win->set_visible(state, true);
			trade_flow_win->impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<trade_details_select_commodity>()) {
			commodity_id = any_cast<trade_details_select_commodity>(payload).commodity_id;
			if(commodity_id)
				details_win->trade_amount = state.world.nation_get_stockpile_targets(state.local_player_nation, commodity_id);
			details_win->impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(state.local_player_nation);
			return message_result::consumed;
		} else if(payload.holds_type<trade_sort_data>()) {
			auto d = any_cast<trade_sort_data>(payload);
			switch(d.assoc) {
			case trade_sort_assoc::market_activity:
				if(list_ma->sort == d.sort)
					list_ma->sort_ascend = !list_ma->sort_ascend;
				else
					list_ma->sort_ascend = false;
				list_ma->sort = d.sort;
				list_ma->impl_on_update(state);
				break;
			case trade_sort_assoc::stockpile:
				if(list_sp->sort == d.sort)
					list_sp->sort_ascend = !list_sp->sort_ascend;
				else
					list_sp->sort_ascend = false;
				list_sp->sort = d.sort;
				list_sp->impl_on_update(state);
				break;
			case trade_sort_assoc::common_market:
				if(list_cm->sort == d.sort)
					list_cm->sort_ascend = !list_cm->sort_ascend;
				else
					list_cm->sort_ascend = false;
				list_cm->sort = d.sort;
				list_cm->impl_on_update(state);
				break;
			case trade_sort_assoc::needs_government:
				if(list_gn->sort == d.sort)
					list_gn->sort_ascend = !list_gn->sort_ascend;
				else
					list_gn->sort_ascend = false;
				list_gn->sort = d.sort;
				list_gn->impl_on_update(state);
				break;
			case trade_sort_assoc::needs_factories:
				if(list_fn->sort == d.sort)
					list_fn->sort_ascend = !list_fn->sort_ascend;
				else
					list_fn->sort_ascend = false;
				list_fn->sort = d.sort;
				list_fn->impl_on_update(state);
				break;
			case trade_sort_assoc::needs_pops:
				if(list_pn->sort == d.sort)
					list_pn->sort_ascend = !list_pn->sort_ascend;
				else
					list_pn->sort_ascend = false;
				list_pn->sort = d.sort;
				list_pn->impl_on_update(state);
				break;
			default:
				break;
			}
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
