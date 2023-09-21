#include <string_view>
#include "dcon_generated.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include "script_constants.hpp"

namespace ui {

namespace effect_tooltip {

inline constexpr int32_t indentation_amount = 15;

#define EFFECT_DISPLAY_PARAMS                                                                                                    \
	sys::state &ws, uint16_t const *tval, text::layout_base &layout, int32_t primary_slot, int32_t this_slot, int32_t from_slot,   \
			uint32_t r_lo, uint32_t r_hi, int32_t indentation

uint32_t internal_make_effect_description(EFFECT_DISPLAY_PARAMS);


void add_adj_this_to_map(sys::state& ws, text::substitution_map& map, dcon::nation_id n) {
	static std::string this_nation = text::produce_simple_string(ws, "this_nation");
	if(n)
		text::add_to_substitution_map(map, text::variable_type::adj, ws.world.nation_get_adjective(n));
	else
		text::add_to_substitution_map(map, text::variable_type::adj, std::string_view{ this_nation });
}
void add_adj_this_to_map(sys::state& ws, text::substitution_map& map, dcon::province_id p) {
	static std::string this_nation = text::produce_simple_string(ws, "this_nation");
	if(auto n = ws.world.province_get_nation_from_province_ownership(p); n)
		text::add_to_substitution_map(map, text::variable_type::adj, ws.world.nation_get_adjective(n));
	else
		text::add_to_substitution_map(map, text::variable_type::adj, std::string_view{ this_nation });
}
void add_adj_this_to_map(sys::state& ws, text::substitution_map& map, dcon::state_instance_id p) {
	static std::string this_nation = text::produce_simple_string(ws, "this_nation");
	if(auto n = ws.world.state_instance_get_nation_from_state_ownership(p); n)
		text::add_to_substitution_map(map, text::variable_type::adj, ws.world.nation_get_adjective(n));
	else
		text::add_to_substitution_map(map, text::variable_type::adj, std::string_view{ this_nation });
}
void add_adj_this_to_map(sys::state& ws, text::substitution_map& map, dcon::pop_id p) {
	static std::string this_nation = text::produce_simple_string(ws, "this_nation");
	if(auto n = nations::owner_of_pop(ws, p); n)
		text::add_to_substitution_map(map, text::variable_type::adj, ws.world.nation_get_adjective(n));
	else
		text::add_to_substitution_map(map, text::variable_type::adj, std::string_view{ this_nation });
}
void add_adj_from_to_map(sys::state& ws, text::substitution_map& map, dcon::rebel_faction_id p) {
	static std::string this_nation = text::produce_simple_string(ws, "from_nation");
	auto fp = fatten(ws.world, p);
	if(auto n = fp.get_defection_target().get_nation_from_identity_holder(); n)
		text::add_to_substitution_map(map, text::variable_type::adj, ws.world.nation_get_adjective(n));
	else
		text::add_to_substitution_map(map, text::variable_type::adj, std::string_view{ this_nation });
}
void add_adj_from_to_map(sys::state& ws, text::substitution_map& map, dcon::nation_id n) {
	static std::string this_nation = text::produce_simple_string(ws, "from_nation");
	if(n)
		text::add_to_substitution_map(map, text::variable_type::adj, ws.world.nation_get_adjective(n));
	else
		text::add_to_substitution_map(map, text::variable_type::adj, std::string_view{ this_nation });
}
void add_adj_from_to_map(sys::state& ws, text::substitution_map& map, dcon::province_id p) {
	static std::string this_nation = text::produce_simple_string(ws, "from_nation");
	if(auto n = ws.world.province_get_nation_from_province_ownership(p); n)
		text::add_to_substitution_map(map, text::variable_type::adj, ws.world.nation_get_adjective(n));
	else
		text::add_to_substitution_map(map, text::variable_type::adj, std::string_view{ this_nation });
}

inline uint32_t display_subeffects(EFFECT_DISPLAY_PARAMS) {

	auto const source_size = 1 + effect::get_effect_scope_payload_size(tval);
	auto sub_units_start = tval + 2 + effect::effect_scope_data_payload(tval[0]);

	uint32_t i = 0;
	while(sub_units_start < tval + source_size) {
		i += internal_make_effect_description(ws, sub_units_start, layout, primary_slot, this_slot, from_slot, r_hi, r_lo + i,
				indentation);
		sub_units_start += 1 + effect::get_generic_effect_payload_size(sub_units_start);
	}
	return i;
}

void display_value(int64_t value, bool positive_is_green, sys::state& ws, text::layout_base& layout, text::layout_box& box) {

	if(value >= 0)
		text::add_to_layout_box(ws, layout, box, "+", positive_is_green ? text::text_color::green : text::text_color::red,
				std::monostate{});

	if(positive_is_green)
		text::add_to_layout_box(ws, layout, box, value, value >= 0 ? text::text_color::green : text::text_color::red);
	else
		text::add_to_layout_box(ws, layout, box, value, value >= 0 ? text::text_color::red : text::text_color::green);

	text::add_space_to_layout_box(ws, layout, box);
}
void display_value(text::fp_currency value, bool positive_is_green, sys::state& ws, text::layout_base& layout,
		text::layout_box& box) {

	if(value.value >= 0)
		text::add_to_layout_box(ws, layout, box, "+", positive_is_green ? text::text_color::green : text::text_color::red,
				std::monostate{});

	if(positive_is_green)
		text::add_to_layout_box(ws, layout, box, value, value.value >= 0 ? text::text_color::green : text::text_color::red);
	else
		text::add_to_layout_box(ws, layout, box, value, value.value >= 0 ? text::text_color::red : text::text_color::green);

	text::add_space_to_layout_box(ws, layout, box);
}
void display_value(text::fp_percentage value, bool positive_is_green, sys::state& ws, text::layout_base& layout,
		text::layout_box& box) {

	if(value.value >= 0)
		text::add_to_layout_box(ws, layout, box, "+", positive_is_green ? text::text_color::green : text::text_color::red,
				std::monostate{});

	if(positive_is_green)
		text::add_to_layout_box(ws, layout, box, value, value.value >= 0 ? text::text_color::green : text::text_color::red);
	else
		text::add_to_layout_box(ws, layout, box, value, value.value >= 0 ? text::text_color::red : text::text_color::green);

	text::add_space_to_layout_box(ws, layout, box);
}
void display_value(text::fp_one_place value, bool positive_is_green, sys::state& ws, text::layout_base& layout,
		text::layout_box& box) {

	if(value.value >= 0)
		text::add_to_layout_box(ws, layout, box, "+", positive_is_green ? text::text_color::green : text::text_color::red,
				std::monostate{});

	if(positive_is_green)
		text::add_to_layout_box(ws, layout, box, value, value.value >= 0 ? text::text_color::green : text::text_color::red);
	else
		text::add_to_layout_box(ws, layout, box, value, value.value >= 0 ? text::text_color::red : text::text_color::green);

	text::add_space_to_layout_box(ws, layout, box);
}
void display_value(text::fp_two_places value, bool positive_is_green, sys::state& ws, text::layout_base& layout,
		text::layout_box& box) {

	if(value.value >= 0)
		text::add_to_layout_box(ws, layout, box, "+", positive_is_green ? text::text_color::green : text::text_color::red,
				std::monostate{});

	if(positive_is_green)
		text::add_to_layout_box(ws, layout, box, value, value.value >= 0 ? text::text_color::green : text::text_color::red);
	else
		text::add_to_layout_box(ws, layout, box, value, value.value >= 0 ? text::text_color::red : text::text_color::green);

	text::add_space_to_layout_box(ws, layout, box);
}
void display_value(text::fp_three_places value, bool positive_is_green, sys::state& ws, text::layout_base& layout,
		text::layout_box& box) {

	if(value.value >= 0)
		text::add_to_layout_box(ws, layout, box, "+", positive_is_green ? text::text_color::green : text::text_color::red,
				std::monostate{});

	if(positive_is_green)
		text::add_to_layout_box(ws, layout, box, value, value.value >= 0 ? text::text_color::green : text::text_color::red);
	else
		text::add_to_layout_box(ws, layout, box, value, value.value >= 0 ? text::text_color::red : text::text_color::green);

	text::add_space_to_layout_box(ws, layout, box);
}

void tag_type_this_nation_effect(int32_t this_slot, sys::state& ws, text::layout_base& layout, text::layout_box& box) {
	text::add_to_layout_box(ws, layout, box,
			this_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(this_slot)))
											: text::produce_simple_string(ws, "this_nation"));
}
void tag_type_this_state_effect(int32_t this_slot, sys::state& ws, text::layout_base& layout, text::layout_box& box) {
	text::add_to_layout_box(ws, layout, box,
			this_slot != -1
					? text::produce_simple_string(ws,
								ws.world.nation_get_name(ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot))))
					: text::produce_simple_string(ws, "this_nation"));
}
void tag_type_this_province_effect(int32_t this_slot, sys::state& ws, text::layout_base& layout, text::layout_box& box) {
	text::add_to_layout_box(ws, layout, box,
			this_slot != -1
					? text::produce_simple_string(ws,
								ws.world.nation_get_name(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot))))
					: text::produce_simple_string(ws, "this_nation"));
}
void tag_type_this_pop_effect(int32_t this_slot, sys::state& ws, text::layout_base& layout, text::layout_box& box) {
	text::add_to_layout_box(ws, layout, box,
			this_slot != -1
					? text::produce_simple_string(ws, ws.world.nation_get_name(nations::owner_of_pop(ws, trigger::to_pop(this_slot))))
					: text::produce_simple_string(ws, "this_nation"));
}
void tag_type_from_nation_effect(int32_t this_slot, sys::state& ws, text::layout_base& layout, text::layout_box& box) {
	text::add_to_layout_box(ws, layout, box,
			this_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(this_slot)))
											: text::produce_simple_string(ws, "from_nation"));
}
void tag_type_from_province_effect(int32_t this_slot, sys::state& ws, text::layout_base& layout, text::layout_box& box) {
	text::add_to_layout_box(ws, layout, box,
			this_slot != -1
					? text::produce_simple_string(ws,
								ws.world.nation_get_name(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot))))
					: text::produce_simple_string(ws, "from_nation"));
}

uint32_t es_generic_scope(EFFECT_DISPLAY_PARAMS) {
	return display_subeffects(ws, tval, layout, primary_slot, this_slot, from_slot, r_lo, r_hi, indentation);
}

inline auto random_or_every(uint16_t tval) {
	return (tval & effect::is_random_scope) != 0 ? "random" : "every";
}

void show_limit(sys::state& ws, uint16_t const* tval, text::layout_base& layout, int32_t this_slot, int32_t from_slot,
		int32_t indentation) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;

		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "where"));
		text::close_layout_box(layout, box);

		trigger_tooltip::make_trigger_description(ws, layout, ws.trigger_data.data() + ws.trigger_data_indices[limit.index() + 1], -1,
				this_slot, from_slot, indentation + 2 * indentation_amount, false);
	}
}

uint32_t es_x_neighbor_province_scope(EFFECT_DISPLAY_PARAMS) {
	if((tval[0] & effect::is_random_scope) != 0) {
		if(primary_slot != -1) {
			auto neighbor_range = ws.world.province_get_province_adjacency(trigger::to_prov(primary_slot));

			std::vector<dcon::province_id> rlist;

			if((tval[0] & effect::scope_has_limit) != 0) {
				auto limit = trigger::payload(tval[2]).tr_id;
				for(auto p : neighbor_range) {
					auto other = p.get_connected_provinces(0) == trigger::to_prov(primary_slot) ? p.get_connected_provinces(1)
																																											: p.get_connected_provinces(0);

					if(other.get_nation_from_province_ownership() &&
							trigger::evaluate(ws, limit, trigger::to_generic(other.id), this_slot, from_slot)) {
						rlist.push_back(other.id);
					}
				}
			} else {
				for(auto p : neighbor_range) {
					auto other = p.get_connected_provinces(0) == trigger::to_prov(primary_slot) ? p.get_connected_provinces(1)
																																											: p.get_connected_provinces(0);

					if(other.get_nation_from_province_ownership()) {
						rlist.push_back(other.id);
					}
				}
			}

			if(rlist.size() != 0) {
				auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();

				auto box = text::open_layout_box(layout, indentation);
				text::add_to_layout_box(ws, layout, box, rlist[r]);
				text::close_layout_box(layout, box);

				return 1 + display_subeffects(ws, tval, layout, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1,
											 indentation + indentation_amount);
			}
			return 0;
		}
	}

	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "neighboring_province"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_x_neighbor_country_scope(EFFECT_DISPLAY_PARAMS) {
	if((tval[0] & effect::is_random_scope) != 0) {
		if(primary_slot != -1) {
			auto neighbor_range = ws.world.nation_get_nation_adjacency(trigger::to_nation(primary_slot));
			std::vector<dcon::nation_id> rlist;

			if((tval[0] & effect::scope_has_limit) != 0) {
				auto limit = trigger::payload(tval[2]).tr_id;
				for(auto p : neighbor_range) {
					auto other = p.get_connected_nations(0) == trigger::to_nation(primary_slot) ? p.get_connected_nations(1)
																																											: p.get_connected_nations(0);

					if(trigger::evaluate(ws, limit, trigger::to_generic(other.id), this_slot, from_slot)) {
						rlist.push_back(other.id);
					}
				}
			} else {
				for(auto p : neighbor_range) {
					auto other = p.get_connected_nations(0) == trigger::to_nation(primary_slot) ? p.get_connected_nations(1)
																																											: p.get_connected_nations(0);

					rlist.push_back(other.id);
				}
			}

			if(rlist.size() != 0) {
				auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();

				auto box = text::open_layout_box(layout, indentation);
				text::add_to_layout_box(ws, layout, box, rlist[r]);
				text::close_layout_box(layout, box);

				return 1 + display_subeffects(ws, tval, layout, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1,
											 indentation + indentation_amount);
			}
			return 0;
		}
	}

	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "neighboring_nation"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_x_country_scope_nation(EFFECT_DISPLAY_PARAMS) {
	if((tval[0] & effect::is_random_scope) != 0) {
		std::vector<dcon::nation_id> rlist;

		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto n : ws.world.in_nation) {
				auto owned = n.get_province_ownership();
				if(trigger::evaluate(ws, limit, trigger::to_generic(n.id), this_slot, from_slot))
					rlist.push_back(n.id);
			}
		} else {
			for(auto n : ws.world.in_nation) {
				rlist.push_back(n.id);
			}
		}

		if(rlist.size() != 0) {
			auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();

			auto box = text::open_layout_box(layout, indentation);
			text::add_to_layout_box(ws, layout, box, rlist[r]);
			text::close_layout_box(layout, box);

			return 1 + display_subeffects(ws, tval, layout, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1,
										 indentation + indentation_amount);
		}
		return 0;
	}

	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "nation"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_x_country_scope(EFFECT_DISPLAY_PARAMS) {
	return es_x_country_scope_nation(ws, tval, layout, primary_slot, this_slot, from_slot, r_lo, r_hi, indentation);
}
uint32_t es_x_empty_neighbor_province_scope(EFFECT_DISPLAY_PARAMS) {
	if((tval[0] & effect::is_random_scope) != 0) {
		if(primary_slot != -1) {
			auto neighbor_range = ws.world.province_get_province_adjacency(trigger::to_prov(primary_slot));
			std::vector<dcon::province_id> rlist;

			if((tval[0] & effect::scope_has_limit) != 0) {
				auto limit = trigger::payload(tval[2]).tr_id;
				for(auto p : neighbor_range) {
					auto other = p.get_connected_provinces(0) == trigger::to_prov(primary_slot) ? p.get_connected_provinces(1)
																																											: p.get_connected_provinces(0);

					if(!other.get_nation_from_province_ownership() &&
							trigger::evaluate(ws, limit, trigger::to_generic(other.id), this_slot, from_slot)) {
						rlist.push_back(other.id);
					}
				}
			} else {
				for(auto p : neighbor_range) {
					auto other = p.get_connected_provinces(0) == trigger::to_prov(primary_slot) ? p.get_connected_provinces(1)
																																											: p.get_connected_provinces(0);

					if(!other.get_nation_from_province_ownership()) {
						rlist.push_back(other.id);
					}
				}
			}

			if(rlist.size() != 0) {
				auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();

				auto box = text::open_layout_box(layout, indentation);
				text::add_to_layout_box(ws, layout, box, rlist[r]);
				text::close_layout_box(layout, box);

				return 1 + display_subeffects(ws, tval, layout, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1,
											 indentation + indentation_amount);
			}
			return 0;
		}
	}

	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "empty_neighboring_province"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_x_greater_power_scope(EFFECT_DISPLAY_PARAMS) {
	if((tval[0] & effect::is_random_scope) != 0) {
		std::vector<dcon::nation_id> rlist;

		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto& n : ws.great_nations) {
				if(trigger::evaluate(ws, limit, trigger::to_generic(n.nation), this_slot, from_slot)) {
					rlist.push_back(n.nation);
				}
			}
		} else {
			for(auto& n : ws.great_nations) {
				rlist.push_back(n.nation);
			}
		}

		if(rlist.size() != 0) {
			auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();

			auto box = text::open_layout_box(layout, indentation);
			text::add_to_layout_box(ws, layout, box, rlist[r]);
			text::close_layout_box(layout, box);

			return 1 + display_subeffects(ws, tval, layout, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1,
										 indentation + indentation_amount);
		}
		return 0;
	}

	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "great_power"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_poor_strata_scope_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "poor_strata_pop"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_poor_strata_scope_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "poor_strata_pop"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_poor_strata_scope_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "poor_strata_pop"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_middle_strata_scope_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "middle_strata_pop"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_middle_strata_scope_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "middle_strata_pop"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_middle_strata_scope_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "middle_strata_pop"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_rich_strata_scope_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "rich_strata_pop"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_rich_strata_scope_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "rich_strata_pop"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_rich_strata_scope_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "rich_strata_pop"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_x_pop_scope_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pop"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_x_pop_scope_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pop"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_x_pop_scope_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pop"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_x_owned_scope_nation(EFFECT_DISPLAY_PARAMS) {
	if((tval[0] & effect::is_random_scope) != 0) {
		if(primary_slot != -1) {
			std::vector<dcon::province_id> rlist;

			if((tval[0] & effect::scope_has_limit) != 0) {
				auto limit = trigger::payload(tval[2]).tr_id;
				for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
					if(trigger::evaluate(ws, limit, trigger::to_generic(p.get_province().id), this_slot, from_slot)) {
						rlist.push_back(p.get_province().id);
					}
				}
			} else {
				for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
					rlist.push_back(p.get_province().id);
				}
			}

			if(rlist.size() != 0) {
				auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();

				auto box = text::open_layout_box(layout, indentation);
				text::add_to_layout_box(ws, layout, box, rlist[r]);
				text::close_layout_box(layout, box);

				return 1 + display_subeffects(ws, tval, layout, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1,
											 indentation + indentation_amount);
			}
			return 0;
		}
	}

	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "owned_province"));
		text::add_space_to_layout_box(ws, layout, box);
		if(primary_slot != -1)
			text::add_to_layout_box(ws, layout, box, trigger::to_nation(primary_slot));
		else
			text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_x_owned_scope_state(EFFECT_DISPLAY_PARAMS) {
	if((tval[0] & effect::is_random_scope) != 0) {
		if(primary_slot != -1) {
			std::vector<dcon::province_id> rlist;

			if((tval[0] & effect::scope_has_limit) != 0) {
				auto limit = trigger::payload(tval[2]).tr_id;
				auto d = ws.world.state_instance_get_definition(trigger::to_state(primary_slot));
				auto o = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
				for(auto p : ws.world.state_definition_get_abstract_state_membership(d)) {
					if(p.get_province().get_nation_from_province_ownership() == o) {
						if(trigger::evaluate(ws, limit, trigger::to_generic(p.get_province().id), this_slot, from_slot)) {
							rlist.push_back(p.get_province().id);
						}
					}
				}
			} else {
				auto d = ws.world.state_instance_get_definition(trigger::to_state(primary_slot));
				auto o = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
				for(auto p : ws.world.state_definition_get_abstract_state_membership(d)) {
					if(p.get_province().get_nation_from_province_ownership() == o) {
						rlist.push_back(p.get_province().id);
					}
				}
			}

			if(rlist.size() != 0) {
				auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();

				auto box = text::open_layout_box(layout, indentation);
				text::add_to_layout_box(ws, layout, box, rlist[r]);
				text::close_layout_box(layout, box);

				return 1 + display_subeffects(ws, tval, layout, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1,
											 indentation + indentation_amount);
			}
			return 0;
		}
	}

	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "owned_province"));
		text::add_space_to_layout_box(ws, layout, box);
		if(primary_slot != -1)
			text::add_to_layout_box(ws, layout, box, trigger::to_state(primary_slot));
		else
			text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "singular_state"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_x_core_scope(EFFECT_DISPLAY_PARAMS) {
	if((tval[0] & effect::is_random_scope) != 0) {
		if(primary_slot != -1) {
			auto tag = ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(primary_slot));
			auto cores_range = ws.world.national_identity_get_core(tag);

			std::vector<dcon::province_id> rlist;

			if((tval[0] & effect::scope_has_limit) != 0) {
				auto limit = trigger::payload(tval[2]).tr_id;
				for(auto p : cores_range) {
					if(trigger::evaluate(ws, limit, trigger::to_generic(p.get_province().id), this_slot, from_slot))
						rlist.push_back(p.get_province().id);
				}
			} else {
				for(auto p : cores_range) {
					rlist.push_back(p.get_province().id);
				}
			}

			if(rlist.size() != 0) {
				auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();

				auto box = text::open_layout_box(layout, indentation);
				text::add_to_layout_box(ws, layout, box, rlist[r]);
				text::close_layout_box(layout, box);

				return 1 + display_subeffects(ws, tval, layout, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1,
											 indentation + indentation_amount);
			}
			return 0;
		}
	}

	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "core_of"));
		text::add_space_to_layout_box(ws, layout, box);
		if(primary_slot != -1)
			text::add_to_layout_box(ws, layout, box, trigger::to_nation(primary_slot));
		else
			text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo,
																																	r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0),
																																	indentation + indentation_amount);
}
uint32_t es_x_substate_scope(EFFECT_DISPLAY_PARAMS) {
	if((tval[0] & effect::is_random_scope) != 0) {
		std::vector<dcon::nation_id> rlist;

		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto si : ws.world.nation_get_overlord_as_ruler(trigger::to_nation(primary_slot))) {
				if(si.get_subject().get_is_substate() && trigger::evaluate(ws, limit, trigger::to_generic(si.get_subject().id), this_slot, from_slot))
					rlist.push_back(si.get_subject().id);
			}
		} else {
			for(auto si : ws.world.nation_get_overlord_as_ruler(trigger::to_nation(primary_slot))) {
				if(si.get_subject().get_is_substate())
					rlist.push_back(si.get_subject().id);
			}
		}
		if(rlist.size() != 0) {
			auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();

			auto box = text::open_layout_box(layout, indentation);
			text::add_to_layout_box(ws, layout, box, rlist[r]);
			text::close_layout_box(layout, box);

			return 1 + display_subeffects(ws, tval, layout, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1, indentation + indentation_amount);
		}
		return 0;
	}

	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "substate_of"));
		text::add_space_to_layout_box(ws, layout, box);
		if(primary_slot != -1)
			text::add_to_layout_box(ws, layout, box, trigger::to_nation(primary_slot));
		else
			text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo, r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0), indentation + indentation_amount);

}
uint32_t es_x_state_scope(EFFECT_DISPLAY_PARAMS) {
	if((tval[0] & effect::is_random_scope) != 0) {
		if(primary_slot != -1) {
			std::vector<dcon::state_instance_id> rlist;

			if((tval[0] & effect::scope_has_limit) != 0) {
				auto limit = trigger::payload(tval[2]).tr_id;
				for(auto si : ws.world.nation_get_state_ownership(trigger::to_nation(primary_slot))) {
					if(trigger::evaluate(ws, limit, trigger::to_generic(si.get_state().id), this_slot, from_slot))
						rlist.push_back(si.get_state().id);
				}
			} else {
				for(auto si : ws.world.nation_get_state_ownership(trigger::to_nation(primary_slot))) {
					rlist.push_back(si.get_state().id);
				}
			}

			if(rlist.size() != 0) {
				auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();

				auto box = text::open_layout_box(layout, indentation);
				text::add_to_layout_box(ws, layout, box, rlist[r]);
				text::close_layout_box(layout, box);

				return 1 + display_subeffects(ws, tval, layout, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1,
											 indentation + indentation_amount);
			}
			return 0;
		}
	}

	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, random_or_every(tval[0])));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "state_of"));
		text::add_space_to_layout_box(ws, layout, box);
		if(primary_slot != -1)
			text::add_to_layout_box(ws, layout, box, trigger::to_nation(primary_slot));
		else
			text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}
	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0) + display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_lo, r_hi + ((tval[0] & effect::is_random_scope) != 0 ? 1 : 0), indentation + indentation_amount);
}
uint32_t es_random_list_scope(EFFECT_DISPLAY_PARAMS) {

	auto const source_size = 1 + effect::get_effect_scope_payload_size(tval);
	auto chances_total = tval[2];

	auto sub_units_start = tval + 3; // [code] + [payload size] + [chances total] + [first sub effect chance]

	auto r = int32_t(rng::get_random(ws, r_hi, r_lo) % chances_total);
	uint32_t rval = 0;
	bool found_res = false;

	while(sub_units_start < tval + source_size) {
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::fp_percentage{float(*sub_units_start) / float(chances_total)});
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "chance_of"));
		text::close_layout_box(layout, box);

		r -= *sub_units_start;
		auto tresult = 1 + internal_make_effect_description(ws, sub_units_start + 1, layout, primary_slot, this_slot, from_slot, r_hi, r_lo + 1,
				indentation + indentation_amount);
		if(r < 0 && !found_res) {
			found_res = true;
			rval = tresult;
		}
		sub_units_start += 2 + effect::get_generic_effect_payload_size(sub_units_start + 1); // each member preceded by uint16_t
	}
	return rval;
}
uint32_t es_random_scope(EFFECT_DISPLAY_PARAMS) {
	auto chance = tval[2];

	auto box = text::open_layout_box(layout, indentation);
	text::add_to_layout_box(ws, layout, box, text::fp_percentage{float(chance) / float(100)});
	text::add_space_to_layout_box(ws, layout, box);
	text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "chance_of"));
	text::close_layout_box(layout, box);

	return 1 + display_subeffects(ws, tval, layout, primary_slot, this_slot, from_slot, r_hi, r_lo + 1,
								 indentation + indentation_amount);
}
uint32_t es_owner_scope_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "owner_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::get_dynamic_state_name(ws, trigger::to_state(primary_slot))
													 : text::produce_simple_string(ws, "singular_state"));
		text::close_layout_box(layout, box);
	}

	return display_subeffects(ws, tval, layout,
			primary_slot != -1
					? trigger::to_generic(ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot)))
					: -1,
			this_slot, from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_owner_scope_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "owner_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.province_get_name(trigger::to_prov(primary_slot)))
													 : text::produce_simple_string(ws, "singular_province"));
		text::close_layout_box(layout, box);
	}

	return display_subeffects(ws, tval, layout,
			primary_slot != -1
					? trigger::to_generic(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)))
					: -1,
			this_slot, from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_controller_scope(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "controller_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.province_get_name(trigger::to_prov(primary_slot)))
													 : text::produce_simple_string(ws, "singular_province"));
		text::close_layout_box(layout, box);
	}

	return display_subeffects(ws, tval, layout,
			primary_slot != -1 ? trigger::to_generic(ws.world.province_get_nation_from_province_control(trigger::to_prov(primary_slot)))
												 : -1,
			this_slot, from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_location_scope(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "location_of_pop"));
		text::close_layout_box(layout, box);
	}

	return display_subeffects(ws, tval, layout,
			primary_slot != -1 ? trigger::to_generic(ws.world.pop_get_province_from_pop_location(trigger::to_pop(primary_slot))) : -1,
			this_slot, from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_country_scope_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "nation_of_pop"));
		text::close_layout_box(layout, box);
	}

	return display_subeffects(ws, tval, layout,
			primary_slot != -1 ? trigger::to_generic(nations::owner_of_pop(ws, trigger::to_pop(primary_slot))) : -1, this_slot,
			from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_country_scope_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "owner_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::get_dynamic_state_name(ws, trigger::to_state(primary_slot))
													 : text::produce_simple_string(ws, "singular_state"));
		text::close_layout_box(layout, box);
	}

	return display_subeffects(ws, tval, layout,
			primary_slot != -1
					? trigger::to_generic(ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot)))
					: -1,
			this_slot, from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_capital_scope(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "capital_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(primary_slot)))
													 : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	return display_subeffects(ws, tval, layout,
			primary_slot != -1 ? trigger::to_generic(ws.world.nation_get_capital(trigger::to_nation(primary_slot))) : -1, this_slot,
			from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_capital_scope_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "capital_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot))))
													 : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	return display_subeffects(ws, tval, layout,
			primary_slot != -1 ? trigger::to_generic(ws.world.nation_get_capital(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)))) : -1, this_slot,
			from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_this_scope_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box,
				this_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(this_slot)))
												: text::produce_simple_string(ws, "this_nation"));
		text::close_layout_box(layout, box);
	}

	return display_subeffects(ws, tval, layout, this_slot, this_slot, from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_this_scope_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box,
				this_slot != -1 ? text::get_dynamic_state_name(ws, trigger::to_state(this_slot))
												: text::produce_simple_string(ws, "this_state"));
		text::close_layout_box(layout, box);
	}
	return display_subeffects(ws, tval, layout, this_slot, this_slot, from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_this_scope_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box,
				this_slot != -1 ? text::produce_simple_string(ws, ws.world.province_get_name(trigger::to_prov(this_slot)))
												: text::produce_simple_string(ws, "this_province"));
		text::close_layout_box(layout, box);
	}
	return display_subeffects(ws, tval, layout, this_slot, this_slot, from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_this_scope_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "this_pop"));
		text::close_layout_box(layout, box);
	}
	return display_subeffects(ws, tval, layout, this_slot, this_slot, from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_from_scope_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box,
				from_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(from_slot)))
												: text::produce_simple_string(ws, "from_nation"));
		text::close_layout_box(layout, box);
	}
	return display_subeffects(ws, tval, layout, from_slot, this_slot, from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_from_scope_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box,
				from_slot != -1 ? text::get_dynamic_state_name(ws, trigger::to_state(from_slot))
												: text::produce_simple_string(ws, "from_state"));
		text::close_layout_box(layout, box);
	}
	return display_subeffects(ws, tval, layout, from_slot, this_slot, from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_from_scope_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box,
				from_slot != -1 ? text::produce_simple_string(ws, ws.world.province_get_name(trigger::to_prov(from_slot)))
												: text::produce_simple_string(ws, "from_province"));
		text::close_layout_box(layout, box);
	}
	return display_subeffects(ws, tval, layout, from_slot, this_slot, from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_from_scope_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "from_pop"));
		text::close_layout_box(layout, box);
	}
	return display_subeffects(ws, tval, layout, from_slot, this_slot, from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_sea_zone_scope(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "adjacent_sea"));
		text::close_layout_box(layout, box);
	}

	auto sea_zone = primary_slot != -1 ? [&ws](int32_t p_slot) {
		auto pid = fatten(ws.world, trigger::to_prov(p_slot));
		for(auto adj : pid.get_province_adjacency()) {
			if(adj.get_connected_provinces(0).id.index() >= ws.province_definitions.first_sea_province.index()) {
				return adj.get_connected_provinces(0).id;
			} else if(adj.get_connected_provinces(1).id.index() >= ws.province_definitions.first_sea_province.index()) {
				return adj.get_connected_provinces(1).id;
			}
		}
		return dcon::province_id{};
	}(primary_slot)
									   : dcon::province_id{};

	return display_subeffects(ws, tval, layout, sea_zone ? trigger::to_generic(sea_zone) : -1, this_slot, from_slot, r_hi, r_lo,
			indentation + indentation_amount);
}
uint32_t es_cultural_union_scope(EFFECT_DISPLAY_PARAMS) {
	auto prim_culture =
			primary_slot != -1 ? ws.world.nation_get_primary_culture(trigger::to_nation(primary_slot)) : dcon::culture_id{};
	auto cg = ws.world.culture_get_group_from_culture_group_membership(prim_culture);

	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "cultural_union_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				cg ? text::produce_simple_string(ws, ws.world.culture_group_get_name(cg))
					 : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	auto union_tag = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	auto group_holder = ws.world.national_identity_get_nation_from_identity_holder(union_tag);

	return display_subeffects(ws, tval, layout, trigger::to_generic(group_holder), this_slot, from_slot, r_hi, r_lo,
			indentation + indentation_amount);
}
uint32_t es_overlord_scope(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "overlord_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(primary_slot)))
													 : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	auto olr = ws.world.nation_get_overlord_as_subject(trigger::to_nation(primary_slot));
	return display_subeffects(ws, tval, layout, trigger::to_generic(ws.world.overlord_get_ruler(olr)), this_slot, from_slot, r_hi,
			r_lo, indentation + indentation_amount);
}
uint32_t es_sphere_owner_scope(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "sphere_leader_of"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.nation_get_name(trigger::to_nation(primary_slot)))
													 : text::produce_simple_string(ws, "singular_nation"));
		text::close_layout_box(layout, box);
	}

	return display_subeffects(ws, tval, layout,
			primary_slot != -1 ? trigger::to_generic(ws.world.nation_get_in_sphere_of((trigger::to_nation(primary_slot)))) : -1,
			this_slot, from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_independence_scope(EFFECT_DISPLAY_PARAMS) {
	auto rtag =
			from_slot != -1 ? ws.world.rebel_faction_get_defection_target(trigger::to_rebel(from_slot)) : dcon::national_identity_id{};
	auto r_holder = ws.world.national_identity_get_nation_from_identity_holder(rtag);

	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "reb_independence_nation"));
		text::close_layout_box(layout, box);
	}

	return display_subeffects(ws, tval, layout, r_holder ? trigger::to_generic(r_holder) : -1, this_slot, from_slot, r_hi, r_lo,
			indentation + indentation_amount);
}
uint32_t es_flashpoint_tag_scope(EFFECT_DISPLAY_PARAMS) {
	auto ctag = ws.world.state_instance_get_flashpoint_tag(trigger::to_state(primary_slot));
	auto fp_nation = ws.world.national_identity_get_nation_from_identity_holder(ctag);

	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "flashpoint_nation"));
		text::close_layout_box(layout, box);
	}
	return display_subeffects(ws, tval, layout, fp_nation ? trigger::to_generic(fp_nation) : -1, this_slot, from_slot, r_hi, r_lo,
			indentation + indentation_amount);
}
uint32_t es_crisis_state_scope(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "crisis_state"));
		text::close_layout_box(layout, box);
	}

	auto cstate = ws.crisis_state ? trigger::to_generic(ws.crisis_state) : -1;
	return display_subeffects(ws, tval, layout, cstate, this_slot, from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_state_scope_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "containing_state"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box,
				primary_slot != -1 ? text::produce_simple_string(ws, ws.world.province_get_name(trigger::to_prov(primary_slot)))
													 : text::produce_simple_string(ws, "singular_province"));
		text::close_layout_box(layout, box);
	}
	auto st =
			primary_slot != -1 ? ws.world.province_get_state_membership(trigger::to_prov(primary_slot)) : dcon::state_instance_id{};
	return display_subeffects(ws, tval, layout, st ? trigger::to_generic(st) : -1, this_slot, from_slot, r_hi, r_lo,
			indentation + indentation_amount);
}
uint32_t es_state_scope_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "containing_state"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "singular_pop"));
		text::close_layout_box(layout, box);
	}
	auto st = primary_slot != -1 ? ws.world.province_get_state_membership(
																		 ws.world.pop_get_province_from_pop_location(trigger::to_pop(primary_slot)))
															 : dcon::state_instance_id{};
	return display_subeffects(ws, tval, layout, st ? trigger::to_generic(st) : -1, this_slot, from_slot, r_hi, r_lo,
			indentation + indentation_amount);
}
uint32_t es_tag_scope(EFFECT_DISPLAY_PARAMS) {
	auto tag = trigger::payload(tval[2]).tag_id;
	auto tag_holder = ws.world.national_identity_get_nation_from_identity_holder(tag);

	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box,
				text::produce_simple_string(ws,
						tag_holder ? ws.world.nation_get_name(tag_holder) : ws.world.national_identity_get_name(tag)));
		text::close_layout_box(layout, box);
	}
	return display_subeffects(ws, tval, layout, tag_holder ? trigger::to_generic(tag_holder) : -1, this_slot, from_slot, r_hi, r_lo,
			indentation + indentation_amount);
}
uint32_t es_integer_scope(EFFECT_DISPLAY_PARAMS) {
	auto p = trigger::payload(tval[2]).prov_id;

	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, ws.world.province_get_name(p)));
		text::close_layout_box(layout, box);
	}
	return display_subeffects(ws, tval, layout, trigger::to_generic(p), this_slot, from_slot, r_hi, r_lo,
			indentation + indentation_amount);
}
uint32_t es_pop_type_scope_nation(EFFECT_DISPLAY_PARAMS) {
	auto type = (tval[0] & effect::scope_has_limit) != 0 ? trigger::payload(tval[3]).popt_id : trigger::payload(tval[2]).popt_id;

	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "every"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, ws.world.pop_type_get_name(type)));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pop"));
		text::close_layout_box(layout, box);
	}

	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_pop_type_scope_state(EFFECT_DISPLAY_PARAMS) {
	auto type = (tval[0] & effect::scope_has_limit) != 0 ? trigger::payload(tval[3]).popt_id : trigger::payload(tval[2]).popt_id;

	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "every"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, ws.world.pop_type_get_name(type)));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pop"));
		text::close_layout_box(layout, box);
	}

	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_pop_type_scope_province(EFFECT_DISPLAY_PARAMS) {
	auto type = (tval[0] & effect::scope_has_limit) != 0 ? trigger::payload(tval[3]).popt_id : trigger::payload(tval[2]).popt_id;

	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "every"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, ws.world.pop_type_get_name(type)));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "pop"));
		text::close_layout_box(layout, box);
	}

	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_hi, r_lo, indentation + indentation_amount);
}
uint32_t es_region_scope(EFFECT_DISPLAY_PARAMS) {

	auto region =
			(tval[0] & effect::scope_has_limit) != 0 ? trigger::payload(tval[3]).state_id : trigger::payload(tval[2]).state_id;

	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, region);
		text::close_layout_box(layout, box);
	}

	show_limit(ws, tval, layout, this_slot, from_slot, indentation);

	return display_subeffects(ws, tval, layout, -1, this_slot, from_slot, r_hi, r_lo, indentation + indentation_amount);
}

uint32_t ef_none(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "no_effect"));
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_capital(EFFECT_DISPLAY_PARAMS) {
	auto new_capital = trigger::payload(tval[1]).prov_id;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "move_capital_to"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, new_capital);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_core_tag(EFFECT_DISPLAY_PARAMS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::adj, ws.world.national_identity_get_adjective(tag));
		text::localised_format_box(ws, layout, box, "add_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_core_tag_state(EFFECT_DISPLAY_PARAMS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::adj, ws.world.national_identity_get_adjective(tag));
		text::localised_format_box(ws, layout, box, "add_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}

uint32_t ef_add_core_state_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_this_to_map(ws, m, trigger::to_nation(this_slot));
		text::localised_format_box(ws, layout, box, "add_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_core_state_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_this_to_map(ws, m, trigger::to_prov(this_slot));
		text::localised_format_box(ws, layout, box, "add_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_core_state_this_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_this_to_map(ws, m, trigger::to_state(this_slot));
		text::localised_format_box(ws, layout, box, "add_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_core_state_this_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_this_to_map(ws, m, trigger::to_pop(this_slot));
		text::localised_format_box(ws, layout, box, "add_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_core_state_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_from_to_map(ws, m, trigger::to_prov(from_slot));
		text::localised_format_box(ws, layout, box, "add_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_core_state_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_from_to_map(ws, m, trigger::to_nation(from_slot));
		text::localised_format_box(ws, layout, box, "add_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_core_state_reb(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_from_to_map(ws, m, trigger::to_rebel(from_slot));
		text::localised_format_box(ws, layout, box, "add_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}

uint32_t ef_add_core_int(EFFECT_DISPLAY_PARAMS) {
	auto prov = trigger::payload(tval[1]).prov_id;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "add_core_to"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, prov);
		text::close_layout_box(layout, box);
	}
	return 0;
}

uint32_t ef_add_core_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_this_to_map(ws, m, trigger::to_nation(this_slot));
		text::localised_format_box(ws, layout, box, "add_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_core_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_this_to_map(ws, m, trigger::to_prov(this_slot));
		text::localised_format_box(ws, layout, box, "add_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_core_this_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_this_to_map(ws, m, trigger::to_state(this_slot));
		text::localised_format_box(ws, layout, box, "add_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_core_this_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_this_to_map(ws, m, trigger::to_pop(this_slot));
		text::localised_format_box(ws, layout, box, "add_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_core_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_from_to_map(ws, m, trigger::to_nation(from_slot));
		text::localised_format_box(ws, layout, box, "add_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_core_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_from_to_map(ws, m, trigger::to_prov(from_slot));
		text::localised_format_box(ws, layout, box, "add_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_core_reb(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_from_to_map(ws, m, trigger::to_rebel(from_slot));
		text::localised_format_box(ws, layout, box, "add_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_core_tag(EFFECT_DISPLAY_PARAMS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::adj, ws.world.national_identity_get_adjective(tag));
		text::localised_format_box(ws, layout, box, "remove_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_core_tag_state(EFFECT_DISPLAY_PARAMS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::adj, ws.world.national_identity_get_adjective(tag));
		text::localised_format_box(ws, layout, box, "remove_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_core_int(EFFECT_DISPLAY_PARAMS) {
	auto prov = trigger::payload(tval[1]).prov_id;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, text::produce_simple_string(ws, "remove_core_from"));
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, prov);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_core_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_this_to_map(ws, m, trigger::to_nation(this_slot));
		text::localised_format_box(ws, layout, box, "remove_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_core_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_this_to_map(ws, m, trigger::to_prov(this_slot));
		text::localised_format_box(ws, layout, box, "remove_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_core_this_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_this_to_map(ws, m, trigger::to_state(this_slot));
		text::localised_format_box(ws, layout, box, "remove_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_core_this_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_this_to_map(ws, m, trigger::to_pop(this_slot));
		text::localised_format_box(ws, layout, box, "remove_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_core_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_from_to_map(ws, m, trigger::to_nation(from_slot));
		text::localised_format_box(ws, layout, box, "remove_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_core_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_from_to_map(ws, m, trigger::to_prov(from_slot));
		text::localised_format_box(ws, layout, box, "remove_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_core_reb(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_adj_from_to_map(ws, m, trigger::to_rebel(from_slot));
		text::localised_format_box(ws, layout, box, "remove_x_core", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_change_region_name_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, trigger::payload(tval[1]).text_id);
		text::localised_format_box(ws, layout, box, "change_name_to", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_change_region_name_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, trigger::payload(tval[1]).text_id);
		text::localised_format_box(ws, layout, box, "change_state_name_to", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_trade_goods(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.commodity_get_name(trigger::payload(tval[1]).com_id));
		text::localised_format_box(ws, layout, box, "change_rgo_production_to", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_accepted_culture(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.culture_get_name(trigger::payload(tval[1]).cul_id));
		text::localised_format_box(ws, layout, box, "make_accepted_culture", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_accepted_culture_union(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "union_culture_accepted", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_primary_culture(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.culture_get_name(trigger::payload(tval[1]).cul_id));
		text::localised_format_box(ws, layout, box, "primary_culture_changes_to", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
dcon::nation_id convert_this(sys::state& ws, dcon::nation_id n) {
	return n;
}
dcon::nation_id convert_this(sys::state& ws, dcon::state_instance_id p) {
	return ws.world.state_instance_get_nation_from_state_ownership(p);
}
dcon::nation_id convert_this(sys::state& ws, dcon::province_id p) {
	return ws.world.province_get_nation_from_province_ownership(p);
}
dcon::nation_id convert_this(sys::state& ws, dcon::pop_id p) {
	return nations::owner_of_pop(ws, p);
}
dcon::nation_id convert_this(sys::state& ws, dcon::rebel_faction_id p) {
	auto fp = fatten(ws.world, p);
	return fp.get_defection_target().get_nation_from_identity_holder();
}
template<typename T, typename F>
void add_to_map(sys::state& ws, text::substitution_map& map, T v, std::string_view alt, F const& f) {
	if(v) {
		text::add_to_substitution_map(map, text::variable_type::text, f(v));
	} else {
		if(auto k = ws.key_to_text_sequence.find(alt); k != ws.key_to_text_sequence.end()) {
			text::add_to_substitution_map(map, text::variable_type::text, k->second);
		} else {
			text::add_to_substitution_map(map, text::variable_type::text, alt);
		}
	}
}
uint32_t ef_primary_culture_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, trigger::to_nation(this_slot), "this_nation_culture",
				[&](auto id) { return ws.world.culture_get_name(ws.world.nation_get_primary_culture(id)); });
		text::localised_format_box(ws, layout, box, "primary_culture_changes_to", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_primary_culture_this_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_state(this_slot)), "this_nation_culture",
				[&](auto id) { return ws.world.culture_get_name(ws.world.nation_get_primary_culture(id)); });
		text::localised_format_box(ws, layout, box, "primary_culture_changes_to", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_primary_culture_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation_culture",
				[&](auto id) { return ws.world.culture_get_name(ws.world.nation_get_primary_culture(id)); });
		text::localised_format_box(ws, layout, box, "primary_culture_changes_to", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_primary_culture_this_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_pop(this_slot)), "this_nation_culture",
				[&](auto id) { return ws.world.culture_get_name(ws.world.nation_get_primary_culture(id)); });
		text::localised_format_box(ws, layout, box, "primary_culture_changes_to", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_primary_culture_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation_culture",
				[&](auto id) { return ws.world.culture_get_name(ws.world.nation_get_primary_culture(id)); });
		text::localised_format_box(ws, layout, box, "primary_culture_changes_to", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_accepted_culture(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.culture_get_name(trigger::payload(tval[1]).cul_id));
		text::localised_format_box(ws, layout, box, "alice_remove_accepted_culture", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_life_rating(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "life_rating", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(trigger::payload(tval[1]).signed_value), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_religion(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.religion_get_name(trigger::payload(tval[1]).rel_id));
		text::localised_format_box(ws, layout, box, "make_national_religion", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_is_slave_state_yes(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "make_slave_state", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_is_slave_pop_yes(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "make_slave_pop", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_research_points(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "research_points", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(trigger::payload(tval[1]).signed_value), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_tech_school(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id));
		text::localised_format_box(ws, layout, box, "change_tech_school", m);
		text::close_layout_box(layout, box);
	}
	modifier_description(ws, layout, trigger::payload(tval[1]).mod_id, indentation + +indentation_amount);
	return 0;
}
uint32_t ef_government(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.culture_definitions.governments[trigger::payload(tval[1]).gov_id].name);
		text::localised_format_box(ws, layout, box, "change_government_to", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_government_reb(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		if(from_slot != -1) {
			auto new_gov = ws.world.rebel_faction_get_type(trigger::to_rebel(from_slot))
												 .get_government_change(ws.world.nation_get_government_type(trigger::to_nation(primary_slot)));
			if(new_gov)
				text::add_to_substitution_map(m, text::variable_type::text, ws.culture_definitions.governments[new_gov].name);
		}
		text::localised_format_box(ws, layout, box, "change_government_to", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_treasury(EFFECT_DISPLAY_PARAMS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	{
		auto box = text::open_layout_box(layout, indentation);
		display_value(text::fp_currency{amount}, true, ws, layout, box);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "add_to_treasury", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_war_exhaustion(EFFECT_DISPLAY_PARAMS) {
	auto amount = trigger::read_float_from_payload(tval + 1) / 100.0f;

	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "military_war_exhaustion", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(text::fp_percentage{amount}, false, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_prestige(EFFECT_DISPLAY_PARAMS) {
	auto delta = trigger::read_float_from_payload(tval + 1);
	float change = delta;
	if(primary_slot != -1) {
		auto prestige_multiplier =
				1.0f + ws.world.nation_get_modifier_values(trigger::to_nation(primary_slot), sys::national_mod_offsets::prestige);
		auto new_prestige = std::max(0.0f,
				ws.world.nation_get_prestige(trigger::to_nation(primary_slot)) + (delta > 0 ? (delta * prestige_multiplier) : delta));
		change = (new_prestige - ws.world.nation_get_prestige(trigger::to_nation(primary_slot)));
	}
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "prestige", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(change), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_change_tag(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, trigger::payload(tval[1]).tag_id);
		text::localised_format_box(ws, layout, box, "become_blank", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_change_tag_culture(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		if(primary_slot != -1) {
			auto prim_culture = ws.world.nation_get_primary_culture(trigger::to_nation(primary_slot));
			auto cg = ws.world.culture_get_group_from_culture_group_membership(prim_culture);
			auto u = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
			text::add_to_substitution_map(m, text::variable_type::text, u);
		} else {
			if(auto k = ws.key_to_text_sequence.find(std::string_view("cultural_union_nation")); k != ws.key_to_text_sequence.end()) {
				text::add_to_substitution_map(m, text::variable_type::text, k->second);
			}
		}
		text::localised_format_box(ws, layout, box, "become_blank", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_change_tag_no_core_switch(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, trigger::payload(tval[1]).tag_id);
		text::localised_format_box(ws, layout, box, "player_control_change", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_change_tag_no_core_switch_culture(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		if(primary_slot != -1) {
			auto prim_culture = ws.world.nation_get_primary_culture(trigger::to_nation(primary_slot));
			auto cg = ws.world.culture_get_group_from_culture_group_membership(prim_culture);
			auto u = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
			text::add_to_substitution_map(m, text::variable_type::text, u);
		} else {
			if(auto k = ws.key_to_text_sequence.find(std::string_view("cultural_union_nation")); k != ws.key_to_text_sequence.end()) {
				text::add_to_substitution_map(m, text::variable_type::text, k->second);
			}
		}
		text::localised_format_box(ws, layout, box, "player_control_change", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_set_country_flag(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.national_definitions.flag_variable_names[trigger::payload(tval[1]).natf_id]);
		text::localised_format_box(ws, layout, box, "set_national_flag", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_clr_country_flag(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.national_definitions.flag_variable_names[trigger::payload(tval[1]).natf_id]);
		text::localised_format_box(ws, layout, box, "remove_national_flag", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_military_access(EFFECT_DISPLAY_PARAMS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(target) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::localised_format_box(ws, layout, box, "get_access_with", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_military_access_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "get_access_with", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_military_access_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "get_access_with", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_military_access_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "get_access_with", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_military_access_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "get_access_with", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_badboy(EFFECT_DISPLAY_PARAMS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "infamy", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(text::fp_one_place{amount}, false, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_secede_province(EFFECT_DISPLAY_PARAMS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(target) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::localised_format_box(ws, layout, box, "change_province_owner", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_secede_province_state(EFFECT_DISPLAY_PARAMS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(target) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::localised_format_box(ws, layout, box, "change_state_owner", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_secede_province_state_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "change_state_owner", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_secede_province_state_this_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_state(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "change_state_owner", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_secede_province_state_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "change_state_owner", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_secede_province_state_this_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_pop(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "change_state_owner", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_secede_province_state_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "change_state_owner", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_secede_province_state_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "change_state_owner", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_secede_province_state_reb(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_rebel(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "change_state_owner", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_secede_province_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "change_province_owner", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_secede_province_this_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_state(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "change_province_owner", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_secede_province_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "change_province_owner", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_secede_province_this_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_pop(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "change_province_owner", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_secede_province_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "change_province_owner", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_secede_province_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "change_province_owner", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_secede_province_reb(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_rebel(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "change_province_owner", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_inherit(EFFECT_DISPLAY_PARAMS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(target) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::localised_format_box(ws, layout, box, "annex_effect", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_inherit_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "annex_effect", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_inherit_this_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_state(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "annex_effect", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_inherit_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "annex_effect", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_inherit_this_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_pop(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "annex_effect", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_inherit_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "annex_effect", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_inherit_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "annex_effect", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_annex_to(EFFECT_DISPLAY_PARAMS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(target) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::localised_format_box(ws, layout, box, "annexed_by_effect", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_annex_to_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "annexed_by_effect", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_annex_to_this_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_state(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "annexed_by_effect", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_annex_to_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "annexed_by_effect", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_annex_to_this_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_pop(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "annexed_by_effect", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_annex_to_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "annexed_by_effect", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_annex_to_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "annexed_by_effect", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_release(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, trigger::payload(tval[1]).tag_id);
		text::localised_format_box(ws, layout, box, "core_return", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_release_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "core_return", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_release_this_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_state(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "core_return", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_release_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "core_return", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_release_this_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_pop(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "core_return", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_release_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "core_return", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_release_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "core_return", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_change_controller(EFFECT_DISPLAY_PARAMS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(target) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::localised_format_box(ws, layout, box, "change_province_controller", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_change_controller_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "change_province_controller", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_change_controller_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "change_province_controller", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_change_controller_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "change_province_controller", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_change_controller_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "change_province_controller", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_infrastructure(EFFECT_DISPLAY_PARAMS) {
	auto type = economy::province_building_type::railroad;
	auto amount = trigger::payload(tval[1]).signed_value;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, economy::province_building_type_get_level_text(type), m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(amount), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_money(EFFECT_DISPLAY_PARAMS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "pop_savings", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(text::fp_currency{amount}, true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_leadership(EFFECT_DISPLAY_PARAMS) {
	auto amount = trigger::payload(tval[1]).signed_value;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "leadership_points", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(amount), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_create_vassal(EFFECT_DISPLAY_PARAMS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(target) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::localised_format_box(ws, layout, box, "release_as_vassal", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_create_vassal_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "release_as_vassal", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_create_vassal_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "release_as_vassal", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_create_vassal_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "release_as_vassal", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_create_vassal_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "release_as_vassal", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_end_military_access(EFFECT_DISPLAY_PARAMS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(target) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::localised_format_box(ws, layout, box, "lose_access_with", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_end_military_access_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "lose_access_with", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_end_military_access_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "lose_access_with", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_end_military_access_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "lose_access_with", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_end_military_access_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "lose_access_with", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_leave_alliance(EFFECT_DISPLAY_PARAMS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(target) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::localised_format_box(ws, layout, box, "end_alliance", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_leave_alliance_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "end_alliance", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_leave_alliance_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "end_alliance", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_leave_alliance_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "end_alliance", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_leave_alliance_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "end_alliance", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_end_war(EFFECT_DISPLAY_PARAMS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(target) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::localised_format_box(ws, layout, box, "end_war_with", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_end_war_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "end_war_with", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_end_war_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "end_war_with", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_end_war_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "end_war_with", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_end_war_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "end_war_with", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_enable_ideology(EFFECT_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	text::substitution_map m;
	text::add_to_substitution_map(m, text::variable_type::text, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id));
	text::localised_format_box(ws, layout, box, "enable_blank", m);
	text::close_layout_box(layout, box);
	return 0;
}
uint32_t ef_ruling_party_ideology(EFFECT_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	text::substitution_map m;
	text::add_to_substitution_map(m, text::variable_type::text, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id));
	text::localised_format_box(ws, layout, box, "change_ruling_party", m);
	text::close_layout_box(layout, box);
	return 0;
}
uint32_t ef_plurality(EFFECT_DISPLAY_PARAMS) {
	auto amount = trigger::read_float_from_payload(tval + 1) / 100.0f;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "plurality", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(text::fp_percentage{amount}, true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_province_modifier(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id));
		text::localised_format_box(ws, layout, box, "remove_prov_mod", m);
		text::close_layout_box(layout, box);
	}
	modifier_description(ws, layout, trigger::payload(tval[1]).mod_id, indentation + indentation_amount);
	return 0;
}
uint32_t ef_remove_country_modifier(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id));
		text::localised_format_box(ws, layout, box, "remove_nat_mod", m);
		text::close_layout_box(layout, box);
	}
	modifier_description(ws, layout, trigger::payload(tval[1]).mod_id, indentation + indentation_amount);
	return 0;
}
uint32_t ef_create_alliance(EFFECT_DISPLAY_PARAMS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(target) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::localised_format_box(ws, layout, box, "make_alliance", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_create_alliance_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "make_alliance", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_create_alliance_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "make_alliance", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_create_alliance_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "make_alliance", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_create_alliance_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "make_alliance", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_release_vassal(EFFECT_DISPLAY_PARAMS) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(!holder)
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	if(hprovs.begin() == hprovs.end()) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, holder);
		text::localised_format_box(ws, layout, box, "release_as_vassal", m);
		text::close_layout_box(layout, box);
	} else {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, holder);
		text::localised_format_box(ws, layout, box, "becomes_independent", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_release_vassal_this_nation(EFFECT_DISPLAY_PARAMS) {
	auto holder = trigger::to_nation(this_slot);
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	if(hprovs.begin() == hprovs.end()) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, holder);
		text::localised_format_box(ws, layout, box, "release_as_vassal", m);
		text::close_layout_box(layout, box);
	} else {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "becomes_independent", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_release_vassal_this_province(EFFECT_DISPLAY_PARAMS) {
	auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	if(hprovs.begin() == hprovs.end()) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, holder);
		text::localised_format_box(ws, layout, box, "release_as_vassal", m);
		text::close_layout_box(layout, box);
	} else {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "becomes_independent", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_release_vassal_from_nation(EFFECT_DISPLAY_PARAMS) {
	auto holder = trigger::to_nation(from_slot);
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	if(hprovs.begin() == hprovs.end()) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, holder);
		text::localised_format_box(ws, layout, box, "release_as_vassal", m);
		text::close_layout_box(layout, box);
	} else {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "becomes_independent", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_release_vassal_from_province(EFFECT_DISPLAY_PARAMS) {
	auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	if(hprovs.begin() == hprovs.end()) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, holder);
		text::localised_format_box(ws, layout, box, "release_as_vassal", m);
		text::close_layout_box(layout, box);
	} else {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "becomes_independent", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_release_vassal_reb(EFFECT_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	text::substitution_map m;
	add_to_map(ws, m, convert_this(ws, trigger::to_rebel(from_slot)), "from_nation", [](auto x) { return x; });
	text::localised_format_box(ws, layout, box, "release_as_independent", m);
	text::close_layout_box(layout, box);
	return 0;
}
uint32_t ef_release_vassal_random(EFFECT_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	text::localised_format_box(ws, layout, box, "no_effect");
	text::close_layout_box(layout, box);
	return 0;
}
uint32_t ef_change_province_name(EFFECT_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	text::substitution_map m;
	text::add_to_substitution_map(m, text::variable_type::text, trigger::payload(tval[1]).text_id);
	text::localised_format_box(ws, layout, box, "change_name_to", m);
	text::close_layout_box(layout, box);
	return 0;
}
uint32_t ef_enable_canal(EFFECT_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	text::substitution_map m;
	auto canal_name = text::produce_simple_string(ws, std::string("canal_") + std::to_string(tval[1]));
	text::add_to_substitution_map(m, text::variable_type::name, std::string_view{canal_name});
	text::localised_format_box(ws, layout, box, "enable_canal", m);
	text::close_layout_box(layout, box);
	return 0;
}
uint32_t ef_set_global_flag(EFFECT_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	text::substitution_map m;
	text::add_to_substitution_map(m, text::variable_type::text,
			ws.national_definitions.global_flag_variable_names[trigger::payload(tval[1]).glob_id]);
	text::localised_format_box(ws, layout, box, "set_global_flag", m);
	text::close_layout_box(layout, box);
	return 0;
}
uint32_t ef_clr_global_flag(EFFECT_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	text::substitution_map m;
	text::add_to_substitution_map(m, text::variable_type::text,
			ws.national_definitions.global_flag_variable_names[trigger::payload(tval[1]).glob_id]);
	text::localised_format_box(ws, layout, box, "remove_global_flag", m);
	text::close_layout_box(layout, box);
	return 0;
}
uint32_t ef_nationalvalue_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id));
		text::localised_format_box(ws, layout, box, "change_national_value", m);
		text::close_layout_box(layout, box);
	}
	modifier_description(ws, layout, trigger::payload(tval[1]).mod_id, indentation + indentation_amount);
	return 0;
}
uint32_t ef_nationalvalue_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id));
		text::localised_format_box(ws, layout, box, "change_national_value", m);
		text::close_layout_box(layout, box);
	}
	modifier_description(ws, layout, trigger::payload(tval[1]).mod_id, indentation + indentation_amount);
	return 0;
}
uint32_t ef_civilized_yes(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::localised_format_box(ws, layout, box, "become_civ");
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_civilized_no(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::localised_format_box(ws, layout, box, "become_unciv");
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_is_slave_state_no(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::localised_format_box(ws, layout, box, "free_slave_state");
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_is_slave_pop_no(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::localised_format_box(ws, layout, box, "free_slave_pop");
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_election(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::localised_format_box(ws, layout, box, "hold_election");
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_social_reform(EFFECT_DISPLAY_PARAMS) {
	{
		auto opt = fatten(ws.world, trigger::payload(tval[1]).opt_id);

		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::issue, opt.get_parent_issue().get_name());
		text::add_to_substitution_map(m, text::variable_type::text, opt.get_name());
		text::localised_format_box(ws, layout, box, "issue_change", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_political_reform(EFFECT_DISPLAY_PARAMS) {
	{
		auto opt = fatten(ws.world, trigger::payload(tval[1]).opt_id);

		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::issue, opt.get_parent_issue().get_name());
		text::add_to_substitution_map(m, text::variable_type::text, opt.get_name());
		text::localised_format_box(ws, layout, box, "issue_change", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_tax_relative_income(EFFECT_DISPLAY_PARAMS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	if(primary_slot != -1) {
		auto income = ws.world.nation_get_total_poor_income(trigger::to_nation(primary_slot)) +
									ws.world.nation_get_total_middle_income(trigger::to_nation(primary_slot)) +
									ws.world.nation_get_total_rich_income(trigger::to_nation(primary_slot));
		auto combined_amount = income * amount;

		auto box = text::open_layout_box(layout, indentation);
		display_value(text::fp_currency{combined_amount}, true, ws, layout, box);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "add_to_treasury", m);
		text::close_layout_box(layout, box);
	} else {
		auto box = text::open_layout_box(layout, indentation);
		display_value(text::fp_percentage{amount}, true, ws, layout, box);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "add_relative_income", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_neutrality(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::localised_format_box(ws, layout, box, "make_neutral");
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_reduce_pop(EFFECT_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	text::substitution_map m;
	text::localised_format_box(ws, layout, box, "pop_size", m);
	text::add_space_to_layout_box(ws, layout, box);
	display_value(text::fp_percentage{trigger::read_float_from_payload(tval + 1) - 1.0f}, true, ws, layout, box);
	text::close_layout_box(layout, box);
	return 0;
}
uint32_t ef_move_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, trigger::payload(tval[1]).prov_id);
		text::localised_format_box(ws, layout, box, "move_pop_to", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_pop_type(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.pop_type_get_name(trigger::payload(tval[1]).popt_id));
		text::localised_format_box(ws, layout, box, "change_pop_type", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_years_of_research(EFFECT_DISPLAY_PARAMS) {
	if(primary_slot != -1) {
		auto amount = trigger::read_float_from_payload(tval + 1);
		auto result = nations::daily_research_points(ws, trigger::to_nation(primary_slot)) * 365.0f * amount;

		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "research_points", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(result), true, ws, layout, box);
		text::close_layout_box(layout, box);
		return 0;
	} else {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, true, ws, layout, box);
		text::localised_format_box(ws, layout, box, "years_of_research", m);
		text::close_layout_box(layout, box);
		return 0;
	}
}
uint32_t ef_prestige_factor_positive(EFFECT_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	text::substitution_map m;
	text::localised_format_box(ws, layout, box, "prestige", m);
	text::add_space_to_layout_box(ws, layout, box);
	display_value(text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, true, ws, layout, box);
	text::close_layout_box(layout, box);
	return 0;
}
uint32_t ef_prestige_factor_negative(EFFECT_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	text::substitution_map m;
	text::localised_format_box(ws, layout, box, "prestige", m);
	text::add_space_to_layout_box(ws, layout, box);
	display_value(text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, true, ws, layout, box);
	text::close_layout_box(layout, box);
	return 0;
}
uint32_t ef_military_reform(EFFECT_DISPLAY_PARAMS) {
	{
		auto opt = fatten(ws.world, trigger::payload(tval[1]).ropt_id);

		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::issue, opt.get_parent_reform().get_name());
		text::add_to_substitution_map(m, text::variable_type::text, opt.get_name());
		text::localised_format_box(ws, layout, box, "issue_change", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_economic_reform(EFFECT_DISPLAY_PARAMS) {
	{
		auto opt = fatten(ws.world, trigger::payload(tval[1]).ropt_id);

		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::issue, opt.get_parent_reform().get_name());
		text::add_to_substitution_map(m, text::variable_type::text, opt.get_name());
		text::localised_format_box(ws, layout, box, "issue_change", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_random_military_reforms(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::value, int64_t(tval[1]));
		text::localised_format_box(ws, layout, box, "remove_mil_reforms", m);
		text::close_layout_box(layout, box);
	}
	return tval[1];
}
uint32_t ef_remove_random_economic_reforms(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::value, int64_t(tval[1]));
		text::localised_format_box(ws, layout, box, "remove_econ_reforms", m);
		text::close_layout_box(layout, box);
	}
	return tval[1];
}
uint32_t ef_add_crime(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.culture_definitions.crimes[trigger::payload(tval[1]).crm_id].name);
		text::localised_format_box(ws, layout, box, "add_crime", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_crime_none(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "remove_crime", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_nationalize(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "perform_nationalization", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_build_factory_in_capital_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.factory_type_get_name(trigger::payload(tval[1]).fac_id));
		text::localised_format_box(ws, layout, box, "build_factory_in_capital", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_activate_technology(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.technology_get_name(trigger::payload(tval[1]).tech_id));
		text::localised_format_box(ws, layout, box, "enable_blank", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_activate_invention(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.invention_get_name(trigger::payload(tval[1]).invt_id));
		text::localised_format_box(ws, layout, box, "enable_blank", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_great_wars_enabled_yes(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "enable_great_wars", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_great_wars_enabled_no(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "disable_great_wars", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_world_wars_enabled_yes(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "enable_world_wars", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_world_wars_enabled_no(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "disable_world_wars", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_assimilate_province(EFFECT_DISPLAY_PARAMS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner) {
		auto owner_c = ws.world.nation_get_primary_culture(owner);

		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.culture_get_name(owner_c));
		text::localised_format_box(ws, layout, box, "assimilate_province", m);
		text::close_layout_box(layout, box);

		return 0;
	} else {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		std::string t = text::produce_simple_string(ws, "owner_primary_culture");
		text::add_to_substitution_map(m, text::variable_type::text, std::string_view{t});
		text::localised_format_box(ws, layout, box, "assimilate_province", m);
		text::close_layout_box(layout, box);

		return 0;
	}
}
uint32_t ef_assimilate_state(EFFECT_DISPLAY_PARAMS) {
	if(auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot)); owner) {
		auto owner_c = ws.world.nation_get_primary_culture(owner);

		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.culture_get_name(owner_c));
		text::localised_format_box(ws, layout, box, "assimilate_province", m);
		text::close_layout_box(layout, box);

		return 0;
	} else {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		std::string t = text::produce_simple_string(ws, "owner_primary_culture");
		text::add_to_substitution_map(m, text::variable_type::text, std::string_view{t});
		text::localised_format_box(ws, layout, box, "assimilate_province", m);
		text::close_layout_box(layout, box);

		return 0;
	}
}
uint32_t ef_assimilate_pop(EFFECT_DISPLAY_PARAMS) {
	if(auto owner = nations::owner_of_pop(ws, trigger::to_pop(primary_slot)); owner) {
		auto owner_c = ws.world.nation_get_primary_culture(owner);

		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.culture_get_name(owner_c));
		text::localised_format_box(ws, layout, box, "assimilate_pop", m);
		text::close_layout_box(layout, box);

		return 0;
	} else {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		std::string t = text::produce_simple_string(ws, "owner_primary_culture");
		text::add_to_substitution_map(m, text::variable_type::text, std::string_view{t});
		text::localised_format_box(ws, layout, box, "assimilate_pop", m);
		text::close_layout_box(layout, box);

		return 0;
	}
}
uint32_t ef_literacy(EFFECT_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	text::substitution_map m;
	text::localised_format_box(ws, layout, box, "literacy", m);
	text::add_space_to_layout_box(ws, layout, box);
	display_value(text::fp_percentage{trigger::read_float_from_payload(tval + 1)}, true, ws, layout, box);
	text::close_layout_box(layout, box);
	return 0;
}
uint32_t ef_add_crisis_interest(EFFECT_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	text::substitution_map m;
	text::localised_format_box(ws, layout, box, "add_crisis_interest", m);
	text::close_layout_box(layout, box);
	return 0;
}
uint32_t ef_flashpoint_tension(EFFECT_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	text::substitution_map m;
	text::localised_format_box(ws, layout, box, "flashpoint_tension_label", m);
	text::add_space_to_layout_box(ws, layout, box);
	display_value(int64_t(trigger::read_float_from_payload(tval + 1)), true, ws, layout, box);
	text::close_layout_box(layout, box);
	return 0;
}
uint32_t ef_add_crisis_temperature(EFFECT_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	text::substitution_map m;
	text::localised_format_box(ws, layout, box, "crisis_temperature_plain", m);
	text::add_space_to_layout_box(ws, layout, box);
	display_value(int64_t(trigger::read_float_from_payload(tval + 1)), true, ws, layout, box);
	text::close_layout_box(layout, box);
	return 0;
}
uint32_t ef_consciousness(EFFECT_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	text::substitution_map m;
	text::localised_format_box(ws, layout, box, "consciousness", m);
	text::add_space_to_layout_box(ws, layout, box);
	display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, true, ws, layout, box);
	text::close_layout_box(layout, box);
	return 0;
}
uint32_t ef_militancy(EFFECT_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	text::substitution_map m;
	text::localised_format_box(ws, layout, box, "militancy", m);
	text::add_space_to_layout_box(ws, layout, box);
	display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, false, ws, layout, box);
	text::close_layout_box(layout, box);
	return 0;
}
uint32_t ef_rgo_size(EFFECT_DISPLAY_PARAMS) {
	auto box = text::open_layout_box(layout, indentation);
	text::substitution_map m;
	text::localised_format_box(ws, layout, box, "rgo_size", m);
	text::add_space_to_layout_box(ws, layout, box);
	display_value(int64_t(trigger::payload(tval[1]).signed_value), false, ws, layout, box);
	text::close_layout_box(layout, box);
	return 0;
}
uint32_t ef_fort(EFFECT_DISPLAY_PARAMS) {
	auto type = economy::province_building_type::fort;
	auto amount = trigger::payload(tval[1]).signed_value;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, economy::province_building_type_get_level_text(type), m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(amount), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_naval_base(EFFECT_DISPLAY_PARAMS) {
	auto type = economy::province_building_type::naval_base;
	auto amount = trigger::payload(tval[1]).signed_value;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, economy::province_building_type_get_level_text(type), m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(amount), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_trigger_revolt_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "trigger_every_revolt", m);
		text::close_layout_box(layout, box);
	}
	if(trigger::payload(tval[1]).reb_id) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.rebel_type_get_name(trigger::payload(tval[1]).reb_id));
		text::localised_format_box(ws, layout, box, "of_type", m);
		text::close_layout_box(layout, box);
	}
	if(trigger::payload(tval[2]).cul_id) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.culture_get_name(trigger::payload(tval[2]).cul_id));
		text::localised_format_box(ws, layout, box, "where_culture_is", m);
		text::close_layout_box(layout, box);
	}
	if(trigger::payload(tval[3]).rel_id) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.religion_get_name(trigger::payload(tval[3]).rel_id));
		text::localised_format_box(ws, layout, box, "where_religion_is", m);
		text::close_layout_box(layout, box);
	}
	if(trigger::payload(tval[4]).ideo_id) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.ideology_get_name(trigger::payload(tval[4]).ideo_id));
		text::localised_format_box(ws, layout, box, "where_ideology_is", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_trigger_revolt_state(EFFECT_DISPLAY_PARAMS) {
	return ef_trigger_revolt_nation(ws, tval, layout, 0, 0, 0, r_lo, r_hi, indentation);
}
uint32_t ef_trigger_revolt_province(EFFECT_DISPLAY_PARAMS) {
	return ef_trigger_revolt_nation(ws, tval, layout, 0, 0, 0, r_lo, r_hi, indentation);
}
uint32_t ef_diplomatic_influence(EFFECT_DISPLAY_PARAMS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(target) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::localised_format_box(ws, layout, box, "diplomatic_influence_with", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(trigger::payload(tval[2]).signed_value), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_diplomatic_influence_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "diplomatic_influence_with", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(trigger::payload(tval[1]).signed_value), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_diplomatic_influence_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "diplomatic_influence_with", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(trigger::payload(tval[1]).signed_value), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_diplomatic_influence_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "diplomatic_influence_with", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(trigger::payload(tval[1]).signed_value), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_diplomatic_influence_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "diplomatic_influence_with", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(trigger::payload(tval[1]).signed_value), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_relation(EFFECT_DISPLAY_PARAMS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(target) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::localised_format_box(ws, layout, box, "relations_with", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(trigger::payload(tval[2]).signed_value), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_relation_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "relations_with", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(trigger::payload(tval[1]).signed_value), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_relation_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "relations_with", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(trigger::payload(tval[1]).signed_value), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_relation_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "relations_with", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(trigger::payload(tval[1]).signed_value), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_relation_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "relations_with", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(trigger::payload(tval[1]).signed_value), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_province_modifier(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id));
		text::add_to_substitution_map(m, text::variable_type::date, ws.current_date + trigger::payload(tval[2]).signed_value);
		text::localised_format_box(ws, layout, box, "add_modifier_until", m);
		text::close_layout_box(layout, box);
	}
	modifier_description(ws, layout, trigger::payload(tval[1]).mod_id, indentation + indentation_amount);
	return 0;
}
uint32_t ef_add_province_modifier_no_duration(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id));
		text::localised_format_box(ws, layout, box, "add_modifier", m);
		text::close_layout_box(layout, box);
	}
	modifier_description(ws, layout, trigger::payload(tval[1]).mod_id, indentation + indentation_amount);
	return 0;
}
uint32_t ef_add_country_modifier(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id));
		text::add_to_substitution_map(m, text::variable_type::date, ws.current_date + trigger::payload(tval[2]).signed_value);
		text::localised_format_box(ws, layout, box, "add_modifier_until", m);
		text::close_layout_box(layout, box);
	}
	modifier_description(ws, layout, trigger::payload(tval[1]).mod_id, indentation + indentation_amount);
	return 0;
}
uint32_t ef_add_country_modifier_no_duration(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id));
		text::localised_format_box(ws, layout, box, "add_modifier", m);
		text::close_layout_box(layout, box);
	}
	modifier_description(ws, layout, trigger::payload(tval[1]).mod_id, indentation + indentation_amount);
	return 0;
}
uint32_t ef_casus_belli_tag(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	auto tag_target = trigger::payload(tval[3]).tag_id;

	auto target = ws.world.national_identity_get_nation_from_identity_holder(tag_target);
	if(target) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::add_to_substitution_map(m, text::variable_type::date, ws.current_date + 31 * months);
		text::localised_format_box(ws, layout, box, months != 0 ? "add_cb_months" : "add_cb_no_months", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_casus_belli_int(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	if(auto holder = ws.world.province_get_nation_from_province_ownership(trigger::payload(tval[3]).prov_id); holder) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, holder);
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::add_to_substitution_map(m, text::variable_type::date, ws.current_date + 31 * months);
		text::localised_format_box(ws, layout, box, months != 0 ? "add_cb_months" : "add_cb_no_months", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_casus_belli_this_nation(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::add_to_substitution_map(m, text::variable_type::date, ws.current_date + 31 * months);
		text::localised_format_box(ws, layout, box, months != 0 ? "add_cb_months" : "add_cb_no_months", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_casus_belli_this_state(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_state(this_slot)), "this_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::add_to_substitution_map(m, text::variable_type::date, ws.current_date + 31 * months);
		text::localised_format_box(ws, layout, box, months != 0 ? "add_cb_months" : "add_cb_no_months", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_casus_belli_this_province(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::add_to_substitution_map(m, text::variable_type::date, ws.current_date + 31 * months);
		text::localised_format_box(ws, layout, box, months != 0 ? "add_cb_months" : "add_cb_no_months", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_casus_belli_this_pop(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_pop(this_slot)), "this_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::add_to_substitution_map(m, text::variable_type::date, ws.current_date + 31 * months);
		text::localised_format_box(ws, layout, box, months != 0 ? "add_cb_months" : "add_cb_no_months", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_casus_belli_from_nation(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::add_to_substitution_map(m, text::variable_type::date, ws.current_date + 31 * months);
		text::localised_format_box(ws, layout, box, months != 0 ? "add_cb_months" : "add_cb_no_months", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_casus_belli_from_province(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::add_to_substitution_map(m, text::variable_type::date, ws.current_date + 31 * months);
		text::localised_format_box(ws, layout, box, months != 0 ? "add_cb_months" : "add_cb_no_months", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_casus_belli_tag(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	auto tag_target = trigger::payload(tval[3]).tag_id;

	auto target = ws.world.national_identity_get_nation_from_identity_holder(tag_target);
	if(target) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::add_to_substitution_map(m, text::variable_type::date, ws.current_date + 31 * months);
		text::localised_format_box(ws, layout, box, months != 0 ? "add_cb_reversed_months" : "add_cb_reversed_no_months", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_casus_belli_int(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	if(auto holder = ws.world.province_get_nation_from_province_ownership(trigger::payload(tval[3]).prov_id); holder) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, holder);
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::add_to_substitution_map(m, text::variable_type::date, ws.current_date + 31 * months);
		text::localised_format_box(ws, layout, box, months != 0 ? "add_cb_reversed_months" : "add_cb_reversed_no_months", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_casus_belli_this_nation(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::add_to_substitution_map(m, text::variable_type::date, ws.current_date + 31 * months);
		text::localised_format_box(ws, layout, box, months != 0 ? "add_cb_reversed_months" : "add_cb_reversed_no_months", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_casus_belli_this_state(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_state(this_slot)), "this_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::add_to_substitution_map(m, text::variable_type::date, ws.current_date + 31 * months);
		text::localised_format_box(ws, layout, box, months != 0 ? "add_cb_reversed_months" : "add_cb_reversed_no_months", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_casus_belli_this_province(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::add_to_substitution_map(m, text::variable_type::date, ws.current_date + 31 * months);
		text::localised_format_box(ws, layout, box, months != 0 ? "add_cb_reversed_months" : "add_cb_reversed_no_months", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_casus_belli_this_pop(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_pop(this_slot)), "this_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::add_to_substitution_map(m, text::variable_type::date, ws.current_date + 31 * months);
		text::localised_format_box(ws, layout, box, months != 0 ? "add_cb_reversed_months" : "add_cb_reversed_no_months", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_casus_belli_from_nation(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::add_to_substitution_map(m, text::variable_type::date, ws.current_date + 31 * months);
		text::localised_format_box(ws, layout, box, months != 0 ? "add_cb_reversed_months" : "add_cb_reversed_no_months", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_casus_belli_from_province(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::add_to_substitution_map(m, text::variable_type::date, ws.current_date + 31 * months);
		text::localised_format_box(ws, layout, box, months != 0 ? "add_cb_reversed_months" : "add_cb_reversed_no_months", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_casus_belli_tag(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto tag_target = trigger::payload(tval[2]).tag_id;

	auto target = ws.world.national_identity_get_nation_from_identity_holder(tag_target);
	if(target) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::localised_format_box(ws, layout, box, "remove_cb", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_casus_belli_int(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	if(auto holder = ws.world.province_get_nation_from_province_ownership(trigger::payload(tval[2]).prov_id); holder) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, holder);
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::localised_format_box(ws, layout, box, "remove_cb", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_casus_belli_this_nation(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::localised_format_box(ws, layout, box, "remove_cb", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_casus_belli_this_state(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_state(this_slot)), "this_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::localised_format_box(ws, layout, box, "remove_cb", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_casus_belli_this_province(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::localised_format_box(ws, layout, box, "remove_cb", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_casus_belli_this_pop(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_pop(this_slot)), "this_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::localised_format_box(ws, layout, box, "remove_cb", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_casus_belli_from_nation(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::localised_format_box(ws, layout, box, "remove_cb", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_remove_casus_belli_from_province(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::localised_format_box(ws, layout, box, "remove_cb", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_this_remove_casus_belli_tag(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto tag_target = trigger::payload(tval[2]).tag_id;

	auto target = ws.world.national_identity_get_nation_from_identity_holder(tag_target);
	if(target) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::localised_format_box(ws, layout, box, "remove_cb_reversed", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_this_remove_casus_belli_int(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	if(auto holder = ws.world.province_get_nation_from_province_ownership(trigger::payload(tval[2]).prov_id); holder) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, holder);
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::localised_format_box(ws, layout, box, "remove_cb_reversed", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_this_remove_casus_belli_this_nation(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::localised_format_box(ws, layout, box, "remove_cb_reversed", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_this_remove_casus_belli_this_state(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_state(this_slot)), "this_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::localised_format_box(ws, layout, box, "remove_cb_reversed", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_this_remove_casus_belli_this_province(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::localised_format_box(ws, layout, box, "remove_cb_reversed", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_this_remove_casus_belli_this_pop(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_pop(this_slot)), "this_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::localised_format_box(ws, layout, box, "remove_cb_reversed", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_this_remove_casus_belli_from_nation(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::localised_format_box(ws, layout, box, "remove_cb_reversed", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_this_remove_casus_belli_from_province(EFFECT_DISPLAY_PARAMS) {
	auto type = trigger::payload(tval[1]).cb_id;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::add_to_substitution_map(m, text::variable_type::name, ws.world.cb_type_get_name(type));
		text::localised_format_box(ws, layout, box, "remove_cb_reversed", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}

uint32_t ef_war_tag(EFFECT_DISPLAY_PARAMS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(!target)
		return 0;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::localised_format_box(ws, layout, box, "declare_war_on", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[5]).cb_id));
		text::localised_format_box(ws, layout, box, "attacker_wg_label", m);
		text::close_layout_box(layout, box);
	}
	if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[6]).prov_id); si) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, si);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(auto t = trigger::payload(tval[7]).tag_id; t) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, t);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(trigger::payload(tval[2]).cb_id) {
		{
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[2]).cb_id));
			text::localised_format_box(ws, layout, box, "defender_wg_label", m);
			text::close_layout_box(layout, box);
		}
		if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[3]).prov_id); si) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, si);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
		if(auto t = trigger::payload(tval[4]).tag_id; t) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, t);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
	}
	return 0;
}
uint32_t ef_war_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "declare_war_on", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[4]).cb_id));
		text::localised_format_box(ws, layout, box, "attacker_wg_label", m);
		text::close_layout_box(layout, box);
	}
	if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[5]).prov_id); si) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, si);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(auto t = trigger::payload(tval[6]).tag_id; t) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, t);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(trigger::payload(tval[1]).cb_id) {
		{
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[1]).cb_id));
			text::localised_format_box(ws, layout, box, "defender_wg_label", m);
			text::close_layout_box(layout, box);
		}
		if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[2]).prov_id); si) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, si);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
		if(auto t = trigger::payload(tval[3]).tag_id; t) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, t);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
	}
	return 0;
}
uint32_t ef_war_this_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_state(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "declare_war_on", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[4]).cb_id));
		text::localised_format_box(ws, layout, box, "attacker_wg_label", m);
		text::close_layout_box(layout, box);
	}
	if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[5]).prov_id); si) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, si);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(auto t = trigger::payload(tval[6]).tag_id; t) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, t);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(trigger::payload(tval[1]).cb_id) {
		{
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[1]).cb_id));
			text::localised_format_box(ws, layout, box, "defender_wg_label", m);
			text::close_layout_box(layout, box);
		}
		if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[2]).prov_id); si) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, si);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
		if(auto t = trigger::payload(tval[3]).tag_id; t) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, t);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
	}
	return 0;
}
uint32_t ef_war_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "declare_war_on", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[4]).cb_id));
		text::localised_format_box(ws, layout, box, "attacker_wg_label", m);
		text::close_layout_box(layout, box);
	}
	if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[5]).prov_id); si) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, si);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(auto t = trigger::payload(tval[6]).tag_id; t) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, t);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(trigger::payload(tval[1]).cb_id) {
		{
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[1]).cb_id));
			text::localised_format_box(ws, layout, box, "defender_wg_label", m);
			text::close_layout_box(layout, box);
		}
		if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[2]).prov_id); si) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, si);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
		if(auto t = trigger::payload(tval[3]).tag_id; t) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, t);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
	}
	return 0;
}
uint32_t ef_war_this_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_pop(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "declare_war_on", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[4]).cb_id));
		text::localised_format_box(ws, layout, box, "attacker_wg_label", m);
		text::close_layout_box(layout, box);
	}
	if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[5]).prov_id); si) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, si);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(auto t = trigger::payload(tval[6]).tag_id; t) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, t);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(trigger::payload(tval[1]).cb_id) {
		{
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[1]).cb_id));
			text::localised_format_box(ws, layout, box, "defender_wg_label", m);
			text::close_layout_box(layout, box);
		}
		if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[2]).prov_id); si) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, si);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
		if(auto t = trigger::payload(tval[3]).tag_id; t) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, t);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
	}
	return 0;
}
uint32_t ef_war_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "declare_war_on", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[4]).cb_id));
		text::localised_format_box(ws, layout, box, "attacker_wg_label", m);
		text::close_layout_box(layout, box);
	}
	if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[5]).prov_id); si) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, si);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(auto t = trigger::payload(tval[6]).tag_id; t) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, t);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(trigger::payload(tval[1]).cb_id) {
		{
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[1]).cb_id));
			text::localised_format_box(ws, layout, box, "defender_wg_label", m);
			text::close_layout_box(layout, box);
		}
		if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[2]).prov_id); si) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, si);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
		if(auto t = trigger::payload(tval[3]).tag_id; t) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, t);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
	}
	return 0;
}
uint32_t ef_war_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "declare_war_on", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[4]).cb_id));
		text::localised_format_box(ws, layout, box, "attacker_wg_label", m);
		text::close_layout_box(layout, box);
	}
	if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[5]).prov_id); si) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, si);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(auto t = trigger::payload(tval[6]).tag_id; t) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, t);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(trigger::payload(tval[1]).cb_id) {
		{
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[1]).cb_id));
			text::localised_format_box(ws, layout, box, "defender_wg_label", m);
			text::close_layout_box(layout, box);
		}
		if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[2]).prov_id); si) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, si);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
		if(auto t = trigger::payload(tval[3]).tag_id; t) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, t);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
	}
	return 0;
}
uint32_t ef_war_no_ally_tag(EFFECT_DISPLAY_PARAMS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(!target)
		return 0;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::localised_format_box(ws, layout, box, "declare_war_on", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "attacker_no_allies", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[5]).cb_id));
		text::localised_format_box(ws, layout, box, "attacker_wg_label", m);
		text::close_layout_box(layout, box);
	}
	if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[6]).prov_id); si) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, si);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(auto t = trigger::payload(tval[7]).tag_id; t) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, t);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(trigger::payload(tval[2]).cb_id) {
		{
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[2]).cb_id));
			text::localised_format_box(ws, layout, box, "defender_wg_label", m);
			text::close_layout_box(layout, box);
		}
		if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[3]).prov_id); si) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, si);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
		if(auto t = trigger::payload(tval[4]).tag_id; t) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, t);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
	}
	return 0;
}
uint32_t ef_war_no_ally_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "declare_war_on", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "attacker_no_allies", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[4]).cb_id));
		text::localised_format_box(ws, layout, box, "attacker_wg_label", m);
		text::close_layout_box(layout, box);
	}
	if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[5]).prov_id); si) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, si);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(auto t = trigger::payload(tval[6]).tag_id; t) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, t);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(trigger::payload(tval[1]).cb_id) {
		{
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[1]).cb_id));
			text::localised_format_box(ws, layout, box, "attacker_wg_label", m);
			text::close_layout_box(layout, box);
		}
		if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[2]).prov_id); si) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, si);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
		if(auto t = trigger::payload(tval[3]).tag_id; t) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, t);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
	}
	return 0;
}
uint32_t ef_war_no_ally_this_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_state(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "declare_war_on", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "attacker_no_allies", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[4]).cb_id));
		text::localised_format_box(ws, layout, box, "attacker_wg_label", m);
		text::close_layout_box(layout, box);
	}
	if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[5]).prov_id); si) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, si);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(auto t = trigger::payload(tval[6]).tag_id; t) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, t);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(trigger::payload(tval[1]).cb_id) {
		{
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[1]).cb_id));
			text::localised_format_box(ws, layout, box, "defender_wg_label", m);
			text::close_layout_box(layout, box);
		}
		if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[2]).prov_id); si) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, si);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
		if(auto t = trigger::payload(tval[3]).tag_id; t) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, t);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
	}
	return 0;
}
uint32_t ef_war_no_ally_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "declare_war_on", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "attacker_no_allies", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[4]).cb_id));
		text::localised_format_box(ws, layout, box, "attacker_wg_label", m);
		text::close_layout_box(layout, box);
	}
	if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[5]).prov_id); si) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, si);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(auto t = trigger::payload(tval[6]).tag_id; t) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, t);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(trigger::payload(tval[1]).cb_id) {
		{
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[1]).cb_id));
			text::localised_format_box(ws, layout, box, "defender_wg_label", m);
			text::close_layout_box(layout, box);
		}
		if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[2]).prov_id); si) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, si);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
		if(auto t = trigger::payload(tval[3]).tag_id; t) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, t);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
	}
	return 0;
}
uint32_t ef_war_no_ally_this_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_pop(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "declare_war_on", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "attacker_no_allies", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[4]).cb_id));
		text::localised_format_box(ws, layout, box, "attacker_wg_label", m);
		text::close_layout_box(layout, box);
	}
	if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[5]).prov_id); si) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, si);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(auto t = trigger::payload(tval[6]).tag_id; t) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, t);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(trigger::payload(tval[1]).cb_id) {
		{
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[1]).cb_id));
			text::localised_format_box(ws, layout, box, "defender_wg_label", m);
			text::close_layout_box(layout, box);
		}
		if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[2]).prov_id); si) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, si);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
		if(auto t = trigger::payload(tval[3]).tag_id; t) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, t);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
	}
	return 0;
}
uint32_t ef_war_no_ally_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "declare_war_on", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "attacker_no_allies", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[4]).cb_id));
		text::localised_format_box(ws, layout, box, "attacker_wg_label", m);
		text::close_layout_box(layout, box);
	}
	if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[5]).prov_id); si) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, si);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(auto t = trigger::payload(tval[6]).tag_id; t) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, t);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(trigger::payload(tval[1]).cb_id) {
		{
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[1]).cb_id));
			text::localised_format_box(ws, layout, box, "defender_wg_label", m);
			text::close_layout_box(layout, box);
		}
		if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[2]).prov_id); si) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, si);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
		if(auto t = trigger::payload(tval[3]).tag_id; t) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, t);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
	}
	return 0;
}
uint32_t ef_war_no_ally_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "declare_war_on", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "attacker_no_allies", m);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[4]).cb_id));
		text::localised_format_box(ws, layout, box, "attacker_wg_label", m);
		text::close_layout_box(layout, box);
	}
	if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[5]).prov_id); si) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, si);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(auto t = trigger::payload(tval[6]).tag_id; t) {
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, t);
		text::localised_format_box(ws, layout, box, "for_text", m);
		text::close_layout_box(layout, box);
	}
	if(trigger::payload(tval[1]).cb_id) {
		{
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[1]).cb_id));
			text::localised_format_box(ws, layout, box, "defender_wg_label", m);
			text::close_layout_box(layout, box);
		}
		if(auto si = ws.world.province_get_state_membership(trigger::payload(tval[2]).prov_id); si) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, si);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
		if(auto t = trigger::payload(tval[3]).tag_id; t) {
			auto box = text::open_layout_box(layout, indentation + indentation_amount);
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::text, t);
			text::localised_format_box(ws, layout, box, "for_text", m);
			text::close_layout_box(layout, box);
		}
	}
	return 0;
}
uint32_t ef_country_event_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.national_event_get_name(trigger::payload(tval[1]).nev_id));
		populate_event_submap(ws, m, event::pending_human_n_event{r_lo, r_hi + 1, primary_slot, this_slot, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date, event::slot_type::nation, event::slot_type::nation});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_country_event_immediate_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.national_event_get_name(trigger::payload(tval[1]).nev_id));
		populate_event_submap(ws, m, event::pending_human_n_event {r_lo, r_hi + 1, primary_slot, this_slot, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date, event::slot_type::nation, event::slot_type::nation});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_province_event_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.provincial_event_get_name(trigger::payload(tval[1]).pev_id));

		populate_event_submap(ws, m, event::pending_human_p_event {r_lo, r_hi + 1,
			this_slot, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), ws.current_date, event::slot_type::nation});

		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_province_event_immediate_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.provincial_event_get_name(trigger::payload(tval[1]).pev_id));
		populate_event_submap(ws, m, event::pending_human_p_event {r_lo, r_hi + 1,
			this_slot, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), ws.current_date, event::slot_type::nation});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_country_event_this_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.national_event_get_name(trigger::payload(tval[1]).nev_id));
		populate_event_submap(ws, m, event::pending_human_n_event {r_lo, r_hi + 1, primary_slot, this_slot, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date, event::slot_type::nation, event::slot_type::state});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_country_event_immediate_this_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.national_event_get_name(trigger::payload(tval[1]).nev_id));
		populate_event_submap(ws, m, event::pending_human_n_event {r_lo, r_hi + 1, primary_slot, this_slot, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date, event::slot_type::nation, event::slot_type::state});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_province_event_this_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.provincial_event_get_name(trigger::payload(tval[1]).pev_id));
		populate_event_submap(ws, m, event::pending_human_p_event {r_lo, r_hi + 1,
			this_slot, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), ws.current_date, event::slot_type::state});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_province_event_immediate_this_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.provincial_event_get_name(trigger::payload(tval[1]).pev_id));
		populate_event_submap(ws, m, event::pending_human_p_event {r_lo, r_hi + 1,
			this_slot, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), ws.current_date, event::slot_type::state});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_country_event_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.national_event_get_name(trigger::payload(tval[1]).nev_id));
		populate_event_submap(ws, m, event::pending_human_n_event {r_lo, r_hi + 1, primary_slot, this_slot, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date, event::slot_type::nation, event::slot_type::province});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_country_event_immediate_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.national_event_get_name(trigger::payload(tval[1]).nev_id));
		populate_event_submap(ws, m, event::pending_human_n_event {r_lo, r_hi + 1, primary_slot, this_slot, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date, event::slot_type::nation, event::slot_type::province});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_province_event_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.provincial_event_get_name(trigger::payload(tval[1]).pev_id));
		populate_event_submap(ws, m, event::pending_human_p_event {r_lo, r_hi + 1,
			this_slot, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), ws.current_date, event::slot_type::province});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_province_event_immediate_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.provincial_event_get_name(trigger::payload(tval[1]).pev_id));
		populate_event_submap(ws, m, event::pending_human_p_event {r_lo, r_hi + 1,
			this_slot, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), ws.current_date, event::slot_type::province});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_country_event_this_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.national_event_get_name(trigger::payload(tval[1]).nev_id));
		populate_event_submap(ws, m, event::pending_human_n_event {r_lo, r_hi + 1, primary_slot, this_slot, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date, event::slot_type::nation, event::slot_type::pop});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_country_event_immediate_this_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.national_event_get_name(trigger::payload(tval[1]).nev_id));
		populate_event_submap(ws, m, event::pending_human_n_event {r_lo, r_hi + 1, primary_slot, this_slot, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date, event::slot_type::nation, event::slot_type::pop});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_province_event_this_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.provincial_event_get_name(trigger::payload(tval[1]).pev_id));
		populate_event_submap(ws, m, event::pending_human_p_event {r_lo, r_hi + 1,
			this_slot, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), ws.current_date, event::slot_type::pop});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_province_event_immediate_this_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.provincial_event_get_name(trigger::payload(tval[1]).pev_id));
		populate_event_submap(ws, m, event::pending_human_p_event {r_lo, r_hi + 1,
			this_slot, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), ws.current_date, event::slot_type::pop});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_country_event_province_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.national_event_get_name(trigger::payload(tval[1]).nev_id));
		populate_event_submap(ws, m, event::pending_human_n_event {r_lo, r_hi + 1,
			trigger::to_generic(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot))),
			this_slot, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date, event::slot_type::nation, event::slot_type::nation});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_country_event_immediate_province_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.national_event_get_name(trigger::payload(tval[1]).nev_id));
		populate_event_submap(ws, m, event::pending_human_n_event {r_lo, r_hi + 1,
			trigger::to_generic(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot))),
			this_slot, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date, event::slot_type::nation, event::slot_type::nation});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_country_event_province_this_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.provincial_event_get_name(trigger::payload(tval[1]).pev_id));
		populate_event_submap(ws, m, event::pending_human_n_event {r_lo, r_hi + 1,
			trigger::to_generic(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot))),
			this_slot, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date, event::slot_type::nation, event::slot_type::state});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_country_event_immediate_province_this_state(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.provincial_event_get_name(trigger::payload(tval[1]).pev_id));
		populate_event_submap(ws, m, event::pending_human_n_event {r_lo, r_hi + 1,
			trigger::to_generic(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot))),
			this_slot, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date, event::slot_type::nation, event::slot_type::state});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_country_event_province_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.national_event_get_name(trigger::payload(tval[1]).nev_id));
		populate_event_submap(ws, m, event::pending_human_n_event {r_lo, r_hi + 1,
			trigger::to_generic(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot))),
			this_slot, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date, event::slot_type::nation, event::slot_type::province});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_country_event_immediate_province_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.national_event_get_name(trigger::payload(tval[1]).nev_id));
		populate_event_submap(ws, m, event::pending_human_n_event {r_lo, r_hi + 1,
			trigger::to_generic(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot))),
			this_slot, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date, event::slot_type::nation, event::slot_type::province});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_country_event_province_this_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.provincial_event_get_name(trigger::payload(tval[1]).pev_id));
		populate_event_submap(ws, m, event::pending_human_n_event {r_lo, r_hi + 1,
			trigger::to_generic(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot))),
			this_slot, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date, event::slot_type::nation, event::slot_type::pop});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_country_event_immediate_province_this_pop(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.world.provincial_event_get_name(trigger::payload(tval[1]).pev_id));
		populate_event_submap(ws, m, event::pending_human_n_event {r_lo, r_hi + 1,
			trigger::to_generic(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot))),
			this_slot, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date, event::slot_type::nation, event::slot_type::pop});
		text::localised_format_box(ws, layout, box, "event_fires", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}

uint32_t ef_sub_unit_int(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "no_effect", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_sub_unit_this(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "no_effect", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_sub_unit_from(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "no_effect", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_sub_unit_current(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "no_effect", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_set_variable(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.national_definitions.variable_names[trigger::payload(tval[1]).natv_id]);
		text::add_to_substitution_map(m, text::variable_type::value, text::fp_one_place{trigger::read_float_from_payload(tval + 2)});
		text::localised_format_box(ws, layout, box, "set_national_variable_to", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_change_variable(EFFECT_DISPLAY_PARAMS) {
	auto amount = trigger::read_float_from_payload(tval + 2);
	if(amount >= 0) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.national_definitions.variable_names[trigger::payload(tval[1]).natv_id]);
		text::add_to_substitution_map(m, text::variable_type::value, text::fp_one_place{amount});
		text::localised_format_box(ws, layout, box, "increase_national_variable_by", m);
		text::close_layout_box(layout, box);
	} else {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.national_definitions.variable_names[trigger::payload(tval[1]).natv_id]);
		text::add_to_substitution_map(m, text::variable_type::value, text::fp_one_place{-amount});
		text::localised_format_box(ws, layout, box, "decrease_national_variable_by", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_ideology(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id));
		text::localised_format_box(ws, layout, box, "support_for_blank", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(text::fp_percentage{trigger::read_float_from_payload(tval + 2)}, true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_upper_house(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id));
		text::localised_format_box(ws, layout, box, "uh_support_for_blank", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(text::fp_percentage{trigger::read_float_from_payload(tval + 2)}, true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_militancy_issue(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		std::string t = text::produce_simple_string(ws, "militancy");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 2)}, false, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id));
		text::localised_format_box(ws, layout, box, "scaled_support", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_militancy_ideology(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		std::string t = text::produce_simple_string(ws, "militancy");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 2)}, false, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id));
		text::localised_format_box(ws, layout, box, "scaled_support", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_militancy_unemployment(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		std::string t = text::produce_simple_string(ws, "militancy");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, false, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::localised_format_box(ws, layout, box, "scaled_unemployment");
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_consciousness_issue(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		std::string t = text::produce_simple_string(ws, "consciousness");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 2)}, true, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id));
		text::localised_format_box(ws, layout, box, "scaled_support", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_consciousness_ideology(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		std::string t = text::produce_simple_string(ws, "consciousness");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 2)}, true, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id));
		text::localised_format_box(ws, layout, box, "scaled_support", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_consciousness_unemployment(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		std::string t = text::produce_simple_string(ws, "consciousness");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, true, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::localised_format_box(ws, layout, box, "scaled_unemployment");
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_militancy_nation_issue(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		std::string t = text::produce_simple_string(ws, "militancy");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 2)}, false, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id));
		text::localised_format_box(ws, layout, box, "scaled_support", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_militancy_nation_ideology(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		std::string t = text::produce_simple_string(ws, "militancy");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 2)}, false, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id));
		text::localised_format_box(ws, layout, box, "scaled_support", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_militancy_nation_unemployment(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		std::string t = text::produce_simple_string(ws, "militancy");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, false, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::localised_format_box(ws, layout, box, "scaled_unemployment");
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_consciousness_nation_issue(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		std::string t = text::produce_simple_string(ws, "consciousness");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 2)}, true, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id));
		text::localised_format_box(ws, layout, box, "scaled_support", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_consciousness_nation_ideology(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		std::string t = text::produce_simple_string(ws, "consciousness");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 2)}, true, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id));
		text::localised_format_box(ws, layout, box, "scaled_support", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_consciousness_nation_unemployment(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		std::string t = text::produce_simple_string(ws, "consciousness");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, true, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::localised_format_box(ws, layout, box, "scaled_unemployment");
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_militancy_state_issue(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		std::string t = text::produce_simple_string(ws, "militancy");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 2)}, false, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id));
		text::localised_format_box(ws, layout, box, "scaled_support", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_militancy_state_ideology(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		std::string t = text::produce_simple_string(ws, "militancy");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 2)}, false, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id));
		text::localised_format_box(ws, layout, box, "scaled_support", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_militancy_state_unemployment(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		std::string t = text::produce_simple_string(ws, "militancy");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, false, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::localised_format_box(ws, layout, box, "scaled_unemployment");
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_consciousness_state_issue(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		std::string t = text::produce_simple_string(ws, "consciousness");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 2)}, true, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id));
		text::localised_format_box(ws, layout, box, "scaled_support", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_consciousness_state_ideology(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		std::string t = text::produce_simple_string(ws, "consciousness");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 2)}, true, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id));
		text::localised_format_box(ws, layout, box, "scaled_support", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_consciousness_state_unemployment(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		std::string t = text::produce_simple_string(ws, "consciousness");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, true, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::localised_format_box(ws, layout, box, "scaled_unemployment");
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_militancy_province_issue(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		std::string t = text::produce_simple_string(ws, "militancy");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 2)}, false, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id));
		text::localised_format_box(ws, layout, box, "scaled_support", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_militancy_province_ideology(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		std::string t = text::produce_simple_string(ws, "militancy");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 2)}, false, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id));
		text::localised_format_box(ws, layout, box, "scaled_support", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_militancy_province_unemployment(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		std::string t = text::produce_simple_string(ws, "militancy");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, false, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::localised_format_box(ws, layout, box, "scaled_unemployment");
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_consciousness_province_issue(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		std::string t = text::produce_simple_string(ws, "consciousness");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 2)}, true, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id));
		text::localised_format_box(ws, layout, box, "scaled_support", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_consciousness_province_ideology(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		std::string t = text::produce_simple_string(ws, "consciousness");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 2)}, true, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id));
		text::localised_format_box(ws, layout, box, "scaled_support", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_scaled_consciousness_province_unemployment(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		std::string t = text::produce_simple_string(ws, "consciousness");
		display_value(text::fp_one_place{trigger::read_float_from_payload(tval + 1)}, true, ws, layout, box);
		text::add_space_to_layout_box(ws, layout, box);
		text::add_to_layout_box(ws, layout, box, t);
		text::localised_format_box(ws, layout, box, "scaled_unemployment");
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_variable_good_name(EFFECT_DISPLAY_PARAMS) {
	{
		auto amount = trigger::read_float_from_payload(tval + 2);
		auto box = text::open_layout_box(layout, indentation);
		display_value(text::fp_one_place{amount}, true, ws, layout, box);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.commodity_get_name(trigger::payload(tval[1]).com_id));
		text::localised_format_box(ws, layout, box, "stockpile_of", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_variable_good_name_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto amount = trigger::read_float_from_payload(tval + 2);
		auto box = text::open_layout_box(layout, indentation);
		display_value(text::fp_one_place{amount}, true, ws, layout, box);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.commodity_get_name(trigger::payload(tval[1]).com_id));
		text::localised_format_box(ws, layout, box, "stockpile_of", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_define_general(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "create_general", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_define_admiral(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "create_admiral", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_dominant_issue(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id));
		text::localised_format_box(ws, layout, box, "support_for_blank", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(text::fp_percentage{trigger::read_float_from_payload(tval + 2)}, true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_dominant_issue_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id));
		text::localised_format_box(ws, layout, box, "support_for_blank", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(text::fp_percentage{trigger::read_float_from_payload(tval + 2)}, true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_war_goal(EFFECT_DISPLAY_PARAMS) {
	{
		auto amount = trigger::read_float_from_payload(tval + 2);
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.cb_type_get_name(trigger::payload(tval[1]).cb_id));
		std::string t;
		if(primary_slot != -1)
			text::add_to_substitution_map(m, text::variable_type::nation, trigger::to_nation(primary_slot));
		else {
			t = text::produce_simple_string(ws, "this_nation");
			text::add_to_substitution_map(m, text::variable_type::nation, std::string_view{t});
		}
		text::localised_format_box(ws, layout, box, "add_war_goal", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_move_issue_percentage_nation(EFFECT_DISPLAY_PARAMS) {
	auto amount = trigger::read_float_from_payload(tval + 3);
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id));
		text::localised_format_box(ws, layout, box, "support_for_blank", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(text::fp_percentage{-amount}, true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.issue_option_get_name(trigger::payload(tval[2]).opt_id));
		text::localised_format_box(ws, layout, box, "support_for_blank", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(text::fp_percentage{amount}, true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_move_issue_percentage_state(EFFECT_DISPLAY_PARAMS) {
	auto amount = trigger::read_float_from_payload(tval + 3);
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id));
		text::localised_format_box(ws, layout, box, "support_for_blank", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(text::fp_percentage{-amount}, true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.issue_option_get_name(trigger::payload(tval[2]).opt_id));
		text::localised_format_box(ws, layout, box, "support_for_blank", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(text::fp_percentage{amount}, true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_move_issue_percentage_province(EFFECT_DISPLAY_PARAMS) {
	auto amount = trigger::read_float_from_payload(tval + 3);
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id));
		text::localised_format_box(ws, layout, box, "support_for_blank", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(text::fp_percentage{-amount}, true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.issue_option_get_name(trigger::payload(tval[2]).opt_id));
		text::localised_format_box(ws, layout, box, "support_for_blank", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(text::fp_percentage{amount}, true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_move_issue_percentage_pop(EFFECT_DISPLAY_PARAMS) {
	auto amount = trigger::read_float_from_payload(tval + 3);
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.issue_option_get_name(trigger::payload(tval[1]).opt_id));
		text::localised_format_box(ws, layout, box, "support_for_blank", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(text::fp_percentage{-amount}, true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.issue_option_get_name(trigger::payload(tval[2]).opt_id));
		text::localised_format_box(ws, layout, box, "support_for_blank", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(text::fp_percentage{amount}, true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_party_loyalty_province(EFFECT_DISPLAY_PARAMS) {
	float amount = float(trigger::payload(tval[2]).signed_value) / 100.0f;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.ideology_get_name(trigger::payload(tval[1]).ideo_id));
		text::localised_format_box(ws, layout, box, "blank_loyalty", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(text::fp_percentage{amount}, true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_party_loyalty(EFFECT_DISPLAY_PARAMS) {
	float amount = float(trigger::payload(tval[3]).signed_value) / 100.0f;
	{
		auto box = text::open_layout_box(layout, indentation);
		text::add_to_layout_box(ws, layout, box, trigger::payload(tval[1]).prov_id);
		text::close_layout_box(layout, box);
	}
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.ideology_get_name(trigger::payload(tval[2]).ideo_id));
		text::localised_format_box(ws, layout, box, "blank_loyalty", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(text::fp_percentage{amount}, true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_build_railway_in_capital_yes_whole_state_yes_limit(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "railroad_in_capital_state", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_build_railway_in_capital_yes_whole_state_no_limit(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "railroad_in_capital_state", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_build_railway_in_capital_no_whole_state_yes_limit(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "railroad_in_capital", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_build_railway_in_capital_no_whole_state_no_limit(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "railroad_in_capital", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_build_fort_in_capital_yes_whole_state_yes_limit(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "fort_in_capital_state", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_build_fort_in_capital_yes_whole_state_no_limit(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "fort_in_capital_state", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_build_fort_in_capital_no_whole_state_yes_limit(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "fort_in_capital", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_build_fort_in_capital_no_whole_state_no_limit(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation + indentation_amount);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "fort_in_capital", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_relation_reb(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_rebel(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "relations_with", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(trigger::payload(tval[1]).signed_value), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_variable_tech_name_yes(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.technology_get_name(trigger::payload(tval[1]).tech_id));
		text::localised_format_box(ws, layout, box, "enable_blank", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_variable_tech_name_no(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.technology_get_name(trigger::payload(tval[1]).tech_id));
		text::localised_format_box(ws, layout, box, "disable_blank", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_variable_invention_name_yes(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.invention_get_name(trigger::payload(tval[1]).invt_id));
		text::localised_format_box(ws, layout, box, "enable_blank", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_variable_invention_name_no(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.invention_get_name(trigger::payload(tval[1]).invt_id));
		text::localised_format_box(ws, layout, box, "disable_blank", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_set_country_flag_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text,
				ws.national_definitions.flag_variable_names[trigger::payload(tval[1]).natf_id]);
		text::localised_format_box(ws, layout, box, "o_set_national_flag", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_add_country_modifier_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id));
		text::add_to_substitution_map(m, text::variable_type::date, ws.current_date + trigger::payload(tval[2]).signed_value);
		text::localised_format_box(ws, layout, box, "o_add_modifier_until", m);
		text::close_layout_box(layout, box);
	}
	modifier_description(ws, layout, trigger::payload(tval[1]).mod_id, indentation + indentation_amount);
	return 0;
}
uint32_t ef_add_country_modifier_province_no_duration(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, ws.world.modifier_get_name(trigger::payload(tval[1]).mod_id));
		text::localised_format_box(ws, layout, box, "o_add_modifier", m);
		text::close_layout_box(layout, box);
	}
	modifier_description(ws, layout, trigger::payload(tval[1]).mod_id, indentation + indentation_amount);
	return 0;
}
uint32_t ef_relation_province(EFFECT_DISPLAY_PARAMS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(target) {
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::text, target);
		text::localised_format_box(ws, layout, box, "o_relations_with", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(trigger::payload(tval[2]).signed_value), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_relation_province_this_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "o_relations_with", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(trigger::payload(tval[1]).signed_value), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_relation_province_this_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(this_slot)), "this_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "o_relations_with", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(trigger::payload(tval[1]).signed_value), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_relation_province_from_nation(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_nation(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "o_relations_with", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(trigger::payload(tval[1]).signed_value), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_relation_province_from_province(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_prov(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "o_relations_with", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(trigger::payload(tval[1]).signed_value), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_relation_province_reb(EFFECT_DISPLAY_PARAMS) {
	{
		auto box = text::open_layout_box(layout, indentation);
		text::substitution_map m;
		add_to_map(ws, m, convert_this(ws, trigger::to_rebel(from_slot)), "from_nation", [](auto x) { return x; });
		text::localised_format_box(ws, layout, box, "o_relations_with", m);
		text::add_space_to_layout_box(ws, layout, box);
		display_value(int64_t(trigger::payload(tval[1]).signed_value), true, ws, layout, box);
		text::close_layout_box(layout, box);
	}
	return 0;
}
uint32_t ef_treasury_province(EFFECT_DISPLAY_PARAMS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	{
		auto box = text::open_layout_box(layout, indentation);
		display_value(text::fp_currency{amount}, true, ws, layout, box);
		text::substitution_map m;
		text::localised_format_box(ws, layout, box, "o_add_to_treasury", m);
		text::close_layout_box(layout, box);
	}
	return 0;
}
inline constexpr uint32_t (*effect_functions[])(EFFECT_DISPLAY_PARAMS) = {
		ef_none,
		ef_capital,																// constexpr inline uint16_t capital = 0x0001;
		ef_add_core_tag,													// constexpr inline uint16_t add_core_tag = 0x0002;
		ef_add_core_int,													// constexpr inline uint16_t add_core_int = 0x0003;
		ef_add_core_this_nation,									// constexpr inline uint16_t add_core_this_nation = 0x0004;
		ef_add_core_this_province,								// constexpr inline uint16_t add_core_this_province = 0x0005;
		ef_add_core_this_state,										// constexpr inline uint16_t add_core_this_state = 0x0006;
		ef_add_core_this_pop,											// constexpr inline uint16_t add_core_this_pop = 0x0007;
		ef_add_core_from_province,								// constexpr inline uint16_t add_core_from_province = 0x0008;
		ef_add_core_from_nation,									// constexpr inline uint16_t add_core_from_nation = 0x0009;
		ef_add_core_reb,													// constexpr inline uint16_t add_core_reb = 0x000A;
		ef_remove_core_tag,												// constexpr inline uint16_t remove_core_tag = 0x000B;
		ef_remove_core_int,												// constexpr inline uint16_t remove_core_int = 0x000C;
		ef_remove_core_this_nation,								// constexpr inline uint16_t remove_core_this_nation = 0x000D;
		ef_remove_core_this_province,							// constexpr inline uint16_t remove_core_this_province = 0x000E;
		ef_remove_core_this_state,								// constexpr inline uint16_t remove_core_this_state = 0x000F;
		ef_remove_core_this_pop,									// constexpr inline uint16_t remove_core_this_pop = 0x0010;
		ef_remove_core_from_province,							// constexpr inline uint16_t remove_core_from_province = 0x0011;
		ef_remove_core_from_nation,								// constexpr inline uint16_t remove_core_from_nation = 0x0012;
		ef_remove_core_reb,												// constexpr inline uint16_t remove_core_reb = 0x0013;
		ef_change_region_name_state,							// constexpr inline uint16_t change_region_name_state = 0x0014;
		ef_change_region_name_province,						// constexpr inline uint16_t change_region_name_province = 0x0015;
		ef_trade_goods,														// constexpr inline uint16_t trade_goods = 0x0016;
		ef_add_accepted_culture,									// constexpr inline uint16_t add_accepted_culture = 0x0017;
		ef_add_accepted_culture_union,						// constexpr inline uint16_t add_accepted_culture_union = 0x0018;
		ef_primary_culture,												// constexpr inline uint16_t primary_culture = 0x0019;
		ef_primary_culture_this_nation,						// constexpr inline uint16_t primary_culture_this_nation = 0x001A;
		ef_primary_culture_this_state,						// constexpr inline uint16_t primary_culture_this_state = 0x001B;
		ef_primary_culture_this_province,					// constexpr inline uint16_t primary_culture_this_province = 0x001C;
		ef_primary_culture_this_pop,							// constexpr inline uint16_t primary_culture_this_pop = 0x001D;
		ef_primary_culture_from_nation,						// constexpr inline uint16_t primary_culture_from_nation = 0x001E;
		ef_remove_accepted_culture,								// constexpr inline uint16_t remove_accepted_culture = 0x001F;
		ef_life_rating,														// constexpr inline uint16_t life_rating = 0x0020;
		ef_religion,															// constexpr inline uint16_t religion = 0x0021;
		ef_is_slave_state_yes,										// constexpr inline uint16_t is_slave_state_yes = 0x0022;
		ef_is_slave_pop_yes,											// constexpr inline uint16_t is_slave_pop_yes = 0x0023;
		ef_research_points,												// constexpr inline uint16_t research_points = 0x0024;
		ef_tech_school,														// constexpr inline uint16_t tech_school = 0x0025;
		ef_government,														// constexpr inline uint16_t government = 0x0026;
		ef_government_reb,												// constexpr inline uint16_t government_reb = 0x0027;
		ef_treasury,															// constexpr inline uint16_t treasury = 0x0028;
		ef_war_exhaustion,												// constexpr inline uint16_t war_exhaustion = 0x0029;
		ef_prestige,															// constexpr inline uint16_t prestige = 0x002A;
		ef_change_tag,														// constexpr inline uint16_t change_tag = 0x002B;
		ef_change_tag_culture,										// constexpr inline uint16_t change_tag_culture = 0x002C;
		ef_change_tag_no_core_switch,							// constexpr inline uint16_t change_tag_no_core_switch = 0x002D;
		ef_change_tag_no_core_switch_culture,			// constexpr inline uint16_t change_tag_no_core_switch_culture = 0x002E;
		ef_set_country_flag,											// constexpr inline uint16_t set_country_flag = 0x002F;
		ef_clr_country_flag,											// constexpr inline uint16_t clr_country_flag = 0x0030;
		ef_military_access,												// constexpr inline uint16_t military_access = 0x0031;
		ef_military_access_this_nation,						// constexpr inline uint16_t military_access_this_nation = 0x0032;
		ef_military_access_this_province,					// constexpr inline uint16_t military_access_this_province = 0x0033;
		ef_military_access_from_nation,						// constexpr inline uint16_t military_access_from_nation = 0x0034;
		ef_military_access_from_province,					// constexpr inline uint16_t military_access_from_province = 0x0035;
		ef_badboy,																// constexpr inline uint16_t badboy = 0x0036;
		ef_secede_province,												// constexpr inline uint16_t secede_province = 0x0037;
		ef_secede_province_this_nation,						// constexpr inline uint16_t secede_province_this_nation = 0x0038;
		ef_secede_province_this_state,						// constexpr inline uint16_t secede_province_this_state = 0x0039;
		ef_secede_province_this_province,					// constexpr inline uint16_t secede_province_this_province = 0x003A;
		ef_secede_province_this_pop,							// constexpr inline uint16_t secede_province_this_pop = 0x003B;
		ef_secede_province_from_nation,						// constexpr inline uint16_t secede_province_from_nation = 0x003C;
		ef_secede_province_from_province,					// constexpr inline uint16_t secede_province_from_province = 0x003D;
		ef_secede_province_reb,										// constexpr inline uint16_t secede_province_reb = 0x003E;
		ef_inherit,																// constexpr inline uint16_t inherit = 0x003F;
		ef_inherit_this_nation,										// constexpr inline uint16_t inherit_this_nation = 0x0040;
		ef_inherit_this_state,										// constexpr inline uint16_t inherit_this_state = 0x0041;
		ef_inherit_this_province,									// constexpr inline uint16_t inherit_this_province = 0x0042;
		ef_inherit_this_pop,											// constexpr inline uint16_t inherit_this_pop = 0x0043;
		ef_inherit_from_nation,										// constexpr inline uint16_t inherit_from_nation = 0x0044;
		ef_inherit_from_province,									// constexpr inline uint16_t inherit_from_province = 0x0045;
		ef_annex_to,															// constexpr inline uint16_t annex_to = 0x0046;
		ef_annex_to_this_nation,									// constexpr inline uint16_t annex_to_this_nation = 0x0047;
		ef_annex_to_this_state,										// constexpr inline uint16_t annex_to_this_state = 0x0048;
		ef_annex_to_this_province,								// constexpr inline uint16_t annex_to_this_province = 0x0049;
		ef_annex_to_this_pop,											// constexpr inline uint16_t annex_to_this_pop = 0x004A;
		ef_annex_to_from_nation,									// constexpr inline uint16_t annex_to_from_nation = 0x004B;
		ef_annex_to_from_province,								// constexpr inline uint16_t annex_to_from_province = 0x004C;
		ef_release,																// constexpr inline uint16_t release = 0x004D;
		ef_release_this_nation,										// constexpr inline uint16_t release_this_nation = 0x004E;
		ef_release_this_state,										// constexpr inline uint16_t release_this_state = 0x004F;
		ef_release_this_province,									// constexpr inline uint16_t release_this_province = 0x0050;
		ef_release_this_pop,											// constexpr inline uint16_t release_this_pop = 0x0051;
		ef_release_from_nation,										// constexpr inline uint16_t release_from_nation = 0x0052;
		ef_release_from_province,									// constexpr inline uint16_t release_from_province = 0x0053;
		ef_change_controller,											// constexpr inline uint16_t change_controller = 0x0054;
		ef_change_controller_this_nation,					// constexpr inline uint16_t change_controller_this_nation = 0x0055;
		ef_change_controller_this_province,				// constexpr inline uint16_t change_controller_this_province = 0x0056;
		ef_change_controller_from_nation,					// constexpr inline uint16_t change_controller_from_nation = 0x0057;
		ef_change_controller_from_province,				// constexpr inline uint16_t change_controller_from_province = 0x0058;
		ef_infrastructure,												// constexpr inline uint16_t infrastructure = 0x0059;
		ef_money,																	// constexpr inline uint16_t money = 0x005A;
		ef_leadership,														// constexpr inline uint16_t leadership = 0x005B;
		ef_create_vassal,													// constexpr inline uint16_t create_vassal = 0x005C;
		ef_create_vassal_this_nation,							// constexpr inline uint16_t create_vassal_this_nation = 0x005D;
		ef_create_vassal_this_province,						// constexpr inline uint16_t create_vassal_this_province = 0x005E;
		ef_create_vassal_from_nation,							// constexpr inline uint16_t create_vassal_from_nation = 0x005F;
		ef_create_vassal_from_province,						// constexpr inline uint16_t create_vassal_from_province = 0x0060;
		ef_end_military_access,										// constexpr inline uint16_t end_military_access = 0x0061;
		ef_end_military_access_this_nation,				// constexpr inline uint16_t end_military_access_this_nation = 0x0062;
		ef_end_military_access_this_province,			// constexpr inline uint16_t end_military_access_this_province = 0x0063;
		ef_end_military_access_from_nation,				// constexpr inline uint16_t end_military_access_from_nation = 0x0064;
		ef_end_military_access_from_province,			// constexpr inline uint16_t end_military_access_from_province = 0x0065;
		ef_leave_alliance,												// constexpr inline uint16_t leave_alliance = 0x0066;
		ef_leave_alliance_this_nation,						// constexpr inline uint16_t leave_alliance_this_nation = 0x0067;
		ef_leave_alliance_this_province,					// constexpr inline uint16_t leave_alliance_this_province = 0x0068;
		ef_leave_alliance_from_nation,						// constexpr inline uint16_t leave_alliance_from_nation = 0x0069;
		ef_leave_alliance_from_province,					// constexpr inline uint16_t leave_alliance_from_province = 0x006A;
		ef_end_war,																// constexpr inline uint16_t end_war = 0x006B;
		ef_end_war_this_nation,										// constexpr inline uint16_t end_war_this_nation = 0x006C;
		ef_end_war_this_province,									// constexpr inline uint16_t end_war_this_province = 0x006D;
		ef_end_war_from_nation,										// constexpr inline uint16_t end_war_from_nation = 0x006E;
		ef_end_war_from_province,									// constexpr inline uint16_t end_war_from_province = 0x006F;
		ef_enable_ideology,												// constexpr inline uint16_t enable_ideology = 0x0070;
		ef_ruling_party_ideology,									// constexpr inline uint16_t ruling_party_ideology = 0x0071;
		ef_plurality,															// constexpr inline uint16_t plurality = 0x0072;
		ef_remove_province_modifier,							// constexpr inline uint16_t remove_province_modifier = 0x0073;
		ef_remove_country_modifier,								// constexpr inline uint16_t remove_country_modifier = 0x0074;
		ef_create_alliance,												// constexpr inline uint16_t create_alliance = 0x0075;
		ef_create_alliance_this_nation,						// constexpr inline uint16_t create_alliance_this_nation = 0x0076;
		ef_create_alliance_this_province,					// constexpr inline uint16_t create_alliance_this_province = 0x0077;
		ef_create_alliance_from_nation,						// constexpr inline uint16_t create_alliance_from_nation = 0x0078;
		ef_create_alliance_from_province,					// constexpr inline uint16_t create_alliance_from_province = 0x0079;
		ef_release_vassal,												// constexpr inline uint16_t release_vassal = 0x007A;
		ef_release_vassal_this_nation,						// constexpr inline uint16_t release_vassal_this_nation = 0x007B;
		ef_release_vassal_this_province,					// constexpr inline uint16_t release_vassal_this_province = 0x007C;
		ef_release_vassal_from_nation,						// constexpr inline uint16_t release_vassal_from_nation = 0x007D;
		ef_release_vassal_from_province,					// constexpr inline uint16_t release_vassal_from_province = 0x007E;
		ef_release_vassal_reb,										// constexpr inline uint16_t release_vassal_reb = 0x007F;
		ef_release_vassal_random,									// constexpr inline uint16_t release_vassal_random = 0x0080;
		ef_change_province_name,									// constexpr inline uint16_t change_province_name = 0x0081;
		ef_enable_canal,													// constexpr inline uint16_t enable_canal = 0x0082;
		ef_set_global_flag,												// constexpr inline uint16_t set_global_flag = 0x0083;
		ef_clr_global_flag,												// constexpr inline uint16_t clr_global_flag = 0x0084;
		ef_nationalvalue_province,								// constexpr inline uint16_t nationalvalue_province = 0x0085;
		ef_nationalvalue_nation,									// constexpr inline uint16_t nationalvalue_nation = 0x0086;
		ef_civilized_yes,													// constexpr inline uint16_t civilized_yes = 0x0087;
		ef_civilized_no,													// constexpr inline uint16_t civilized_no = 0x0088;
		ef_is_slave_state_no,											// constexpr inline uint16_t is_slave_state_no = 0x0089;
		ef_is_slave_pop_no,												// constexpr inline uint16_t is_slave_pop_no = 0x008A;
		ef_election,															// constexpr inline uint16_t election = 0x008B;
		ef_social_reform,													// constexpr inline uint16_t social_reform = 0x008C;
		ef_political_reform,											// constexpr inline uint16_t political_reform = 0x008D;
		ef_add_tax_relative_income,								// constexpr inline uint16_t add_tax_relative_income = 0x008E;
		ef_neutrality,														// constexpr inline uint16_t neutrality = 0x008F;
		ef_reduce_pop,														// constexpr inline uint16_t reduce_pop = 0x0090;
		ef_move_pop,															// constexpr inline uint16_t move_pop = 0x0091;
		ef_pop_type,															// constexpr inline uint16_t pop_type = 0x0092;
		ef_years_of_research,											// constexpr inline uint16_t years_of_research = 0x0093;
		ef_prestige_factor_positive,							// constexpr inline uint16_t prestige_factor_positive = 0x0094;
		ef_prestige_factor_negative,							// constexpr inline uint16_t prestige_factor_negative = 0x0095;
		ef_military_reform,												// constexpr inline uint16_t military_reform = 0x0096;
		ef_economic_reform,												// constexpr inline uint16_t economic_reform = 0x0097;
		ef_remove_random_military_reforms,				// constexpr inline uint16_t remove_random_military_reforms = 0x0098;
		ef_remove_random_economic_reforms,				// constexpr inline uint16_t remove_random_economic_reforms = 0x0099;
		ef_add_crime,															// constexpr inline uint16_t add_crime = 0x009A;
		ef_add_crime_none,												// constexpr inline uint16_t add_crime_none = 0x009B;
		ef_nationalize,														// constexpr inline uint16_t nationalize = 0x009C;
		ef_build_factory_in_capital_state,				// constexpr inline uint16_t build_factory_in_capital_state = 0x009D;
		ef_activate_technology,										// constexpr inline uint16_t activate_technology = 0x009E;
		ef_great_wars_enabled_yes,								// constexpr inline uint16_t great_wars_enabled_yes = 0x009F;
		ef_great_wars_enabled_no,									// constexpr inline uint16_t great_wars_enabled_no = 0x00A0;
		ef_world_wars_enabled_yes,								// constexpr inline uint16_t world_wars_enabled_yes = 0x00A1;
		ef_world_wars_enabled_no,									// constexpr inline uint16_t world_wars_enabled_no = 0x00A2;
		ef_assimilate_province,										// constexpr inline uint16_t assimilate_province = 0x00A3;
		ef_assimilate_pop,												// constexpr inline uint16_t assimilate_pop = 0x00A4;
		ef_literacy,															// constexpr inline uint16_t literacy = 0x00A5;
		ef_add_crisis_interest,										// constexpr inline uint16_t add_crisis_interest = 0x00A6;
		ef_flashpoint_tension,										// constexpr inline uint16_t flashpoint_tension = 0x00A7;
		ef_add_crisis_temperature,								// constexpr inline uint16_t add_crisis_temperature = 0x00A8;
		ef_consciousness,													// constexpr inline uint16_t consciousness = 0x00A9;
		ef_militancy,															// constexpr inline uint16_t militancy = 0x00AA;
		ef_rgo_size,															// constexpr inline uint16_t rgo_size = 0x00AB;
		ef_fort,																	// constexpr inline uint16_t fort = 0x00AC;
		ef_naval_base,														// constexpr inline uint16_t naval_base = 0x00AD;
		ef_trigger_revolt_nation,									// constexpr inline uint16_t trigger_revolt_nation = 0x00AE;
		ef_trigger_revolt_state,									// constexpr inline uint16_t trigger_revolt_state = 0x00AF;
		ef_trigger_revolt_province,								// constexpr inline uint16_t trigger_revolt_province = 0x00B0;
		ef_diplomatic_influence,									// constexpr inline uint16_t diplomatic_influence = 0x00B1;
		ef_diplomatic_influence_this_nation,			// constexpr inline uint16_t diplomatic_influence_this_nation = 0x00B2;
		ef_diplomatic_influence_this_province,		// constexpr inline uint16_t diplomatic_influence_this_province = 0x00B3;
		ef_diplomatic_influence_from_nation,			// constexpr inline uint16_t diplomatic_influence_from_nation = 0x00B4;
		ef_diplomatic_influence_from_province,		// constexpr inline uint16_t diplomatic_influence_from_province = 0x00B5;
		ef_relation,															// constexpr inline uint16_t relation = 0x00B6;
		ef_relation_this_nation,									// constexpr inline uint16_t relation_this_nation = 0x00B7;
		ef_relation_this_province,								// constexpr inline uint16_t relation_this_province = 0x00B8;
		ef_relation_from_nation,									// constexpr inline uint16_t relation_from_nation = 0x00B9;
		ef_relation_from_province,								// constexpr inline uint16_t relation_from_province = 0x00BA;
		ef_add_province_modifier,									// constexpr inline uint16_t add_province_modifier = 0x00BB;
		ef_add_province_modifier_no_duration,			// constexpr inline uint16_t add_province_modifier_no_duration = 0x00BC;
		ef_add_country_modifier,									// constexpr inline uint16_t add_country_modifier = 0x00BD;
		ef_add_country_modifier_no_duration,			// constexpr inline uint16_t add_country_modifier_no_duration = 0x00BE;
		ef_casus_belli_tag,												// constexpr inline uint16_t casus_belli_tag = 0x00BF;
		ef_casus_belli_int,												// constexpr inline uint16_t casus_belli_int = 0x00C0;
		ef_casus_belli_this_nation,								// constexpr inline uint16_t casus_belli_this_nation = 0x00C1;
		ef_casus_belli_this_state,								// constexpr inline uint16_t casus_belli_this_state = 0x00C2;
		ef_casus_belli_this_province,							// constexpr inline uint16_t casus_belli_this_province = 0x00C3;
		ef_casus_belli_this_pop,									// constexpr inline uint16_t casus_belli_this_pop = 0x00C4;
		ef_casus_belli_from_nation,								// constexpr inline uint16_t casus_belli_from_nation = 0x00C5;
		ef_casus_belli_from_province,							// constexpr inline uint16_t casus_belli_from_province = 0x00C6;
		ef_add_casus_belli_tag,										// constexpr inline uint16_t add_casus_belli_tag = 0x00C7;
		ef_add_casus_belli_int,										// constexpr inline uint16_t add_casus_belli_int = 0x00C8;
		ef_add_casus_belli_this_nation,						// constexpr inline uint16_t add_casus_belli_this_nation = 0x00C9;
		ef_add_casus_belli_this_state,						// constexpr inline uint16_t add_casus_belli_this_state = 0x00CA;
		ef_add_casus_belli_this_province,					// constexpr inline uint16_t add_casus_belli_this_province = 0x00CB;
		ef_add_casus_belli_this_pop,							// constexpr inline uint16_t add_casus_belli_this_pop = 0x00CC;
		ef_add_casus_belli_from_nation,						// constexpr inline uint16_t add_casus_belli_from_nation = 0x00CD;
		ef_add_casus_belli_from_province,					// constexpr inline uint16_t add_casus_belli_from_province = 0x00CE;
		ef_remove_casus_belli_tag,								// constexpr inline uint16_t remove_casus_belli_tag = 0x00CF;
		ef_remove_casus_belli_int,								// constexpr inline uint16_t remove_casus_belli_int = 0x00D0;
		ef_remove_casus_belli_this_nation,				// constexpr inline uint16_t remove_casus_belli_this_nation = 0x00D1;
		ef_remove_casus_belli_this_state,					// constexpr inline uint16_t remove_casus_belli_this_state = 0x00D2;
		ef_remove_casus_belli_this_province,			// constexpr inline uint16_t remove_casus_belli_this_province = 0x00D3;
		ef_remove_casus_belli_this_pop,						// constexpr inline uint16_t remove_casus_belli_this_pop = 0x00D4;
		ef_remove_casus_belli_from_nation,				// constexpr inline uint16_t remove_casus_belli_from_nation = 0x00D5;
		ef_remove_casus_belli_from_province,			// constexpr inline uint16_t remove_casus_belli_from_province = 0x00D6;
		ef_this_remove_casus_belli_tag,						// constexpr inline uint16_t this_remove_casus_belli_tag = 0x00D7;
		ef_this_remove_casus_belli_int,						// constexpr inline uint16_t this_remove_casus_belli_int = 0x00D8;
		ef_this_remove_casus_belli_this_nation,		// constexpr inline uint16_t this_remove_casus_belli_this_nation = 0x00D9;
		ef_this_remove_casus_belli_this_state,		// constexpr inline uint16_t this_remove_casus_belli_this_state = 0x00DA;
		ef_this_remove_casus_belli_this_province, // constexpr inline uint16_t this_remove_casus_belli_this_province = 0x00DB;
		ef_this_remove_casus_belli_this_pop,			// constexpr inline uint16_t this_remove_casus_belli_this_pop = 0x00DC;
		ef_this_remove_casus_belli_from_nation,		// constexpr inline uint16_t this_remove_casus_belli_from_nation = 0x00DD;
		ef_this_remove_casus_belli_from_province, // constexpr inline uint16_t this_remove_casus_belli_from_province = 0x00DE;
		ef_war_tag,																// constexpr inline uint16_t war_tag = 0x00DF;
		ef_war_this_nation,												// constexpr inline uint16_t war_this_nation = 0x00E0;
		ef_war_this_state,												// constexpr inline uint16_t war_this_state = 0x00E1;
		ef_war_this_province,											// constexpr inline uint16_t war_this_province = 0x00E2;
		ef_war_this_pop,													// constexpr inline uint16_t war_this_pop = 0x00E3;
		ef_war_from_nation,												// constexpr inline uint16_t war_from_nation = 0x00E4;
		ef_war_from_province,											// constexpr inline uint16_t war_from_province = 0x00E5;
		ef_war_no_ally_tag,												// constexpr inline uint16_t war_no_ally_tag = 0x00E6;
		ef_war_no_ally_this_nation,								// constexpr inline uint16_t war_no_ally_this_nation = 0x00E7;
		ef_war_no_ally_this_state,								// constexpr inline uint16_t war_no_ally_this_state = 0x00E8;
		ef_war_no_ally_this_province,							// constexpr inline uint16_t war_no_ally_this_province = 0x00E9;
		ef_war_no_ally_this_pop,									// constexpr inline uint16_t war_no_ally_this_pop = 0x00EA;
		ef_war_no_ally_from_nation,								// constexpr inline uint16_t war_no_ally_from_nation = 0x00EB;
		ef_war_no_ally_from_province,							// constexpr inline uint16_t war_no_ally_from_province = 0x00EC;
		ef_country_event_this_nation,							// constexpr inline uint16_t country_event_this_nation = 0x00ED;
		ef_country_event_immediate_this_nation,		// constexpr inline uint16_t country_event_immediate_this_nation = 0x00EE;
		ef_province_event_this_nation,						// constexpr inline uint16_t province_event_this_nation = 0x00EF;
		ef_province_event_immediate_this_nation,	// constexpr inline uint16_t province_event_immediate_this_nation = 0x00F0;
		ef_sub_unit_int,													// constexpr inline uint16_t sub_unit_int = 0x00F1;
		ef_sub_unit_this,													// constexpr inline uint16_t sub_unit_this = 0x00F2;
		ef_sub_unit_from,													// constexpr inline uint16_t sub_unit_from = 0x00F3;
		ef_sub_unit_current,											// constexpr inline uint16_t sub_unit_current = 0x00F4;
		ef_set_variable,													// constexpr inline uint16_t set_variable = 0x00F5;
		ef_change_variable,												// constexpr inline uint16_t change_variable = 0x00F6;
		ef_ideology,															// constexpr inline uint16_t ideology = 0x00F7;
		ef_upper_house,														// constexpr inline uint16_t upper_house = 0x00F8;
		ef_scaled_militancy_issue,								// constexpr inline uint16_t scaled_militancy_issue = 0x00F9;
		ef_scaled_militancy_ideology,							// constexpr inline uint16_t scaled_militancy_ideology = 0x00FA;
		ef_scaled_militancy_unemployment,					// constexpr inline uint16_t scaled_militancy_unemployment = 0x00FB;
		ef_scaled_consciousness_issue,						// constexpr inline uint16_t scaled_consciousness_issue = 0x00FC;
		ef_scaled_consciousness_ideology,					// constexpr inline uint16_t scaled_consciousness_ideology = 0x00FD;
		ef_scaled_consciousness_unemployment,			// constexpr inline uint16_t scaled_consciousness_unemployment = 0x00FE;
		ef_define_general,												// constexpr inline uint16_t define_general = 0x00FF;
		ef_define_admiral,												// constexpr inline uint16_t define_admiral = 0x0100;
		ef_dominant_issue,												// constexpr inline uint16_t dominant_issue = 0x0101;
		ef_add_war_goal,													// constexpr inline uint16_t add_war_goal = 0x0102;
		ef_move_issue_percentage_nation,					// constexpr inline uint16_t move_issue_percentage_nation = 0x0103;
		ef_move_issue_percentage_state,						// constexpr inline uint16_t move_issue_percentage_state = 0x0104;
		ef_move_issue_percentage_province,				// constexpr inline uint16_t move_issue_percentage_province = 0x0105;
		ef_move_issue_percentage_pop,							// constexpr inline uint16_t move_issue_percentage_pop = 0x0106;
		ef_party_loyalty,													// constexpr inline uint16_t party_loyalty = 0x0107;
		ef_party_loyalty_province,								// constexpr inline uint16_t party_loyalty_province = 0x0108;
		ef_variable_tech_name_no,									// constexpr inline uint16_t variable_tech_name_no = 0x0109;
		ef_variable_invention_name_yes,						// constexpr inline uint16_t variable_invention_name_yes = 0x010A;
		ef_build_railway_in_capital_yes_whole_state_yes_limit, // constexpr inline uint16_t
																													 // build_railway_in_capital_yes_whole_state_yes_limit = 0x010B;
		ef_build_railway_in_capital_yes_whole_state_no_limit,	 // constexpr inline uint16_t
																													 // build_railway_in_capital_yes_whole_state_no_limit = 0x010C;
		ef_build_railway_in_capital_no_whole_state_yes_limit,	 // constexpr inline uint16_t
																													 // build_railway_in_capital_no_whole_state_yes_limit = 0x010D;
		ef_build_railway_in_capital_no_whole_state_no_limit,	 // constexpr inline uint16_t
																													 // build_railway_in_capital_no_whole_state_no_limit = 0x010E;
		ef_build_fort_in_capital_yes_whole_state_yes_limit,		 // constexpr inline uint16_t
																													 // build_fort_in_capital_yes_whole_state_yes_limit = 0x010F;
		ef_build_fort_in_capital_yes_whole_state_no_limit, // constexpr inline uint16_t build_fort_in_capital_yes_whole_state_no_limit
																											 // = 0x0110;
		ef_build_fort_in_capital_no_whole_state_yes_limit, // constexpr inline uint16_t build_fort_in_capital_no_whole_state_yes_limit
																											 // = 0x0111;
		ef_build_fort_in_capital_no_whole_state_no_limit, // constexpr inline uint16_t build_fort_in_capital_no_whole_state_no_limit =
																											// 0x0112;
		ef_relation_reb,																	// constexpr inline uint16_t relation_reb = 0x0113;
		ef_variable_tech_name_yes,												// constexpr inline uint16_t variable_tech_name_yes = 0x0114;
		ef_variable_good_name,														// constexpr inline uint16_t variable_good_name = 0x0115;
		ef_set_country_flag_province,											// constexpr inline uint16_t set_country_flag_province = 0x0116;
		ef_add_country_modifier_province,									// constexpr inline uint16_t add_country_modifier_province = 0x0117;
		ef_add_country_modifier_province_no_duration, // constexpr inline uint16_t add_country_modifier_province_no_duration = 0x0118;
		ef_dominant_issue_nation,											// constexpr inline uint16_t dominant_issue_nation = 0x0119;
		ef_relation_province,													// constexpr inline uint16_t relation_province = 0x011A;
		ef_relation_province_this_nation,							// constexpr inline uint16_t relation_province_this_nation = 0x011B;
		ef_relation_province_this_province,						// constexpr inline uint16_t relation_province_this_province = 0x011C;
		ef_relation_province_from_nation,							// constexpr inline uint16_t relation_province_from_nation = 0x011D;
		ef_relation_province_from_province,						// constexpr inline uint16_t relation_province_from_province = 0x011E;
		ef_relation_province_reb,											// constexpr inline uint16_t relation_province_reb = 0x011F;
		ef_scaled_militancy_nation_issue,							// constexpr inline uint16_t scaled_militancy_nation_issue = 0x0120;
		ef_scaled_militancy_nation_ideology,					// constexpr inline uint16_t scaled_militancy_nation_ideology = 0x0121;
		ef_scaled_militancy_nation_unemployment,			// constexpr inline uint16_t scaled_militancy_nation_unemployment = 0x0122;
		ef_scaled_consciousness_nation_issue,					// constexpr inline uint16_t scaled_consciousness_nation_issue = 0x0123;
		ef_scaled_consciousness_nation_ideology,			// constexpr inline uint16_t scaled_consciousness_nation_ideology = 0x0124;
		ef_scaled_consciousness_nation_unemployment,	// constexpr inline uint16_t scaled_consciousness_nation_unemployment = 0x0125;
		ef_scaled_militancy_state_issue,							// constexpr inline uint16_t scaled_militancy_state_issue = 0x0126;
		ef_scaled_militancy_state_ideology,						// constexpr inline uint16_t scaled_militancy_state_ideology = 0x0127;
		ef_scaled_militancy_state_unemployment,				// constexpr inline uint16_t scaled_militancy_state_unemployment = 0x0128;
		ef_scaled_consciousness_state_issue,					// constexpr inline uint16_t scaled_consciousness_state_issue = 0x0129;
		ef_scaled_consciousness_state_ideology,				// constexpr inline uint16_t scaled_consciousness_state_ideology = 0x012A;
		ef_scaled_consciousness_state_unemployment,		// constexpr inline uint16_t scaled_consciousness_state_unemployment = 0x012B;
		ef_scaled_militancy_province_issue,						// constexpr inline uint16_t scaled_militancy_province_issue = 0x012C;
		ef_scaled_militancy_province_ideology,				// constexpr inline uint16_t scaled_militancy_province_ideology = 0x012D;
		ef_scaled_militancy_province_unemployment,		// constexpr inline uint16_t scaled_militancy_province_unemployment = 0x012E;
		ef_scaled_consciousness_province_issue,				// constexpr inline uint16_t scaled_consciousness_province_issue = 0x012F;
		ef_scaled_consciousness_province_ideology,		// constexpr inline uint16_t scaled_consciousness_province_ideology = 0x0130;
		ef_scaled_consciousness_province_unemployment,	 // constexpr inline uint16_t scaled_consciousness_province_unemployment =
																										 // 0x0131;
		ef_variable_good_name_province,									 // constexpr inline uint16_t variable_good_name_province = 0x0132;
		ef_treasury_province,														 // constexpr inline uint16_t treasury_province = 0x0133;
		ef_country_event_this_state,										 // constexpr inline uint16_t country_event_this_state = 0x0134;
		ef_country_event_immediate_this_state,					 // constexpr inline uint16_t country_event_immediate_this_state = 0x0135;
		ef_province_event_this_state,										 // constexpr inline uint16_t province_event_this_state = 0x0136;
		ef_province_event_immediate_this_state,					 // constexpr inline uint16_t province_event_immediate_this_state = 0x0137;
		ef_country_event_this_province,									 // constexpr inline uint16_t country_event_this_province = 0x0138;
		ef_country_event_immediate_this_province,				 // constexpr inline uint16_t country_event_immediate_this_province = 0x0139;
		ef_province_event_this_province,								 // constexpr inline uint16_t province_event_this_province = 0x013A;
		ef_province_event_immediate_this_province,			 // constexpr inline uint16_t province_event_immediate_this_province = 0x013B;
		ef_country_event_this_pop,											 // constexpr inline uint16_t country_event_this_pop = 0x013C;
		ef_country_event_immediate_this_pop,						 // constexpr inline uint16_t country_event_immediate_this_pop = 0x013D;
		ef_province_event_this_pop,											 // constexpr inline uint16_t province_event_this_pop = 0x013E;
		ef_province_event_immediate_this_pop,						 // constexpr inline uint16_t province_event_immediate_this_pop = 0x013F;
		ef_country_event_province_this_nation,					 // constexpr inline uint16_t country_event_province_this_nation = 0x0140;
		ef_country_event_immediate_province_this_nation, // constexpr inline uint16_t country_event_immediate_province_this_nation =
																										 // 0x0141;
		ef_country_event_province_this_state,						 // constexpr inline uint16_t country_event_province_this_state = 0x0142;
		ef_country_event_immediate_province_this_state,	 // constexpr inline uint16_t country_event_immediate_province_this_state =
																										 // 0x0143;
		ef_country_event_province_this_province,				 // constexpr inline uint16_t country_event_province_this_province = 0x0144;
		ef_country_event_immediate_province_this_province, // constexpr inline uint16_t country_event_immediate_province_this_province
																											 // = 0x0145;
		ef_country_event_province_this_pop,								 // constexpr inline uint16_t country_event_province_this_pop = 0x0146;
		ef_country_event_immediate_province_this_pop, // constexpr inline uint16_t country_event_immediate_province_this_pop = 0x0147;
		ef_activate_invention,												// constexpr inline uint16_t activate_invention = 0x0148;
		ef_variable_invention_name_no,								// constexpr inline uint16_t variable_invention_name_no = 0x0149;
		ef_add_core_tag_state,												// constexpr inline uint16_t add_core_tag_state = 0x014A;
		ef_remove_core_tag_state,											// constexpr inline uint16_t remove_core_tag_state = 0x014B;
		ef_secede_province_state,											// constexpr inline uint16_t secede_province_state = 0x014C;
		ef_assimilate_state,													// constexpr inline uint16_t assimilate_state = 0x014D;
		ef_add_core_state_this_nation, //constexpr inline uint16_t add_core_state_this_nation = 0x014E;
		ef_add_core_state_this_province, //constexpr inline uint16_t add_core_state_this_province = 0x014F;
		ef_add_core_state_this_state, //constexpr inline uint16_t add_core_state_this_state = 0x0150;
		ef_add_core_state_this_pop, //constexpr inline uint16_t add_core_state_this_pop = 0x0151;
		ef_add_core_state_from_province, //constexpr inline uint16_t add_core_state_from_province = 0x0152;
		ef_add_core_state_from_nation, //constexpr inline uint16_t add_core_state_from_nation = 0x0153;
		ef_add_core_state_reb, //constexpr inline uint16_t add_core_state_reb = 0x0154;
		ef_add_province_modifier, //constexpr inline uint16_t add_province_modifier_state = 0x0155;
		ef_add_province_modifier_no_duration, //constexpr inline uint16_t add_province_modifier_state_no_duration = 0x0156;
		ef_remove_core_this_nation, //constexpr inline uint16_t remove_core_state_this_nation = 0x0157;
		ef_remove_core_this_province, //constexpr inline uint16_t remove_core_state_this_province = 0x0158;
		ef_remove_core_this_state, //constexpr inline uint16_t remove_core_state_this_state = 0x0159;
		ef_remove_core_this_pop, //constexpr inline uint16_t remove_core_state_this_pop = 0x015A;
		ef_remove_core_from_province, //constexpr inline uint16_t remove_core_state_from_province = 0x015B;
		ef_remove_core_from_nation, //constexpr inline uint16_t remove_core_state_from_nation = 0x015C;
		ef_remove_core_reb, //constexpr inline uint16_t remove_core_state_reb = 0x015D;
		ef_remove_province_modifier, //constexpr inline uint16_t remove_province_modifier_state = 0x015E;
		ef_life_rating, //constexpr inline uint16_t life_rating_state = 0x015F;
		ef_secede_province_state_this_nation, //constexpr inline uint16_t secede_province_state_this_nation = 0x0160;
		ef_secede_province_state_this_state, //constexpr inline uint16_t secede_province_state_this_state = 0x0161;
		ef_secede_province_state_this_province, //constexpr inline uint16_t secede_province_state_this_province = 0x0162;
		ef_secede_province_state_this_pop, //constexpr inline uint16_t secede_province_state_this_pop = 0x0163;
		ef_secede_province_state_from_nation, //constexpr inline uint16_t secede_province_state_from_nation = 0x0164;
		ef_secede_province_state_from_province, //constexpr inline uint16_t secede_province_state_from_province = 0x0165;
		ef_secede_province_state_reb, //constexpr inline uint16_t secede_province_state_reb = 0x0166;
		ef_infrastructure, //constexpr inline uint16_t infrastructure_state = 0x0167;
		ef_fort, //constexpr inline uint16_t fort_state = 0x0168;
		ef_naval_base, //constexpr inline uint16_t naval_base_state = 0x0169;
		ef_is_slave_state_yes, //constexpr inline uint16_t is_slave_province_yes = 0x016A;
		ef_is_slave_state_no, //constexpr inline uint16_t is_slave_province_no = 0x016B;

		//
		// SCOPES
		//
		es_generic_scope, // constexpr inline uint16_t generic_scope = first_scope_code + 0x0000; // default grouping of effects (or
											// hidden_tooltip)
		es_x_neighbor_province_scope,				// constexpr inline uint16_t x_neighbor_province_scope = first_scope_code + 0x0001;
		es_x_neighbor_country_scope,				// constexpr inline uint16_t x_neighbor_country_scope = first_scope_code + 0x0002;
		es_x_country_scope,									// constexpr inline uint16_t x_country_scope = first_scope_code + 0x0003;
		es_x_country_scope_nation,					// constexpr inline uint16_t x_country_scope_nation = first_scope_code + 0x0004;
		es_x_empty_neighbor_province_scope, // constexpr inline uint16_t x_empty_neighbor_province_scope = first_scope_code + 0x0005;
		es_x_greater_power_scope,						// constexpr inline uint16_t x_greater_power_scope = first_scope_code + 0x0006;
		es_poor_strata_scope_nation,				// constexpr inline uint16_t poor_strata_scope_nation = first_scope_code + 0x0007;
		es_poor_strata_scope_state,					// constexpr inline uint16_t poor_strata_scope_state = first_scope_code + 0x0008;
		es_poor_strata_scope_province,			// constexpr inline uint16_t poor_strata_scope_province = first_scope_code + 0x0009;
		es_middle_strata_scope_nation,			// constexpr inline uint16_t middle_strata_scope_nation = first_scope_code + 0x000A;
		es_middle_strata_scope_state,				// constexpr inline uint16_t middle_strata_scope_state = first_scope_code + 0x000B;
		es_middle_strata_scope_province,		// constexpr inline uint16_t middle_strata_scope_province = first_scope_code + 0x000C;
		es_rich_strata_scope_nation,				// constexpr inline uint16_t rich_strata_scope_nation = first_scope_code + 0x000D;
		es_rich_strata_scope_state,					// constexpr inline uint16_t rich_strata_scope_state = first_scope_code + 0x000E;
		es_rich_strata_scope_province,			// constexpr inline uint16_t rich_strata_scope_province = first_scope_code + 0x000F;
		es_x_pop_scope_nation,							// constexpr inline uint16_t x_pop_scope_nation = first_scope_code + 0x0010;
		es_x_pop_scope_state,								// constexpr inline uint16_t x_pop_scope_state = first_scope_code + 0x0011;
		es_x_pop_scope_province,						// constexpr inline uint16_t x_pop_scope_province = first_scope_code + 0x0012;
		es_x_owned_scope_nation,						// constexpr inline uint16_t x_owned_scope_nation = first_scope_code + 0x0013;
		es_x_owned_scope_state,							// constexpr inline uint16_t x_owned_scope_state = first_scope_code + 0x0014;
		es_x_core_scope,										// constexpr inline uint16_t x_core_scope = first_scope_code + 0x0015;
		es_x_state_scope,										// constexpr inline uint16_t x_state_scope = first_scope_code + 0x0016;
		es_random_list_scope,								// constexpr inline uint16_t random_list_scope = first_scope_code + 0x0017;
		es_random_scope,										// constexpr inline uint16_t random_scope = first_scope_code + 0x0018;
		es_owner_scope_state,								// constexpr inline uint16_t owner_scope_state = first_scope_code + 0x0019;
		es_owner_scope_province,						// constexpr inline uint16_t owner_scope_province = first_scope_code + 0x001A;
		es_controller_scope,								// constexpr inline uint16_t controller_scope = first_scope_code + 0x001B;
		es_location_scope,									// constexpr inline uint16_t location_scope = first_scope_code + 0x001C;
		es_country_scope_pop,								// constexpr inline uint16_t country_scope_pop = first_scope_code + 0x001D;
		es_country_scope_state,							// constexpr inline uint16_t country_scope_state = first_scope_code + 0x001E;
		es_capital_scope,										// constexpr inline uint16_t capital_scope = first_scope_code + 0x001F;
		es_this_scope_nation,								// constexpr inline uint16_t this_scope_nation = first_scope_code + 0x0020;
		es_this_scope_state,								// constexpr inline uint16_t this_scope_state = first_scope_code + 0x0021;
		es_this_scope_province,							// constexpr inline uint16_t this_scope_province = first_scope_code + 0x0022;
		es_this_scope_pop,									// constexpr inline uint16_t this_scope_pop = first_scope_code + 0x0023;
		es_from_scope_nation,								// constexpr inline uint16_t from_scope_nation = first_scope_code + 0x0024;
		es_from_scope_state,								// constexpr inline uint16_t from_scope_state = first_scope_code + 0x0025;
		es_from_scope_province,							// constexpr inline uint16_t from_scope_province = first_scope_code + 0x0026;
		es_from_scope_pop,									// constexpr inline uint16_t from_scope_pop = first_scope_code + 0x0027;
		es_sea_zone_scope,									// constexpr inline uint16_t sea_zone_scope = first_scope_code + 0x0028;
		es_cultural_union_scope,						// constexpr inline uint16_t cultural_union_scope = first_scope_code + 0x0029;
		es_overlord_scope,									// constexpr inline uint16_t overlord_scope = first_scope_code + 0x002A;
		es_sphere_owner_scope,							// constexpr inline uint16_t sphere_owner_scope = first_scope_code + 0x002B;
		es_independence_scope,							// constexpr inline uint16_t independence_scope = first_scope_code + 0x002C;
		es_flashpoint_tag_scope,						// constexpr inline uint16_t flashpoint_tag_scope = first_scope_code + 0x002D;
		es_crisis_state_scope,							// constexpr inline uint16_t crisis_state_scope = first_scope_code + 0x002E;
		es_state_scope_pop,									// constexpr inline uint16_t state_scope_pop = first_scope_code + 0x002F;
		es_state_scope_province,						// constexpr inline uint16_t state_scope_province = first_scope_code + 0x0030;
		es_x_substate_scope,
		es_capital_scope_province,										// constexpr inline uint16_t capital_scope = first_scope_code + 0x0032;
		es_tag_scope,												// constexpr inline uint16_t tag_scope = first_scope_code + 0x0033;
		es_integer_scope,										// constexpr inline uint16_t integer_scope = first_scope_code + 0x0034;
		es_pop_type_scope_nation,						// constexpr inline uint16_t pop_type_scope_nation = first_scope_code + 0x0035;
		es_pop_type_scope_state,						// constexpr inline uint16_t pop_type_scope_state = first_scope_code + 0x0036;
		es_pop_type_scope_province,					// constexpr inline uint16_t pop_type_scope_province = first_scope_code + 0x0037;
		es_region_scope,										// constexpr inline uint16_t region_scope = first_scope_code + 0x0038;
};

uint32_t internal_make_effect_description(EFFECT_DISPLAY_PARAMS) {
	return effect_functions[*tval & effect::code_mask](ws, tval, layout, primary_slot, this_slot, from_slot, r_lo, r_hi,
			indentation);
}

#undef EFFECT_DISPLAY_PARAMS

} // namespace effect_tooltip

void effect_description(sys::state& state, text::layout_base& layout, dcon::effect_key k, int32_t primary_slot, int32_t this_slot,
		int32_t from_slot, uint32_t r_lo, uint32_t r_hi) {
	effect_tooltip::internal_make_effect_description(state, state.effect_data.data() + state.effect_data_indices[k.index() + 1], layout, primary_slot, this_slot,
			from_slot, r_lo, r_hi, 0);
}

} // namespace ui
