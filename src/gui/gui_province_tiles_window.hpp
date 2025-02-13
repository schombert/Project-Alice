#include "gui_element_types.hpp"
#include "construction.hpp"

namespace ui {

class province_tile : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		disabled = false;
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto fid = retrieve<dcon::factory_id>(state, parent);
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
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
	std::vector< element_base*> tiles = std::vector<element_base*>(64);
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		for(int i = 0; i < 8; i++) {
			for(int j = 0; j < 8; j++) {
				auto ptr = make_element_by_type<image_element_base>(state, "province_tiles_tile");
				ptr->base_data.position.y += int16_t((ptr->base_data.size.y + 1) * i);
				ptr->base_data.position.x += int16_t((ptr->base_data.size.x + 1) * j);
				tiles[i * 8 + j] = ptr.get();

				add_child_to_front(std::move(ptr));
			}
		}
	}
};


}
