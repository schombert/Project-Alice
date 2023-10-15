#pragma once

#include "gui_element_types.hpp"

namespace ui {

enum class diploaction_type : uint8_t {
    requestmilaccess = 0,
    offermilaccess = 1,
    offeralliance = 2,
    calltoarms = 3,
    offerpeace = 4,
    invitecrisis = 5,
};

struct evententry {
    diploaction_type type;
    sys::date enddate;
    dcon::nation_id sender{};
};

class diplomessage_button : public button_element_base {
public:
    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
        return tooltip_behavior::variable_tooltip;
    }

    void update_tooltip(sys::state& state, int16_t x, int16_t y, text::columnar_layout& contents) noexcept override {
        auto box = text::open_layout_box(contents);
        text::localised_format_box(state, contents, box, std::string_view("diploicon_tip"));
        text::close_layout_box(contents, box);
    }
};

class diplomessage_entry_win : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
        if(name == "diplomessageicon_button") {
            return make_element_by_type<diplomessage_button>(state, id);
        } else if(name == "flag") {
            return make_element_by_type<image_element_base>(state, id);
        } else if(name == "messageicon_bg_overlay") {
            return make_element_by_type<image_element_base>(state, id);
        } else {
            return nullptr;
        }
    }
};

class diplomessage_listbox : public overlapping_listbox_element_base<diplomessage_entry_win, evententry> {
public:
	void on_update(sys::state& state) noexcept override {
		std::vector<evententry> entries = retrieve<std::vector<evententry>>(state, parent);
		row_contents.clear();
		for(auto n : entries) {
			row_contents.push_back(n);
		}
		update(state);
	}
};

class diplomessage_window : public window_element_base {
private:
    std::vector<evententry> entries;
    window_element_base* diplomessage_win = nullptr;
protected:
    void add_entry(sys::state& state, evententry event) {
        // TODO - place the new diplomessage sound here
        entries.push_back(event);
    }
public:
    void on_create(sys::state& state) noexcept override {
        {
            auto win1 = make_element_by_type<diplomessage_listbox>(state, id);
			diplomessage_win = win1.get();
			add_child_to_front(std::move(win1));
        }
        window_element_base::on_create(state);
    }

    void on_update(sys::state& state) noexcept override {
        for(size_t i = 0; i < entries.size(); i++) {
            if(entries[i].enddate <= state.current_date && entries[i].enddate.value != 0) {
                entries.erase(entries.begin() + i);
            }
        }
        window_element_base::on_update(state);
    }

    message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
        if(payload.holds_type<std::vector<evententry>>()) {
            payload.emplace<std::vector<evententry>>(entries);
            return message_result::consumed;
        }
        return message_result::unseen;
    }
};

}
