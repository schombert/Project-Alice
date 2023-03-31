#pragma once

#include <vector>

#include "gui_element_types.hpp"
#include "triggers.hpp"

namespace ui {

class decision_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override;
};

class decision_image : public image_element_base {};
class decision_desc : public window_element_base {};
class decision_requirements : public image_element_base {};
class ignore_checkbox : public checkbox_button {};
class make_decision : public  button_element_base {};

class decision_item : public listbox_row_element_base<dcon::decision_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

class decision_listbox : public listbox_element_base<decision_item, dcon::decision_id> {
protected:
  std::string_view get_row_element_name() override;
};

class decision_window : public window_element_base {
private:
  decision_listbox* decision_list{nullptr};
	std::vector<dcon::decision_id> get_decisions(sys::state& state);
public:
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
};

} //namespace