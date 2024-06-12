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
		state.news_definitions.num_collected = 0;
	}
}

dcon::text_sequence_id generator_get_title(sys::state& state, dcon::news_article_generator_id gen, dcon::nation_id n) {
	auto const& list = state.world.news_article_generator_get_title_case(gen);
	for(uint32_t i = 0; i < sys::max_news_generator_cases; i++) {
		if(!list[i].text)
			break;
		auto k = list[i].trigger;
		if(!k || trigger::evaluate(state, k, trigger::to_generic(n), -1, -1)) {
			return list[i].text;
		}
	}
	return list[0].text;
}
dcon::text_sequence_id generator_get_desc(sys::state& state, dcon::news_article_generator_id gen, dcon::nation_id n) {
	auto const& list = state.world.news_article_generator_get_desc_case(gen);
	for(uint32_t i = 0; i < sys::max_news_generator_cases; i++) {
		if(!list[i].text)
			break;
		auto k = list[i].trigger;
		if(!k || trigger::evaluate(state, k, trigger::to_generic(n), -1, -1)) {
			return list[i].text;
		}
	}
	return list[0].text;
}
dcon::gfx_object_id generator_get_picture(sys::state& state, dcon::news_article_generator_id gen, dcon::nation_id n) {
	auto const& list = state.world.news_article_generator_get_picture_case(gen);
	for(uint32_t i = 0; i < sys::max_news_generator_cases; i++) {
		if(!list[i].picture)
			break;
		auto k = list[i].trigger;
		if(!k || trigger::evaluate(state, k, trigger::to_generic(n), -1, -1)) {
			return list[i].picture;
		}
	}
	return list[0].picture;
}

void issue_newspaper(sys::state& state, dcon::nation_id reader) {
	uint8_t small_article_count = 0;
	uint8_t medium_article_count = 0;
	uint8_t large_article_count = 0;
	for(uint32_t j = 0; j < state.news_definitions.num_collected; j++) {
		auto& ar = state.news_definitions.article_pool[j];
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
			auto title = generator_get_title(state, gen, reader);
			auto desc = generator_get_desc(state, gen, reader);
			auto picture = generator_get_picture(state, gen, reader);
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
	//generate fake news
	if(large_article_count == 0) {
		for(auto gen : state.world.in_news_article_generator) {
			if(large_article_count >= 1)
				break;
			auto title = generator_get_title(state, gen, reader);
			auto desc = generator_get_desc(state, gen, reader);
			auto picture = generator_get_picture(state, gen, reader);
			state.news_definitions.large_articles[large_article_count].title = title;
			state.news_definitions.large_articles[large_article_count].desc = desc;
			state.news_definitions.large_articles[large_article_count].picture = picture;
			++large_article_count;
		}
	}

	state.news_definitions.last_print = state.current_date;
}
}
