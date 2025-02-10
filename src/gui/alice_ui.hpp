#pragma once
#include "gui_element_base.hpp"
#include "system_state.hpp"
#include "economy_government.hpp"
#include "construction.hpp"
#include "graphics/color.hpp"
#include "province_templates.hpp"
#include "rebels.hpp"
#include "gui_population_window.hpp"

namespace alice_ui {

struct measure_result {
	int32_t x_space;
	int32_t y_space;
	enum class special {
		none, space_consumer, end_line, end_page, no_break
	} other;
};

enum class layout_type : uint8_t {
	single_horizontal,
	single_vertical,
	overlapped_horizontal,
	overlapped_vertical,
	mulitline_horizontal,
	multiline_vertical
};
enum class layout_line_alignment : uint8_t {
	leading, trailing, centered
};
enum class glue_type : uint8_t {
	standard,
	at_least,
	line_break,
	page_break,
	glue_don_t_break
};

struct layout_control {
	ui::element_base* ptr;
	int16_t abs_x = 0;
	int16_t abs_y = 0;
	bool absolute_position = false;
};
struct layout_window {
	std::unique_ptr<ui::element_base> ptr;
	int16_t abs_x = 0;
	int16_t abs_y = 0;
	bool absolute_position = false;
};
struct layout_glue {
	glue_type type = glue_type::standard;
	int16_t amount = 0;
};

class layout_generator {
public:
	virtual measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) = 0;
	virtual size_t item_count() = 0;
	virtual void reset_pools() = 0;
	virtual ~layout_generator() { }
};

struct generator_instance {
	layout_generator* generator;
};

struct layout_level;
struct sub_layout {
	std::unique_ptr<layout_level> layout;

	sub_layout() noexcept = default;
	sub_layout(sub_layout const& o) noexcept {
		std::abort();
	}
	sub_layout(sub_layout&& o) noexcept = default;
	sub_layout& operator=(sub_layout&& o) noexcept = default;
	~sub_layout() = default;
};

enum class layout_item_types : uint8_t {
	control, window, glue, generator, layout
};

using layout_item = std::variant<std::monostate, layout_control, layout_window, layout_glue, generator_instance, sub_layout>;

class page_buttons;
class layout_window_element;

enum class animation_type : uint8_t {
	none,
	page_left,
	page_right,
	page_up,
	page_middle,
};

struct page_info {
	int16_t last_index;
	int16_t last_sub_index;
	int16_t space_used;
	int16_t space_consumer_count;
	int16_t non_glue_count;
};
struct layout_level {
	std::vector<layout_item> contents;
	std::unique_ptr<page_buttons> page_controls;
	std::vector<page_info> page_starts;
	int16_t current_page = 0;
	int16_t size_x = -1;
	int16_t size_y = -1;
	int16_t margin_top = 0;
	int16_t margin_bottom = -1;
	int16_t margin_left = -1;
	int16_t margin_right = -1;
	int16_t resolved_x_pos = 0;
	int16_t resolved_y_pos = 0;
	int16_t resolved_x_size = 0;
	int16_t resolved_y_size = 0;
	layout_line_alignment line_alignment = layout_line_alignment::leading;
	layout_line_alignment line_internal_alignment = layout_line_alignment::leading;
	layout_type type = layout_type::single_horizontal;
	animation_type page_animation;
	uint8_t interline_spacing = 0;
	bool paged = false;

	void change_page(sys::state& state, layout_window_element& container, int32_t new_page);
};

struct layout_window;

class page_buttons : public ui::element_base {
public:
	text::layout text_layout;

	layout_level* for_layout = nullptr;
	int16_t last_page = -1;
	int16_t last_size = -1;

	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return ui::tooltip_behavior::no_tooltip;
	}
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(type == ui::mouse_probe_type::click && last_size > 1) {
			return ui::message_result::consumed;
		} else {
			return ui::message_result::unseen;
		}
	}
	
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		return ui::message_result::consumed;
	}
	void on_update(sys::state& state) noexcept override;
};

struct layout_iterator;

class layout_window_element : public ui::non_owning_container_base {
private:
	void remake_layout_internal(layout_level& lvl, sys::state& state, int32_t x, int32_t y, int32_t w, int32_t h, bool remake_lists);
	void clear_pages_internal(layout_level& lvl);
public:
	layout_level layout;
	
	std::string_view page_left_texture_key;
	std::string_view page_right_texture_key;
	dcon::texture_id page_left_texture_id;
	dcon::texture_id page_right_texture_id;
	text::text_color page_text_color = text::text_color::black;

	void remake_layout(sys::state& state, bool remake_lists) {
		children.clear();
		if(remake_lists)
			clear_pages_internal(layout);
		remake_layout_internal(layout, state, 0, 0, base_data.size.x, base_data.size.y, remake_lists);
		std::reverse(children.begin(), children.end());
	}
	ui::message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override;
	void impl_on_update(sys::state& state) noexcept override;

	friend struct layout_iterator;
};

namespace budget_categories {
inline constexpr int32_t diplomatic_income = 0;
inline constexpr int32_t poor_tax = 1;
inline constexpr int32_t middle_tax = 2;
inline constexpr int32_t rich_tax = 3;
inline constexpr int32_t tariffs_import = 4;
inline constexpr int32_t tariffs_export = 5;
inline constexpr int32_t gold = 6;
inline constexpr int32_t diplomatic_expenses = 7;
inline constexpr int32_t social = 8;
inline constexpr int32_t military = 9;
inline constexpr int32_t education = 10;
inline constexpr int32_t admin = 11;
inline constexpr int32_t domestic_investment = 12;
inline constexpr int32_t overseas_spending = 13;
inline constexpr int32_t subsidies = 14;
inline constexpr int32_t construction = 15;
inline constexpr int32_t army_upkeep = 16;
inline constexpr int32_t navy_upkeep = 17;
inline constexpr int32_t debt_payment = 18;
inline constexpr int32_t stockpile = 19;
inline constexpr int32_t category_count = 20;
inline static bool expanded[category_count] = { false };
}

namespace popwindow {
inline static ankerl::unordered_dense::set<int32_t> open_states;
inline static ankerl::unordered_dense::set<int32_t> open_provs;
inline static ankerl::unordered_dense::set<int32_t> excluded_types;
inline static ankerl::unordered_dense::set<int32_t> excluded_cultures;
inline static ankerl::unordered_dense::set<int32_t> excluded_states;
inline static ankerl::unordered_dense::set<int32_t> excluded_religions;
inline static bool show_non_colonial = true;
inline static bool show_colonial = true;
inline static bool show_only_pops = false;
}

bool pop_passes_filter(sys::state& state, dcon::pop_id p);

std::unique_ptr<ui::element_base> make_macrobuilder2_main(sys::state& state);
std::unique_ptr<ui::element_base> make_budgetwindow_main(sys::state& state);
std::unique_ptr<ui::element_base> make_demographicswindow_main(sys::state& state);
std::unique_ptr<ui::element_base> make_pop_details_main(sys::state& state);

inline int8_t cmp3(std::string_view a, std::string_view b) {
	return int8_t(std::clamp(a.compare(b), -1, 1));
}
template<typename T>
int8_t cmp3(T const& a, T const& b) {
	if(a == b) return int8_t(0);
	return (a < b) ? int8_t(-1) : int8_t(1);
}

void describe_conversion(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_migration(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_colonial_migration(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_emigration(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_promotion(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_demotion(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_con(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_mil(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_lit(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_growth(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_assimilation(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);

}
