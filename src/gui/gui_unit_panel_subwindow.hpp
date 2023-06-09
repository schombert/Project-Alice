#pragma once

#include "gui_element_types.hpp"

namespace ui {

class leader_listbox_element : public listbox_row_element_base<uint8_t> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "leader_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "prestige_bar_frame") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "leader_prestige_bar") {
			return make_element_by_type<vertical_progress_bar>(state, id);

		} else if(name == "photo") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "leader_name") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "unitleader_personality") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "unitleader_background") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else {
			return nullptr;
		}
	}
};

class available_leader_listbox : public listbox_element_base<leader_listbox_element, uint16_t> {
protected:
	std::string_view get_row_element_name() override {
		return "leader_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		for(uint16_t i = 0; i < 16; i++) {
			row_contents.push_back(i);
		}
		update(state);
	}
};

class leader_selection_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "select_leader_bg") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "prestige_bar_frame") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "current_leader_prestige_bar") {
			return make_element_by_type<vertical_progress_bar>(state, id);

		} else if(name == "selected_photo") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "selected_leader_name") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "unitleader_personality") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "unitleader_background") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "sort_prestige") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "sort_prestige_icon") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "sort_name") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "sort_presonality") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "sort_background") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "back_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "noleader_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "auto_assign") {
			return make_element_by_type<checkbox_button>(state, id);

		} else if(name == "assign_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "leaderlist") {
			return make_element_by_type<available_leader_listbox>(state, id);

		} else {
			return nullptr;
		}
	}
};
//======================================================================================================================================

class reorg_unit_listbox_row : public listbox_row_element_base<dcon::regiment_id> {
private:
	bool isRightSide = false;
	simple_text_element_base* str_land = nullptr;
	simple_text_element_base* str_naval = nullptr;
	simple_text_element_base* subunit_type = nullptr;
	simple_text_element_base* subunit_name = nullptr;
	image_element_base* subunit_icon = nullptr;
	vertical_progress_bar* subunit_strbar = nullptr;
	vertical_progress_bar* subunit_orgbar = nullptr;
	image_element_base* rebellion_icon = nullptr;

public:
	void on_update(sys::state& state) noexcept override {
		auto fat = dcon::fatten(state.world, content);
		subunit_name->set_text(state, std::string{state.to_string_view(fat.get_name())});
		// subunit_type->set_text(state, std::string{state.to_string_view(fat.get_type())});
		subunit_strbar->progress = (fat.get_strength() * 100);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "band_reorg_regiment") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "band_reorg_naval") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "unit_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			subunit_icon = ptr.get();
			return ptr;

		} else if(name == "subunit_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			subunit_name = ptr.get();
			return ptr;

		} else if(name == "subunit_type") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			subunit_type = ptr.get();
			return ptr;

		} else if(name == "subunut_strength") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			str_land = ptr.get();
			return ptr;

		} else if(name == "subunit_strength_naval") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			str_naval = ptr.get();
			return ptr;

		} else if(name == "connected_pop") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "rebel_faction") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			rebellion_icon = ptr.get();
			return ptr;

		} else if(name == "org_bar") {
			auto ptr = make_element_by_type<vertical_progress_bar>(state, id);
			subunit_orgbar = ptr.get();
			return ptr;

		} else if(name == "str_bar") {
			auto ptr = make_element_by_type<vertical_progress_bar>(state, id);
			subunit_strbar = ptr.get();
			return ptr;

		} else if(name == "transferbutton") {
			return make_element_by_type<button_element_base>(state, id);

		} else {
			return nullptr;
		}
	}
};

class reorg_unit_listbox_left : public listbox_element_base<reorg_unit_listbox_row, dcon::regiment_id> {
protected:
	std::string_view get_row_element_name() override {
		return "reorg_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::army_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::army_id>(payload);
			auto fat = dcon::fatten(state.world, content);

			row_contents.clear();

			for(auto regi : fat.get_army_membership()) {
				row_contents.push_back(regi.get_regiment().id);
			}

			update(state);
		}
	}
};

class reorg_unit_listbox_right : public listbox_element_base<reorg_unit_listbox_row, dcon::regiment_id> {
protected:
	std::string_view get_row_element_name() override {
		return "reorg_entry_right";
	}

public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::army_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::army_id>(payload);
			auto fat = dcon::fatten(state.world, content);

			row_contents.clear();

			for(auto regi : fat.get_army_membership()) {
				row_contents.push_back(regi.get_regiment().id);
			}

			update(state);
		}
	}
};

class unit_reorg_balance_button : public button_element_base {
public:
	// TODO - super complex behaviour here , much stuff
};

class unit_reorg_window : public window_element_base {
private:
	dcon::army_id armyToReorg{};

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "reorg_bg") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "reorg_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "unitname_1") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "unitname_2") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "cell_window_left") {
			auto ptr = make_element_by_type<window_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;

		} else if(name == "cell_window_right") {
			auto ptr = make_element_by_type<window_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;

		} else if(name == "balancebutton") {
			return make_element_by_type<unit_reorg_balance_button>(state, id);

		} else if(name == "closebutton") {
			return make_element_by_type<generic_close_button>(state, id);

		} else if(name == "left_list") {
			return make_element_by_type<reorg_unit_listbox_left>(state, id);

		} else if(name == "right_list") {
			return make_element_by_type<reorg_unit_listbox_right>(state, id);

		} else if(name == "external_scroll_slider_left") {
			auto ptr = make_element_by_type<scrollbar>(state, id);
			ptr->set_visible(state, false);
			return ptr;

		} else if(name == "external_scroll_slider_right") {
			auto ptr = make_element_by_type<scrollbar>(state, id);
			ptr->set_visible(state, false);
			return ptr;

		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<dcon::army_id>>()) {
			armyToReorg = any_cast<element_selection_wrapper<dcon::army_id>>(payload).data;
			return message_result::consumed;
		}
		//=======================================================================================
		else if(payload.holds_type<dcon::army_id>()) {
			payload.emplace<dcon::army_id>(armyToReorg);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
