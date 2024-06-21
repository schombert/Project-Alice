#pragma once

#include <variant>
#include "gui_element_types.hpp"

namespace ui {

struct option_taken_notification {
	int a = 0;
};

using event_data_wrapper =  std::variant< event::pending_human_n_event, event::pending_human_f_n_event, event::pending_human_p_event, event::pending_human_f_p_event>;

class event_option_button : public button_element_base {
public:
	sound::audio_instance& get_click_sound(sys::state& state) noexcept override {
		return sound::get_event_sound(state);
	}
	void on_update(sys::state& state) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void button_action(sys::state& state) noexcept override;
};
class event_auto_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "event_auto");
	}
	void button_action(sys::state& state) noexcept override;
};
class event_option_button_row : public listbox_row_element_base<int32_t> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "opt_button") {
			return make_element_by_type<event_option_button>(state, id);
		} else if(name == "auto_button") {
			return make_element_by_type<event_auto_button>(state, id);
		} else {
			return nullptr;
		}
	}
};
class event_image : public image_element_base {
public:
	bool get_horizontal_flip(sys::state& state) noexcept override {
		return false; //never flip
	}
	void on_update(sys::state& state) noexcept override;
};
class event_desc_text : public scrollable_text {
public:
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
class event_name_text : public multiline_text_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		multiline_text_element_base::on_create(state);
		on_reset_text(state);
	}
	void on_reset_text(sys::state& state) noexcept override {
		//TODO: Adapt to classic fonts too!
		auto fh = base_data.data.text.font_handle;
		auto font_index = text::font_index_from_font_id(state, fh);
		auto font_size = text::size_from_font_id(fh);
		auto& font = state.font_collection.get_font(state, font_index);
		auto text_height = int32_t(std::ceil(font.line_height(font_size)));
		base_data.size.y = int16_t((text_height - 15) * 2);
	}
	void on_update(sys::state& state) noexcept override;
};
class event_requirements_icon : public image_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};
class event_odds_icon : public image_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};

class event_subtitle_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override;
};
class event_state_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override;
};
class event_population_amount_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override;
};
class election_issue_support_item : public listbox_row_element_base<dcon::issue_option_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
};
class elections_issue_option_listbox : public listbox_element_base<election_issue_support_item, dcon::issue_option_id> {
protected:
	std::string_view get_row_element_name() override {
		return "ew_issue_option_window";
	}
public:
	void on_update(sys::state& state) noexcept override;
};

//
// National events
//
enum event_pool_slot : uint8_t {
	country,
	country_election,
	country_major,
	province,
	count
};
class base_event_option_listbox : public listbox_element_base<event_option_button_row, int32_t> {
public:
	void on_update(sys::state& state) noexcept override;
};
class national_event_listbox : public base_event_option_listbox {
protected:
	std::string_view get_row_element_name() override {
		return "alice_nation_event_button";
	}
};
class provincial_event_listbox : public base_event_option_listbox {
protected:
	std::string_view get_row_element_name() override {
		return "alice_prov_event_button";
	}
};
class national_election_event_listbox : public base_event_option_listbox {
protected:
	std::string_view get_row_element_name() override {
		return "alice_election_event_button";
	}
};
class base_event_window : public window_element_base {
	elections_issue_option_listbox* issues_listbox = nullptr;
public:
	virtual std::string_view get_option_start_element_name() noexcept {
		return "event_country_option_start";
	}
	virtual event_pool_slot get_pool_slot() noexcept {
		return event_pool_slot::country;
	}
	event_image* image = nullptr;
	event_data_wrapper event_data;
	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
class national_election_event_window : public base_event_window {
public:
	static constexpr std::string_view window_element_name = "event_election_window";
	std::string_view get_option_start_element_name() noexcept override {
		return "eew_event_option_start";
	}
	event_pool_slot get_pool_slot() noexcept override {
		return event_pool_slot::country_election;
	}
};
class national_event_window : public base_event_window {
public:
	static constexpr std::string_view window_element_name = "event_country_window";
	std::string_view get_option_start_element_name() noexcept override {
		return "event_country_option_start";
	}
	event_pool_slot get_pool_slot() noexcept override {
		return event_pool_slot::country;
	}
};
class national_major_event_window : public base_event_window {
public:
	static constexpr std::string_view window_element_name = "event_major_window";
	std::string_view get_option_start_element_name() noexcept override {
		return "event_major_option_start";
	}
	event_pool_slot get_pool_slot() noexcept override {
		return event_pool_slot::country_major;
	}
};
class provincial_event_window : public base_event_window {
public:
	static constexpr std::string_view window_element_name = "event_province_window";
	std::string_view get_option_start_element_name() noexcept override {
		return "event_province_option_start";
	}
	event_pool_slot get_pool_slot() noexcept override {
		return event_pool_slot::province;
	}
};

void populate_event_submap(sys::state& state, text::substitution_map& sub, std::variant<event::pending_human_n_event, event::pending_human_f_n_event, event::pending_human_p_event, event::pending_human_f_p_event> const& phe) noexcept;
void close_expired_event_windows(sys::state& state);
void clear_event_windows(sys::state& state);
bool events_pause_test(sys::state& state);
void new_event_window(sys::state& state, event_data_wrapper dat);

} // namespace ui
