#pragma once

#include "gui_element_types.hpp"
#include <vector>

namespace ui {

//======================================================================================================================================
//	REORGANISATION WINDOW
//======================================================================================================================================

enum class reorg_win_action : uint8_t {
	close = 0,
	balance = 1,
};

template<class T>
class reorg_unit_transfer_button : public shift_button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);
		if(content)
			send(state, parent, element_selection_wrapper<T>{content});
		
	}
	void button_shift_action(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);
		if constexpr(std::is_same_v<T, dcon::regiment_id>) {
			auto army = state.world.regiment_get_army_from_army_membership(content);
			auto type = state.world.regiment_get_type(content);
			for(auto r : state.world.army_get_army_membership(army)) {
				if(r.get_regiment().get_type() == type) {
					send(state, parent, element_selection_wrapper<T>{r.get_regiment().id});
				}
			}
		} else {
			auto n = state.world.ship_get_navy_from_navy_membership(content);
			auto type = state.world.ship_get_type(content);
			for(auto r : state.world.navy_get_navy_membership(n)) {
				if(r.get_ship().get_type() == type) {
					send(state, parent, element_selection_wrapper<T>{r.get_ship().id});
				}
			}
		}
	}
};

template<class T>
class reorg_unit_listbox_row : public listbox_row_element_base<T> {
using listrow = listbox_row_element_base<T>;
private:
	simple_text_element_base* str_land = nullptr;
	simple_text_element_base* str_naval = nullptr;
	simple_text_element_base* subunit_type = nullptr;
	simple_text_element_base* subunit_name = nullptr;
	image_element_base* subunit_icon = nullptr;
	vertical_progress_bar* subunit_strbar = nullptr;
	vertical_progress_bar* subunit_orgbar = nullptr;
	image_element_base* rebellion_icon = nullptr;
	button_element_base* connectedpop_icon = nullptr;

public:
	void on_create(sys::state& state) noexcept override {
		listrow::base_data.position.y += 128;	// Nudge
		listrow::base_data.position.x += 256;
		listrow::on_create(state);
	}

	void on_update(sys::state& state) noexcept override {
		auto fat = dcon::fatten(state.world, listbox_row_element_base<T>::content);
		subunit_name->set_text(state, std::string{state.to_string_view(fat.get_name())});
		dcon::unit_type_id utid = dcon::fatten(state.world, listrow::content).get_type();
		if(!utid)
			return;

		subunit_type->set_text(state, text::produce_simple_string(state, state.military_definitions.unit_base_definitions[utid].name));
		subunit_icon->frame = state.military_definitions.unit_base_definitions[utid].icon - 1;
		subunit_orgbar->progress = (fat.get_org() * 100);
		subunit_strbar->progress = (fat.get_strength() * 100);

		if constexpr(std::is_same_v<T, dcon::regiment_id>) {
			rebellion_icon->set_visible(state, true);
			connectedpop_icon->set_visible(state, true);
			connectedpop_icon->frame = fat.get_pop_from_regiment_source().get_poptype().get_sprite() - 1;

			// TODO - im not sure why this stuff isnt working, but it doesnt matter much
			if(fat.get_pop_from_regiment_source().get_rebel_faction_from_pop_rebellion_membership().is_valid()) {
				rebellion_icon->frame = fat.get_pop_from_regiment_source().get_rebel_faction_from_pop_rebellion_membership().get_type().get_icon() - 1;
			} else if(fat.get_pop_from_regiment_source().get_movement_from_pop_movement_membership().is_valid()) {
				rebellion_icon->frame = fat.get_pop_from_regiment_source().get_movement_from_pop_movement_membership().get_associated_issue_option().get_parent_issue().get_issue_type() - 1;
			} else {
				rebellion_icon->set_visible(state, false);
			}
		} else {
			rebellion_icon->set_visible(state, false);
			connectedpop_icon->set_visible(state, false);
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "band_reorg_regiment") {
			return make_element_by_type<reorg_unit_transfer_button<T>>(state, id);

		} else if(name == "band_reorg_naval") {
			return make_element_by_type<reorg_unit_transfer_button<T>>(state, id);

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
			auto ptr = make_element_by_type<button_element_base>(state, id);
			connectedpop_icon = ptr.get();
			return ptr;

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
			return make_element_by_type<reorg_unit_transfer_button<T>>(state, id);

		} else {
			return nullptr;
		}
	}
};

template<class T, class T2>
class reorg_unit_listbox_left : public listbox_element_base<reorg_unit_listbox_row<T2>, T2> {
using listbox_left = listbox_element_base<reorg_unit_listbox_row<T2>, T2>;
protected:
	std::string_view get_row_element_name() override {
		return "reorg_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		if(listbox_left::parent) {
			Cyto::Any payload = T{};
			listbox_left::parent->impl_get(state, payload);
			auto content = any_cast<T>(payload);

			Cyto::Any vpayload = std::vector<T2>{};
			listbox_left::parent->impl_get(state, vpayload);
			std::vector<T2> selectedunits = any_cast<std::vector<T2>>(vpayload);

			auto fat = dcon::fatten(state.world, content);

			listbox_left::row_contents.clear();

			if constexpr(std::is_same_v<T, dcon::army_id>) {
				for(auto regi : fat.get_army_membership()) {
					if(auto result = std::find(begin(selectedunits), end(selectedunits), regi.get_regiment().id); result == std::end(selectedunits)) {
						listbox_left::row_contents.push_back(regi.get_regiment().id);
					}
				}
			} else {
				for(auto regi : fat.get_navy_membership()) {
					if(auto result = std::find(begin(selectedunits), end(selectedunits), regi.get_ship().id); result == std::end(selectedunits)) {
						listbox_left::row_contents.push_back(regi.get_ship().id);
					}
				}
			}

			listbox_left::update(state);
		}
	}
};

template<class T, class T2>
class reorg_unit_listbox_right : public listbox_element_base<reorg_unit_listbox_row<T2>, T2> {
using listbox_right = listbox_element_base<reorg_unit_listbox_row<T2>, T2>;
protected:
	std::string_view get_row_element_name() override {
		return "reorg_entry_right";
	}

public:
	void on_update(sys::state& state) noexcept override {
		if(listbox_right::parent) {
			Cyto::Any payload = T{};
			listbox_right::parent->impl_get(state, payload);
			auto content = any_cast<T>(payload);

			Cyto::Any vpayload = std::vector<T2>{};
			listbox_right::parent->impl_get(state, vpayload);
			std::vector<T2> selectedunits = any_cast<std::vector<T2>>(vpayload);

			auto fat = dcon::fatten(state.world, content);

			listbox_right::row_contents.clear();

			if constexpr(std::is_same_v<T, dcon::army_id>) {
				for(auto regi : fat.get_army_membership()) {
					if(auto result = std::find(begin(selectedunits), end(selectedunits), regi.get_regiment().id); result != std::end(selectedunits)) {
						listbox_right::row_contents.push_back(regi.get_regiment().id);
					}
				}
			} else {
				for(auto regi : fat.get_navy_membership()) {
					if(auto result = std::find(begin(selectedunits), end(selectedunits), regi.get_ship().id); result != std::end(selectedunits)) {
						listbox_right::row_contents.push_back(regi.get_ship().id);
					}
				}
			}

			listbox_right::update(state);
		}
	}
};


class unit_reorg_balance_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<reorg_win_action>{reorg_win_action{reorg_win_action::balance}};
			parent->impl_get(state, payload);
		}
	}
};

class reorg_win_close_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<reorg_win_action>{reorg_win_action{reorg_win_action::close}};
			parent->impl_get(state, payload);
		}
	}
};

template<class T, class T2>
class unit_reorg_window : public window_element_base {
private:
	std::vector<T2> selectedsubunits;
	T unitToReorg{};
	simple_text_element_base* orginialunit_text = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "reorg_bg") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "reorg_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "unitname_1") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			orginialunit_text = ptr.get();
			return ptr;

		} else if(name == "unitname_2") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "cell_window_left" || name == "cell_window_right") {
			return make_element_by_type<invisible_element>(state, id);

		} else if(name == "balancebutton") {
			return make_element_by_type<unit_reorg_balance_button>(state, id);

		} else if(name == "closebutton") {
			return make_element_by_type<reorg_win_close_button>(state, id);

		} else if(name == "left_list") {
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				auto ptr = make_element_by_type<reorg_unit_listbox_left<T, T2>>(state, id);
				//ptr->base_data.position.x += 255;	// Nudge
				//ptr->list_scrollbar->base_data.position.x += 256;
				ptr->base_data.size.x += 256;
				return ptr;
			} else {
				auto ptr = make_element_by_type<reorg_unit_listbox_left<T, T2>>(state, id);
				//ptr->base_data.position.x += 255;	// Nudge
				//ptr->list_scrollbar->base_data.position.x += 256;
				ptr->base_data.size.x += 256;
				return ptr;
			}

		} else if(name == "right_list") {
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				auto ptr = make_element_by_type<reorg_unit_listbox_right<T, T2>>(state, id);
				//ptr->base_data.position.x += 255;	// Nudge
				//ptr->list_scrollbar->base_data.position.x += 256;
				ptr->base_data.size.x += 256;
				return ptr;
			} else {
				auto ptr = make_element_by_type<reorg_unit_listbox_right<T, T2>>(state, id);
				//ptr->base_data.position.x += 255;	// Nudge
				//ptr->list_scrollbar->base_data.position.x += 256;
				ptr->base_data.size.x += 256;
				return ptr;
			}

		} else if(name == "external_scroll_slider_left" || name == "external_scroll_slider_right") {
			return make_element_by_type<invisible_element>(state, id);

		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		unitToReorg = retrieve<T>(state, parent);
		auto fat = dcon::fatten(state.world, unitToReorg);
		orginialunit_text->set_text(state, std::string(state.to_string_view(fat.get_name())));
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<T>>()) {
			unitToReorg = any_cast<element_selection_wrapper<T>>(payload).data;
			return message_result::consumed;
		}
		//=======================================================================================
		else if(payload.holds_type<T>()) {
			payload.emplace<T>(unitToReorg);
			return message_result::consumed;
		}
		//=======================================================================================
		// SELECTION GETS
		//=======================================================================================
		else if(payload.holds_type<element_selection_wrapper<T2>>()) {
			auto content = any_cast<element_selection_wrapper<T2>>(payload).data;
			if(!selectedsubunits.empty()) {
				if(auto result = std::find(selectedsubunits.begin(), selectedsubunits.end(), content); result != selectedsubunits.end()) {
					selectedsubunits.erase(result);
				} else {
					selectedsubunits.push_back(content);
				}
			} else {
				selectedsubunits.push_back(content);
			}
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<std::vector<T2>>()) {
			payload.emplace<std::vector<T2>>(selectedsubunits);
			return message_result::consumed;
		}
		//=======================================================================================
		//	REORGANISATION WINDOW ACTIONS 
		//=======================================================================================
		else if(payload.holds_type<element_selection_wrapper<reorg_win_action>>()) {
			auto content = any_cast<element_selection_wrapper<reorg_win_action>>(payload).data;
			switch(content) {
				case reorg_win_action::close:
					if(selectedsubunits.size() <= command::num_packed_units && !selectedsubunits.empty()) {
						if constexpr(std::is_same_v<T2, dcon::regiment_id>) {
							std::array<dcon::regiment_id, command::num_packed_units> tosplit{};
							for(size_t i = 0; i < selectedsubunits.size(); i++) {
								tosplit[i] = selectedsubunits[i];
							}
							command::mark_regiments_to_split(state, state.local_player_nation, tosplit);
							command::split_army(state, state.local_player_nation, unitToReorg);
						} else {
							std::array<dcon::ship_id, command::num_packed_units> tosplit{};
							for(size_t i = 0; i < selectedsubunits.size(); i++) {
								tosplit[i] = selectedsubunits[i];
							}
							command::mark_ships_to_split( state, state.local_player_nation, tosplit);
							command::split_navy(state, state.local_player_nation, unitToReorg);
						}
					} else if(selectedsubunits.size() > command::num_packed_units){
						if constexpr(std::is_same_v<T2, dcon::regiment_id>) {
							std::array<dcon::regiment_id, command::num_packed_units> tosplit{};
							//while(selectedsubunits.size() > command::num_packed_units) {
							while(selectedsubunits.size() > 0) {
								tosplit.fill(dcon::regiment_id{});
								for(size_t i = 0; i < command::num_packed_units && i < selectedsubunits.size(); i++) {
									tosplit[i] = selectedsubunits[i];
								}
								command::mark_regiments_to_split(state, state.local_player_nation, tosplit);
								(selectedsubunits.size() > command::num_packed_units) ? selectedsubunits.erase(selectedsubunits.begin(), selectedsubunits.begin() + command::num_packed_units)
																						: selectedsubunits.erase(selectedsubunits.begin(), selectedsubunits.end());
							}
							command::split_army(state, state.local_player_nation, unitToReorg);
						} else {
							std::array<dcon::ship_id, command::num_packed_units> tosplit{};
							//while(selectedsubunits.size() > command::num_packed_units) {
							while(selectedsubunits.size() > 0) {
								tosplit.fill(dcon::ship_id{});
								for(size_t i = 0; i < command::num_packed_units && i < selectedsubunits.size(); i++) {
									tosplit[i] = selectedsubunits[i];
								}
								command::mark_ships_to_split(state, state.local_player_nation, tosplit);
								(selectedsubunits.size() > command::num_packed_units) ? selectedsubunits.erase(selectedsubunits.begin(), selectedsubunits.begin() + command::num_packed_units)
																						: selectedsubunits.erase(selectedsubunits.begin(), selectedsubunits.end());
							}
							command::split_navy(state, state.local_player_nation, unitToReorg);
						}
					}
					if(selectedsubunits.empty()) { selectedsubunits.erase(selectedsubunits.begin(), selectedsubunits.end()); }
					set_visible(state, false);
					break;
				case reorg_win_action::balance:
					// Disregard any of the units the player already selected, because its our way or the hi(fi)-way
					selectedsubunits.erase(selectedsubunits.begin(), selectedsubunits.end());
					if constexpr(std::is_same_v<T, dcon::army_id>) {
						for(auto reg : dcon::fatten(state.world, unitToReorg).get_army_membership()) {
							selectedsubunits.push_back(reg.get_regiment().id);
						}
					} else {
						for(auto reg : dcon::fatten(state.world, unitToReorg).get_navy_membership()) {
							selectedsubunits.push_back(reg.get_ship().id);
						}
					}
					std::sort(selectedsubunits.begin(), selectedsubunits.end(), [&](auto a, auto b){return dcon::fatten(state.world,a).get_type().value > dcon::fatten(state.world,b).get_type().value;});
					//for(size_t i = selectedsubunits.size(); i > 0; i--) {
					for(size_t i = selectedsubunits.size(); i-->0;) {
						if((i % 2) == 0) {
							selectedsubunits.erase(selectedsubunits.begin() + i);
						}
					}
					impl_on_update(state);
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
