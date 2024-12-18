#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "diplomatic_messages.hpp"
#include "gui_diplomacy_request_window.hpp"

namespace ui {

class diplomatic_message_topbar_button : public button_element_base {
	std::string_view get_type_key(diplomatic_message::type_t type) {
		switch(type) {
		case diplomatic_message::type_t::none:
			return "???";
		case diplomatic_message::type_t::access_request:
			return "askmilitaryaccess_di";
		case diplomatic_message::type_t::alliance_request:
			return "alliance_di";
		case diplomatic_message::type_t::call_ally_request:
			return "callally_di";
		case diplomatic_message::type_t::be_crisis_primary_defender:
			return "back_crisis_di";
		case diplomatic_message::type_t::be_crisis_primary_attacker:
			return "back_crisis_di";
		case diplomatic_message::type_t::peace_offer:
			return "peace_di";
		case diplomatic_message::type_t::take_crisis_side_offer:
			return "back_crisis_di";
		case diplomatic_message::type_t::crisis_peace_offer:
			return "crisis_offer_di";
		case diplomatic_message::type_t::state_transfer:
			return "state_transfer_di";
		default:
			return "???";
		}
		
	}
public:
	void on_update(sys::state& state) noexcept override {
		auto const m = retrieve<diplomatic_message::message>(state, parent);
		/*
			requestmilaccess = 0,
			offermilaccess = 1,
			offeralliance = 2,
			calltoarms = 3,
			offerpeace = 4,
			invitecrisis = 5,
		*/
		switch(m.type) {
		case diplomatic_message::type_t::none:
		case diplomatic_message::type_t::state_transfer:
			frame = 0;
			break;
		case diplomatic_message::type_t::access_request:
			frame = 1;
			break;
		case diplomatic_message::type_t::alliance_request:
			frame = 2;
			break;
		case diplomatic_message::type_t::call_ally_request:
			frame = 3;
			break;
		case diplomatic_message::type_t::take_crisis_side_offer:
		case diplomatic_message::type_t::peace_offer:
		case diplomatic_message::type_t::crisis_peace_offer:
			frame = 4;
			break;
		case diplomatic_message::type_t::be_crisis_primary_defender:
		case diplomatic_message::type_t::be_crisis_primary_attacker:
			frame = 5;
			break;
		default:
			break;
		}
	}

    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
        return tooltip_behavior::variable_tooltip;
    }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto m = retrieve<diplomatic_message::message>(state, parent);
		auto box = text::open_layout_box(contents);
		auto tstr = text::produce_simple_string(state, get_type_key(m.type));
		text::substitution_map sub{};
		text::add_to_substitution_map(sub, text::variable_type::nation, m.from);
		text::add_to_substitution_map(sub, text::variable_type::date, m.when + diplomatic_message::expiration_in_days);
		text::add_to_substitution_map(sub, text::variable_type::type, std::string_view(tstr.c_str()));
		text::localised_format_box(state, contents, box, std::string_view("diploicon_tip"), sub);
		text::close_layout_box(contents, box);
    }

	void button_action(sys::state& state) noexcept override;
};
class diplomatic_message_topbar_flag_button : public flag_button {
public:
	void button_action(sys::state& state) noexcept override;
};
class diplomatic_message_topbar_entry_window : public listbox_row_element_base<diplomatic_message::message> {
public:
	diplomatic_message_topbar_button* btn = nullptr;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
        if(name == "diplomessageicon_button") {
            auto ptr = make_element_by_type<diplomatic_message_topbar_button>(state, id);
	    btn = ptr.get();
	    return ptr;
        } else if(name == "flag") {
            return make_element_by_type<diplomatic_message_topbar_flag_button>(state, id);
        } else if(name == "messageicon_bg_overlay") {
            return make_element_by_type<image_element_base>(state, id);
        } else {
            return nullptr;
        }
    }
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(content.from);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::national_identity_id>()) {
			payload.emplace<dcon::national_identity_id>(state.world.nation_get_identity_from_identity_holder(content.from));
			return message_result::consumed;
		}
		return listbox_row_element_base<diplomatic_message::message>::get(state, payload);
	}
};

class diplomatic_message_topbar_listbox : public overlapping_listbox_element_base<diplomatic_message_topbar_entry_window, diplomatic_message::message> {
	std::string_view get_row_element_name() override {
		return "alice_diplomessageicon_entry_window";
	}
public:
	std::vector< diplomatic_message::message> messages;
	void on_update(sys::state& state) noexcept override {
		auto it = std::remove_if(messages.begin(), messages.end(), [&](auto& m) {
			return m.when + diplomatic_message::expiration_in_days <= state.current_date
				|| !diplomatic_message::can_accept(state, m);
		});
		messages.erase(it, messages.end());

		row_contents.clear();
		for(auto m : messages) {
			row_contents.push_back(m);
		}
		update(state);
	}
};

void diplomatic_message_topbar_button::button_action(sys::state& state) noexcept {
	auto const m = retrieve<diplomatic_message::message>(state, parent);
	//Invoke window
	auto dpw = static_cast<ui::diplomacy_request_window*>(state.ui_state.request_window);
	dpw->messages.push_back(m);
	dpw->set_visible(state, true);
	dpw->impl_on_update(state);
	//Remove from listbox
	auto dmtl = static_cast<diplomatic_message_topbar_listbox*>(state.ui_state.request_topbar_listbox);
	auto it = std::remove_if(dmtl->messages.begin(), dmtl->messages.end(),
			[&](auto& e) { return e.from == m.from && e.to == m.to && e.type == m.type && e.when == m.when; });
	auto r = std::distance(it, dmtl->messages.end());
	dmtl->messages.erase(it, dmtl->messages.end());
	dmtl->impl_on_update(state);
}

void diplomatic_message_topbar_flag_button::button_action(sys::state& state) noexcept {

	// TODO: right click must remove message
	if(parent) {
		auto win = static_cast<diplomatic_message_topbar_entry_window*>(parent);
		if(win->btn)
			win->btn->button_action(state);
	}
}

}
