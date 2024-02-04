#pragma once

#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "gui_common_elements.hpp"
#include "province_templates.hpp"
#include "color.hpp"
#include "triggers.hpp"
#include "gui_province_window.hpp"

namespace ui {

std::vector<dcon::pop_id> const& get_pop_window_list(sys::state& state);
dcon::pop_id get_pop_details_pop(sys::state& state);
void describe_conversion(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_migration(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_colonial_migration(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_emigration(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_promotion_demotion(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_con(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_mil(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_lit(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_growth(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_assimilation(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);

class popwin_state_population : public state_population_text {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("provinceview_totalpop"), text::substitution_map{});
		text::close_layout_box(contents, box);
	}
};

class popwin_religion_type : public religion_type_icon {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			describe_conversion(state, contents, retrieve<dcon::pop_id>(state, parent));
		}
	}
};

class province_growth_indicator : public opaque_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::province_id>(state, parent);
		auto result = demographics::get_monthly_pop_increase(state, content);
		if(result > 0) {
			frame = 0;
		} else if(result < 0) {
			frame = 2;
		} else {
			frame = 1;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {

			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, "pop_growth_1");
			auto content = retrieve<dcon::province_id>(state, parent);
			auto result = demographics::get_monthly_pop_increase(state, content);

			if(result >= 0) {
				text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
				text::add_to_layout_box(state, contents, box, int64_t(result), text::text_color::green);
			} else {
				text::add_to_layout_box(state, contents, box, int64_t(result), text::text_color::red);
			}
			text::close_layout_box(contents, box);

	}
};

class state_growth_indicator : public opaque_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		auto result = demographics::get_monthly_pop_increase(state, content);
		if(result > 0) {
			frame = 0;
		} else if(result < 0) {
			frame = 2;
		} else {
			frame = 1;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {

			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, "pop_growth_1");
			auto content = retrieve<dcon::state_instance_id>(state, parent);
			auto result = demographics::get_monthly_pop_increase(state, content);

			if(result >= 0) {
				text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
				text::add_to_layout_box(state, contents, box, int64_t(result), text::text_color::green);
			} else {
				text::add_to_layout_box(state, contents, box, int64_t(result), text::text_color::red);
			}
			text::close_layout_box(contents, box);

	}
};

class nation_growth_indicator : public opaque_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::nation_id>(state, parent);
		auto result = demographics::get_monthly_pop_increase(state, content);
		if(result > 0) {
			frame = 0;
		} else if(result < 0) {
			frame = 2;
		} else {
			frame = 1;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {

			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, "pop_growth_1");
			auto content = retrieve<dcon::nation_id>(state, parent);
			auto result = demographics::get_monthly_pop_increase(state, content);

			if(result >= 0) {
				text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
				text::add_to_layout_box(state, contents, box, int64_t(result), text::text_color::green);
			} else {
				text::add_to_layout_box(state, contents, box, int64_t(result), text::text_color::red);
			}
			text::close_layout_box(contents, box);

	}
};

class pop_revolt_faction : public opaque_element_base {
public:
	bool show = false;

	int32_t get_icon_frame(sys::state& state) noexcept {
		auto content = retrieve<dcon::pop_id>(state, parent);

		auto fat_id = dcon::fatten(state.world, content);
		auto rebel_fact = fat_id.get_pop_rebellion_membership().get_rebel_faction().get_type();
		if(rebel_fact) {
			show = true;
			auto icon = rebel_fact.get_icon();
			return int32_t(icon - 1);
		} else {
			show = false;
			return int32_t(0);
		}
	}

	void on_update(sys::state& state) noexcept override {
		frame = get_icon_frame(state);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			opaque_element_base::render(state, x, y);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return show ? tooltip_behavior::variable_tooltip : tooltip_behavior::no_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(show) {
			auto content = retrieve<dcon::pop_id>(state, parent);
			auto fat_id = dcon::fatten(state.world, content);
			auto rebel_fact = fat_id.get_pop_rebellion_membership().get_rebel_faction().get_type();
			auto box = text::open_layout_box(contents, 0);
			if(rebel_fact) {
				text::add_to_layout_box(state, contents, box, rebel_fact.get_name());
				text::add_divider_to_layout_box(state, contents, box);
				text::add_to_layout_box(state, contents, box, rebel_fact.get_description());
			}
			text::close_layout_box(contents, box);
		}
	}
};

class pop_movement_social : public opaque_element_base {
public:

	bool show = false;
	int32_t get_icon_frame(sys::state& state) noexcept {
		Cyto::Any payload = dcon::pop_id{};
		parent->impl_get(state, payload);
		auto content = retrieve<dcon::pop_id>(state, parent);

		auto fat_id = dcon::fatten(state.world, content);
		auto movement_fact = fat_id.get_pop_movement_membership().get_movement().get_associated_issue_option();
		if(movement_fact) {
			auto parent_issue = movement_fact.get_parent_issue();
			if(parent_issue.get_issue_type() != uint8_t(culture::issue_category::social)) {
				show = false;
			} else {
				int32_t count = 0;
				for(; count < int32_t(state.culture_definitions.social_issues.size()); ++count) {
					if(state.culture_definitions.social_issues[count] == parent_issue)
						break;
				}
				show = true;
				return count;
			}
		}
		show = false;
		return int32_t(0);
	}

	void on_update(sys::state& state) noexcept override {
		frame = get_icon_frame(state);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return show ? tooltip_behavior::variable_tooltip : tooltip_behavior::no_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(show) {
			auto content = retrieve<dcon::pop_id>(state, parent);
			auto fat_id = dcon::fatten(state.world, content);
			auto movement_fact = fat_id.get_pop_movement_membership();
			auto box = text::open_layout_box(contents, 0);
			if(movement_fact) {
				auto movement_issue = movement_fact.get_movement().get_associated_issue_option();
				text::add_to_layout_box(state, contents, box, movement_issue.get_movement_name());
			}
			text::close_layout_box(contents, box);
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			opaque_element_base::render(state, x, y);
	}
};

class pop_movement_political : public opaque_element_base {
public:
	bool show = false;

	int32_t get_icon_frame(sys::state& state) noexcept {
		auto content = retrieve<dcon::pop_id>(state, parent);
		auto fat_id = dcon::fatten(state.world, content);
		auto movement_fact = fat_id.get_pop_movement_membership().get_movement().get_associated_issue_option();
		if(movement_fact) {
			auto parent_issue = movement_fact.get_parent_issue();
			if(parent_issue.get_issue_type() != uint8_t(culture::issue_category::political)) {
				show = false;
			} else {
				int32_t count = 0;
				for(; count < int32_t(state.culture_definitions.political_issues.size()); ++count) {
					if(state.culture_definitions.political_issues[count] == parent_issue)
						break;
				}
				show = true;
				return count;
			}
		}
		show = false;
		return int32_t(0);
	}

	void on_update(sys::state& state) noexcept override {
		frame = get_icon_frame(state);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return show ? tooltip_behavior::variable_tooltip : tooltip_behavior::no_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(show) {
			auto content = retrieve<dcon::pop_id>(state, parent);
			auto fat_id = dcon::fatten(state.world, content);
			auto movement_fact = fat_id.get_pop_movement_membership();
			auto box = text::open_layout_box(contents, 0);
			if(movement_fact) {
				auto movement_issue = movement_fact.get_movement().get_associated_issue_option();
				text::add_to_layout_box(state, contents, box, movement_issue.get_movement_name());
			}
			text::close_layout_box(contents, box);
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			opaque_element_base::render(state, x, y);
	}
};

class pop_movement_flag : public flag_button {
public:
	bool show = false;

	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto pop = retrieve<dcon::pop_id>(state, parent);
		auto movement = state.world.pop_get_movement_from_pop_movement_membership(pop);
		if(movement) {
			if(auto id = state.world.movement_get_associated_independence(movement); id) {
				show = true;
				return id;
			}
		}
		show = false;
		// TODO: check if independence factions need the flag instead

		return dcon::national_identity_id{};
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return show ? flag_button::has_tooltip(state) : tooltip_behavior::no_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(show)
			flag_button::update_tooltip(state, x, y, contents);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			flag_button::render(state, x, y);
	}

	void button_action(sys::state&) noexcept override { }
};

class pop_cash_reserve_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::pop_id>(state, parent);
		set_text(state, text::format_money(state.world.pop_get_savings(content)));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::pop_id>(state, parent);

		auto fat_id = dcon::fatten(state.world, content);
		auto box = text::open_layout_box(contents, 0);
		text::localised_single_sub_box(state, contents, box, std::string_view("pop_daily_money"), text::variable_type::val,
				text::fp_currency{state.world.pop_get_savings(fat_id.id)});
		text::add_divider_to_layout_box(state, contents, box);
		text::localised_single_sub_box(state, contents, box, std::string_view("pop_daily_needs"), text::variable_type::val,
				text::fp_currency{1984});
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_single_sub_box(state, contents, box, std::string_view("pop_daily_salary"), text::variable_type::val,
				text::fp_currency{1984});
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_single_sub_box(state, contents, box, std::string_view("available_in_bank"), text::variable_type::val,
				text::fp_currency{1984});
		text::close_layout_box(contents, box);
	}
};
class pop_size_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::pop_id>(state, parent);

		auto const fat_id = dcon::fatten(state.world, content);
		set_text(state, std::to_string(int32_t(fat_id.get_size())));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto pop = retrieve<dcon::pop_id>(state, parent);
		auto growth = int64_t(demographics::get_monthly_pop_increase(state, pop));
		auto promote = -int64_t(demographics::get_estimated_type_change(state, pop));
		auto assimilation = -int64_t(demographics::get_estimated_assimilation(state, pop));
		auto conversion = -int64_t(demographics::get_estimated_conversion(state, pop));
		auto internal_migration = -int64_t(demographics::get_estimated_internal_migration(state, pop));
		auto colonial_migration = -int64_t(demographics::get_estimated_colonial_migration(state, pop));
		auto emigration = -int64_t(demographics::get_estimated_emigration(state, pop));
		auto total = int64_t(growth) + promote + assimilation + conversion + internal_migration + colonial_migration + emigration;

		{
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, "pop_size_1");
			if(total >= 0) {
				text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
				text::add_to_layout_box(state, contents, box, total, text::text_color::green);
			} else {
				text::add_to_layout_box(state, contents, box, total, text::text_color::red);
			}
			text::close_layout_box(contents, box);
		}
		text::add_line_break_to_layout(state, contents);
		{
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, "pop_size_2");
			if(growth >= 0) {
				text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
				text::add_to_layout_box(state, contents, box, growth, text::text_color::green);
			} else {
				text::add_to_layout_box(state, contents, box, growth, text::text_color::red);
			}
			text::close_layout_box(contents, box);
		}
		{
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, "pop_size_3");
			if(promote >= 0) {
				text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
				text::add_to_layout_box(state, contents, box, promote, text::text_color::green);
			} else {
				text::add_to_layout_box(state, contents, box, promote, text::text_color::red);
			}
			text::close_layout_box(contents, box);
		}
		{
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, "pop_size_4");
			if(assimilation >= 0) {
				text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
				text::add_to_layout_box(state, contents, box, assimilation, text::text_color::green);
			} else {
				text::add_to_layout_box(state, contents, box, assimilation, text::text_color::red);
			}
			text::close_layout_box(contents, box);
		}
		{
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, "pop_size_5");
			if(internal_migration >= 0) {
				text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
				text::add_to_layout_box(state, contents, box, internal_migration, text::text_color::green);
			} else {
				text::add_to_layout_box(state, contents, box, internal_migration, text::text_color::red);
			}
			text::close_layout_box(contents, box);
		}
		{
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, "pop_size_6");
			if(colonial_migration >= 0) {
				text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
				text::add_to_layout_box(state, contents, box, colonial_migration, text::text_color::green);
			} else {
				text::add_to_layout_box(state, contents, box, colonial_migration, text::text_color::red);
			}
			text::close_layout_box(contents, box);
		}
		{
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, "pop_size_7");
			if(emigration >= 0) {
				text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
				text::add_to_layout_box(state, contents, box, emigration, text::text_color::green);
			} else {
				text::add_to_layout_box(state, contents, box, emigration, text::text_color::red);
			}
			text::close_layout_box(contents, box);
		}
		{
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, "pop_size_8");
			if(conversion >= 0) {
				text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
				text::add_to_layout_box(state, contents, box, conversion, text::text_color::green);
			} else {
				text::add_to_layout_box(state, contents, box, conversion, text::text_color::red);
			}
			text::close_layout_box(contents, box);
		}
	}
};

class pop_location_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto pop = retrieve<dcon::pop_id>(state, parent);
		auto loc = state.world.pop_get_province_from_pop_location(pop);
		set_text(state, text::produce_simple_string(state, state.world.province_get_name(loc)));
	}

};
class pop_militancy_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::format_float(state.world.pop_get_militancy(retrieve<dcon::pop_id>(state, parent))));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		describe_mil(state, contents, retrieve<dcon::pop_id>(state, parent));
	}
};
class pop_con_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::format_float(state.world.pop_get_consciousness(retrieve<dcon::pop_id>(state, parent))));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		describe_con(state, contents, retrieve<dcon::pop_id>(state, parent));
	}
};
class pop_literacy_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			set_text(state, text::format_percentage(state.world.pop_get_literacy(retrieve<dcon::pop_id>(state, parent)), 2));
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		describe_lit(state, contents, retrieve<dcon::pop_id>(state, parent));
	}
};
class pop_culture_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::pop_id>(state, parent);
		set_text(state, text::produce_simple_string(state, dcon::fatten(state.world, content).get_culture().get_name()));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		describe_assimilation(state, contents, retrieve<dcon::pop_id>(state, parent));
	}
};

class pop_growth_indicator : public opaque_element_base {
public:
	int32_t get_icon_frame(sys::state& state) noexcept {
		auto content = retrieve<dcon::pop_id>(state, parent);

		// 0 == Going up
		// 1 == Staying same
		// 2 == Going down
		auto result = int64_t(demographics::get_monthly_pop_increase(state, content));
		if(result > 0) {
			return 0;
		} else if(result < 0) {
			return 2;
		} else {
			return 1;
		}
	}

	void on_update(sys::state& state) noexcept override {
		frame = get_icon_frame(state);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		describe_growth(state, contents, retrieve<dcon::pop_id>(state, parent));
	}
};


enum class pop_list_sort : uint8_t {
	size,
	type,
	nationality,
	religion,
	location,
	mil,
	con,
	ideology,
	issues,
	unemployment,
	cash,
	life_needs,
	everyday_needs,
	luxury_needs,
	revoltrisk,
	change,
	literacy
};

class standard_pop_progress_bar : public progress_bar {
public:
	virtual float get_progress(sys::state& state, dcon::pop_id content) noexcept {
		return 0.f;
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::pop_id>(state, parent);
		progress = get_progress(state, content);
	}

	void on_create(sys::state& state) noexcept override {
		base_data.position.x -= 14;
		base_data.position.y -= 12;
		base_data.size.y = 25;
		base_data.size.x = 13;
	}
};
class standard_pop_needs_progress_bar : public progress_bar {
public:
	virtual float get_progress(sys::state& state, dcon::pop_id content) noexcept {
		return 0.f;
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::pop_id>(state, parent);
		progress = get_progress(state, content);
	}

	void on_create(sys::state& state) noexcept override {
		base_data.position.x -= 15;
		base_data.position.y -= 4;
		base_data.size.y = 20;
		base_data.size.x = 15;
	}
};

class pop_unemployment_progress_bar : public standard_pop_progress_bar {
public:
	float get_progress(sys::state& state, dcon::pop_id content) noexcept override {
		auto pfat_id = dcon::fatten(state.world, content);
		if(state.world.pop_type_get_has_unemployment(state.world.pop_get_poptype(content)))
			return (1 - pfat_id.get_employment() / pfat_id.get_size());
		return 0.0f;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::pop_id>(state, parent);

		auto pfat_id = dcon::fatten(state.world, content);
		float un_empl = state.world.pop_type_get_has_unemployment(state.world.pop_get_poptype(content))
												? (1 - pfat_id.get_employment() / pfat_id.get_size())
												: 0.0f;
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("unemployment"), text::substitution_map{});
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage{un_empl});
		text::close_layout_box(contents, box);
	}
};
class pop_life_needs_progress_bar : public standard_pop_needs_progress_bar {
public:
	float get_progress(sys::state& state, dcon::pop_id content) noexcept override {
		auto fat_id = dcon::fatten(state.world, content);
		return fat_id.get_life_needs_satisfaction();
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::pop_id>(state, parent);

		auto fat_id = dcon::fatten(state.world, content);
		auto need = text::produce_simple_string(state, "life_needs");
		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::need, std::string_view(need));
		text::add_to_substitution_map(sub, text::variable_type::val, text::fp_one_place{fat_id.get_life_needs_satisfaction() * 100.f});
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("getting_needs"), sub);
		text::close_layout_box(contents, box);
	}
};

class pop_everyday_needs_progress_bar : public standard_pop_needs_progress_bar {
public:
	float get_progress(sys::state& state, dcon::pop_id content) noexcept override {
		auto fat_id = dcon::fatten(state.world, content);
		return fat_id.get_everyday_needs_satisfaction();
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::pop_id>(state, parent);

		auto fat_id = dcon::fatten(state.world, content);
		auto need = text::produce_simple_string(state, "everyday_needs");
		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::need, std::string_view(need));
		text::add_to_substitution_map(sub, text::variable_type::val, text::fp_one_place{fat_id.get_everyday_needs_satisfaction() * 100.f });
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("getting_needs"), sub);
		text::close_layout_box(contents, box);
	}
};

class pop_luxury_needs_progress_bar : public standard_pop_needs_progress_bar {
public:
	float get_progress(sys::state& state, dcon::pop_id content) noexcept override {
		auto fat_id = dcon::fatten(state.world, content);
		return fat_id.get_luxury_needs_satisfaction();
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::pop_id>(state, parent);

		auto fat_id = dcon::fatten(state.world, content);
		auto need = text::produce_simple_string(state, "luxury_needs");
		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::need, std::string_view(need));
		text::add_to_substitution_map(sub, text::variable_type::val, text::fp_one_place{fat_id.get_luxury_needs_satisfaction() * 100.f });
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("getting_needs"), sub);
		text::close_layout_box(contents, box);
	}
};

class pop_issues_piechart : public demographic_piechart<dcon::pop_id, dcon::issue_option_id> {
protected:
	void for_each_demo(sys::state& state, std::function<void(dcon::issue_option_id)> fun) override {
		state.world.for_each_issue_option(fun);
	}

public:
	void on_create(sys::state& state) noexcept override {
		base_data.size.x = 28;
		base_data.size.y = 28;
		base_data.position.x -= 13;
		radius = 13;
	}
};
class pop_ideology_piechart : public demographic_piechart<dcon::pop_id, dcon::ideology_id> {
protected:
	void for_each_demo(sys::state& state, std::function<void(dcon::ideology_id)> fun) override {
		state.world.for_each_ideology(fun);
	}

public:
	void on_create(sys::state& state) noexcept override {
		base_data.size.x = 28;
		base_data.size.y = 28;
		base_data.position.x -= 13;
		radius = 13;
	}
};

using pop_left_side_data = std::variant< std::monostate, dcon::nation_id, dcon::state_instance_id, dcon::province_id>;

template<typename T>
class pop_left_side_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		T id = retrieve<T>(state, parent);
		if(state.ui_state.population_subwindow) {
			Cyto::Any filter_payload = pop_list_filter{};
			state.ui_state.population_subwindow->impl_get(state, filter_payload);
			auto filter = any_cast<pop_list_filter>(filter_payload);
			frame = std::holds_alternative<T>(filter) && std::get<T>(filter) == id ? 1 : 0;
		}
	}

	void button_action(sys::state& state) noexcept override {
		T id = retrieve<T>(state, parent);
		if(state.ui_state.population_subwindow) {
			Cyto::Any new_payload = pop_list_filter(id);
			state.ui_state.population_subwindow->impl_set(state, new_payload);
		}
		on_update(state);
	}
};
class pop_left_side_country_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "poplistbutton") {
			return make_element_by_type<pop_left_side_button<dcon::nation_id>>(state, id);
		} else if(name == "poplist_name") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} else if(name == "poplist_numpops") {
			return make_element_by_type<nation_population_text>(state, id);
		} else if(name == "growth_indicator") {
			return make_element_by_type<nation_growth_indicator>(state, id);
		} else {
			return nullptr;
		}
	}
};

using pop_left_side_expand_action =  std::variant< std::monostate, dcon::state_instance_id, bool> ;

class pop_left_side_expand_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, "");
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			auto id = retrieve<dcon::state_instance_id>(state, parent);

			Cyto::Any payload = pop_left_side_expand_action(id);
			parent->impl_get(state, payload);
			frame = std::get<bool>(any_cast<pop_left_side_expand_action>(payload)) ? 1 : 0;
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			auto id = retrieve<dcon::state_instance_id>(state, parent);
			if(state.ui_state.population_subwindow) {
				Cyto::Any new_payload = pop_left_side_expand_action(id);
				state.ui_state.population_subwindow->impl_set(state, new_payload);
			}
			on_update(state);
		}
	}
};

class pop_national_focus_button : public right_click_button_element_base {
public:
	int32_t get_icon_frame(sys::state& state) noexcept {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		return bool(state.world.state_instance_get_owner_focus(content).id)
								? state.world.state_instance_get_owner_focus(content).get_icon() - 1
								: 0;
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		disabled = true;
		state.world.for_each_national_focus([&](dcon::national_focus_id nfid) {
			disabled = command::can_set_national_focus(state, state.local_player_nation, content, nfid) ? false : disabled;
		});
		frame = get_icon_frame(state);
	}

	void button_action(sys::state& state) noexcept override;
	void button_right_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		command::set_national_focus(state, state.local_player_nation, content, dcon::national_focus_id{});
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);

		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		auto fat_si = dcon::fatten(state.world, sid);
		text::add_to_layout_box(state, contents, box, sid);
		text::add_line_break_to_layout_box(state, contents, box);
		auto content = state.world.state_instance_get_owner_focus(sid);
		if(bool(content)) {
			auto fat_nf = dcon::fatten(state.world, content);
			text::add_to_layout_box(state, contents, box, state.world.national_focus_get_name(content), text::substitution_map{});
			text::add_line_break_to_layout_box(state, contents, box);
			auto color = text::text_color::white;
			if(fat_nf.get_promotion_type()) {
				//Is the NF not optimal? Recolor it
				if(fat_nf.get_promotion_type() == state.culture_definitions.clergy) {
					if((fat_si.get_demographics(demographics::to_key(state, fat_nf.get_promotion_type())) / fat_si.get_demographics(demographics::total)) > state.defines.max_clergy_for_literacy) {
						color = text::text_color::red;
					}
				} else if(fat_nf.get_promotion_type() == state.culture_definitions.bureaucrat) {
					if(province::state_admin_efficiency(state, fat_si.id) > state.defines.max_bureaucracy_percentage) {
						color = text::text_color::red;
					}
				}
				auto full_str = text::format_percentage(fat_si.get_demographics(demographics::to_key(state, fat_nf.get_promotion_type())) / fat_si.get_demographics(demographics::total));
				text::add_to_layout_box(state, contents, box, std::string_view(full_str), color);
			}
		}
		text::close_layout_box(contents, box);
		if(auto mid = state.world.national_focus_get_modifier(content);  mid) {
			modifier_description(state, contents, mid, 15);
		}
		text::add_line(state, contents, "alice_nf_controls");
	}
};

class pop_left_side_state_window : public window_element_base {
	province_colony_button* colonial_icon = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "poplistbutton") {
			return make_element_by_type<pop_left_side_button<dcon::state_instance_id>>(state, id);
		} else if(name == "poplist_name") {
			return make_element_by_type<state_name_text>(state, id);
		} else if(name == "poplist_numpops") {
			return make_element_by_type<popwin_state_population>(state, id);
		} else if(name == "colonial_state_icon") {
			auto ptr = make_element_by_type<province_colony_button>(state, id);
			colonial_icon = ptr.get();
			return ptr;
		} else if(name == "state_focus") {
			return make_element_by_type<pop_national_focus_button>(state, id);
		} else if(name == "expand") {
			return make_element_by_type<pop_left_side_expand_button>(state, id);
		} else if(name == "growth_indicator") {
			return make_element_by_type<state_growth_indicator>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		colonial_icon->set_visible(state, state.world.province_get_is_colonial(state.world.state_instance_get_capital(content)));
	}
};
class pop_left_side_province_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "poplistbutton") {
			return make_element_by_type<pop_left_side_button<dcon::province_id>>(state, id);
		} else if(name == "poplist_name") {
			return make_element_by_type<generic_name_text<dcon::province_id>>(state, id);
		} else if(name == "poplist_numpops") {
			return make_element_by_type<province_population_text>(state, id);
		} else if(name == "growth_indicator") {
			return make_element_by_type<province_growth_indicator>(state, id);
		} else {
			return nullptr;
		}
	}
};

class pop_left_side_item : public listbox_row_element_base<pop_left_side_data> {
	pop_left_side_country_window* country_window = nullptr;
	pop_left_side_state_window* state_window = nullptr;
	pop_left_side_province_window* province_window = nullptr;

public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base<pop_left_side_data>::on_create(state);

		auto ptr1 = make_element_by_type<pop_left_side_country_window>(state,
				state.ui_state.defs_by_name.find("poplistitem_country")->second.definition);
		country_window = ptr1.get();
		add_child_to_back(std::move(ptr1));

		auto ptr2 = make_element_by_type<pop_left_side_state_window>(state,
				state.ui_state.defs_by_name.find("poplistitem_state")->second.definition);
		state_window = ptr2.get();
		add_child_to_back(std::move(ptr2));

		auto ptr3 = make_element_by_type<pop_left_side_province_window>(state,
				state.ui_state.defs_by_name.find("poplistitem_province")->second.definition);
		province_window = ptr3.get();
		add_child_to_back(std::move(ptr3));
		// After this, the widget will be immediately set by the parent
	}

	void on_update(sys::state& state) noexcept override {
		country_window->set_visible(state, std::holds_alternative<dcon::nation_id>(content));
		state_window->set_visible(state, std::holds_alternative<dcon::state_instance_id>(content));
		province_window->set_visible(state, std::holds_alternative<dcon::province_id>(content));
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<pop_left_side_data>()) {
			payload.emplace<pop_left_side_data>(content);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::province_id>()) {
			if(std::holds_alternative<dcon::province_id>(content))
				payload.emplace<dcon::province_id>(std::get<dcon::province_id>(content));
			return message_result::consumed;
		} else if(payload.holds_type<dcon::state_instance_id>()) {
			if(std::holds_alternative<dcon::state_instance_id>(content))
				payload.emplace<dcon::state_instance_id>(std::get<dcon::state_instance_id>(content));
			return message_result::consumed;
		} else if(payload.holds_type<dcon::nation_id>()) {
			if(std::holds_alternative<dcon::nation_id>(content))
				payload.emplace<dcon::nation_id>(std::get<dcon::nation_id>(content));
			return message_result::consumed;
		}
		return listbox_row_element_base<pop_left_side_data>::get(state, payload);
	}
};

class pop_left_side_listbox : public listbox_element_base<pop_left_side_item, pop_left_side_data> {
protected:
	std::string_view get_row_element_name() override {
		return "pop_left_side_list_base_window";
	}
};

template<typename T>
class pop_distribution_plupp : public tinted_image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		color = ogl::get_ui_color<T>(state, retrieve<T>(state, parent));
	}
};

template<typename T, bool Multiple>
class pop_distribution_piechart : public piechart<T> {
	void iterate_one_pop(sys::state& state, dcon::pop_id pop_id) {
		auto const weight_fn = [&](T id, float weight) {
			auto it = std::find_if(this->distribution.begin(), this->distribution.end(), [id](auto& e) { return e.key == id;  });
			if(it != this->distribution.end())
				it->value += weight;
			else
				this->distribution.emplace_back(id, weight);
		};

		if constexpr(std::is_same_v<T, dcon::issue_option_id>) {
			for(auto iopt : state.world.in_issue_option) {
				weight_fn(iopt, state.world.pop_get_demographics(pop_id, pop_demographics::to_key(state, iopt)));
			}
		} else if constexpr(std::is_same_v<T, dcon::ideology_id>) {
			for(auto iopt : state.world.in_ideology) {
				weight_fn(iopt, state.world.pop_get_demographics(pop_id, pop_demographics::to_key(state, iopt)));
			}
		} else if constexpr(std::is_same_v<T, dcon::political_party_id>) {
			auto prov_id = state.world.pop_location_get_province(state.world.pop_get_pop_location_as_pop(pop_id));
			if(!state.world.province_get_is_colonial(prov_id)) {
				auto tag = state.world.nation_get_identity_from_identity_holder(
						state.world.province_get_nation_from_province_ownership(prov_id));
				auto start = state.world.national_identity_get_political_party_first(tag).id.index();
				auto end = start + state.world.national_identity_get_political_party_count(tag);
				for(int32_t i = start; i < end; i++) {
					auto pid = T(typename T::value_base_t(i));
					if(politics::political_party_is_active(state, pid)) {
						auto support = politics::party_total_support(state, pop_id, pid,
								state.world.province_get_nation_from_province_ownership(prov_id), prov_id);
						weight_fn(pid, support);
					}
				}
			}
		} else if constexpr(std::is_same_v<T, dcon::culture_id>) {
			auto size = state.world.pop_get_size(pop_id);
			weight_fn(state.world.pop_get_culture(pop_id).id, size);
		} else if constexpr(std::is_same_v<T, dcon::religion_id>) {
			auto size = state.world.pop_get_size(pop_id);
			weight_fn(state.world.pop_get_religion(pop_id).id, size);
		} else if constexpr(std::is_same_v<T, dcon::pop_type_id>) {
			auto size = state.world.pop_get_size(pop_id);
			weight_fn(state.world.pop_get_poptype(pop_id).id, size);
		}
	}

protected:
	void on_update(sys::state& state) noexcept override {
		piechart<T>::distribution.clear();

		if(piechart<T>::parent) {
			if constexpr(Multiple) {
				auto& pop_list = get_pop_window_list(state);
				for(auto const pop_id : pop_list)
					iterate_one_pop(state, pop_id);
			} else {
				iterate_one_pop(state, get_pop_details_pop(state));
			}
		}

		piechart<T>::update_chart(state);
	}

public:
	void on_create(sys::state& state) noexcept override {
		piechart<T>::on_create(state);
		// piechart<T>::base_data.position.x -= piechart<T>::base_data.size.x;
		piechart<T>::base_data.position.x -= 17;
		piechart<T>::radius = float(piechart<T>::base_data.size.x);
		piechart<T>::base_data.size.x *= 2;
		piechart<T>::base_data.size.y *= 2;
	}
};

template<typename T>
class pop_distribution_item : public listbox_row_element_base<std::pair<T, float>> {
	element_base* title_text = nullptr;
	simple_text_element_base* value_text = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "legend_color") {
			return make_element_by_type<pop_distribution_plupp<T>>(state, id);
		} else if(name == "legend_title") {
			return make_element_by_type<generic_name_text<T>>(state, id);
		} else if(name == "legend_value") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			value_text = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		value_text->set_text(state, text::format_percentage(listbox_row_element_base<std::pair<T, float>>::content.second, 1));
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<T>()) {
			payload.emplace<T>(listbox_row_element_base<std::pair<T, float>>::content.first);
			return message_result::consumed;
		}
		return listbox_row_element_base<std::pair<T, float>>::get(state, payload);
	}
};

template<typename T>
class pop_distribution_listbox : public listbox_element_base<pop_distribution_item<T>, std::pair<T, float>> {
public:
	std::string_view get_row_element_name() override {
		return "pop_legend_item";
	}
};

template<typename T, bool Multiple>
class pop_distribution_window : public window_element_base {
	pop_distribution_listbox<T>* distrib_listbox;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "item_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			if constexpr(std::is_same_v<T, dcon::issue_option_id>) {
				ptr->set_text(state, text::produce_simple_string(state, "dominant_issues_disttitle"));
			} else if constexpr(std::is_same_v<T, dcon::culture_id>) {
				ptr->set_text(state, text::produce_simple_string(state, "nationality_disttitle"));
			} else if constexpr(std::is_same_v<T, dcon::ideology_id>) {
				ptr->set_text(state, text::produce_simple_string(state, "ideology_disttitle"));
			} else if constexpr(std::is_same_v<T, dcon::religion_id>) {
				ptr->set_text(state, text::produce_simple_string(state, "religion_disttitle"));
			} else if constexpr(std::is_same_v<T, dcon::pop_type_id>) {
				ptr->set_text(state, text::produce_simple_string(state, "workforce_disttitle"));
			} else if constexpr(std::is_same_v<T, dcon::political_party_id>) {
				ptr->set_text(state, text::produce_simple_string(state, "electorate_disttitle"));
			}
			return ptr;
		} else if(name == "chart") {
			return make_element_by_type<pop_distribution_piechart<T, Multiple>>(state, id);
		} else if(name == "member_names") {
			auto ptr = make_element_by_type<pop_distribution_listbox<T>>(state, id);
			distrib_listbox = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			auto& pop_list = get_pop_window_list(state);

			std::unordered_map<typename T::value_base_t, float> distrib{};
			for(auto const pop_id : pop_list) {
				auto const weight_fn = [&](auto id) {
					auto weight = state.world.pop_get_demographics(pop_id, pop_demographics::to_key(state, id));
					distrib[typename T::value_base_t(id.index())] += weight;
				};
				// Can obtain via simple pop_demographics query
				if constexpr(std::is_same_v<T, dcon::issue_option_id>)
					state.world.for_each_issue_option(weight_fn);
				else if constexpr(std::is_same_v<T, dcon::ideology_id>)
					state.world.for_each_ideology(weight_fn);
				// Needs to be queried directly from the pop
				if constexpr(std::is_same_v<T, dcon::culture_id>)
					distrib[typename T::value_base_t(state.world.pop_get_culture(pop_id).id.index())] += state.world.pop_get_size(pop_id);
				else if constexpr(std::is_same_v<T, dcon::religion_id>)
					distrib[typename T::value_base_t(state.world.pop_get_religion(pop_id).id.index())] += state.world.pop_get_size(pop_id);
				else if constexpr(std::is_same_v<T, dcon::pop_type_id>)
					distrib[typename T::value_base_t(state.world.pop_get_poptype(pop_id).id.index())] += state.world.pop_get_size(pop_id);
				else if constexpr(std::is_same_v<T, dcon::political_party_id>) {
					auto prov_id = state.world.pop_location_get_province(state.world.pop_get_pop_location_as_pop(pop_id));
					if(state.world.province_get_is_colonial(prov_id))
						continue;
					auto tag = state.world.nation_get_identity_from_identity_holder(
							state.world.province_get_nation_from_province_ownership(prov_id));
					auto start = state.world.national_identity_get_political_party_first(tag).id.index();
					auto end = start + state.world.national_identity_get_political_party_count(tag);
					for(int32_t i = start; i < end; i++) {
						auto pid = T(typename T::value_base_t(i));
						if(politics::political_party_is_active(state, pid)) {
							auto support = politics::party_total_support(state, pop_id, pid,
									state.world.province_get_nation_from_province_ownership(prov_id), prov_id);
							distrib[typename T::value_base_t(pid.index())] += support;
						}
					}
				}
			}

			std::vector<std::pair<T, float>> sorted_distrib{};
			for(auto const& e : distrib)
				if(e.second > 0.f)
					sorted_distrib.emplace_back(T(e.first), e.second);
			std::sort(sorted_distrib.begin(), sorted_distrib.end(),
					[&](std::pair<T, float> a, std::pair<T, float> b) { return a.second > b.second; });

			distrib_listbox->row_contents.clear();
			// Add (and scale elements) into the distribution listbox
			auto total = 0.f;
			for(auto const& e : sorted_distrib)
				total += e.second;
			for(auto const& e : sorted_distrib)
				distrib_listbox->row_contents.emplace_back(e.first, e.second / total);
			distrib_listbox->update(state);
		}
	}
};

class issue_with_explanation : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto issue = retrieve<dcon::issue_option_id>(state, parent);
		set_text(state, text::produce_simple_string(state, state.world.issue_option_get_name(issue)));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {

			auto issue = retrieve<dcon::issue_option_id>(state, parent);

			auto opt = fatten(state.world, issue);
			auto allow = opt.get_allow();
			auto parent_issue = opt.get_parent_issue();
			auto is_party_issue = state.world.issue_get_issue_type(parent_issue) == uint8_t(culture::issue_type::party);
			auto is_social_issue = state.world.issue_get_issue_type(parent_issue) == uint8_t(culture::issue_type::social);
			auto is_political_issue = state.world.issue_get_issue_type(parent_issue) == uint8_t(culture::issue_type::political);
			auto has_modifier = is_social_issue || is_political_issue;

			auto modifier_key = is_social_issue ? sys::national_mod_offsets::social_reform_desire : sys::national_mod_offsets::political_reform_desire;


			auto ids = retrieve<dcon::pop_id>(state, parent);
			auto owner = nations::owner_of_pop(state, ids);
			auto current_issue_setting = state.world.nation_get_issues(owner, parent_issue).id;
			auto allowed_by_owner = (state.world.nation_get_is_civilized(owner) || is_party_issue) &&
				(allow ? trigger::evaluate(state, allow, trigger::to_generic(owner), trigger::to_generic(owner), 0) : true) && (current_issue_setting != issue || is_party_issue) && (!state.world.issue_get_is_next_step_only(parent_issue) || (current_issue_setting.index() == issue.index() - 1) || (current_issue_setting.index() == issue.index() + 1));
			auto owner_modifier = has_modifier ? (state.world.nation_get_modifier_values(owner, modifier_key) + 1.0f) : 1.0f;
			auto pop_type = state.world.pop_get_poptype(ids);

			if(!allowed_by_owner) {
				text::add_line(state, contents, "pop_issue_attraction_1", text::variable_type::x, int64_t(0));
				text::add_line_break_to_layout(state, contents);
				text::add_line(state, contents, "pop_issue_attraction_2");
				return;
			}

			auto attraction_factor =  [&]() {
				if(auto mtrigger = state.world.pop_type_get_issues(pop_type, issue); mtrigger) {
					return trigger::evaluate_multiplicative_modifier(state, mtrigger, trigger::to_generic(ids), trigger::to_generic(ids), 0);
				} else {
					return 0.0f;
				}
			}();


			text::add_line(state, contents, "pop_issue_attraction_1", text::variable_type::x, text::fp_two_places{attraction_factor * owner_modifier});
			text::add_line_break_to_layout(state, contents);
			if(has_modifier) {
				text::add_line(state, contents, "pop_issue_attraction_3");
				text::add_line(state, contents, "pop_issue_attraction_4", text::variable_type::x, text::fp_percentage{owner_modifier});
				if(is_social_issue) {
					active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::social_reform_desire, false);
				} else {
					active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::political_reform_desire, false);
				}
				text::add_line(state, contents, "pop_issue_attraction_5", text::variable_type::x, text::fp_two_places{attraction_factor});
				if(auto mtrigger = state.world.pop_type_get_issues(pop_type, issue); mtrigger) {
					multiplicative_value_modifier_description(state, contents, mtrigger, trigger::to_generic(ids), trigger::to_generic(ids), 0);
				}
			} else {
				if(auto mtrigger = state.world.pop_type_get_issues(pop_type, issue); mtrigger) {
					multiplicative_value_modifier_description(state, contents, mtrigger, trigger::to_generic(ids), trigger::to_generic(ids), 0);
				}
			}

	}
};

class pop_detailed_issue_distribution_item : public listbox_row_element_base<std::pair<dcon::issue_option_id, float>> {
	element_base* title_text = nullptr;
	simple_text_element_base* value_text = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "legend_color") {
			return make_element_by_type<pop_distribution_plupp<dcon::issue_option_id>>(state, id);
		} else if(name == "legend_title") {
			return make_element_by_type<issue_with_explanation>(state, id);
		} else if(name == "legend_value") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			value_text = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		value_text->set_text(state, text::format_percentage(listbox_row_element_base<std::pair<dcon::issue_option_id, float>>::content.second, 1));
		for(auto& c : listbox_row_element_base<std::pair<dcon::issue_option_id, float>>::children)
			c->impl_on_update(state);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::issue_option_id>()) {
			payload.emplace<dcon::issue_option_id>(listbox_row_element_base<std::pair<dcon::issue_option_id, float>>::content.first);
			return message_result::consumed;
		}
		return listbox_row_element_base<std::pair<dcon::issue_option_id, float>>::get(state, payload);
	}
};

class pop_detailed_issue_distribution_listbox : public listbox_element_base<pop_detailed_issue_distribution_item, std::pair<dcon::issue_option_id, float>> {
public:
	std::string_view get_row_element_name() override {
		return "pop_legend_item";
	}
};

class pop_detailed_issue_distribution : public window_element_base {
	pop_detailed_issue_distribution_listbox* distrib_listbox;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "item_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_text(state, text::produce_simple_string(state, "dominant_issues_disttitle"));
			return ptr;
		} else if(name == "chart") {
			return make_element_by_type<pop_distribution_piechart<dcon::issue_option_id, false>>(state, id);
		} else if(name == "member_names") {
			auto ptr = make_element_by_type<pop_detailed_issue_distribution_listbox>(state, id);
			distrib_listbox = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			auto pop = retrieve<dcon::pop_id>(state, parent);

			std::vector<dcon::issue_option_id> distrib;
			for(auto io : state.world.in_issue_option) {
				distrib.push_back(io.id);
			}

			std::sort(distrib.begin(), distrib.end(), [&](auto a, auto b) {
				return state.world.pop_get_demographics(pop, pop_demographics::to_key(state, a)) > state.world.pop_get_demographics(pop, pop_demographics::to_key(state, b));
			});

			distrib_listbox->row_contents.clear();

			for(auto const& e : distrib)
				distrib_listbox->row_contents.emplace_back(e, state.world.pop_get_demographics(pop, pop_demographics::to_key(state, e)));
			distrib_listbox->update(state);
		}
	}
};

class ideology_with_explanation : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto issue = retrieve<dcon::ideology_id>(state, parent);
		set_text(state, text::produce_simple_string(state, state.world.ideology_get_name(issue)));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {

			auto i = retrieve<dcon::ideology_id>(state, parent);
			auto ids = retrieve<dcon::pop_id>(state, parent);
			auto type = state.world.pop_get_poptype(ids);

			if(state.world.ideology_get_enabled(i)) {
				if(state.world.ideology_get_is_civilized_only(i)) {
					if(state.world.nation_get_is_civilized(nations::owner_of_pop(state, ids))) {
						auto ptrigger = state.world.pop_type_get_ideology(type, i);
						if(ptrigger) {
							auto amount = trigger::evaluate_multiplicative_modifier(state, ptrigger, trigger::to_generic(ids),
									trigger::to_generic(ids), 0);

							text::add_line(state, contents, "pop_ideology_attraction_1", text::variable_type::x, text::fp_two_places{ amount });
							text::add_line_break_to_layout(state, contents);
							multiplicative_value_modifier_description(state, contents, ptrigger, trigger::to_generic(ids), trigger::to_generic(ids), 0);
						} else {
							text::add_line(state, contents, "pop_ideology_attraction_1", text::variable_type::x, text::fp_two_places{ 0.0f });
							text::add_line_break_to_layout(state, contents);
							text::substitution_map map{};
							text::add_to_substitution_map(map, text::variable_type::val, text::substitution(int64_t(0)));
							auto box = text::open_layout_box(contents, 15);
							text::localised_format_box(state, contents, box, std::string_view("comwid_base"), map);
							text::close_layout_box(contents, box);
						}
					} else {
						text::add_line(state, contents, "pop_ideology_attraction_1", text::variable_type::x, int64_t(0));
						text::add_line_break_to_layout(state, contents);
						text::add_line(state, contents, "pop_ideology_attraction_2");
					}
				} else {
					auto ptrigger = state.world.pop_type_get_ideology(type, i);
					if(ptrigger) {
						auto amount = trigger::evaluate_multiplicative_modifier(state, ptrigger, trigger::to_generic(ids),
								trigger::to_generic(ids), 0);

						text::add_line(state, contents, "pop_ideology_attraction_1", text::variable_type::x, text::fp_two_places{ amount });
						text::add_line_break_to_layout(state, contents);
						multiplicative_value_modifier_description(state, contents, ptrigger, trigger::to_generic(ids), trigger::to_generic(ids), 0);
					} else {
						text::add_line(state, contents, "pop_ideology_attraction_1", text::variable_type::x, text::fp_two_places{ 0.0f });
						text::add_line_break_to_layout(state, contents);
						text::substitution_map map{};
						text::add_to_substitution_map(map, text::variable_type::val, text::substitution(int64_t(0)));
						auto box = text::open_layout_box(contents, 15);
						text::localised_format_box(state, contents, box, std::string_view("comwid_base"), map);
						text::close_layout_box(contents, box);
					}
				}
			} else {
				text::add_line(state, contents, "pop_ideology_attraction_1", text::variable_type::x, int64_t(0));
				text::add_line_break_to_layout(state, contents);
				text::add_line(state, contents, "pop_ideology_attraction_2");
			}

	}
};

class pop_detailed_ideology_distribution_item : public listbox_row_element_base<std::pair<dcon::ideology_id, float>> {
	element_base* title_text = nullptr;
	simple_text_element_base* value_text = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "legend_color") {
			return make_element_by_type<pop_distribution_plupp<dcon::ideology_id>>(state, id);
		} else if(name == "legend_title") {
			return make_element_by_type<ideology_with_explanation>(state, id);
		} else if(name == "legend_value") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			value_text = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		value_text->set_text(state, text::format_percentage(listbox_row_element_base<std::pair<dcon::ideology_id, float>>::content.second, 1));
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::ideology_id>()) {
			payload.emplace<dcon::ideology_id>(listbox_row_element_base<std::pair<dcon::ideology_id, float>>::content.first);
			return message_result::consumed;
		}
		return listbox_row_element_base<std::pair<dcon::ideology_id, float>>::get(state, payload);
	}
};

class pop_detailed_ideology_distribution_listbox : public listbox_element_base<pop_detailed_ideology_distribution_item, std::pair<dcon::ideology_id, float>> {
public:
	std::string_view get_row_element_name() override {
		return "pop_legend_item";
	}
};

class pop_detailed_ideology_distribution : public window_element_base {
	pop_detailed_ideology_distribution_listbox* distrib_listbox;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "item_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_text(state, text::produce_simple_string(state, "ideology_disttitle"));
			return ptr;
		} else if(name == "chart") {
			return make_element_by_type<pop_distribution_piechart<dcon::ideology_id, false>>(state, id);
		} else if(name == "member_names") {
			auto ptr = make_element_by_type<pop_detailed_ideology_distribution_listbox>(state, id);
			distrib_listbox = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			auto pop = retrieve<dcon::pop_id>(state, parent);

			std::vector<dcon::ideology_id> distrib;
			for(auto io : state.world.in_ideology) {
				distrib.push_back(io.id);
			}

			std::sort(distrib.begin(), distrib.end(), [&](auto a, auto b) {
				return state.world.pop_get_demographics(pop, pop_demographics::to_key(state, a)) > state.world.pop_get_demographics(pop, pop_demographics::to_key(state, b));
			});

			distrib_listbox->row_contents.clear();

			for(auto const& e : distrib)
				distrib_listbox->row_contents.emplace_back(e, state.world.pop_get_demographics(pop, pop_demographics::to_key(state, e)));
			distrib_listbox->update(state);
		}
	}
};

class pop_details_promotion_percent_text : public button_element_base {
	dcon::value_modifier_key mod_key{};
	dcon::pop_location_id pop_loc{};
	float chance = 0.f;

public:
	void on_update(sys::state& state) noexcept override {
		set_button_text(state, text::format_percentage(chance, 1));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto prov_id = state.world.pop_location_get_province(pop_loc);
		auto pop_id = state.world.pop_location_get_pop(pop_loc);
		auto nat_id = state.world.province_get_nation_from_province_ownership(prov_id);
		additive_value_modifier_description(state, contents, mod_key, trigger::to_generic(pop_id), trigger::to_generic(pop_id), 0);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::pop_location_id>()) {
			pop_loc = any_cast<dcon::pop_location_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::value_modifier_key>()) {
			mod_key = any_cast<dcon::value_modifier_key>(payload);
			on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<float>()) {
			chance = any_cast<float>(payload);
			on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

template<size_t N>
class pop_details_promotion_window : public window_element_base {
	dcon::pop_type_id content{};
	float chance = 0.f;
	fixed_pop_type_icon* type_icon = nullptr;
	pop_details_promotion_percent_text* percent_text = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "pop_type") {
			auto ptr = make_element_by_type<fixed_pop_type_icon>(state, id);
			type_icon = ptr.get();
			return ptr;
		} else if(name == "percentage") {
			auto ptr = make_element_by_type<pop_details_promotion_percent_text>(state, id);
			percent_text = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		type_icon->set_type(state, content);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::pop_type_id>()) {
			content = any_cast<dcon::pop_type_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::pop_location_id>() || payload.holds_type<dcon::value_modifier_key>() || payload.holds_type<float>()) {
			percent_text->impl_set(state, payload);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

using pop_details_needs_data = std::pair< dcon::commodity_id, float> ;

class pop_details_needs_item : public listbox_row_element_base<pop_details_needs_data> {
	commodity_image* commodity_icon = nullptr;
	simple_text_element_base* value_text = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "goods_type") {
			auto ptr = make_element_by_type<commodity_image>(state, id);
			commodity_icon = ptr.get();
			return ptr;
		} else if(name == "value") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			value_text = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		value_text->set_text(state, text::format_float(content.second, 1));
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::commodity_id>()) {
			payload.emplace<dcon::commodity_id>(content.first);
			return message_result::consumed;
		}
		return listbox_row_element_base<pop_details_needs_data>::get(state, payload);
	}
};
class pop_details_needs_listbox : public listbox_element_base<pop_details_needs_item, pop_details_needs_data> {
public:
	std::string_view get_row_element_name() override {
		return "popdetail_needs_entry";
	}
};

class pop_details_migration_value : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto internal_migration =
				int64_t(demographics::get_estimated_internal_migration(state, retrieve<dcon::pop_id>(state, parent)));
		set_text(state, std::to_string(internal_migration));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		describe_migration(state, contents, retrieve<dcon::pop_id>(state, parent));
	}
};
class pop_details_migration_label : public simple_text_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		describe_migration(state, contents, retrieve<dcon::pop_id>(state, parent));
	}
};

class pop_details_colonial_migration_value : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto internal_migration =
				int64_t(demographics::get_estimated_colonial_migration(state, retrieve<dcon::pop_id>(state, parent)));
		set_text(state, std::to_string(internal_migration));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		describe_colonial_migration(state, contents, retrieve<dcon::pop_id>(state, parent));
	}
};
class pop_details_colonial_migration_label : public simple_text_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		describe_colonial_migration(state, contents, retrieve<dcon::pop_id>(state, parent));
	}
};

class pop_details_emigration_value : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto internal_migration = int64_t(demographics::get_estimated_emigration(state, retrieve<dcon::pop_id>(state, parent)));
		set_text(state, std::to_string(internal_migration));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		describe_emigration(state, contents, retrieve<dcon::pop_id>(state, parent));
	}
};
class pop_details_emigration_label : public simple_text_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		describe_emigration(state, contents, retrieve<dcon::pop_id>(state, parent));
	}
};

class pop_details_promotion_value : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto promotion = int64_t(demographics::get_estimated_promotion(state, retrieve<dcon::pop_id>(state, parent)));
		set_text(state, std::to_string(promotion));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		describe_promotion_demotion(state, contents, retrieve<dcon::pop_id>(state, parent));
	}
};
class pop_details_demotion_value : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto demotion = int64_t(demographics::get_estimated_demotion(state, retrieve<dcon::pop_id>(state, parent)));
		set_text(state, std::to_string(demotion));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		describe_promotion_demotion(state, contents, retrieve<dcon::pop_id>(state, parent));
	}
};
class pop_details_promotion_label : public simple_text_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		describe_promotion_demotion(state, contents, retrieve<dcon::pop_id>(state, parent));
	}
};

using pop_details_data = std::variant< std::monostate, dcon::pop_id>;

class generic_rebel_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto pop = retrieve<dcon::pop_id>(state, parent);
		auto movement = state.world.pop_get_movement_from_pop_movement_membership(pop);
		if(movement) {
			if(auto issue = state.world.movement_get_associated_issue_option(movement); issue) {
				set_text(state, text::produce_simple_string(state, issue.get_movement_name()));
			} else if(auto independence_target = state.world.movement_get_associated_independence(movement); independence_target) {
				if(state.world.national_identity_get_cultural_union_of(independence_target)) {
					text::substitution_map sub;
					std::string movementAdj = text::get_adjective_as_string(state, independence_target);
					text::add_to_substitution_map(sub, text::variable_type::country_adj, std::string_view(movementAdj));
					set_text(state, text::resolve_string_substitution(state, "nationalist_union_movement", sub));
				} else {
					text::substitution_map sub;
					std::string movementAdj = text::get_adjective_as_string(state, independence_target);
					text::add_to_substitution_map(sub, text::variable_type::country_adj, std::string_view(movementAdj));
					set_text(state, text::resolve_string_substitution(state, "nationalist_liberation_movement", sub));
				}
			}
			return;
		}

		auto faction = state.world.pop_get_rebel_faction_from_pop_rebellion_membership(pop);
		if(faction) {
			text::substitution_map sub;

			auto nation = state.world.rebel_faction_get_ruler_from_rebellion_within(faction);
			text::add_to_substitution_map(sub, text::variable_type::country, state.world.nation_get_adjective(nation));
			auto culture = state.world.rebel_faction_get_primary_culture(faction);
			auto defection_target = state.world.rebel_faction_get_defection_target(faction);
			if(culture) {
				// auto rebelName = text::get_name_as_string(state, culture);
				text::add_to_substitution_map(sub, text::variable_type::culture, culture.get_name());
			}
			if(defection_target) {
				auto adjective = state.world.national_identity_get_adjective(defection_target);
				text::add_to_substitution_map(sub, text::variable_type::indep, adjective);
				text::add_to_substitution_map(sub, text::variable_type::union_adj, adjective);
			}
			set_text(state, text::resolve_string_substitution(state, state.world.rebel_type_get_name(state.world.rebel_faction_get_type(faction)), sub));
			return;
		}

		set_text(state, "");
	}
};

class pop_rebels_details : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "rebel_faction") {
			return make_element_by_type<generic_rebel_name_text>(state, id);
		} else if(name == "rebel_faction_icon") {
			return make_element_by_type<pop_revolt_faction>(state, id);
		} else if(name == "pop_movement_icon") {
			return make_element_by_type<pop_movement_flag>(state, id);
		} else if(name == "pop_movement_social_icon") {
			return make_element_by_type<pop_movement_social>(state, id);
		} else if(name == "pop_movement_political_icon") {
			return make_element_by_type<pop_movement_political>(state, id);
		}

		return nullptr;
	}
};

class pop_details_window : public generic_settable_element<main_window_element_base, pop_details_data> {
	pop_type_icon* type_icon = nullptr;
	popwin_religion_type* religion_icon = nullptr;
	simple_text_element_base* religion_text = nullptr;
	simple_text_element_base* savings_text = nullptr;
	std::vector<element_base*> promotion_windows;
	std::vector<element_base*> dist_windows;
	pop_details_needs_listbox* life_needs_list = nullptr;
	pop_details_needs_listbox* everyday_needs_list = nullptr;
	pop_details_needs_listbox* luxury_needs_list = nullptr;

	template<std::size_t... Targs>
	void generate_promotion_items(sys::state& state, std::integer_sequence<std::size_t, Targs...> const&) {
		const xy_pair cell_offset{312, 153};
		(([&] {
			auto win = make_element_by_type<pop_details_promotion_window<Targs>>(state,
					state.ui_state.defs_by_name.find("pop_promotion_item")->second.definition);
			win->base_data.position.x = cell_offset.x + (Targs * win->base_data.size.x);
			win->base_data.position.y = cell_offset.y;
			promotion_windows.push_back(win.get());
			add_child_to_front(std::move(win));
		})(),
				...);
	}

public:
	void on_create(sys::state& state) noexcept override {
		main_window_element_base::on_create(state);
		set_visible(state, false);

		generate_promotion_items(state, std::integer_sequence<std::size_t, 0, 1, 2, 3, 4, 5, 6>{});
		{
			auto win = make_element_by_type<pop_detailed_ideology_distribution>(state, state.ui_state.defs_by_name.find("distribution_window")->second.definition);
			dist_windows.push_back(win.get());
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<pop_detailed_issue_distribution>(state, state.ui_state.defs_by_name.find("distribution_window")->second.definition);
			dist_windows.push_back(win.get());
			add_child_to_front(std::move(win));
		}

		// It should be proper to reposition the windows now
		const xy_pair cell_offset =
				state.ui_defs.gui[state.ui_state.defs_by_name.find("popdetaildistribution_start")->second.definition].position;
		const xy_pair cell_size =
				state.ui_defs.gui[state.ui_state.defs_by_name.find("popdetaildistribution_offset")->second.definition].position;
		xy_pair offset = cell_offset;
		for(auto const win : dist_windows) {
			win->base_data.position = offset;
			offset.x += cell_size.x;
			if(offset.x + cell_size.x >= base_data.size.x) {
				offset.x = cell_offset.x;
				offset.y += cell_size.y;
			}
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "rebel_win") {
			return make_element_by_type<pop_rebels_details>(state, id);
		} else if(name == "background") {
			return make_element_by_type<draggable_target>(state, id);
		} else if(name == "pop_type_icon") {
			return make_element_by_type<pop_type_icon>(state, id);
		} else if(name == "pop_size") {
			return make_element_by_type<pop_size_text>(state, id);
		} else if(name == "pop_culture") {
			return make_element_by_type<pop_culture_text>(state, id);
		} else if(name == "pop_location") {
			return make_element_by_type<pop_location_text>(state, id);
		} else if(name == "internal_migration_label") {
			return make_element_by_type<pop_details_migration_label>(state, id);
		} else if(name == "internal_migration_val") {
			return make_element_by_type<pop_details_migration_value>(state, id);
		} else if(name == "colonial_migration_label") {
			return make_element_by_type<pop_details_colonial_migration_label>(state, id);
		} else if(name == "colonial_migration_val") {
			return make_element_by_type<pop_details_colonial_migration_value>(state, id);
		} else if(name == "growth_indicator") {
			return make_element_by_type<pop_growth_indicator>(state, id);
		} else if(name == "external_migration_label") {
			return make_element_by_type<pop_details_emigration_label>(state, id);
		} else if(name == "external_migration_val") {
			return make_element_by_type<pop_details_emigration_value>(state, id);
		} else if(name == "promotions_label") {
			return make_element_by_type<pop_details_promotion_label>(state, id);
		} else if(name == "promotions_val") {
			return make_element_by_type<pop_details_promotion_value>(state, id);
		} else if(name == "demotions_label") {
			return make_element_by_type<pop_details_promotion_label>(state, id);
		} else if(name == "demotions_val") {
			return make_element_by_type<pop_details_demotion_value>(state, id);
		} else if(name == "mil_value") {
			return make_element_by_type<pop_militancy_text>(state, id);
		} else if(name == "con_value") {
			return make_element_by_type<pop_con_text>(state, id);
		} else if(name == "literacy_value") {
			return make_element_by_type<pop_literacy_text>(state, id);
		} else if(name == "icon_religion") {
			return make_element_by_type<popwin_religion_type>(state, id);
		} else if(name == "bank_value") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			savings_text = ptr.get();
			return ptr;
		} else if(name == "pop_unemployment_bar") {
			return make_element_by_type<pop_unemployment_progress_bar>(state, id);
		} else if(name == "lifeneed_progress") {
			return make_element_by_type<pop_life_needs_progress_bar>(state, id);
		} else if(name == "eveneed_progress") {
			return make_element_by_type<pop_everyday_needs_progress_bar>(state, id);
		} else if(name == "luxneed_progress") {
			return make_element_by_type<pop_luxury_needs_progress_bar>(state, id);
		} else if(name == "life_needs_list") {
			auto ptr = make_element_by_type<pop_details_needs_listbox>(state, id);
			life_needs_list = ptr.get();
			return ptr;
		} else if(name == "everyday_needs_list") {
			auto ptr = make_element_by_type<pop_details_needs_listbox>(state, id);
			everyday_needs_list = ptr.get();
			return ptr;
		} else if(name == "luxury_needs_list" || name == "luxuary_needs_list") {
			auto ptr = make_element_by_type<pop_details_needs_listbox>(state, id);
			luxury_needs_list = ptr.get();
			return ptr;
		} else if(name == "religion") {
			return make_element_by_type<generic_name_text<dcon::religion_id>>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(!std::holds_alternative<dcon::pop_id>(content))
			return;

		auto fat_id = dcon::fatten(state.world, std::get<dcon::pop_id>(content));
		// updated below ...
		savings_text->set_text(state, text::format_float(state.world.pop_get_savings(fat_id.id)));
		Cyto::Any payload = fat_id.id;
		for(auto& c : children) {
			c->impl_set(state, payload);
			c->impl_on_update(state);
		}

		auto prov_id = state.world.pop_location_get_province(state.world.pop_get_pop_location_as_pop(fat_id.id));
		auto nat_id = state.world.province_get_nation_from_province_ownership(prov_id);

		// Hide all promotion windows...
		for(std::size_t i = 0; i < promotion_windows.size(); ++i)
			promotion_windows[i]->set_visible(state, false);
		std::unordered_map<dcon::pop_type_id::value_base_t, float> distrib = {};
		auto total = 0.f;
		state.world.for_each_pop_type([&](dcon::pop_type_id ptid) {
			auto mod_key = fat_id.get_poptype().get_promotion(ptid);
			if(mod_key) {
				auto chance = std::max(0.0f,
						trigger::evaluate_additive_modifier(state, mod_key, trigger::to_generic(fat_id.id), trigger::to_generic(fat_id.id), 0));
				distrib[dcon::pop_type_id::value_base_t(ptid.index())] = chance;
				total += chance;
			}
		});
		// And then show them as appropriate!
		size_t index = 0;
		for(auto const& e : distrib)
			if(e.second > 0.f && index < promotion_windows.size()) {
				promotion_windows[index]->set_visible(state, true);
				Cyto::Any pt_payload = dcon::pop_type_id(e.first);
				promotion_windows[index]->impl_set(state, pt_payload);
				Cyto::Any pl_payload = state.world.pop_get_pop_location_as_pop(fat_id.id);
				promotion_windows[index]->impl_set(state, pl_payload);
				Cyto::Any mod_payload = state.world.pop_get_poptype(fat_id.id).get_promotion(dcon::pop_type_id(e.first));
				promotion_windows[index]->impl_set(state, mod_payload);
				Cyto::Any chance_payload = float(total > 0.0f ? e.second / total : 0.0f);
				promotion_windows[index]->impl_set(state, chance_payload);
				++index;
			}

		life_needs_list->row_contents.clear();
		everyday_needs_list->row_contents.clear();
		luxury_needs_list->row_contents.clear();
		state.world.for_each_commodity([&](dcon::commodity_id cid) {
			auto kf = state.world.commodity_get_key_factory(cid);
			if(state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(nat_id, kf))) {
				auto lfn = state.world.pop_type_get_life_needs(fat_id.get_poptype(), cid);
				if(lfn > 0.f)
					life_needs_list->row_contents.emplace_back(cid, lfn);
				auto evn = state.world.pop_type_get_everyday_needs(fat_id.get_poptype(), cid);
				if(evn > 0.f)
					everyday_needs_list->row_contents.emplace_back(cid, evn);
				auto lxn = state.world.pop_type_get_luxury_needs(fat_id.get_poptype(), cid);
				if(lxn > 0.f)
					luxury_needs_list->row_contents.emplace_back(cid, lxn);
			}
		});
		life_needs_list->update(state);
		everyday_needs_list->update(state);
		luxury_needs_list->update(state);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(std::holds_alternative<dcon::pop_id>(content)) {
			if(payload.holds_type<dcon::pop_id>()) {
				payload.emplace<dcon::pop_id>(std::get<dcon::pop_id>(content));
				return message_result::consumed;
			} else if(payload.holds_type<dcon::religion_id>()) {
				payload.emplace<dcon::religion_id>(state.world.pop_get_religion(std::get<dcon::pop_id>(content)));
				return message_result::consumed;
			} else if(payload.holds_type<dcon::pop_type_id>()) {
				payload.emplace<dcon::pop_type_id>(state.world.pop_get_poptype(std::get<dcon::pop_id>(content)).id);
				return message_result::consumed;
			} else if(payload.holds_type<dcon::province_id>()) {
				payload.emplace<dcon::province_id>(state.world.pop_get_province_from_pop_location(std::get<dcon::pop_id>(content)));
				return message_result::consumed;
			} else if(payload.holds_type<dcon::nation_id>()) {
				payload.emplace<dcon::nation_id>(state.local_player_nation);
				return message_result::consumed;
			}
		}
		return message_result::unseen;
	}
};

class pop_details_icon : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::pop_id>(state, parent);
		auto fat_id = dcon::fatten(state.world, state.world.pop_get_poptype(content));
		frame = int32_t(fat_id.get_sprite() - 1);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::pop_id>(state, parent);
		auto name = state.world.pop_type_get_name(state.world.pop_get_poptype(content));
		if(bool(name)) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, name);
			text::close_layout_box(contents, box);
		}
	}
};

class show_pop_detail_button : public button_element_base {
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::pop_id>(state, parent);
		Cyto::Any dt_payload = pop_details_data(content);
		state.ui_state.population_subwindow->impl_set(state, dt_payload);
	}
};

class pop_item : public listbox_row_element_base<dcon::pop_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "pops_pop_entry_bg") {
			return make_element_by_type<show_pop_detail_button>(state, id);
		} else if(name == "pop_size") {
			return make_element_by_type<pop_size_text>(state, id);
		} else if(name == "pop_nation") {
			return make_element_by_type<pop_culture_text>(state, id);
		} else if(name == "pop_location") {
			return make_element_by_type<pop_location_text>(state, id);
		} else if(name == "pop_mil") {
			return make_element_by_type<pop_militancy_text>(state, id);
		} else if(name == "pop_con") {
			return make_element_by_type<pop_con_text>(state, id);
		} else if(name == "pop_type") {
			return make_element_by_type<pop_details_icon>(state, id);
		} else if(name == "pop_religion") {
			return make_element_by_type<popwin_religion_type>(state, id);
		} else if(name == "pop_ideology") {
			return make_element_by_type<pop_ideology_piechart>(state, id);
		} else if(name == "pop_issues") {
			return make_element_by_type<pop_issues_piechart>(state, id);
		} else if(name == "pop_unemployment_bar") {
			return make_element_by_type<pop_unemployment_progress_bar>(state, id);
		} else if(name == "pop_revolt") {
			return make_element_by_type<pop_revolt_faction>(state, id);
		} else if(name == "pop_movement_social") {
			return make_element_by_type<pop_movement_social>(state, id);
		} else if(name == "pop_movement_political") {
			return make_element_by_type<pop_movement_political>(state, id);
		} else if(name == "pop_movement_flag") {
			return make_element_by_type<pop_movement_flag>(state, id);
		} else if(name == "pop_cash") {
			return make_element_by_type<pop_cash_reserve_text>(state, id);
		} else if(name == "lifeneed_progress") {
			return make_element_by_type<pop_life_needs_progress_bar>(state, id);
		} else if(name == "eveneed_progress") {
			return make_element_by_type<pop_everyday_needs_progress_bar>(state, id);
		} else if(name == "luxneed_progress") {
			return make_element_by_type<pop_luxury_needs_progress_bar>(state, id);
		} else if(name == "growth_indicator") {
			return make_element_by_type<pop_growth_indicator>(state, id);
		} else if(name == "pop_literacy") {
			return make_element_by_type<pop_literacy_text>(state, id);
		} else {
			return nullptr;
		}
	};

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::religion_id>()) {
			payload.emplace<dcon::religion_id>(state.world.pop_get_religion(content));
			return message_result::consumed;
		} else if(payload.holds_type<dcon::pop_type_id>()) {
			payload.emplace<dcon::pop_type_id>(state.world.pop_get_poptype(content).id);
			return message_result::consumed;
		}
		return listbox_row_element_base<dcon::pop_id>::get(state, payload);
	}
};
class pop_listbox : public listbox_element_base<pop_item, dcon::pop_id> {
protected:
	std::string_view get_row_element_name() override {
		return "popinfomember_popview";
	}
};

typedef std::variant< std::monostate, dcon::pop_type_id, bool> pop_filter_data;
class pop_filter_button : public generic_settable_element<button_element_base, dcon::pop_type_id> {
public:
	void on_update(sys::state& state) noexcept override {
		frame = int32_t(state.world.pop_type_get_sprite(content) - 1);
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = pop_filter_data(content);
			parent->impl_set(state, payload);
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		bool is_gray = false;
		if(parent) {
			Cyto::Any payload = pop_filter_data(content);
			parent->impl_get(state, payload);
			is_gray = !std::get<bool>(any_cast<pop_filter_data>(payload));
		}

		assert(base_data.get_element_type() == element_type::button);
		auto gid = base_data.data.button.button_image;
		// TODO: More elements defaults?
		if(gid) {
			auto& gfx_def = state.ui_defs.gfx[gid];
			if(gfx_def.primary_texture_handle) {
				assert(gfx_def.number_of_frames > 1);
				ogl::render_subsprite(state, get_color_modification(this == state.ui_state.under_mouse, is_gray, interactable), frame,
						gfx_def.number_of_frames, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
						ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
						base_data.get_rotation(), gfx_def.is_vertically_flipped());
			}
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::substitution_map sub;
		text::substitution_map sub2;
		auto pop_fat_id = dcon::fatten(state.world, content);
		auto nation_fat = dcon::fatten(state.world, state.local_player_nation);

		float pop_growth = demographics::get_effective_estimation_type_change(state, state.local_player_nation, pop_fat_id.id);

		//check if the pop is growing or not and change the text accordingly
		text::add_to_substitution_map(sub2, text::variable_type::val,
				text::pretty_integer{
						int32_t(pop_growth) });
		text::add_to_substitution_map(sub, text::variable_type::val,
				text::pretty_integer{
						int32_t(state.world.nation_get_demographics(state.local_player_nation, demographics::to_key(state, content)))});

		text::add_to_substitution_map(sub, text::variable_type::who, pop_fat_id.get_name());
		text::add_to_substitution_map(sub, text::variable_type::where, nation_fat.get_name());

		text::add_to_substitution_map(sub2, text::variable_type::who, pop_fat_id.get_name());
		text::add_to_substitution_map(sub2, text::variable_type::where, nation_fat.get_name());
		
		text::localised_format_box(state, contents, box, std::string_view("pop_size_info_on_sel"), sub);
		text::add_divider_to_layout_box(state, contents, box);
		// TODO replace $VAL from earlier with a new one showing how many people have signed up recently -breizh
		// NOW IT'S FUCKING DONE!!!
		text::localised_format_box(state, contents, box, std::string_view("pop_promote_info_on_sel_2"), sub2);
		text::close_layout_box(contents, box);
	}
};

class pop_filter_select_action {
public:
	bool value;
};
template<bool B>
class pop_filter_select_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = pop_filter_select_action{B};
			parent->impl_set(state, payload);
		}
	}
};

template<pop_list_sort Sort>
class pop_sort_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = Sort;
			parent->impl_set(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		// if(Sort == pop_list_sort::size || Sort == pop_list_sort::type || Sort == pop_list_sort::nationality || Sort ==
		// pop_list_sort::religion || Sort == pop_list_sort::location) { return; }
		auto box = text::open_layout_box(contents, 0);
		switch(Sort) {
		case pop_list_sort::literacy:
			text::localised_format_box(state, contents, box, std::string_view("popv_sort_by_lit"), text::substitution_map{});
			break;
		case pop_list_sort::change:
			text::localised_format_box(state, contents, box, std::string_view("popv_sort_by_change"), text::substitution_map{});
			break;
		case pop_list_sort::revoltrisk:
			text::localised_format_box(state, contents, box, std::string_view("popv_sort_by_rr"), text::substitution_map{});
			break;
		case pop_list_sort::luxury_needs:
			text::localised_format_box(state, contents, box, std::string_view("popv_sort_by_luxury"), text::substitution_map{});
			break;
		case pop_list_sort::everyday_needs:
			text::localised_format_box(state, contents, box, std::string_view("popv_sort_by_every"), text::substitution_map{});
			break;
		case pop_list_sort::life_needs:
			text::localised_format_box(state, contents, box, std::string_view("popv_sort_by_life"), text::substitution_map{});
			break;
		case pop_list_sort::cash:
			text::localised_format_box(state, contents, box, std::string_view("popv_sort_by_cash"), text::substitution_map{});
			break;
		case pop_list_sort::unemployment:
			text::localised_format_box(state, contents, box, std::string_view("popv_sort_by_unemployment"), text::substitution_map{});
			break;
		case pop_list_sort::issues:
			text::localised_format_box(state, contents, box, std::string_view("popv_sort_by_iss"), text::substitution_map{});
			break;
		case pop_list_sort::ideology:
			text::localised_format_box(state, contents, box, std::string_view("popv_sort_by_ide"), text::substitution_map{});
			break;
		case pop_list_sort::mil:
			text::localised_format_box(state, contents, box, std::string_view("popv_sort_by_mil"), text::substitution_map{});
			break;
		case pop_list_sort::con:
			text::localised_format_box(state, contents, box, std::string_view("popv_sort_by_con"), text::substitution_map{});
			break;
		default:
			// text::add_to_layout_box(state, contents, box, std::string_view("Not sure how you got here but have a UwU"));
			break;
		}
		text::close_layout_box(contents, box);
	}
};

class population_window : public window_element_base {
private:
	pop_listbox* country_pop_listbox = nullptr;
	pop_left_side_listbox* left_side_listbox = nullptr;
	pop_list_filter filter = std::monostate{};
	pop_details_window* details_win = nullptr;
	element_base* nf_win = nullptr;
	std::vector<element_base*> dist_windows;
	// Whether or not to show provinces below the state element in the listbox!
	ankerl::unordered_dense::map<dcon::state_instance_id::value_base_t, bool> view_expanded_state;
	std::vector<bool> pop_filters;
	dcon::state_instance_id focus_state{};
	pop_list_sort sort = pop_list_sort::size;
	bool sort_ascend = true;

	void update_pop_list(sys::state& state) {
		country_pop_listbox->row_contents.clear();

		auto nation_id = std::holds_alternative<dcon::nation_id>(filter) ? std::get<dcon::nation_id>(filter) : state.local_player_nation;
		std::vector<dcon::state_instance_id> state_list{};
		for(auto si : state.world.nation_get_state_ownership(nation_id))
			state_list.push_back(si.get_state().id);

		std::vector<dcon::province_id> province_list{};
		for(auto& state_id : state_list) {
			if(std::holds_alternative<dcon::state_instance_id>(filter) && std::get<dcon::state_instance_id>(filter) != state_id)
				continue;
			auto fat_id = dcon::fatten(state.world, state_id);
			province::for_each_province_in_state_instance(state, fat_id, [&](dcon::province_id id) { province_list.push_back(id); });
		}

		for(auto& province_id : province_list) {
			if(std::holds_alternative<dcon::province_id>(filter) && std::get<dcon::province_id>(filter) != province_id)
				continue;
			auto fat_id = dcon::fatten(state.world, province_id);
			fat_id.for_each_pop_location_as_province([&](dcon::pop_location_id id) {
				// Apply pop filters properly...
				auto pop_id = state.world.pop_location_get_pop(id);
				auto pt_id = state.world.pop_get_poptype(pop_id);
				if(pop_filters[dcon::pop_type_id::value_base_t(pt_id.id.index())])
					country_pop_listbox->row_contents.push_back(pop_id);
			});
		}
	}

	void sort_pop_list(sys::state& state) {
		std::function<bool(dcon::pop_id, dcon::pop_id)> fn;
		switch(sort) {
		case pop_list_sort::type:
			fn = [&](dcon::pop_id a, dcon::pop_id b) {
				auto a_fat_id = dcon::fatten(state.world, a);
				auto b_fat_id = dcon::fatten(state.world, b);
				return a_fat_id.get_poptype().id.index() < b_fat_id.get_poptype().id.index();
			};
			break;
		case pop_list_sort::size:
			fn = [&](dcon::pop_id a, dcon::pop_id b) {
				auto a_fat_id = dcon::fatten(state.world, a);
				auto b_fat_id = dcon::fatten(state.world, b);
				return a_fat_id.get_size() > b_fat_id.get_size();
			};
			break;
		case pop_list_sort::con:
			fn = [&](dcon::pop_id a, dcon::pop_id b) {
				auto a_fat_id = dcon::fatten(state.world, a);
				auto b_fat_id = dcon::fatten(state.world, b);
				return a_fat_id.get_consciousness() > b_fat_id.get_consciousness();
			};
			break;
		case pop_list_sort::mil:
			fn = [&](dcon::pop_id a, dcon::pop_id b) {
				auto a_fat_id = dcon::fatten(state.world, a);
				auto b_fat_id = dcon::fatten(state.world, b);
				return a_fat_id.get_militancy() > b_fat_id.get_militancy();
			};
			break;
		case pop_list_sort::religion:
			fn = [&](dcon::pop_id a, dcon::pop_id b) {
				auto a_fat_id = dcon::fatten(state.world, a);
				auto b_fat_id = dcon::fatten(state.world, b);
				return text::get_name_as_string(state, a_fat_id.get_religion()) < text::get_name_as_string(state, b_fat_id.get_religion());
			};
			break;
		case pop_list_sort::nationality:
			fn = [&](dcon::pop_id a, dcon::pop_id b) {
				auto a_fat_id = dcon::fatten(state.world, a);
				auto b_fat_id = dcon::fatten(state.world, b);
				return text::get_name_as_string(state, a_fat_id.get_culture()) < text::get_name_as_string(state, b_fat_id.get_culture());
			};
			break;
		case pop_list_sort::location:
			fn = [&](dcon::pop_id a, dcon::pop_id b) {
				auto a_fat_id = dcon::fatten(state.world, a);
				auto b_fat_id = dcon::fatten(state.world, b);
				return text::get_name_as_string(state, a_fat_id.get_province_from_pop_location()) < text::get_name_as_string(state, b_fat_id.get_province_from_pop_location());
			};
			break;
		case pop_list_sort::cash:
			fn = [&](dcon::pop_id a, dcon::pop_id b) {
				auto a_fat_id = dcon::fatten(state.world, a);
				auto b_fat_id = dcon::fatten(state.world, b);
				return a_fat_id.get_savings() > b_fat_id.get_savings();
			};
			break;
		case pop_list_sort::unemployment:
			fn = [&](dcon::pop_id a, dcon::pop_id b) {
				auto a_fat_id = dcon::fatten(state.world, a);
				auto b_fat_id = dcon::fatten(state.world, b);
				return a_fat_id.get_employment()/a_fat_id.get_size() < b_fat_id.get_employment()/b_fat_id.get_size();
			};
			break;
		case pop_list_sort::ideology:
			fn = [&](dcon::pop_id a, dcon::pop_id b) {
				auto a_fat_id = dcon::fatten(state.world, a);
				auto b_fat_id = dcon::fatten(state.world, b);
				return a_fat_id.get_dominant_ideology().id.index() < b_fat_id.get_dominant_ideology().id.index();
			};
			break;
		case pop_list_sort::issues:
			fn = [&](dcon::pop_id a, dcon::pop_id b) {
				auto a_fat_id = dcon::fatten(state.world, a);
				auto b_fat_id = dcon::fatten(state.world, b);
				return a_fat_id.get_dominant_issue_option().id.index() < b_fat_id.get_dominant_issue_option().id.index();
			};
			break;
		case pop_list_sort::life_needs:
			fn = [&](dcon::pop_id a, dcon::pop_id b) {
				auto a_fat_id = dcon::fatten(state.world, a);
				auto b_fat_id = dcon::fatten(state.world, b);
				return a_fat_id.get_life_needs_satisfaction() > b_fat_id.get_life_needs_satisfaction();
			};
			break;
		case pop_list_sort::everyday_needs:
			fn = [&](dcon::pop_id a, dcon::pop_id b) {
				auto a_fat_id = dcon::fatten(state.world, a);
				auto b_fat_id = dcon::fatten(state.world, b);
				return a_fat_id.get_everyday_needs_satisfaction() > b_fat_id.get_everyday_needs_satisfaction();
			};
			break;
		case pop_list_sort::luxury_needs:
			fn = [&](dcon::pop_id a, dcon::pop_id b) {
				auto a_fat_id = dcon::fatten(state.world, a);
				auto b_fat_id = dcon::fatten(state.world, b);
				return a_fat_id.get_luxury_needs_satisfaction() > b_fat_id.get_luxury_needs_satisfaction();
			};
			break;
		case pop_list_sort::literacy:
			fn = [&](dcon::pop_id a, dcon::pop_id b) {
				auto a_fat_id = dcon::fatten(state.world, a);
				auto b_fat_id = dcon::fatten(state.world, b);
				return a_fat_id.get_literacy() > b_fat_id.get_literacy();
			};
			break;
		case pop_list_sort::revoltrisk:
			fn = [&](dcon::pop_id a, dcon::pop_id b) {
				auto a_fat_id = dcon::fatten(state.world, a);
				auto b_fat_id = dcon::fatten(state.world, b);
				auto a_reb = a_fat_id.get_rebel_faction_from_pop_rebellion_membership();
				auto b_reb = b_fat_id.get_rebel_faction_from_pop_rebellion_membership();
				auto a_mov = a_fat_id.get_movement_from_pop_movement_membership();
				auto b_mov = b_fat_id.get_movement_from_pop_movement_membership();
				if(a_reb || b_reb) {
					return a_reb ? (b_reb ? a_reb.id.index() > b_reb.id.index() : true) : false;
				} else if(a_mov || b_mov) {
					return a_mov ? (b_mov ? a_mov.id.index() > b_mov.id.index() : true) : false;
				} else {
					return a_fat_id.id.index() > b_fat_id.id.index();
				}
			};
			break;
		case pop_list_sort::change:
			fn = [&](dcon::pop_id a, dcon::pop_id b) {
				auto a_fat_id = dcon::fatten(state.world, a);
				auto b_fat_id = dcon::fatten(state.world, b);
				return demographics::get_monthly_pop_increase(state, a_fat_id.id) > demographics::get_monthly_pop_increase(state, b_fat_id.id);
			};
			break;
		}
		std::stable_sort(country_pop_listbox->row_contents.begin(), country_pop_listbox->row_contents.end(),
				[&](dcon::pop_id a, dcon::pop_id b) {
					return fn(a, b);
				});
		if(!sort_ascend) {
			std::reverse(country_pop_listbox->row_contents.begin(), country_pop_listbox->row_contents.end());
		}
	}

	void populate_left_side_list(sys::state& state) {
		auto nation_id =
				std::holds_alternative<dcon::nation_id>(filter) ? std::get<dcon::nation_id>(filter) : state.local_player_nation;

		// & then populate the separate, left side listbox
		left_side_listbox->row_contents.push_back(pop_left_side_data(nation_id));

		// States are sorted by total population
		std::vector<dcon::state_instance_id> state_list;
		for(auto si : state.world.nation_get_state_ownership(nation_id))
			state_list.push_back(si.get_state().id);
		std::sort(state_list.begin(), state_list.end(), [&](dcon::state_instance_id a, dcon::state_instance_id b) {
			// Colonial states go last
			if(state.world.province_get_is_colonial(state.world.state_instance_get_capital(a)) !=
					state.world.province_get_is_colonial(state.world.state_instance_get_capital(b)))
				return !state.world.province_get_is_colonial(state.world.state_instance_get_capital(a));
			return state.world.state_instance_get_demographics(a, demographics::total) >
						 state.world.state_instance_get_demographics(b, demographics::total);
		});

		std::vector<dcon::province_id> province_list;
		for(auto const state_id : state_list) {
			left_side_listbox->row_contents.push_back(pop_left_side_data(state_id));
			// Provinces are sorted by total population too
			province_list.clear();
			auto fat_id = dcon::fatten(state.world, state_id);
			province::for_each_province_in_state_instance(state, fat_id, [&](dcon::province_id id) { province_list.push_back(id); });
			std::sort(province_list.begin(), province_list.end(), [&](dcon::province_id a, dcon::province_id b) {
				return state.world.province_get_demographics(a, demographics::total) >
							 state.world.province_get_demographics(b, demographics::total);
			});
			// Only put if the state is "expanded"
			if(view_expanded_state[dcon::state_instance_id::value_base_t(state_id.index())] == true)
				for(auto const province_id : province_list)
					left_side_listbox->row_contents.push_back(pop_left_side_data(province_id));
		}
	}

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);

		{
			auto win1 = make_element_by_type<pop_distribution_window<dcon::pop_type_id, true>>(state, state.ui_state.defs_by_name.find("distribution_window")->second.definition);
			dist_windows.push_back(win1.get());
			add_child_to_front(std::move(win1));

			auto win2 = make_element_by_type<pop_distribution_window<dcon::religion_id, true>>(state, state.ui_state.defs_by_name.find("distribution_window")->second.definition);
			dist_windows.push_back(win2.get());
			add_child_to_front(std::move(win2));

			auto win3 = make_element_by_type<pop_distribution_window<dcon::ideology_id, true>>(state, state.ui_state.defs_by_name.find("distribution_window")->second.definition);
			dist_windows.push_back(win3.get());
			add_child_to_front(std::move(win3));

			auto win4 = make_element_by_type<pop_distribution_window<dcon::culture_id, true>>(state, state.ui_state.defs_by_name.find("distribution_window")->second.definition);
			dist_windows.push_back(win4.get());
			add_child_to_front(std::move(win4));

			auto win5 = make_element_by_type<pop_distribution_window<dcon::issue_option_id, true>>(state, state.ui_state.defs_by_name.find("distribution_window")->second.definition);
			dist_windows.push_back(win5.get());
			add_child_to_front(std::move(win5));

			auto win6 = make_element_by_type<pop_distribution_window<dcon::political_party_id, true>>(state, state.ui_state.defs_by_name.find("distribution_window")->second.definition);
			dist_windows.push_back(win6.get());
			add_child_to_front(std::move(win6));

			// It should be proper to reposition the windows now
			const xy_pair cell_offset =
					state.ui_defs.gui[state.ui_state.defs_by_name.find("popdistribution_start")->second.definition].position;
			const xy_pair cell_size =
					state.ui_defs.gui[state.ui_state.defs_by_name.find("popdistribution_offset")->second.definition].position;
			xy_pair offset = cell_offset;
			for(auto const win : dist_windows) {
				win->base_data.position = offset;
				offset.x += cell_size.x;
				if(offset.x + cell_size.x >= base_data.size.x) {
					offset.x = cell_offset.x;
					offset.y += cell_size.y;
				}
			}
		}

		{
			// Now add the filtering windows
			const xy_pair cell_offset =
					state.ui_defs.gui[state.ui_state.defs_by_name.find("popfilter_start")->second.definition].position;
			const xy_pair cell_size =
					state.ui_defs.gui[state.ui_state.defs_by_name.find("popfilter_offset")->second.definition].position;
			xy_pair offset = cell_offset;

			state.world.for_each_pop_type([&](dcon::pop_type_id id) {
				auto win = make_element_by_type<pop_filter_button>(state,
						state.ui_state.defs_by_name.find("pop_filter_button")->second.definition);
				Cyto::Any payload = id;
				win->base_data.position = offset;
				win->impl_set(state, payload);
				add_child_to_front(std::move(win));
				offset.x += cell_size.x;
				pop_filters.push_back(true);
			});
		}

		auto win7 =
				make_element_by_type<pop_details_window>(state, state.ui_state.defs_by_name.find("pop_details_win")->second.definition);
		details_win = win7.get();
		add_child_to_front(std::move(win7));

		{
			auto ptr = make_element_by_type<national_focus_window>(state, "state_focus_window");
			ptr->set_visible(state, false);
			nf_win = ptr.get();
			add_child_to_front(std::move(ptr));
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "pop_list") {
			auto ptr = make_element_by_type<pop_listbox>(state, id);
			country_pop_listbox = ptr.get();
			return ptr;
		} else if(name == "external_scroll_slider") {
			auto ptr = make_element_by_type<opaque_element_base>(state, id);
			return ptr;
		} else if(name == "sortby_size_button") {
			auto ptr = make_element_by_type<pop_sort_button<pop_list_sort::size>>(state, id);
			return ptr;
		} else if(name == "sortby_type_button") {
			auto ptr = make_element_by_type<pop_sort_button<pop_list_sort::type>>(state, id);
			return ptr;
		} else if(name == "sortby_nationality_button") {
			auto ptr = make_element_by_type<pop_sort_button<pop_list_sort::nationality>>(state, id);
			return ptr;
		} else if(name == "sortby_religion_button") {
			auto ptr = make_element_by_type<pop_sort_button<pop_list_sort::religion>>(state, id);
			return ptr;
		} else if(name == "sortby_location_button") {
			auto ptr = make_element_by_type<pop_sort_button<pop_list_sort::location>>(state, id);
			return ptr;
		} else if(name == "sortby_mil_button") {
			auto ptr = make_element_by_type<pop_sort_button<pop_list_sort::mil>>(state, id);
			ptr->set_button_text(state, ""); // Nudge clear
			ptr->base_data.position.y -= 1;	 // Nudge
			return ptr;
		} else if(name == "sortby_con_button") {
			auto ptr = make_element_by_type<pop_sort_button<pop_list_sort::con>>(state, id);
			ptr->set_button_text(state, ""); // Nudge clear
			ptr->base_data.position.y -= 1;	 // Nudge
			return ptr;
		} else if(name == "sortby_ideology_button") {
			auto ptr = make_element_by_type<pop_sort_button<pop_list_sort::ideology>>(state, id);
			ptr->set_button_text(state, ""); // Nudge clear
			ptr->base_data.position.y -= 1;	 // Nudge
			return ptr;
		} else if(name == "sortby_issues_button") {
			auto ptr = make_element_by_type<pop_sort_button<pop_list_sort::issues>>(state, id);
			ptr->set_button_text(state, ""); // Nudge clear
			ptr->base_data.position.y -= 1;	 // Nudge
			return ptr;
		} else if(name == "sortby_unemployment_button") {
			auto ptr = make_element_by_type<pop_sort_button<pop_list_sort::unemployment>>(state, id);
			ptr->set_button_text(state, ""); // Nudge clear
			ptr->base_data.position.y -= 1;	 // Nudge
			return ptr;
		} else if(name == "sortby_cash_button") {
			auto ptr = make_element_by_type<pop_sort_button<pop_list_sort::cash>>(state, id);
			ptr->set_button_text(state, ""); // Nudge clear
			ptr->base_data.position.y -= 1;	 // Nudge
			return ptr;
		} else if(name == "sortby_subsistence_button") {
			auto ptr = make_element_by_type<pop_sort_button<pop_list_sort::life_needs>>(state, id);
			ptr->set_button_text(state, ""); // Nudge clear
			ptr->base_data.position.y -= 1;	 // Nudge
			return ptr;
		} else if(name == "sortby_eve_button") {
			auto ptr = make_element_by_type<pop_sort_button<pop_list_sort::everyday_needs>>(state, id);
			ptr->set_button_text(state, ""); // Nudge clear
			ptr->base_data.position.y -= 1;	 // Nudge
			return ptr;
		} else if(name == "sortby_luxury_button") {
			auto ptr = make_element_by_type<pop_sort_button<pop_list_sort::luxury_needs>>(state, id);
			ptr->set_button_text(state, ""); // Nudge clear
			ptr->base_data.position.y -= 1;	 // Nudge
			return ptr;
		} else if(name == "sortby_revoltrisk_button") {
			auto ptr = make_element_by_type<pop_sort_button<pop_list_sort::revoltrisk>>(state, id);
			ptr->set_button_text(state, ""); // Nudge clear
			ptr->base_data.position.y -= 1;	 // Nudge
			return ptr;
		} else if(name == "sortby_change_button") {
			auto ptr = make_element_by_type<pop_sort_button<pop_list_sort::change>>(state, id);
			ptr->set_button_text(state, ""); // Nudge clear
			ptr->base_data.position.y -= 1;	 // Nudge
			return ptr;
		} else if(name == "sortby_literacy_button") {
			auto ptr = make_element_by_type<pop_sort_button<pop_list_sort::literacy>>(state, id);
			ptr->set_button_text(state, ""); // Nudge clear
			ptr->base_data.position.y -= 1;	 // Nudge
			return ptr;
		} else if(name == "pop_province_list") {
			auto ptr = make_element_by_type<pop_left_side_listbox>(state, id);
			left_side_listbox = ptr.get();
			return ptr;
		} else if(name == "popfilter_all") {
			return make_element_by_type<pop_filter_select_button<true>>(state, id);
		} else if(name == "popfilter_deselect_all") {
			return make_element_by_type<pop_filter_select_button<false>>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(country_pop_listbox) {
			update_pop_list(state);
			sort_pop_list(state);
			country_pop_listbox->update(state);
		}
		if(left_side_listbox) {
			left_side_listbox->row_contents.clear();
			populate_left_side_list(state);
			left_side_listbox->update(state);
		}
		window_element_base::on_update(state);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<pop_list_filter>()) {
			filter = any_cast<pop_list_filter>(payload);
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<pop_details_data>()) {
			move_child_to_front(details_win);
			details_win->set_visible(state, true);
			details_win->impl_set(state, payload);
			return message_result::consumed;
		} else if(payload.holds_type<pop_left_side_expand_action>()) {
			auto expand_action = any_cast<pop_left_side_expand_action>(payload);
			auto sid = std::get<dcon::state_instance_id>(expand_action);
			view_expanded_state[dcon::state_instance_id::value_base_t(sid.index())] =
					!view_expanded_state[dcon::state_instance_id::value_base_t(sid.index())];
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<pop_filter_data>()) {
			auto data = any_cast<pop_filter_data>(payload);
			auto ptid = std::get<dcon::pop_type_id>(data);
			pop_filters[dcon::pop_type_id::value_base_t(ptid.index())] = !pop_filters[dcon::pop_type_id::value_base_t(ptid.index())];
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<pop_filter_select_action>()) {
			auto data = any_cast<pop_filter_select_action>(payload);
			state.world.for_each_pop_type(
					[&](dcon::pop_type_id id) { pop_filters[dcon::pop_type_id::value_base_t(id.index())] = data.value; });
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<pop_list_sort>()) {
			auto new_sort = any_cast<pop_list_sort>(payload);
			sort_ascend = (new_sort == sort) ? !sort_ascend : true;
			sort = new_sort;
			impl_on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<pop_list_filter>()) {
			payload.emplace<pop_list_filter>(filter);
			return message_result::consumed;
		} else if(payload.holds_type<pop_left_side_expand_action>()) {
			auto expand_action = any_cast<pop_left_side_expand_action>(payload);
			auto sid = std::get<dcon::state_instance_id>(expand_action);
			payload.emplace<pop_left_side_expand_action>(
					pop_left_side_expand_action(view_expanded_state[dcon::state_instance_id::value_base_t(sid.index())]));
			return message_result::consumed;
		} else if(payload.holds_type<pop_filter_data>()) {
			auto data = any_cast<pop_filter_data>(payload);
			auto ptid = std::get<dcon::pop_type_id>(data);
			payload.emplace<pop_filter_data>(pop_filter_data(bool(pop_filters[dcon::pop_type_id::value_base_t(ptid.index())])));
			return message_result::consumed;
		} else if(payload.holds_type<dcon::state_instance_id>()) {
			payload.emplace<dcon::state_instance_id>(focus_state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	friend class pop_national_focus_button;
	friend std::vector<dcon::pop_id> const& get_pop_window_list(sys::state& state);
	friend dcon::pop_id get_pop_details_pop(sys::state& state);
};

} // namespace ui
