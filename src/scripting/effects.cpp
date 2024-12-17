#include "effects.hpp"
#include "system_state.hpp"
#include "ai.hpp"
#include "demographics.hpp"
#include "politics.hpp"
#include "prng.hpp"
#include "province_templates.hpp"
#include "rebels.hpp"
#include "triggers.hpp"
#include "script_constants.hpp"
#include "nations.hpp"
#include "nations_templates.hpp"

namespace effect {

#define EFFECT_PARAMTERS                                                                                                         \
	uint16_t const *tval, sys::state &ws, int32_t primary_slot, int32_t this_slot, int32_t from_slot, uint32_t r_hi, uint32_t r_lo, bool& els

uint32_t internal_execute_effect(EFFECT_PARAMTERS);

inline uint32_t apply_subeffects(EFFECT_PARAMTERS) {
	auto const source_size = 1 + get_effect_scope_payload_size(tval);
	auto sub_units_start = tval + 2 + effect_scope_data_payload(tval[0]);

	uint32_t i = 0;
	while(sub_units_start < tval + source_size) {
		i += internal_execute_effect(sub_units_start, ws, primary_slot, this_slot, from_slot, r_hi, r_lo + i, els);
		sub_units_start += 1 + get_generic_effect_payload_size(sub_units_start);
	}
	return i;
}

uint32_t es_generic_scope(EFFECT_PARAMTERS) {
	return apply_subeffects(tval, ws, primary_slot, this_slot, from_slot, r_hi, r_lo, els);
}

uint32_t es_if_scope(EFFECT_PARAMTERS) {
	uint32_t ret = 0;
	els = false;
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		if(trigger::evaluate(ws, limit, primary_slot, this_slot, from_slot)) {
			ret = apply_subeffects(tval, ws, primary_slot, this_slot, from_slot, r_hi, r_lo, els);
			els = true;
		}
	} else {
		ret = apply_subeffects(tval, ws, primary_slot, this_slot, from_slot, r_hi, r_lo, els);
		els = true;
	}
	return ret;
}
uint32_t es_else_if_scope(EFFECT_PARAMTERS) {
	uint32_t ret = 0;
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		if(!els && trigger::evaluate(ws, limit, primary_slot, this_slot, from_slot)) {
			ret = apply_subeffects(tval, ws, primary_slot, this_slot, from_slot, r_hi, r_lo, els);
			els = true;
		}
	} else if(!els) {
		ret = apply_subeffects(tval, ws, primary_slot, this_slot, from_slot, r_hi, r_lo, els);
		els = true;
	}
	return ret;
}

uint32_t es_x_neighbor_province_scope(EFFECT_PARAMTERS) {
	auto neighbor_range = ws.world.province_get_province_adjacency(trigger::to_prov(primary_slot));

	if((tval[0] & effect::is_random_scope) != 0) {
		std::vector<dcon::province_id> rlist;

		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto p : neighbor_range) {
				auto other = p.get_connected_provinces(p.get_connected_provinces(0) == trigger::to_prov(primary_slot) ? 1 : 0);
				if(other.get_nation_from_province_ownership() && trigger::evaluate(ws, limit, trigger::to_generic(other.id), this_slot, from_slot)) {
					rlist.push_back(other.id);
				}
			}
		} else {
			for(auto p : neighbor_range) {
				auto other = p.get_connected_provinces(p.get_connected_provinces(0) == trigger::to_prov(primary_slot) ? 1 : 0);
				if(other.get_nation_from_province_ownership()) {
					rlist.push_back(other.id);
				}
			}
		}

		if(rlist.size() != 0) {
			auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	} else {
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			uint32_t i = 0;
			for(auto p : neighbor_range) {
				auto other = p.get_connected_provinces(p.get_connected_provinces(0) == trigger::to_prov(primary_slot) ? 1 : 0);
				if(other.get_nation_from_province_ownership() && trigger::evaluate(ws, limit, trigger::to_generic(other.id), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(other.id), this_slot, from_slot, r_hi, r_lo + i, els);
				}
			}
			return i;
		} else {
			uint32_t i = 0;
			for(auto p : neighbor_range) {
				auto other = p.get_connected_provinces(p.get_connected_provinces(0) == trigger::to_prov(primary_slot) ? 1 : 0);
				if(other.get_nation_from_province_ownership()) {
					i += apply_subeffects(tval, ws, trigger::to_generic(other.id), this_slot, from_slot, r_hi, r_lo + i, els);
				}
			}
			return i;
		}
	}
}

std::vector<dcon::province_id> country_get_province_adjacency(sys::state &state, dcon::nation_id nat_id) {
	std::vector<dcon::province_id> v;

	for(auto own : state.world.nation_get_province_ownership(nat_id)) {
		auto prov = own.get_province().id;
		auto neighbor_range = state.world.province_get_province_adjacency(prov);

		for(auto adj : state.world.province_get_province_adjacency(prov)) {
			if((adj.get_type() & (province::border::impassible_bit | province::border::coastal_bit)) == 0) {
				auto other = adj.get_connected_provinces(0) != prov ? adj.get_connected_provinces(0) : adj.get_connected_provinces(1);

				if(std::find(v.begin(), v.end(), other.id) == v.end()) {
					v.push_back(other.id);
				}
			}
		}		
	}

	return v;
}
uint32_t es_x_neighbor_province_scope_nation(EFFECT_PARAMTERS) {
	auto neighbor_range = country_get_province_adjacency(ws, trigger::to_nation(primary_slot));


	// random_
	if((tval[0] & effect::is_random_scope) != 0) {
		std::vector<dcon::province_id> rlist;

		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto other : neighbor_range) {
				if(dcon::fatten(ws.world, other).get_nation_from_province_ownership() && trigger::evaluate(ws, limit, trigger::to_generic(other), this_slot, from_slot)) {
					rlist.push_back(other);
				}
			}
		} else {
			for(auto other : neighbor_range) {
				if(dcon::fatten(ws.world, other).get_nation_from_province_ownership()) {
					rlist.push_back(other);
				}
			}
		}

		if(rlist.size() != 0) {
			auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	} else {
		// any_
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			uint32_t i = 0;
			for(auto other : neighbor_range) {
				if(dcon::fatten(ws.world, other).get_nation_from_province_ownership() && trigger::evaluate(ws, limit, trigger::to_generic(other), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(other), this_slot, from_slot, r_hi, r_lo + i, els);
				}
			}
			return i;
		} else {
			uint32_t i = 0;
			for(auto other : neighbor_range) {
				if(dcon::fatten(ws.world, other).get_nation_from_province_ownership()) {
					i += apply_subeffects(tval, ws, trigger::to_generic(other), this_slot, from_slot, r_hi, r_lo + i, els);
				}
			}
			return i;
		}
	}
}
uint32_t es_x_empty_neighbor_province_scope_nation(EFFECT_PARAMTERS) {
	auto neighbor_range = country_get_province_adjacency(ws, trigger::to_nation(primary_slot));

	// random_
	if((tval[0] & effect::is_random_scope) != 0) {
		std::vector<dcon::province_id> rlist;

		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto other : neighbor_range) {
				if(!dcon::fatten(ws.world, other).get_nation_from_province_ownership() && trigger::evaluate(ws, limit, trigger::to_generic(other), this_slot, from_slot)) {
					rlist.push_back(other);
				}
			}
		} else {
			for(auto other : neighbor_range) {
				if(!dcon::fatten(ws.world, other).get_nation_from_province_ownership()) {
					rlist.push_back(other);
				}
			}
		}

		if(rlist.size() != 0) {
			auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	} else {
		// any_
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			uint32_t i = 0;
			for(auto other : neighbor_range) {
				if(!dcon::fatten(ws.world, other).get_nation_from_province_ownership() && trigger::evaluate(ws, limit, trigger::to_generic(other), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(other), this_slot, from_slot, r_hi, r_lo + i, els);
				}
			}
			return i;
		} else {
			uint32_t i = 0;
			for(auto other : neighbor_range) {
				if(!dcon::fatten(ws.world, other).get_nation_from_province_ownership()) {
					i += apply_subeffects(tval, ws, trigger::to_generic(other), this_slot, from_slot, r_hi, r_lo + i, els);
				}
			}
			return i;
		}
	}
}
uint32_t es_x_neighbor_country_scope(EFFECT_PARAMTERS) {
	auto neighbor_range = ws.world.nation_get_nation_adjacency(trigger::to_nation(primary_slot));

	if((tval[0] & effect::is_random_scope) != 0) {
		std::vector<dcon::nation_id> rlist;

		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto p : neighbor_range) {
				auto other = p.get_connected_nations(0) == trigger::to_nation(primary_slot) ? p.get_connected_nations(1) : p.get_connected_nations(0);
				if(trigger::evaluate(ws, limit, trigger::to_generic(other.id), this_slot, from_slot)) {
					rlist.push_back(other.id);
				}
			}
		} else {
			for(auto p : neighbor_range) {
				auto other = p.get_connected_nations(0) == trigger::to_nation(primary_slot) ? p.get_connected_nations(1) : p.get_connected_nations(0);
				rlist.push_back(other.id);
			}
		}

		if(rlist.size() != 0) {
			auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	} else {
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			uint32_t i = 0;
			for(auto p : neighbor_range) {
				auto other = p.get_connected_nations(0) == trigger::to_nation(primary_slot) ? p.get_connected_nations(1) : p.get_connected_nations(0);
				if(trigger::evaluate(ws, limit, trigger::to_generic(other.id), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(other.id), this_slot, from_slot, r_hi, r_lo + i, els);
				}
			}
			return i;
		} else {
			uint32_t i = 0;
			for(auto p : neighbor_range) {
				auto other = p.get_connected_nations(0) == trigger::to_nation(primary_slot) ? p.get_connected_nations(1) : p.get_connected_nations(0);
				i += apply_subeffects(tval, ws, trigger::to_generic(other.id), this_slot, from_slot, r_hi, r_lo + i, els);
			}
			return i;
		}
	}
}
uint32_t es_x_country_scope_nation(EFFECT_PARAMTERS) {
	if((tval[0] & effect::is_random_scope) != 0) {
		std::vector<dcon::nation_id> rlist;
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto n : ws.world.in_nation) {
				if(n.get_owned_province_count() != 0 && trigger::evaluate(ws, limit, trigger::to_generic(n.id), this_slot, from_slot))
					rlist.push_back(n.id);
			}
		} else {
			for(auto n : ws.world.in_nation) {
				if(n.get_owned_province_count() != 0)
					rlist.push_back(n.id);
			}
		}

		if(rlist.size() != 0) {
			auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	} else {
		uint32_t i = 0;
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto n : ws.world.in_nation) {
				if(n.get_owned_province_count() != 0 && trigger::evaluate(ws, limit, trigger::to_generic(n.id), this_slot, from_slot))
					i += apply_subeffects(tval, ws, trigger::to_generic(n.id), this_slot, from_slot, r_hi, r_lo + i, els);
			}
		} else {
			for(auto n : ws.world.in_nation) {
				if(n.get_owned_province_count() != 0)
					i += apply_subeffects(tval, ws, trigger::to_generic(n.id), this_slot, from_slot, r_hi, r_lo + i, els);
			}
		}
		return i;
	}
}
uint32_t es_x_event_country_scope_nation(EFFECT_PARAMTERS) {
	if((tval[0] & effect::is_random_scope) != 0) {
		std::vector<dcon::nation_id> rlist;
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto n : ws.world.in_nation) {
				if(n != trigger::to_nation(primary_slot) && trigger::evaluate(ws, limit, trigger::to_generic(n.id), this_slot, from_slot))
					rlist.push_back(n.id);
			}
		} else {
			for(auto n : ws.world.in_nation) {
				if(n != trigger::to_nation(primary_slot))
					rlist.push_back(n.id);
			}
		}
		if(rlist.size() != 0) {
			auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	} else {
		uint32_t i = 0;
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto n : ws.world.in_nation) {
				if(n != trigger::to_nation(primary_slot) && trigger::evaluate(ws, limit, trigger::to_generic(n.id), this_slot, from_slot))
					i += apply_subeffects(tval, ws, trigger::to_generic(n.id), this_slot, from_slot, r_hi, r_lo + i, els);
			}
		} else {
			for(auto n : ws.world.in_nation) {
				if(n != trigger::to_nation(primary_slot))
					i += apply_subeffects(tval, ws, trigger::to_generic(n.id), this_slot, from_slot, r_hi, r_lo + i, els);
			}
		}
		return i;
	}
}
uint32_t es_x_decision_country_scope_nation(EFFECT_PARAMTERS) {
	if((tval[0] & effect::is_random_scope) != 0) {
		std::vector<dcon::nation_id> rlist;
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto n : ws.world.in_nation) {
				if(n != trigger::to_nation(primary_slot) && n.get_owned_province_count() != 0 && trigger::evaluate(ws, limit, trigger::to_generic(n.id), this_slot, from_slot))
					rlist.push_back(n.id);
			}
		} else {
			for(auto n : ws.world.in_nation) {
				if(n != trigger::to_nation(primary_slot) && n.get_owned_province_count() != 0)
					rlist.push_back(n.id);
			}
		}
		if(rlist.size() != 0) {
			auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	} else {
		uint32_t i = 0;
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto n : ws.world.in_nation) {
				if(n != trigger::to_nation(primary_slot) && n.get_owned_province_count() != 0 && trigger::evaluate(ws, limit, trigger::to_generic(n.id), this_slot, from_slot))
					i += apply_subeffects(tval, ws, trigger::to_generic(n.id), this_slot, from_slot, r_hi, r_lo + i, els);
			}
		} else {
			for(auto n : ws.world.in_nation) {
				if(n != trigger::to_nation(primary_slot) && n.get_owned_province_count() != 0)
					i += apply_subeffects(tval, ws, trigger::to_generic(n.id), this_slot, from_slot, r_hi, r_lo + i, els);
			}
		}
		return i;
	}
}
uint32_t es_from_bounce_scope(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		if(trigger::evaluate(ws, limit, primary_slot, this_slot, primary_slot)) {
			return apply_subeffects(tval, ws, primary_slot, this_slot, primary_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	}
	return apply_subeffects(tval, ws, primary_slot, this_slot, primary_slot, r_hi, r_lo + 1, els);
}
uint32_t es_this_bounce_scope(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		if(trigger::evaluate(ws, limit, primary_slot, primary_slot, from_slot)) {
			return apply_subeffects(tval, ws, primary_slot, primary_slot, from_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	}
	return apply_subeffects(tval, ws, primary_slot, primary_slot, from_slot, r_hi, r_lo + 1, els);
}

uint32_t es_x_country_scope(EFFECT_PARAMTERS) {
	return es_x_country_scope_nation(tval, ws, trigger::to_generic(dcon::nation_id{}), this_slot, from_slot, r_hi, r_lo, els);
}
uint32_t es_x_event_country_scope(EFFECT_PARAMTERS) {
	return es_x_event_country_scope_nation(tval, ws, trigger::to_generic(dcon::nation_id{}), this_slot, from_slot, r_hi, r_lo, els);
}
uint32_t es_x_decision_country_scope(EFFECT_PARAMTERS) {
	return es_x_decision_country_scope_nation(tval, ws, trigger::to_generic(dcon::nation_id{}), this_slot, from_slot, r_hi, r_lo, els);
}
uint32_t es_x_empty_neighbor_province_scope(EFFECT_PARAMTERS) {
	auto neighbor_range = ws.world.province_get_province_adjacency(trigger::to_prov(primary_slot));
	if((tval[0] & effect::is_random_scope) != 0) {
		std::vector<dcon::province_id> rlist;
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto p : neighbor_range) {
				auto other = p.get_connected_provinces(p.get_connected_provinces(0) == trigger::to_prov(primary_slot) ? 1 : 0);
				if(!other.get_nation_from_province_ownership() && trigger::evaluate(ws, limit, trigger::to_generic(other.id), this_slot, from_slot)) {
					rlist.push_back(other.id);
				}
			}
		} else {
			for(auto p : neighbor_range) {
				auto other = p.get_connected_provinces(p.get_connected_provinces(0) == trigger::to_prov(primary_slot) ? 1 : 0);
				if(!other.get_nation_from_province_ownership()) {
					rlist.push_back(other.id);
				}
			}
		}

		if(rlist.size() != 0) {
			auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	} else {
		uint32_t i = 0;
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto p : neighbor_range) {
				auto other = p.get_connected_provinces(p.get_connected_provinces(0) == trigger::to_prov(primary_slot) ? 1 : 0);
				if(!other.get_nation_from_province_ownership() &&
						trigger::evaluate(ws, limit, trigger::to_generic(other.id), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(other.id), this_slot, from_slot, r_hi, r_lo + i, els);
				}
			}
		} else {
			for(auto p : neighbor_range) {
				auto other = p.get_connected_provinces(p.get_connected_provinces(0) == trigger::to_prov(primary_slot) ? 1 : 0);
				if(!other.get_nation_from_province_ownership()) {
					i += apply_subeffects(tval, ws, trigger::to_generic(other.id), this_slot, from_slot, r_hi, r_lo + i, els);
				}
			}
		}
		return i;
	}
}
uint32_t es_x_greater_power_scope(EFFECT_PARAMTERS) {
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
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	} else {
		uint32_t i = 0;
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto& n : ws.great_nations) {
				if(trigger::evaluate(ws, limit, trigger::to_generic(n.nation), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(n.nation), this_slot, from_slot, r_hi, r_lo + i, els);
				}
			}
			return i;
		} else {
			for(auto& n : ws.great_nations) {
				i += apply_subeffects(tval, ws, trigger::to_generic(n.nation), this_slot, from_slot, r_hi, r_lo + i, els);
			}
			return i;
		}
	}
}
uint32_t es_poor_strata_scope_nation(EFFECT_PARAMTERS) {
	uint32_t i = 0;
	std::vector<dcon::pop_id> plist;
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
			for(auto pop : p.get_province().get_pop_location()) {
				if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::poor) && trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
					plist.push_back(pop.get_pop().id);
				}
			}
		}
	} else {
		for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
			for(auto pop : p.get_province().get_pop_location()) {
				if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::poor)) {
					plist.push_back(pop.get_pop().id);
				}
			}
		}
	}

	for(auto p : plist)
		i += apply_subeffects(tval, ws, trigger::to_generic(p), this_slot, from_slot, r_hi, r_lo + i, els);

	return i;
}
uint32_t es_poor_strata_scope_state(EFFECT_PARAMTERS) {
	uint32_t i = 0;
	std::vector<dcon::pop_id> plist;

	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
			for(auto pop : ws.world.province_get_pop_location(p)) {
				if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::poor) && trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
					plist.push_back(pop.get_pop().id);
				}
			}
		});
	} else {
		province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
			for(auto pop : ws.world.province_get_pop_location(p)) {
				if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::poor)) {
					plist.push_back(pop.get_pop().id);
				}
			}
		});
	}

	for(auto p : plist)
		i += apply_subeffects(tval, ws, trigger::to_generic(p), this_slot, from_slot, r_hi, r_lo + i, els);
	return i;
}
uint32_t es_poor_strata_scope_province(EFFECT_PARAMTERS) {
	uint32_t i = 0;
	std::vector<dcon::pop_id> plist;

	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
			if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::poor) && trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
				plist.push_back(pop.get_pop().id);
			}
		}
	} else {
		for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
			if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::poor)) {
				plist.push_back(pop.get_pop().id);
			}
		}
	}

	for(auto p : plist)
		i += apply_subeffects(tval, ws, trigger::to_generic(p), this_slot, from_slot, r_hi, r_lo + i, els);
	return i;
}
uint32_t es_middle_strata_scope_nation(EFFECT_PARAMTERS) {
	uint32_t i = 0;
	std::vector<dcon::pop_id> plist;

	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
			for(auto pop : p.get_province().get_pop_location()) {
				if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::middle) &&
						trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
					plist.push_back(pop.get_pop().id);
				}
			}
		}
	} else {
		for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
			for(auto pop : p.get_province().get_pop_location()) {
				if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::middle)) {
					plist.push_back(pop.get_pop().id);
				}
			}
		}
	}

	for(auto p : plist)
		i += apply_subeffects(tval, ws, trigger::to_generic(p), this_slot, from_slot, r_hi, r_lo + i, els);
	return i;
}
uint32_t es_middle_strata_scope_state(EFFECT_PARAMTERS) {
	uint32_t i = 0;
	std::vector<dcon::pop_id> plist;

	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
			for(auto pop : ws.world.province_get_pop_location(p)) {
				if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::middle) && trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
					plist.push_back(pop.get_pop().id);
				}
			}
		});
	} else {
		province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
			for(auto pop : ws.world.province_get_pop_location(p)) {
				if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::middle)) {
					plist.push_back(pop.get_pop().id);
				}
			}
		});
	}
	for(auto p : plist)
		i += apply_subeffects(tval, ws, trigger::to_generic(p), this_slot, from_slot, r_hi, r_lo + i, els);
	return i;
}
uint32_t es_middle_strata_scope_province(EFFECT_PARAMTERS) {
	uint32_t i = 0;
	std::vector<dcon::pop_id> plist;

	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
			if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::middle) &&
					trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
				plist.push_back(pop.get_pop().id);
			}
		}
	} else {
		for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
			if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::middle)) {
				plist.push_back(pop.get_pop().id);
			}
		}
	}
	return i;
}
uint32_t es_rich_strata_scope_nation(EFFECT_PARAMTERS) {
	uint32_t i = 0;
	std::vector<dcon::pop_id> plist;

	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
			for(auto pop : p.get_province().get_pop_location()) {
				if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::rich) &&
						trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
					plist.push_back(pop.get_pop().id);
				}
			}
		}
	} else {
		for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
			for(auto pop : p.get_province().get_pop_location()) {
				if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::rich)) {
					plist.push_back(pop.get_pop().id);
				}
			}
		}
	}

	for(auto p : plist)
		i += apply_subeffects(tval, ws, trigger::to_generic(p), this_slot, from_slot, r_hi, r_lo + i, els);
	return i;
}
uint32_t es_rich_strata_scope_state(EFFECT_PARAMTERS) {
	uint32_t i = 0;
	std::vector<dcon::pop_id> plist;

	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
			for(auto pop : ws.world.province_get_pop_location(p)) {
				if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::rich) && trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
					plist.push_back(pop.get_pop().id);
				}
			}
		});
	} else {
		province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
			for(auto pop : ws.world.province_get_pop_location(p)) {
				if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::rich)) {
					plist.push_back(pop.get_pop().id);
				}
			}
		});
	}

	for(auto p : plist)
		i += apply_subeffects(tval, ws, trigger::to_generic(p), this_slot, from_slot, r_hi, r_lo + i, els);
	return i;
}
uint32_t es_rich_strata_scope_province(EFFECT_PARAMTERS) {
	uint32_t i = 0;
	std::vector<dcon::pop_id> plist;

	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
			if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::rich) && trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
				plist.push_back(pop.get_pop().id);
			}
		}
	} else {
		for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
			if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::rich)) {
				plist.push_back(pop.get_pop().id);
			}
		}
	}
	for(auto p : plist)
		i += apply_subeffects(tval, ws, trigger::to_generic(p), this_slot, from_slot, r_hi, r_lo + i, els);
	return i;
}
uint32_t es_x_pop_scope_nation(EFFECT_PARAMTERS) {
	if((tval[0] & effect::is_random_scope) != 0) {
		std::vector<dcon::pop_id> rlist;
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
				for(auto pop : p.get_province().get_pop_location()) {
					if(trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
						rlist.push_back(pop.get_pop().id);
					}
				}
			}
		} else {
			for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
				for(auto pop : p.get_province().get_pop_location()) {
					rlist.push_back(pop.get_pop().id);
				}
			}
		}
		if(rlist.size() != 0) {
			auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	} else {
		uint32_t i = 0;
		std::vector<dcon::pop_id> plist;

		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
				for(auto pop : p.get_province().get_pop_location()) {
					if(trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
						plist.push_back(pop.get_pop().id);
					}
				}
			}
		} else {
			for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
				for(auto pop : p.get_province().get_pop_location()) {
					plist.push_back(pop.get_pop().id);
				}
			}
		}

		for(auto p : plist)
			i += apply_subeffects(tval, ws, trigger::to_generic(p), this_slot, from_slot, r_hi, r_lo + i, els);
		return i;
	}
}
uint32_t es_x_pop_scope_state(EFFECT_PARAMTERS) {
	if((tval[0] & effect::is_random_scope) != 0) {
		std::vector<dcon::pop_id> rlist;
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
				for(auto pop : ws.world.province_get_pop_location(p)) {
					if(trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
						rlist.push_back(pop.get_pop().id);
					}
				}
			});
		} else {
			province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
				for(auto pop : ws.world.province_get_pop_location(p)) {
					rlist.push_back(pop.get_pop().id);
				}
			});
		}
		if(rlist.size() != 0) {
			auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	} else {
		uint32_t i = 0;
		std::vector<dcon::pop_id> plist;

		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
				for(auto pop : ws.world.province_get_pop_location(p)) {
					if(trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
						plist.push_back(pop.get_pop().id);
					}
				}
			});
		} else {
			province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
				for(auto pop : ws.world.province_get_pop_location(p)) {
					plist.push_back(pop.get_pop().id);
				}
			});
		}

		for(auto p : plist)
			i += apply_subeffects(tval, ws, trigger::to_generic(p), this_slot, from_slot, r_hi, r_lo + i, els);
		return i;
	}
}
uint32_t es_x_pop_scope_province(EFFECT_PARAMTERS) {
	if((tval[0] & effect::is_random_scope) != 0) {
		std::vector<dcon::pop_id> rlist;

		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
				if(trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
					rlist.push_back(pop.get_pop().id);
				}
			}
		} else {
			for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
				rlist.push_back(pop.get_pop().id);
			}
		}

		if(rlist.size() != 0) {
			auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	} else {
		uint32_t i = 0;
		std::vector<dcon::pop_id> plist;

		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
				if(trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
					plist.push_back(pop.get_pop().id);
				}
			}
		} else {
			for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
				plist.push_back(pop.get_pop().id);
			}
		}

		for(auto p : plist)
			i += apply_subeffects(tval, ws, trigger::to_generic(p), this_slot, from_slot, r_hi, r_lo + i, els);
		return i;
	}
}
uint32_t es_x_owned_scope_nation(EFFECT_PARAMTERS) {
	if((tval[0] & effect::is_random_scope) != 0) {
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
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	} else {
		std::vector<dcon::province_id> plist;

		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
				if(trigger::evaluate(ws, limit, trigger::to_generic(p.get_province().id), this_slot, from_slot)) {
					plist.push_back(p.get_province());
				}
			}
		} else {
			for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
				plist.push_back(p.get_province());
			}
		}

		uint32_t i = 0;
		for(auto p : plist) {
			i += apply_subeffects(tval, ws, trigger::to_generic(p), this_slot, from_slot, r_hi, r_lo + i, els);
		}
		return i;
	}
}
uint32_t es_x_owned_scope_state(EFFECT_PARAMTERS) {
	if((tval[0] & effect::is_random_scope) != 0) {
		std::vector<dcon::province_id> rlist;

		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
				if(trigger::evaluate(ws, limit, trigger::to_generic(p), this_slot, from_slot)) {
					rlist.push_back(p);
				}
			});
		} else {
			province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
				rlist.push_back(p);
			});
		}

		if(rlist.size() != 0) {
			auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	} else {
		uint32_t i = 0;
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
				if(trigger::evaluate(ws, limit, trigger::to_generic(p), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(p), this_slot, from_slot, r_hi, r_lo + i, els);
				}
			});
		} else {
			province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
				i += apply_subeffects(tval, ws, trigger::to_generic(p), this_slot, from_slot, r_hi, r_lo + i, els);
			});
		}
		return i;
	}
}
uint32_t es_x_core_scope(EFFECT_PARAMTERS) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(primary_slot));
	auto cores_range = ws.world.national_identity_get_core(tag);

	if((tval[0] & effect::is_random_scope) != 0) {
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
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	} else {
		std::vector<dcon::province_id> plist;

		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto p : cores_range) {
				if(trigger::evaluate(ws, limit, trigger::to_generic(p.get_province().id), this_slot, from_slot))
					plist.push_back(p.get_province());
			}
		} else {
			for(auto p : cores_range) {
				plist.push_back(p.get_province());
			}
		}

		uint32_t i = 0;
		for(auto p : plist) {
			i += apply_subeffects(tval, ws, trigger::to_generic(p), this_slot, from_slot, r_hi, r_lo + i, els);
		}
		return i;
	}
}
uint32_t es_x_core_scope_province(EFFECT_PARAMTERS) {
	auto prov = trigger::to_prov(primary_slot);
	auto cores_range = ws.world.province_get_core(prov);

	if((tval[0] & effect::is_random_scope) != 0) {
		std::vector<dcon::nation_id> rlist;

		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto p : cores_range) {
				auto h = p.get_identity().get_nation_from_identity_holder();
				if(h && trigger::evaluate(ws, limit, trigger::to_generic(h.id), this_slot, from_slot))
					rlist.push_back(h.id);
			}
		} else {
			for(auto p : cores_range) {
				auto h = p.get_identity().get_nation_from_identity_holder();
				if(h)
					rlist.push_back(h.id);
			}
		}

		if(rlist.size() != 0) {
			auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	} else {
		std::vector<dcon::nation_id> plist;

		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto p : cores_range) {
				auto h = p.get_identity().get_nation_from_identity_holder();
				if(h && trigger::evaluate(ws, limit, trigger::to_generic(h.id), this_slot, from_slot))
					plist.push_back(h);
			}
		} else {
			for(auto p : cores_range) {
				auto h = p.get_identity().get_nation_from_identity_holder();
				if(h)
					plist.push_back(h);
			}
		}

		uint32_t i = 0;
		for(auto p : plist) {
			i += apply_subeffects(tval, ws, trigger::to_generic(p), this_slot, from_slot, r_hi, r_lo + i, els);
		}
		return i;
	}
}
uint32_t es_x_state_scope(EFFECT_PARAMTERS) {
	if((tval[0] & effect::is_random_scope) != 0) {
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
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	} else {
		std::vector<dcon::state_instance_id> slist;

		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto si : ws.world.nation_get_state_ownership(trigger::to_nation(primary_slot))) {
				if(trigger::evaluate(ws, limit, trigger::to_generic(si.get_state().id), this_slot, from_slot))
					slist.push_back(si.get_state());
			}
		} else {
			for(auto si : ws.world.nation_get_state_ownership(trigger::to_nation(primary_slot))) {
				slist.push_back(si.get_state());
			}
		}

		uint32_t i = 0;
		for(auto p : slist) {
			i += apply_subeffects(tval, ws, trigger::to_generic(p), this_slot, from_slot, r_hi, r_lo + i, els);
		}
		return i;
	}
}
uint32_t es_x_substate_scope(EFFECT_PARAMTERS) {
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
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1, els);
		}
		return 0;
	} else {
		uint32_t i = 0;
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto si : ws.world.nation_get_overlord_as_ruler(trigger::to_nation(primary_slot))) {
				if(si.get_subject().get_is_substate() && trigger::evaluate(ws, limit, trigger::to_generic(si.get_subject().id), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(si.get_subject()), this_slot, from_slot, r_hi, r_lo + i, els);
				}
			}
		} else {
			for(auto si : ws.world.nation_get_overlord_as_ruler(trigger::to_nation(primary_slot))) {
				if(si.get_subject().get_is_substate()) {
					i += apply_subeffects(tval, ws, trigger::to_generic(si.get_subject()), this_slot, from_slot, r_hi, r_lo + i, els);
				}
			}
		}
		return i;
	}
}
uint32_t es_random_list_scope(EFFECT_PARAMTERS) {
	auto const source_size = 1 + effect::get_effect_scope_payload_size(tval);
	auto chances_total = tval[2];

	auto r = int32_t(rng::get_random(ws, r_hi, r_lo) % chances_total);

	auto sub_units_start = tval + 3; // [code] + [payload size] + [chances total] + [first sub effect chance]
	while(sub_units_start < tval + source_size) {
		r -= *sub_units_start;
		if(r < 0) {
			return 1 + internal_execute_effect(sub_units_start + 1, ws, primary_slot, this_slot, from_slot, r_hi, r_lo + 1, els);
		}
		sub_units_start += 2 + effect::get_generic_effect_payload_size(sub_units_start + 1); // each member preceeded by uint16_t
	}
	return 0;
}
uint32_t es_random_scope(EFFECT_PARAMTERS) {
	auto chance = tval[2];
	auto r = int32_t(rng::get_random(ws, r_hi, r_lo) % 100);
	if(r < chance)
		return 1 + apply_subeffects(tval, ws, primary_slot, this_slot, from_slot, r_hi, r_lo + 1, els);
	return 1;
}
uint32_t es_random_by_modifier_scope(EFFECT_PARAMTERS) {
	auto mod_k = dcon::value_modifier_key{ dcon::value_modifier_key::value_base_t(tval[2]) };
	auto chance = trigger::evaluate_multiplicative_modifier(ws, mod_k, primary_slot, this_slot, from_slot);
	assert(chance >= 0.f);
	auto r = int32_t(rng::get_random(ws, r_hi, r_lo) % 100);
	if(r < chance)
		return 1 + apply_subeffects(tval, ws, primary_slot, this_slot, from_slot, r_hi, r_lo + 1, els);
	return 1;
}
uint32_t es_owner_scope_state(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot)); owner)
		return apply_subeffects(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo, els);
	return 0;
}
uint32_t es_owner_scope_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return apply_subeffects(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo, els);
	return 0;
}
uint32_t es_controller_scope(EFFECT_PARAMTERS) {
	if(auto controller = ws.world.province_get_nation_from_province_control(trigger::to_prov(primary_slot)); controller)
		return apply_subeffects(tval, ws, trigger::to_generic(controller), this_slot, from_slot, r_hi, r_lo, els);
	return 0;
}
uint32_t es_location_scope(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.pop_get_province_from_pop_location(trigger::to_pop(primary_slot)); owner)
		return apply_subeffects(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo, els);
	return 0;
}
uint32_t es_country_scope_pop(EFFECT_PARAMTERS) {
	auto owner = nations::owner_of_pop(ws, trigger::to_pop(primary_slot));
	if(owner)
		return apply_subeffects(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo, els);
	return 0;
}
uint32_t es_country_scope_state(EFFECT_PARAMTERS) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
	if(owner)
		return apply_subeffects(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo, els);
	return 0;
}
uint32_t es_capital_scope(EFFECT_PARAMTERS) {
	auto owner = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	if(owner)
		return apply_subeffects(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo, els);
	return 0;
}
uint32_t es_capital_scope_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.nation_get_capital(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)));
	if(owner)
		return apply_subeffects(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo, els);
	return 0;
}
uint32_t es_this_scope_nation(EFFECT_PARAMTERS) {
	return apply_subeffects(tval, ws, this_slot, this_slot, from_slot, r_hi, r_lo, els);
}
uint32_t es_this_scope_state(EFFECT_PARAMTERS) {
	return apply_subeffects(tval, ws, this_slot, this_slot, from_slot, r_hi, r_lo, els);
}
uint32_t es_this_scope_province(EFFECT_PARAMTERS) {
	return apply_subeffects(tval, ws, this_slot, this_slot, from_slot, r_hi, r_lo, els);
}
uint32_t es_this_scope_pop(EFFECT_PARAMTERS) {
	return apply_subeffects(tval, ws, this_slot, this_slot, from_slot, r_hi, r_lo, els);
}
uint32_t es_from_scope_nation(EFFECT_PARAMTERS) {
	return apply_subeffects(tval, ws, from_slot, this_slot, from_slot, r_hi, r_lo, els);
}
uint32_t es_from_scope_state(EFFECT_PARAMTERS) {
	return apply_subeffects(tval, ws, from_slot, this_slot, from_slot, r_hi, r_lo, els);
}
uint32_t es_from_scope_province(EFFECT_PARAMTERS) {
	return apply_subeffects(tval, ws, from_slot, this_slot, from_slot, r_hi, r_lo, els);
}
uint32_t es_from_scope_pop(EFFECT_PARAMTERS) {
	return apply_subeffects(tval, ws, from_slot, this_slot, from_slot, r_hi, r_lo, els);
}
uint32_t es_sea_zone_scope(EFFECT_PARAMTERS) {
	auto pid = fatten(ws.world, trigger::to_prov(primary_slot));
	for(auto adj : pid.get_province_adjacency()) {
		if(adj.get_connected_provinces(0).id.index() >= ws.province_definitions.first_sea_province.index()) {
			return apply_subeffects(tval, ws, trigger::to_generic(adj.get_connected_provinces(0).id), this_slot, from_slot, r_hi, r_lo, els);
		} else if(adj.get_connected_provinces(1).id.index() >= ws.province_definitions.first_sea_province.index()) {
			return apply_subeffects(tval, ws, trigger::to_generic(adj.get_connected_provinces(1).id), this_slot, from_slot, r_hi, r_lo, els);
		}
	}
	return 0;
}
uint32_t es_cultural_union_scope(EFFECT_PARAMTERS) {
	auto cultures = ws.world.nation_get_primary_culture(trigger::to_nation(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(cultures);
	auto union_tags = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	auto group_holders = ws.world.national_identity_get_nation_from_identity_holder(union_tags);
	if(group_holders)
		return apply_subeffects(tval, ws, trigger::to_generic(group_holders), this_slot, from_slot, r_hi, r_lo, els);
	return 0;
}
uint32_t es_overlord_scope(EFFECT_PARAMTERS) {
	auto overlord = ws.world.nation_get_overlord_as_subject(trigger::to_nation(primary_slot));
	auto on = ws.world.overlord_get_ruler(overlord);
	if(on)
		return apply_subeffects(tval, ws, trigger::to_generic(on), this_slot, from_slot, r_hi, r_lo, els);
	return 0;
}
uint32_t es_sphere_owner_scope(EFFECT_PARAMTERS) {
	auto sphere_leader = ws.world.nation_get_in_sphere_of(trigger::to_nation(primary_slot));
	if(sphere_leader)
		return apply_subeffects(tval, ws, trigger::to_generic(sphere_leader), this_slot, from_slot, r_hi, r_lo, els);
	return 0;
}
uint32_t es_independence_scope(EFFECT_PARAMTERS) {
	auto rtag = ws.world.rebel_faction_get_defection_target(trigger::to_rebel(primary_slot));
	auto rnation = ws.world.national_identity_get_nation_from_identity_holder(rtag);
	if(rnation)
		return apply_subeffects(tval, ws, trigger::to_generic(rnation), this_slot, from_slot, r_hi, r_lo, els);
	return 0;
}
uint32_t es_flashpoint_tag_scope(EFFECT_PARAMTERS) {
	auto ctag = ws.world.state_instance_get_flashpoint_tag(trigger::to_state(primary_slot));
	auto rnation = ws.world.national_identity_get_nation_from_identity_holder(ctag);
	if(rnation)
		return apply_subeffects(tval, ws, trigger::to_generic(rnation), this_slot, from_slot, r_hi, r_lo, els);
	return 0;
}
uint32_t es_crisis_state_scope(EFFECT_PARAMTERS) {
	auto cstate = ws.crisis_state;
	if(cstate)
		return apply_subeffects(tval, ws, trigger::to_generic(cstate), this_slot, from_slot, r_hi, r_lo, els);
	return 0;
}
uint32_t es_state_scope_province(EFFECT_PARAMTERS) {
	auto state = ws.world.province_get_state_membership(trigger::to_prov(primary_slot));
	if(state)
		return apply_subeffects(tval, ws, trigger::to_generic(state), this_slot, from_slot, r_hi, r_lo, els);
	return 0;
}
uint32_t es_state_scope_pop(EFFECT_PARAMTERS) {
	auto pop_province = ws.world.pop_get_province_from_pop_location(trigger::to_pop(primary_slot));
	auto state = ws.world.province_get_state_membership(pop_province);
	if(state)
		return apply_subeffects(tval, ws, trigger::to_generic(state), this_slot, from_slot, r_hi, r_lo, els);
	return 0;
}
uint32_t es_tag_scope(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		auto tag = trigger::payload(tval[3]).tag_id;
		auto tag_holder = ws.world.national_identity_get_nation_from_identity_holder(tag);
		if(tag_holder && trigger::evaluate(ws, limit, trigger::to_generic(tag_holder), this_slot, from_slot))
			return apply_subeffects(tval, ws, trigger::to_generic(tag_holder), this_slot, from_slot, r_hi, r_lo, els);
		return 0;
	} else {
		auto tag = trigger::payload(tval[2]).tag_id;
		auto tag_holder = ws.world.national_identity_get_nation_from_identity_holder(tag);
		if(tag_holder)
			return apply_subeffects(tval, ws, trigger::to_generic(tag_holder), this_slot, from_slot, r_hi, r_lo, els);
		return 0;
	}
}
uint32_t es_integer_scope(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		auto prov = trigger::payload(tval[3]).prov_id;
		if(prov && trigger::evaluate(ws, limit, trigger::to_generic(prov), this_slot, from_slot))
			return apply_subeffects(tval, ws, trigger::to_generic(prov), this_slot, from_slot, r_hi, r_lo, els);
		return 0;
	} else {
		auto prov = trigger::payload(tval[2]).prov_id;
		if(prov)
			return apply_subeffects(tval, ws, trigger::to_generic(prov), this_slot, from_slot, r_hi, r_lo, els);
		return 0;
	}
}
uint32_t es_pop_type_scope_nation(EFFECT_PARAMTERS) {
	uint32_t i = 0;
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		auto type = trigger::payload(tval[3]).popt_id;
		for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
			for(auto pop : p.get_province().get_pop_location()) {
				if(pop.get_pop().get_poptype() == type && trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i, els);
				}
			}
		}
	} else {
		auto type = trigger::payload(tval[2]).popt_id;
		for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
			for(auto pop : p.get_province().get_pop_location()) {
				if(pop.get_pop().get_poptype() == type) {
					i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i, els);
				}
			}
		}
	}
	return i;
}
uint32_t es_pop_type_scope_state(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		auto type = trigger::payload(tval[3]).popt_id;
		uint32_t i = 0;
		province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
			for(auto pop : ws.world.province_get_pop_location(p)) {
				if(pop.get_pop().get_poptype() == type && trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i, els);
				}
			}
		});
		return i;
	} else {
		auto type = trigger::payload(tval[2]).popt_id;
		uint32_t i = 0;
		province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
			for(auto pop : ws.world.province_get_pop_location(p)) {
				if(pop.get_pop().get_poptype() == type) {
					i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i, els);
				}
			}
		});
		return i;
	}
}
uint32_t es_pop_type_scope_province(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		auto type = trigger::payload(tval[3]).popt_id;

		uint32_t i = 0;
		for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
			if(pop.get_pop().get_poptype() == type &&
					trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
				i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i, els);
			}
		}
		return i;
	} else {
		auto type = trigger::payload(tval[2]).popt_id;

		uint32_t i = 0;
		for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
			if(pop.get_pop().get_poptype() == type) {
				i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i, els);
			}
		}
		return i;
	}
}
uint32_t es_region_scope(EFFECT_PARAMTERS) {
	uint32_t i = 0;
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto region = trigger::payload(tval[3]).state_id;
		auto limit = trigger::payload(tval[2]).tr_id;
		for(auto p : ws.world.state_definition_get_abstract_state_membership(region)) {
			if(trigger::evaluate(ws, limit, trigger::to_generic(p.get_province().id), this_slot, from_slot)) {
				i += apply_subeffects(tval, ws, trigger::to_generic(p.get_province().id), this_slot, from_slot, r_hi, r_lo + i, els);
			}
		}
	} else {
		auto region = trigger::payload(tval[2]).state_id;
		for(auto p : ws.world.state_definition_get_abstract_state_membership(region)) {
			i += apply_subeffects(tval, ws, trigger::to_generic(p.get_province().id), this_slot, from_slot, r_hi, r_lo + i, els);
		}
	}
	return i;
}
uint32_t es_region_proper_scope(EFFECT_PARAMTERS) {
	uint32_t i = 0;
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto region = trigger::payload(tval[3]).reg_id;
		auto limit = trigger::payload(tval[2]).tr_id;
		for(auto p : ws.world.region_get_region_membership(region)) {
			if(trigger::evaluate(ws, limit, trigger::to_generic(p.get_province().id), this_slot, from_slot)) {
				i += apply_subeffects(tval, ws, trigger::to_generic(p.get_province().id), this_slot, from_slot, r_hi, r_lo + i, els);
			}
		}
	} else {
		auto region = trigger::payload(tval[2]).reg_id;
		for(auto p : ws.world.region_get_region_membership(region)) {
			i += apply_subeffects(tval, ws, trigger::to_generic(p.get_province().id), this_slot, from_slot, r_hi, r_lo + i, els);
		}
	}
	return i;
}

uint32_t ef_none(EFFECT_PARAMTERS) {
	return 0;
}
uint32_t ef_capital(EFFECT_PARAMTERS) {
	auto new_capital = trigger::payload(tval[1]).prov_id;
	if(ws.world.province_get_nation_from_province_ownership(new_capital) == trigger::to_nation(primary_slot))
		ws.world.nation_set_capital(trigger::to_nation(primary_slot), new_capital);
	return 0;
}
uint32_t ef_add_core_tag(EFFECT_PARAMTERS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	province::add_core(ws, trigger::to_prov(primary_slot), tag);
	return 0;
}
uint32_t ef_add_core_tag_state(EFFECT_PARAMTERS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) { province::add_core(ws, p, tag); });
	return 0;
}
uint32_t ef_add_core_int(EFFECT_PARAMTERS) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(primary_slot));
	auto prov = trigger::payload(tval[1]).prov_id;
	province::add_core(ws, prov, tag);
	return 0;
}
uint32_t ef_add_core_this_nation(EFFECT_PARAMTERS) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(this_slot));
	auto prov = trigger::to_prov(primary_slot);
	province::add_core(ws, prov, tag);
	return 0;
}
uint32_t ef_add_core_this_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(owner)
		return ef_add_core_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_add_core_this_state(EFFECT_PARAMTERS) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot));
	if(owner)
		return ef_add_core_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_add_core_this_pop(EFFECT_PARAMTERS) {
	auto owner = nations::owner_of_pop(ws, trigger::to_pop(this_slot));
	if(owner)
		return ef_add_core_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_add_core_from_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	if(owner)
		return ef_add_core_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_add_core_from_nation(EFFECT_PARAMTERS) {
	return ef_add_core_this_nation(tval, ws, primary_slot, from_slot, 0, r_hi, r_lo, els);
}
uint32_t ef_add_core_reb(EFFECT_PARAMTERS) {
	auto tag = ws.world.rebel_faction_get_defection_target(trigger::to_rebel(from_slot));
	province::add_core(ws, trigger::to_prov(primary_slot), tag);
	return 0;
}

uint32_t ef_add_core_state_this_nation(EFFECT_PARAMTERS) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(this_slot));
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot),
				[&](dcon::province_id p) { province::add_core(ws, p, tag); });
	return 0;
}
uint32_t ef_add_core_state_this_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(owner)
		return ef_add_core_state_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_add_core_state_this_state(EFFECT_PARAMTERS) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot));
	if(owner)
		return ef_add_core_state_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_add_core_state_this_pop(EFFECT_PARAMTERS) {
	auto owner = nations::owner_of_pop(ws, trigger::to_pop(this_slot));
	if(owner)
		return ef_add_core_state_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_add_core_state_from_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	if(owner)
		return ef_add_core_state_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_add_core_state_from_nation(EFFECT_PARAMTERS) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(from_slot));
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot),
			[&](dcon::province_id p) { province::add_core(ws, p, tag); });
	return 0;
}
uint32_t ef_add_core_state_reb(EFFECT_PARAMTERS) {
	auto tag = ws.world.rebel_faction_get_defection_target(trigger::to_rebel(from_slot));
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot),
			[&](dcon::province_id p) { province::add_core(ws, p, tag); });
	return 0;
}

uint32_t ef_remove_core_tag(EFFECT_PARAMTERS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	auto prov = trigger::to_prov(primary_slot);
	province::remove_core(ws, prov, tag);
	return 0;
}
uint32_t ef_remove_core_tag_state(EFFECT_PARAMTERS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot),
			[&](dcon::province_id p) { province::remove_core(ws, p, tag); });
	return 0;
}
uint32_t ef_remove_core_tag_nation(EFFECT_PARAMTERS) {
	auto tag = trigger::payload(tval[1]).tag_id;
	for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
		province::remove_core(ws, p.get_province(), tag);
	}
	return 0;
}
uint32_t ef_remove_core_int(EFFECT_PARAMTERS) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(primary_slot));
	auto prov = trigger::payload(tval[1]).prov_id;
	province::remove_core(ws, prov, tag);
	return 0;
}
uint32_t ef_remove_core_nation_this_nation(EFFECT_PARAMTERS) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(this_slot));
	for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
		province::remove_core(ws, p.get_province(), tag);
	}
	return 0;
}
uint32_t ef_remove_core_nation_this_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(owner)
		return ef_remove_core_nation_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_remove_core_nation_this_state(EFFECT_PARAMTERS) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot));
	if(owner)
		return ef_remove_core_nation_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_remove_core_nation_this_pop(EFFECT_PARAMTERS) {
	auto owner = nations::owner_of_pop(ws, trigger::to_pop(this_slot));
	if(owner)
		return ef_remove_core_nation_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_remove_core_nation_from_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	if(owner)
		return ef_remove_core_nation_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	return 0;
}
uint32_t ef_remove_core_nation_from_nation(EFFECT_PARAMTERS) {
	return ef_remove_core_nation_this_nation(tval, ws, primary_slot, from_slot, 0, r_hi, r_lo, els);
}
uint32_t ef_remove_core_nation_reb(EFFECT_PARAMTERS) {
	auto tag = ws.world.rebel_faction_get_defection_target(trigger::to_rebel(from_slot));
	for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
		province::remove_core(ws, p.get_province(), tag);
	}
	return 0;
}
uint32_t ef_remove_core_state_this_nation(EFFECT_PARAMTERS) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(this_slot));
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot),
		[&](dcon::province_id p) { province::remove_core(ws, p, tag);
	});
	return 0;
}
uint32_t ef_remove_core_state_this_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(owner)
		return ef_remove_core_state_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_remove_core_state_this_state(EFFECT_PARAMTERS) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot));
	if(owner)
		return ef_remove_core_state_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_remove_core_state_this_pop(EFFECT_PARAMTERS) {
	auto owner = nations::owner_of_pop(ws, trigger::to_pop(this_slot));
	if(owner)
		return ef_remove_core_state_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_remove_core_state_from_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	if(owner)
		return ef_remove_core_state_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	return 0;
}
uint32_t ef_remove_core_state_from_nation(EFFECT_PARAMTERS) {
	return ef_remove_core_state_this_nation(tval, ws, primary_slot, from_slot, 0, r_hi, r_lo, els);
}
uint32_t ef_remove_core_state_reb(EFFECT_PARAMTERS) {
	auto tag = ws.world.rebel_faction_get_defection_target(trigger::to_rebel(from_slot));
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot),
		[&](dcon::province_id p) { province::remove_core(ws, p, tag); });
	return 0;
}

uint32_t ef_remove_core_this_nation(EFFECT_PARAMTERS) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(this_slot));
	auto prov = trigger::to_prov(primary_slot);
	province::remove_core(ws, prov, tag);
	return 0;
}
uint32_t ef_remove_core_this_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(owner)
		return ef_remove_core_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_remove_core_this_state(EFFECT_PARAMTERS) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot));
	if(owner)
		return ef_remove_core_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_remove_core_this_pop(EFFECT_PARAMTERS) {
	auto owner = nations::owner_of_pop(ws, trigger::to_pop(this_slot));
	if(owner)
		return ef_remove_core_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_remove_core_from_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	if(owner)
		return ef_remove_core_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
	return 0;
}
uint32_t ef_remove_core_from_nation(EFFECT_PARAMTERS) {
	return ef_remove_core_this_nation(tval, ws, primary_slot, from_slot, 0, r_hi, r_lo, els);
}
uint32_t ef_remove_core_reb(EFFECT_PARAMTERS) {
	auto tag = ws.world.rebel_faction_get_defection_target(trigger::to_rebel(from_slot));
	province::remove_core(ws, trigger::to_prov(primary_slot), tag);
	return 0;
}
uint32_t ef_change_region_name_state(EFFECT_PARAMTERS) {
	auto def = ws.world.state_instance_get_definition(trigger::to_state(primary_slot));
	dcon::text_key name{ dcon::text_key::value_base_t(trigger::read_int32_t_from_payload(tval + 1)) };
	ws.world.state_definition_set_name(def, name);
	return 0;
}
uint32_t ef_change_region_name_province(EFFECT_PARAMTERS) {
	auto def = ws.world.province_get_state_from_abstract_state_membership(trigger::to_prov(primary_slot));
	if(def) {
		dcon::text_key name{ dcon::text_key::value_base_t(trigger::read_int32_t_from_payload(tval + 1)) };
		ws.world.state_definition_set_name(def, name);
	}
	return 0;
}
uint32_t ef_trade_goods(EFFECT_PARAMTERS) {
	province::set_rgo(ws, trigger::to_prov(primary_slot), trigger::payload(tval[1]).com_id);
	return 0;
}
uint32_t ef_add_accepted_culture(EFFECT_PARAMTERS) {
	if(ws.world.nation_get_primary_culture(trigger::to_nation(primary_slot)) == trigger::payload(tval[1]).cul_id) {
		return 0;
	}
	ws.world.nation_set_accepted_cultures(trigger::to_nation(primary_slot), trigger::payload(tval[1]).cul_id, true);
	nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_add_accepted_culture_union(EFFECT_PARAMTERS) {
	auto prim_culture = ws.world.nation_get_primary_culture(trigger::to_nation(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(prim_culture);
	for(auto c : ws.world.culture_group_get_culture_group_membership(cg)) {
		if(ws.world.nation_get_primary_culture(trigger::to_nation(primary_slot)) != c.get_member().id) {
			ws.world.nation_set_accepted_cultures(trigger::to_nation(primary_slot), c.get_member().id, true);
		}
	}
	nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	return 0;
}

uint32_t ef_add_accepted_culture_this(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_primary_culture(trigger::to_nation(this_slot));
	if(ws.world.nation_get_primary_culture(trigger::to_nation(primary_slot)) == c) {
		return 0;
	}
	ws.world.nation_set_accepted_cultures(trigger::to_nation(primary_slot), c, true);
	nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_add_accepted_culture_union_this(EFFECT_PARAMTERS) {
	auto prim_culture = ws.world.nation_get_primary_culture(trigger::to_nation(this_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(prim_culture);
	for(auto c : ws.world.culture_group_get_culture_group_membership(cg)) {
		if(ws.world.nation_get_primary_culture(trigger::to_nation(primary_slot)) != c.get_member().id) {
			ws.world.nation_set_accepted_cultures(trigger::to_nation(primary_slot), c.get_member().id, true);
		}
	}
	nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_add_accepted_culture_from(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_primary_culture(trigger::to_nation(from_slot));
	if(ws.world.nation_get_primary_culture(trigger::to_nation(primary_slot)) == c) {
		return 0;
	}
	ws.world.nation_set_accepted_cultures(trigger::to_nation(primary_slot), c, true);
	nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_add_accepted_culture_union_from(EFFECT_PARAMTERS) {
	auto prim_culture = ws.world.nation_get_primary_culture(trigger::to_nation(from_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(prim_culture);
	for(auto c : ws.world.culture_group_get_culture_group_membership(cg)) {
		if(ws.world.nation_get_primary_culture(trigger::to_nation(primary_slot)) != c.get_member().id) {
			ws.world.nation_set_accepted_cultures(trigger::to_nation(primary_slot), c.get_member().id, true);
		}
	}
	nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	return 0;
}

uint32_t ef_primary_culture(EFFECT_PARAMTERS) {
	ws.world.nation_set_primary_culture(trigger::to_nation(primary_slot), trigger::payload(tval[1]).cul_id);
	ws.world.nation_set_accepted_cultures(trigger::to_nation(primary_slot), trigger::payload(tval[1]).cul_id, false);
	nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_primary_culture_this_nation(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_primary_culture(trigger::to_nation(this_slot));
	ws.world.nation_set_primary_culture(trigger::to_nation(primary_slot), c);
	ws.world.nation_set_accepted_cultures(trigger::to_nation(primary_slot), c.id, false);
	nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_primary_culture_this_state(EFFECT_PARAMTERS) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot));
	if(owner) {
		auto c = ws.world.nation_get_primary_culture(owner);
		ws.world.nation_set_primary_culture(trigger::to_nation(primary_slot), c);
		ws.world.nation_set_accepted_cultures(trigger::to_nation(primary_slot), c.id, false);
		nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	}
	return 0;
}
uint32_t ef_primary_culture_this_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(owner) {
		auto c = ws.world.nation_get_primary_culture(owner);
		ws.world.nation_set_primary_culture(trigger::to_nation(primary_slot), c);
		ws.world.nation_set_accepted_cultures(trigger::to_nation(primary_slot), c.id, false);
		nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	}
	return 0;
}
uint32_t ef_primary_culture_this_pop(EFFECT_PARAMTERS) {
	auto owner = nations::owner_of_pop(ws, trigger::to_pop(this_slot));
	if(owner) {
		auto c = ws.world.nation_get_primary_culture(owner);
		ws.world.nation_set_primary_culture(trigger::to_nation(primary_slot), c);
		ws.world.nation_set_accepted_cultures(trigger::to_nation(primary_slot), c.id, false);
		nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	}
	return 0;
}
uint32_t ef_primary_culture_from_nation(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_primary_culture(trigger::to_nation(from_slot));
	ws.world.nation_set_primary_culture(trigger::to_nation(primary_slot), c);
	ws.world.nation_set_accepted_cultures(trigger::to_nation(primary_slot), c.id, false);
	nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_remove_accepted_culture(EFFECT_PARAMTERS) {
	ws.world.nation_set_accepted_cultures(trigger::to_nation(primary_slot), trigger::payload(tval[1]).cul_id, false);
	nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_life_rating(EFFECT_PARAMTERS) {
	ws.world.province_set_life_rating(trigger::to_prov(primary_slot),
			uint8_t(std::clamp(int32_t(ws.world.province_get_life_rating(trigger::to_prov(primary_slot))) +
				trigger::payload(tval[1]).signed_value,
				0, 255)));
	return 0;
}
uint32_t ef_life_rating_state(EFFECT_PARAMTERS) {
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
		ws.world.province_set_life_rating(
			p,
			uint8_t(std::clamp(int32_t(ws.world.province_get_life_rating(p)) + trigger::payload(tval[1]).signed_value, 0, 255)));
	});

	return 0;
}
uint32_t ef_religion(EFFECT_PARAMTERS) {
	ws.world.nation_set_religion(trigger::to_nation(primary_slot), trigger::payload(tval[1]).rel_id);
	return 0;
}
uint32_t ef_religion_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner) {
		auto owner_c = ws.world.nation_get_primary_culture(owner);
		for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
			pop.get_pop().set_religion(trigger::payload(tval[1]).rel_id);
		}
	}
	return 0;
}
uint32_t ef_religion_pop(EFFECT_PARAMTERS) {
	auto pop = trigger::to_pop(primary_slot);
	dcon::fatten(ws.world, pop).set_religion(trigger::payload(tval[1]).rel_id);
	return 0;
}
uint32_t ef_is_slave_state_yes(EFFECT_PARAMTERS) {
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot),
			[&](dcon::province_id p) { ws.world.province_set_is_slave(p, true); });
	return 0;
}
uint32_t ef_is_slave_province_yes(EFFECT_PARAMTERS) {
	ws.world.province_set_is_slave(trigger::to_prov(primary_slot), true);
	return 0;
}
uint32_t ef_is_slave_pop_yes(EFFECT_PARAMTERS) {
	ws.world.pop_set_poptype(trigger::to_pop(primary_slot), ws.culture_definitions.slaves);
	return 0;
}
uint32_t ef_research_points(EFFECT_PARAMTERS) {
	ws.world.nation_get_research_points(trigger::to_nation(primary_slot)) += float(trigger::payload(tval[1]).signed_value);
	return 0;
}
uint32_t ef_tech_school(EFFECT_PARAMTERS) {
	ws.world.nation_set_tech_school(trigger::to_nation(primary_slot), trigger::payload(tval[1]).mod_id);
	return 0;
}
uint32_t ef_government(EFFECT_PARAMTERS) {
	politics::change_government_type(ws, trigger::to_nation(primary_slot), trigger::payload(tval[1]).gov_id);
	return 0;
}
uint32_t ef_government_reb(EFFECT_PARAMTERS) {

	auto new_gov = ws.world.rebel_faction_get_type(trigger::to_rebel(from_slot))
		.get_government_change(ws.world.nation_get_government_type(trigger::to_nation(primary_slot)));
	if(new_gov) {
		politics::change_government_type(ws, trigger::to_nation(primary_slot), new_gov);
	}
	return 0;
}
uint32_t ef_treasury(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	auto& t = ws.world.nation_get_stockpiles(trigger::to_nation(primary_slot), economy::money);
	if(ws.world.nation_get_is_player_controlled(trigger::to_nation(primary_slot)))
		t += amount;
	else
		t = std::max(0.0f, t + amount);
	return 0;
}
uint32_t ef_war_exhaustion(EFFECT_PARAMTERS) {
	auto& war_x = ws.world.nation_get_war_exhaustion(trigger::to_nation(primary_slot));
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	war_x = std::clamp(war_x + amount, 0.0f, 100.0f);
	return 0;
}
uint32_t ef_prestige(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	nations::adjust_prestige(ws, trigger::to_nation(primary_slot), amount);
	return 0;
}
uint32_t ef_change_tag(EFFECT_PARAMTERS) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(primary_slot));
	culture::replace_cores(ws, tag, trigger::payload(tval[1]).tag_id);
	auto old_holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	ws.world.nation_set_identity_from_identity_holder(trigger::to_nation(primary_slot), trigger::payload(tval[1]).tag_id);
	politics::update_displayed_identity(ws, trigger::to_nation(primary_slot));
	if(old_holder) {
		ws.world.nation_set_identity_from_identity_holder(old_holder, tag);
		politics::update_displayed_identity(ws, old_holder);
	}
	return 0;
}
uint32_t ef_change_tag_culture(EFFECT_PARAMTERS) {
	auto prim_culture = ws.world.nation_get_primary_culture(trigger::to_nation(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(prim_culture);
	auto u = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	if(!u)
		return 0;

	auto old_holder = ws.world.national_identity_get_nation_from_identity_holder(u);
	auto tag = ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(primary_slot));
	culture::replace_cores(ws, tag, u);
	ws.world.nation_set_identity_from_identity_holder(trigger::to_nation(primary_slot), u);
	politics::update_displayed_identity(ws, trigger::to_nation(primary_slot));
	if(old_holder) {
		ws.world.nation_set_identity_from_identity_holder(old_holder, tag);
		politics::update_displayed_identity(ws, old_holder);
	}
	return 0;
}
uint32_t ef_change_tag_no_core_switch(EFFECT_PARAMTERS) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(!holder)
		return 0;

	if(ws.world.nation_get_is_player_controlled(trigger::to_nation(primary_slot))) {
		network::switch_player(ws, holder, trigger::to_nation(primary_slot));
	} else if(ws.world.nation_get_is_player_controlled(holder)) {
		network::switch_player(ws, trigger::to_nation(primary_slot), holder);
	}

	auto old_controller = ws.world.nation_get_is_player_controlled(holder);
	ws.world.nation_set_is_player_controlled(holder, ws.world.nation_get_is_player_controlled(trigger::to_nation(primary_slot)));
	ws.world.nation_set_is_player_controlled(trigger::to_nation(primary_slot), old_controller);

	if(ws.world.nation_get_is_player_controlled(trigger::to_nation(primary_slot)))
		ai::remove_ai_data(ws, trigger::to_nation(primary_slot));
	if(ws.world.nation_get_is_player_controlled(holder))
		ai::remove_ai_data(ws, holder);

	if(ws.local_player_nation == trigger::to_nation(primary_slot)) {
		ws.local_player_nation = holder;
	} else if(ws.local_player_nation == holder) {
		ws.local_player_nation = trigger::to_nation(primary_slot);
	}
	return 0;
}
uint32_t ef_change_tag_no_core_switch_culture(EFFECT_PARAMTERS) {
	auto prim_culture = ws.world.nation_get_primary_culture(trigger::to_nation(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(prim_culture);
	auto u = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(u);
	if(!holder)
		return 0;

	if(ws.world.nation_get_is_player_controlled(trigger::to_nation(primary_slot))) {
		network::switch_player(ws, holder, trigger::to_nation(primary_slot));
	} else if(ws.world.nation_get_is_player_controlled(holder)) {
		network::switch_player(ws, trigger::to_nation(primary_slot), holder);
	}

	auto old_controller = ws.world.nation_get_is_player_controlled(holder);
	ws.world.nation_set_is_player_controlled(holder, ws.world.nation_get_is_player_controlled(trigger::to_nation(primary_slot)));
	ws.world.nation_set_is_player_controlled(trigger::to_nation(primary_slot), old_controller);

	if(ws.world.nation_get_is_player_controlled(trigger::to_nation(primary_slot)))
		ai::remove_ai_data(ws, trigger::to_nation(primary_slot));
	if(ws.world.nation_get_is_player_controlled(holder))
		ai::remove_ai_data(ws, holder);

	if(ws.local_player_nation == trigger::to_nation(primary_slot)) {
		ws.local_player_nation = holder;
	} else if(ws.local_player_nation == holder) {
		ws.local_player_nation = trigger::to_nation(primary_slot);
	}
	return 0;
}
uint32_t ef_set_country_flag(EFFECT_PARAMTERS) {
	ws.world.nation_set_flag_variables(trigger::to_nation(primary_slot), trigger::payload(tval[1]).natf_id, true);
	return 0;
}
uint32_t ef_clr_country_flag(EFFECT_PARAMTERS) {
	ws.world.nation_set_flag_variables(trigger::to_nation(primary_slot), trigger::payload(tval[1]).natf_id, false);
	return 0;
}
uint32_t ef_military_access(EFFECT_PARAMTERS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(target)
		military::give_military_access(ws, trigger::to_nation(primary_slot), target);
	return 0;
}
uint32_t ef_military_access_this_nation(EFFECT_PARAMTERS) {
	military::give_military_access(ws, trigger::to_nation(primary_slot), trigger::to_nation(this_slot));
	return 0;
}
uint32_t ef_military_access_this_province(EFFECT_PARAMTERS) {
	auto target = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(target)
		military::give_military_access(ws, trigger::to_nation(primary_slot), target);
	return 0;
}
uint32_t ef_military_access_from_nation(EFFECT_PARAMTERS) {
	military::give_military_access(ws, trigger::to_nation(primary_slot), trigger::to_nation(from_slot));
	return 0;
}
uint32_t ef_military_access_from_province(EFFECT_PARAMTERS) {
	auto target = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	if(target)
		military::give_military_access(ws, trigger::to_nation(primary_slot), target);
	return 0;
}
uint32_t ef_badboy(EFFECT_PARAMTERS) {
	auto& inf = ws.world.nation_get_infamy(trigger::to_nation(primary_slot));
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	inf = std::max(0.0f, inf + amount);
	return 0;
}
uint32_t ef_secede_province(EFFECT_PARAMTERS) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(!holder)
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	if(hprovs.begin() == hprovs.end()) {
		auto powner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot));
		if(powner)
			nations::create_nation_based_on_template(ws, holder, powner);
	}
	province::change_province_owner(ws, trigger::to_prov(primary_slot), holder);
	return 0;
}
uint32_t ef_secede_province_state(EFFECT_PARAMTERS) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(!holder)
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	if(hprovs.begin() == hprovs.end()) {
		auto powner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
		if(powner)
			nations::create_nation_based_on_template(ws, holder, powner);
	}
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot),
			[&](dcon::province_id p) { province::change_province_owner(ws, p, holder); });
	return 0;
}
uint32_t ef_secede_province_state_this_nation(EFFECT_PARAMTERS) {
	auto target = trigger::to_nation(this_slot);
	auto hprovs = ws.world.nation_get_province_ownership(target);
	if(hprovs.begin() == hprovs.end()) {
		auto powner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
		if(powner)
			nations::create_nation_based_on_template(ws, target, powner);
	}
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot),
			[&](dcon::province_id p) { province::change_province_owner(ws, p, target); });
	return 0;
}
uint32_t ef_secede_province_state_this_state(EFFECT_PARAMTERS) {
	auto target = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot));
	if(!target)
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(target);
	if(hprovs.begin() == hprovs.end()) {
		auto powner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
		if(powner)
			nations::create_nation_based_on_template(ws, target, powner);
	}
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot),
			[&](dcon::province_id p) { province::change_province_owner(ws, p, target); });
	return 0;
}
uint32_t ef_secede_province_state_this_province(EFFECT_PARAMTERS) {
	auto target = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(!target)
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(target);
	if(hprovs.begin() == hprovs.end()) {
		auto powner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
		if(powner)
			nations::create_nation_based_on_template(ws, target, powner);
	}
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot),
			[&](dcon::province_id p) { province::change_province_owner(ws, p, target); });
	return 0;
}
uint32_t ef_secede_province_state_this_pop(EFFECT_PARAMTERS) {
	auto target = nations::owner_of_pop(ws, trigger::to_pop(this_slot));
	if(!target)
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(target);
	if(hprovs.begin() == hprovs.end()) {
		auto powner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
		if(powner)
			nations::create_nation_based_on_template(ws, target, powner);
	}
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot),
			[&](dcon::province_id p) { province::change_province_owner(ws, p, target); });
	return 0;
}
uint32_t ef_secede_province_state_from_nation(EFFECT_PARAMTERS) {
	return ef_secede_province_state_this_nation(tval, ws, primary_slot, from_slot, 0, r_hi, r_lo, els);
}
uint32_t ef_secede_province_state_from_province(EFFECT_PARAMTERS) {
	return ef_secede_province_state_this_province(tval, ws, primary_slot, from_slot, 0, r_hi, r_lo, els);
}
uint32_t ef_secede_province_state_reb(EFFECT_PARAMTERS) {
	auto target = ws.world.rebel_faction_get_defection_target(trigger::to_rebel(from_slot));
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(target);
	if(!holder)
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	if(hprovs.begin() == hprovs.end()) {
		auto powner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
		if(powner)
			nations::create_nation_based_on_template(ws, holder, powner);
	}
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot),
			[&](dcon::province_id p) { province::change_province_owner(ws, p, holder); });
	return 0;
}

uint32_t ef_secede_province_this_nation(EFFECT_PARAMTERS) {
	auto target = trigger::to_nation(this_slot);
	auto hprovs = ws.world.nation_get_province_ownership(target);
	if(hprovs.begin() == hprovs.end()) {
		auto powner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot));
		if(powner)
			nations::create_nation_based_on_template(ws, target, powner);
	}
	province::change_province_owner(ws, trigger::to_prov(primary_slot), target);
	return 0;
}
uint32_t ef_secede_province_this_state(EFFECT_PARAMTERS) {
	auto target = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot));
	if(!target)
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(target);
	if(hprovs.begin() == hprovs.end()) {
		auto powner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot));
		if(powner)
			nations::create_nation_based_on_template(ws, target, powner);
	}
	province::change_province_owner(ws, trigger::to_prov(primary_slot), target);
	return 0;
}
uint32_t ef_secede_province_this_province(EFFECT_PARAMTERS) {
	auto target = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(!target)
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(target);
	if(hprovs.begin() == hprovs.end()) {
		auto powner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot));
		if(powner)
			nations::create_nation_based_on_template(ws, target, powner);
	}
	province::change_province_owner(ws, trigger::to_prov(primary_slot), target);
	return 0;
}
uint32_t ef_secede_province_this_pop(EFFECT_PARAMTERS) {
	auto target = nations::owner_of_pop(ws, trigger::to_pop(this_slot));
	if(!target)
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(target);
	if(hprovs.begin() == hprovs.end()) {
		auto powner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot));
		if(powner)
			nations::create_nation_based_on_template(ws, target, powner);
	}
	province::change_province_owner(ws, trigger::to_prov(primary_slot), target);
	return 0;
}
uint32_t ef_secede_province_from_nation(EFFECT_PARAMTERS) {
	return ef_secede_province_this_nation(tval, ws, primary_slot, from_slot, 0, r_hi, r_lo, els);
}
uint32_t ef_secede_province_from_province(EFFECT_PARAMTERS) {
	return ef_secede_province_this_province(tval, ws, primary_slot, from_slot, 0, r_hi, r_lo, els);
}
uint32_t ef_secede_province_reb(EFFECT_PARAMTERS) {
	auto target = ws.world.rebel_faction_get_defection_target(trigger::to_rebel(from_slot));
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(target);
	if(!holder)
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	if(hprovs.begin() == hprovs.end()) {
		auto powner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot));
		if(powner)
			nations::create_nation_based_on_template(ws, holder, powner);
	}
	province::change_province_owner(ws, trigger::to_prov(primary_slot), holder);
	return 0;
}
uint32_t ef_inherit(EFFECT_PARAMTERS) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(!holder || holder == trigger::to_nation(primary_slot))
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	auto sprovs = ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot));
	if(sprovs.begin() == sprovs.end()) {
		nations::create_nation_based_on_template(ws, trigger::to_nation(primary_slot), holder);
	}
	while(hprovs.begin() != hprovs.end()) {
		province::change_province_owner(ws, (*hprovs.begin()).get_province().id, trigger::to_nation(primary_slot));
	}
	return 0;
}
uint32_t ef_inherit_this_nation(EFFECT_PARAMTERS) {
	auto holder = trigger::to_nation(this_slot);
	if(holder == trigger::to_nation(primary_slot))
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	auto sprovs = ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot));
	if(sprovs.begin() == sprovs.end()) {
		nations::create_nation_based_on_template(ws, trigger::to_nation(primary_slot), holder);
	}
	while(hprovs.begin() != hprovs.end()) {
		province::change_province_owner(ws, (*hprovs.begin()).get_province().id, trigger::to_nation(primary_slot));
	}
	return 0;
}
uint32_t ef_inherit_this_state(EFFECT_PARAMTERS) {
	auto holder = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot));
	if(!holder || holder == trigger::to_nation(primary_slot))
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	auto sprovs = ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot));
	if(sprovs.begin() == sprovs.end()) {
		nations::create_nation_based_on_template(ws, trigger::to_nation(primary_slot), holder);
	}
	while(hprovs.begin() != hprovs.end()) {
		province::change_province_owner(ws, (*hprovs.begin()).get_province().id, trigger::to_nation(primary_slot));
	}
	return 0;
}
uint32_t ef_inherit_this_province(EFFECT_PARAMTERS) {
	auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(!holder || holder == trigger::to_nation(primary_slot))
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	auto sprovs = ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot));
	if(sprovs.begin() == sprovs.end()) {
		nations::create_nation_based_on_template(ws, trigger::to_nation(primary_slot), holder);
	}
	while(hprovs.begin() != hprovs.end()) {
		province::change_province_owner(ws, (*hprovs.begin()).get_province().id, trigger::to_nation(primary_slot));
	}
	return 0;
}
uint32_t ef_inherit_this_pop(EFFECT_PARAMTERS) {
	auto holder = nations::owner_of_pop(ws, trigger::to_pop(this_slot));
	if(!holder || holder == trigger::to_nation(primary_slot))
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	auto sprovs = ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot));
	if(sprovs.begin() == sprovs.end()) {
		nations::create_nation_based_on_template(ws, trigger::to_nation(primary_slot), holder);
	}
	while(hprovs.begin() != hprovs.end()) {
		province::change_province_owner(ws, (*hprovs.begin()).get_province().id, trigger::to_nation(primary_slot));
	}
	return 0;
}
uint32_t ef_inherit_from_nation(EFFECT_PARAMTERS) {
	auto holder = trigger::to_nation(from_slot);
	if(holder == trigger::to_nation(primary_slot))
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	auto sprovs = ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot));
	if(sprovs.begin() == sprovs.end()) {
		nations::create_nation_based_on_template(ws, trigger::to_nation(primary_slot), holder);
	}
	while(hprovs.begin() != hprovs.end()) {
		province::change_province_owner(ws, (*hprovs.begin()).get_province().id, trigger::to_nation(primary_slot));
	}
	return 0;
}
uint32_t ef_inherit_from_province(EFFECT_PARAMTERS) {
	auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	if(!holder || holder == trigger::to_nation(primary_slot))
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	auto sprovs = ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot));
	if(sprovs.begin() == sprovs.end()) {
		nations::create_nation_based_on_template(ws, trigger::to_nation(primary_slot), holder);
	}
	while(hprovs.begin() != hprovs.end()) {
		province::change_province_owner(ws, (*hprovs.begin()).get_province().id, trigger::to_nation(primary_slot));
	}
	return 0;
}
uint32_t ef_annex_to(EFFECT_PARAMTERS) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(!holder || holder == trigger::to_nation(primary_slot))
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	auto sprovs = ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot));
	if(hprovs.begin() == hprovs.end()) {
		nations::create_nation_based_on_template(ws, holder, trigger::to_nation(primary_slot));
	}
	while(sprovs.begin() != sprovs.end()) {
		province::change_province_owner(ws, (*sprovs.begin()).get_province().id, holder);
	}
	return 0;
}
uint32_t ef_annex_to_this_nation(EFFECT_PARAMTERS) {
	auto holder = trigger::to_nation(this_slot);
	if(holder == trigger::to_nation(primary_slot))
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	auto sprovs = ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot));
	if(hprovs.begin() == hprovs.end()) {
		nations::create_nation_based_on_template(ws, holder, trigger::to_nation(primary_slot));
	}
	while(sprovs.begin() != sprovs.end()) {
		province::change_province_owner(ws, (*sprovs.begin()).get_province().id, holder);
	}
	return 0;
}
uint32_t ef_annex_to_this_state(EFFECT_PARAMTERS) {
	auto holder = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot));
	if(!holder || holder == trigger::to_nation(primary_slot))
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	auto sprovs = ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot));
	if(hprovs.begin() == hprovs.end()) {
		nations::create_nation_based_on_template(ws, holder, trigger::to_nation(primary_slot));
	}
	while(sprovs.begin() != sprovs.end()) {
		province::change_province_owner(ws, (*sprovs.begin()).get_province().id, holder);
	}
	return 0;
}
uint32_t ef_annex_to_this_province(EFFECT_PARAMTERS) {
	auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(!holder || holder == trigger::to_nation(primary_slot))
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	auto sprovs = ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot));
	if(hprovs.begin() == hprovs.end()) {
		nations::create_nation_based_on_template(ws, holder, trigger::to_nation(primary_slot));
	}
	while(sprovs.begin() != sprovs.end()) {
		province::change_province_owner(ws, (*sprovs.begin()).get_province().id, holder);
	}
	return 0;
}
uint32_t ef_annex_to_this_pop(EFFECT_PARAMTERS) {
	auto holder = nations::owner_of_pop(ws, trigger::to_pop(this_slot));
	if(!holder || holder == trigger::to_nation(primary_slot))
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	auto sprovs = ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot));
	if(hprovs.begin() == hprovs.end()) {
		nations::create_nation_based_on_template(ws, holder, trigger::to_nation(primary_slot));
	}
	while(sprovs.begin() != sprovs.end()) {
		province::change_province_owner(ws, (*sprovs.begin()).get_province().id, holder);
	}
	return 0;
}
uint32_t ef_annex_to_from_nation(EFFECT_PARAMTERS) {
	auto holder = trigger::to_nation(from_slot);
	if(holder == trigger::to_nation(primary_slot))
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	auto sprovs = ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot));
	if(hprovs.begin() == hprovs.end()) {
		nations::create_nation_based_on_template(ws, holder, trigger::to_nation(primary_slot));
	}
	while(sprovs.begin() != sprovs.end()) {
		province::change_province_owner(ws, (*sprovs.begin()).get_province().id, holder);
	}
	return 0;
}
uint32_t ef_annex_to_from_province(EFFECT_PARAMTERS) {
	auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	if(!holder || holder == trigger::to_nation(primary_slot))
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	auto sprovs = ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot));
	if(hprovs.begin() == hprovs.end()) {
		nations::create_nation_based_on_template(ws, holder, trigger::to_nation(primary_slot));
	}
	while(sprovs.begin() != sprovs.end()) {
		province::change_province_owner(ws, (*sprovs.begin()).get_province().id, holder);
	}
	return 0;
}
uint32_t ef_release(EFFECT_PARAMTERS) {
	nations::liberate_nation_from(ws, trigger::payload(tval[1]).tag_id, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_release_this_nation(EFFECT_PARAMTERS) {
	nations::liberate_nation_from(ws, ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(this_slot)),
			trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_release_this_state(EFFECT_PARAMTERS) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot));
	return ef_release_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
}
uint32_t ef_release_this_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	return ef_release_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
}
uint32_t ef_release_this_pop(EFFECT_PARAMTERS) {
	auto owner = nations::owner_of_pop(ws, trigger::to_pop(this_slot));
	return ef_release_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
}
uint32_t ef_release_from_nation(EFFECT_PARAMTERS) {
	nations::liberate_nation_from(ws, ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(from_slot)),
			trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_release_from_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	return ef_release_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo, els);
}
uint32_t ef_change_controller(EFFECT_PARAMTERS) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(holder) {
		province::set_province_controller(ws, trigger::to_prov(primary_slot), holder);
		military::eject_ships(ws, trigger::to_prov(primary_slot));
	}
	return 0;
}
uint32_t ef_change_controller_this_nation(EFFECT_PARAMTERS) {
	province::set_province_controller(ws, trigger::to_prov(primary_slot), trigger::to_nation(this_slot));
	military::eject_ships(ws, trigger::to_prov(primary_slot));
	return 0;
}
uint32_t ef_change_controller_this_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(owner) {
		province::set_province_controller(ws, trigger::to_prov(primary_slot), owner);
		military::eject_ships(ws, trigger::to_prov(primary_slot));
	}
	return 0;
}
uint32_t ef_change_controller_from_nation(EFFECT_PARAMTERS) {
	province::set_province_controller(ws, trigger::to_prov(primary_slot), trigger::to_nation(from_slot));
	military::eject_ships(ws, trigger::to_prov(primary_slot));
	return 0;
}
uint32_t ef_change_controller_from_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	if(owner) {
		province::set_province_controller(ws, trigger::to_prov(primary_slot), owner);
		military::eject_ships(ws, trigger::to_prov(primary_slot));
	}
	return 0;
}
uint32_t ef_change_controller_state(EFFECT_PARAMTERS) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(holder) {
		province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&ws, holder](dcon::province_id p) {
			province::set_province_controller(ws, p, holder);
			military::eject_ships(ws, p);
		});
	}
	return 0;
}
uint32_t ef_change_controller_state_this_nation(EFFECT_PARAMTERS) {
	auto holder = trigger::to_nation(this_slot);
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&ws, holder](dcon::province_id p) {
		province::set_province_controller(ws, p, holder);
		military::eject_ships(ws, p);
	});
	return 0;
}
uint32_t ef_change_controller_state_this_province(EFFECT_PARAMTERS) {
	auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(holder) {
		province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&ws, holder](dcon::province_id p) {
			province::set_province_controller(ws, p, holder);
			military::eject_ships(ws, p);
		});
	}
	return 0;
}
uint32_t ef_change_controller_state_from_nation(EFFECT_PARAMTERS) {
	auto holder = trigger::to_nation(from_slot);
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&ws, holder](dcon::province_id p) {
		province::set_province_controller(ws, p, holder);
		military::eject_ships(ws, p);
	});
	return 0;
}
uint32_t ef_change_controller_state_from_province(EFFECT_PARAMTERS) {
	auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	if(holder) {
		province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&ws, holder](dcon::province_id p) {
			province::set_province_controller(ws, p, holder);
			military::eject_ships(ws, p);
		});
	}
	return 0;
}
uint32_t ef_infrastructure(EFFECT_PARAMTERS) {
	auto& building_level = ws.world.province_get_building_level(trigger::to_prov(primary_slot), uint8_t(economy::province_building_type::railroad));
	building_level = uint8_t(std::clamp(int32_t(building_level) + int32_t(trigger::payload(tval[1]).signed_value),
		0,
		int32_t(ws.world.nation_get_max_building_level(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)), uint8_t(economy::province_building_type::railroad)))));
	ws.railroad_built.store(true, std::memory_order::release);
	return 0;
}
uint32_t ef_infrastructure_state(EFFECT_PARAMTERS) {
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
		auto& building_level = ws.world.province_get_building_level(p, uint8_t(economy::province_building_type::railroad));
		building_level = uint8_t(std::clamp(int32_t(building_level) + int32_t(trigger::payload(tval[1]).signed_value),
			0,
			int32_t(ws.world.nation_get_max_building_level(ws.world.province_get_nation_from_province_ownership(p), uint8_t(economy::province_building_type::railroad)))));
	});
	ws.railroad_built.store(true, std::memory_order::release);
	return 0;
}

uint32_t ef_money(EFFECT_PARAMTERS) {
	auto& m = ws.world.pop_get_savings(trigger::to_pop(primary_slot));
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	m = std::max(0.0f, m + amount);
	return 0;
}
uint32_t ef_leadership(EFFECT_PARAMTERS) {
	ws.world.nation_get_leadership_points(trigger::to_nation(primary_slot)) += float(trigger::payload(tval[1]).signed_value);
	return 0;
}
uint32_t ef_create_vassal(EFFECT_PARAMTERS) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(!holder)
		return 0;
	nations::make_vassal(ws, holder, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_create_vassal_this_nation(EFFECT_PARAMTERS) {
	nations::make_vassal(ws, trigger::to_nation(this_slot), trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_create_vassal_this_province(EFFECT_PARAMTERS) {
	auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(!holder)
		return 0;
	nations::make_vassal(ws, holder, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_create_vassal_from_nation(EFFECT_PARAMTERS) {
	nations::make_vassal(ws, trigger::to_nation(from_slot), trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_create_vassal_from_province(EFFECT_PARAMTERS) {
	auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	if(!holder)
		return 0;
	nations::make_vassal(ws, holder, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_end_military_access(EFFECT_PARAMTERS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(target)
		military::remove_military_access(ws, trigger::to_nation(primary_slot), target);
	return 0;
}
uint32_t ef_end_military_access_this_nation(EFFECT_PARAMTERS) {
	military::remove_military_access(ws, trigger::to_nation(primary_slot), trigger::to_nation(this_slot));
	return 0;
}
uint32_t ef_end_military_access_this_province(EFFECT_PARAMTERS) {
	auto target = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(target)
		military::remove_military_access(ws, trigger::to_nation(primary_slot), target);
	return 0;
}
uint32_t ef_end_military_access_from_nation(EFFECT_PARAMTERS) {
	military::remove_military_access(ws, trigger::to_nation(primary_slot), trigger::to_nation(from_slot));
	return 0;
}
uint32_t ef_end_military_access_from_province(EFFECT_PARAMTERS) {
	auto target = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	if(target)
		military::remove_military_access(ws, trigger::to_nation(primary_slot), target);
	return 0;
}
uint32_t ef_leave_alliance(EFFECT_PARAMTERS) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	nations::break_alliance(ws, holder, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_leave_alliance_this_nation(EFFECT_PARAMTERS) {
	nations::break_alliance(ws, trigger::to_nation(this_slot), trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_leave_alliance_this_province(EFFECT_PARAMTERS) {
	nations::break_alliance(ws, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)),
			trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_leave_alliance_from_nation(EFFECT_PARAMTERS) {
	nations::break_alliance(ws, trigger::to_nation(from_slot), trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_leave_alliance_from_province(EFFECT_PARAMTERS) {
	nations::break_alliance(ws, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)),
			trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_end_war(EFFECT_PARAMTERS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	military::end_wars_between(ws, trigger::to_nation(primary_slot), target);
	return 0;
}
uint32_t ef_end_war_this_nation(EFFECT_PARAMTERS) {
	military::end_wars_between(ws, trigger::to_nation(primary_slot), trigger::to_nation(this_slot));
	return 0;
}
uint32_t ef_end_war_this_province(EFFECT_PARAMTERS) {
	military::end_wars_between(ws, trigger::to_nation(primary_slot), ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)));
	return 0;
}
uint32_t ef_end_war_from_nation(EFFECT_PARAMTERS) {
	military::end_wars_between(ws, trigger::to_nation(primary_slot), trigger::to_nation(from_slot));
	return 0;
}
uint32_t ef_end_war_from_province(EFFECT_PARAMTERS) {
	military::end_wars_between(ws, trigger::to_nation(primary_slot), ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)));
	return 0;
}
uint32_t ef_enable_ideology(EFFECT_PARAMTERS) {
	ws.world.ideology_set_enabled(trigger::payload(tval[1]).ideo_id, true);
	return 0;
}
uint32_t ef_ruling_party_ideology(EFFECT_PARAMTERS) {
	politics::force_ruling_party_ideology(ws, trigger::to_nation(primary_slot), trigger::payload(tval[1]).ideo_id);
	return 0;
}
uint32_t ef_plurality(EFFECT_PARAMTERS) {
	auto& plur = ws.world.nation_get_plurality(trigger::to_nation(primary_slot));
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	plur = std::clamp(plur + amount, 0.0f, 100.0f);
	return 0;
}
uint32_t ef_remove_province_modifier(EFFECT_PARAMTERS) {
	sys::remove_modifier_from_province(ws, trigger::to_prov(primary_slot), trigger::payload(tval[1]).mod_id);
	return 0;
}
uint32_t ef_remove_province_modifier_state(EFFECT_PARAMTERS) {
	auto mod = trigger::payload(tval[1]).mod_id;
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
		sys::remove_modifier_from_province(ws, p, mod);
	});
	return 0;
}
uint32_t ef_remove_country_modifier(EFFECT_PARAMTERS) {
	sys::remove_modifier_from_nation(ws, trigger::to_nation(primary_slot), trigger::payload(tval[1]).mod_id);
	return 0;
}
uint32_t ef_create_alliance(EFFECT_PARAMTERS) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(holder && ws.world.nation_get_owned_province_count(holder) != 0 && ws.world.nation_get_owned_province_count(trigger::to_nation(primary_slot)) != 0)
		nations::make_alliance(ws, holder, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_create_alliance_this_nation(EFFECT_PARAMTERS) {
	if(ws.world.nation_get_owned_province_count(trigger::to_nation(this_slot)) != 0 && ws.world.nation_get_owned_province_count(trigger::to_nation(primary_slot)) != 0)
		nations::make_alliance(ws, trigger::to_nation(primary_slot), trigger::to_nation(this_slot));
	return 0;
}
uint32_t ef_create_alliance_this_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(owner && ws.world.nation_get_owned_province_count(owner) != 0 && ws.world.nation_get_owned_province_count(trigger::to_nation(primary_slot)) != 0)
		nations::make_alliance(ws, trigger::to_nation(primary_slot), owner);
	return 0;
}
uint32_t ef_create_alliance_from_nation(EFFECT_PARAMTERS) {
	if(ws.world.nation_get_owned_province_count(trigger::to_nation(from_slot)) != 0 && ws.world.nation_get_owned_province_count(trigger::to_nation(primary_slot)) != 0)
		nations::make_alliance(ws, trigger::to_nation(primary_slot), trigger::to_nation(from_slot));
	return 0;
}
uint32_t ef_create_alliance_from_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	if(owner && ws.world.nation_get_owned_province_count(owner) != 0 && ws.world.nation_get_owned_province_count(trigger::to_nation(primary_slot)) != 0)
		nations::make_alliance(ws, trigger::to_nation(primary_slot), owner);
	return 0;
}
uint32_t ef_release_vassal(EFFECT_PARAMTERS) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(!holder)
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	if(hprovs.begin() == hprovs.end()) {
		nations::liberate_nation_from(ws, trigger::payload(tval[1]).tag_id, trigger::to_nation(primary_slot));
		if(ws.world.nation_get_owned_province_count(holder) == 0)
			return 0;
		ws.world.force_create_overlord(holder, trigger::to_nation(primary_slot));
		if(ws.world.nation_get_is_great_power(trigger::to_nation(primary_slot))) {
			auto sr = ws.world.force_create_gp_relationship(holder, trigger::to_nation(primary_slot));
			auto& flags = ws.world.gp_relationship_get_status(sr);
			flags = uint8_t((flags & ~nations::influence::level_mask) | nations::influence::level_in_sphere);
			ws.world.nation_set_in_sphere_of(holder, trigger::to_nation(primary_slot));
		}
	} else {
		auto rel = ws.world.nation_get_overlord_as_subject(holder);
		if(rel) {
			nations::release_vassal(ws, rel);
		}
	}
	return 0;
}
uint32_t ef_release_vassal_this_nation(EFFECT_PARAMTERS) {
	auto hprovs = ws.world.nation_get_province_ownership(trigger::to_nation(this_slot));
	if(hprovs.begin() == hprovs.end()) {
		nations::liberate_nation_from(ws, ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(this_slot)),
				trigger::to_nation(primary_slot));
		if(ws.world.nation_get_owned_province_count(trigger::to_nation(this_slot)) == 0)
			return 0;
		ws.world.force_create_overlord(trigger::to_nation(this_slot), trigger::to_nation(primary_slot));
		if(ws.world.nation_get_is_great_power(trigger::to_nation(primary_slot))) {
			auto sr = ws.world.force_create_gp_relationship(trigger::to_nation(this_slot), trigger::to_nation(primary_slot));
			auto& flags = ws.world.gp_relationship_get_status(sr);
			flags = uint8_t((flags & ~nations::influence::level_mask) | nations::influence::level_in_sphere);
			ws.world.nation_set_in_sphere_of(trigger::to_nation(this_slot), trigger::to_nation(primary_slot));
		}
	} else {
		auto rel = ws.world.nation_get_overlord_as_subject(trigger::to_nation(this_slot));
		if(rel) {
			nations::release_vassal(ws, rel);
		}
	}
	return 0;
}
uint32_t ef_release_vassal_this_province(EFFECT_PARAMTERS) {
	auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(!holder)
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	if(hprovs.begin() == hprovs.end()) {
		nations::liberate_nation_from(ws, ws.world.nation_get_identity_from_identity_holder(holder),
				trigger::to_nation(primary_slot));
		if(ws.world.nation_get_owned_province_count(holder) == 0)
			return 0;
		ws.world.force_create_overlord(holder, trigger::to_nation(primary_slot));
		if(ws.world.nation_get_is_great_power(trigger::to_nation(primary_slot))) {
			auto sr = ws.world.force_create_gp_relationship(holder, trigger::to_nation(primary_slot));
			auto& flags = ws.world.gp_relationship_get_status(sr);
			flags = uint8_t((flags & ~nations::influence::level_mask) | nations::influence::level_in_sphere);
			ws.world.nation_set_in_sphere_of(holder, trigger::to_nation(primary_slot));
		}
	} else {
		auto rel = ws.world.nation_get_overlord_as_subject(holder);
		if(rel) {
			nations::release_vassal(ws, rel);
		}
	}
	return 0;
}
uint32_t ef_release_vassal_from_nation(EFFECT_PARAMTERS) {
	auto hprovs = ws.world.nation_get_province_ownership(trigger::to_nation(from_slot));
	if(hprovs.begin() == hprovs.end()) {
		nations::liberate_nation_from(ws, ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(this_slot)),
				trigger::to_nation(primary_slot));
		if(ws.world.nation_get_owned_province_count(trigger::to_nation(from_slot)) == 0)
			return 0;
		ws.world.force_create_overlord(trigger::to_nation(from_slot), trigger::to_nation(primary_slot));
		if(ws.world.nation_get_is_great_power(trigger::to_nation(primary_slot))) {
			auto sr = ws.world.force_create_gp_relationship(trigger::to_nation(from_slot), trigger::to_nation(primary_slot));
			auto& flags = ws.world.gp_relationship_get_status(sr);
			flags = uint8_t((flags & ~nations::influence::level_mask) | nations::influence::level_in_sphere);
			ws.world.nation_set_in_sphere_of(trigger::to_nation(from_slot), trigger::to_nation(primary_slot));
		}
	} else {
		auto rel = ws.world.nation_get_overlord_as_subject(trigger::to_nation(from_slot));
		if(rel) {
			nations::release_vassal(ws, rel);
		}
	}
	return 0;
}
uint32_t ef_release_vassal_from_province(EFFECT_PARAMTERS) {
	auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	if(!holder)
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	if(hprovs.begin() == hprovs.end()) {
		nations::liberate_nation_from(ws, ws.world.nation_get_identity_from_identity_holder(holder),
				trigger::to_nation(primary_slot));
		if(ws.world.nation_get_owned_province_count(holder) == 0)
			return 0;
		ws.world.force_create_overlord(holder, trigger::to_nation(primary_slot));
		if(ws.world.nation_get_is_great_power(trigger::to_nation(primary_slot))) {
			auto sr = ws.world.force_create_gp_relationship(holder, trigger::to_nation(primary_slot));
			auto& flags = ws.world.gp_relationship_get_status(sr);
			flags = uint8_t((flags & ~nations::influence::level_mask) | nations::influence::level_in_sphere);
			ws.world.nation_set_in_sphere_of(holder, trigger::to_nation(primary_slot));
		}
	} else {
		auto rel = ws.world.nation_get_overlord_as_subject(holder);
		if(rel) {
			nations::release_vassal(ws, rel);
		}
	}
	return 0;
}
uint32_t ef_release_vassal_reb(EFFECT_PARAMTERS) {
	auto itag = ws.world.rebel_faction_get_defection_target(trigger::to_rebel(from_slot));
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(itag);
	if(!holder)
		return 0;
	auto hprovs = ws.world.nation_get_province_ownership(holder);
	if(hprovs.begin() == hprovs.end()) {
		nations::liberate_nation_from(ws, itag, trigger::to_nation(primary_slot));
		if(ws.world.nation_get_owned_province_count(holder) == 0)
			return 0;
		ws.world.force_create_overlord(holder, trigger::to_nation(primary_slot));
		if(ws.world.nation_get_is_great_power(trigger::to_nation(primary_slot))) {
			auto sr = ws.world.force_create_gp_relationship(holder, trigger::to_nation(primary_slot));
			auto& flags = ws.world.gp_relationship_get_status(sr);
			flags = uint8_t((flags & ~nations::influence::level_mask) | nations::influence::level_in_sphere);
			ws.world.nation_set_in_sphere_of(holder, trigger::to_nation(primary_slot));
		}
	} else {
		auto rel = ws.world.nation_get_overlord_as_subject(holder);
		if(rel) {
			nations::release_vassal(ws, rel);
		}
	}
	return 0;
}
uint32_t ef_release_vassal_random(EFFECT_PARAMTERS) {
	// unused
	return 0;
}
uint32_t ef_release_vassal_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot));
	if(owner)
		return ef_release_vassal(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_release_vassal_province_this_nation(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot));
	if(owner)
		return ef_release_vassal_this_nation(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_release_vassal_province_this_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot));
	if(owner)
		return ef_release_vassal_this_province(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_release_vassal_province_from_nation(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot));
	if(owner)
		return ef_release_vassal_from_nation(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_release_vassal_province_from_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot));
	if(owner)
		return ef_release_vassal_from_province(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_release_vassal_province_reb(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot));
	if(owner)
		return ef_release_vassal_reb(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_release_vassal_province_random(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot));
	if(owner)
		return ef_release_vassal_random(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo, els);
	else
		return 0;
}
uint32_t ef_change_province_name(EFFECT_PARAMTERS) {
	dcon::text_key name{ dcon::text_key::value_base_t(trigger::read_int32_t_from_payload(tval + 1)) };
	ws.world.province_set_name(trigger::to_prov(primary_slot), name);
	return 0;
}
uint32_t ef_enable_canal(EFFECT_PARAMTERS) {
	province::enable_canal(ws, tval[1] - 1);
	return 0;
}
uint32_t ef_set_global_flag(EFFECT_PARAMTERS) {
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[1]).glob_id, true);
	return 0;
}
uint32_t ef_clr_global_flag(EFFECT_PARAMTERS) {
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[1]).glob_id, false);
	return 0;
}
uint32_t ef_nationalvalue_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot));
	if(owner)
		ws.world.nation_set_national_value(owner, trigger::payload(tval[1]).mod_id);
	return 0;
}
uint32_t ef_nationalvalue_nation(EFFECT_PARAMTERS) {
	ws.world.nation_set_national_value(trigger::to_nation(primary_slot), trigger::payload(tval[1]).mod_id);
	return 0;
}
uint32_t ef_civilized_yes(EFFECT_PARAMTERS) {
	nations::make_civilized(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_civilized_no(EFFECT_PARAMTERS) {
	nations::make_uncivilized(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_is_slave_state_no(EFFECT_PARAMTERS) {
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
		ws.world.province_set_is_slave(p, false);
		bool mine = ws.world.commodity_get_is_mine(ws.world.province_get_rgo(p));
		for(auto pop : ws.world.province_get_pop_location(p)) {
			if(pop.get_pop().get_poptype() == ws.culture_definitions.slaves) {
				pop.get_pop().set_poptype(mine ? ws.culture_definitions.laborers : ws.culture_definitions.farmers);
			}
		}
	});
	return 0;
}
uint32_t ef_is_slave_pop_no(EFFECT_PARAMTERS) {
	if(ws.world.pop_get_poptype(trigger::to_pop(primary_slot)) == ws.culture_definitions.slaves) {
		bool mine = ws.world.commodity_get_is_mine(
				ws.world.province_get_rgo(ws.world.pop_get_province_from_pop_location(trigger::to_pop(primary_slot))));
		ws.world.pop_set_poptype(trigger::to_pop(primary_slot),
				mine ? ws.culture_definitions.laborers : ws.culture_definitions.farmers);
	}
	return 0;
}
uint32_t ef_is_slave_province_no(EFFECT_PARAMTERS) {
	auto p = trigger::to_prov(primary_slot);
	ws.world.province_set_is_slave(p, false);
	bool mine = ws.world.commodity_get_is_mine(ws.world.province_get_rgo(p));
	for(auto pop : ws.world.province_get_pop_location(p)) {
		if(pop.get_pop().get_poptype() == ws.culture_definitions.slaves) {
			pop.get_pop().set_poptype(mine ? ws.culture_definitions.laborers : ws.culture_definitions.farmers);
		}
	}
	return 0;
}
uint32_t ef_election(EFFECT_PARAMTERS) {
	politics::start_election(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_social_reform(EFFECT_PARAMTERS) {
	auto opt = trigger::payload(tval[1]).opt_id;
	politics::set_issue_option(ws, trigger::to_nation(primary_slot), opt);
	culture::update_nation_issue_rules(ws, trigger::to_nation(primary_slot));
	sys::update_single_nation_modifiers(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_social_reform_province(EFFECT_PARAMTERS) {
	auto opt = trigger::payload(tval[1]).opt_id;
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot));
	if(owner) {
		politics::set_issue_option(ws, owner, opt);
		culture::update_nation_issue_rules(ws, owner);
		sys::update_single_nation_modifiers(ws, owner);
	}
	return 0;
}
uint32_t ef_political_reform(EFFECT_PARAMTERS) {
	auto opt = trigger::payload(tval[1]).opt_id;
	politics::set_issue_option(ws, trigger::to_nation(primary_slot), opt);
	culture::update_nation_issue_rules(ws, trigger::to_nation(primary_slot));
	sys::update_single_nation_modifiers(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_political_reform_province(EFFECT_PARAMTERS) {
	auto opt = trigger::payload(tval[1]).opt_id;
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot));
	if(owner) {
		politics::set_issue_option(ws, owner, opt);
		culture::update_nation_issue_rules(ws, owner);
		sys::update_single_nation_modifiers(ws, owner);
	}
	return 0;
}
uint32_t ef_add_tax_relative_income(EFFECT_PARAMTERS) {
	auto income = ws.world.nation_get_total_poor_income(trigger::to_nation(primary_slot)) +
		ws.world.nation_get_total_middle_income(trigger::to_nation(primary_slot)) +
		ws.world.nation_get_total_rich_income(trigger::to_nation(primary_slot));
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	auto combined_amount = income * amount;
	assert(std::isfinite(combined_amount));
	auto& v = ws.world.nation_get_stockpiles(trigger::to_nation(primary_slot), economy::money);

	if(ws.world.nation_get_is_player_controlled(trigger::to_nation(primary_slot)))
		v = v + combined_amount;
	else
		v = std::max(v + combined_amount, 0.0f); // temporary measure since there is no debt
	return 0;
}
uint32_t ef_neutrality(EFFECT_PARAMTERS) {
	nations::destroy_diplomatic_relationships(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_reduce_pop(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	ws.world.pop_get_size(trigger::to_pop(primary_slot)) *= amount;
	return 0;
}
uint32_t ef_reduce_pop_abs(EFFECT_PARAMTERS) {
	auto amount = trigger::read_int32_t_from_payload(tval + 1);
	
	demographics::reduce_pop_size_safe(ws, trigger::to_pop(primary_slot), amount);
	return 0;
}
uint32_t ef_reduce_pop_province(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	for(auto p : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
		p.get_pop().get_size() *= amount;
	}
	return 0;
}
uint32_t ef_reduce_pop_nation(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	for(auto pr : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
		for(auto p : pr.get_province().get_pop_location()) {
			p.get_pop().get_size() *= amount;
		}
	}
	return 0;
}
uint32_t ef_reduce_pop_state(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&ws, amount](dcon::province_id pr) {
		for(auto p : ws.world.province_get_pop_location(pr)) {
			p.get_pop().get_size() *= amount;
		}
	});
	return 0;
}
uint32_t ef_move_pop(EFFECT_PARAMTERS) {
	ws.world.pop_set_province_from_pop_location(trigger::to_pop(primary_slot), trigger::payload(tval[1]).prov_id);
	return 0;
}
uint32_t ef_pop_type(EFFECT_PARAMTERS) {
	ws.world.pop_set_poptype(trigger::to_pop(primary_slot), trigger::payload(tval[1]).popt_id);
	return 0;
}
uint32_t ef_years_of_research(EFFECT_PARAMTERS) {
	auto& rp = ws.world.nation_get_research_points(trigger::to_nation(primary_slot));
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	rp += nations::daily_research_points(ws, trigger::to_nation(primary_slot)) * 365.0f * amount;
	return 0;
}
uint32_t ef_prestige_factor_positive(EFFECT_PARAMTERS) {
	auto& bp = ws.world.nation_get_prestige(trigger::to_nation(primary_slot));
	auto score = nations::prestige_score(ws, trigger::to_nation(primary_slot));
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));

	bp += (score) *
		(ws.world.nation_get_modifier_values(trigger::to_nation(primary_slot), sys::national_mod_offsets::prestige) + 1.0f) *
		amount;
	return 0;
}
uint32_t ef_prestige_factor_negative(EFFECT_PARAMTERS) {
	auto& bp = ws.world.nation_get_prestige(trigger::to_nation(primary_slot));
	auto score = nations::prestige_score(ws, trigger::to_nation(primary_slot));
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));

	bp = std::max(0.0f, bp + (score)*amount);
	return 0;
}
uint32_t ef_military_reform(EFFECT_PARAMTERS) {
	auto opt = trigger::payload(tval[1]).ropt_id;
	politics::set_reform_option(ws, trigger::to_nation(primary_slot), opt);
	culture::update_nation_issue_rules(ws, trigger::to_nation(primary_slot));
	sys::update_single_nation_modifiers(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_economic_reform(EFFECT_PARAMTERS) {
	auto opt = trigger::payload(tval[1]).ropt_id;
	politics::set_reform_option(ws, trigger::to_nation(primary_slot), opt);
	culture::update_nation_issue_rules(ws, trigger::to_nation(primary_slot));
	sys::update_single_nation_modifiers(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_remove_random_military_reforms(EFFECT_PARAMTERS) {
	std::vector<dcon::reform_option_id> active_reforms;
	auto nation_id = trigger::to_nation(primary_slot);
	for(auto issue : ws.world.in_reform) {
		if(issue.get_reform_type() == uint8_t(culture::issue_type::military) &&
				ws.world.nation_get_reforms(nation_id, issue) != ws.world.reform_get_options(issue)[0])
			active_reforms.push_back(ws.world.reform_get_options(issue)[0]);
	}
	for(int32_t i = tval[1] - 1; active_reforms.size() != 0 && i >= 0; --i) {
		auto r = rng::get_random(ws, r_hi, uint32_t(r_lo + i)) % active_reforms.size();
		politics::set_reform_option(ws, nation_id, active_reforms[r]);
		active_reforms[r] = active_reforms.back();
		active_reforms.pop_back();
	}
	culture::update_nation_issue_rules(ws, nation_id);
	sys::update_single_nation_modifiers(ws, nation_id);
	return tval[1];
}
uint32_t ef_remove_random_economic_reforms(EFFECT_PARAMTERS) {
	std::vector<dcon::reform_option_id> active_reforms;
	auto nation_id = trigger::to_nation(primary_slot);
	for(auto issue : ws.world.in_reform) {
		if(issue.get_reform_type() == uint8_t(culture::issue_type::economic) &&
				ws.world.nation_get_reforms(nation_id, issue) != ws.world.reform_get_options(issue)[0])
			active_reforms.push_back(ws.world.reform_get_options(issue)[0]);
	}
	for(int32_t i = tval[1] - 1; active_reforms.size() != 0 && i >= 0; --i) {
		auto r = rng::get_random(ws, r_hi, uint32_t(r_lo + i)) % active_reforms.size();
		politics::set_reform_option(ws, nation_id, active_reforms[r]);
		active_reforms[r] = active_reforms.back();
		active_reforms.pop_back();
	}
	culture::update_nation_issue_rules(ws, nation_id);
	sys::update_single_nation_modifiers(ws, nation_id);
	return tval[1];
}
uint32_t ef_add_crime(EFFECT_PARAMTERS) {
	ws.world.province_set_crime(trigger::to_prov(primary_slot), trigger::payload(tval[1]).crm_id);
	return 0;
}
uint32_t ef_add_crime_none(EFFECT_PARAMTERS) {
	ws.world.province_set_crime(trigger::to_prov(primary_slot), dcon::crime_id{});
	return 0;
}
uint32_t ef_nationalize(EFFECT_PARAMTERS) {
	nations::perform_nationalization(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_build_factory_in_capital_state(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	auto cs = ws.world.province_get_state_membership(c);
	if(!cs)
		return 0;
	economy::try_add_factory_to_state(ws, cs, trigger::payload(tval[1]).fac_id);
	return 0;
}
uint32_t ef_activate_technology(EFFECT_PARAMTERS) {
	if(ws.world.nation_get_active_technologies(trigger::to_nation(primary_slot), trigger::payload(tval[1]).tech_id) == false)
		culture::apply_technology(ws, trigger::to_nation(primary_slot), trigger::payload(tval[1]).tech_id);
	return 0;
}
uint32_t ef_activate_invention(EFFECT_PARAMTERS) {
	if(ws.world.nation_get_active_inventions(trigger::to_nation(primary_slot), trigger::payload(tval[1]).invt_id) == true)
		culture::apply_invention(ws, trigger::to_nation(primary_slot), trigger::payload(tval[1]).invt_id);
	return 0;
}
uint32_t ef_great_wars_enabled_yes(EFFECT_PARAMTERS) {
	ws.military_definitions.great_wars_enabled = true;
	return 0;
}
uint32_t ef_great_wars_enabled_no(EFFECT_PARAMTERS) {
	ws.military_definitions.great_wars_enabled = false;
	return 0;
}
uint32_t ef_world_wars_enabled_yes(EFFECT_PARAMTERS) {
	ws.military_definitions.world_wars_enabled = true;
	return 0;
}
uint32_t ef_world_wars_enabled_no(EFFECT_PARAMTERS) {
	ws.military_definitions.world_wars_enabled = false;
	return 0;
}
uint32_t ef_assimilate_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner) {
		auto owner_c = ws.world.nation_get_primary_culture(owner);
		for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
			pop.get_pop().set_culture(owner_c);
			pop.get_pop().set_is_primary_or_accepted_culture(true);
		}
	}
	return 0;
}
uint32_t ef_assimilate_state(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot)); owner) {
		auto owner_c = ws.world.nation_get_primary_culture(owner);
		province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
			for(auto pop : ws.world.province_get_pop_location(p)) {
				pop.get_pop().set_culture(owner_c);
				pop.get_pop().set_is_primary_or_accepted_culture(true);
			}
		});
	}
	return 0;
}
uint32_t ef_assimilate_pop(EFFECT_PARAMTERS) {
	if(auto owner = nations::owner_of_pop(ws, trigger::to_pop(primary_slot)); owner) {
		auto owner_c = ws.world.nation_get_primary_culture(owner);
		ws.world.pop_set_culture(trigger::to_pop(primary_slot), owner_c);
		ws.world.pop_set_is_primary_or_accepted_culture(trigger::to_pop(primary_slot), true);
	}
	return 0;
}
uint32_t ef_set_culture_pop(EFFECT_PARAMTERS) {
	if(auto owner = nations::owner_of_pop(ws, trigger::to_pop(primary_slot)); owner) {
		auto c = trigger::payload(tval[1]).cul_id;
		ws.world.pop_set_culture(trigger::to_pop(primary_slot), c);
		ws.world.pop_set_is_primary_or_accepted_culture(trigger::to_pop(primary_slot), nations::nation_accepts_culture(ws, owner, c));
	}
	return 0;
}
uint32_t ef_literacy(EFFECT_PARAMTERS) {
	auto l = pop_demographics::get_literacy(ws, trigger::to_pop(primary_slot));
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	pop_demographics::set_literacy(ws, trigger::to_pop(primary_slot),  std::clamp(l + amount, 0.0f, 1.0f));
	return 0;
}
uint32_t ef_add_crisis_interest(EFFECT_PARAMTERS) {
	if(ws.current_crisis != sys::crisis_type::none && ws.current_crisis_mode == sys::crisis_mode::heating_up) {
		for(auto& im : ws.crisis_participants) {
			if(im.id == trigger::to_nation(primary_slot)) {
				return 0;
			}
			if(!im.id) {
				im.id = trigger::to_nation(primary_slot);
				im.merely_interested = true;
				im.supports_attacker = false;
				return 0;
			}
		}
	}
	return 0;
}
uint32_t ef_flashpoint_tension(EFFECT_PARAMTERS) {
	auto& current_tension = ws.world.state_instance_get_flashpoint_tension(trigger::to_state(primary_slot));
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));

	current_tension = std::clamp(current_tension + amount, 0.0f, 100.0f);
	return 0;
}
uint32_t ef_flashpoint_tension_province(EFFECT_PARAMTERS) {
	auto state = ws.world.province_get_state_membership(trigger::to_prov(primary_slot));
	if(!state)
		return 0;

	auto& current_tension = ws.world.state_instance_get_flashpoint_tension(state);
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));

	current_tension = std::clamp(current_tension + amount, 0.0f, 100.0f);
	return 0;
}
uint32_t ef_add_crisis_temperature(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));

	ws.crisis_temperature = std::clamp(ws.crisis_temperature + amount, 0.0f, 100.0f);
	return 0;
}
uint32_t ef_consciousness(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));

	auto c = pop_demographics::get_consciousness(ws, trigger::to_pop(primary_slot));
	pop_demographics::set_consciousness(ws, trigger::to_pop(primary_slot), std::clamp(c + amount, 0.0f, 10.0f));
	return 0;
}
uint32_t ef_consciousness_province(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	for(auto p : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
		auto c = pop_demographics::get_consciousness(ws, p.get_pop());
		pop_demographics::set_consciousness(ws, p.get_pop(), std::clamp(c + amount, 0.0f, 10.0f));
	}
	return 0;
}
uint32_t ef_consciousness_nation(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	for(auto pr : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
		for(auto p : pr.get_province().get_pop_location()) {
			auto c = pop_demographics::get_consciousness(ws, p.get_pop());
			pop_demographics::set_consciousness(ws, p.get_pop(), std::clamp(c + amount, 0.0f, 10.0f));
		}
	}
	return 0;
}
uint32_t ef_consciousness_state(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&ws, amount](dcon::province_id pr) {
		for(auto p : ws.world.province_get_pop_location(pr)) {
			auto c = pop_demographics::get_consciousness(ws, p.get_pop());
			pop_demographics::set_consciousness(ws, p.get_pop(), std::clamp(c + amount, 0.0f, 10.0f));
		}
	});
	return 0;
}
uint32_t ef_militancy(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));

	auto c = pop_demographics::get_militancy(ws, trigger::to_pop(primary_slot));
	pop_demographics::set_militancy(ws, trigger::to_pop(primary_slot), std::clamp(c + amount, 0.0f, 10.0f));
	return 0;
}
uint32_t ef_militancy_province(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	for(auto p : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
		auto c = pop_demographics::get_militancy(ws, p.get_pop());
		pop_demographics::set_militancy(ws, p.get_pop(), std::clamp(c + amount, 0.0f, 10.0f));
	}
	return 0;
}
uint32_t ef_militancy_nation(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	for(auto pr : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
		for(auto p : pr.get_province().get_pop_location()) {
			auto c = pop_demographics::get_militancy(ws, p.get_pop());
			pop_demographics::set_militancy(ws, p.get_pop(), std::clamp(c + amount, 0.0f, 10.0f));
		}
	}
	return 0;
}
uint32_t ef_militancy_state(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&ws, amount](dcon::province_id pr) {
		for(auto p : ws.world.province_get_pop_location(pr)) {
			auto c = pop_demographics::get_militancy(ws, p.get_pop());
			pop_demographics::set_militancy(ws, p.get_pop(), std::clamp(c + amount, 0.0f, 10.0f));
		}
	});
	return 0;
}
uint32_t ef_rgo_size(EFFECT_PARAMTERS) {
	auto& s = ws.world.province_get_rgo_size(trigger::to_prov(primary_slot));
	s = std::max(s + float(trigger::payload(tval[1]).signed_value), 0.0f);
	return 0;
}
uint32_t ef_fort(EFFECT_PARAMTERS) {
	auto& building_level = ws.world.province_get_building_level(trigger::to_prov(primary_slot), uint8_t(economy::province_building_type::fort));
	building_level = uint8_t(std::clamp(int32_t(building_level) + int32_t(trigger::payload(tval[1]).signed_value),
		0,
		int32_t(ws.world.nation_get_max_building_level(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)), uint8_t(economy::province_building_type::fort)))));
	return 0;
}
uint32_t ef_naval_base(EFFECT_PARAMTERS) {
	auto& building_level = ws.world.province_get_building_level(trigger::to_prov(primary_slot), uint8_t(economy::province_building_type::naval_base));
	building_level = uint8_t(std::clamp(int32_t(building_level) + int32_t(trigger::payload(tval[1]).signed_value), 0, int32_t(ws.world.nation_get_max_building_level(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)), uint8_t(economy::province_building_type::naval_base)))));
	if(building_level > 0) {
		auto si = ws.world.province_get_state_membership(trigger::to_prov(primary_slot));
		ws.world.state_instance_set_naval_base_is_taken(si, true);
	}
	return 0;
}
uint32_t ef_bank(EFFECT_PARAMTERS) {
	auto& building_level = ws.world.province_get_building_level(trigger::to_prov(primary_slot), uint8_t(economy::province_building_type::bank));
	building_level = uint8_t(std::clamp(int32_t(building_level) + int32_t(trigger::payload(tval[1]).signed_value), 0, int32_t(ws.world.nation_get_max_building_level(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)), uint8_t(economy::province_building_type::bank)))));
	return 0;
}
uint32_t ef_university(EFFECT_PARAMTERS) {
	auto& building_level = ws.world.province_get_building_level(trigger::to_prov(primary_slot), uint8_t(economy::province_building_type::university));
	building_level = uint8_t(std::clamp(int32_t(building_level) + int32_t(trigger::payload(tval[1]).signed_value), 0, int32_t(ws.world.nation_get_max_building_level(ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)), uint8_t(economy::province_building_type::university)))));
	return 0;
}
uint32_t ef_fort_state(EFFECT_PARAMTERS) {
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
		auto& building_level = ws.world.province_get_building_level(p, uint8_t(economy::province_building_type::fort));
		building_level = uint8_t(std::clamp(int32_t(building_level) + int32_t(trigger::payload(tval[1]).signed_value), 0, int32_t(ws.world.nation_get_max_building_level(ws.world.province_get_nation_from_province_ownership(p), uint8_t(economy::province_building_type::fort)))));
	});
	return 0;
}
uint32_t ef_naval_base_state(EFFECT_PARAMTERS) {
	uint32_t lvl = 0;
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
		auto& building_level = ws.world.province_get_building_level(p, uint8_t(economy::province_building_type::naval_base));
		building_level = uint8_t(std::clamp(int32_t(building_level) + int32_t(trigger::payload(tval[1]).signed_value), 0, int32_t(ws.world.nation_get_max_building_level(ws.world.province_get_nation_from_province_ownership(p), uint8_t(economy::province_building_type::naval_base)))));
		lvl = std::max<uint32_t>(lvl, building_level);
	});
	if(lvl > 0) {
		auto si = ws.world.province_get_state_membership(trigger::to_prov(primary_slot));
		ws.world.state_instance_set_naval_base_is_taken(si, true);
	}
	return 0;
}
uint32_t ef_bank_state(EFFECT_PARAMTERS) {
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
		auto& building_level = ws.world.province_get_building_level(p, uint8_t(economy::province_building_type::bank));
		building_level = uint8_t(std::clamp(int32_t(building_level) + int32_t(trigger::payload(tval[1]).signed_value), 0, int32_t(ws.world.nation_get_max_building_level(ws.world.province_get_nation_from_province_ownership(p), uint8_t(economy::province_building_type::bank)))));
	});
	return 0;
}
uint32_t ef_university_state(EFFECT_PARAMTERS) {
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
		auto& building_level = ws.world.province_get_building_level(p, uint8_t(economy::province_building_type::university));
		building_level = uint8_t(std::clamp(int32_t(building_level) + int32_t(trigger::payload(tval[1]).signed_value), 0, int32_t(ws.world.nation_get_max_building_level(ws.world.province_get_nation_from_province_ownership(p), uint8_t(economy::province_building_type::university)))));
	});
	return 0;
}

uint32_t ef_trigger_revolt_nation(EFFECT_PARAMTERS) {
	rebel::trigger_revolt(ws, trigger::to_nation(primary_slot), trigger::payload(tval[1]).reb_id, trigger::payload(tval[4]).ideo_id,
			trigger::payload(tval[2]).cul_id, trigger::payload(tval[3]).rel_id);
	return 0;
}
uint32_t ef_trigger_revolt_state(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot)); owner)
		rebel::trigger_revolt(ws, owner, trigger::payload(tval[1]).reb_id, trigger::payload(tval[4]).ideo_id,
				trigger::payload(tval[2]).cul_id, trigger::payload(tval[3]).rel_id);
	return 0;
}
uint32_t ef_trigger_revolt_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		rebel::trigger_revolt(ws, owner, trigger::payload(tval[1]).reb_id, trigger::payload(tval[4]).ideo_id,
				trigger::payload(tval[2]).cul_id, trigger::payload(tval[3]).rel_id);
	return 0;
}
uint32_t ef_diplomatic_influence(EFFECT_PARAMTERS) {
	if(auto holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id); holder)
		nations::adjust_influence_with_overflow(ws, trigger::to_nation(primary_slot), holder,
				float(trigger::payload(tval[2]).signed_value));
	return 0;
}
uint32_t ef_diplomatic_influence_this_nation(EFFECT_PARAMTERS) {
	nations::adjust_influence_with_overflow(ws, trigger::to_nation(primary_slot), trigger::to_nation(this_slot),
			float(trigger::payload(tval[1]).signed_value));
	return 0;
}
uint32_t ef_diplomatic_influence_this_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)); owner)
		nations::adjust_influence_with_overflow(ws, trigger::to_nation(primary_slot), owner,
				float(trigger::payload(tval[1]).signed_value));
	return 0;
}
uint32_t ef_diplomatic_influence_from_nation(EFFECT_PARAMTERS) {
	nations::adjust_influence_with_overflow(ws, trigger::to_nation(primary_slot), trigger::to_nation(from_slot),
			float(trigger::payload(tval[1]).signed_value));
	return 0;
}
uint32_t ef_diplomatic_influence_from_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)); owner)
		nations::adjust_influence_with_overflow(ws, trigger::to_nation(primary_slot), owner,
				float(trigger::payload(tval[1]).signed_value));
	return 0;
}
uint32_t ef_relation(EFFECT_PARAMTERS) {
	if(auto holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id); holder)
		nations::adjust_relationship(ws, trigger::to_nation(primary_slot), holder, float(trigger::payload(tval[2]).signed_value));
	return 0;
}
uint32_t ef_relation_this_nation(EFFECT_PARAMTERS) {
	nations::adjust_relationship(ws, trigger::to_nation(primary_slot), trigger::to_nation(this_slot),
			float(trigger::payload(tval[1]).signed_value));
	return 0;
}
uint32_t ef_relation_this_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)); owner)
		nations::adjust_relationship(ws, trigger::to_nation(primary_slot), owner, float(trigger::payload(tval[1]).signed_value));
	return 0;
}
uint32_t ef_relation_from_nation(EFFECT_PARAMTERS) {
	nations::adjust_relationship(ws, trigger::to_nation(primary_slot), trigger::to_nation(from_slot),
			float(trigger::payload(tval[1]).signed_value));
	return 0;
}
uint32_t ef_relation_from_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)); owner)
		nations::adjust_relationship(ws, trigger::to_nation(primary_slot), owner, float(trigger::payload(tval[1]).signed_value));
	return 0;
}
uint32_t ef_add_province_modifier(EFFECT_PARAMTERS) {
	sys::add_modifier_to_province(ws, trigger::to_prov(primary_slot), trigger::payload(tval[1]).mod_id,
			ws.current_date + trigger::payload(tval[2]).signed_value);
	return 0;
}
uint32_t ef_add_province_modifier_no_duration(EFFECT_PARAMTERS) {
	sys::add_modifier_to_province(ws, trigger::to_prov(primary_slot), trigger::payload(tval[1]).mod_id, sys::date{});
	return 0;
}
uint32_t ef_add_province_modifier_state(EFFECT_PARAMTERS) {
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
		sys::add_modifier_to_province(ws, p, trigger::payload(tval[1]).mod_id, ws.current_date + trigger::payload(tval[2]).signed_value);
	});
	return 0;
}
uint32_t ef_add_province_modifier_state_no_duration(EFFECT_PARAMTERS) {
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
		sys::add_modifier_to_province(ws, p, trigger::payload(tval[1]).mod_id, sys::date{});
	});
	return 0;
}
uint32_t ef_add_country_modifier(EFFECT_PARAMTERS) {
	sys::add_modifier_to_nation(ws, trigger::to_nation(primary_slot), trigger::payload(tval[1]).mod_id,
			ws.current_date + trigger::payload(tval[2]).signed_value);
	return 0;
}
uint32_t ef_add_country_modifier_no_duration(EFFECT_PARAMTERS) {
	sys::add_modifier_to_nation(ws, trigger::to_nation(primary_slot), trigger::payload(tval[1]).mod_id, sys::date{});
	return 0;
}
// Effects give "blank check" CBs that can be used on any state, thus target state is empty.
// Scenario of a CB without target state is to be correctly handled in the UI and game logic.
uint32_t ef_casus_belli_tag(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	auto tag_target = trigger::payload(tval[3]).tag_id;

	if(auto holder = ws.world.national_identity_get_nation_from_identity_holder(tag_target); holder) {
		ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot))
			.push_back(military::available_cb{ months > 0 ? ws.current_date + 31 * months : sys::date{}, holder, type , dcon::state_definition_id{} });
	}
	return 0;
}
uint32_t ef_casus_belli_int(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = ws.world.province_get_nation_from_province_ownership(trigger::payload(tval[3]).prov_id); holder) {
		ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot))
			.push_back(military::available_cb{ months > 0 ? ws.current_date + 31 * months : sys::date{}, holder, type, dcon::state_definition_id{} });
	}
	return 0;
}
uint32_t ef_casus_belli_this_nation(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot))
		.push_back(
				military::available_cb{ months > 0 ? ws.current_date + 31 * months : sys::date{}, trigger::to_nation(this_slot), type, dcon::state_definition_id{} });

	return 0;
}
uint32_t ef_casus_belli_this_state(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)); holder) {
		ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot))
			.push_back(military::available_cb{ months > 0 ? ws.current_date + 31 * months : sys::date{}, holder, type, dcon::state_definition_id{} });
	}
	return 0;
}
uint32_t ef_casus_belli_this_province(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)); holder) {
		ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot))
			.push_back(military::available_cb{ months > 0 ? ws.current_date + 31 * months : sys::date{}, holder, type, dcon::state_definition_id{} });
	}
	return 0;
}
uint32_t ef_casus_belli_this_pop(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = nations::owner_of_pop(ws, trigger::to_pop(this_slot)); holder) {
		ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot))
			.push_back(military::available_cb{ months > 0 ? ws.current_date + 31 * months : sys::date{}, holder, type, dcon::state_definition_id{} });
	}
	return 0;
}
uint32_t ef_casus_belli_from_nation(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot))
		.push_back(
				military::available_cb{ months > 0 ? ws.current_date + 31 * months : sys::date{}, trigger::to_nation(from_slot), type, dcon::state_definition_id{} });

	return 0;
}
uint32_t ef_casus_belli_from_province(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)); holder) {
		ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot))
			.push_back(military::available_cb{ months > 0 ? ws.current_date + 31 * months : sys::date{}, holder, type, dcon::state_definition_id{} });
	}
	return 0;
}
uint32_t ef_add_casus_belli_tag(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	auto tag_target = trigger::payload(tval[3]).tag_id;

	if(auto holder = ws.world.national_identity_get_nation_from_identity_holder(tag_target); holder) {
		ws.world.nation_get_available_cbs(holder).push_back(
				military::available_cb{ months > 0 ? ws.current_date + 31 * months : sys::date{}, trigger::to_nation(primary_slot), type, dcon::state_definition_id{} });
	}
	return 0;
}
uint32_t ef_add_casus_belli_int(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = ws.world.province_get_nation_from_province_ownership(trigger::payload(tval[3]).prov_id); holder) {
		ws.world.nation_get_available_cbs(holder).push_back(
				military::available_cb{ months > 0 ? ws.current_date + 31 * months : sys::date{}, trigger::to_nation(primary_slot), type, dcon::state_definition_id{} });
	}
	return 0;
}
uint32_t ef_add_casus_belli_this_nation(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	ws.world.nation_get_available_cbs(trigger::to_nation(this_slot))
		.push_back(military::available_cb{ months > 0 ? ws.current_date + 31 * months : sys::date{}, trigger::to_nation(primary_slot), type, dcon::state_definition_id{} });
	return 0;
}
uint32_t ef_add_casus_belli_this_state(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)); holder) {
		ws.world.nation_get_available_cbs(holder).push_back(
				military::available_cb{ months > 0 ? ws.current_date + 31 * months : sys::date{}, trigger::to_nation(primary_slot), type, dcon::state_definition_id{} });
	}
	return 0;
}
uint32_t ef_add_casus_belli_this_province(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)); holder) {
		ws.world.nation_get_available_cbs(holder).push_back(
				military::available_cb{ months > 0 ? ws.current_date + 31 * months : sys::date{}, trigger::to_nation(primary_slot), type, dcon::state_definition_id{} });
	}
	return 0;
}
uint32_t ef_add_casus_belli_this_pop(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = nations::owner_of_pop(ws, trigger::to_pop(this_slot)); holder) {
		ws.world.nation_get_available_cbs(holder).push_back(
				military::available_cb{ months > 0 ? ws.current_date + 31 * months : sys::date{}, trigger::to_nation(primary_slot), type, dcon::state_definition_id{} });
	}
	return 0;
}
uint32_t ef_add_casus_belli_from_nation(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	ws.world.nation_get_available_cbs(trigger::to_nation(from_slot))
		.push_back(military::available_cb{ months > 0 ? ws.current_date + 31 * months : sys::date{}, trigger::to_nation(primary_slot), type, dcon::state_definition_id{} });
	return 0;
}
uint32_t ef_add_casus_belli_from_province(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)); holder) {
		ws.world.nation_get_available_cbs(holder).push_back(
			military::available_cb{ months > 0 ? ws.current_date + 31 * months : sys::date{}, trigger::to_nation(primary_slot), type, dcon::state_definition_id{} });
	}
	return 0;
}
uint32_t ef_remove_casus_belli_tag(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto tag_target = trigger::payload(tval[2]).tag_id;

	auto holder = ws.world.national_identity_get_nation_from_identity_holder(tag_target);
	auto cbs = ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot));
	for(uint32_t i = cbs.size(); i-- > 0;) {
		if(cbs.at(i).cb_type == type && cbs.at(i).target == holder)
			cbs.remove_at(i);
	}
	return 0;
}
uint32_t ef_remove_casus_belli_int(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;

	auto holder = ws.world.province_get_nation_from_province_ownership(trigger::payload(tval[2]).prov_id);
	auto cbs = ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot));
	for(uint32_t i = cbs.size(); i-- > 0;) {
		if(cbs.at(i).cb_type == type && cbs.at(i).target == holder)
			cbs.remove_at(i);
	}
	return 0;
}
uint32_t ef_remove_casus_belli_this_nation(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;

	auto holder = trigger::to_nation(this_slot);
	auto cbs = ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot));
	for(uint32_t i = cbs.size(); i-- > 0;) {
		if(cbs.at(i).cb_type == type && cbs.at(i).target == holder)
			cbs.remove_at(i);
	}
	return 0;
}
uint32_t ef_remove_casus_belli_this_state(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;

	auto holder = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot));
	auto cbs = ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot));
	for(uint32_t i = cbs.size(); i-- > 0;) {
		if(cbs.at(i).cb_type == type && cbs.at(i).target == holder)
			cbs.remove_at(i);
	}
	return 0;
}
uint32_t ef_remove_casus_belli_this_province(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;

	auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	auto cbs = ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot));
	for(uint32_t i = cbs.size(); i-- > 0;) {
		if(cbs.at(i).cb_type == type && cbs.at(i).target == holder)
			cbs.remove_at(i);
	}
	return 0;
}
uint32_t ef_remove_casus_belli_this_pop(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;

	auto holder = nations::owner_of_pop(ws, trigger::to_pop(this_slot));
	auto cbs = ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot));
	for(uint32_t i = cbs.size(); i-- > 0;) {
		if(cbs.at(i).cb_type == type && cbs.at(i).target == holder)
			cbs.remove_at(i);
	}
	return 0;
}
uint32_t ef_remove_casus_belli_from_nation(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;

	auto holder = trigger::to_nation(from_slot);
	auto cbs = ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot));
	for(uint32_t i = cbs.size(); i-- > 0;) {
		if(cbs.at(i).cb_type == type && cbs.at(i).target == holder)
			cbs.remove_at(i);
	}
	return 0;
}
uint32_t ef_remove_casus_belli_from_province(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;

	auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	auto cbs = ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot));
	for(uint32_t i = cbs.size(); i-- > 0;) {
		if(cbs.at(i).cb_type == type && cbs.at(i).target == holder)
			cbs.remove_at(i);
	}
	return 0;
}
uint32_t ef_this_remove_casus_belli_tag(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto tag_target = trigger::payload(tval[2]).tag_id;

	auto holder = ws.world.national_identity_get_nation_from_identity_holder(tag_target);
	if(holder) {
		auto cbs = ws.world.nation_get_available_cbs(holder);
		for(uint32_t i = cbs.size(); i-- > 0;) {
			if(cbs.at(i).cb_type == type && cbs.at(i).target == trigger::to_nation(primary_slot))
				cbs.remove_at(i);
		}
	}
	return 0;
}
uint32_t ef_this_remove_casus_belli_int(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;

	auto holder = ws.world.province_get_nation_from_province_ownership(trigger::payload(tval[2]).prov_id);
	if(holder) {
		auto cbs = ws.world.nation_get_available_cbs(holder);
		for(uint32_t i = cbs.size(); i-- > 0;) {
			if(cbs.at(i).cb_type == type && cbs.at(i).target == trigger::to_nation(primary_slot))
				cbs.remove_at(i);
		}
	}
	return 0;
}
uint32_t ef_this_remove_casus_belli_this_nation(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;

	auto holder = trigger::to_nation(this_slot);
	auto cbs = ws.world.nation_get_available_cbs(holder);
	for(uint32_t i = cbs.size(); i-- > 0;) {
		if(cbs.at(i).cb_type == type && cbs.at(i).target == trigger::to_nation(primary_slot))
			cbs.remove_at(i);
	}
	return 0;
}
uint32_t ef_this_remove_casus_belli_this_state(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;

	auto holder = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot));
	if(holder) {
		auto cbs = ws.world.nation_get_available_cbs(holder);
		for(uint32_t i = cbs.size(); i-- > 0;) {
			if(cbs.at(i).cb_type == type && cbs.at(i).target == trigger::to_nation(primary_slot))
				cbs.remove_at(i);
		}
	}
	return 0;
}
uint32_t ef_this_remove_casus_belli_this_province(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;

	auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(holder) {
		auto cbs = ws.world.nation_get_available_cbs(holder);
		for(uint32_t i = cbs.size(); i-- > 0;) {
			if(cbs.at(i).cb_type == type && cbs.at(i).target == trigger::to_nation(primary_slot))
				cbs.remove_at(i);
		}
	}
	return 0;
}
uint32_t ef_this_remove_casus_belli_this_pop(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;

	auto holder = nations::owner_of_pop(ws, trigger::to_pop(this_slot));
	if(holder) {
		auto cbs = ws.world.nation_get_available_cbs(holder);
		for(uint32_t i = cbs.size(); i-- > 0;) {
			if(cbs.at(i).cb_type == type && cbs.at(i).target == trigger::to_nation(primary_slot))
				cbs.remove_at(i);
		}
	}
	return 0;
}
uint32_t ef_this_remove_casus_belli_from_nation(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;

	auto holder = trigger::to_nation(from_slot);
	auto cbs = ws.world.nation_get_available_cbs(holder);
	for(uint32_t i = cbs.size(); i-- > 0;) {
		if(cbs.at(i).cb_type == type && cbs.at(i).target == trigger::to_nation(primary_slot))
			cbs.remove_at(i);
	}
	return 0;
}
uint32_t ef_this_remove_casus_belli_from_province(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;

	auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	if(holder) {
		auto cbs = ws.world.nation_get_available_cbs(holder);
		for(uint32_t i = cbs.size(); i-- > 0;) {
			if(cbs.at(i).cb_type == type && cbs.at(i).target == trigger::to_nation(primary_slot))
				cbs.remove_at(i);
		}
	}
	return 0;
}

uint32_t ef_add_truce_tag(EFFECT_PARAMTERS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(ws.world.nation_get_owned_province_count(target) == 0 || ws.world.nation_get_owned_province_count(trigger::to_nation(primary_slot)) == 0)
		return 0;
	military::add_truce(ws, target, trigger::to_nation(primary_slot), int32_t(tval[2] * 30.5f));
	return 0;
}
uint32_t ef_add_truce_this_nation(EFFECT_PARAMTERS) {
	auto target = trigger::to_nation(this_slot);
	if(ws.world.nation_get_owned_province_count(target) == 0 || ws.world.nation_get_owned_province_count(trigger::to_nation(primary_slot)) == 0)
		return 0;
	military::add_truce(ws, target, trigger::to_nation(primary_slot), int32_t(tval[1] * 30.5f));
	return 0;
}
uint32_t ef_add_truce_this_state(EFFECT_PARAMTERS) {
	auto target = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot));
	if(ws.world.nation_get_owned_province_count(target) == 0 || ws.world.nation_get_owned_province_count(trigger::to_nation(primary_slot)) == 0)
		return 0;
	military::add_truce(ws, target, trigger::to_nation(primary_slot), int32_t(tval[1] * 30.5f));
	return 0;
}
uint32_t ef_add_truce_this_province(EFFECT_PARAMTERS) {
	auto target = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(ws.world.nation_get_owned_province_count(target) == 0 || ws.world.nation_get_owned_province_count(trigger::to_nation(primary_slot)) == 0)
		return 0;
	military::add_truce(ws, target, trigger::to_nation(primary_slot), int32_t(tval[1] * 30.5f));
	return 0;
}
uint32_t ef_add_truce_this_pop(EFFECT_PARAMTERS) {
	auto target = nations::owner_of_pop(ws, trigger::to_pop(this_slot));
	if(ws.world.nation_get_owned_province_count(target) == 0 || ws.world.nation_get_owned_province_count(trigger::to_nation(primary_slot)) == 0)
		return 0;
	military::add_truce(ws, target, trigger::to_nation(primary_slot), int32_t(tval[1] * 30.5f));
	return 0;
}
uint32_t ef_add_truce_from_nation(EFFECT_PARAMTERS) {
	auto target = trigger::to_nation(from_slot);
	if(ws.world.nation_get_owned_province_count(target) == 0 || ws.world.nation_get_owned_province_count(trigger::to_nation(primary_slot)) == 0)
		return 0;
	military::add_truce(ws, target, trigger::to_nation(primary_slot), int32_t(tval[1] * 30.5f));
	return 0;
}
uint32_t ef_add_truce_from_province(EFFECT_PARAMTERS) {
	auto target = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(ws.world.nation_get_owned_province_count(target) == 0 || ws.world.nation_get_owned_province_count(trigger::to_nation(primary_slot)) == 0)
		return 0;
	military::add_truce(ws, target, trigger::to_nation(primary_slot), int32_t(tval[1] * 30.5f));
	return 0;
}
uint32_t ef_call_allies(EFFECT_PARAMTERS) {
	for(auto drel : ws.world.nation_get_diplomatic_relation(trigger::to_nation(primary_slot))) {
		auto other_nation = drel.get_related_nations(0) != trigger::to_nation(primary_slot) ? drel.get_related_nations(0) : drel.get_related_nations(1);
		if(drel.get_are_allied()) {
			for(auto wfor : ws.world.nation_get_war_participant(trigger::to_nation(primary_slot))) {
				if(wfor.get_war().get_primary_attacker() == trigger::to_nation(primary_slot)) {
					if(!military::has_truce_with(ws, other_nation, wfor.get_war().get_primary_defender()) && military::standard_war_joining_is_possible(ws, wfor.get_war(), other_nation, true)) {

						diplomatic_message::message m;
						std::memset(&m, 0, sizeof(m));
						m.from = trigger::to_nation(primary_slot);
						m.to = other_nation;
						m.type = diplomatic_message::type_t::call_ally_request;
						m.data.war = wfor.get_war();
						diplomatic_message::post(ws, m);
					}
				}
				if(wfor.get_war().get_primary_defender() == trigger::to_nation(primary_slot)) {
					if(!military::has_truce_with(ws, other_nation, wfor.get_war().get_primary_attacker()) && military::standard_war_joining_is_possible(ws, wfor.get_war(), other_nation, false)) {

						diplomatic_message::message m;
						std::memset(&m, 0, sizeof(m));
						m.from = trigger::to_nation(primary_slot);
						m.to = other_nation;
						m.type = diplomatic_message::type_t::call_ally_request;
						m.data.war = wfor.get_war();
						diplomatic_message::post(ws, m);
					}
				}
			}
		}

	}

	return 0;
}

uint32_t ef_ruling_party_this(EFFECT_PARAMTERS) {
	politics::force_ruling_party_ideology(ws, trigger::to_nation(primary_slot), ws.world.nation_get_ruling_party(trigger::to_nation(this_slot)).get_ideology());
	return 0;
}
uint32_t ef_ruling_party_from(EFFECT_PARAMTERS) {
	politics::force_ruling_party_ideology(ws, trigger::to_nation(primary_slot), ws.world.nation_get_ruling_party(trigger::to_nation(from_slot)).get_ideology());
	return 0;
}

uint32_t ef_war_tag(EFFECT_PARAMTERS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(!target)
		return 0;
	if(ws.world.nation_get_owned_province_count(target) == 0 || ws.world.nation_get_owned_province_count(trigger::to_nation(primary_slot)) == 0)
		return 0;
	if(target == trigger::to_nation(primary_slot))
		return 0;
	auto war = military::find_war_between(ws, trigger::to_nation(primary_slot), target);
	if(!war) {
		military::remove_from_common_allied_wars(ws, trigger::to_nation(primary_slot), target);
		war = military::create_war(ws, trigger::to_nation(primary_slot), target, trigger::payload(tval[5]).cb_id,
			ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[6]).prov_id),
			trigger::payload(tval[7]).tag_id,
			ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[7]).tag_id));
	} else {
		if(trigger::payload(tval[5]).cb_id) { //attacker
			military::add_wargoal(ws, war, trigger::to_nation(primary_slot), target, trigger::payload(tval[5]).cb_id,
				ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[6]).prov_id),
				trigger::payload(tval[7]).tag_id,
				ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[7]).tag_id));
		}
	}
	if(trigger::payload(tval[2]).cb_id) { //defender
		military::add_wargoal(ws, war, target, trigger::to_nation(primary_slot), trigger::payload(tval[2]).cb_id,
			ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[3]).prov_id),
			trigger::payload(tval[4]).tag_id,
			ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[4]).tag_id));
	}
	military::call_defender_allies(ws, war);
	military::call_attacker_allies(ws, war);
	return 0;
}
uint32_t ef_war_this_nation(EFFECT_PARAMTERS) {
	auto target = trigger::to_nation(this_slot);
	if(ws.world.nation_get_owned_province_count(target) == 0 || ws.world.nation_get_owned_province_count(trigger::to_nation(primary_slot)) == 0)
		return 0;
	if(target == trigger::to_nation(primary_slot))
		return 0;
	auto war = military::find_war_between(ws, trigger::to_nation(primary_slot), target);
	if(!war) {
		military::remove_from_common_allied_wars(ws, trigger::to_nation(primary_slot), target);
		war = military::create_war(ws, trigger::to_nation(primary_slot), target, trigger::payload(tval[4]).cb_id,
			ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[5]).prov_id),
			trigger::payload(tval[6]).tag_id,
			ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[6]).tag_id));
	} else {
		if(trigger::payload(tval[4]).cb_id) { //attacker
			military::add_wargoal(ws, war, trigger::to_nation(primary_slot), target, trigger::payload(tval[4]).cb_id,
				ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[5]).prov_id),
				trigger::payload(tval[6]).tag_id,
				ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[6]).tag_id));
		}
	}
	if(trigger::payload(tval[1]).cb_id) { //defender
		military::add_wargoal(ws, war, target, trigger::to_nation(primary_slot), trigger::payload(tval[1]).cb_id,
			ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[2]).prov_id),
			trigger::payload(tval[3]).tag_id,
			ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[3]).tag_id));
	}
	military::call_defender_allies(ws, war);
	military::call_attacker_allies(ws, war);
	return 0;
}
uint32_t ef_war_this_state(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)); owner)
		return ef_war_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_lo, r_hi, els);
	return 0;
}
uint32_t ef_war_this_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)); owner)
		return ef_war_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_lo, r_hi, els);
	return 0;
}
uint32_t ef_war_this_pop(EFFECT_PARAMTERS) {
	if(auto owner = nations::owner_of_pop(ws, trigger::to_pop(this_slot)); owner)
		return ef_war_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_lo, r_hi, els);
	return 0;
}
uint32_t ef_war_from_nation(EFFECT_PARAMTERS) {
	return ef_war_this_nation(tval, ws, primary_slot, from_slot, 0, r_lo, r_hi, els);
}
uint32_t ef_war_from_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)); owner)
		return ef_war_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_lo, r_hi, els);
	return 0;
}
uint32_t ef_war_no_ally_tag(EFFECT_PARAMTERS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(!target)
		return 0;
	if(ws.world.nation_get_owned_province_count(target) == 0 || ws.world.nation_get_owned_province_count(trigger::to_nation(primary_slot)) == 0)
		return 0;
	if(target == trigger::to_nation(primary_slot))
		return 0;
	auto war = military::find_war_between(ws, trigger::to_nation(primary_slot), target);
	if(!war) {
		military::remove_from_common_allied_wars(ws, trigger::to_nation(primary_slot), target);
		war = military::create_war(ws, trigger::to_nation(primary_slot), target, trigger::payload(tval[5]).cb_id,
			ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[6]).prov_id),
			trigger::payload(tval[7]).tag_id,
			ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[7]).tag_id));
	} else {
		if(trigger::payload(tval[5]).cb_id) { //attacker
			military::add_wargoal(ws, war, trigger::to_nation(primary_slot), target, trigger::payload(tval[5]).cb_id,
				ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[6]).prov_id),
				trigger::payload(tval[7]).tag_id,
				ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[7]).tag_id));
		}
	}
	if(trigger::payload(tval[2]).cb_id) { //defender
		military::add_wargoal(ws, war, target, trigger::to_nation(primary_slot), trigger::payload(tval[2]).cb_id,
			ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[3]).prov_id),
			trigger::payload(tval[4]).tag_id,
			ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[4]).tag_id));
	}
	military::call_defender_allies(ws, war);
	return 0;
}
uint32_t ef_war_no_ally_this_nation(EFFECT_PARAMTERS) {
	auto target = trigger::to_nation(this_slot);
	if(ws.world.nation_get_owned_province_count(target) == 0 || ws.world.nation_get_owned_province_count(trigger::to_nation(primary_slot)) == 0)
		return 0;
	if(target == trigger::to_nation(primary_slot))
		return 0;
	auto war = military::find_war_between(ws, trigger::to_nation(primary_slot), target);
	if(!war) {
		military::remove_from_common_allied_wars(ws, trigger::to_nation(primary_slot), target);
		war = military::create_war(ws, trigger::to_nation(primary_slot), target, trigger::payload(tval[4]).cb_id,
		   ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[5]).prov_id),
		   trigger::payload(tval[6]).tag_id,
		   ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[6]).tag_id));
	} else {
		if(trigger::payload(tval[4]).cb_id) { //attacker
			military::add_wargoal(ws, war, trigger::to_nation(primary_slot), target, trigger::payload(tval[4]).cb_id,
				ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[5]).prov_id),
				trigger::payload(tval[6]).tag_id,
				ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[6]).tag_id));
		}
	}
	if(trigger::payload(tval[1]).cb_id) { //defender
		military::add_wargoal(ws, war, target, trigger::to_nation(primary_slot), trigger::payload(tval[1]).cb_id,
			ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[2]).prov_id),
			trigger::payload(tval[3]).tag_id,
			ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[3]).tag_id));
	}
	military::call_defender_allies(ws, war);
	return 0;
}
uint32_t ef_war_no_ally_this_state(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)); owner)
		ef_war_no_ally_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_lo, r_hi, els);
	return 0;
}
uint32_t ef_war_no_ally_this_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)); owner)
		ef_war_no_ally_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_lo, r_hi, els);
	return 0;
}
uint32_t ef_war_no_ally_this_pop(EFFECT_PARAMTERS) {
	if(auto owner = nations::owner_of_pop(ws, trigger::to_pop(this_slot)); owner)
		ef_war_no_ally_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_lo, r_hi, els);
	return 0;
}
uint32_t ef_war_no_ally_from_nation(EFFECT_PARAMTERS) {
	ef_war_no_ally_this_nation(tval, ws, primary_slot, from_slot, 0, r_lo, r_hi, els);
	return 0;
}
uint32_t ef_war_no_ally_from_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)); owner)
		ef_war_no_ally_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_lo, r_hi, els);
	return 0;
}
uint32_t ef_country_event_this_nation(EFFECT_PARAMTERS) {
	auto postpone = int32_t(tval[2]);
	assert(postpone > 0);
	auto future_date = ws.current_date + postpone;
	auto name = text::produce_simple_string(ws, dcon::fatten(ws.world, trigger::payload(tval[1]).nev_id).get_name());
	auto nationtag = text::produce_simple_string(ws, dcon::fatten(ws.world, trigger::to_nation(primary_slot)).get_identity_from_identity_holder().get_name());
	if(!event::would_be_duplicate_instance(ws, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), future_date))
		ws.future_n_event.push_back(event::pending_human_n_event {r_lo + 1, r_hi, primary_slot, this_slot, future_date, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), event::slot_type::nation, event::slot_type::nation});
	return 0;
}
uint32_t ef_country_event_immediate_this_nation(EFFECT_PARAMTERS) {
	if(!event::would_be_duplicate_instance(ws, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date))
		ws.future_n_event.push_back(event::pending_human_n_event {r_lo + 1, r_hi, primary_slot, this_slot, ws.current_date, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), event::slot_type::nation, event::slot_type::nation});
	return 0;
}
uint32_t ef_province_event_this_nation(EFFECT_PARAMTERS) {
	auto postpone = int32_t(tval[2]);
	assert(postpone > 0);
	auto future_date = ws.current_date + postpone;
	ws.future_p_event.push_back(event::pending_human_p_event {r_lo + 1, r_hi, this_slot, future_date, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), event::slot_type::nation});
	return 0;
}
uint32_t ef_province_event_immediate_this_nation(EFFECT_PARAMTERS) {
	ws.future_p_event.push_back(event::pending_human_p_event {r_lo + 1, r_hi, this_slot, ws.current_date, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), event::slot_type::nation});
	return 0;
}
uint32_t ef_country_event_this_state(EFFECT_PARAMTERS) {
	auto postpone = int32_t(tval[2]);
	assert(postpone > 0);
	auto future_date = ws.current_date + postpone;
	if(!event::would_be_duplicate_instance(ws, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), future_date))
		ws.future_n_event.push_back(event::pending_human_n_event {r_lo + 1, r_hi, primary_slot, this_slot, future_date, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), event::slot_type::nation, event::slot_type::state});
	return 0;
}
uint32_t ef_country_event_immediate_this_state(EFFECT_PARAMTERS) {
	if(!event::would_be_duplicate_instance(ws, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date))
		ws.future_n_event.push_back(event::pending_human_n_event {r_lo + 1, r_hi, primary_slot, this_slot, ws.current_date, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), event::slot_type::nation, event::slot_type::state});
	return 0;
}
uint32_t ef_province_event_this_state(EFFECT_PARAMTERS) {
	auto postpone = int32_t(tval[2]);
	assert(postpone > 0);
	auto future_date = ws.current_date + postpone;
	ws.future_p_event.push_back(event::pending_human_p_event {r_lo + 1, r_hi, this_slot, future_date, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), event::slot_type::state});
	return 0;
}
uint32_t ef_province_event_immediate_this_state(EFFECT_PARAMTERS) {
	ws.future_p_event.push_back(event::pending_human_p_event {r_lo + 1, r_hi, this_slot, ws.current_date, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), event::slot_type::state});
	return 0;
}
uint32_t ef_country_event_this_province(EFFECT_PARAMTERS) {
	auto postpone = int32_t(tval[2]);
	assert(postpone > 0);
	auto future_date = ws.current_date + postpone;
	if(!event::would_be_duplicate_instance(ws, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), future_date))
		ws.future_n_event.push_back(event::pending_human_n_event {r_lo + 1, r_hi, primary_slot, this_slot, future_date, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), event::slot_type::nation, event::slot_type::province});
	return 0;
}
uint32_t ef_country_event_immediate_this_province(EFFECT_PARAMTERS) {
	if(!event::would_be_duplicate_instance(ws, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date))
		ws.future_n_event.push_back(event::pending_human_n_event {r_lo + 1, r_hi, primary_slot, this_slot, ws.current_date, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), event::slot_type::nation, event::slot_type::province});
	return 0;
}
uint32_t ef_province_event_this_province(EFFECT_PARAMTERS) {
	auto postpone = int32_t(tval[2]);
	assert(postpone > 0);
	auto future_date = ws.current_date + postpone;
	ws.future_p_event.push_back(event::pending_human_p_event {r_lo + 1, r_hi, this_slot, future_date, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), event::slot_type::province});
	return 0;
}
uint32_t ef_province_event_immediate_this_province(EFFECT_PARAMTERS) {
	ws.future_p_event.push_back(event::pending_human_p_event {r_lo + 1, r_hi, this_slot, ws.current_date, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), event::slot_type::province});
	return 0;
}
uint32_t ef_country_event_this_pop(EFFECT_PARAMTERS) {
	auto postpone = int32_t(tval[2]);
	assert(postpone > 0);
	auto future_date = ws.current_date + postpone;
	if(!event::would_be_duplicate_instance(ws, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), future_date))
		ws.future_n_event.push_back(event::pending_human_n_event {r_lo + 1, r_hi, primary_slot, this_slot, future_date, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), event::slot_type::nation, event::slot_type::pop});
	return 0;
}
uint32_t ef_country_event_immediate_this_pop(EFFECT_PARAMTERS) {
	if(!event::would_be_duplicate_instance(ws, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), ws.current_date))
		ws.future_n_event.push_back(event::pending_human_n_event {r_lo + 1, r_hi, primary_slot, this_slot, ws.current_date, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), event::slot_type::nation, event::slot_type::pop});
	return 0;
}
uint32_t ef_province_event_this_pop(EFFECT_PARAMTERS) {
	auto postpone = int32_t(tval[2]);
	assert(postpone > 0);
	auto future_date = ws.current_date + postpone;
	ws.future_p_event.push_back(event::pending_human_p_event {r_lo + 1, r_hi, this_slot, future_date, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), event::slot_type::pop});
	return 0;
}
uint32_t ef_province_event_immediate_this_pop(EFFECT_PARAMTERS) {
	ws.future_p_event.push_back(event::pending_human_p_event {r_lo + 1, r_hi, this_slot, ws.current_date, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), event::slot_type::pop});
	return 0;
}
uint32_t ef_country_event_province_this_nation(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_country_event_this_nation(tval, ws, trigger::to_generic(owner), this_slot, 0, r_lo, r_hi, els);
	return 0;
}
uint32_t ef_country_event_immediate_province_this_nation(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_country_event_immediate_this_nation(tval, ws, trigger::to_generic(owner), this_slot, 0, r_lo, r_hi, els);
	return 0;
}
uint32_t ef_country_event_province_this_state(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_country_event_this_state(tval, ws, trigger::to_generic(owner), this_slot, 0, r_lo, r_hi, els);
	return 0;
}
uint32_t ef_country_event_immediate_province_this_state(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_country_event_immediate_this_state(tval, ws, trigger::to_generic(owner), this_slot, 0, r_lo, r_hi, els);
	return 0;
}
uint32_t ef_country_event_province_this_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_country_event_this_province(tval, ws, trigger::to_generic(owner), this_slot, 0, r_lo, r_hi, els);
	return 0;
}
uint32_t ef_country_event_immediate_province_this_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_country_event_immediate_this_province(tval, ws, trigger::to_generic(owner), this_slot, 0, r_lo, r_hi, els);
	return 0;
}
uint32_t ef_country_event_province_this_pop(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_country_event_this_pop(tval, ws, trigger::to_generic(owner), this_slot, 0, r_lo, r_hi, els);
	return 0;
}
uint32_t ef_country_event_immediate_province_this_pop(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_country_event_immediate_this_pop(tval, ws, trigger::to_generic(owner), this_slot, 0, r_lo, r_hi, els);
	return 0;
}

uint32_t ef_sub_unit_int(EFFECT_PARAMTERS) {
	// do nothing
	return 0;
}
uint32_t ef_sub_unit_this(EFFECT_PARAMTERS) {
	// do nothing
	return 0;
}
uint32_t ef_sub_unit_from(EFFECT_PARAMTERS) {
	// do nothing
	return 0;
}
uint32_t ef_sub_unit_current(EFFECT_PARAMTERS) {
	// do nothing
	return 0;
}
uint32_t ef_set_variable(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(amount));

	ws.world.nation_get_variables(trigger::to_nation(primary_slot), trigger::payload(tval[1]).natv_id) = amount;
	return 0;
}
uint32_t ef_change_variable(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(amount));

	ws.world.nation_get_variables(trigger::to_nation(primary_slot), trigger::payload(tval[1]).natv_id) += amount;
	return 0;
}
uint32_t ef_ideology(EFFECT_PARAMTERS) {
	auto i = trigger::payload(tval[1]).ideo_id;
	auto factor = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(factor));

	auto s = pop_demographics::get_demo(ws, trigger::to_pop(primary_slot), pop_demographics::to_key(ws, i));
	float new_total = 1.0f  - s + std::max(0.0f, s + factor);
	pop_demographics::set_demo(ws, trigger::to_pop(primary_slot), pop_demographics::to_key(ws, i), std::max(0.0f, s + factor));

	for(auto j : ws.world.in_ideology) {
		pop_demographics::set_demo(ws, trigger::to_pop(primary_slot), pop_demographics::to_key(ws, j),
			pop_demographics::get_demo(ws, trigger::to_pop(primary_slot), pop_demographics::to_key(ws, j)) / new_total);
	}

	return 0;
}
uint32_t ef_upper_house(EFFECT_PARAMTERS) {
	auto i = trigger::payload(tval[1]).ideo_id;
	auto amount = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(amount));

	auto& u = ws.world.nation_get_upper_house(trigger::to_nation(primary_slot), i);
	float new_total = 100.0f - u + std::max(0.0f, u + 100.0f * amount);
	u = std::max(0.0f, u + 100.0f * amount);
	

	for(auto j : ws.world.in_ideology) {
		//auto prior_value = ws.world.nation_get_upper_house(trigger::to_nation(primary_slot), j);
		//auto new_value = prior_value * 100.0f / (new_total);
		//ws.world.nation_set_upper_house(trigger::to_nation(primary_slot), j, prior_value);
		ws.world.nation_get_upper_house(trigger::to_nation(primary_slot), j) *= 100.0f / (new_total);
	}

	return 0;
}
uint32_t ef_scaled_militancy_issue(EFFECT_PARAMTERS) {
	auto issue_demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);

	auto support = pop_demographics::get_demo(ws, trigger::to_pop(primary_slot), issue_demo_tag);
	float adjustment = trigger::read_float_from_payload(tval + 2) * float(support);
	assert(std::isfinite(adjustment));
	auto v = pop_demographics::get_militancy(ws, trigger::to_pop(primary_slot));
	pop_demographics::set_militancy(ws, trigger::to_pop(primary_slot), std::clamp(v + adjustment, 0.0f, 10.0f));

	return 0;
}
uint32_t ef_scaled_militancy_ideology(EFFECT_PARAMTERS) {
	auto ideology_demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).ideo_id);

	auto support = pop_demographics::get_demo(ws, trigger::to_pop(primary_slot), ideology_demo_tag);
	float adjustment = trigger::read_float_from_payload(tval + 2) * float(support);
	assert(std::isfinite(adjustment));
	auto v = pop_demographics::get_militancy(ws, trigger::to_pop(primary_slot));
	pop_demographics::set_militancy(ws, trigger::to_pop(primary_slot), std::clamp(v + adjustment, 0.0f, 10.0f));

	return 0;
}
uint32_t ef_scaled_militancy_unemployment(EFFECT_PARAMTERS) {
	auto unemployed = 1.0f - pop_demographics::get_raw_employment(ws, trigger::to_pop(primary_slot));
	float adjustment = trigger::read_float_from_payload(tval + 1) * float(unemployed);
	assert(std::isfinite(adjustment));
	auto v = pop_demographics::get_militancy(ws, trigger::to_pop(primary_slot));
	pop_demographics::set_militancy(ws, trigger::to_pop(primary_slot), std::clamp(v + adjustment, 0.0f, 10.0f));
	
	return 0;
}
uint32_t ef_scaled_consciousness_issue(EFFECT_PARAMTERS) {
	auto issue_demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);

	auto support = pop_demographics::get_demo(ws, trigger::to_pop(primary_slot), issue_demo_tag);
	float adjustment = trigger::read_float_from_payload(tval + 2) * float(support);
	assert(std::isfinite(adjustment));
	auto v = pop_demographics::get_consciousness(ws, trigger::to_pop(primary_slot));
	pop_demographics::set_consciousness(ws, trigger::to_pop(primary_slot), std::clamp(v + adjustment, 0.0f, 10.0f));

	return 0;
}
uint32_t ef_scaled_consciousness_ideology(EFFECT_PARAMTERS) {
	auto ideology_demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).ideo_id);

	auto support = pop_demographics::get_demo(ws, trigger::to_pop(primary_slot), ideology_demo_tag);
	float adjustment = trigger::read_float_from_payload(tval + 2) * float(support);
	assert(std::isfinite(adjustment));
	auto v = pop_demographics::get_consciousness(ws, trigger::to_pop(primary_slot));
	pop_demographics::set_consciousness(ws, trigger::to_pop(primary_slot), std::clamp(v + adjustment, 0.0f, 10.0f));

	return 0;
}
uint32_t ef_scaled_consciousness_unemployment(EFFECT_PARAMTERS) {
	auto unemployed = 1.0 - pop_demographics::get_employment(ws, trigger::to_pop(primary_slot));
	float adjustment = trigger::read_float_from_payload(tval + 1) * float(unemployed);
	auto v = pop_demographics::get_consciousness(ws, trigger::to_pop(primary_slot));
	pop_demographics::set_consciousness(ws, trigger::to_pop(primary_slot), std::clamp(v + adjustment, 0.0f, 10.0f));
	
	return 0;
}
uint32_t ef_scaled_militancy_nation_issue(EFFECT_PARAMTERS) {
	auto demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);
	auto factor = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(factor));

	for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
		for(auto pop : p.get_province().get_pop_location()) {
			auto support = pop_demographics::get_demo(ws, pop.get_pop(), demo_tag);
			float adjustment = factor * support;
			auto v = pop_demographics::get_militancy(ws, pop.get_pop());
			pop_demographics::set_militancy(ws, pop.get_pop(), std::clamp(v + adjustment, 0.0f, 10.0f));
		}
	}

	return 0;
}
uint32_t ef_scaled_militancy_nation_ideology(EFFECT_PARAMTERS) {
	auto demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).ideo_id);
	auto factor = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(factor));

	for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
		for(auto pop : p.get_province().get_pop_location()) {
			auto support = pop_demographics::get_demo(ws, pop.get_pop(), demo_tag);
			float adjustment = factor * support;
			auto v = pop_demographics::get_militancy(ws, pop.get_pop());
			pop_demographics::set_militancy(ws, pop.get_pop(), std::clamp(v + adjustment, 0.0f, 10.0f));
		}
	}

	return 0;
}
uint32_t ef_scaled_militancy_nation_unemployment(EFFECT_PARAMTERS) {
	auto factor = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(factor));

	for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
		for(auto pop : p.get_province().get_pop_location()) {
			auto unemployed = 1.0f - pop_demographics::get_raw_employment(ws, pop.get_pop());
			float adjustment = factor * unemployed;
			auto v = pop_demographics::get_militancy(ws, pop.get_pop());
			pop_demographics::set_militancy(ws, pop.get_pop(), std::clamp(v + adjustment, 0.0f, 10.0f));
		}
	}

	return 0;
}
uint32_t ef_scaled_consciousness_nation_issue(EFFECT_PARAMTERS) {
	auto demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);
	auto factor = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(factor));

	for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
		for(auto pop : p.get_province().get_pop_location()) {
			auto support = pop_demographics::get_demo(ws, pop.get_pop(), demo_tag);
			float adjustment = factor * support;
			auto v = pop_demographics::get_consciousness(ws, pop.get_pop());
			pop_demographics::set_consciousness(ws, pop.get_pop(), std::clamp(v + adjustment, 0.0f, 10.0f));
		}
	}

	return 0;
}
uint32_t ef_scaled_consciousness_nation_ideology(EFFECT_PARAMTERS) {
	auto demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).ideo_id);
	auto factor = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(factor));

	for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
		for(auto pop : p.get_province().get_pop_location()) {
			auto support = pop_demographics::get_demo(ws, pop.get_pop(), demo_tag);
			float adjustment = factor * support;
			auto v = pop_demographics::get_consciousness(ws, pop.get_pop());
			pop_demographics::set_consciousness(ws, pop.get_pop(), std::clamp(v + adjustment, 0.0f, 10.0f));
		}
	}

	return 0;
}
uint32_t ef_scaled_consciousness_nation_unemployment(EFFECT_PARAMTERS) {
	auto factor = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(factor));

	for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
		for(auto pop : p.get_province().get_pop_location()) {
			auto unemployed = 1.0f - pop_demographics::get_raw_employment(ws, pop.get_pop());
			float adjustment = factor * unemployed;
			auto v = pop_demographics::get_consciousness(ws, pop.get_pop());
			pop_demographics::set_consciousness(ws, pop.get_pop(), std::clamp(v + adjustment, 0.0f, 10.0f));
		}
	}

	return 0;
}
uint32_t ef_scaled_militancy_state_issue(EFFECT_PARAMTERS) {
	auto demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);
	auto factor = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(factor));

	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
		for(auto pop : ws.world.province_get_pop_location(p)) {
			auto support = pop_demographics::get_demo(ws, pop.get_pop(), demo_tag);
			float adjustment = factor * support;
			auto v = pop_demographics::get_militancy(ws, pop.get_pop());
			pop_demographics::set_militancy(ws, pop.get_pop(), std::clamp(v + adjustment, 0.0f, 10.0f));
		}
	});

	return 0;
}
uint32_t ef_scaled_militancy_state_ideology(EFFECT_PARAMTERS) {
	auto demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).ideo_id);
	auto factor = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(factor));

	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
		for(auto pop : ws.world.province_get_pop_location(p)) {
			auto support = pop_demographics::get_demo(ws, pop.get_pop(), demo_tag);
			float adjustment = factor * support;
			auto v = pop_demographics::get_militancy(ws, pop.get_pop());
			pop_demographics::set_militancy(ws, pop.get_pop(), std::clamp(v + adjustment, 0.0f, 10.0f));
		}
	});

	return 0;
}
uint32_t ef_scaled_militancy_state_unemployment(EFFECT_PARAMTERS) {
	auto factor = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(factor));

	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
		for(auto pop : ws.world.province_get_pop_location(p)) {
			auto unemployed = 1.0f - pop_demographics::get_raw_employment(ws, pop.get_pop());
			float adjustment = factor * unemployed;
			auto v = pop_demographics::get_militancy(ws, pop.get_pop());
			pop_demographics::set_militancy(ws, pop.get_pop(), std::clamp(v + adjustment, 0.0f, 10.0f));
		}
	});

	return 0;
}
uint32_t ef_scaled_consciousness_state_issue(EFFECT_PARAMTERS) {
	auto demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);
	auto factor = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(factor));

	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
		for(auto pop : ws.world.province_get_pop_location(p)) {
			auto support = pop_demographics::get_demo(ws, pop.get_pop(), demo_tag);
			float adjustment = factor * support;
			auto v = pop_demographics::get_consciousness(ws, pop.get_pop());
			pop_demographics::set_consciousness(ws, pop.get_pop(), std::clamp(v + adjustment, 0.0f, 10.0f));
		}
	});

	return 0;
}
uint32_t ef_scaled_consciousness_state_ideology(EFFECT_PARAMTERS) {
	auto demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).ideo_id);
	auto factor = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(factor));

	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
		for(auto pop : ws.world.province_get_pop_location(p)) {
			auto support = pop_demographics::get_demo(ws, pop.get_pop(), demo_tag);
			float adjustment = factor * support;
			auto v = pop_demographics::get_consciousness(ws, pop.get_pop());
			pop_demographics::set_consciousness(ws, pop.get_pop(), std::clamp(v + adjustment, 0.0f, 10.0f));
		}
	});

	return 0;
}
uint32_t ef_scaled_consciousness_state_unemployment(EFFECT_PARAMTERS) {
	auto factor = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(factor));

	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
		for(auto pop : ws.world.province_get_pop_location(p)) {
			auto unemployed = 1.0f - pop_demographics::get_raw_employment(ws, pop.get_pop());
			float adjustment = factor * unemployed;
			auto v = pop_demographics::get_consciousness(ws, pop.get_pop());
			pop_demographics::set_consciousness(ws, pop.get_pop(), std::clamp(v + adjustment, 0.0f, 10.0f));
		}
	});

	return 0;
}
uint32_t ef_scaled_militancy_province_issue(EFFECT_PARAMTERS) {
	auto demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);
	auto factor = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(factor));

	for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
		auto support = pop_demographics::get_demo(ws, pop.get_pop(), demo_tag);
		float adjustment = factor * support;
		auto v = pop_demographics::get_militancy(ws, pop.get_pop());
		pop_demographics::set_militancy(ws, pop.get_pop(), std::clamp(v + adjustment, 0.0f, 10.0f));
	}

	return 0;
}
uint32_t ef_scaled_militancy_province_ideology(EFFECT_PARAMTERS) {
	auto demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).ideo_id);
	auto factor = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(factor));

	for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
		auto support = pop_demographics::get_demo(ws, pop.get_pop(), demo_tag);
		float adjustment = factor * support;
		auto v = pop_demographics::get_militancy(ws, pop.get_pop());
		pop_demographics::set_militancy(ws, pop.get_pop(), std::clamp(v + adjustment, 0.0f, 10.0f));
	}

	return 0;
}
uint32_t ef_scaled_militancy_province_unemployment(EFFECT_PARAMTERS) {
	auto factor = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(factor));

	for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
		auto unemployed = 1.0f - pop_demographics::get_raw_employment(ws, pop.get_pop());
		float adjustment = factor * unemployed;
		auto v = pop_demographics::get_militancy(ws, pop.get_pop());
		pop_demographics::set_militancy(ws, pop.get_pop(), std::clamp(v + adjustment, 0.0f, 10.0f));
	}

	return 0;
}
uint32_t ef_scaled_consciousness_province_issue(EFFECT_PARAMTERS) {
	auto demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);
	auto factor = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(factor));

	for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
		auto support = pop_demographics::get_demo(ws, pop.get_pop(), demo_tag);
		float adjustment = factor * support;
		auto v = pop_demographics::get_consciousness(ws, pop.get_pop());
		pop_demographics::set_consciousness(ws, pop.get_pop(), std::clamp(v + adjustment, 0.0f, 10.0f));
	}

	return 0;
}
uint32_t ef_scaled_consciousness_province_ideology(EFFECT_PARAMTERS) {
	auto demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).ideo_id);
	auto factor = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(factor));

	for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
		auto support = pop_demographics::get_demo(ws, pop.get_pop(), demo_tag);
		float adjustment = factor * support;
		auto v = pop_demographics::get_consciousness(ws, pop.get_pop());
		pop_demographics::set_consciousness(ws, pop.get_pop(), std::clamp(v + adjustment, 0.0f, 10.0f));
	}

	return 0;
}
uint32_t ef_scaled_consciousness_province_unemployment(EFFECT_PARAMTERS) {
	auto factor = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(factor));

	for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
		auto unemployed = 1.0f - pop_demographics::get_raw_employment(ws, pop.get_pop());
		float adjustment = factor * unemployed;
		auto v = pop_demographics::get_consciousness(ws, pop.get_pop());
		pop_demographics::set_consciousness(ws, pop.get_pop(), std::clamp(v + adjustment, 0.0f, 10.0f));
	}

	return 0;
}
uint32_t ef_variable_good_name(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(amount));
	auto& v = ws.world.nation_get_stockpiles(trigger::to_nation(primary_slot), trigger::payload(tval[1]).com_id);
	v = std::max(v + amount, 0.0f);
	return 0;
}
uint32_t ef_variable_good_name_province(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(amount));
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner) {
		auto& v = ws.world.nation_get_stockpiles(owner, trigger::payload(tval[1]).com_id);
		v = std::max(v + amount, 0.0f);
	}
	return 0;
}
uint32_t ef_kill_leader(EFFECT_PARAMTERS) {
	dcon::unit_name_id ename{ dcon::unit_name_id::value_base_t(trigger::read_int32_t_from_payload(tval + 1)) };
	auto esv = ws.to_string_view(ename);
	for(auto l : ws.world.nation_get_leader_loyalty(trigger::to_nation(primary_slot))) {
		auto n = l.get_leader().get_name();
		auto nsv = ws.to_string_view(n);
		if(nsv == esv) {
			military::kill_leader(ws, l.get_leader());
			return 0;
		}
	}
	return 0;
}
uint32_t ef_define_general(EFFECT_PARAMTERS) {
	auto l = fatten(ws.world, ws.world.create_leader());
	l.set_since(ws.current_date);
	l.set_is_admiral(false);
	l.set_background(trigger::payload(tval[4]).lead_id);
	l.set_personality(trigger::payload(tval[3]).lead_id);
	l.set_name(dcon::unit_name_id(dcon::unit_name_id::value_base_t(trigger::read_int32_t_from_payload(tval + 1))));
	l.set_nation_from_leader_loyalty(trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_define_admiral(EFFECT_PARAMTERS) {
	auto l = fatten(ws.world, ws.world.create_leader());
	l.set_since(ws.current_date);
	l.set_is_admiral(true);
	l.set_background(trigger::payload(tval[4]).lead_id);
	l.set_personality(trigger::payload(tval[3]).lead_id);
	l.set_name(dcon::unit_name_id(dcon::unit_name_id::value_base_t(trigger::read_int32_t_from_payload(tval + 1))));
	l.set_nation_from_leader_loyalty(trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_dominant_issue(EFFECT_PARAMTERS) {
	auto t = trigger::payload(tval[1]).opt_id;
	auto factor = trigger::read_float_from_payload(tval + 2);

	auto s = pop_demographics::get_demo(ws, trigger::to_pop(primary_slot), pop_demographics::to_key(ws, t));
	auto new_total = 1.0f - s + std::max(0.0f, s + factor);
	pop_demographics::set_demo(ws, trigger::to_pop(primary_slot), pop_demographics::to_key(ws, t), std::max(0.0f, s + factor));

	for(auto i : ws.world.in_issue_option) {
		pop_demographics::set_demo(ws, trigger::to_pop(primary_slot), pop_demographics::to_key(ws, i),
			pop_demographics::get_demo(ws, trigger::to_pop(primary_slot), pop_demographics::to_key(ws, i)) / new_total);
	}

	return 0;
}
uint32_t ef_dominant_issue_nation(EFFECT_PARAMTERS) {
	auto demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);
	auto factor = trigger::read_float_from_payload(tval + 2);

	for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
		for(auto pop : p.get_province().get_pop_location()) {
			auto s = pop_demographics::get_demo(ws, pop.get_pop(), demo_tag);
			auto new_total = 1.0f - s + std::max(0.0f, s + factor);
			pop_demographics::set_demo(ws, pop.get_pop(), demo_tag, std::max(0.0f, s + factor));

			for(auto i : ws.world.in_issue_option) {
				pop_demographics::set_demo(ws, pop.get_pop(), pop_demographics::to_key(ws, i),
					pop_demographics::get_demo(ws, pop.get_pop(), pop_demographics::to_key(ws, i)) / new_total);
			}
		}
	}

	return 0;
}
uint32_t ef_add_war_goal(EFFECT_PARAMTERS) {
	if(auto w = military::find_war_between(ws, trigger::to_nation(primary_slot), trigger::to_nation(from_slot)); w) {
		military::add_wargoal(ws, w, trigger::to_nation(from_slot), trigger::to_nation(primary_slot), trigger::payload(tval[1]).cb_id,
				dcon::state_definition_id{}, dcon::national_identity_id{}, trigger::to_nation(primary_slot));
	}
	return 0;
}
uint32_t ef_move_issue_percentage_nation(EFFECT_PARAMTERS) {
	auto from_issue = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);
	auto to_issue = pop_demographics::to_key(ws, trigger::payload(tval[2]).opt_id);
	auto amount = trigger::read_float_from_payload(tval + 3);

	for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
		for(auto pop : p.get_province().get_pop_location()) {
			auto s = pop_demographics::get_demo(ws, pop.get_pop(), from_issue);
			pop_demographics::set_demo(ws, pop.get_pop(), from_issue, s - s * amount);
			auto s2 = pop_demographics::get_demo(ws, pop.get_pop(), to_issue);
			pop_demographics::set_demo(ws, pop.get_pop(), from_issue, s2 + s * amount);
		}
	}

	return 0;
}
uint32_t ef_move_issue_percentage_state(EFFECT_PARAMTERS) {
	auto from_issue = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);
	auto to_issue = pop_demographics::to_key(ws, trigger::payload(tval[2]).opt_id);
	auto amount = trigger::read_float_from_payload(tval + 3);

	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
		for(auto pop : ws.world.province_get_pop_location(p)) {
			auto s = pop_demographics::get_demo(ws, pop.get_pop(), from_issue);
			pop_demographics::set_demo(ws, pop.get_pop(), from_issue, s - s * amount);
			auto s2 = pop_demographics::get_demo(ws, pop.get_pop(), to_issue);
			pop_demographics::set_demo(ws, pop.get_pop(), from_issue, s2 + s * amount);
		}
	});

	return 0;
}
uint32_t ef_move_issue_percentage_province(EFFECT_PARAMTERS) {
	auto from_issue = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);
	auto to_issue = pop_demographics::to_key(ws, trigger::payload(tval[2]).opt_id);
	auto amount = trigger::read_float_from_payload(tval + 3);

	for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
		auto s = pop_demographics::get_demo(ws, pop.get_pop(), from_issue);
		pop_demographics::set_demo(ws, pop.get_pop(), from_issue, s - s * amount);
		auto s2 = pop_demographics::get_demo(ws, pop.get_pop(), to_issue);
		pop_demographics::set_demo(ws, pop.get_pop(), from_issue, s2 + s * amount);
	}

	return 0;
}
uint32_t ef_move_issue_percentage_pop(EFFECT_PARAMTERS) {
	auto from_issue = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);
	auto to_issue = pop_demographics::to_key(ws, trigger::payload(tval[2]).opt_id);
	auto amount = trigger::read_float_from_payload(tval + 3);

	auto s = pop_demographics::get_demo(ws, trigger::to_pop(primary_slot), from_issue);
	pop_demographics::set_demo(ws, trigger::to_pop(primary_slot), from_issue, s - s * amount);
	auto s2 = pop_demographics::get_demo(ws, trigger::to_pop(primary_slot), to_issue);
	pop_demographics::set_demo(ws, trigger::to_pop(primary_slot), from_issue, s2 + s * amount);

	return 0;
}
uint32_t ef_party_loyalty(EFFECT_PARAMTERS) {
	auto i = trigger::payload(tval[2]).ideo_id;
	auto amount = float(trigger::payload(tval[3]).signed_value) / 100.0f;
	auto& v = ws.world.province_get_party_loyalty(trigger::payload(tval[1]).prov_id, i);
	v = std::clamp(v + amount, -1.0f, 1.0f);
	return 0;
}
uint32_t ef_party_loyalty_province(EFFECT_PARAMTERS) {
	auto i = trigger::payload(tval[1]).ideo_id;
	auto amount = float(trigger::payload(tval[2]).signed_value) / 100.0f;
	auto& v = ws.world.province_get_party_loyalty(trigger::to_prov(primary_slot), i);
	v = std::clamp(v + amount, -1.0f, 1.0f);
	return 0;
}

uint32_t ef_build_railway_in_capital_yes_whole_state_yes_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	auto cs = ws.world.province_get_state_membership(c);
	province::for_each_province_in_state_instance(ws, cs, [&](dcon::province_id p) {
		if(ws.world.province_get_modifier_values(p, sys::provincial_mod_offsets::min_build_railroad) <= 1.0f)
			ws.world.province_get_building_level(p, uint8_t(economy::province_building_type::railroad)) += uint8_t(1);
	});
	ws.railroad_built.store(true, std::memory_order::release);
	return 0;
}
uint32_t ef_build_railway_in_capital_yes_whole_state_no_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	auto cs = ws.world.province_get_state_membership(c);
	province::for_each_province_in_state_instance(ws, cs, [&](dcon::province_id p) {
		if(ws.world.province_get_modifier_values(p, sys::provincial_mod_offsets::min_build_railroad) <= 1.0f)
			ws.world.province_get_building_level(p, uint8_t(economy::province_building_type::railroad)) += uint8_t(1);
	});
	ws.railroad_built.store(true, std::memory_order::release);
	return 0;
}
uint32_t ef_build_railway_in_capital_no_whole_state_yes_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	if(c) {
		if(ws.world.province_get_modifier_values(c, sys::provincial_mod_offsets::min_build_railroad) <= 1.0f) {
			ws.world.province_get_building_level(c, uint8_t(economy::province_building_type::railroad)) += uint8_t(1);
			ws.railroad_built.store(true, std::memory_order::release);
		}
	}
	return 0;
}
uint32_t ef_build_railway_in_capital_no_whole_state_no_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	if(c) {
		if(ws.world.province_get_modifier_values(c, sys::provincial_mod_offsets::min_build_railroad) <= 1.0f) {
			ws.world.province_get_building_level(c, uint8_t(economy::province_building_type::railroad)) += uint8_t(1);
			ws.railroad_built.store(true, std::memory_order::release);
		}
	}
	return 0;
}
uint32_t ef_build_fort_in_capital_yes_whole_state_yes_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	auto cs = ws.world.province_get_state_membership(c);
	province::for_each_province_in_state_instance(ws, cs,
			[&](dcon::province_id p) { ws.world.province_get_building_level(p, uint8_t(economy::province_building_type::fort)) += uint8_t(1); });
	return 0;
}
uint32_t ef_build_fort_in_capital_yes_whole_state_no_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	auto cs = ws.world.province_get_state_membership(c);
	province::for_each_province_in_state_instance(ws, cs,
			[&](dcon::province_id p) { ws.world.province_get_building_level(p, uint8_t(economy::province_building_type::fort)) += uint8_t(1); });
	return 0;
}
uint32_t ef_build_fort_in_capital_no_whole_state_yes_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	if(c) {
		ws.world.province_get_building_level(c, uint8_t(economy::province_building_type::fort)) += uint8_t(1);
	}
	return 0;
}
uint32_t ef_build_fort_in_capital_no_whole_state_no_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	if(c) {
		ws.world.province_get_building_level(c, uint8_t(economy::province_building_type::fort)) += uint8_t(1);
	}
	return 0;
}
uint32_t ef_relation_reb(EFFECT_PARAMTERS) {
	auto itag = ws.world.rebel_faction_get_defection_target(trigger::to_rebel(from_slot));
	if(auto holder = ws.world.national_identity_get_nation_from_identity_holder(itag); holder)
		nations::adjust_relationship(ws, trigger::to_nation(primary_slot), holder, float(trigger::payload(tval[1]).signed_value));
	return 0;
}
uint32_t ef_variable_tech_name_yes(EFFECT_PARAMTERS) {
	if(ws.world.nation_get_active_technologies(trigger::to_nation(primary_slot), trigger::payload(tval[1]).tech_id) == false)
		culture::apply_technology(ws, trigger::to_nation(primary_slot), trigger::payload(tval[1]).tech_id);
	return 0;
}
uint32_t ef_variable_tech_name_no(EFFECT_PARAMTERS) {
	if(ws.world.nation_get_active_technologies(trigger::to_nation(primary_slot), trigger::payload(tval[1]).tech_id) == true)
		culture::remove_technology(ws, trigger::to_nation(primary_slot), trigger::payload(tval[1]).tech_id);
	return 0;
}
uint32_t ef_variable_invention_name_yes(EFFECT_PARAMTERS) {
	if(ws.world.nation_get_active_inventions(trigger::to_nation(primary_slot), trigger::payload(tval[1]).invt_id) == false)
		culture::apply_invention(ws, trigger::to_nation(primary_slot), trigger::payload(tval[1]).invt_id);
	return 0;
}
uint32_t ef_variable_invention_name_no(EFFECT_PARAMTERS) {
	if(ws.world.nation_get_active_inventions(trigger::to_nation(primary_slot), trigger::payload(tval[1]).invt_id) == true)
		culture::remove_invention(ws, trigger::to_nation(primary_slot), trigger::payload(tval[1]).invt_id);
	return 0;
}
uint32_t ef_set_country_flag_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_set_country_flag(tval, ws, trigger::to_generic(owner), 0, 0, 0, 0, els);
	return 0;
}
uint32_t ef_set_country_flag_pop(EFFECT_PARAMTERS) {
	if(auto owner = nations::owner_of_pop(ws, trigger::to_pop(primary_slot)); owner)
		return ef_set_country_flag(tval, ws, trigger::to_generic(owner), 0, 0, 0, 0, els);
	return 0;
}
uint32_t ef_add_country_modifier_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_add_country_modifier(tval, ws, trigger::to_generic(owner), 0, 0, 0, 0, els);
	return 0;
}
uint32_t ef_add_country_modifier_province_no_duration(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_add_country_modifier_no_duration(tval, ws, trigger::to_generic(owner), 0, 0, 0, 0, els);
	return 0;
}
uint32_t ef_relation_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_relation(tval, ws, trigger::to_generic(owner), 0, 0, 0, 0, els);
	return 0;
}
uint32_t ef_relation_province_this_nation(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_relation_this_nation(tval, ws, trigger::to_generic(owner), this_slot, 0, 0, 0, els);
	return 0;
}
uint32_t ef_relation_province_this_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_relation_this_province(tval, ws, trigger::to_generic(owner), this_slot, 0, 0, 0, els);
	return 0;
}
uint32_t ef_relation_province_from_nation(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_relation_this_nation(tval, ws, trigger::to_generic(owner), from_slot, 0, 0, 0, els);
	return 0;
}
uint32_t ef_relation_province_from_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_relation_this_province(tval, ws, trigger::to_generic(owner), from_slot, 0, 0, 0, els);
	return 0;
}
uint32_t ef_relation_province_reb(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_relation_reb(tval, ws, trigger::to_generic(owner), 0, from_slot, 0, 0, els);
	return 0;
}
uint32_t ef_treasury_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_treasury(tval, ws, trigger::to_generic(owner), 0, 0, 0, 0, els);
	return 0;
}

//
// Banks
//
uint32_t ef_build_bank_in_capital_yes_whole_state_yes_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	auto cs = ws.world.province_get_state_membership(c);
	province::for_each_province_in_state_instance(ws, cs, [&](dcon::province_id p) {
		if(ws.world.province_get_modifier_values(p, sys::provincial_mod_offsets::min_build_bank) <= 1.0f)
			ws.world.province_get_building_level(p, uint8_t(economy::province_building_type::bank)) += uint8_t(1);
	});
	return 0;
}
uint32_t ef_build_bank_in_capital_yes_whole_state_no_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	auto cs = ws.world.province_get_state_membership(c);
	province::for_each_province_in_state_instance(ws, cs, [&](dcon::province_id p) {
		if(ws.world.province_get_modifier_values(p, sys::provincial_mod_offsets::min_build_bank) <= 1.0f)
			ws.world.province_get_building_level(p, uint8_t(economy::province_building_type::bank)) += uint8_t(1);
	});
	return 0;
}
uint32_t ef_build_bank_in_capital_no_whole_state_yes_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	if(c) {
		if(ws.world.province_get_modifier_values(c, sys::provincial_mod_offsets::min_build_bank) <= 1.0f)
			ws.world.province_get_building_level(c, uint8_t(economy::province_building_type::bank)) += uint8_t(1);
	}
	return 0;
}
uint32_t ef_build_bank_in_capital_no_whole_state_no_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	if(c) {
		if(ws.world.province_get_modifier_values(c, sys::provincial_mod_offsets::min_build_bank) <= 1.0f)
			ws.world.province_get_building_level(c, uint8_t(economy::province_building_type::bank)) += uint8_t(1);
	}
	return 0;
}
//
// Banks
//
uint32_t ef_build_university_in_capital_yes_whole_state_yes_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	auto cs = ws.world.province_get_state_membership(c);
	province::for_each_province_in_state_instance(ws, cs, [&](dcon::province_id p) {
		if(ws.world.province_get_modifier_values(p, sys::provincial_mod_offsets::min_build_university) <= 1.0f)
			ws.world.province_get_building_level(p, uint8_t(economy::province_building_type::university)) += uint8_t(1);
	});
	return 0;
}
uint32_t ef_build_university_in_capital_yes_whole_state_no_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	auto cs = ws.world.province_get_state_membership(c);
	province::for_each_province_in_state_instance(ws, cs, [&](dcon::province_id p) {
		if(ws.world.province_get_modifier_values(p, sys::provincial_mod_offsets::min_build_university) <= 1.0f)
			ws.world.province_get_building_level(p, uint8_t(economy::province_building_type::university)) += uint8_t(1);
	});
	return 0;
}
uint32_t ef_build_university_in_capital_no_whole_state_yes_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	if(c) {
		if(ws.world.province_get_modifier_values(c, sys::provincial_mod_offsets::min_build_university) <= 1.0f)
			ws.world.province_get_building_level(c, uint8_t(economy::province_building_type::university)) += uint8_t(1);
	}
	return 0;
}
uint32_t ef_build_university_in_capital_no_whole_state_no_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	if(c) {
		if(ws.world.province_get_modifier_values(c, sys::provincial_mod_offsets::min_build_university) <= 1.0f)
			ws.world.province_get_building_level(c, uint8_t(economy::province_building_type::university)) += uint8_t(1);
	}
	return 0;
}

uint32_t ef_annex_to_null_nation(EFFECT_PARAMTERS) {
	auto sprovs = ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot));
	while(sprovs.begin() != sprovs.end()) {
		province::change_province_owner(ws, (*sprovs.begin()).get_province().id, dcon::nation_id{});
	}
	return 0;
}
uint32_t ef_annex_to_null_province(EFFECT_PARAMTERS) {
	province::change_province_owner(ws, trigger::to_prov(primary_slot), dcon::nation_id{ });
	return 0;
}

uint32_t ef_fop_clr_global_flag_2(EFFECT_PARAMTERS) {
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[1]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[2]).glob_id, false);
	return 0;
}
uint32_t ef_fop_clr_global_flag_3(EFFECT_PARAMTERS) {
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[1]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[2]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[3]).glob_id, false);
	return 0;
}
uint32_t ef_fop_clr_global_flag_4(EFFECT_PARAMTERS) {
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[1]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[2]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[3]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[4]).glob_id, false);
	return 0;
}
uint32_t ef_fop_clr_global_flag_5(EFFECT_PARAMTERS) {
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[1]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[2]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[3]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[4]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[5]).glob_id, false);
	return 0;
}
uint32_t ef_fop_clr_global_flag_6(EFFECT_PARAMTERS) {
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[1]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[2]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[3]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[4]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[5]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[6]).glob_id, false);
	return 0;
}
uint32_t ef_fop_clr_global_flag_7(EFFECT_PARAMTERS) {
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[1]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[2]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[3]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[4]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[5]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[6]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[7]).glob_id, false);
	return 0;
}
uint32_t ef_fop_clr_global_flag_8(EFFECT_PARAMTERS) {
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[1]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[2]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[3]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[4]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[5]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[6]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[7]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[8]).glob_id, false);
	return 0;
}
uint32_t ef_fop_clr_global_flag_9(EFFECT_PARAMTERS) {
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[1]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[2]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[3]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[4]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[5]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[6]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[7]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[8]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[9]).glob_id, false);
	return 0;
}
uint32_t ef_fop_clr_global_flag_10(EFFECT_PARAMTERS) {
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[1]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[2]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[3]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[4]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[5]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[6]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[7]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[8]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[9]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[10]).glob_id, false);
	return 0;
}
uint32_t ef_fop_clr_global_flag_11(EFFECT_PARAMTERS) {
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[1]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[2]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[3]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[4]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[5]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[6]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[7]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[8]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[9]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[10]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[11]).glob_id, false);
	return 0;
}
uint32_t ef_fop_clr_global_flag_12(EFFECT_PARAMTERS) {
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[1]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[2]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[3]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[4]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[5]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[6]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[7]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[8]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[9]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[10]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[11]).glob_id, false);
	ws.national_definitions.set_global_flag_variable(trigger::payload(tval[12]).glob_id, false);
	return 0;
}
uint32_t ef_fop_change_province_name(EFFECT_PARAMTERS) {
	dcon::text_key name{ dcon::text_key::value_base_t(trigger::read_int32_t_from_payload(tval + 1)) };
	ws.world.province_set_name(trigger::payload(tval[3]).prov_id, name);
	return 0;
}
uint32_t ef_change_terrain_province(EFFECT_PARAMTERS) {
	auto const p = trigger::to_prov(primary_slot);
	ws.world.province_set_terrain(p, trigger::payload(tval[1]).mod_id);
	return 0;
}
uint32_t ef_change_terrain_pop(EFFECT_PARAMTERS) {
	auto const p = ws.world.pop_get_province_from_pop_location(trigger::to_pop(primary_slot));
	ws.world.province_set_terrain(p, trigger::payload(tval[1]).mod_id);
	return 0;
}
uint32_t ef_masquerade_as_nation_this(EFFECT_PARAMTERS) {
	auto dnid = ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(this_slot));
	ws.world.nation_set_masquerade_identity(trigger::to_nation(primary_slot), dnid);
	return 0;
}
uint32_t ef_masquerade_as_nation_from(EFFECT_PARAMTERS) {
	auto dnid = ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(from_slot));
	ws.world.nation_set_masquerade_identity(trigger::to_nation(primary_slot), dnid);
	return 0;
}

uint32_t ef_change_party_name(EFFECT_PARAMTERS) {
	auto ideo = trigger::payload(tval[1]).ideo_id;
	dcon::text_key new_name{ dcon::text_key::value_base_t(trigger::read_int32_t_from_payload(tval + 2)) };

	if(ideo) {
		auto n = trigger::to_nation(primary_slot);
		auto holder = ws.world.nation_get_identity_from_identity_holder(n);

		auto start = ws.world.national_identity_get_political_party_first(holder).id.index();
		auto end = start + ws.world.national_identity_get_political_party_count(holder);

		for(int32_t i = start; i < end; i++) {
			auto pid = dcon::political_party_id(dcon::political_party_id::value_base_t(i));
			if(politics::political_party_is_active(ws, n, pid) && ws.world.political_party_get_ideology(pid) == ideo) {
				ws.world.political_party_set_name(pid, new_name);
				return 0;
			}
		}
	} else {
		auto n = trigger::to_nation(primary_slot);
		auto rp = ws.world.nation_get_ruling_party(n);
		if(rp) {
			ws.world.political_party_set_name(rp, new_name);
		}
	}
	return 0;
}

uint32_t ef_change_party_position(EFFECT_PARAMTERS) {
	auto ideo = trigger::payload(tval[1]).ideo_id;
	dcon::issue_option_id new_opt = trigger::payload(tval[2]).opt_id;
	auto popt = ws.world.issue_option_get_parent_issue(new_opt);

	if(ideo) {
		auto n = trigger::to_nation(primary_slot);
		auto holder = ws.world.nation_get_identity_from_identity_holder(n);

		auto start = ws.world.national_identity_get_political_party_first(holder).id.index();
		auto end = start + ws.world.national_identity_get_political_party_count(holder);

		for(int32_t i = start; i < end; i++) {
			auto pid = dcon::political_party_id(dcon::political_party_id::value_base_t(i));
			if(politics::political_party_is_active(ws, n, pid) && ws.world.political_party_get_ideology(pid) == ideo) {
				ws.world.political_party_set_party_issues(pid, popt, new_opt);
				if(ws.world.nation_get_ruling_party(n) == pid)
					politics::set_ruling_party(ws, n, pid); // force update of issues and rules
				return 0;
			}
		}
	} else {
		auto n = trigger::to_nation(primary_slot);
		auto rp = ws.world.nation_get_ruling_party(n);
		if(rp) {
			ws.world.political_party_set_party_issues(rp, popt, new_opt);
			politics::set_ruling_party(ws, n, rp); // force update of issues and rules
		}
	}
	return 0;
}

inline constexpr uint32_t(*effect_functions[])(EFFECT_PARAMTERS) = {
		ef_none,
#define EFFECT_BYTECODE_ELEMENT(code, name, arg) ef_##name,
		EFFECT_BYTECODE_LIST
#undef EFFECT_BYTECODE_ELEMENT
		//
		// SCOPES
		//
		es_generic_scope, // constexpr inline uint16_t generic_scope = first_scope_code + 0x0000; // default grouping of effects (or hidden_tooltip)
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
		es_x_substate_scope,										// constexpr inline uint16_t x_state_scope = first_scope_code + 0x0031;
		es_capital_scope_province,								// constexpr inline uint16_t capital_scope = first_scope_code + 0x0032;
		es_x_core_scope_province,                  //constexpr inline uint16_t x_core_scope_province = first_scope_code + 0x0033;
		es_tag_scope,												// constexpr inline uint16_t tag_scope = first_scope_code + 0x0034;
		es_integer_scope,										// constexpr inline uint16_t integer_scope = first_scope_code + 0x0035;
		es_pop_type_scope_nation,						// constexpr inline uint16_t pop_type_scope_nation = first_scope_code + 0x0036;
		es_pop_type_scope_state,						// constexpr inline uint16_t pop_type_scope_state = first_scope_code + 0x0037;
		es_pop_type_scope_province,					// constexpr inline uint16_t pop_type_scope_province = first_scope_code + 0x0038;
		es_region_proper_scope, //constexpr inline uint16_t region_proper_scope = first_scope_code + 0x0039;
		es_region_scope,										// constexpr inline uint16_t region_scope = first_scope_code + 0x003A;
		es_if_scope, // constexpr inline uint16_t if_scope = first_scope_code + 0x003B;
		es_else_if_scope, // constexpr inline uint16_t else_if_scope = first_scope_code + 0x003C;
		es_x_event_country_scope, // constexpr inline uint16_t x_event_country_scope = first_scope_code + 0x003D;
		es_x_decision_country_scope, // constexpr inline uint16_t x_decision_country_scope = first_scope_code + 0x003E;
		es_x_event_country_scope_nation,//constexpr inline uint16_t x_event_country_scope_nation = first_scope_code + 0x003F;
		es_x_decision_country_scope_nation,//constexpr inline uint16_t x_decision_country_scope_nation = first_scope_code + 0x0040;
		es_from_bounce_scope,//constexpr inline uint16_t from_bounce_scope = first_scope_code + 0x0041;
		es_this_bounce_scope,//constexpr inline uint16_t this_bounce_scope = first_scope_code + 0x0042;
		es_random_by_modifier_scope,//constexpr inline uint16_t random_by_modifier_scope = first_scope_code + 0x0043;
		es_x_neighbor_province_scope_nation, // constexpr inline uint16_t x_neighbor_province_scope_nation = first_scope_code + 0x0044;
	es_x_empty_neighbor_province_scope_nation // constexpr inline uint16_t x_empty_neighbor_province_scope_nation = first_scope_code + 0x0045;
	// constexpr inline uint16_t first_invalid_code = first_scope_code + 0x0045;
};

uint32_t internal_execute_effect(EFFECT_PARAMTERS) {
	assert(0 <= (*tval & effect::code_mask) && (*tval & effect::code_mask) < effect::first_invalid_code);
	return effect_functions[*tval & effect::code_mask](tval, ws, primary_slot, this_slot, from_slot, r_lo, r_hi, els);
}

void execute(sys::state& state, dcon::effect_key key, int32_t primary, int32_t this_slot, int32_t from_slot, uint32_t r_lo,
		uint32_t r_hi) {
	bool els = false;
	internal_execute_effect(state.effect_data.data() + state.effect_data_indices[key.index() + 1], state, primary, this_slot, from_slot, r_lo, r_hi, els);
}

void execute(sys::state& state, uint16_t const* data, int32_t primary, int32_t this_slot, int32_t from_slot, uint32_t r_lo,
		uint32_t r_hi) {
	bool els = false;
	internal_execute_effect(data, state, primary, this_slot, from_slot, r_lo, r_hi, els);
}

} // namespace effect
