#pragma once
#include "dcon_generated.hpp"
#include "date_interface.hpp"

namespace sys {
struct state;
}

namespace news {

struct news_scope {
	dcon::text_sequence_id strings[4][4]; //4*4*4
	int32_t values[4][4]; //4*4*4
	dcon::text_sequence_id title; //4
	dcon::text_sequence_id desc; //4
	sys::date dates[4][4]; //4*4*2
	dcon::national_identity_id tags[4][4]; //4*4*2
	dcon::gfx_object_id picture; //2
	sys::news_generator_type type; //1
	uint8_t padding = 0;
};
/*
static_assert(sizeof(news_scope) ==
	sizeof(news_scope::strings)
	+ sizeof(news_scope::tags)
	+ sizeof(news_scope::values)
	+ sizeof(news_scope::dates)
	+ sizeof(news_scope::picture)
	+ sizeof(news_scope::title)
	+ sizeof(news_scope::desc)
	+ sizeof(news_scope::type)
	+ sizeof(news_scope::padding));
*/

struct global_news_state {
	news_scope currently_collected; //currently collected
	news_scope article_pool[5 + 2 + 1];
	news_scope small_articles[5];
	news_scope medium_articles[2];
	news_scope large_articles[1];
	uint32_t num_collected = 0;
	dcon::effect_key on_collect_effect[sys::max_news_generator_types];
	dcon::trigger_key on_collect_trigger[sys::max_news_generator_types];
	dcon::effect_key on_printing_effect[sys::max_news_generator_types];
	dcon::trigger_key on_printing_trigger[sys::max_news_generator_types];
	sys::date last_print;
};
// TODO: Make memcpyable

void collect_scope(sys::state& state, news_scope& scope);
void issue_newspaper(sys::state& state, dcon::nation_id reader);

}
