#pragma once

#include "dcon_generated_ids.hpp"
#include "system_state_forward.hpp"
#include "text.hpp"
#include "gui_element_types.hpp"

namespace ui {

struct release_query_wrapper {
	dcon::nation_id content;
};
struct release_emplace_wrapper {
	dcon::nation_id content;
};

void produce_decision_substitutions(sys::state& state, text::substitution_map& m, dcon::nation_id n);
void reform_rules_description(sys::state& state, text::columnar_layout& contents, uint32_t rules);
void reform_description(sys::state& state, text::columnar_layout& contents, dcon::issue_option_id ref);
void reform_description(sys::state& state, text::columnar_layout& contents, dcon::reform_option_id ref);

class reforms_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
};
class unciv_reforms_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
};

class movements_list;

class movements_window : public window_element_base {
private:
	movements_list* movements_listbox = nullptr;
public:
	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

class decision_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
};

class release_nation_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
};

std::unique_ptr<element_base> make_release_nation_window(sys::state& state, std::string_view name);

}
