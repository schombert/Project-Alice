namespace alice_ui {
struct pop_budget_details_main_unemployment_value_t;
struct pop_budget_details_main_pension_value_t;
struct pop_budget_details_main_rgo_value_t;
struct pop_budget_details_main_nation_value_t;
struct pop_budget_details_main_trade_value_t;
struct pop_budget_details_main_artisan_value_t;
struct pop_budget_details_main_tax_s_value_t;
struct pop_budget_details_main_education_s_value_t;
struct pop_budget_details_main_trade_s_value_t;
struct pop_budget_details_main_investment_s_value_t;
struct pop_budget_details_main_needs_s_value_t;
struct pop_budget_details_main_t;
struct pop_budget_details_wage_income_labor_type_t;
struct pop_budget_details_wage_income_labor_ratio_t;
struct pop_budget_details_wage_income_labor_wage_t;
struct pop_budget_details_wage_income_t;
struct pop_budget_details_production_income_commodity_t;
struct pop_budget_details_production_income_income_t;
struct pop_budget_details_production_income_t;
struct pop_budget_details_consumption_name_t;
struct pop_budget_details_consumption_satisfaction_t;
struct pop_budget_details_consumption_cost_t;
struct pop_budget_details_consumption_t;
struct pop_budget_details_main_unemployment_value_t : public alice_ui::template_label {
// BEGIN main::unemployment_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_budget_details_main_pension_value_t : public alice_ui::template_label {
// BEGIN main::pension_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_budget_details_main_rgo_value_t : public alice_ui::template_label {
// BEGIN main::rgo_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_budget_details_main_nation_value_t : public alice_ui::template_label {
// BEGIN main::nation_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_budget_details_main_trade_value_t : public alice_ui::template_label {
// BEGIN main::trade_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_budget_details_main_artisan_value_t : public alice_ui::template_label {
// BEGIN main::artisan_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_budget_details_main_tax_s_value_t : public alice_ui::template_label {
// BEGIN main::tax_s_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_budget_details_main_education_s_value_t : public alice_ui::template_label {
// BEGIN main::education_s_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_budget_details_main_trade_s_value_t : public alice_ui::template_label {
// BEGIN main::trade_s_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_budget_details_main_investment_s_value_t : public alice_ui::template_label {
// BEGIN main::investment_s_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_budget_details_main_needs_s_value_t : public alice_ui::template_label {
// BEGIN main::needs_s_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_budget_details_main_wage_per_labor_t : public layout_generator {
// BEGIN main::wage_per_labor::variables
// END
	struct wage_income_option { int32_t labor_type_index; float ratio; float wage; };
	std::vector<std::unique_ptr<ui::element_base>> wage_income_pool;
	int32_t wage_income_pool_used = 0;
	void add_wage_income( int32_t labor_type_index,  float ratio,  float wage);
	std::vector<std::variant<std::monostate, wage_income_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct pop_budget_details_main_needs_t : public layout_generator {
// BEGIN main::needs::variables
// END
	struct consumption_option { dcon::commodity_id cid; uint8_t category; float cost_val; float satisfaction_val; };
	std::vector<std::unique_ptr<ui::element_base>> consumption_pool;
	int32_t consumption_pool_used = 0;
	void add_consumption( dcon::commodity_id cid,  uint8_t category,  float cost_val,  float satisfaction_val);
	std::vector<std::variant<std::monostate, consumption_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct pop_budget_details_wage_income_labor_type_t : public alice_ui::template_label {
// BEGIN wage_income::labor_type::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_budget_details_wage_income_labor_ratio_t : public alice_ui::template_label {
// BEGIN wage_income::labor_ratio::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_budget_details_wage_income_labor_wage_t : public alice_ui::template_label {
// BEGIN wage_income::labor_wage::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_budget_details_production_income_commodity_t : public alice_ui::template_label {
// BEGIN production_income::commodity::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_budget_details_production_income_income_t : public alice_ui::template_label {
// BEGIN production_income::income::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_budget_details_consumption_name_t : public alice_ui::template_label {
// BEGIN consumption::name::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_budget_details_consumption_satisfaction_t : public alice_ui::template_label {
// BEGIN consumption::satisfaction::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_budget_details_consumption_cost_t : public alice_ui::template_label {
// BEGIN consumption::cost::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_budget_details_main_t : public layout_window_element {
// BEGIN main::variables
// END
	dcon::pop_id for_pop;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<template_label> header;
	std::unique_ptr<template_label> header_income;
	std::unique_ptr<template_label> header_spending;
	std::unique_ptr<template_bg_graphic> fancy_bar_1;
	std::unique_ptr<template_bg_graphic> fancy_bar_2;
	std::unique_ptr<template_bg_graphic> fancy_bar_3;
	std::unique_ptr<template_bg_graphic> fancy_bar_4;
	std::unique_ptr<template_bg_graphic> fancy_bar_5;
	std::unique_ptr<template_bg_graphic> fancy_bar_6;
	std::unique_ptr<template_bg_graphic> fancy_bar_7;
	std::unique_ptr<template_bg_graphic> fancy_bar_8;
	std::unique_ptr<template_label> unemployment_label;
	std::unique_ptr<template_label> pension_label;
	std::unique_ptr<template_label> rgo_label;
	std::unique_ptr<template_label> nation_label;
	std::unique_ptr<template_label> trade_label;
	std::unique_ptr<pop_budget_details_main_unemployment_value_t> unemployment_value;
	std::unique_ptr<pop_budget_details_main_pension_value_t> pension_value;
	std::unique_ptr<pop_budget_details_main_rgo_value_t> rgo_value;
	std::unique_ptr<pop_budget_details_main_nation_value_t> nation_value;
	std::unique_ptr<pop_budget_details_main_trade_value_t> trade_value;
	std::unique_ptr<template_label> wage_label;
	std::unique_ptr<template_label> business_label;
	std::unique_ptr<template_label> artisan_label;
	std::unique_ptr<pop_budget_details_main_artisan_value_t> artisan_value;
	std::unique_ptr<template_bg_graphic> simple_bar_1;
	std::unique_ptr<template_label> tax_s_label;
	std::unique_ptr<template_label> education_s_label;
	std::unique_ptr<template_label> trade_s_label;
	std::unique_ptr<template_label> investment_s_label;
	std::unique_ptr<pop_budget_details_main_tax_s_value_t> tax_s_value;
	std::unique_ptr<pop_budget_details_main_education_s_value_t> education_s_value;
	std::unique_ptr<pop_budget_details_main_trade_s_value_t> trade_s_value;
	std::unique_ptr<pop_budget_details_main_investment_s_value_t> investment_s_value;
	std::unique_ptr<template_label> needs_s_label;
	std::unique_ptr<pop_budget_details_main_needs_s_value_t> needs_s_value;
	pop_budget_details_main_wage_per_labor_t wage_per_labor;
	pop_budget_details_main_needs_t needs;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::consumed;
	}
	void on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		auto location_abs = get_absolute_location(state, *this);
		if(location_abs.x <= oldx && oldx < base_data.size.x + location_abs.x && location_abs.y <= oldy && oldy < base_data.size.y + location_abs.y) {
			ui::xy_pair new_abs_pos = location_abs;
			new_abs_pos.x += int16_t(x - oldx);
			new_abs_pos.y += int16_t(y - oldy);
			if(ui::ui_width(state) > base_data.size.x)
				new_abs_pos.x = int16_t(std::clamp(int32_t(new_abs_pos.x), 0, ui::ui_width(state) - base_data.size.x));
			if(ui::ui_height(state) > base_data.size.y)
				new_abs_pos.y = int16_t(std::clamp(int32_t(new_abs_pos.y), 0, ui::ui_height(state) - base_data.size.y));
			if(state_is_rtl(state)) {
				base_data.position.x -= int16_t(new_abs_pos.x - location_abs.x);
			} else {
				base_data.position.x += int16_t(new_abs_pos.x - location_abs.x);
			}
		base_data.position.y += int16_t(new_abs_pos.y - location_abs.y);
		}
	}
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "for_pop") {
			return (void*)(&for_pop);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_pop_budget_details_main(sys::state& state);
struct pop_budget_details_wage_income_t : public layout_window_element {
// BEGIN wage_income::variables
// END
	int32_t labor_type_index;
	float ratio;
	float wage;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<pop_budget_details_wage_income_labor_type_t> labor_type;
	std::unique_ptr<pop_budget_details_wage_income_labor_ratio_t> labor_ratio;
	std::unique_ptr<pop_budget_details_wage_income_labor_wage_t> labor_wage;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "labor_type_index") {
			return (void*)(&labor_type_index);
		}
		if(name_parameter == "ratio") {
			return (void*)(&ratio);
		}
		if(name_parameter == "wage") {
			return (void*)(&wage);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_pop_budget_details_wage_income(sys::state& state);
struct pop_budget_details_production_income_t : public layout_window_element {
// BEGIN production_income::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<pop_budget_details_production_income_commodity_t> commodity;
	std::unique_ptr<pop_budget_details_production_income_income_t> income;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_pop_budget_details_production_income(sys::state& state);
struct pop_budget_details_consumption_t : public layout_window_element {
// BEGIN consumption::variables
// END
	dcon::commodity_id cid;
	uint8_t category;
	float cost_val;
	float satisfaction_val;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<pop_budget_details_consumption_name_t> name;
	std::unique_ptr<pop_budget_details_consumption_satisfaction_t> satisfaction;
	std::unique_ptr<pop_budget_details_consumption_cost_t> cost;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "cid") {
			return (void*)(&cid);
		}
		if(name_parameter == "category") {
			return (void*)(&category);
		}
		if(name_parameter == "cost_val") {
			return (void*)(&cost_val);
		}
		if(name_parameter == "satisfaction_val") {
			return (void*)(&satisfaction_val);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_pop_budget_details_consumption(sys::state& state);
void pop_budget_details_main_wage_per_labor_t::add_wage_income(int32_t labor_type_index, float ratio, float wage) {
	values.emplace_back(wage_income_option{labor_type_index, ratio, wage});
}
void  pop_budget_details_main_wage_per_labor_t::on_create(sys::state& state, layout_window_element* parent) {
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent)); 
// BEGIN main::wage_per_labor::on_create
// END
}
void  pop_budget_details_main_wage_per_labor_t::update(sys::state& state, layout_window_element* parent) {
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent)); 
// BEGIN main::wage_per_labor::update
	values.clear();
	auto list = economy::pops::estimate_wage(state, main.for_pop);
	for(auto& item : list) {
		add_wage_income(item.labor_type, item.ratio, item.wage);
	}
// END
}
measure_result  pop_budget_details_main_wage_per_labor_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<wage_income_option>(values[index])) {
		if(wage_income_pool.empty()) wage_income_pool.emplace_back(make_pop_budget_details_wage_income(state));
		if(destination) {
			if(wage_income_pool.size() <= size_t(wage_income_pool_used)) wage_income_pool.emplace_back(make_pop_budget_details_wage_income(state));
			wage_income_pool[wage_income_pool_used]->base_data.position.x = int16_t(x);
			wage_income_pool[wage_income_pool_used]->base_data.position.y = int16_t(y);
			wage_income_pool[wage_income_pool_used]->parent = destination;
			destination->children.push_back(wage_income_pool[wage_income_pool_used].get());
			((pop_budget_details_wage_income_t*)(wage_income_pool[wage_income_pool_used].get()))->labor_type_index = std::get<wage_income_option>(values[index]).labor_type_index;
			((pop_budget_details_wage_income_t*)(wage_income_pool[wage_income_pool_used].get()))->ratio = std::get<wage_income_option>(values[index]).ratio;
			((pop_budget_details_wage_income_t*)(wage_income_pool[wage_income_pool_used].get()))->wage = std::get<wage_income_option>(values[index]).wage;
			((pop_budget_details_wage_income_t*)(wage_income_pool[wage_income_pool_used].get()))->set_alternate(alternate);
			wage_income_pool[wage_income_pool_used]->impl_on_update(state);
			wage_income_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ wage_income_pool[0]->base_data.size.x, wage_income_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  pop_budget_details_main_wage_per_labor_t::reset_pools() {
	wage_income_pool_used = 0;
}
void pop_budget_details_main_needs_t::add_consumption(dcon::commodity_id cid, uint8_t category, float cost_val, float satisfaction_val) {
	values.emplace_back(consumption_option{cid, category, cost_val, satisfaction_val});
}
void  pop_budget_details_main_needs_t::on_create(sys::state& state, layout_window_element* parent) {
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent)); 
// BEGIN main::needs::on_create
// END
}
void  pop_budget_details_main_needs_t::update(sys::state& state, layout_window_element* parent) {
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent)); 
// BEGIN main::needs::update
	values.clear();

	auto pid = state.world.pop_get_province_from_pop_location(main.for_pop);
	auto nation = state.world.province_get_nation_from_province_ownership(pid);
	auto zone = state.world.province_get_state_membership(pid);
	auto market = state.world.state_instance_get_market_from_local_market(zone);

	// life
	state.world.for_each_commodity([&](dcon::commodity_id cid) {
		auto cost = economy::pops::estimate_pop_spending_life(state, main.for_pop, cid);
		if(cost > 0.001f) {
			add_consumption(cid, 0, cost, state.world.market_get_actual_probability_to_buy(market, cid));
		}
	});

	// everyday
	state.world.for_each_commodity([&](dcon::commodity_id cid) {
		auto cost = economy::pops::estimate_pop_spending_everyday(state, main.for_pop, cid);
		if(cost > 0.001f) {
			add_consumption(cid, 1, cost, state.world.market_get_actual_probability_to_buy(market, cid));
		}
	});

	//luxury
	state.world.for_each_commodity([&](dcon::commodity_id cid) {
		auto cost = economy::pops::estimate_pop_spending_luxury(state, main.for_pop, cid);
		if(cost > 0.001f) {
			add_consumption(cid, 2, cost, state.world.market_get_actual_probability_to_buy(market, cid));
		}
	});
// END
}
measure_result  pop_budget_details_main_needs_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<consumption_option>(values[index])) {
		if(consumption_pool.empty()) consumption_pool.emplace_back(make_pop_budget_details_consumption(state));
		if(destination) {
			if(consumption_pool.size() <= size_t(consumption_pool_used)) consumption_pool.emplace_back(make_pop_budget_details_consumption(state));
			consumption_pool[consumption_pool_used]->base_data.position.x = int16_t(x);
			consumption_pool[consumption_pool_used]->base_data.position.y = int16_t(y);
			consumption_pool[consumption_pool_used]->parent = destination;
			destination->children.push_back(consumption_pool[consumption_pool_used].get());
			((pop_budget_details_consumption_t*)(consumption_pool[consumption_pool_used].get()))->cid = std::get<consumption_option>(values[index]).cid;
			((pop_budget_details_consumption_t*)(consumption_pool[consumption_pool_used].get()))->category = std::get<consumption_option>(values[index]).category;
			((pop_budget_details_consumption_t*)(consumption_pool[consumption_pool_used].get()))->cost_val = std::get<consumption_option>(values[index]).cost_val;
			((pop_budget_details_consumption_t*)(consumption_pool[consumption_pool_used].get()))->satisfaction_val = std::get<consumption_option>(values[index]).satisfaction_val;
			((pop_budget_details_consumption_t*)(consumption_pool[consumption_pool_used].get()))->set_alternate(alternate);
			consumption_pool[consumption_pool_used]->impl_on_update(state);
			consumption_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ consumption_pool[0]->base_data.size.x, consumption_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  pop_budget_details_main_needs_t::reset_pools() {
	consumption_pool_used = 0;
}
void pop_budget_details_main_unemployment_value_t::on_update(sys::state& state) noexcept {
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent)); 
// BEGIN main::unemployment_value::update
	auto data = economy::pops::estimate_income_from_nation(state, main.for_pop);
	set_text(state, text::format_money(data.unemployment));
// END
}
void pop_budget_details_main_pension_value_t::on_update(sys::state& state) noexcept {
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent)); 
// BEGIN main::pension_value::update
	auto data = economy::pops::estimate_income_from_nation(state, main.for_pop);
	set_text(state, text::format_money(data.pension));
// END
}
void pop_budget_details_main_rgo_value_t::on_update(sys::state& state) noexcept {
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent)); 
// BEGIN main::rgo_value::update
	set_text(state, text::format_money(economy::pops::estimate_rgo_income(state, main.for_pop)));
// END
}
void pop_budget_details_main_nation_value_t::on_update(sys::state& state) noexcept {
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent)); 
// BEGIN main::nation_value::update
	auto data = economy::pops::estimate_income_from_nation(state, main.for_pop);
	set_text(state, text::format_money(data.investment + data.military));
// END
}
void pop_budget_details_main_trade_value_t::on_update(sys::state& state) noexcept {
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent)); 
// BEGIN main::trade_value::update
	set_text(state, text::format_money(economy::pops::estimate_trade_income(state, main.for_pop)));
// END
}
void pop_budget_details_main_artisan_value_t::on_update(sys::state& state) noexcept {
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent)); 
// BEGIN main::artisan_value::update
	set_text(state, text::format_money(economy::pops::estimate_artisan_income(state, main.for_pop)));
// END
}
void pop_budget_details_main_tax_s_value_t::on_update(sys::state& state) noexcept {
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent)); 
// BEGIN main::tax_s_value::update
	auto pid = state.world.pop_get_province_from_pop_location(main.for_pop);
	auto nid = state.world.province_get_nation_from_province_control(pid);
	auto tax_rate = float(state.world.nation_get_poor_tax(nid)) / 100.f;
	auto tax_efficiency = economy::tax_collection_rate(state, nid, pid);
	set_text(state, text::format_money(economy::pops::estimate_tax_spending(state, main.for_pop, tax_rate * tax_efficiency)));
// END
}
void pop_budget_details_main_education_s_value_t::on_update(sys::state& state) noexcept {
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent)); 
// BEGIN main::education_s_value::update
	auto budget = economy::pops::prepare_pop_budget<dcon::pop_id>(state, main.for_pop);
	set_text(state, text::format_money(budget.education.spent));
// END
}
void pop_budget_details_main_trade_s_value_t::on_update(sys::state& state) noexcept {
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent)); 
// BEGIN main::trade_s_value::update
	set_text(state, text::format_money(economy::pops::estimate_trade_spending(state, main.for_pop)));
// END
}
void pop_budget_details_main_investment_s_value_t::on_update(sys::state& state) noexcept {
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent)); 
// BEGIN main::investment_s_value::update
	auto budget = economy::pops::prepare_pop_budget<dcon::pop_id>(state, main.for_pop);
	set_text(state, text::format_money(budget.investments.spent + budget.bank_savings.spent));
// END
}
void pop_budget_details_main_needs_s_value_t::on_update(sys::state& state) noexcept {
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent)); 
// BEGIN main::needs_s_value::update
	auto pid = state.world.pop_get_province_from_pop_location(main.for_pop);
	auto nation = state.world.province_get_nation_from_province_ownership(pid);
	auto zone = state.world.province_get_state_membership(pid);
	auto market = state.world.state_instance_get_market_from_local_market(zone);

	auto total = 0.f;
	// life
	state.world.for_each_commodity([&](dcon::commodity_id cid) {
		auto cost = economy::pops::estimate_pop_spending_life(state, main.for_pop, cid);
		total += cost;
	});

	// everyday
	state.world.for_each_commodity([&](dcon::commodity_id cid) {
		auto cost = economy::pops::estimate_pop_spending_everyday(state, main.for_pop, cid);
		total += cost;
	});

	//luxury
	state.world.for_each_commodity([&](dcon::commodity_id cid) {
		auto cost = economy::pops::estimate_pop_spending_luxury(state, main.for_pop, cid);
		total += cost;
	});

	set_text(state, text::format_money(total));
// END
}
ui::message_result pop_budget_details_main_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result pop_budget_details_main_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_budget_details_main_t::on_update(sys::state& state) noexcept {
// BEGIN main::update
// END
	wage_per_labor.update(state, this);
	needs.update(state, this);
	remake_layout(state, true);
}
void pop_budget_details_main_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
	serialization::in_buffer buffer(ldata, sz);
	buffer.read(lvl.size_x); 
	buffer.read(lvl.size_y); 
	buffer.read(lvl.margin_top); 
	buffer.read(lvl.margin_bottom); 
	buffer.read(lvl.margin_left); 
	buffer.read(lvl.margin_right); 
	buffer.read(lvl.line_alignment); 
	buffer.read(lvl.line_internal_alignment); 
	buffer.read(lvl.type); 
	buffer.read(lvl.page_animation); 
	buffer.read(lvl.interline_spacing); 
	buffer.read(lvl.paged); 
	if(lvl.paged) {
		lvl.page_controls = std::make_unique<page_buttons>();
		lvl.page_controls->for_layout = &lvl;
		lvl.page_controls->parent = this;
		lvl.page_controls->base_data.size.x = int16_t(grid_size * 10);
		lvl.page_controls->base_data.size.y = int16_t(grid_size * 2);
	}
	auto expansion_section = buffer.read_section();
	if(expansion_section)
		expansion_section.read(lvl.template_id);
	if(lvl.template_id == -1 && window_template != -1)
		lvl.template_id = int16_t(state.ui_templates.window_t[window_template].layout_region_definition);
	while(buffer) {
		layout_item_types t;
		buffer.read(t);
		switch(t) {
			case layout_item_types::texture_layer:
			{
				texture_layer temp;
				buffer.read(temp.texture_type);
				buffer.read(temp.texture);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::control:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				temp.ptr = nullptr;
				if(cname == "header") {
					temp.ptr = header.get();
				} else
				if(cname == "header_income") {
					temp.ptr = header_income.get();
				} else
				if(cname == "header_spending") {
					temp.ptr = header_spending.get();
				} else
				if(cname == "fancy_bar_1") {
					temp.ptr = fancy_bar_1.get();
				} else
				if(cname == "fancy_bar_2") {
					temp.ptr = fancy_bar_2.get();
				} else
				if(cname == "fancy_bar_3") {
					temp.ptr = fancy_bar_3.get();
				} else
				if(cname == "fancy_bar_4") {
					temp.ptr = fancy_bar_4.get();
				} else
				if(cname == "fancy_bar_5") {
					temp.ptr = fancy_bar_5.get();
				} else
				if(cname == "fancy_bar_6") {
					temp.ptr = fancy_bar_6.get();
				} else
				if(cname == "fancy_bar_7") {
					temp.ptr = fancy_bar_7.get();
				} else
				if(cname == "fancy_bar_8") {
					temp.ptr = fancy_bar_8.get();
				} else
				if(cname == "unemployment_label") {
					temp.ptr = unemployment_label.get();
				} else
				if(cname == "pension_label") {
					temp.ptr = pension_label.get();
				} else
				if(cname == "rgo_label") {
					temp.ptr = rgo_label.get();
				} else
				if(cname == "nation_label") {
					temp.ptr = nation_label.get();
				} else
				if(cname == "trade_label") {
					temp.ptr = trade_label.get();
				} else
				if(cname == "unemployment_value") {
					temp.ptr = unemployment_value.get();
				} else
				if(cname == "pension_value") {
					temp.ptr = pension_value.get();
				} else
				if(cname == "rgo_value") {
					temp.ptr = rgo_value.get();
				} else
				if(cname == "nation_value") {
					temp.ptr = nation_value.get();
				} else
				if(cname == "trade_value") {
					temp.ptr = trade_value.get();
				} else
				if(cname == "wage_label") {
					temp.ptr = wage_label.get();
				} else
				if(cname == "business_label") {
					temp.ptr = business_label.get();
				} else
				if(cname == "artisan_label") {
					temp.ptr = artisan_label.get();
				} else
				if(cname == "artisan_value") {
					temp.ptr = artisan_value.get();
				} else
				if(cname == "simple_bar_1") {
					temp.ptr = simple_bar_1.get();
				} else
				if(cname == "tax_s_label") {
					temp.ptr = tax_s_label.get();
				} else
				if(cname == "education_s_label") {
					temp.ptr = education_s_label.get();
				} else
				if(cname == "trade_s_label") {
					temp.ptr = trade_s_label.get();
				} else
				if(cname == "investment_s_label") {
					temp.ptr = investment_s_label.get();
				} else
				if(cname == "tax_s_value") {
					temp.ptr = tax_s_value.get();
				} else
				if(cname == "education_s_value") {
					temp.ptr = education_s_value.get();
				} else
				if(cname == "trade_s_value") {
					temp.ptr = trade_s_value.get();
				} else
				if(cname == "investment_s_value") {
					temp.ptr = investment_s_value.get();
				} else
				if(cname == "needs_s_label") {
					temp.ptr = needs_s_label.get();
				} else
				if(cname == "needs_s_value") {
					temp.ptr = needs_s_value.get();
				} else
				{
					std::string str_cname {cname};
					auto found = scripted_elements.find(str_cname);
					if (found != scripted_elements.end()) {
						temp.ptr = found->second.get();
					}
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::window:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				if(cname == "main") {
					temp.ptr = make_pop_budget_details_main(state);
				}
				if(cname == "wage_income") {
					temp.ptr = make_pop_budget_details_wage_income(state);
				}
				if(cname == "production_income") {
					temp.ptr = make_pop_budget_details_production_income(state);
				}
				if(cname == "consumption") {
					temp.ptr = make_pop_budget_details_consumption(state);
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::glue:
			{
				layout_glue temp;
				buffer.read(temp.type);
				buffer.read(temp.amount);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::generator:
			{
				generator_instance temp;
				std::string_view cname = buffer.read<std::string_view>();
				auto gen_details = buffer.read_section(); // ignored
				if(cname == "wage_per_labor") {
					temp.generator = &wage_per_labor;
				}
				if(cname == "needs") {
					temp.generator = &needs;
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::layout:
			{
				sub_layout temp;
				temp.layout = std::make_unique<layout_level>();
				auto layout_section = buffer.read_section();
				create_layout_level(state, *temp.layout, layout_section.view_data() + layout_section.view_read_position(), layout_section.view_size() - layout_section.view_read_position());
				lvl.contents.emplace_back(std::move(temp));
			} break;
		}
	}
}
void pop_budget_details_main_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("pop_budget_details::main"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	layout_window_element::initialize_template(state, win_data.template_id, win_data.grid_size, win_data.auto_close_button);
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "header") {
			header = std::make_unique<template_label>();
			header->parent = this;
			auto cptr = header.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "header_income") {
			header_income = std::make_unique<template_label>();
			header_income->parent = this;
			auto cptr = header_income.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "header_spending") {
			header_spending = std::make_unique<template_label>();
			header_spending->parent = this;
			auto cptr = header_spending.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "fancy_bar_1") {
			fancy_bar_1 = std::make_unique<template_bg_graphic>();
			fancy_bar_1->parent = this;
			auto cptr = fancy_bar_1.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "fancy_bar_2") {
			fancy_bar_2 = std::make_unique<template_bg_graphic>();
			fancy_bar_2->parent = this;
			auto cptr = fancy_bar_2.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "fancy_bar_3") {
			fancy_bar_3 = std::make_unique<template_bg_graphic>();
			fancy_bar_3->parent = this;
			auto cptr = fancy_bar_3.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "fancy_bar_4") {
			fancy_bar_4 = std::make_unique<template_bg_graphic>();
			fancy_bar_4->parent = this;
			auto cptr = fancy_bar_4.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "fancy_bar_5") {
			fancy_bar_5 = std::make_unique<template_bg_graphic>();
			fancy_bar_5->parent = this;
			auto cptr = fancy_bar_5.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "fancy_bar_6") {
			fancy_bar_6 = std::make_unique<template_bg_graphic>();
			fancy_bar_6->parent = this;
			auto cptr = fancy_bar_6.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "fancy_bar_7") {
			fancy_bar_7 = std::make_unique<template_bg_graphic>();
			fancy_bar_7->parent = this;
			auto cptr = fancy_bar_7.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "fancy_bar_8") {
			fancy_bar_8 = std::make_unique<template_bg_graphic>();
			fancy_bar_8->parent = this;
			auto cptr = fancy_bar_8.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "unemployment_label") {
			unemployment_label = std::make_unique<template_label>();
			unemployment_label->parent = this;
			auto cptr = unemployment_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "pension_label") {
			pension_label = std::make_unique<template_label>();
			pension_label->parent = this;
			auto cptr = pension_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "rgo_label") {
			rgo_label = std::make_unique<template_label>();
			rgo_label->parent = this;
			auto cptr = rgo_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "nation_label") {
			nation_label = std::make_unique<template_label>();
			nation_label->parent = this;
			auto cptr = nation_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "trade_label") {
			trade_label = std::make_unique<template_label>();
			trade_label->parent = this;
			auto cptr = trade_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "unemployment_value") {
			unemployment_value = std::make_unique<pop_budget_details_main_unemployment_value_t>();
			unemployment_value->parent = this;
			auto cptr = unemployment_value.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "pension_value") {
			pension_value = std::make_unique<pop_budget_details_main_pension_value_t>();
			pension_value->parent = this;
			auto cptr = pension_value.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "rgo_value") {
			rgo_value = std::make_unique<pop_budget_details_main_rgo_value_t>();
			rgo_value->parent = this;
			auto cptr = rgo_value.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "nation_value") {
			nation_value = std::make_unique<pop_budget_details_main_nation_value_t>();
			nation_value->parent = this;
			auto cptr = nation_value.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "trade_value") {
			trade_value = std::make_unique<pop_budget_details_main_trade_value_t>();
			trade_value->parent = this;
			auto cptr = trade_value.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "wage_label") {
			wage_label = std::make_unique<template_label>();
			wage_label->parent = this;
			auto cptr = wage_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "business_label") {
			business_label = std::make_unique<template_label>();
			business_label->parent = this;
			auto cptr = business_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "artisan_label") {
			artisan_label = std::make_unique<template_label>();
			artisan_label->parent = this;
			auto cptr = artisan_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "artisan_value") {
			artisan_value = std::make_unique<pop_budget_details_main_artisan_value_t>();
			artisan_value->parent = this;
			auto cptr = artisan_value.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "simple_bar_1") {
			simple_bar_1 = std::make_unique<template_bg_graphic>();
			simple_bar_1->parent = this;
			auto cptr = simple_bar_1.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "tax_s_label") {
			tax_s_label = std::make_unique<template_label>();
			tax_s_label->parent = this;
			auto cptr = tax_s_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "education_s_label") {
			education_s_label = std::make_unique<template_label>();
			education_s_label->parent = this;
			auto cptr = education_s_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "trade_s_label") {
			trade_s_label = std::make_unique<template_label>();
			trade_s_label->parent = this;
			auto cptr = trade_s_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "investment_s_label") {
			investment_s_label = std::make_unique<template_label>();
			investment_s_label->parent = this;
			auto cptr = investment_s_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "tax_s_value") {
			tax_s_value = std::make_unique<pop_budget_details_main_tax_s_value_t>();
			tax_s_value->parent = this;
			auto cptr = tax_s_value.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "education_s_value") {
			education_s_value = std::make_unique<pop_budget_details_main_education_s_value_t>();
			education_s_value->parent = this;
			auto cptr = education_s_value.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "trade_s_value") {
			trade_s_value = std::make_unique<pop_budget_details_main_trade_s_value_t>();
			trade_s_value->parent = this;
			auto cptr = trade_s_value.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "investment_s_value") {
			investment_s_value = std::make_unique<pop_budget_details_main_investment_s_value_t>();
			investment_s_value->parent = this;
			auto cptr = investment_s_value.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "needs_s_label") {
			needs_s_label = std::make_unique<template_label>();
			needs_s_label->parent = this;
			auto cptr = needs_s_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "needs_s_value") {
			needs_s_value = std::make_unique<pop_budget_details_main_needs_s_value_t>();
			needs_s_value->parent = this;
			auto cptr = needs_s_value.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if (child_data.is_lua) { 
			std::string str_name {child_data.name};
			scripted_elements[str_name] = std::make_unique<ui::lua_scripted_element>();
			auto cptr = scripted_elements[str_name].get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_update_lname = child_data.text_key;
			if(child_data.tooltip_text_key.length() > 0) {
				cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			}
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		pending_children.pop_back();
	}
	wage_per_labor.on_create(state, this);
	needs.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN main::create
// END
}
std::unique_ptr<ui::element_base> make_pop_budget_details_main(sys::state& state) {
	auto ptr = std::make_unique<pop_budget_details_main_t>();
	ptr->on_create(state);
	return ptr;
}
void pop_budget_details_wage_income_labor_type_t::on_update(sys::state& state) noexcept {
	pop_budget_details_wage_income_t& wage_income = *((pop_budget_details_wage_income_t*)(parent)); 
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent->parent)); 
// BEGIN wage_income::labor_type::update
	set_text(state, text::produce_simple_string(state, ui::labour_type_to_employment_name_text_key(wage_income.labor_type_index)));
// END
}
void pop_budget_details_wage_income_labor_ratio_t::on_update(sys::state& state) noexcept {
	pop_budget_details_wage_income_t& wage_income = *((pop_budget_details_wage_income_t*)(parent)); 
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent->parent)); 
// BEGIN wage_income::labor_ratio::update
	set_text(state, text::format_percentage(wage_income.ratio));
// END
}
void pop_budget_details_wage_income_labor_wage_t::on_update(sys::state& state) noexcept {
	pop_budget_details_wage_income_t& wage_income = *((pop_budget_details_wage_income_t*)(parent)); 
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent->parent)); 
// BEGIN wage_income::labor_wage::update
	set_text(state, text::format_money(wage_income.wage));
// END
}
void  pop_budget_details_wage_income_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 4 : 3;
}
ui::message_result pop_budget_details_wage_income_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_budget_details_wage_income_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_budget_details_wage_income_t::on_update(sys::state& state) noexcept {
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent->parent)); 
// BEGIN wage_income::update
// END
	remake_layout(state, true);
}
void pop_budget_details_wage_income_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
	serialization::in_buffer buffer(ldata, sz);
	buffer.read(lvl.size_x); 
	buffer.read(lvl.size_y); 
	buffer.read(lvl.margin_top); 
	buffer.read(lvl.margin_bottom); 
	buffer.read(lvl.margin_left); 
	buffer.read(lvl.margin_right); 
	buffer.read(lvl.line_alignment); 
	buffer.read(lvl.line_internal_alignment); 
	buffer.read(lvl.type); 
	buffer.read(lvl.page_animation); 
	buffer.read(lvl.interline_spacing); 
	buffer.read(lvl.paged); 
	if(lvl.paged) {
		lvl.page_controls = std::make_unique<page_buttons>();
		lvl.page_controls->for_layout = &lvl;
		lvl.page_controls->parent = this;
		lvl.page_controls->base_data.size.x = int16_t(grid_size * 10);
		lvl.page_controls->base_data.size.y = int16_t(grid_size * 2);
	}
	auto expansion_section = buffer.read_section();
	if(expansion_section)
		expansion_section.read(lvl.template_id);
	if(lvl.template_id == -1 && window_template != -1)
		lvl.template_id = int16_t(state.ui_templates.window_t[window_template].layout_region_definition);
	while(buffer) {
		layout_item_types t;
		buffer.read(t);
		switch(t) {
			case layout_item_types::texture_layer:
			{
				texture_layer temp;
				buffer.read(temp.texture_type);
				buffer.read(temp.texture);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::control:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				temp.ptr = nullptr;
				if(cname == "labor_type") {
					temp.ptr = labor_type.get();
				} else
				if(cname == "labor_ratio") {
					temp.ptr = labor_ratio.get();
				} else
				if(cname == "labor_wage") {
					temp.ptr = labor_wage.get();
				} else
				{
					std::string str_cname {cname};
					auto found = scripted_elements.find(str_cname);
					if (found != scripted_elements.end()) {
						temp.ptr = found->second.get();
					}
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::window:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				if(cname == "main") {
					temp.ptr = make_pop_budget_details_main(state);
				}
				if(cname == "wage_income") {
					temp.ptr = make_pop_budget_details_wage_income(state);
				}
				if(cname == "production_income") {
					temp.ptr = make_pop_budget_details_production_income(state);
				}
				if(cname == "consumption") {
					temp.ptr = make_pop_budget_details_consumption(state);
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::glue:
			{
				layout_glue temp;
				buffer.read(temp.type);
				buffer.read(temp.amount);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::generator:
			{
				generator_instance temp;
				std::string_view cname = buffer.read<std::string_view>();
				auto gen_details = buffer.read_section(); // ignored
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::layout:
			{
				sub_layout temp;
				temp.layout = std::make_unique<layout_level>();
				auto layout_section = buffer.read_section();
				create_layout_level(state, *temp.layout, layout_section.view_data() + layout_section.view_read_position(), layout_section.view_size() - layout_section.view_read_position());
				lvl.contents.emplace_back(std::move(temp));
			} break;
		}
	}
}
void pop_budget_details_wage_income_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("pop_budget_details::wage_income"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	layout_window_element::initialize_template(state, win_data.template_id, win_data.grid_size, win_data.auto_close_button);
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "labor_type") {
			labor_type = std::make_unique<pop_budget_details_wage_income_labor_type_t>();
			labor_type->parent = this;
			auto cptr = labor_type.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "labor_ratio") {
			labor_ratio = std::make_unique<pop_budget_details_wage_income_labor_ratio_t>();
			labor_ratio->parent = this;
			auto cptr = labor_ratio.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "labor_wage") {
			labor_wage = std::make_unique<pop_budget_details_wage_income_labor_wage_t>();
			labor_wage->parent = this;
			auto cptr = labor_wage.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if (child_data.is_lua) { 
			std::string str_name {child_data.name};
			scripted_elements[str_name] = std::make_unique<ui::lua_scripted_element>();
			auto cptr = scripted_elements[str_name].get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_update_lname = child_data.text_key;
			if(child_data.tooltip_text_key.length() > 0) {
				cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			}
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		pending_children.pop_back();
	}
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN wage_income::create
// END
}
std::unique_ptr<ui::element_base> make_pop_budget_details_wage_income(sys::state& state) {
	auto ptr = std::make_unique<pop_budget_details_wage_income_t>();
	ptr->on_create(state);
	return ptr;
}
void pop_budget_details_production_income_commodity_t::on_update(sys::state& state) noexcept {
	pop_budget_details_production_income_t& production_income = *((pop_budget_details_production_income_t*)(parent)); 
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent->parent->parent)); 
// BEGIN production_income::commodity::update
// END
}
void pop_budget_details_production_income_income_t::on_update(sys::state& state) noexcept {
	pop_budget_details_production_income_t& production_income = *((pop_budget_details_production_income_t*)(parent)); 
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent->parent->parent)); 
// BEGIN production_income::income::update
// END
}
void  pop_budget_details_production_income_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 4 : 3;
}
ui::message_result pop_budget_details_production_income_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_budget_details_production_income_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_budget_details_production_income_t::on_update(sys::state& state) noexcept {
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent->parent)); 
// BEGIN production_income::update
// END
	remake_layout(state, true);
}
void pop_budget_details_production_income_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
	serialization::in_buffer buffer(ldata, sz);
	buffer.read(lvl.size_x); 
	buffer.read(lvl.size_y); 
	buffer.read(lvl.margin_top); 
	buffer.read(lvl.margin_bottom); 
	buffer.read(lvl.margin_left); 
	buffer.read(lvl.margin_right); 
	buffer.read(lvl.line_alignment); 
	buffer.read(lvl.line_internal_alignment); 
	buffer.read(lvl.type); 
	buffer.read(lvl.page_animation); 
	buffer.read(lvl.interline_spacing); 
	buffer.read(lvl.paged); 
	if(lvl.paged) {
		lvl.page_controls = std::make_unique<page_buttons>();
		lvl.page_controls->for_layout = &lvl;
		lvl.page_controls->parent = this;
		lvl.page_controls->base_data.size.x = int16_t(grid_size * 10);
		lvl.page_controls->base_data.size.y = int16_t(grid_size * 2);
	}
	auto expansion_section = buffer.read_section();
	if(expansion_section)
		expansion_section.read(lvl.template_id);
	if(lvl.template_id == -1 && window_template != -1)
		lvl.template_id = int16_t(state.ui_templates.window_t[window_template].layout_region_definition);
	while(buffer) {
		layout_item_types t;
		buffer.read(t);
		switch(t) {
			case layout_item_types::texture_layer:
			{
				texture_layer temp;
				buffer.read(temp.texture_type);
				buffer.read(temp.texture);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::control:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				temp.ptr = nullptr;
				if(cname == "commodity") {
					temp.ptr = commodity.get();
				} else
				if(cname == "income") {
					temp.ptr = income.get();
				} else
				{
					std::string str_cname {cname};
					auto found = scripted_elements.find(str_cname);
					if (found != scripted_elements.end()) {
						temp.ptr = found->second.get();
					}
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::window:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				if(cname == "main") {
					temp.ptr = make_pop_budget_details_main(state);
				}
				if(cname == "wage_income") {
					temp.ptr = make_pop_budget_details_wage_income(state);
				}
				if(cname == "production_income") {
					temp.ptr = make_pop_budget_details_production_income(state);
				}
				if(cname == "consumption") {
					temp.ptr = make_pop_budget_details_consumption(state);
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::glue:
			{
				layout_glue temp;
				buffer.read(temp.type);
				buffer.read(temp.amount);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::generator:
			{
				generator_instance temp;
				std::string_view cname = buffer.read<std::string_view>();
				auto gen_details = buffer.read_section(); // ignored
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::layout:
			{
				sub_layout temp;
				temp.layout = std::make_unique<layout_level>();
				auto layout_section = buffer.read_section();
				create_layout_level(state, *temp.layout, layout_section.view_data() + layout_section.view_read_position(), layout_section.view_size() - layout_section.view_read_position());
				lvl.contents.emplace_back(std::move(temp));
			} break;
		}
	}
}
void pop_budget_details_production_income_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("pop_budget_details::production_income"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	layout_window_element::initialize_template(state, win_data.template_id, win_data.grid_size, win_data.auto_close_button);
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "commodity") {
			commodity = std::make_unique<pop_budget_details_production_income_commodity_t>();
			commodity->parent = this;
			auto cptr = commodity.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "income") {
			income = std::make_unique<pop_budget_details_production_income_income_t>();
			income->parent = this;
			auto cptr = income.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if (child_data.is_lua) { 
			std::string str_name {child_data.name};
			scripted_elements[str_name] = std::make_unique<ui::lua_scripted_element>();
			auto cptr = scripted_elements[str_name].get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_update_lname = child_data.text_key;
			if(child_data.tooltip_text_key.length() > 0) {
				cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			}
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		pending_children.pop_back();
	}
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN production_income::create
// END
}
std::unique_ptr<ui::element_base> make_pop_budget_details_production_income(sys::state& state) {
	auto ptr = std::make_unique<pop_budget_details_production_income_t>();
	ptr->on_create(state);
	return ptr;
}
void pop_budget_details_consumption_name_t::on_update(sys::state& state) noexcept {
	pop_budget_details_consumption_t& consumption = *((pop_budget_details_consumption_t*)(parent)); 
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent->parent)); 
// BEGIN consumption::name::update
	std::string modifier{};
	if(consumption.category == 0) {
		modifier = text::produce_simple_string(state, "alice_pop_details_ln_short");
	} else if(consumption.category == 1) {
		modifier = text::produce_simple_string(state, "alice_pop_details_en_short");
	} else if(consumption.category == 2) {
		modifier = text::produce_simple_string(state, "alice_pop_details_lx_short");
	}
	auto commodity_name = text::produce_simple_string(state, state.world.commodity_get_name(consumption.cid));		
	set_text(state, "(" + modifier + ")" + commodity_name);
// END
}
void pop_budget_details_consumption_satisfaction_t::on_update(sys::state& state) noexcept {
	pop_budget_details_consumption_t& consumption = *((pop_budget_details_consumption_t*)(parent)); 
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent->parent)); 
// BEGIN consumption::satisfaction::update
	set_text(state, text::format_percentage(consumption.satisfaction_val, 0));
// END
}
void pop_budget_details_consumption_cost_t::on_update(sys::state& state) noexcept {
	pop_budget_details_consumption_t& consumption = *((pop_budget_details_consumption_t*)(parent)); 
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent->parent)); 
// BEGIN consumption::cost::update
	set_text(state, text::format_money(consumption.cost_val));
// END
}
void  pop_budget_details_consumption_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 4 : 3;
}
ui::message_result pop_budget_details_consumption_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_budget_details_consumption_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_budget_details_consumption_t::on_update(sys::state& state) noexcept {
	pop_budget_details_main_t& main = *((pop_budget_details_main_t*)(parent->parent)); 
// BEGIN consumption::update
// END
	remake_layout(state, true);
}
void pop_budget_details_consumption_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
	serialization::in_buffer buffer(ldata, sz);
	buffer.read(lvl.size_x); 
	buffer.read(lvl.size_y); 
	buffer.read(lvl.margin_top); 
	buffer.read(lvl.margin_bottom); 
	buffer.read(lvl.margin_left); 
	buffer.read(lvl.margin_right); 
	buffer.read(lvl.line_alignment); 
	buffer.read(lvl.line_internal_alignment); 
	buffer.read(lvl.type); 
	buffer.read(lvl.page_animation); 
	buffer.read(lvl.interline_spacing); 
	buffer.read(lvl.paged); 
	if(lvl.paged) {
		lvl.page_controls = std::make_unique<page_buttons>();
		lvl.page_controls->for_layout = &lvl;
		lvl.page_controls->parent = this;
		lvl.page_controls->base_data.size.x = int16_t(grid_size * 10);
		lvl.page_controls->base_data.size.y = int16_t(grid_size * 2);
	}
	auto expansion_section = buffer.read_section();
	if(expansion_section)
		expansion_section.read(lvl.template_id);
	if(lvl.template_id == -1 && window_template != -1)
		lvl.template_id = int16_t(state.ui_templates.window_t[window_template].layout_region_definition);
	while(buffer) {
		layout_item_types t;
		buffer.read(t);
		switch(t) {
			case layout_item_types::texture_layer:
			{
				texture_layer temp;
				buffer.read(temp.texture_type);
				buffer.read(temp.texture);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::control:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				temp.ptr = nullptr;
				if(cname == "name") {
					temp.ptr = name.get();
				} else
				if(cname == "satisfaction") {
					temp.ptr = satisfaction.get();
				} else
				if(cname == "cost") {
					temp.ptr = cost.get();
				} else
				{
					std::string str_cname {cname};
					auto found = scripted_elements.find(str_cname);
					if (found != scripted_elements.end()) {
						temp.ptr = found->second.get();
					}
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::window:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				if(cname == "main") {
					temp.ptr = make_pop_budget_details_main(state);
				}
				if(cname == "wage_income") {
					temp.ptr = make_pop_budget_details_wage_income(state);
				}
				if(cname == "production_income") {
					temp.ptr = make_pop_budget_details_production_income(state);
				}
				if(cname == "consumption") {
					temp.ptr = make_pop_budget_details_consumption(state);
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::glue:
			{
				layout_glue temp;
				buffer.read(temp.type);
				buffer.read(temp.amount);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::generator:
			{
				generator_instance temp;
				std::string_view cname = buffer.read<std::string_view>();
				auto gen_details = buffer.read_section(); // ignored
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::layout:
			{
				sub_layout temp;
				temp.layout = std::make_unique<layout_level>();
				auto layout_section = buffer.read_section();
				create_layout_level(state, *temp.layout, layout_section.view_data() + layout_section.view_read_position(), layout_section.view_size() - layout_section.view_read_position());
				lvl.contents.emplace_back(std::move(temp));
			} break;
		}
	}
}
void pop_budget_details_consumption_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("pop_budget_details::consumption"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	layout_window_element::initialize_template(state, win_data.template_id, win_data.grid_size, win_data.auto_close_button);
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "name") {
			name = std::make_unique<pop_budget_details_consumption_name_t>();
			name->parent = this;
			auto cptr = name.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "satisfaction") {
			satisfaction = std::make_unique<pop_budget_details_consumption_satisfaction_t>();
			satisfaction->parent = this;
			auto cptr = satisfaction.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "cost") {
			cost = std::make_unique<pop_budget_details_consumption_cost_t>();
			cost->parent = this;
			auto cptr = cost.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if (child_data.is_lua) { 
			std::string str_name {child_data.name};
			scripted_elements[str_name] = std::make_unique<ui::lua_scripted_element>();
			auto cptr = scripted_elements[str_name].get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_update_lname = child_data.text_key;
			if(child_data.tooltip_text_key.length() > 0) {
				cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			}
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		pending_children.pop_back();
	}
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN consumption::create
// END
}
std::unique_ptr<ui::element_base> make_pop_budget_details_consumption(sys::state& state) {
	auto ptr = std::make_unique<pop_budget_details_consumption_t>();
	ptr->on_create(state);
	return ptr;
}
// LOST-CODE
}
