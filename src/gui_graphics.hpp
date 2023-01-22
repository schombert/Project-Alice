#pragma once

#include <vector>
#include "dcon_generated.hpp"
#include "unordered_dense.h"

namespace ui {
	enum class object_type : uint8_t {
		generic_sprite = 0x00,
		bordered_rect = 0x01,
		horizontal_progress_bar = 0x02,
		vertical_progress_bar = 0x03,
		flag_mask = 0x04,
		tile_sprite = 0x05,
		text_sprite = 0x06,
		barchart = 0x07,
		piechart = 0x08,
		linegraph = 0x09
	};

	struct xy_pair {
		int16_t x = 0;
		int16_t y = 0;
	};

	struct gfx_object {
		constexpr static uint8_t always_transparent = 0x10;
		constexpr static uint8_t flip_v = 0x20;
		constexpr static uint8_t has_click_sound = 0x40;
		constexpr static uint8_t do_transparency_check = 0x80;

		constexpr static uint8_t type_mask = 0x0F;

		xy_pair size; //4bytes

		dcon::texture_id primary_texture_handle; //6bytes
		uint16_t type_dependant = 0; // secondary texture handle or border size -- 8bytes

		uint8_t flags = 0; //9bytes
		uint8_t number_of_frames = 1; //10bytes
	};

	//very temporary
	struct texture_definition {
		std::string file_name;
	};

	class defintions {
	public:
		std::vector<gfx_object> gfx; // TODO: upgrade this to a vector that only accepts gfx_object_id as indices

		std::vector<texture_definition> textures;
		ankerl::unordered_dense::map<std::string, dcon::texture_id> map_of_textures;
	};
}