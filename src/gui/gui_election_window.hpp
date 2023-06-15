#pragma once

#include "dcon_generated.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "nations.hpp"
#include "system_state.hpp"
#include "text.hpp"

namespace ui {

/*	##   ##		       ##   ##
	##   ##  ##   ##   ##  ##   ##
	##   ##  ##   ##   ##  ##   ##
	##   ##  ##   ##   ##  ##   ##
	 ## ##    ## #### ##    ## ##
		###      ###  ###      ###
*/

//===============================================================================================================================

class elecwin_voter_issue_entry : public listbox_row_element_base<dcon::issue_option_id> {
private:
	simple_text_element_base* issname = nullptr;
	simple_text_element_base* issvalue = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "issue_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			issname = ptr.get();
			return ptr;
		} else if(name == "voters_perc") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			issvalue = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		issname->set_text(state, text::produce_simple_string(state, dcon::fatten(state.world, content).get_name()));
		auto value = state.world.nation_get_demographics(state.local_player_nation, demographics::to_key(state, content)) /
								 state.world.nation_get_demographics(state.local_player_nation, demographics::total);
		issvalue->set_text(state, text::format_percentage(value));
	}
};

class elecwin_voter_issues_listbox : public listbox_element_base<elecwin_voter_issue_entry, dcon::issue_option_id> {
protected:
	std::string_view get_row_element_name() override {
		return "ew_issue_option_window";
	}

public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::issue_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::issue_id>(payload);

			row_contents.clear();

			state.world.for_each_issue_option([&](dcon::issue_option_id id) {
				if(content == dcon::fatten(state.world, id).get_parent_issue().id) {
					row_contents.push_back(id);
				}
			});

			update(state);
		}
	}
};

//===============================================================================================================================

class elecwin_party_entry : public listbox_row_element_base<dcon::political_party_id> {
	simple_text_element_base* partyname = nullptr;
	simple_text_element_base* partyvalue = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "legend_color") {
			return make_element_by_type<ideology_plupp>(state, id);
		} else if(name == "legend_title") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			partyname = ptr.get();
			return ptr;
		} else if(name == "legend_desc") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			partyvalue = ptr.get();
			return ptr;
		} else if(name == "legend_value") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false); // Unused ?
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		Cyto::Any payload = dcon::issue_id{};
		parent->impl_get(state, payload);
		auto issid = any_cast<dcon::issue_id>(payload);

		partyname->set_text(state, text::produce_simple_string(state, state.world.political_party_get_name(content)));
		partyvalue->set_text(state,
				text::produce_simple_string(state, dcon::fatten(state.world, content).get_party_issues(issid).get_name()));
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::ideology_id>()) {
			payload.emplace<dcon::ideology_id>(state.world.political_party_get_ideology(content));
			return message_result::consumed;
		}
		return listbox_row_element_base<dcon::political_party_id>::get(state, payload);
	}

	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override {
		return parent->impl_on_scroll(state, x, y, amount, mods);
	}
};

class elecwin_parties_listbox : public listbox_element_base<elecwin_party_entry, dcon::political_party_id> {
protected:
	std::string_view get_row_element_name() override {
		return "eew_item";
	}

public:
	void on_update(sys::state& state) noexcept override {
		nations::get_active_political_parties(state, state.local_player_nation).swap(row_contents);
		update(state);
	}
};

//===============================================================================================================================

class elecwin_popularity_entry : public listbox_row_element_base<dcon::political_party_id> {
public:
	simple_text_element_base* partyname = nullptr;
	simple_text_element_base* partyvalue = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "legend_color") {
			return make_element_by_type<ideology_plupp>(state, id);
		} else if(name == "legend_title") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			partyname = ptr.get();
			return ptr;
		} else if(name == "legend_desc") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			// partyvalue = ptr.get();
			return ptr;
		} else if(name == "legend_value") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			// ptr->set_visible(state, false);	// Unused ?
			partyvalue = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		Cyto::Any payload = dcon::issue_id{};
		parent->impl_get(state, payload);
		auto issid = any_cast<dcon::issue_id>(payload);

		partyname->set_text(state, text::produce_simple_string(state, state.world.political_party_get_name(content)));
		// partyvalue->set_text(state, text::produce_simple_string(state, dcon::fatten(state.world,
		// content).get_party_issues(issid).get_name()));
		partyvalue->set_text(state, "UwU");
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::ideology_id>()) {
			payload.emplace<dcon::ideology_id>(state.world.political_party_get_ideology(content));
			return message_result::consumed;
		}
		return listbox_row_element_base<dcon::political_party_id>::get(state, payload);
	}

	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override {
		return parent->impl_on_scroll(state, x, y, amount, mods);
	}
};

class elecwin_popularity_listbox : public listbox_element_base<elecwin_popularity_entry, dcon::political_party_id> {
protected:
	std::string_view get_row_element_name() override {
		return "eew_item";
	}

public:
	void on_update(sys::state& state) noexcept override {
		nations::get_active_political_parties(state, state.local_player_nation).swap(row_contents);
		update(state);
	}
};

//===============================================================================================================================

class election_event_window : public window_element_base {
private:
	element_base* elecwin_parties = nullptr;

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			return make_element_by_type<draggable_target>(state, id);

		} else if(name == "title") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "subtitle") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "description") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "state_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_text(state, text::produce_simple_string(state, dcon::fatten(state.world, state.local_player_nation).get_name()));
			return ptr;

		} else if(name == "population_amount") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_text(state, text::prettify(int64_t(state.world.nation_get_demographics(state.local_player_nation, demographics::total))));
			return ptr;

		} else if(name == "perc_of_parlament") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false); // This does not appear to be used in Vic2 ?
			return ptr;

		} else if(name == "ideology_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "parties_listbox") {
			auto ptr = make_element_by_type<elecwin_parties_listbox>(state, id);
			elecwin_parties = ptr.get();
			return ptr;

		} else if(name == "popularity_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "chart_popularity") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "popularity_overlay") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "popularity_listbox") {
			return make_element_by_type<elecwin_popularity_listbox>(state, id);

		} else if(name == "voter_issues_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "sort_by_issue_name") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "sort_by_voters") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "issues_listbox") {
			return make_element_by_type<elecwin_voter_issues_listbox>(state, id);

		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::issue_id>()) {
			payload.emplace<dcon::issue_id>(dcon::issue_id{2});
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
