#include "gui_element_types.hpp"
#include "construction.hpp"
#include "province_tiles.hpp"
#include <gui_province_tiles_tiles.hpp>


namespace ui {

inline static empty_tile empty_tile_logic;
inline static factory_tile factory_tile_logic;
inline static rgo_tile rgo_tile_logic;
inline static regiment_tile regiment_tile_logic;
inline static province_building_tile province_building_tile_logic;

class province_tile : public button_element_base {
public:
	int ind = 0;

	tile_type_logic* tile_logic;

	void on_update(sys::state& state) noexcept override {
		auto tile = economy::retrieve_nth_province_tile(state, state.map_state.selected_province, ind);

		if(tile.empty) {
			tile_logic = &empty_tile_logic;
		}
		else if(tile.rgo_commodity) {
			tile_logic = &rgo_tile_logic;
		}
		else if(tile.factory) {
			tile_logic = &factory_tile_logic;
		}
		else if(tile.regiment) {
			tile_logic = &regiment_tile_logic;
		}
		else if(tile.has_province_building) {
			tile_logic = &province_building_tile_logic;
		}

		frame = tile_logic->get_frame(state, tile);
	}
	void button_action(sys::state& state) noexcept override {
		// send(state, parent, open_factory_refit{fid});
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}


	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto tile = economy::retrieve_nth_province_tile(state, state.map_state.selected_province, ind);
		tile_logic->update_tooltip(state, x, y, contents, tile);
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
