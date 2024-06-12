#include "news_parsing.hpp"

namespace parsers {

void news_picture_case::picture(association_type, std::string_view name, error_handler& err, int32_t line, news_context& context) {
	auto root = get_root(context.outer_context.state.common_fs);
	auto gfx = open_directory(root, NATIVE("gfx"));
	auto pictures = open_directory(gfx, NATIVE("pictures"));

	std::string file_name = simple_fs::remove_double_backslashes(std::string("gfx\\pictures\\") + [&]() {
		if(peek_file(pictures, simple_fs::utf8_to_native(name) + NATIVE(".tga"))) {
			return std::string(name) + ".tga";
		} else if(peek_file(pictures, simple_fs::utf8_to_native(name) + NATIVE(".dds"))) {
			return std::string(name) + ".tga";
		} else {
			return std::string("news\\canal.tga");
		}
	}());

	if(auto it = context.outer_context.gfx_context.map_of_names.find(file_name);
			it != context.outer_context.gfx_context.map_of_names.end()) {
		picture_ = it->second;
	} else {
		auto gfxindex = context.outer_context.state.ui_defs.gfx.size();
		context.outer_context.state.ui_defs.gfx.emplace_back();
		ui::gfx_object& new_obj = context.outer_context.state.ui_defs.gfx.back();
		auto new_id = dcon::gfx_object_id(uint16_t(gfxindex));
		context.outer_context.gfx_context.map_of_names.insert_or_assign(file_name, new_id);
		new_obj.number_of_frames = uint8_t(1);
		if(auto itb = context.outer_context.gfx_context.map_of_texture_names.find(file_name);
				itb != context.outer_context.gfx_context.map_of_texture_names.end()) {
			new_obj.primary_texture_handle = itb->second;
		} else {
			auto index = context.outer_context.state.ui_defs.textures.size();
			context.outer_context.state.ui_defs.textures.emplace_back(context.outer_context.state.add_to_pool(file_name));
			new_obj.primary_texture_handle = dcon::texture_id(uint16_t(index));
			context.outer_context.gfx_context.map_of_texture_names.insert_or_assign(file_name, dcon::texture_id(uint16_t(index)));
		}
		new_obj.flags |= uint8_t(ui::object_type::generic_sprite);
		picture_ = new_id;
	}
}

void news_generate_article::type(association_type, std::string_view value, error_handler& err, int32_t line, news_context& context) {
	if(is_fixed_token_ci(value.data(), value.data() + value.length(), "peace_offer_accept")) {
		flags |= uint8_t(sys::news_generator_type::peace_offer_accept);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "game_event")) {
		flags |= uint8_t(sys::news_generator_type::game_event);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "province_change_controller")) {
		flags |= uint8_t(sys::news_generator_type::province_change_controller);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "province_change_owner")) {
		flags |= uint8_t(sys::news_generator_type::province_change_owner);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "construction_complete")) {
		flags |= uint8_t(sys::news_generator_type::construction_complete);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "research_complete")) {
		flags |= uint8_t(sys::news_generator_type::research_complete);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "battle_over")) {
		flags |= uint8_t(sys::news_generator_type::battle_over);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "rebel_break_country")) {
		flags |= uint8_t(sys::news_generator_type::rebel_break_country);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "new_party")) {
		flags |= uint8_t(sys::news_generator_type::new_party);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "war_declared")) {
		flags |= uint8_t(sys::news_generator_type::war_declared);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "crisis_started")) {
		flags |= uint8_t(sys::news_generator_type::crisis_started);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "crisis_backer")) {
		flags |= uint8_t(sys::news_generator_type::crisis_backer);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "crisis_side_joined")) {
		flags |= uint8_t(sys::news_generator_type::crisis_side_joined);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "crisis_resolved")) {
		flags |= uint8_t(sys::news_generator_type::crisis_resolved);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "decision")) {
		flags |= uint8_t(sys::news_generator_type::decision);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "goods_price_change")) {
		flags |= uint8_t(sys::news_generator_type::goods_price_change);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "ai_afraid_of")) {
		flags |= uint8_t(sys::news_generator_type::ai_afraid_of);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "ai_likes_very_much")) {
		flags |= uint8_t(sys::news_generator_type::ai_likes_very_much);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "fake")) {
		flags |= uint8_t(sys::news_generator_type::fake);
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "invention")) {
		flags |= uint8_t(sys::news_generator_type::invention);
	} else {
		err.accumulated_errors += "Unknown news type " + std::string(value) + " in file " + err.file_name + " line " + std::to_string(line) + "\n";
	}
}

void news_generate_article::size(association_type, std::string_view value, error_handler& err, int32_t line, news_context& context) {
	if(is_fixed_token_ci(value.data(), value.data() + value.length(), "small")) {
		flags |= sys::news_size_small;
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "medium")) {
		flags |= sys::news_size_medium;
	} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "huge")
		|| is_fixed_token_ci(value.data(), value.data() + value.length(), "large")) {
		flags |= sys::news_size_huge;
	} else {
		err.accumulated_errors += "Unknown news size " + std::string(value) + " in file " + err.file_name + " line " + std::to_string(line) + "\n";
	}
}

void news_generate_article::picture_case(news_picture_case value, error_handler& err, int32_t line, news_context& context) {
	if(last_picture_case >= sys::max_news_generator_cases) {
		err.accumulated_errors += "Too many picture cases (" + std::to_string(last_picture_case) + ") in file " + err.file_name + " line " + std::to_string(line) + "\n";
	} else {
		picture_cases[last_picture_case].trigger = value.trigger;
		picture_cases[last_picture_case].picture = value.picture_;
		++last_picture_case;
	}
}
void news_generate_article::title_case(news_title_case value, error_handler& err, int32_t line, news_context& context) {
	if(last_title_case >= sys::max_news_generator_cases) {
		err.accumulated_errors += "Too many title cases (" + std::to_string(last_title_case) + ") in file " + err.file_name + " line " + std::to_string(line) + "\n";
	} else {
		title_cases[last_title_case].trigger = value.trigger;
		title_cases[last_title_case].text = value.text;
		++last_title_case;
	}
}
void news_generate_article::description_case(news_desc_case value, error_handler& err, int32_t line, news_context& context) {
	if(last_desc_case >= sys::max_news_generator_cases) {
		err.accumulated_errors += "Too many description cases (" + std::to_string(last_desc_case) + ") in file " + err.file_name + " line " + std::to_string(line) + "\n";
	} else {
		desc_cases[last_desc_case].trigger = value.trigger;
		desc_cases[last_desc_case].text = value.text;
		++last_desc_case;
	}
}

void news_generate_article::finish(news_context& context) {
	auto id = context.outer_context.state.world.create_news_article_generator();
	context.outer_context.state.world.news_article_generator_set_picture_case(id, picture_cases);
	context.outer_context.state.world.news_article_generator_set_title_case(id, title_cases);
	context.outer_context.state.world.news_article_generator_set_desc_case(id, desc_cases);
}

void news_text_add::free_value(std::string_view value, error_handler& err, int32_t line, news_context& context) {
	text_ = text::find_or_add_key(context.outer_context.state, value);
}

dcon::trigger_key make_news_trigger(token_generator& gen, error_handler& err, news_context& context) {
	trigger_building_context t_context{ context.outer_context, trigger::slot_contents::empty, trigger::slot_contents::empty, trigger::slot_contents::empty };
	return make_trigger(gen, err, t_context);
}

dcon::trigger_key make_news_case_trigger(token_generator& gen, error_handler& err, news_context& context) {
	trigger_building_context t_context{ context.outer_context, trigger::slot_contents::nation, trigger::slot_contents::empty, trigger::slot_contents::empty };
	return make_trigger(gen, err, t_context);
}

}
