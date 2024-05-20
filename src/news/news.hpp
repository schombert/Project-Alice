#pragma once
#include "dcon_generated.hpp"
#include "date_interface.hpp"

namespace news {

enum news_type : uint8_t {
	peace_offer_accept,
	game_event,
	province_change_controller,
	province_change_owner,
	construction_complete,
	research_complete,
	battle_over,
	rebel_break_country,
	new_party,
	war_declared,
	crisis_started,
	crisis_backer,
	crisis_side_joined,
	crisis_resolved,
	decision,
	goods_price_change,
	ai_afraid_of,
	ai_likes_very_much,
	fake,
};

struct news_scope {
	dcon::text_sequence_id strings[1][3];
	dcon::national_identity_id tags[1][3];
	int32_t values[1][3];
	sys::date dates[1][3];
};

struct global_news_state {
	news_scope currently_collected; //currently collected
	news_scope large_article;
	news_scope medium_articles[3];
	news_scope small_articles[6];
};

}
