#pragma once

#include "dcon_generated.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"

namespace ui {


void describe_reform(sys::state& state, text::columnar_layout& contents, dcon::issue_option_id ref) {
	auto reform = fatten(state.world, ref);

	auto mod_id = reform.get_modifier();
	if(bool(mod_id)) {
		modifier_description(state, contents, mod_id);
		text::add_line_break_to_layout(state, contents);
	}

	auto total = state.world.nation_get_demographics(state.local_player_nation, demographics::total);
	auto support = state.world.nation_get_demographics(state.local_player_nation, demographics::to_key(state, ref));
	if(total > 0) {
		text::add_line(state, contents, "there_are_backing", text::variable_type::val, text::fp_percentage{ support / total });
		text::add_line_break_to_layout(state, contents);
	}

	auto time_limit = state.world.nation_get_last_issue_or_reform_change(state.local_player_nation);
	auto parent = state.world.issue_option_get_parent_issue(ref);

	if(parent.get_issue_type() != uint8_t(culture::issue_type::party) && time_limit && !(time_limit + int32_t(state.defines.min_delay_between_reforms * 30) <= state.current_date)) {
		text::add_line_with_condition(state, contents, "too_soon_for_reform", false, text::variable_type::date, time_limit + int32_t(state.defines.min_delay_between_reforms * 30));
		text::add_line_break_to_layout(state, contents);
	}

	auto allow = reform.get_allow();
	if(allow) {
		//allow_reform_cond
		text::add_line(state, contents, "allow_reform_cond");
		trigger_description(state, contents, allow, trigger::to_generic(state.local_player_nation),
				trigger::to_generic(state.local_player_nation), -1);
		text::add_line_break_to_layout(state, contents);
	}

	auto ext = reform.get_on_execute_trigger();
	if(ext) {
		text::add_line(state, contents, "reform_effect_if_desc");
		trigger_description(state, contents, ext, trigger::to_generic(state.local_player_nation),
				trigger::to_generic(state.local_player_nation), -1);
	}
	auto ex = reform.get_on_execute_effect();
	if(ex) {
		if(ext)
			text::add_line(state, contents, "reform_effect_then_desc");
		else
			text::add_line(state, contents, "reform_effect_desc");

		effect_description(state, contents, ex, trigger::to_generic(state.local_player_nation),
				trigger::to_generic(state.local_player_nation), -1, uint32_t(state.current_date.value),
				uint32_t((ref.index() << 2) ^ state.local_player_nation.index()));
		text::add_line_break_to_layout(state, contents);
	}

	auto rules = reform.get_rules();

	if((rules & (issue_rule::primary_culture_voting | issue_rule::culture_voting | issue_rule::culture_voting | issue_rule::all_voting | issue_rule::largest_share | issue_rule::dhont | issue_rule::sainte_laque | issue_rule::same_as_ruling_party | issue_rule::rich_only | issue_rule::state_vote | issue_rule::population_vote)) !=
			0) {

		text::add_line(state, contents, "voting_rules");
		if((rules & issue_rule::primary_culture_voting) != 0) {
			text::add_line(state, contents, "rule_primary_culture_voting");
		}
		if((rules & issue_rule::culture_voting) != 0) {
			text::add_line(state, contents, "rule_culture_voting");
		}
		if((rules & issue_rule::all_voting) != 0) {
			text::add_line(state, contents, "rule_all_voting");
		}
		if((rules & issue_rule::largest_share) != 0) {
			text::add_line(state, contents, "rule_largest_share");
		}
		if((rules & issue_rule::dhont) != 0) {
			text::add_line(state, contents, "rule_dhont");
		}
		if((rules & issue_rule::sainte_laque) != 0) {
			text::add_line(state, contents, "rule_sainte_laque");
		}
		if((rules & issue_rule::same_as_ruling_party) != 0) {
			text::add_line(state, contents, "rule_same_as_ruling_party");
		}
		if((rules & issue_rule::rich_only) != 0) {
			text::add_line(state, contents, "rule_rich_only");
		}
		if((rules & issue_rule::state_vote) != 0) {
			text::add_line(state, contents, "rule_state_vote");
		}
		if((rules & issue_rule::population_vote) != 0) {
			text::add_line(state, contents, "rule_population_vote");
		}
		text::add_line_break_to_layout(state, contents);
	}

	if((rules & (issue_rule::build_factory | issue_rule::expand_factory | issue_rule::open_factory | issue_rule::destroy_factory | issue_rule::factory_priority | issue_rule::can_subsidise | issue_rule::pop_build_factory | issue_rule::pop_expand_factory | issue_rule::pop_open_factory | issue_rule::delete_factory_if_no_input | issue_rule::allow_foreign_investment | issue_rule::slavery_allowed | issue_rule::build_railway | issue_rule::build_bank | issue_rule::build_university)) != 0) {

		text::add_line(state, contents, "special_rules");

		if((rules & issue_rule::build_factory) != 0) {
			text::add_line(state, contents, "rule_build_factory");
		}
		if((rules & issue_rule::expand_factory) != 0) {
			text::add_line(state, contents, "rule_expand_factory");
		}
		if((rules & issue_rule::open_factory) != 0) {
			text::add_line(state, contents, "remove_rule_open_factory");
		}
		if((rules & issue_rule::destroy_factory) != 0) {
			text::add_line(state, contents, "rule_destroy_factory");
		}
		if((rules & issue_rule::factory_priority) != 0) {
			text::add_line(state, contents, "rule_factory_priority");
		}
		if((rules & issue_rule::can_subsidise) != 0) {
			text::add_line(state, contents, "rule_can_subsidise");
		}
		if((rules & issue_rule::pop_build_factory) != 0) {
			text::add_line(state, contents, "rule_pop_build_factory");
		}
		if((rules & issue_rule::pop_expand_factory) != 0) {
			text::add_line(state, contents, "rule_pop_expand_factory");
		}
		if((rules & issue_rule::pop_open_factory) != 0) {
			text::add_line(state, contents, "rule_pop_open_factory");
		}
		if((rules & issue_rule::delete_factory_if_no_input) != 0) {
			text::add_line(state, contents, "rule_delete_factory_if_no_input");
		}
		if((rules & issue_rule::allow_foreign_investment) != 0) {
			text::add_line(state, contents, "rule_allow_foreign_investment");
		}
		if((rules & issue_rule::slavery_allowed) != 0) {
			text::add_line(state, contents, "rule_slavery_allowed");
		}
		if((rules & issue_rule::build_railway) != 0) {
			text::add_line(state, contents, "rule_build_railway");
		}
		if(state.economy_definitions.building_definitions[int32_t(economy::province_building_type::bank)].defined) {
			if((rules & issue_rule::build_bank) != 0) {
				text::add_line(state, contents, "rule_build_bank");
			}
		}
		if(state.economy_definitions.building_definitions[int32_t(economy::province_building_type::university)].defined) {
			if((rules & issue_rule::build_university) != 0) {
				text::add_line(state, contents, "rule_build_university");
			}
		}

		text::add_line_break_to_layout(state, contents);
	}
}

class reforms_reform_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::issue_option_id>(state, parent);
		command::enact_issue(state, state.local_player_nation, content);
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::issue_option_id>(state, parent);
		disabled = !command::can_enact_issue(state, state.local_player_nation, content);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			auto content = retrieve<dcon::issue_option_id>(state, parent);

			describe_reform(state, contents, content);
		}
	}
};

class reforms_option : public listbox_row_element_base<dcon::issue_option_id> {
	image_element_base* selected_icon = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "reform_name") {
			return make_element_by_type<generic_name_text<dcon::issue_option_id>>(state, id);
		} else if(name == "selected") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			selected_icon = ptr.get();
			return ptr;
		} else if(name == "reform_option") {
			return make_element_by_type<reforms_reform_button>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		selected_icon->set_visible(state, politics::issue_is_selected(state, state.local_player_nation, content));
	}
};

class reforms_listbox : public listbox_element_base<reforms_option, dcon::issue_option_id> {
protected:
	std::string_view get_row_element_name() override {
		return "reform_option_window";
	}

public:
	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::issue_id>()) {
			auto issue_id = any_cast<dcon::issue_id>(payload);
			row_contents.clear();
			auto fat_id = dcon::fatten(state.world, issue_id);
			for(auto& option : fat_id.get_options())
				if(option)
					row_contents.push_back(option);
			update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class reforms_reform_window : public window_element_base {
	dcon::issue_id issue_id{};

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "reform_name") {
			return make_element_by_type<generic_multiline_name_text<dcon::issue_id>>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		auto reforms_box = std::make_unique<reforms_listbox>();
		reforms_box->base_data.size.x = 130;
		reforms_box->base_data.size.y = 100;
		make_size_from_graphics(state, reforms_box->base_data);
		reforms_box->on_create(state);
		add_child_to_front(std::move(reforms_box));
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::issue_id>()) {
			payload.emplace<dcon::issue_id>(issue_id);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::issue_id>()) {
			issue_id = any_cast<dcon::issue_id>(payload);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class reforms_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(auto issue_id = politics::get_issue_by_name(state, name); bool(issue_id)) {
			auto ptr = make_element_by_type<reforms_reform_window>(state, id);
			Cyto::Any payload = issue_id;
			ptr->impl_set(state, payload);
			return ptr;
		} else {
			return nullptr;
		}
	}
};

} // namespace ui
