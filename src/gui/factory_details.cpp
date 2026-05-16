// BEGIN prelude
#include "alice_ui.hpp"
#include "system_state.hpp"
#include "gui_deserialize.hpp"
#include "lua_alice_api.hpp"
#include "economy_production.hpp"
#include "labour_details.hpp"
#include "commands.hpp"
#include "ai_economy.hpp"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
#endif
// END

namespace alice_ui {
struct factory_details_main_total_wage_value_t;
struct factory_details_main_expected_profit_value_t;
struct factory_details_main_wage_no_education_t;
struct factory_details_main_wage_basic_education_t;
struct factory_details_main_wage_secondary_t;
struct factory_details_main_hired_no_education_t;
struct factory_details_main_hired_basic_education_t;
struct factory_details_main_employment_units_t;
struct factory_details_main_throughput_multiplier_t;
struct factory_details_main_production_units_value_t;
struct factory_details_main_secondary_hired_t;
struct factory_details_main_output_mult_secondary_t;
struct factory_details_main_other_output_mult_t;
struct factory_details_main_output_t;
struct factory_details_main_base_output_t;
struct factory_details_main_icon_output_t;
struct factory_details_main_information_employment_units_t;
struct factory_details_main_information_production_units_t;
struct factory_details_main_effective_output_t;
struct factory_details_main_subsidy_t;
struct factory_details_main_total_investments_value_t;
struct factory_details_main_investment_expansion_t;
struct factory_details_main_investment_efficiency_t;
struct factory_details_main_t;
struct factory_details_commodity_row_icon_t;
struct factory_details_commodity_row_name_t;
struct factory_details_commodity_row_amount_value_t;
struct factory_details_commodity_row_ratio_value_t;
struct factory_details_commodity_row_price_value_t;
struct factory_details_commodity_row_final_value_t;
struct factory_details_commodity_row_t;
struct factory_details_main_total_wage_value_t : public alice_ui::template_label {
// BEGIN main::total_wage_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_expected_profit_value_t : public alice_ui::template_label {
// BEGIN main::expected_profit_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_wage_no_education_t : public alice_ui::template_label {
// BEGIN main::wage_no_education::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_wage_basic_education_t : public alice_ui::template_label {
// BEGIN main::wage_basic_education::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_wage_secondary_t : public alice_ui::template_label {
// BEGIN main::wage_secondary::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_hired_no_education_t : public alice_ui::template_label {
// BEGIN main::hired_no_education::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_hired_basic_education_t : public alice_ui::template_label {
// BEGIN main::hired_basic_education::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_employment_units_t : public alice_ui::template_label {
// BEGIN main::employment_units::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_throughput_multiplier_t : public alice_ui::template_label {
// BEGIN main::throughput_multiplier::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_production_units_value_t : public alice_ui::template_label {
// BEGIN main::production_units_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_secondary_hired_t : public alice_ui::template_label {
// BEGIN main::secondary_hired::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_output_mult_secondary_t : public alice_ui::template_label {
// BEGIN main::output_mult_secondary::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_other_output_mult_t : public alice_ui::template_label {
// BEGIN main::other_output_mult::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_output_t : public alice_ui::template_label {
// BEGIN main::output::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_base_output_t : public alice_ui::template_label {
// BEGIN main::base_output::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_icon_output_t : public alice_ui::legacy_commodity_icon {
// BEGIN main::icon_output::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_information_employment_units_t : public alice_ui::template_icon_graphic {
// BEGIN main::information_employment_units::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_information_production_units_t : public alice_ui::template_icon_graphic {
// BEGIN main::information_production_units::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_effective_output_t : public alice_ui::template_label {
// BEGIN main::effective_output::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_subsidy_t : public alice_ui::template_label {
// BEGIN main::subsidy::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_total_investments_value_t : public alice_ui::template_label {
// BEGIN main::total_investments_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_investment_expansion_t : public alice_ui::template_label {
// BEGIN main::investment_expansion::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_investment_efficiency_t : public alice_ui::template_label {
// BEGIN main::investment_efficiency::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_output_row_t : public layout_generator {
// BEGIN main::output_row::variables
// END
	struct commodity_row_option { dcon::commodity_id commodity; float amount; float ratio; float price; };
	std::vector<std::unique_ptr<ui::element_base>> commodity_row_pool;
	int32_t commodity_row_pool_used = 0;
	void add_commodity_row( dcon::commodity_id commodity,  float amount,  float ratio,  float price);
	std::vector<std::variant<std::monostate, commodity_row_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct factory_details_main_inputs_row_t : public layout_generator {
// BEGIN main::inputs_row::variables
// END
	struct commodity_row_option { dcon::commodity_id commodity; float amount; float ratio; float price; };
	std::vector<std::unique_ptr<ui::element_base>> commodity_row_pool;
	int32_t commodity_row_pool_used = 0;
	void add_commodity_row( dcon::commodity_id commodity,  float amount,  float ratio,  float price);
	std::vector<std::variant<std::monostate, commodity_row_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct factory_details_commodity_row_icon_t : public alice_ui::legacy_commodity_icon {
// BEGIN commodity_row::icon::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_commodity_row_name_t : public alice_ui::template_label {
// BEGIN commodity_row::name::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_commodity_row_amount_value_t : public alice_ui::template_label {
// BEGIN commodity_row::amount_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_commodity_row_ratio_value_t : public alice_ui::template_label {
// BEGIN commodity_row::ratio_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_commodity_row_price_value_t : public alice_ui::template_label {
// BEGIN commodity_row::price_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_commodity_row_final_value_t : public alice_ui::template_label {
// BEGIN commodity_row::final_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct factory_details_main_t : public layout_window_element {
// BEGIN main::variables
	economy::factory_operation::detailed_explanation last_explanation;
// END
	std::unique_ptr<template_label> total_wage_label;
	std::unique_ptr<factory_details_main_total_wage_value_t> total_wage_value;
	std::unique_ptr<factory_details_main_expected_profit_value_t> expected_profit_value;
	std::unique_ptr<factory_details_main_wage_no_education_t> wage_no_education;
	std::unique_ptr<factory_details_main_wage_basic_education_t> wage_basic_education;
	std::unique_ptr<factory_details_main_wage_secondary_t> wage_secondary;
	std::unique_ptr<template_icon_graphic> no_education_arrow;
	std::unique_ptr<template_icon_graphic> basic_education_arrow;
	std::unique_ptr<factory_details_main_hired_no_education_t> hired_no_education;
	std::unique_ptr<factory_details_main_hired_basic_education_t> hired_basic_education;
	std::unique_ptr<template_icon_graphic> primary_to_eu_arrow;
	std::unique_ptr<factory_details_main_employment_units_t> employment_units;
	std::unique_ptr<template_icon_graphic> eu_to_pu_arrow;
	std::unique_ptr<template_icon_graphic> secondary_to_wage_arrow;
	std::unique_ptr<factory_details_main_throughput_multiplier_t> throughput_multiplier;
	std::unique_ptr<template_icon_graphic> throughput_to_pu_arrow;
	std::unique_ptr<factory_details_main_production_units_value_t> production_units_value;
	std::unique_ptr<factory_details_main_secondary_hired_t> secondary_hired;
	std::unique_ptr<template_icon_graphic> secondary_to_mult;
	std::unique_ptr<factory_details_main_output_mult_secondary_t> output_mult_secondary;
	std::unique_ptr<template_icon_graphic> pu_to_output_arrow;
	std::unique_ptr<factory_details_main_other_output_mult_t> other_output_mult;
	std::unique_ptr<template_icon_graphic> output_mult_to_output;
	std::unique_ptr<template_icon_graphic> secondary_mult_to_ouput;
	std::unique_ptr<template_icon_graphic> arrow_to_output;
	std::unique_ptr<factory_details_main_output_t> output;
	std::unique_ptr<template_icon_graphic> base_to_output;
	std::unique_ptr<factory_details_main_base_output_t> base_output;
	std::unique_ptr<template_label> expected_profit_label;
	std::unique_ptr<template_icon_graphic> icon_unskilled;
	std::unique_ptr<template_icon_graphic> icon_skilled;
	std::unique_ptr<template_icon_graphic> icon_expert;
	std::unique_ptr<factory_details_main_icon_output_t> icon_output;
	std::unique_ptr<factory_details_main_information_employment_units_t> information_employment_units;
	std::unique_ptr<factory_details_main_information_production_units_t> information_production_units;
	std::unique_ptr<template_icon_graphic> secondary_arrow;
	std::unique_ptr<template_label> subsidies_label;
	std::unique_ptr<factory_details_main_effective_output_t> effective_output;
	std::unique_ptr<template_icon_graphic> subsidies_explanation;
	std::unique_ptr<template_icon_graphic> effective_output_to_subsidy;
	std::unique_ptr<factory_details_main_subsidy_t> subsidy;
	std::unique_ptr<template_label> investment;
	std::unique_ptr<template_label> total_investments_label;
	std::unique_ptr<factory_details_main_total_investments_value_t> total_investments_value;
	std::unique_ptr<template_icon_graphic> icon_split_investment;
	std::unique_ptr<factory_details_main_investment_expansion_t> investment_expansion;
	std::unique_ptr<factory_details_main_investment_efficiency_t> investment_efficiency;
	factory_details_main_output_row_t output_row;
	factory_details_main_inputs_row_t inputs_row;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
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
};
std::unique_ptr<ui::element_base> make_factory_details_main(sys::state& state);
struct factory_details_commodity_row_t : public layout_window_element {
// BEGIN commodity_row::variables
// END
	dcon::commodity_id commodity;
	float amount;
	float ratio;
	float price;
	std::unique_ptr<factory_details_commodity_row_icon_t> icon;
	std::unique_ptr<factory_details_commodity_row_name_t> name;
	std::unique_ptr<factory_details_commodity_row_amount_value_t> amount_value;
	std::unique_ptr<factory_details_commodity_row_ratio_value_t> ratio_value;
	std::unique_ptr<factory_details_commodity_row_price_value_t> price_value;
	std::unique_ptr<template_icon_graphic> arrow;
	std::unique_ptr<factory_details_commodity_row_final_value_t> final_value;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "commodity") {
			return (void*)(&commodity);
		}
		if(name_parameter == "amount") {
			return (void*)(&amount);
		}
		if(name_parameter == "ratio") {
			return (void*)(&ratio);
		}
		if(name_parameter == "price") {
			return (void*)(&price);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_factory_details_commodity_row(sys::state& state);
void factory_details_main_output_row_t::add_commodity_row(dcon::commodity_id commodity, float amount, float ratio, float price) {
	values.emplace_back(commodity_row_option{commodity, amount, ratio, price});
}
void  factory_details_main_output_row_t::on_create(sys::state& state, layout_window_element* parent) {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::output_row::on_create
// END
}
void  factory_details_main_output_row_t::update(sys::state& state, layout_window_element* parent) {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::output_row::update
	values.clear();
	add_commodity_row(
		main.last_explanation.output,
		main.last_explanation.output_actual_amount,
		main.last_explanation.output_actually_sold_ratio,
		main.last_explanation.output_price
	);
// END
}
measure_result  factory_details_main_output_row_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<commodity_row_option>(values[index])) {
		if(commodity_row_pool.empty()) commodity_row_pool.emplace_back(make_factory_details_commodity_row(state));
		if(destination) {
			if(commodity_row_pool.size() <= size_t(commodity_row_pool_used)) commodity_row_pool.emplace_back(make_factory_details_commodity_row(state));
			commodity_row_pool[commodity_row_pool_used]->base_data.position.x = int16_t(x);
			commodity_row_pool[commodity_row_pool_used]->base_data.position.y = int16_t(y);
			commodity_row_pool[commodity_row_pool_used]->parent = destination;
			destination->children.push_back(commodity_row_pool[commodity_row_pool_used].get());
			((factory_details_commodity_row_t*)(commodity_row_pool[commodity_row_pool_used].get()))->commodity = std::get<commodity_row_option>(values[index]).commodity;
			((factory_details_commodity_row_t*)(commodity_row_pool[commodity_row_pool_used].get()))->amount = std::get<commodity_row_option>(values[index]).amount;
			((factory_details_commodity_row_t*)(commodity_row_pool[commodity_row_pool_used].get()))->ratio = std::get<commodity_row_option>(values[index]).ratio;
			((factory_details_commodity_row_t*)(commodity_row_pool[commodity_row_pool_used].get()))->price = std::get<commodity_row_option>(values[index]).price;
			((factory_details_commodity_row_t*)(commodity_row_pool[commodity_row_pool_used].get()))->set_alternate(alternate);
			commodity_row_pool[commodity_row_pool_used]->impl_on_update(state);
			commodity_row_pool_used++;
		}
		alternate = !alternate;
	 	 	bool stick_to_next = false;
		return measure_result{ commodity_row_pool[0]->base_data.size.x, commodity_row_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  factory_details_main_output_row_t::reset_pools() {
	commodity_row_pool_used = 0;
}
void factory_details_main_inputs_row_t::add_commodity_row(dcon::commodity_id commodity, float amount, float ratio, float price) {
	values.emplace_back(commodity_row_option{commodity, amount, ratio, price});
}
void  factory_details_main_inputs_row_t::on_create(sys::state& state, layout_window_element* parent) {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::inputs_row::on_create
// END
}
void  factory_details_main_inputs_row_t::update(sys::state& state, layout_window_element* parent) {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::inputs_row::update
	values.clear();
	for(uint32_t i = 0; i < economy::commodity_set::set_size; i++) {
		auto& value = main.last_explanation.primary_inputs;
		if (!value.commodity_type[i]) break;
		add_commodity_row(value.commodity_type[i], value.commodity_actual_amount[i], value.efficient_ratio[i], value.commodity_price[i]);
	}
// END
}
measure_result  factory_details_main_inputs_row_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<commodity_row_option>(values[index])) {
		if(commodity_row_pool.empty()) commodity_row_pool.emplace_back(make_factory_details_commodity_row(state));
		if(destination) {
			if(commodity_row_pool.size() <= size_t(commodity_row_pool_used)) commodity_row_pool.emplace_back(make_factory_details_commodity_row(state));
			commodity_row_pool[commodity_row_pool_used]->base_data.position.x = int16_t(x);
			commodity_row_pool[commodity_row_pool_used]->base_data.position.y = int16_t(y);
			commodity_row_pool[commodity_row_pool_used]->parent = destination;
			destination->children.push_back(commodity_row_pool[commodity_row_pool_used].get());
			((factory_details_commodity_row_t*)(commodity_row_pool[commodity_row_pool_used].get()))->commodity = std::get<commodity_row_option>(values[index]).commodity;
			((factory_details_commodity_row_t*)(commodity_row_pool[commodity_row_pool_used].get()))->amount = std::get<commodity_row_option>(values[index]).amount;
			((factory_details_commodity_row_t*)(commodity_row_pool[commodity_row_pool_used].get()))->ratio = std::get<commodity_row_option>(values[index]).ratio;
			((factory_details_commodity_row_t*)(commodity_row_pool[commodity_row_pool_used].get()))->price = std::get<commodity_row_option>(values[index]).price;
			((factory_details_commodity_row_t*)(commodity_row_pool[commodity_row_pool_used].get()))->set_alternate(alternate);
			commodity_row_pool[commodity_row_pool_used]->impl_on_update(state);
			commodity_row_pool_used++;
		}
		alternate = !alternate;
	 	 	bool stick_to_next = false;
		return measure_result{ commodity_row_pool[0]->base_data.size.x, commodity_row_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  factory_details_main_inputs_row_t::reset_pools() {
	commodity_row_pool_used = 0;
}
void factory_details_main_total_wage_value_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::total_wage_value::update
	set_text(state, text::format_money(main.last_explanation.spending_from_wages));
// END
}
void factory_details_main_expected_profit_value_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::expected_profit_value::update
	set_text(state, text::format_money(main.last_explanation.profit));
// END
}
void factory_details_main_wage_no_education_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::wage_no_education::update
	set_text(state, text::format_money(main.last_explanation.employment_wages_paid.unqualified));
// END
}
void factory_details_main_wage_basic_education_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::wage_basic_education::update
	set_text(state, text::format_money(main.last_explanation.employment_wages_paid.primary));
// END
}
void factory_details_main_wage_secondary_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::wage_secondary::update
	set_text(state, text::format_money(main.last_explanation.employment_wages_paid.secondary));
// END
}
void factory_details_main_hired_no_education_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::hired_no_education::tooltip
	text::add_line(state, contents, "factory_hired_unqualified",
		text::variable_type::value, text::format_float(main.last_explanation.employment.unqualified),
		text::variable_type::x, text::format_percentage(economy::unqualified_throughput_multiplier)
	);
	if(main.last_explanation.employment_expected_change.unqualified >= 0) {
		text::add_line(state, contents, "factory_hiring_rate_unqualified",
			text::variable_type::value, text::format_float(main.last_explanation.employment_expected_change.unqualified),
			text::variable_type::x, text::format_float(main.last_explanation.employment_target.unqualified)
		);
	} else {
		text::add_line(state, contents, "factory_firing_rate_unqualified",
			text::variable_type::value, text::format_float(-main.last_explanation.employment_expected_change.unqualified),
			text::variable_type::x, text::format_float(main.last_explanation.employment_target.unqualified)
		);
	}
// END
}
void factory_details_main_hired_no_education_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::hired_no_education::update
	set_text(state, text::prettify((int)main.last_explanation.employment.unqualified));
// END
}
void factory_details_main_hired_basic_education_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::hired_basic_education::tooltip
	auto& details = main.last_explanation;
	text::add_line(state, contents, "factory_hired_primary",
		text::variable_type::value, text::format_float(details.employment.primary)
	);

	if(details.employment_expected_change.primary >= 0) {
		text::add_line(state, contents, "factory_hiring_rate_primary",
			text::variable_type::value, text::format_float(details.employment_expected_change.primary),
			text::variable_type::x, text::format_float(details.employment_target.primary)
		);
	} else {
		text::add_line(state, contents, "factory_firing_rate_primary",
			text::variable_type::value, text::format_float(-details.employment_expected_change.primary),
			text::variable_type::x, text::format_float(details.employment_target.primary)
		);
	}
// END
}
void factory_details_main_hired_basic_education_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::hired_basic_education::update
	set_text(state, text::prettify((int)main.last_explanation.employment.primary));
// END
}
void factory_details_main_employment_units_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::employment_units::update
	set_text(state, text::prettify_float((main.last_explanation.employment_units)));
// END
}
void factory_details_main_throughput_multiplier_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::throughput_multiplier::update
	set_text(state, text::format_percentage(main.last_explanation.throughput_multipliers.total));
// END
}
void factory_details_main_production_units_value_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::production_units_value::update
	set_text(state, text::prettify_float(main.last_explanation.production_units));
// END
}
void factory_details_main_secondary_hired_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::secondary_hired::tooltip
	auto& details = main.last_explanation;
	text::add_line(state, contents, "factory_hired_secondary",
		text::variable_type::value, text::format_float(details.employment.secondary),
		text::variable_type::x, text::format_percentage(details.output_multipliers.from_secondary_workers)
	);

	if(details.employment_expected_change.secondary >= 0) {
		text::add_line(state, contents, "factory_hiring_rate_secondary",
			text::variable_type::value, text::format_float(details.employment_expected_change.secondary),
			text::variable_type::x, text::format_float(details.employment_target.secondary)
		);
	} else {
		text::add_line(state, contents, "factory_firing_rate_secondary",
			text::variable_type::value, text::format_float(-details.employment_expected_change.secondary),
			text::variable_type::x, text::format_float(details.employment_target.secondary)
		);
	}
// END
}
void factory_details_main_secondary_hired_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::secondary_hired::update
	set_text(state, text::prettify((int)main.last_explanation.employment.secondary));
// END
}
void factory_details_main_output_mult_secondary_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::output_mult_secondary::update
	set_text(state, text::format_percentage(main.last_explanation.output_multipliers.from_secondary_workers));
// END
}
void factory_details_main_other_output_mult_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::other_output_mult::tooltip
	auto& details  = main.last_explanation;
	text::add_line(state, contents, "factory_output_multiplier_explanation",
		text::variable_type::val, text::format_percentage(details.output_multipliers.total)
	);
	text::add_line(state, contents, "factory_output_multiplier_lack_of_inputs",
		text::variable_type::val, text::format_percentage(details.output_multipliers.from_inputs_lack)
	);
	text::add_line(state, contents, "factory_output_multiplier_secondary_workers",
		text::variable_type::val, text::format_percentage(details.output_multipliers.from_secondary_workers)
	);
	text::add_line(state, contents, "factory_output_multiplier_modifiers",
		text::variable_type::val, text::format_percentage(details.output_multipliers.from_modifiers)
	);
// END
}
void factory_details_main_other_output_mult_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::other_output_mult::update
	set_text(state, text::format_percentage(main.last_explanation.output_multipliers.total / main.last_explanation.output_multipliers.from_secondary_workers));
// END
}
void factory_details_main_output_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::output::update
	set_text(state, text::format_float(main.last_explanation.output_actual_amount));
// END
}
void factory_details_main_base_output_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::base_output::update
	set_text(state, text::format_float(state.world.factory_type_get_output_amount(state.world.factory_get_building_type(state.selected_factory))));
// END
}
void factory_details_main_icon_output_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::icon_output::update
	content = main.last_explanation.output;
// END
}
void factory_details_main_information_employment_units_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::information_employment_units::tooltip
// END
}
void factory_details_main_information_employment_units_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::information_employment_units::update
// END
}
void factory_details_main_information_production_units_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::information_production_units::tooltip
	auto& details = main.last_explanation;
	auto indent = 15;

	text::add_line(state, contents, "factory_throughput_explanation",
				text::variable_type::val, text::format_percentage(details.throughput_multipliers.total),
				text::variable_type::x, text::format_float(details.production_units),
				text::variable_type::y, text::format_float(details.employment_units)
	);

	text::add_line(state, contents, "factory_throughput_base",
		text::variable_type::val, text::format_percentage(details.throughput_multipliers.base),
		indent
	);
	text::add_line(state, contents, "factory_stats_7",
		text::variable_type::val, text::format_percentage(details.throughput_multipliers.from_scale),
		indent
	);
	text::add_line(state, contents, "factory_throughput_modifiers",
		text::variable_type::val, text::format_percentage(details.throughput_multipliers.from_modifiers),
		indent
	);
	text::add_line(state, contents, "factory_throughput_subsistence",
		text::variable_type::val, text::format_percentage(details.throughput_multipliers.from_forced_subsistence),
		indent
	);
// END
}
void factory_details_main_information_production_units_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::information_production_units::update
// END
}
void factory_details_main_effective_output_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::effective_output::update
	set_text(state, text::format_float(main.last_explanation.output_actual_amount / main.last_explanation.output_base_amount));
// END
}
void factory_details_main_subsidy_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::subsidy::update
	set_text(state, text::format_money(main.last_explanation.revenue_from_subsidies));
// END
}
void factory_details_main_total_investments_value_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::total_investments_value::update
	auto factory = state.selected_factory;
	auto province = state.world.factory_get_province_from_factory_location(factory);
	auto nation = state.world.province_get_nation_from_province_ownership(province);
	auto total_tokens = economy::total_nation_investments_tokens(state, nation);
	auto total_investment = state.world.nation_get_private_investment(nation);
	auto tokens = main.last_explanation.investments_tokens;
	if(total_tokens == 0.f) {
		set_text(state, text::format_money(0.f));
		return;
	}
	auto investment = tokens / total_tokens * total_investment;
	set_text(state, text::format_money(investment));
// END
}
void factory_details_main_investment_expansion_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::investment_expansion::update
	auto factory = state.selected_factory;
	auto province = state.world.factory_get_province_from_factory_location(factory);
	auto nation = state.world.province_get_nation_from_province_ownership(province);
	auto total_tokens = economy::total_nation_investments_tokens(state, nation);
	auto total_investment = state.world.nation_get_private_investment(nation);
	auto tokens = main.last_explanation.investments_tokens;
	if(total_tokens == 0.f) {
		set_text(state, text::format_money(0.f));
		return;
	}
	auto investment = tokens / total_tokens * total_investment;
	set_text(state, text::format_money(investment * main.last_explanation.investments_expansion_priority));
// END
}
void factory_details_main_investment_efficiency_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent)); 
// BEGIN main::investment_efficiency::update
	auto factory = state.selected_factory;
	auto province = state.world.factory_get_province_from_factory_location(factory);
	auto nation = state.world.province_get_nation_from_province_ownership(province);
	auto total_tokens = economy::total_nation_investments_tokens(state, nation);
	auto total_investment = state.world.nation_get_private_investment(nation);
	auto tokens = main.last_explanation.investments_tokens;
	if(total_tokens == 0.f) {
		set_text(state, text::format_money(0.f));
		return;
	}
	auto investment = tokens / total_tokens * total_investment;
	set_text(state, text::format_money(investment * (1.f - main.last_explanation.investments_expansion_priority)));
// END
}
ui::message_result factory_details_main_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result factory_details_main_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void factory_details_main_t::on_update(sys::state& state) noexcept {
// BEGIN main::update
	last_explanation = economy::factory_operation::explain_everything(state, state.selected_factory);
// END
	output_row.update(state, this);
	inputs_row.update(state, this);
	remake_layout(state, true);
}
void factory_details_main_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
			case layout_item_types::control2:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				buffer.read(temp.fill_x);
				buffer.read(temp.fill_y);
				temp.ptr = nullptr;
				if(cname == "total_wage_label") {
					temp.ptr = total_wage_label.get();
				} else
				if(cname == "total_wage_value") {
					temp.ptr = total_wage_value.get();
				} else
				if(cname == "expected_profit_value") {
					temp.ptr = expected_profit_value.get();
				} else
				if(cname == "wage_no_education") {
					temp.ptr = wage_no_education.get();
				} else
				if(cname == "wage_basic_education") {
					temp.ptr = wage_basic_education.get();
				} else
				if(cname == "wage_secondary") {
					temp.ptr = wage_secondary.get();
				} else
				if(cname == "no_education_arrow") {
					temp.ptr = no_education_arrow.get();
				} else
				if(cname == "basic_education_arrow") {
					temp.ptr = basic_education_arrow.get();
				} else
				if(cname == "hired_no_education") {
					temp.ptr = hired_no_education.get();
				} else
				if(cname == "hired_basic_education") {
					temp.ptr = hired_basic_education.get();
				} else
				if(cname == "primary_to_eu_arrow") {
					temp.ptr = primary_to_eu_arrow.get();
				} else
				if(cname == "employment_units") {
					temp.ptr = employment_units.get();
				} else
				if(cname == "eu_to_pu_arrow") {
					temp.ptr = eu_to_pu_arrow.get();
				} else
				if(cname == "secondary_to_wage_arrow") {
					temp.ptr = secondary_to_wage_arrow.get();
				} else
				if(cname == "throughput_multiplier") {
					temp.ptr = throughput_multiplier.get();
				} else
				if(cname == "throughput_to_pu_arrow") {
					temp.ptr = throughput_to_pu_arrow.get();
				} else
				if(cname == "production_units_value") {
					temp.ptr = production_units_value.get();
				} else
				if(cname == "secondary_hired") {
					temp.ptr = secondary_hired.get();
				} else
				if(cname == "secondary_to_mult") {
					temp.ptr = secondary_to_mult.get();
				} else
				if(cname == "output_mult_secondary") {
					temp.ptr = output_mult_secondary.get();
				} else
				if(cname == "pu_to_output_arrow") {
					temp.ptr = pu_to_output_arrow.get();
				} else
				if(cname == "other_output_mult") {
					temp.ptr = other_output_mult.get();
				} else
				if(cname == "output_mult_to_output") {
					temp.ptr = output_mult_to_output.get();
				} else
				if(cname == "secondary_mult_to_ouput") {
					temp.ptr = secondary_mult_to_ouput.get();
				} else
				if(cname == "arrow_to_output") {
					temp.ptr = arrow_to_output.get();
				} else
				if(cname == "output") {
					temp.ptr = output.get();
				} else
				if(cname == "base_to_output") {
					temp.ptr = base_to_output.get();
				} else
				if(cname == "base_output") {
					temp.ptr = base_output.get();
				} else
				if(cname == "expected_profit_label") {
					temp.ptr = expected_profit_label.get();
				} else
				if(cname == "icon_unskilled") {
					temp.ptr = icon_unskilled.get();
				} else
				if(cname == "icon_skilled") {
					temp.ptr = icon_skilled.get();
				} else
				if(cname == "icon_expert") {
					temp.ptr = icon_expert.get();
				} else
				if(cname == "icon_output") {
					temp.ptr = icon_output.get();
				} else
				if(cname == "information_employment_units") {
					temp.ptr = information_employment_units.get();
				} else
				if(cname == "information_production_units") {
					temp.ptr = information_production_units.get();
				} else
				if(cname == "secondary_arrow") {
					temp.ptr = secondary_arrow.get();
				} else
				if(cname == "subsidies_label") {
					temp.ptr = subsidies_label.get();
				} else
				if(cname == "effective_output") {
					temp.ptr = effective_output.get();
				} else
				if(cname == "subsidies_explanation") {
					temp.ptr = subsidies_explanation.get();
				} else
				if(cname == "effective_output_to_subsidy") {
					temp.ptr = effective_output_to_subsidy.get();
				} else
				if(cname == "subsidy") {
					temp.ptr = subsidy.get();
				} else
				if(cname == "investment") {
					temp.ptr = investment.get();
				} else
				if(cname == "total_investments_label") {
					temp.ptr = total_investments_label.get();
				} else
				if(cname == "total_investments_value") {
					temp.ptr = total_investments_value.get();
				} else
				if(cname == "icon_split_investment") {
					temp.ptr = icon_split_investment.get();
				} else
				if(cname == "investment_expansion") {
					temp.ptr = investment_expansion.get();
				} else
				if(cname == "investment_efficiency") {
					temp.ptr = investment_efficiency.get();
				} else
				{
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::window2:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				buffer.read(temp.fill_x);
				buffer.read(temp.fill_y);
				if(cname == "main") {
					temp.ptr = make_factory_details_main(state);
				}
				if(cname == "commodity_row") {
					temp.ptr = make_factory_details_commodity_row(state);
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
			case layout_item_types::generator2:
			{
				generator_instance temp;
				std::string_view cname = buffer.read<std::string_view>();
				auto gen_details = buffer.read_section(); // ignored
				if(cname == "output_row") {
					temp.generator = &output_row;
				}
				if(cname == "inputs_row") {
					temp.generator = &inputs_row;
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
void factory_details_main_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("factory_details::main"));
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
		if(child_data.name == "total_wage_label") {
			total_wage_label = std::make_unique<template_label>();
			total_wage_label->parent = this;
			auto cptr = total_wage_label.get();
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
		if(child_data.name == "total_wage_value") {
			total_wage_value = std::make_unique<factory_details_main_total_wage_value_t>();
			total_wage_value->parent = this;
			auto cptr = total_wage_value.get();
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
		if(child_data.name == "expected_profit_value") {
			expected_profit_value = std::make_unique<factory_details_main_expected_profit_value_t>();
			expected_profit_value->parent = this;
			auto cptr = expected_profit_value.get();
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
		if(child_data.name == "wage_no_education") {
			wage_no_education = std::make_unique<factory_details_main_wage_no_education_t>();
			wage_no_education->parent = this;
			auto cptr = wage_no_education.get();
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
		if(child_data.name == "wage_basic_education") {
			wage_basic_education = std::make_unique<factory_details_main_wage_basic_education_t>();
			wage_basic_education->parent = this;
			auto cptr = wage_basic_education.get();
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
		if(child_data.name == "wage_secondary") {
			wage_secondary = std::make_unique<factory_details_main_wage_secondary_t>();
			wage_secondary->parent = this;
			auto cptr = wage_secondary.get();
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
		if(child_data.name == "no_education_arrow") {
			no_education_arrow = std::make_unique<template_icon_graphic>();
			no_education_arrow->parent = this;
			auto cptr = no_education_arrow.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "basic_education_arrow") {
			basic_education_arrow = std::make_unique<template_icon_graphic>();
			basic_education_arrow->parent = this;
			auto cptr = basic_education_arrow.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "hired_no_education") {
			hired_no_education = std::make_unique<factory_details_main_hired_no_education_t>();
			hired_no_education->parent = this;
			auto cptr = hired_no_education.get();
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
		if(child_data.name == "hired_basic_education") {
			hired_basic_education = std::make_unique<factory_details_main_hired_basic_education_t>();
			hired_basic_education->parent = this;
			auto cptr = hired_basic_education.get();
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
		if(child_data.name == "primary_to_eu_arrow") {
			primary_to_eu_arrow = std::make_unique<template_icon_graphic>();
			primary_to_eu_arrow->parent = this;
			auto cptr = primary_to_eu_arrow.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "employment_units") {
			employment_units = std::make_unique<factory_details_main_employment_units_t>();
			employment_units->parent = this;
			auto cptr = employment_units.get();
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
		if(child_data.name == "eu_to_pu_arrow") {
			eu_to_pu_arrow = std::make_unique<template_icon_graphic>();
			eu_to_pu_arrow->parent = this;
			auto cptr = eu_to_pu_arrow.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "secondary_to_wage_arrow") {
			secondary_to_wage_arrow = std::make_unique<template_icon_graphic>();
			secondary_to_wage_arrow->parent = this;
			auto cptr = secondary_to_wage_arrow.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "throughput_multiplier") {
			throughput_multiplier = std::make_unique<factory_details_main_throughput_multiplier_t>();
			throughput_multiplier->parent = this;
			auto cptr = throughput_multiplier.get();
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
		if(child_data.name == "throughput_to_pu_arrow") {
			throughput_to_pu_arrow = std::make_unique<template_icon_graphic>();
			throughput_to_pu_arrow->parent = this;
			auto cptr = throughput_to_pu_arrow.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "production_units_value") {
			production_units_value = std::make_unique<factory_details_main_production_units_value_t>();
			production_units_value->parent = this;
			auto cptr = production_units_value.get();
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
		if(child_data.name == "secondary_hired") {
			secondary_hired = std::make_unique<factory_details_main_secondary_hired_t>();
			secondary_hired->parent = this;
			auto cptr = secondary_hired.get();
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
		if(child_data.name == "secondary_to_mult") {
			secondary_to_mult = std::make_unique<template_icon_graphic>();
			secondary_to_mult->parent = this;
			auto cptr = secondary_to_mult.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "output_mult_secondary") {
			output_mult_secondary = std::make_unique<factory_details_main_output_mult_secondary_t>();
			output_mult_secondary->parent = this;
			auto cptr = output_mult_secondary.get();
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
		if(child_data.name == "pu_to_output_arrow") {
			pu_to_output_arrow = std::make_unique<template_icon_graphic>();
			pu_to_output_arrow->parent = this;
			auto cptr = pu_to_output_arrow.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "other_output_mult") {
			other_output_mult = std::make_unique<factory_details_main_other_output_mult_t>();
			other_output_mult->parent = this;
			auto cptr = other_output_mult.get();
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
		if(child_data.name == "output_mult_to_output") {
			output_mult_to_output = std::make_unique<template_icon_graphic>();
			output_mult_to_output->parent = this;
			auto cptr = output_mult_to_output.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "secondary_mult_to_ouput") {
			secondary_mult_to_ouput = std::make_unique<template_icon_graphic>();
			secondary_mult_to_ouput->parent = this;
			auto cptr = secondary_mult_to_ouput.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "arrow_to_output") {
			arrow_to_output = std::make_unique<template_icon_graphic>();
			arrow_to_output->parent = this;
			auto cptr = arrow_to_output.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "output") {
			output = std::make_unique<factory_details_main_output_t>();
			output->parent = this;
			auto cptr = output.get();
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
		if(child_data.name == "base_to_output") {
			base_to_output = std::make_unique<template_icon_graphic>();
			base_to_output->parent = this;
			auto cptr = base_to_output.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "base_output") {
			base_output = std::make_unique<factory_details_main_base_output_t>();
			base_output->parent = this;
			auto cptr = base_output.get();
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
		if(child_data.name == "expected_profit_label") {
			expected_profit_label = std::make_unique<template_label>();
			expected_profit_label->parent = this;
			auto cptr = expected_profit_label.get();
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
		if(child_data.name == "icon_unskilled") {
			icon_unskilled = std::make_unique<template_icon_graphic>();
			icon_unskilled->parent = this;
			auto cptr = icon_unskilled.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "icon_skilled") {
			icon_skilled = std::make_unique<template_icon_graphic>();
			icon_skilled->parent = this;
			auto cptr = icon_skilled.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "icon_expert") {
			icon_expert = std::make_unique<template_icon_graphic>();
			icon_expert->parent = this;
			auto cptr = icon_expert.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "icon_output") {
			icon_output = std::make_unique<factory_details_main_icon_output_t>();
			icon_output->parent = this;
			auto cptr = icon_output.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "information_employment_units") {
			information_employment_units = std::make_unique<factory_details_main_information_employment_units_t>();
			information_employment_units->parent = this;
			auto cptr = information_employment_units.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "information_production_units") {
			information_production_units = std::make_unique<factory_details_main_information_production_units_t>();
			information_production_units->parent = this;
			auto cptr = information_production_units.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "secondary_arrow") {
			secondary_arrow = std::make_unique<template_icon_graphic>();
			secondary_arrow->parent = this;
			auto cptr = secondary_arrow.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "subsidies_label") {
			subsidies_label = std::make_unique<template_label>();
			subsidies_label->parent = this;
			auto cptr = subsidies_label.get();
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
		if(child_data.name == "effective_output") {
			effective_output = std::make_unique<factory_details_main_effective_output_t>();
			effective_output->parent = this;
			auto cptr = effective_output.get();
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
		if(child_data.name == "subsidies_explanation") {
			subsidies_explanation = std::make_unique<template_icon_graphic>();
			subsidies_explanation->parent = this;
			auto cptr = subsidies_explanation.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "effective_output_to_subsidy") {
			effective_output_to_subsidy = std::make_unique<template_icon_graphic>();
			effective_output_to_subsidy->parent = this;
			auto cptr = effective_output_to_subsidy.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "subsidy") {
			subsidy = std::make_unique<factory_details_main_subsidy_t>();
			subsidy->parent = this;
			auto cptr = subsidy.get();
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
		if(child_data.name == "investment") {
			investment = std::make_unique<template_label>();
			investment->parent = this;
			auto cptr = investment.get();
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
		if(child_data.name == "total_investments_label") {
			total_investments_label = std::make_unique<template_label>();
			total_investments_label->parent = this;
			auto cptr = total_investments_label.get();
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
		if(child_data.name == "total_investments_value") {
			total_investments_value = std::make_unique<factory_details_main_total_investments_value_t>();
			total_investments_value->parent = this;
			auto cptr = total_investments_value.get();
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
		if(child_data.name == "icon_split_investment") {
			icon_split_investment = std::make_unique<template_icon_graphic>();
			icon_split_investment->parent = this;
			auto cptr = icon_split_investment.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "investment_expansion") {
			investment_expansion = std::make_unique<factory_details_main_investment_expansion_t>();
			investment_expansion->parent = this;
			auto cptr = investment_expansion.get();
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
		if(child_data.name == "investment_efficiency") {
			investment_efficiency = std::make_unique<factory_details_main_investment_efficiency_t>();
			investment_efficiency->parent = this;
			auto cptr = investment_efficiency.get();
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
		{ } 
		pending_children.pop_back();
	}
	output_row.on_create(state, this);
	inputs_row.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN main::create
// END
}
std::unique_ptr<ui::element_base> make_factory_details_main(sys::state& state) {
	auto ptr = std::make_unique<factory_details_main_t>();
	ptr->on_create(state);
	return ptr;
}
void factory_details_commodity_row_icon_t::on_update(sys::state& state) noexcept {
	factory_details_commodity_row_t& commodity_row = *((factory_details_commodity_row_t*)(parent)); 
	factory_details_main_t& main = *((factory_details_main_t*)(parent->parent)); 
// BEGIN commodity_row::icon::update
	content = commodity_row.commodity;
// END
}
void factory_details_commodity_row_name_t::on_update(sys::state& state) noexcept {
	factory_details_commodity_row_t& commodity_row = *((factory_details_commodity_row_t*)(parent)); 
	factory_details_main_t& main = *((factory_details_main_t*)(parent->parent)); 
// BEGIN commodity_row::name::update
	set_text(state,  text::produce_simple_string(state, state.world.commodity_get_name(commodity_row.commodity)));
// END
}
void factory_details_commodity_row_amount_value_t::on_update(sys::state& state) noexcept {
	factory_details_commodity_row_t& commodity_row = *((factory_details_commodity_row_t*)(parent)); 
	factory_details_main_t& main = *((factory_details_main_t*)(parent->parent)); 
// BEGIN commodity_row::amount_value::update
	set_text(state, text::format_float(commodity_row.amount));
// END
}
void factory_details_commodity_row_ratio_value_t::on_update(sys::state& state) noexcept {
	factory_details_commodity_row_t& commodity_row = *((factory_details_commodity_row_t*)(parent)); 
	factory_details_main_t& main = *((factory_details_main_t*)(parent->parent)); 
// BEGIN commodity_row::ratio_value::update
	set_text(state, text::format_float(commodity_row.ratio));
// END
}
void factory_details_commodity_row_price_value_t::on_update(sys::state& state) noexcept {
	factory_details_commodity_row_t& commodity_row = *((factory_details_commodity_row_t*)(parent)); 
	factory_details_main_t& main = *((factory_details_main_t*)(parent->parent)); 
// BEGIN commodity_row::price_value::update
	set_text(state, text::format_money(commodity_row.price));
// END
}
void factory_details_commodity_row_final_value_t::on_update(sys::state& state) noexcept {
	factory_details_commodity_row_t& commodity_row = *((factory_details_commodity_row_t*)(parent)); 
	factory_details_main_t& main = *((factory_details_main_t*)(parent->parent)); 
// BEGIN commodity_row::final_value::update
	set_text(state, text::format_money(commodity_row.price * commodity_row.amount * commodity_row.ratio));
// END
}
void  factory_details_commodity_row_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 2 : 4;
}
ui::message_result factory_details_commodity_row_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result factory_details_commodity_row_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void factory_details_commodity_row_t::on_update(sys::state& state) noexcept {
	factory_details_main_t& main = *((factory_details_main_t*)(parent->parent)); 
// BEGIN commodity_row::update
// END
	remake_layout(state, true);
}
void factory_details_commodity_row_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
			case layout_item_types::control2:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				buffer.read(temp.fill_x);
				buffer.read(temp.fill_y);
				temp.ptr = nullptr;
				if(cname == "icon") {
					temp.ptr = icon.get();
				} else
				if(cname == "name") {
					temp.ptr = name.get();
				} else
				if(cname == "amount_value") {
					temp.ptr = amount_value.get();
				} else
				if(cname == "ratio_value") {
					temp.ptr = ratio_value.get();
				} else
				if(cname == "price_value") {
					temp.ptr = price_value.get();
				} else
				if(cname == "arrow") {
					temp.ptr = arrow.get();
				} else
				if(cname == "final_value") {
					temp.ptr = final_value.get();
				} else
				{
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::window2:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				buffer.read(temp.fill_x);
				buffer.read(temp.fill_y);
				if(cname == "main") {
					temp.ptr = make_factory_details_main(state);
				}
				if(cname == "commodity_row") {
					temp.ptr = make_factory_details_commodity_row(state);
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
			case layout_item_types::generator2:
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
void factory_details_commodity_row_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("factory_details::commodity_row"));
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
		if(child_data.name == "icon") {
			icon = std::make_unique<factory_details_commodity_row_icon_t>();
			icon->parent = this;
			auto cptr = icon.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "name") {
			name = std::make_unique<factory_details_commodity_row_name_t>();
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
		if(child_data.name == "amount_value") {
			amount_value = std::make_unique<factory_details_commodity_row_amount_value_t>();
			amount_value->parent = this;
			auto cptr = amount_value.get();
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
		if(child_data.name == "ratio_value") {
			ratio_value = std::make_unique<factory_details_commodity_row_ratio_value_t>();
			ratio_value->parent = this;
			auto cptr = ratio_value.get();
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
		if(child_data.name == "price_value") {
			price_value = std::make_unique<factory_details_commodity_row_price_value_t>();
			price_value->parent = this;
			auto cptr = price_value.get();
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
		if(child_data.name == "arrow") {
			arrow = std::make_unique<template_icon_graphic>();
			arrow->parent = this;
			auto cptr = arrow.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "final_value") {
			final_value = std::make_unique<factory_details_commodity_row_final_value_t>();
			final_value->parent = this;
			auto cptr = final_value.get();
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
		{ } 
		pending_children.pop_back();
	}
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN commodity_row::create
// END
}
std::unique_ptr<ui::element_base> make_factory_details_commodity_row(sys::state& state) {
	auto ptr = std::make_unique<factory_details_commodity_row_t>();
	ptr->on_create(state);
	return ptr;
}
// LOST-CODE
}
