#include "news.hpp"
#include "triggers.hpp"
#include "system_state.hpp"

namespace news {

void collect_news_scope(sys::state& state, news_scope& scope) {
	state.news_definitions.currently_collected = scope; //do some wololollo
	state.news_definitions.article_pool[state.news_definitions.num_collected] = scope;
	++state.news_definitions.num_collected;
	if(state.news_definitions.num_collected >= std::extent_v<decltype(state.news_definitions.article_pool)>) {
		issue_newspaper(state, dcon::nation_id{ 1 }); //static
	}
}

void issue_newspaper(sys::state& state, dcon::nation_id reader) {
	uint8_t small_article_count = 0;
	uint8_t medium_article_count = 0;
	uint8_t large_article_count = 0;
	for(uint32_t i = 0; i < state.news_definitions.num_collected; i++) {
		auto& ar = state.news_definitions.article_pool[i];
		for(auto gen : state.world.in_news_article_generator) {
			if((gen.get_flags() & sys::news_type_mask) != uint8_t(ar.type))
				continue;
			if((gen.get_flags() & sys::news_size_mask) == sys::news_size_small && small_article_count >= 5) {
				continue;
			} else if((gen.get_flags() & sys::news_size_mask) == sys::news_size_medium && medium_article_count >= 2) {
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

				std::memcpy(state.news_definitions.small_articles[small_article_count].tags, ar.tags, sizeof(ar.tags));
				std::memcpy(state.news_definitions.small_articles[small_article_count].values, ar.values, sizeof(ar.values));
				std::memcpy(state.news_definitions.small_articles[small_article_count].strings, ar.strings, sizeof(ar.strings));
				std::memcpy(state.news_definitions.small_articles[small_article_count].dates, ar.dates, sizeof(ar.dates));
				++small_article_count;
			} else if((gen.get_flags() & sys::news_size_mask) == sys::news_size_medium) {
				state.news_definitions.medium_articles[medium_article_count].title = title;
				state.news_definitions.medium_articles[medium_article_count].desc = desc;
				state.news_definitions.medium_articles[medium_article_count].picture = picture;

				std::memcpy(state.news_definitions.medium_articles[medium_article_count].tags, ar.tags, sizeof(ar.tags));
				std::memcpy(state.news_definitions.medium_articles[medium_article_count].values, ar.values, sizeof(ar.values));
				std::memcpy(state.news_definitions.medium_articles[medium_article_count].strings, ar.strings, sizeof(ar.strings));
				std::memcpy(state.news_definitions.medium_articles[medium_article_count].dates, ar.dates, sizeof(ar.dates));
				++medium_article_count;
			} else if((gen.get_flags() & sys::news_size_mask) == sys::news_size_huge) {
				state.news_definitions.large_articles[large_article_count].title = title;
				state.news_definitions.large_articles[large_article_count].desc = desc;
				state.news_definitions.large_articles[large_article_count].picture = picture;

				std::memcpy(state.news_definitions.large_articles[large_article_count].tags, ar.tags, sizeof(ar.tags));
				std::memcpy(state.news_definitions.large_articles[large_article_count].values, ar.values, sizeof(ar.values));
				std::memcpy(state.news_definitions.large_articles[large_article_count].strings, ar.strings, sizeof(ar.strings));
				std::memcpy(state.news_definitions.large_articles[large_article_count].dates, ar.dates, sizeof(ar.dates));
				++large_article_count;
			}
		}
	}
	state.news_definitions.last_print = state.current_date;
}
}
