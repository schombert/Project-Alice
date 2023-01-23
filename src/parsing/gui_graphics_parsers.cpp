#include "gui_graphics.hpp"
#include "parsers_declarations.hpp"
#include "system_state.hpp"

namespace parsers {
	struct obj_and_horizontal {
		ui::gfx_object* obj = nullptr;
		bool horizontal = false;
	};
	obj_and_horizontal common_create_object(gfx_object const& obj_in, building_gfx_context& context) {
		auto gfxindex = context.ui_defs.gfx.size();
		context.ui_defs.gfx.emplace_back();
		ui::gfx_object& new_obj = context.ui_defs.gfx.back();

		context.map_of_names.insert_or_assign(std::string(obj_in.name), dcon::gfx_object_id(uint16_t(gfxindex)));

		if(obj_in.allwaystransparent) {
			new_obj.flags |= ui::gfx_object::always_transparent;
		}
		if(obj_in.clicksound_set) {
			new_obj.flags |= ui::gfx_object::has_click_sound;
		}
		if(obj_in.flipv) {
			new_obj.flags |= ui::gfx_object::flip_v;
		}
		if(obj_in.transparencecheck) {
			new_obj.flags |= ui::gfx_object::do_transparency_check;
		}

		new_obj.number_of_frames = uint8_t(obj_in.noofframes);

		if(obj_in.primary_texture.length() > 0) {
			if(auto it = context.map_of_texture_names.find(std::string(obj_in.primary_texture)); it != context.map_of_texture_names.end()) {
				new_obj.primary_texture_handle = it->second;
			} else {
				auto index = context.ui_defs.textures.size();
				context.ui_defs.textures.emplace_back(context.full_state.add_to_pool(obj_in.primary_texture));
				new_obj.primary_texture_handle = dcon::texture_id(uint16_t(index));
				context.map_of_texture_names.insert_or_assign(std::string(obj_in.primary_texture), dcon::texture_id(uint16_t(index)));
			}
		}
		if(obj_in.secondary_texture.length() > 0) {
			if(auto it = context.map_of_texture_names.find(std::string(obj_in.primary_texture)); it != context.map_of_texture_names.end()) {
				new_obj.type_dependant = uint16_t(it->second.index() + 1);
			} else {
				auto index = context.ui_defs.textures.size();
				context.ui_defs.textures.emplace_back(context.full_state.add_to_pool(obj_in.secondary_texture));
				new_obj.type_dependant = uint16_t(index + 1);
				context.map_of_texture_names.insert_or_assign(std::string(obj_in.secondary_texture), dcon::texture_id(uint16_t(index)));
			}
		}

		if(obj_in.size) {
			new_obj.size.x = int16_t(*obj_in.size);
			new_obj.size.y = int16_t(*obj_in.size);
		}
		if(obj_in.size_obj) {
			new_obj.size.x = int16_t(obj_in.size_obj->x);
			new_obj.size.y = int16_t(obj_in.size_obj->y);
		}
		if(obj_in.bordersize) {
			new_obj.type_dependant = uint16_t(obj_in.bordersize->x);
		}

		return obj_and_horizontal{ &new_obj, obj_in.horizontal };
	}

	void gfx_object_outer::spritetype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context) {
		auto res = common_create_object(obj, context);
		res.obj->flags |= uint8_t(ui::object_type::generic_sprite);

	}
	void gfx_object_outer::corneredtilespritetype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context) {
		auto res = common_create_object(obj, context);
		res.obj->flags |= uint8_t(ui::object_type::bordered_rect);
	}
	void gfx_object_outer::maskedshieldtype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context) {
		auto res = common_create_object(obj, context);
		res.obj->flags |= uint8_t(ui::object_type::flag_mask);
	}
	void gfx_object_outer::textspritetype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context) {
		auto res = common_create_object(obj, context);
		res.obj->flags |= uint8_t(ui::object_type::text_sprite);
	}
	void gfx_object_outer::tilespritetype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context) {
		auto res = common_create_object(obj, context);
		res.obj->flags |= uint8_t(ui::object_type::tile_sprite);
	}
	void gfx_object_outer::progressbartype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context) {
		auto res = common_create_object(obj, context);
		if(res.horizontal)
			res.obj->flags |= uint8_t(ui::object_type::horizontal_progress_bar);
		else
			res.obj->flags |= uint8_t(ui::object_type::vertical_progress_bar);
	}
	void gfx_object_outer::barcharttype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context) {
		auto res = common_create_object(obj, context);
		res.obj->flags |= uint8_t(ui::object_type::barchart);
	}
	void gfx_object_outer::piecharttype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context) {
		auto res = common_create_object(obj, context);
		res.obj->flags |= uint8_t(ui::object_type::piechart);
	}
	void gfx_object_outer::linecharttype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context) {
		auto res = common_create_object(obj, context);
		res.obj->flags |= uint8_t(ui::object_type::linegraph);
	}

	

	void gfx_add_obj(parsers::token_generator& gen, parsers::error_handler& err, building_gfx_context& context) {
		parsers::parse_gfx_object_outer(gen, err, context);
	}
}
