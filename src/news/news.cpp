#include "news.hpp"
#include "triggers.hpp"
#include "system_state.hpp"

namespace news {
void issue_newspaper(sys::state& state, dcon::nation_id reader) {
	/*
	news_scope currently_collected; //currently collected
	news_scope article_pool[8];
	news_scope small_articles[6];
	news_scope medium_articles[3];
	news_scope large_article[1];
	dcon::effect_key on_collect_effect[24];
	dcon::trigger_key on_collect_trigger[24];
	dcon::effect_key on_printing_effect[24];
	dcon::trigger_key on_printing_trigger[24];
	*/
	uint8_t small_article_count = 0;
	uint8_t medium_article_count = 0;
	uint8_t large_article_count = 0;
	for(auto gen : state.world.in_news_article_generator) {
		if((gen.get_flags() & sys::news_size_mask) == sys::news_size_small && small_article_count >= 6) {
			continue;
		} else if((gen.get_flags() & sys::news_size_mask) == sys::news_size_medium && medium_article_count >= 3) {
			continue;
		} else if((gen.get_flags() & sys::news_size_mask) == sys::news_size_huge && large_article_count >= 1) {
			continue;
		}
		auto title = gen.get_title_case()[0].text;
		for(uint32_t i = 0; i < sys::max_news_generator_cases; i++) {
			if(!gen.get_title_case()[i].text)
				break;
			auto k = gen.get_title_case()[i].trigger;
			if(!k || trigger::evaluate(state, k, trigger::to_generic(reader), -1, -1)) {
				title = gen.get_title_case()[i].text;
				break;
			}
		}
		auto desc = gen.get_desc_case()[0].text;
		for(uint32_t i = 0; i < sys::max_news_generator_cases; i++) {
			if(!gen.get_desc_case()[i].text)
				break;
			auto k = gen.get_desc_case()[i].trigger;
			if(!k || trigger::evaluate(state, k, trigger::to_generic(reader), -1, -1)) {
				desc = gen.get_desc_case()[i].text;
				break;
			}
		}
		auto picture = gen.get_picture_case()[0].picture;
		for(uint32_t i = 0; i < sys::max_news_generator_cases; i++) {
			if(!gen.get_picture_case()[i].picture)
				break;
			auto k = gen.get_picture_case()[i].trigger;
			if(!k || trigger::evaluate(state, k, trigger::to_generic(reader), -1, -1)) {
				picture = gen.get_picture_case()[i].picture;
				break;
			}
		}
		if((gen.get_flags() & sys::news_size_mask) == sys::news_size_small) {
			state.news_definitions.small_articles[small_article_count].title = title;
			state.news_definitions.small_articles[small_article_count].desc = desc;
			state.news_definitions.small_articles[small_article_count].picture = picture;
			++small_article_count;
		} else if((gen.get_flags() & sys::news_size_mask) == sys::news_size_medium) {
			state.news_definitions.medium_articles[medium_article_count].title = title;
			state.news_definitions.medium_articles[medium_article_count].desc = desc;
			state.news_definitions.medium_articles[medium_article_count].picture = picture;
			++medium_article_count;
		} else if((gen.get_flags() & sys::news_size_mask) == sys::news_size_huge) {
			state.news_definitions.large_articles[large_article_count].title = title;
			state.news_definitions.large_articles[large_article_count].desc = desc;
			state.news_definitions.large_articles[large_article_count].picture = picture;
			++large_article_count;
		}
	}
	state.news_definitions.last_print = state.current_date;
}
}
