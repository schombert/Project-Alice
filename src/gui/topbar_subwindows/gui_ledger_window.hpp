#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"

namespace ui {

class ledger_page_number {
public:
    int8_t value;
};

class ledger_prev_button : public generic_settable_element<button_element_base, ledger_page_number> {
public:
    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any new_payload = ledger_page_number{ int8_t(content.value - 1) };
            parent->impl_set(state, new_payload);
        }
    }
};

class ledger_next_button : public generic_settable_element<button_element_base, ledger_page_number> {
public:
    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any new_payload = ledger_page_number{ int8_t(content.value + 1) };
            parent->impl_set(state, new_payload);
        }
    }
};

class ledger_window : public window_element_base {
    window_element_base* linegraph = nullptr;
    window_element_base* linegraph_legend = nullptr;
    image_element_base* linegraph_image = nullptr;
    simple_text_element_base* page_number = nullptr;
    ledger_page_number page_num{};
public:
    std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "ledger_linegraphs") {
            auto ptr = make_element_by_type<window_element_base>(state, id);
            linegraph = ptr.get();
            return ptr;
        } else if(name == "ledger_linegraph_legend") {
            auto ptr = make_element_by_type<window_element_base>(state, id);
            linegraph_legend = ptr.get();
            return ptr;
        } else if(name == "ledger_linegraph_bg") {
            auto ptr = make_element_by_type<image_element_base>(state, id);
            linegraph_image = ptr.get();
            return ptr;
        } else if(name == "page_number") {
            auto ptr = make_element_by_type<simple_text_element_base>(state, id);
            page_number = ptr.get();
            return ptr;
        } else if(name == "prev") {
            return make_element_by_type<ledger_prev_button>(state, id);
        } else if(name == "next") {
            return make_element_by_type<ledger_next_button>(state, id);
        } else {
			return nullptr;
		}
	}

    void on_update(sys::state& state) noexcept override {
        // TODO: what did each page display???
        bool has_linegraphs = (page_num.value == 5);
        linegraph->set_visible(state, has_linegraphs);
        linegraph_legend->set_visible(state, has_linegraphs);
        linegraph_image->set_visible(state, has_linegraphs);
        page_number->set_text(state, std::to_string(page_num.value));
    }

    message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
        if(payload.holds_type<ledger_page_number>()) {
            page_num = any_cast<ledger_page_number>(payload);
            for(auto& child : children)
                child->impl_set(state, payload);
            on_update(state);
            return message_result::consumed;
        }
        return message_result::unseen;
    }
};
}
