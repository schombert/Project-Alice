#pragma once

#include "gui_element_types.hpp"
#include "military.hpp"
#include "ai.hpp"

namespace ui {

enum class province_tile_types : uint8_t {
	province_building,
	factory,
	regiment,
};

class tile_type_logic {
public:
	virtual dcon::text_key get_name(sys::state& state, economy::province_tile target) {
		return state.lookup_key("null");
	}

	virtual int get_frame(sys::state& state, economy::province_tile target) {
		return 0;
	}

	virtual bool is_available(sys::state& state, economy::province_tile target) {
		return false;
	}

	virtual void button_action(sys::state& state, economy::province_tile target, ui::element_base* parent) {

	}

	virtual void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, economy::province_tile target) {

	}
	virtual ~tile_type_logic() {
	};
};


class empty_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, economy::province_tile target) noexcept override {
		return state.lookup_key("???");
	}

	bool is_available(sys::state& state, economy::province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, economy::province_tile target) {
		return 0;
	}

	void button_action(sys::state& state, economy::province_tile target, ui::element_base* parent) noexcept override { }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, economy::province_tile target) noexcept override {
		
	}
};

class factory_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, economy::province_tile target) noexcept override {
		auto type = state.world.factory_get_building_type(target.factory);
		return state.world.factory_type_get_name(type);
	}

	bool is_available(sys::state& state, economy::province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, economy::province_tile target) {
		return 5;
	}

	void button_action(sys::state& state, economy::province_tile target, ui::element_base* parent) noexcept override {
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, economy::province_tile target) noexcept override {
		auto type = state.world.factory_get_building_type(target.factory);

		text::add_line(state, contents, state.world.factory_type_get_name(type));
		text::add_line_break_to_layout(state, contents);

		text::add_line_with_condition(state, contents, "iaction_explain_5", state.world.nation_get_is_great_power(state.local_player_nation));

	}
};

class rgo_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, economy::province_tile target) noexcept override {
		return state.world.commodity_get_name(target.rgo_commodity);
	}

	bool is_available(sys::state& state, economy::province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, economy::province_tile target) {
		return (state.world.commodity_get_is_mine(target.rgo_commodity) ? 3 : 2);
	}

	void button_action(sys::state& state, economy::province_tile target, ui::element_base* parent) noexcept override { }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, economy::province_tile target) noexcept override {
		text::add_line(state, contents, state.world.commodity_get_name(target.rgo_commodity));
		text::add_line_break_to_layout(state, contents);

		text::add_line_with_condition(state, contents, "iaction_explain_5", state.world.nation_get_is_great_power(state.local_player_nation));
	}
};


class regiment_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, economy::province_tile target) noexcept override {
		auto type = state.world.regiment_get_type(target.regiment);
		return state.military_definitions.unit_base_definitions[type].name;
	}

	bool is_available(sys::state& state, economy::province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, economy::province_tile target) {
		return 10;
	}

	void button_action(sys::state& state, economy::province_tile target, ui::element_base* parent) noexcept override { }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, economy::province_tile target) noexcept override {
		auto type = state.world.regiment_get_type(target.regiment);
		text::add_line(state, contents, state.military_definitions.unit_base_definitions[type].name);
		text::add_line_break_to_layout(state, contents);

		text::add_line_with_condition(state, contents, "iaction_explain_5", state.world.nation_get_is_great_power(state.local_player_nation));
	}
};


class province_building_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, economy::province_tile target) noexcept override {
		return state.lookup_key(economy::province_building_type_get_name(target.province_building));
	}

	bool is_available(sys::state& state, economy::province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, economy::province_tile target) {
		if(target.province_building == economy::province_building_type::railroad) {
			return 8;
		} else if(target.province_building == economy::province_building_type::naval_base) {
			return 9;
		} else if(target.province_building == economy::province_building_type::fort) {
			return 13;
		}
		return 0;
	}

	void button_action(sys::state& state, economy::province_tile target, ui::element_base* parent) noexcept override { }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, economy::province_tile target) noexcept override {
		text::add_line(state, contents, state.lookup_key(economy::province_building_type_get_name(target.province_building)));
		text::add_line_break_to_layout(state, contents);

		text::add_line_with_condition(state, contents, "iaction_explain_5", state.world.nation_get_is_great_power(state.local_player_nation));
	}
};

} // namespace ui
