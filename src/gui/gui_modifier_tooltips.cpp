#include <string_view>
#include "dcon_generated.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include "script_constants.hpp"

namespace ui {

enum class modifier_display_type : uint8_t {
	integer,
	percent,
	fp_two_places,
	fp_three_places,
};
struct modifier_display_info {
	bool positive_is_green;
	modifier_display_type type;
	std::string_view name;
};

static const modifier_display_info province_modifier_names[sys::provincial_mod_offsets::count] = {
#define MOD_LIST_ELEMENT(num, name, green_is_negative, display_type, locale_name) \
	modifier_display_info{green_is_negative, display_type, locale_name},
    MOD_PROV_LIST
#undef MOD_LIST_ELEMENT
};
static const modifier_display_info national_modifier_names[sys::national_mod_offsets::count] = {
#define MOD_LIST_ELEMENT(num, name, green_is_negative, display_type, locale_name) \
	modifier_display_info{green_is_negative, display_type, locale_name},
    MOD_NAT_LIST
#undef MOD_LIST_ELEMENT
};

std::string format_modifier_value(sys::state &state, float value, modifier_display_type type) {
	switch (type) {
	case modifier_display_type::integer:
		return (value > 0.f ? "+" : "") + text::prettify(int64_t(value));
	case modifier_display_type::percent:
		return (value > 0.f ? "+" : "") + text::format_percentage(value, 1);
	case modifier_display_type::fp_two_places:
		return text::format_float(value, 2);
	case modifier_display_type::fp_three_places:
		return text::format_float(value, 3);
	}
	return "x%";
}

void modifier_description(sys::state &state, text::layout_base &layout, dcon::modifier_id mid, int32_t indentation) {
	auto fat_id = dcon::fatten(state.world, mid);

	const auto &prov_def = fat_id.get_province_values();
	for (uint32_t i = 0; i < prov_def.modifier_definition_size; ++i) {
		if (prov_def.values[i] == 0.f)
			continue;

		auto offset = uint32_t(prov_def.offsets[i].index());
		auto data = province_modifier_names[offset];

		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, state, box, text::produce_simple_string(state, data.name), text::text_color::white);
		text::add_to_layout_box(layout, state, box, std::string_view{":"}, text::text_color::white);
		text::add_space_to_layout_box(layout, state, box);
		auto color = data.positive_is_green
		                 ? (prov_def.values[i] > 0.f ? text::text_color::green : text::text_color::red)
		                 : (prov_def.values[i] > 0.f ? text::text_color::red : text::text_color::green);
		text::add_to_layout_box(layout, state, box, format_modifier_value(state, prov_def.values[i], data.type), color);
		text::close_layout_box(layout, box);
	}

	const auto &nat_def = fat_id.get_national_values();
	for (uint32_t i = 0; i < nat_def.modifier_definition_size; ++i) {
		if (nat_def.values[i] == 0.f)
			continue;

		auto offset = uint32_t(nat_def.offsets[i].index());
		auto data = national_modifier_names[offset];

		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(layout, state, box, text::produce_simple_string(state, data.name), text::text_color::white);
		text::add_to_layout_box(layout, state, box, std::string_view{":"}, text::text_color::white);
		text::add_space_to_layout_box(layout, state, box);
		auto color = data.positive_is_green
		                 ? (nat_def.values[i] > 0.f ? text::text_color::green : text::text_color::red)
		                 : (nat_def.values[i] > 0.f ? text::text_color::red : text::text_color::green);
		text::add_to_layout_box(layout, state, box, format_modifier_value(state, nat_def.values[i], data.type), color);
		text::close_layout_box(layout, box);
	}
}

} // namespace ui
