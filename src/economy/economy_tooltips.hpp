#pragma once
#include "dcon_generated_ids.hpp"
#include "text.hpp"

namespace economy {

void factory_construction_tooltip(
	sys::state& state,
	text::columnar_layout& contents,
	dcon::factory_construction_id fcid
);
void build_land_unit_construction_tooltip(
	sys::state& state,
	text::columnar_layout& contents,
	dcon::province_land_construction_id conid
);
void build_naval_unit_construction_tooltip(
	sys::state& state,
	text::columnar_layout& contents,
	dcon::province_naval_construction_id conid
);
void make_trade_center_tooltip(
	sys::state& state,
	text::columnar_layout& contents,
	dcon::market_id market
);
void make_trade_volume_tooltip(
	sys::state& state,
	text::columnar_layout& contents,
	dcon::trade_route_id route,
	dcon::commodity_id cid,
	dcon::market_id point_of_view
);

}
