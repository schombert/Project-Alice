#include "gui_element_types.hpp"
#include "construction.hpp"
#include "province_tiles.hpp"


namespace ui {

class province_tile : public button_element_base {
public:
	int ind = 0;

	void on_update(sys::state& state) noexcept override {
		auto tile = economy::retrieve_nth_province_tile(state, state.local_player_nation, state.map_state.selected_province, ind);

		if(tile.rgo_commodity && state.world.commodity_get_is_mine(tile.rgo_commodity)) {
			frame = 3;
		}
		else if(tile.rgo_commodity) {
			frame = 2;
		}
		else if(tile.factory) {
			frame = 5;
		}
		else if(tile.regiment) {
			frame = 10;
		}
		else if(tile.empty) {
			frame = 0;
		}
		else if(tile.railroad) {
			frame = 8;
		}
		else if(tile.naval_base) {
			frame = 9;
		}
		else if(tile.fort) {
			frame = 13;
		}
	}
	void button_action(sys::state& state) noexcept override {
		// send(state, parent, open_factory_refit{fid});
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}


	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_9", false);
	}
};


class province_tiles_window : public window_element_base {
public:
	std::vector< province_tile*> tiles = std::vector<province_tile*>(64);
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		for(int i = 0; i < 8; i++) {
			for(int j = 0; j < 8; j++) {
				auto ptr = make_element_by_type<province_tile>(state, "province_tiles_tile");
				ptr->base_data.position.y += int16_t((ptr->base_data.size.y + 1) * i);
				ptr->base_data.position.x += int16_t((ptr->base_data.size.x + 1) * j);

				tiles[i * 8 + j] = ptr.get();
				tiles[i * 8 + j]->ind = i * 8 + j;

				add_child_to_front(std::move(ptr));
			}
		}
	}
};


}
