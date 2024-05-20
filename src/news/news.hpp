#pragma once
#include "dcon_generated.hpp"
#include "date_interface.hpp"

namespace news {

struct news_scope {
	dcon::text_sequence_id strings[1][3];
	dcon::national_identity_id tags[1][3];
	int32_t values[1][3];
	sys::date dates[1][3];
	dcon::gfx_object_id picture;
	dcon::text_sequence_id title;
	dcon::text_sequence_id desc;
	sys::news_generator_type type;
};

struct global_news_state {
	news_scope currently_collected; //currently collected
	news_scope large_article;
	news_scope medium_articles[3];
	news_scope small_articles[6];
};

}
