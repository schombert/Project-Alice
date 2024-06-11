#pragma once

namespace parsers {
struct news_context {
	scenario_building_context& outer_context;
	ankerl::unordered_dense::map<std::string, int32_t> map_of_news_pattern_names;
	news_context(scenario_building_context& outer_context) : outer_context(outer_context) { }
};

struct news_picture_case {
	dcon::trigger_key trigger;
	dcon::gfx_object_id picture_;
	void picture(association_type, std::string_view value, error_handler& err, int32_t line, news_context& context);
	void finish(news_context& context) { }
};
struct news_text_add {
	dcon::text_sequence_id text_;
	void free_value(std::string_view value, error_handler& err, int32_t line, news_context& context);
	void finish(news_context& context) { }
};
struct news_title_case {
	dcon::trigger_key trigger;
	dcon::text_sequence_id text;
	void text_add(association_type, news_text_add value, error_handler& err, int32_t line, news_context& context) {
		text = value.text_;
	}
	void finish(news_context& context) { }
};
struct news_desc_case {
	dcon::trigger_key trigger;
	dcon::text_sequence_id text;
	void text_add(association_type, news_text_add value, error_handler& err, int32_t line, news_context& context) {
		text = value.text_;
	}
	void finish(news_context& context) { }
};
struct news_generator_selector {
	void finish(news_context& context) { }
};
struct news_generate_article {
	uint8_t flags = 0;
	int32_t last_picture_case = 0;
	int32_t last_title_case = 0;
	int32_t last_desc_case = 0;
	std::array<sys::news_picture_case, sys::max_news_generator_cases> picture_cases;
	std::array<sys::news_text_case, sys::max_news_generator_cases> title_cases;
	std::array<sys::news_text_case, sys::max_news_generator_cases> desc_cases;
	void type(association_type, std::string_view value, error_handler& err, int32_t line, news_context& context);
	void size(association_type, std::string_view value, error_handler& err, int32_t line, news_context& context);
	void picture_case(association_type, news_picture_case value, error_handler& err, int32_t line, news_context& context);
	void title_case(association_type, news_title_case value, error_handler& err, int32_t line, news_context& context);
	void description_case(association_type, news_desc_case value, error_handler& err, int32_t line, news_context& context);
	void finish(news_context& context) { }
};
struct news_pattern {
	std::string name_;
	void name(association_type, std::string_view value, error_handler& err, int32_t line, news_context& context) {
		// TODO: Add news patterns to database
		context.map_of_news_pattern_names.insert_or_assign(std::string(value), int32_t(line));
	}
	void finish(news_context& context) { }
};
struct news_case {
	void finish(news_context& context) { }
};
struct news_priority {
	void finish(news_context& context) { }
};
struct news_on_printing {
	void finish(news_context& context) { }
};
struct news_on_collection {
	void finish(news_context& context) { }
};
struct news_pattern_instance {
	void finish(news_context& context) { }
};
struct news_article_types {
	int32_t peace_offer_accept = 0;
	int32_t game_event = 0;
	int32_t province_change_controller = 0;
	int32_t province_change_owner = 0;
	int32_t construction_complete = 0;
	int32_t research_complete = 0;
	int32_t battle_over = 0;
	int32_t rebel_break_country = 0;
	int32_t new_party = 0;
	int32_t war_declared = 0;
	int32_t crisis_started = 0;
	int32_t crisis_backer = 0;
	int32_t crisis_side_joined = 0;
	int32_t crisis_resolved = 0;
	int32_t decision = 0;
	int32_t goods_price_change = 0;
	int32_t ai_afraid_of = 0;
	int32_t ai_likes_very_much = 0;
	int32_t fake = 0;
	void finish(news_context& context) { }
};
struct news_style_article {
	void finish(news_context& context) { }
};
struct news_title_image {
	void finish(news_context& context) { }
};
struct news_style {
	void finish(news_context& context) { }
};
struct news_file {
	void any_group(std::string_view name, news_pattern_instance, error_handler& err, int32_t line, news_context& context) {
		if(auto it = context.map_of_news_pattern_names.find(std::string(name)); it != context.map_of_news_pattern_names.end()) {
			//do something
		} else {
			err.accumulated_errors += "Unknown news pattern " + std::string(name) + " in file " + err.file_name + " line " + std::to_string(line) + "\n";
		}
	}
	void finish(news_context& context) { }
};
} //parsers
