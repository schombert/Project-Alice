#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"
#include "constants_ui.hpp"
#include "dcon_generated_ids.hpp"
#include "unordered_dense.h"
#include "container_types_ui.hpp"
#include "fonts.hpp"

#include "immediate_mode_state.hpp"


namespace text {
struct text_chunk;
struct endless_layout;
struct layout;
}

namespace ogl {
struct color3f;
}

namespace sys {
struct state;
};

namespace ui {
void render_text_chunk(
	sys::state& state,
	text::text_chunk t,
	float x,
	float baseline_y,
	uint16_t font_id,
	ogl::color3f text_color,
	ogl::color_modification cmod
);
ogl::color3f get_text_color(sys::state& state, text::text_color text_color);
};


namespace iui {

enum class alignment_vertical : uint8_t {
	top, center, horizontal
};
enum class alignment_horizontal : uint8_t {
	left, center, right
};


std::string inline localize_commodity_info_mode(commodity_info_mode mode) {
	switch(mode) {
	case commodity_info_mode::price:
		return "price";
	case commodity_info_mode::supply:
		return "supply";
	case commodity_info_mode::demand:
		return "demand";
	case commodity_info_mode::production:
		return "production";
	case commodity_info_mode::consumption:
		return "consumption";
	case commodity_info_mode::stockpiles:
		return "stockpiles";
	case commodity_info_mode::balance:
		return "balance";
	case commodity_info_mode::trade_in:
		return "trade_in";
	case commodity_info_mode::trade_out:
		return "trade_out";
	case commodity_info_mode::trade_balance:
		return "trade_balance";
	case commodity_info_mode::potentials:
		return "potentials";
	default:
		return "alice_invalid_value";
	}
};

std::string inline localize_labor_info_mode(labor_info_mode mode) {
	switch(mode) {
	case labor_info_mode::price:
		return "labour_price_10k";
	case labor_info_mode::supply:
		return "supply";
	case labor_info_mode::demand:
		return "demand";
	case labor_info_mode::supply_demand_ratio:
		return "supply_demand_ratio";
	default:
		return "alice_invalid_value";
	}
};



std::string inline localize_trade_volume_info_mode(trade_volume_info_mode mode) {
	switch(mode) {
	case trade_volume_info_mode::exported_volume:
		return "export";
	case trade_volume_info_mode::imported_volume:
		return "import";
	case trade_volume_info_mode::total_volume:
		return "total_volume";
	case trade_volume_info_mode::trade_balance:
		return "trade_balance";
	case trade_volume_info_mode::embargo:
		return "embargo";
	default:
		return "alice_invalid_value";
	}
};

std::string inline localize_infrastructure_mode(infrastructure_mode mode) {
	switch(mode) {
	case infrastructure_mode::civilian_ports:
		return "ports_size";
	default:
		return "alice_invalid_value";
	}
};

void shrink(rect& rectangle, float value);
void move_to(rect& rectangle, float x, float y);
rect subrect(rect& rectangle, float w, float h, alignment_horizontal align_hor, alignment_vertical align_vert);
}
