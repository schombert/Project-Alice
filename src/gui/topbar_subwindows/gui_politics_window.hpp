#pragma once

#include "cyto_any.hpp"
#include "dcon_generated.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "gui_movements_window.hpp"
#include "gui_decision_window.hpp"
#include "gui_reforms_window.hpp"
#include "gui_release_nation_window.hpp"
#include "gui_unciv_reforms_window.hpp"
#include "nations.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include <vector>

namespace ui {

enum class politics_window_tab : uint8_t {
	reforms = 0x0,
	movements = 0x1,
	decisions = 0x2,
	releasables = 0x3
};

class politics_unciv_overlay : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state) noexcept override {
		set_visible(state, !state.world.nation_get_is_civilized(nation_id));
		return 0;
	}
};

class politics_upper_house_entry : public listbox_row_element_base<dcon::ideology_id> {
private:
	dcon::nation_id nation_id{};

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "ideology_icon") {
			return make_element_by_type<ideology_plupp>(state, id);
		} else if(name == "ideology_name") {
			return make_element_by_type<generic_name_text<dcon::ideology_id>>(state, id);
		} else if(name == "ideology_perc") {
			return make_element_by_type<nation_ideology_percentage_text>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			nation_id = any_cast<dcon::nation_id>(payload);
			update(state);
			return message_result::consumed;
		} else {
			return listbox_row_element_base<dcon::ideology_id>::set(state, payload);
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<wrapped_listbox_row_content<dcon::nation_id>>()) {
			Cyto::Any new_payload = any_cast<wrapped_listbox_row_content<dcon::nation_id>>(payload).content;
			impl_set(state, new_payload);
			return message_result::consumed;
		} else if(payload.holds_type<wrapped_listbox_row_content<dcon::ideology_id>>()) {
			Cyto::Any new_payload = any_cast<wrapped_listbox_row_content<dcon::ideology_id>>(payload).content;
			impl_set(state, new_payload);
			return message_result::consumed;
		} else {
			return listbox_row_element_base<dcon::ideology_id>::get(state, payload);
		}
	}
};

class politics_upper_house_listbox : public listbox_element_base<politics_upper_house_entry, dcon::ideology_id> {
private:
	dcon::nation_id nation_id{};

protected:
	std::string_view get_row_element_name() override {
        return "ideology_option_window";
    }

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_ideology([&](dcon::ideology_id ideology_id) {
			if(state.world.nation_get_upper_house(nation_id, ideology_id) > 0.f) {
				row_contents.push_back(ideology_id);
			}
		});
		update(state);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			nation_id = any_cast<dcon::nation_id>(payload);
			update(state);
			return message_result::consumed;
		} else {
			return listbox_element_base<politics_upper_house_entry, dcon::ideology_id>::set(state, payload);
		}
	}
};

class politics_party_issue_entry : public listbox_row_element_base<dcon::issue_option_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "issue_group") {
			return make_element_by_type<generic_name_text<dcon::issue_id>>(state, id);
		} else if(name == "issue_name") {
			return make_element_by_type<generic_name_text<dcon::issue_option_id>>(state, id);
		} else {
			return nullptr;
		}
	}

	void update(sys::state& state) noexcept override {
		Cyto::Any issue_option_payload = content;
		Cyto::Any parent_issue_payload = state.world.issue_option_get_parent_issue(content).id;
		impl_set(state, issue_option_payload);
		impl_set(state, parent_issue_payload);
	}
};

class politics_party_issues_listbox : public listbox_element_base<politics_party_issue_entry, dcon::issue_option_id> {
protected:
	std::string_view get_row_element_name() override {
        return "party_issue_option_window";
    }

public:
	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::political_party_id>()) {
			auto party = any_cast<dcon::political_party_id>(payload);
			row_contents.clear();
			state.world.for_each_issue([&](dcon::issue_id issue) {
				auto issue_option = state.world.political_party_get_party_issues(party, issue);
				if(row_contents.size() < 5) {
					row_contents.push_back(issue_option.id);
				}
			});
			update(state);
			return message_result::consumed;
		} else {
			return listbox_element_base<politics_party_issue_entry, dcon::issue_option_id>::set(state, payload);
		}
	}

	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override {
		return parent->impl_on_scroll(state, x, y, amount, mods);
	}
};

class politics_choose_party_button : public standard_party_button {
public:
	void on_update(sys::state& state) noexcept override {
		set_button_text(state, text::produce_simple_string(state, state.world.political_party_get_name(political_party_id)));
	}

	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override {
		return parent->impl_on_scroll(state, x, y, amount, mods);
	}
};

class politics_all_party_entry : public listbox_row_element_base<dcon::political_party_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "party_name") {
			return make_element_by_type<politics_choose_party_button>(state, id);
		} else if(name == "party_icon") {
			return make_element_by_type<ideology_plupp>(state, id);
		} else if(name == "issue_listbox") {
			return make_element_by_type<politics_party_issues_listbox>(state, id);
		} else {
			return nullptr;
		}
	}

	void update(sys::state& state) noexcept override {
		Cyto::Any party_payload = content;
		Cyto::Any ideology_payload = state.world.political_party_get_ideology(content).id;
		impl_set(state, party_payload);
		impl_set(state, ideology_payload);
	}

	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override {
		return parent->impl_on_scroll(state, x, y, amount, mods);
	}
};

class politics_all_party_listbox : public listbox_element_base<politics_all_party_entry, dcon::political_party_id> {
private:
	dcon::nation_id nation_id{};

protected:
	std::string_view get_row_element_name() override {
        return "party_window";
    }

	void on_update(sys::state& state) noexcept override {
		nations::get_active_political_parties(state, nation_id).swap(row_contents);
		update(state);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			nation_id = any_cast<dcon::nation_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return listbox_element_base<politics_all_party_entry, dcon::political_party_id>::set(state, payload);
		}
	}
};

class politics_all_party_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		base_data.position.y -= 66;
		Cyto::Any payload = state.local_player_nation;
		impl_set(state, payload);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "all_party_window_close") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "party_listbox") {
			return make_element_by_type<politics_all_party_listbox>(state, id);
		} else {
			return nullptr;
		}
	}
};

class politics_change_party_button : public standard_nation_button {
private:
	politics_all_party_window* all_party_window = nullptr;

public:
	void on_update(sys::state& state) noexcept override {
		auto party = state.world.nation_get_ruling_party(nation_id);
		set_button_text(state, text::produce_simple_string(state, state.world.political_party_get_name(party)));
	}

	void button_action(sys::state& state) noexcept override {
		if(all_party_window == nullptr) {
			auto ptr = make_element_by_type<politics_all_party_window>(state, "all_party_window");
			all_party_window = static_cast<politics_all_party_window*>(ptr.get());
			parent->parent->add_child_to_front(std::move(ptr));
		} else {
			all_party_window->set_visible(state, !all_party_window->is_visible());
		}
	}
};

class politics_ruling_party_window : public window_element_base {
private:
	dcon::nation_id nation_id{};
	dcon::political_party_id party_id{};

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		base_data.position.x = 37;
		base_data.position.y = 236;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "politics_selectparty_entrybg") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "party_name") {
			return make_element_by_type<politics_change_party_button>(state, id);
		} else if(name == "party_icon") {
			return make_element_by_type<nation_ruling_party_ideology_plupp>(state, id);
		} else if(name == "issue_listbox") {
			return make_element_by_type<politics_party_issues_listbox>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto current_party = state.world.nation_get_ruling_party(nation_id).id;
		if(current_party != party_id) {
			party_id = current_party;
			Cyto::Any party_payload = party_id;
			impl_set(state, party_payload);
		}
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

class politics_window : public generic_tabbed_window<politics_window_tab> {
private:
	dcon::nation_id nation_id{};
	reforms_window* reforms_win = nullptr;
	unciv_reforms_window* unciv_reforms_win = nullptr;
	movements_window* movements_win = nullptr;
	decision_window* decision_win = nullptr;
	release_nation_window* release_nation_win = nullptr;

public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window::on_create(state);
		auto ptr = make_element_by_type<politics_ruling_party_window>(state, "party_window");
		add_child_to_front(std::move(ptr));
		set_visible(state, false);
		nation_id = state.local_player_nation;
		Cyto::Any payload = nation_id;
		impl_set(state, payload);
	}
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "reforms_tab") {
			auto ptr = make_element_by_type<generic_tab_button<politics_window_tab>>(state, id);
			ptr->target = politics_window_tab::reforms;
			return ptr;
		} else if(name == "movements_tab") {
			auto ptr = make_element_by_type<generic_tab_button<politics_window_tab>>(state, id);
			ptr->target = politics_window_tab::movements;
			return ptr;
		} else if(name == "decisions_tab") {
			auto ptr = make_element_by_type<generic_tab_button<politics_window_tab>>(state, id);
			ptr->target = politics_window_tab::decisions;
			return ptr;
		} else if(name == "release_nations_tab") {
			auto ptr = make_element_by_type<generic_tab_button<politics_window_tab>>(state, id);
			ptr->target = politics_window_tab::releasables;
			return ptr;
		} else if(name == "reforms_window") {
			auto ptr = make_element_by_type<reforms_window>(state, id);
			reforms_win = ptr.get();
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "movements_window") {
			auto ptr = make_element_by_type<movements_window>(state, id);
			movements_win = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "decision_window") {
			auto ptr = make_element_by_type<decision_window>(state, id);
			decision_win = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "release_nation") {
			auto ptr = make_element_by_type<release_nation_window>(state, id);
			release_nation_win = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "unciv_reforms_window") {
			auto ptr = make_element_by_type<unciv_reforms_window>(state, id);
			unciv_reforms_win = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "government_name") {
			return make_element_by_type<nation_government_type_text>(state, id);
		} else if(name == "national_value") {
			return make_element_by_type<nation_national_value_icon>(state, id);
		} else if(name == "plurality_value") {
			return make_element_by_type<nation_plurality_text>(state, id);
		} else if(name == "revanchism_value") {
			return make_element_by_type<nation_revanchism_text>(state, id);
		} else if(name == "can_do_social_reforms") {
			return make_element_by_type<nation_can_do_social_reform_text>(state, id);
		} else if(name == "social_reforms_bock") {
			return make_element_by_type<nation_can_do_social_reform_icon>(state, id);
		} else if(name == "can_do_political_reforms") {
			return make_element_by_type<nation_can_do_political_reform_text>(state, id);
		} else if(name == "political_reforms_bock") {
			return make_element_by_type<nation_can_do_political_reform_icon>(state, id);
		} else if(name == "chart_upper_house") {
			return make_element_by_type<upper_house_piechart>(state, id);
		} else if(name == "chart_voters_ideologies") {
			return make_element_by_type<voter_ideology_piechart>(state, id);
		} else if(name == "chart_people_ideologies") {
			return make_element_by_type<ideology_piechart<dcon::nation_id>>(state, id);
		} else if(name == "upperhouse_ideology_listbox") {
			return make_element_by_type<politics_upper_house_listbox>(state, id);
		} else if(name == "unciv_overlay") {
			return make_element_by_type<politics_unciv_overlay>(state, id);
		} else {
			return nullptr;
		}
	}
	void on_update(sys::state& state) noexcept override {
		if(nation_id != state.local_player_nation) {
			nation_id = state.local_player_nation;
			Cyto::Any payload = nation_id;
			impl_set(state, payload);
		}
		if(state.world.nation_get_is_civilized(nation_id) && unciv_reforms_win->is_visible()) {
			unciv_reforms_win->set_visible(state, false);
			reforms_win->set_visible(state, true);
		} else if(!state.world.nation_get_is_civilized(nation_id) && reforms_win->is_visible()) {
			reforms_win->set_visible(state, false);
			unciv_reforms_win->set_visible(state, true);
		}
	}
	void hide_sub_windows(sys::state& state) {
		reforms_win->set_visible(state, false);
		unciv_reforms_win->set_visible(state, false);
		decision_win->set_visible(state, false);
		movements_win->set_visible(state, false);
		release_nation_win->set_visible(state, false);
	}
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<politics_window_tab>()) {
			auto enum_val = any_cast<politics_window_tab>(payload);
			hide_sub_windows(state);
			switch(enum_val) {
				case politics_window_tab::reforms:
					if(state.world.nation_get_is_civilized(nation_id)) {
						reforms_win->set_visible(state, true);
					} else {
						unciv_reforms_win->set_visible(state, true);
					}
					break;
				case politics_window_tab::movements:
					movements_win->set_visible(state, true);
					break;
				case politics_window_tab::decisions:
					decision_win->set_visible(state, true);
					break;
				case politics_window_tab::releasables:
					release_nation_win->set_visible(state, true);
					break;
			}
			active_tab = enum_val;
			return message_result::consumed;
		} else if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(nation_id);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

}
