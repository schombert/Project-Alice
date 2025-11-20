#pragma once

#include "gui_element_types.hpp"
#include "construction.hpp"
#include "province_tiles.hpp"
#include "gui_province_tiles_tiles.hpp"

namespace ui {

inline static commodity_tile commodity_tile_logic;

inline static province_tile retrieve_nth_province_market_commodity(sys::state& state, dcon::province_id prov, int ind) {
	dcon::commodity_id cid{ dcon::commodity_id::value_base_t(ind) };
	auto res = province_tile{};
	uint32_t total_commodities = state.world.commodity_size();

	if(cid.index() >= int(total_commodities)) {
		res.empty = true;
	} else {
		res.commodity = cid;
	}
	return res;
}

class province_market_commodity_button : public button_element_base {
public:
	int ind = 0;

	tile_type_logic* tile_logic;
	image_element_base* subicon_commodity;

	void on_update(sys::state& state) noexcept override {
		auto tile = retrieve_nth_province_market_commodity(state, state.map_state.selected_province, ind);
		uint32_t total_commodities = state.world.commodity_size();

		if(tile.empty) {
			tile_logic = &empty_tile_logic;
		}
		else {
			tile_logic = &commodity_tile_logic;
		}

		frame = tile_logic->get_frame(state, tile);
		subicon_commodity->frame = state.world.commodity_get_icon(tile_logic->get_commodity_frame(state, tile));

		hide_context_menu(state);
		hide_factory_refit_menu(state);
	}
	void button_action(sys::state& state) noexcept override {
		hide_context_menu(state);
		hide_factory_refit_menu(state);
		auto tile = retrieve_nth_province_market_commodity(state, state.map_state.selected_province, ind);
		tile_logic->button_action(state, tile, parent);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto tile = retrieve_nth_province_tile(state, state.map_state.selected_province, ind);
		tile_logic->update_tooltip(state, x, y, contents, tile);
	}

	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
	}
};

class province_market_window : public window_element_base {
public:
	std::vector<province_market_commodity_button*> tiles = std::vector<province_market_commodity_button*>(96);
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		for(int i = 0; i < 12; i++) {
			for(int j = 0; j < 8; j++) {
				auto ptr = make_element_by_type<province_market_commodity_button>(state, "province_tiles_tile");
				ptr->base_data.position.y += int16_t(ptr->base_data.size.y * i);
				ptr->base_data.position.x += int16_t(ptr->base_data.size.x * j);

				tiles[i * 8 + j] = ptr.get();
				tiles[i * 8 + j]->ind = i * 8 + j;

				auto ptr2 = make_element_by_type<image_element_base>(state, "province_tiles_tile_subicon_commodity");
				ptr->subicon_commodity = ptr2.get();
				ptr2->base_data.position.y += int16_t(ptr->base_data.size.y * i);
				ptr2->base_data.position.x += int16_t(ptr->base_data.size.x * j);

				add_child_to_front(std::move(ptr));
				add_child_to_front(std::move(ptr2));
			}
		}
	}
};

}
