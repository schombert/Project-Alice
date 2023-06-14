#include "effects.hpp"
#include "system_state.hpp"

namespace effect {

#define EFFECT_PARAMTERS                                                                                                         \
	uint16_t const *tval, sys::state &ws, int32_t primary_slot, int32_t this_slot, int32_t from_slot, uint32_t r_hi, uint32_t r_lo

uint32_t internal_execute_effect(EFFECT_PARAMTERS);

inline uint32_t apply_subeffects(EFFECT_PARAMTERS) {
	auto const source_size = 1 + get_effect_scope_payload_size(tval);
	auto sub_units_start = tval + 2 + effect_scope_data_payload(tval[0]);

	uint32_t i = 0;
	while(sub_units_start < tval + source_size) {
		i += internal_execute_effect(sub_units_start, ws, primary_slot, this_slot, from_slot, r_hi, r_lo + i);
		sub_units_start += 1 + get_generic_effect_payload_size(sub_units_start);
	}
	return i;
}

uint32_t es_generic_scope(EFFECT_PARAMTERS) {
	return apply_subeffects(tval, ws, primary_slot, this_slot, from_slot, r_hi, r_lo);
}

uint32_t es_x_neighbor_province_scope(EFFECT_PARAMTERS) {
	auto neighbor_range = ws.world.province_get_province_adjacency(trigger::to_prov(primary_slot));

	if((tval[0] & effect::is_random_scope) != 0) {
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
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1);
		}
		return 0;
	} else {
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			uint32_t i = 0;
			for(auto p : neighbor_range) {
				auto other = p.get_connected_provinces(0) == trigger::to_prov(primary_slot) ? p.get_connected_provinces(1)
																																										: p.get_connected_provinces(0);

				if(other.get_nation_from_province_ownership() &&
						trigger::evaluate(ws, limit, trigger::to_generic(other.id), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(other.id), this_slot, from_slot, r_hi, r_lo + i);
				}
			}
			return i;
		} else {
			uint32_t i = 0;
			for(auto p : neighbor_range) {
				auto other = p.get_connected_provinces(0) == trigger::to_prov(primary_slot) ? p.get_connected_provinces(1)
																																										: p.get_connected_provinces(0);

				if(other.get_nation_from_province_ownership()) {
					i += apply_subeffects(tval, ws, trigger::to_generic(other.id), this_slot, from_slot, r_hi, r_lo + i);
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
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1);
		}
		return 0;
	} else {
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			uint32_t i = 0;
			for(auto p : neighbor_range) {
				auto other = p.get_connected_nations(0) == trigger::to_nation(primary_slot) ? p.get_connected_nations(1)
																																										: p.get_connected_nations(0);

				if(trigger::evaluate(ws, limit, trigger::to_generic(other.id), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(other.id), this_slot, from_slot, r_hi, r_lo + i);
				}
			}
			return i;
		} else {
			uint32_t i = 0;
			for(auto p : neighbor_range) {
				auto other = p.get_connected_nations(0) == trigger::to_nation(primary_slot) ? p.get_connected_nations(1)
																																										: p.get_connected_nations(0);

				i += apply_subeffects(tval, ws, trigger::to_generic(other.id), this_slot, from_slot, r_hi, r_lo + i);
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
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1);
		}
		return 0;
	} else {
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			uint32_t i = 0;
			for(auto n : ws.world.in_nation) {
				if(trigger::evaluate(ws, limit, trigger::to_generic(n.id), this_slot, from_slot))
					i += apply_subeffects(tval, ws, trigger::to_generic(n.id), this_slot, from_slot, r_hi, r_lo + i);
			}
			return i;
		} else {
			uint32_t i = 0;
			for(auto n : ws.world.in_nation) {
				i += apply_subeffects(tval, ws, trigger::to_generic(n.id), this_slot, from_slot, r_hi, r_lo + i);
			}
			return i;
		}
	}
}
uint32_t es_x_country_scope(EFFECT_PARAMTERS) {
	return es_x_country_scope_nation(tval, ws, trigger::to_generic(dcon::nation_id{}), this_slot, from_slot, r_hi, r_lo);
}
uint32_t es_x_empty_neighbor_province_scope(EFFECT_PARAMTERS) {
	auto neighbor_range = ws.world.province_get_province_adjacency(trigger::to_prov(primary_slot));

	if((tval[0] & effect::is_random_scope) != 0) {
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
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1);
		}
		return 0;
	} else {
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			uint32_t i = 0;
			for(auto p : neighbor_range) {
				auto other = p.get_connected_provinces(0) == trigger::to_prov(primary_slot) ? p.get_connected_provinces(1)
																																										: p.get_connected_provinces(0);

				if(!other.get_nation_from_province_ownership() &&
						trigger::evaluate(ws, limit, trigger::to_generic(other.id), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(other.id), this_slot, from_slot, r_hi, r_lo + i);
				}
			}
			return i;
		} else {
			uint32_t i = 0;
			for(auto p : neighbor_range) {
				auto other = p.get_connected_provinces(0) == trigger::to_prov(primary_slot) ? p.get_connected_provinces(1)
																																										: p.get_connected_provinces(0);

				if(!other.get_nation_from_province_ownership()) {
					i += apply_subeffects(tval, ws, trigger::to_generic(other.id), this_slot, from_slot, r_hi, r_lo + i);
				}
			}
			return i;
		}
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
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1);
		}
		return 0;
	} else {
		if((tval[0] & effect::scope_has_limit) != 0) {
			uint32_t i = 0;
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto& n : ws.great_nations) {
				if(trigger::evaluate(ws, limit, trigger::to_generic(n.nation), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(n.nation), this_slot, from_slot, r_hi, r_lo + i);
				}
			}
			return i;
		} else {
			uint32_t i = 0;
			auto limit = trigger::payload(tval[2]).tr_id;
			for(auto& n : ws.great_nations) {
				i += apply_subeffects(tval, ws, trigger::to_generic(n.nation), this_slot, from_slot, r_hi, r_lo + i);
			}
			return i;
		}
	}
}
uint32_t es_poor_strata_scope_nation(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		uint32_t i = 0;
		for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
			for(auto pop : p.get_province().get_pop_location()) {
				if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::poor) &&
						trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
				}
			}
		}
		return i;
	} else {
		uint32_t i = 0;
		for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
			for(auto pop : p.get_province().get_pop_location()) {
				if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::poor)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
				}
			}
		}
		return i;
	}
}
uint32_t es_poor_strata_scope_state(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		uint32_t i = 0;
		auto d = ws.world.state_instance_get_definition(trigger::to_state(primary_slot));
		auto o = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
		for(auto p : ws.world.state_definition_get_abstract_state_membership(d)) {
			if(p.get_province().get_nation_from_province_ownership() == o) {
				for(auto pop : p.get_province().get_pop_location()) {
					if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::poor) &&
							trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
						i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
					}
				}
			}
		}
		return i;
	} else {
		uint32_t i = 0;
		auto d = ws.world.state_instance_get_definition(trigger::to_state(primary_slot));
		auto o = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
		for(auto p : ws.world.state_definition_get_abstract_state_membership(d)) {
			if(p.get_province().get_nation_from_province_ownership() == o) {
				for(auto pop : p.get_province().get_pop_location()) {
					if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::poor)) {
						i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
					}
				}
			}
		}
		return i;
	}
}
uint32_t es_poor_strata_scope_province(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		uint32_t i = 0;
		for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
			if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::poor) &&
					trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
				i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
			}
		}
		return i;
	} else {
		uint32_t i = 0;
		for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
			if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::poor)) {
				i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
			}
		}
		return i;
	}
}
uint32_t es_middle_strata_scope_nation(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		uint32_t i = 0;
		for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
			for(auto pop : p.get_province().get_pop_location()) {
				if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::middle) &&
						trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
				}
			}
		}
		return i;
	} else {
		uint32_t i = 0;
		for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
			for(auto pop : p.get_province().get_pop_location()) {
				if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::middle)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
				}
			}
		}
		return i;
	}
}
uint32_t es_middle_strata_scope_state(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		uint32_t i = 0;
		auto d = ws.world.state_instance_get_definition(trigger::to_state(primary_slot));
		auto o = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
		for(auto p : ws.world.state_definition_get_abstract_state_membership(d)) {
			if(p.get_province().get_nation_from_province_ownership() == o) {
				for(auto pop : p.get_province().get_pop_location()) {
					if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::middle) &&
							trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
						i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
					}
				}
			}
		}
		return i;
	} else {
		uint32_t i = 0;
		auto d = ws.world.state_instance_get_definition(trigger::to_state(primary_slot));
		auto o = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
		for(auto p : ws.world.state_definition_get_abstract_state_membership(d)) {
			if(p.get_province().get_nation_from_province_ownership() == o) {
				for(auto pop : p.get_province().get_pop_location()) {
					if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::middle)) {
						i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
					}
				}
			}
		}
		return i;
	}
}
uint32_t es_middle_strata_scope_province(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		uint32_t i = 0;
		for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
			if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::middle) &&
					trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
				i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
			}
		}
		return i;
	} else {
		uint32_t i = 0;
		for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
			if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::middle)) {
				i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
			}
		}
		return i;
	}
}
uint32_t es_rich_strata_scope_nation(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		uint32_t i = 0;
		for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
			for(auto pop : p.get_province().get_pop_location()) {
				if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::rich) &&
						trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
				}
			}
		}
		return i;
	} else {
		uint32_t i = 0;
		for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
			for(auto pop : p.get_province().get_pop_location()) {
				if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::rich)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
				}
			}
		}
		return i;
	}
}
uint32_t es_rich_strata_scope_state(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		uint32_t i = 0;
		auto d = ws.world.state_instance_get_definition(trigger::to_state(primary_slot));
		auto o = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
		for(auto p : ws.world.state_definition_get_abstract_state_membership(d)) {
			if(p.get_province().get_nation_from_province_ownership() == o) {
				for(auto pop : p.get_province().get_pop_location()) {
					if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::rich) &&
							trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
						i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
					}
				}
			}
		}
		return i;
	} else {
		uint32_t i = 0;
		auto d = ws.world.state_instance_get_definition(trigger::to_state(primary_slot));
		auto o = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
		for(auto p : ws.world.state_definition_get_abstract_state_membership(d)) {
			if(p.get_province().get_nation_from_province_ownership() == o) {
				for(auto pop : p.get_province().get_pop_location()) {
					if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::rich)) {
						i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
					}
				}
			}
		}
		return i;
	}
}
uint32_t es_rich_strata_scope_province(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		uint32_t i = 0;
		for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
			if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::rich) &&
					trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
				i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
			}
		}
		return i;
	} else {
		uint32_t i = 0;
		for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
			if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::rich)) {
				i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
			}
		}
		return i;
	}
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
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1);
		}
		return 0;
	} else {
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			uint32_t i = 0;
			for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
				for(auto pop : p.get_province().get_pop_location()) {
					if(trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
						i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
					}
				}
			}
			return i;
		} else {
			uint32_t i = 0;
			for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
				for(auto pop : p.get_province().get_pop_location()) {
					i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
				}
			}
			return i;
		}
	}
}
uint32_t es_x_pop_scope_state(EFFECT_PARAMTERS) {
	if((tval[0] & effect::is_random_scope) != 0) {
		std::vector<dcon::pop_id> rlist;

		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			auto d = ws.world.state_instance_get_definition(trigger::to_state(primary_slot));
			auto o = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
			for(auto p : ws.world.state_definition_get_abstract_state_membership(d)) {
				if(p.get_province().get_nation_from_province_ownership() == o) {
					for(auto pop : p.get_province().get_pop_location()) {
						if(trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
							rlist.push_back(pop.get_pop().id);
						}
					}
				}
			}
		} else {
			auto d = ws.world.state_instance_get_definition(trigger::to_state(primary_slot));
			auto o = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
			for(auto p : ws.world.state_definition_get_abstract_state_membership(d)) {
				if(p.get_province().get_nation_from_province_ownership() == o) {
					for(auto pop : p.get_province().get_pop_location()) {
						rlist.push_back(pop.get_pop().id);
					}
				}
			}
		}

		if(rlist.size() != 0) {
			auto r = rng::get_random(ws, r_hi, r_lo) % rlist.size();
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1);
		}
		return 0;
	} else {
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			uint32_t i = 0;
			auto d = ws.world.state_instance_get_definition(trigger::to_state(primary_slot));
			auto o = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
			for(auto p : ws.world.state_definition_get_abstract_state_membership(d)) {
				if(p.get_province().get_nation_from_province_ownership() == o) {
					for(auto pop : p.get_province().get_pop_location()) {
						if(trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
							i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
						}
					}
				}
			}
			return i;
		} else {
			uint32_t i = 0;
			auto d = ws.world.state_instance_get_definition(trigger::to_state(primary_slot));
			auto o = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
			for(auto p : ws.world.state_definition_get_abstract_state_membership(d)) {
				if(p.get_province().get_nation_from_province_ownership() == o) {
					for(auto pop : p.get_province().get_pop_location()) {
						i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
					}
				}
			}
			return i;
		}
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
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1);
		}
		return 0;
	} else {
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			uint32_t i = 0;
			for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
				if(trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
				}
			}
			return i;
		} else {
			uint32_t i = 0;
			for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
				i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
			}
			return i;
		}
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
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1);
		}
		return 0;
	} else {
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			uint32_t i = 0;
			for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
				if(trigger::evaluate(ws, limit, trigger::to_generic(p.get_province().id), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(p.get_province().id), this_slot, from_slot, r_hi, r_lo + i);
				}
			}
			return i;
		} else {
			uint32_t i = 0;
			for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
				i += apply_subeffects(tval, ws, trigger::to_generic(p.get_province().id), this_slot, from_slot, r_hi, r_lo + i);
			}
			return i;
		}
	}
}
uint32_t es_x_owned_scope_state(EFFECT_PARAMTERS) {
	if((tval[0] & effect::is_random_scope) != 0) {
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
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1);
		}
		return 0;
	} else {
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			uint32_t i = 0;
			auto d = ws.world.state_instance_get_definition(trigger::to_state(primary_slot));
			auto o = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
			for(auto p : ws.world.state_definition_get_abstract_state_membership(d)) {
				if(p.get_province().get_nation_from_province_ownership() == o) {
					if(trigger::evaluate(ws, limit, trigger::to_generic(p.get_province().id), this_slot, from_slot)) {
						i += apply_subeffects(tval, ws, trigger::to_generic(p.get_province().id), this_slot, from_slot, r_hi, r_lo + i);
					}
				}
			}
			return i;
		} else {
			uint32_t i = 0;
			auto d = ws.world.state_instance_get_definition(trigger::to_state(primary_slot));
			auto o = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
			for(auto p : ws.world.state_definition_get_abstract_state_membership(d)) {
				if(p.get_province().get_nation_from_province_ownership() == o) {
					i += apply_subeffects(tval, ws, trigger::to_generic(p.get_province().id), this_slot, from_slot, r_hi, r_lo + i);
				}
			}
			return i;
		}
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
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1);
		}
		return 0;
	} else {
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			uint32_t i = 0;
			for(auto p : cores_range) {
				if(trigger::evaluate(ws, limit, trigger::to_generic(p.get_province().id), this_slot, from_slot))
					i += apply_subeffects(tval, ws, trigger::to_generic(p.get_province().id), this_slot, from_slot, r_hi, r_lo + i);
			}
			return i;
		} else {
			uint32_t i = 0;
			for(auto p : cores_range) {
				i += apply_subeffects(tval, ws, trigger::to_generic(p.get_province().id), this_slot, from_slot, r_hi, r_lo + i);
			}
			return i;
		}
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
			return 1 + apply_subeffects(tval, ws, trigger::to_generic(rlist[r]), this_slot, from_slot, r_hi, r_lo + 1);
		}
		return 0;
	} else {
		if((tval[0] & effect::scope_has_limit) != 0) {
			auto limit = trigger::payload(tval[2]).tr_id;
			uint32_t i = 0;
			for(auto si : ws.world.nation_get_state_ownership(trigger::to_nation(primary_slot))) {
				if(trigger::evaluate(ws, limit, trigger::to_generic(si.get_state().id), this_slot, from_slot))
					i += apply_subeffects(tval, ws, trigger::to_generic(si.get_state().id), this_slot, from_slot, r_hi, r_lo + i);
			}
			return i;
		} else {
			uint32_t i = 0;
			for(auto si : ws.world.nation_get_state_ownership(trigger::to_nation(primary_slot))) {
				i += apply_subeffects(tval, ws, trigger::to_generic(si.get_state().id), this_slot, from_slot, r_hi, r_lo + i);
			}
			return i;
		}
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
			return 1 + internal_execute_effect(sub_units_start + 1, ws, primary_slot, this_slot, from_slot, r_hi, r_lo + 1);
		}
		sub_units_start += 2 + effect::get_generic_effect_payload_size(sub_units_start + 1); // each member preceeded by uint16_t
	}
	return 0;
}
uint32_t es_random_scope(EFFECT_PARAMTERS) {
	auto chance = tval[2];
	auto r = int32_t(rng::get_random(ws, r_hi, r_lo) % 100);
	if(r < chance)
		return 1 + apply_subeffects(tval, ws, primary_slot, this_slot, from_slot, r_hi, r_lo + 1);
	return 1;
}
uint32_t es_owner_scope_state(EFFECT_PARAMTERS) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
	if(owner)
		return apply_subeffects(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo);
	return 0;
}
uint32_t es_owner_scope_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot));
	if(owner)
		return apply_subeffects(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo);
	return 0;
}
uint32_t es_controller_scope(EFFECT_PARAMTERS) {
	auto controller = ws.world.province_get_nation_from_province_control(trigger::to_prov(primary_slot));
	if(controller)
		return apply_subeffects(tval, ws, trigger::to_generic(controller), this_slot, from_slot, r_hi, r_lo);
	return 0;
}
uint32_t es_location_scope(EFFECT_PARAMTERS) {
	auto owner = ws.world.pop_get_province_from_pop_location(trigger::to_pop(primary_slot));
	if(owner)
		return apply_subeffects(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo);
	return 0;
}
uint32_t es_country_scope_pop(EFFECT_PARAMTERS) {
	auto owner = nations::owner_of_pop(ws, trigger::to_pop(primary_slot));
	if(owner)
		return apply_subeffects(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo);
	return 0;
}
uint32_t es_country_scope_state(EFFECT_PARAMTERS) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
	if(owner)
		return apply_subeffects(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo);
	return 0;
}
uint32_t es_capital_scope(EFFECT_PARAMTERS) {
	auto owner = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	if(owner)
		return apply_subeffects(tval, ws, trigger::to_generic(owner), this_slot, from_slot, r_hi, r_lo);
	return 0;
}
uint32_t es_this_scope_nation(EFFECT_PARAMTERS) {
	return apply_subeffects(tval, ws, this_slot, this_slot, from_slot, r_hi, r_lo);
}
uint32_t es_this_scope_state(EFFECT_PARAMTERS) {
	return apply_subeffects(tval, ws, this_slot, this_slot, from_slot, r_hi, r_lo);
}
uint32_t es_this_scope_province(EFFECT_PARAMTERS) {
	return apply_subeffects(tval, ws, this_slot, this_slot, from_slot, r_hi, r_lo);
}
uint32_t es_this_scope_pop(EFFECT_PARAMTERS) {
	return apply_subeffects(tval, ws, this_slot, this_slot, from_slot, r_hi, r_lo);
}
uint32_t es_from_scope_nation(EFFECT_PARAMTERS) {
	return apply_subeffects(tval, ws, from_slot, this_slot, from_slot, r_hi, r_lo);
}
uint32_t es_from_scope_state(EFFECT_PARAMTERS) {
	return apply_subeffects(tval, ws, from_slot, this_slot, from_slot, r_hi, r_lo);
}
uint32_t es_from_scope_province(EFFECT_PARAMTERS) {
	return apply_subeffects(tval, ws, from_slot, this_slot, from_slot, r_hi, r_lo);
}
uint32_t es_from_scope_pop(EFFECT_PARAMTERS) {
	return apply_subeffects(tval, ws, from_slot, this_slot, from_slot, r_hi, r_lo);
}
uint32_t es_sea_zone_scope(EFFECT_PARAMTERS) {
	auto pid = fatten(ws.world, trigger::to_prov(primary_slot));
	for(auto adj : pid.get_province_adjacency()) {
		if(adj.get_connected_provinces(0).id.index() >= ws.province_definitions.first_sea_province.index()) {
			return apply_subeffects(tval, ws, trigger::to_generic(adj.get_connected_provinces(0).id), this_slot, from_slot, r_hi, r_lo);
		} else if(adj.get_connected_provinces(1).id.index() >= ws.province_definitions.first_sea_province.index()) {
			return apply_subeffects(tval, ws, trigger::to_generic(adj.get_connected_provinces(1).id), this_slot, from_slot, r_hi, r_lo);
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
		return apply_subeffects(tval, ws, trigger::to_generic(group_holders), this_slot, from_slot, r_hi, r_lo);
	return 0;
}
uint32_t es_overlord_scope(EFFECT_PARAMTERS) {
	auto overlord = ws.world.nation_get_overlord_as_subject(trigger::to_nation(primary_slot));
	auto on = ws.world.overlord_get_ruler(overlord);
	if(on)
		return apply_subeffects(tval, ws, trigger::to_generic(on), this_slot, from_slot, r_hi, r_lo);
	return 0;
}
uint32_t es_sphere_owner_scope(EFFECT_PARAMTERS) {
	auto sphere_leader = ws.world.nation_get_in_sphere_of(trigger::to_nation(primary_slot));
	if(sphere_leader)
		return apply_subeffects(tval, ws, trigger::to_generic(sphere_leader), this_slot, from_slot, r_hi, r_lo);
	return 0;
}
uint32_t es_independence_scope(EFFECT_PARAMTERS) {
	auto rtag = ws.world.rebel_faction_get_defection_target(trigger::to_rebel(primary_slot));
	auto rnation = ws.world.national_identity_get_nation_from_identity_holder(rtag);
	if(rnation) {
		return apply_subeffects(tval, ws, trigger::to_generic(rnation), this_slot, from_slot, r_hi, r_lo);
	}
	return 0;
}
uint32_t es_flashpoint_tag_scope(EFFECT_PARAMTERS) {
	auto ctag = ws.world.state_instance_get_flashpoint_tag(trigger::to_state(primary_slot));
	auto rnation = ws.world.national_identity_get_nation_from_identity_holder(ctag);
	if(rnation) {
		return apply_subeffects(tval, ws, trigger::to_generic(rnation), this_slot, from_slot, r_hi, r_lo);
	}
	return 0;
}
uint32_t es_crisis_state_scope(EFFECT_PARAMTERS) {
	auto cstate = ws.crisis_state;
	if(cstate)
		return apply_subeffects(tval, ws, trigger::to_generic(cstate), this_slot, from_slot, r_hi, r_lo);
	return 0;
}
uint32_t es_state_scope_province(EFFECT_PARAMTERS) {
	auto state = ws.world.province_get_state_membership(trigger::to_prov(primary_slot));
	if(state)
		return apply_subeffects(tval, ws, trigger::to_generic(state), this_slot, from_slot, r_hi, r_lo);
	return 0;
}
uint32_t es_state_scope_pop(EFFECT_PARAMTERS) {
	auto pop_province = ws.world.pop_get_province_from_pop_location(trigger::to_pop(primary_slot));
	auto state = ws.world.province_get_state_membership(pop_province);
	if(state)
		return apply_subeffects(tval, ws, trigger::to_generic(state), this_slot, from_slot, r_hi, r_lo);
	return 0;
}
uint32_t es_tag_scope(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		auto tag = trigger::payload(tval[3]).tag_id;
		auto tag_holder = ws.world.national_identity_get_nation_from_identity_holder(tag);
		if(tag_holder && trigger::evaluate(ws, limit, trigger::to_generic(tag_holder), this_slot, from_slot))
			return apply_subeffects(tval, ws, trigger::to_generic(tag_holder), this_slot, from_slot, r_hi, r_lo);
		return 0;
	} else {
		auto tag = trigger::payload(tval[2]).tag_id;
		auto tag_holder = ws.world.national_identity_get_nation_from_identity_holder(tag);
		if(tag_holder)
			return apply_subeffects(tval, ws, trigger::to_generic(tag_holder), this_slot, from_slot, r_hi, r_lo);
		return 0;
	}
}
uint32_t es_integer_scope(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		auto prov = trigger::payload(tval[3]).prov_id;

		if(prov && trigger::evaluate(ws, limit, trigger::to_generic(prov), this_slot, from_slot))
			return apply_subeffects(tval, ws, trigger::to_generic(prov), this_slot, from_slot, r_hi, r_lo);
		return 0;
	} else {
		auto prov = trigger::payload(tval[2]).prov_id;
		if(prov)
			return apply_subeffects(tval, ws, trigger::to_generic(prov), this_slot, from_slot, r_hi, r_lo);
		return 0;
	}
}
uint32_t es_pop_type_scope_nation(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		auto type = trigger::payload(tval[3]).popt_id;

		uint32_t i = 0;
		for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
			for(auto pop : p.get_province().get_pop_location()) {
				if(pop.get_pop().get_poptype() == type &&
						trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
					i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
				}
			}
		}
		return i;
	} else {
		auto type = trigger::payload(tval[2]).popt_id;

		uint32_t i = 0;
		for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
			for(auto pop : p.get_province().get_pop_location()) {
				if(pop.get_pop().get_poptype() == type) {
					i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
				}
			}
		}
		return i;
	}
}
uint32_t es_pop_type_scope_state(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto limit = trigger::payload(tval[2]).tr_id;
		auto type = trigger::payload(tval[3]).popt_id;

		uint32_t i = 0;
		auto d = ws.world.state_instance_get_definition(trigger::to_state(primary_slot));
		auto o = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
		for(auto p : ws.world.state_definition_get_abstract_state_membership(d)) {
			if(p.get_province().get_nation_from_province_ownership() == o) {
				for(auto pop : p.get_province().get_pop_location()) {
					if(pop.get_pop().get_poptype() == type &&
							trigger::evaluate(ws, limit, trigger::to_generic(pop.get_pop().id), this_slot, from_slot)) {
						i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
					}
				}
			}
		}
		return i;
	} else {
		auto type = trigger::payload(tval[2]).popt_id;

		uint32_t i = 0;
		auto d = ws.world.state_instance_get_definition(trigger::to_state(primary_slot));
		auto o = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(primary_slot));
		for(auto p : ws.world.state_definition_get_abstract_state_membership(d)) {
			if(p.get_province().get_nation_from_province_ownership() == o) {
				for(auto pop : p.get_province().get_pop_location()) {
					if(pop.get_pop().get_poptype() == type) {
						i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
					}
				}
			}
		}
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
				i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
			}
		}
		return i;
	} else {
		auto type = trigger::payload(tval[2]).popt_id;

		uint32_t i = 0;
		for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
			if(pop.get_pop().get_poptype() == type) {
				i += apply_subeffects(tval, ws, trigger::to_generic(pop.get_pop().id), this_slot, from_slot, r_hi, r_lo + i);
			}
		}
		return i;
	}
}
uint32_t es_region_scope(EFFECT_PARAMTERS) {
	if((tval[0] & effect::scope_has_limit) != 0) {
		auto region = trigger::payload(tval[3]).state_id;
		auto limit = trigger::payload(tval[2]).tr_id;

		uint32_t i = 0;
		for(auto si : ws.world.in_state_instance) {
			if(si.get_definition() == region && trigger::evaluate(ws, limit, trigger::to_generic(si.id), this_slot, from_slot)) {
				i += apply_subeffects(tval, ws, trigger::to_generic(si.id), this_slot, from_slot, r_hi, r_lo + i);
			}
		}
		return i;
	} else {
		auto region = trigger::payload(tval[2]).state_id;
		uint32_t i = 0;
		for(auto si : ws.world.in_state_instance) {
			if(si.get_definition() == region) {
				i += apply_subeffects(tval, ws, trigger::to_generic(si.id), this_slot, from_slot, r_hi, r_lo + i);
			}
		}
		return i;
	}
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
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot),
			[&](dcon::province_id p) { province::add_core(ws, p, tag); });
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
		return ef_add_core_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo);
	else
		return 0;
}
uint32_t ef_add_core_this_state(EFFECT_PARAMTERS) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot));
	if(owner)
		return ef_add_core_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo);
	else
		return 0;
}
uint32_t ef_add_core_this_pop(EFFECT_PARAMTERS) {
	auto owner = nations::owner_of_pop(ws, trigger::to_pop(this_slot));
	if(owner)
		return ef_add_core_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo);
	else
		return 0;
}
uint32_t ef_add_core_from_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	if(owner)
		return ef_add_core_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo);
	else
		return 0;
}
uint32_t ef_add_core_from_nation(EFFECT_PARAMTERS) {
	return ef_add_core_this_nation(tval, ws, primary_slot, from_slot, 0, r_hi, r_lo);
}
uint32_t ef_add_core_reb(EFFECT_PARAMTERS) {
	auto tag = ws.world.rebel_faction_get_defection_target(trigger::to_rebel(from_slot));
	province::add_core(ws, trigger::to_prov(primary_slot), tag);
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
uint32_t ef_remove_core_int(EFFECT_PARAMTERS) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(primary_slot));
	auto prov = trigger::payload(tval[1]).prov_id;
	province::remove_core(ws, prov, tag);
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
		return ef_remove_core_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo);
	else
		return 0;
}
uint32_t ef_remove_core_this_state(EFFECT_PARAMTERS) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot));
	if(owner)
		return ef_remove_core_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo);
	else
		return 0;
}
uint32_t ef_remove_core_this_pop(EFFECT_PARAMTERS) {
	auto owner = nations::owner_of_pop(ws, trigger::to_pop(this_slot));
	if(owner)
		return ef_remove_core_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo);
	else
		return 0;
}
uint32_t ef_remove_core_from_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	if(owner)
		return ef_remove_core_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo);
	return 0;
}
uint32_t ef_remove_core_from_nation(EFFECT_PARAMTERS) {
	return ef_remove_core_this_nation(tval, ws, primary_slot, from_slot, 0, r_hi, r_lo);
}
uint32_t ef_remove_core_reb(EFFECT_PARAMTERS) {
	auto tag = ws.world.rebel_faction_get_defection_target(trigger::to_rebel(from_slot));
	province::remove_core(ws, trigger::to_prov(primary_slot), tag);
	return 0;
}
uint32_t ef_change_region_name_state(EFFECT_PARAMTERS) {
	auto def = ws.world.state_instance_get_definition(trigger::to_state(primary_slot));
	ws.world.state_definition_set_name(def, trigger::payload(tval[1]).text_id);
	return 0;
}
uint32_t ef_change_region_name_province(EFFECT_PARAMTERS) {
	auto def = ws.world.province_get_state_from_abstract_state_membership(trigger::to_prov(primary_slot));
	if(def) {
		ws.world.state_definition_set_name(def, trigger::payload(tval[1]).text_id);
	}
	return 0;
}
uint32_t ef_trade_goods(EFFECT_PARAMTERS) {
	province::set_rgo(ws, trigger::to_prov(primary_slot), trigger::payload(tval[1]).com_id);
	return 0;
}
uint32_t ef_add_accepted_culture(EFFECT_PARAMTERS) {
	ws.world.nation_get_accepted_cultures(trigger::to_nation(primary_slot)).add_unique(trigger::payload(tval[1]).cul_id);
	nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_add_accepted_culture_union(EFFECT_PARAMTERS) {
	auto prim_culture = ws.world.nation_get_primary_culture(trigger::to_nation(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(prim_culture);
	for(auto c : ws.world.culture_group_get_culture_group_membership(cg)) {
		ws.world.nation_get_accepted_cultures(trigger::to_nation(primary_slot)).add_unique(c.get_member().id);
	}
	nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_primary_culture(EFFECT_PARAMTERS) {
	ws.world.nation_set_primary_culture(trigger::to_nation(primary_slot), trigger::payload(tval[1]).cul_id);
	nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_primary_culture_this_nation(EFFECT_PARAMTERS) {
	ws.world.nation_set_primary_culture(trigger::to_nation(primary_slot),
			ws.world.nation_get_primary_culture(trigger::to_nation(this_slot)));
	nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_primary_culture_this_state(EFFECT_PARAMTERS) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot));
	if(owner) {
		ws.world.nation_set_primary_culture(trigger::to_nation(primary_slot), ws.world.nation_get_primary_culture(owner));
		nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	}
	return 0;
}
uint32_t ef_primary_culture_this_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(owner) {
		ws.world.nation_set_primary_culture(trigger::to_nation(primary_slot), ws.world.nation_get_primary_culture(owner));
		nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	}
	return 0;
}
uint32_t ef_primary_culture_this_pop(EFFECT_PARAMTERS) {
	auto owner = nations::owner_of_pop(ws, trigger::to_pop(this_slot));
	if(owner) {
		ws.world.nation_set_primary_culture(trigger::to_nation(primary_slot), ws.world.nation_get_primary_culture(owner));
		nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	}
	return 0;
}
uint32_t ef_primary_culture_from_nation(EFFECT_PARAMTERS) {
	ws.world.nation_set_primary_culture(trigger::to_nation(primary_slot),
			ws.world.nation_get_primary_culture(trigger::to_nation(from_slot)));
	nations::update_pop_acceptance(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_remove_accepted_culture(EFFECT_PARAMTERS) {
	ws.world.nation_get_accepted_cultures(trigger::to_nation(primary_slot)).remove_unique(trigger::payload(tval[1]).cul_id);
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
uint32_t ef_religion(EFFECT_PARAMTERS) {
	ws.world.nation_set_religion(trigger::to_nation(primary_slot), trigger::payload(tval[1]).rel_id);
	return 0;
}
uint32_t ef_is_slave_state_yes(EFFECT_PARAMTERS) {
	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot),
			[&](dcon::province_id p) { ws.world.province_set_is_slave(p, true); });
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
	if(old_holder)
		ws.world.nation_set_identity_from_identity_holder(old_holder, tag);
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
	if(old_holder)
		ws.world.nation_set_identity_from_identity_holder(old_holder, tag);
	politics::update_displayed_identity(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_change_tag_no_core_switch(EFFECT_PARAMTERS) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(!holder)
		return 0;

	auto old_controller = ws.world.nation_get_is_player_controlled(holder);
	ws.world.nation_set_is_player_controlled(holder, ws.world.nation_get_is_player_controlled(trigger::to_nation(primary_slot)));
	ws.world.nation_set_is_player_controlled(trigger::to_nation(primary_slot), old_controller);

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

	auto old_controller = ws.world.nation_get_is_player_controlled(holder);
	ws.world.nation_set_is_player_controlled(holder, ws.world.nation_get_is_player_controlled(trigger::to_nation(primary_slot)));
	ws.world.nation_set_is_player_controlled(trigger::to_nation(primary_slot), old_controller);

	if(ws.local_player_nation == trigger::to_nation(primary_slot)) {
		ws.local_player_nation = holder;
	} else if(ws.local_player_nation == holder) {
		ws.local_player_nation = trigger::to_nation(primary_slot);
	}

	auto tag = ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(primary_slot));
	culture::replace_cores(ws, tag, u);
	ws.world.nation_set_identity_from_identity_holder(trigger::to_nation(primary_slot), u);
	politics::update_displayed_identity(ws, trigger::to_nation(primary_slot));
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
	return ef_secede_province_this_nation(tval, ws, primary_slot, from_slot, 0, r_hi, r_lo);
}
uint32_t ef_secede_province_from_province(EFFECT_PARAMTERS) {
	return ef_secede_province_this_province(tval, ws, primary_slot, from_slot, 0, r_hi, r_lo);
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
	return ef_release_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo);
}
uint32_t ef_release_this_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	return ef_release_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo);
}
uint32_t ef_release_this_pop(EFFECT_PARAMTERS) {
	auto owner = nations::owner_of_pop(ws, trigger::to_pop(this_slot));
	return ef_release_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo);
}
uint32_t ef_release_from_nation(EFFECT_PARAMTERS) {
	nations::liberate_nation_from(ws, ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(from_slot)),
			trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_release_from_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	return ef_release_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_hi, r_lo);
}
uint32_t ef_change_controller(EFFECT_PARAMTERS) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(holder && ws.world.province_get_nation_from_province_control(trigger::to_prov(primary_slot)) != holder) {
		ws.world.province_set_nation_from_province_control(trigger::to_prov(primary_slot), holder);
		ws.world.province_set_rebel_faction_from_province_rebel_control(trigger::to_prov(primary_slot), dcon::rebel_faction_id{});
		ws.world.province_set_last_control_change(trigger::to_prov(primary_slot), ws.current_date);
		military::update_blackflag_status(ws, trigger::to_prov(primary_slot));
		military::eject_ships(ws, trigger::to_prov(primary_slot));
	}
	return 0;
}
uint32_t ef_change_controller_this_nation(EFFECT_PARAMTERS) {
	if(ws.world.province_get_nation_from_province_control(trigger::to_prov(primary_slot)) != trigger::to_nation(this_slot)) {
		ws.world.province_set_nation_from_province_control(trigger::to_prov(primary_slot), trigger::to_nation(this_slot));
		ws.world.province_set_rebel_faction_from_province_rebel_control(trigger::to_prov(primary_slot), dcon::rebel_faction_id{});
		ws.world.province_set_last_control_change(trigger::to_prov(primary_slot), ws.current_date);
		military::update_blackflag_status(ws, trigger::to_prov(primary_slot));
		military::eject_ships(ws, trigger::to_prov(primary_slot));
	}
	return 0;
}
uint32_t ef_change_controller_this_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(owner && ws.world.province_get_nation_from_province_control(trigger::to_prov(primary_slot)) != owner) {
		ws.world.province_set_nation_from_province_control(trigger::to_prov(primary_slot), owner);
		ws.world.province_set_rebel_faction_from_province_rebel_control(trigger::to_prov(primary_slot), dcon::rebel_faction_id{});
		ws.world.province_set_last_control_change(trigger::to_prov(primary_slot), ws.current_date);
		military::update_blackflag_status(ws, trigger::to_prov(primary_slot));
		military::eject_ships(ws, trigger::to_prov(primary_slot));
	}
	return 0;
}
uint32_t ef_change_controller_from_nation(EFFECT_PARAMTERS) {
	if(ws.world.province_get_nation_from_province_control(trigger::to_prov(primary_slot)) != trigger::to_nation(from_slot)) {
		ws.world.province_set_nation_from_province_control(trigger::to_prov(primary_slot), trigger::to_nation(from_slot));
		ws.world.province_set_rebel_faction_from_province_rebel_control(trigger::to_prov(primary_slot), dcon::rebel_faction_id{});
		ws.world.province_set_last_control_change(trigger::to_prov(primary_slot), ws.current_date);
		military::update_blackflag_status(ws, trigger::to_prov(primary_slot));
		military::eject_ships(ws, trigger::to_prov(primary_slot));
	}
	return 0;
}
uint32_t ef_change_controller_from_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	if(owner && ws.world.province_get_nation_from_province_control(trigger::to_prov(primary_slot)) != owner) {
		ws.world.province_set_nation_from_province_control(trigger::to_prov(primary_slot), owner);
		ws.world.province_set_rebel_faction_from_province_rebel_control(trigger::to_prov(primary_slot), dcon::rebel_faction_id{});
		ws.world.province_set_last_control_change(trigger::to_prov(primary_slot), ws.current_date);
		military::update_blackflag_status(ws, trigger::to_prov(primary_slot));
		military::eject_ships(ws, trigger::to_prov(primary_slot));
	}
	return 0;
}
uint32_t ef_infrastructure(EFFECT_PARAMTERS) {
	auto& rr_level = ws.world.province_get_railroad_level(trigger::to_prov(primary_slot));
	rr_level = uint8_t(std::clamp(int32_t(rr_level) + int32_t(trigger::payload(tval[1]).signed_value), 0, 255));
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
	military::end_wars_between(ws, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)),
			trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_end_war_from_nation(EFFECT_PARAMTERS) {
	military::end_wars_between(ws, trigger::to_nation(primary_slot), trigger::to_nation(from_slot));
	return 0;
}
uint32_t ef_end_war_from_province(EFFECT_PARAMTERS) {
	military::end_wars_between(ws, ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)),
			trigger::to_nation(primary_slot));
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
uint32_t ef_remove_country_modifier(EFFECT_PARAMTERS) {
	sys::remove_modifier_from_nation(ws, trigger::to_nation(primary_slot), trigger::payload(tval[1]).mod_id);
	return 0;
}
uint32_t ef_create_alliance(EFFECT_PARAMTERS) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(holder)
		nations::make_alliance(ws, holder, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_create_alliance_this_nation(EFFECT_PARAMTERS) {
	nations::make_alliance(ws, trigger::to_nation(primary_slot), trigger::to_nation(this_slot));
	return 0;
}
uint32_t ef_create_alliance_this_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	if(owner)
		nations::make_alliance(ws, trigger::to_nation(primary_slot), owner);
	return 0;
}
uint32_t ef_create_alliance_from_nation(EFFECT_PARAMTERS) {
	nations::make_alliance(ws, trigger::to_nation(primary_slot), trigger::to_nation(from_slot));
	return 0;
}
uint32_t ef_create_alliance_from_province(EFFECT_PARAMTERS) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot));
	if(owner)
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
		ws.world.force_create_overlord(holder, trigger::to_nation(primary_slot));
		if(ws.world.nation_get_is_great_power(trigger::to_nation(primary_slot))) {
			auto sr = ws.world.force_create_gp_relationship(holder, trigger::to_nation(primary_slot));
			auto& flags = ws.world.gp_relationship_get_status(sr);
			flags = uint8_t((flags & ~nations::influence::level_mask) | nations::influence::level_in_sphere);
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
		ws.world.force_create_overlord(trigger::to_nation(this_slot), trigger::to_nation(primary_slot));
		if(ws.world.nation_get_is_great_power(trigger::to_nation(primary_slot))) {
			auto sr = ws.world.force_create_gp_relationship(trigger::to_nation(this_slot), trigger::to_nation(primary_slot));
			auto& flags = ws.world.gp_relationship_get_status(sr);
			flags = uint8_t((flags & ~nations::influence::level_mask) | nations::influence::level_in_sphere);
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
		ws.world.force_create_overlord(holder, trigger::to_nation(primary_slot));
		if(ws.world.nation_get_is_great_power(trigger::to_nation(primary_slot))) {
			auto sr = ws.world.force_create_gp_relationship(holder, trigger::to_nation(primary_slot));
			auto& flags = ws.world.gp_relationship_get_status(sr);
			flags = uint8_t((flags & ~nations::influence::level_mask) | nations::influence::level_in_sphere);
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
		ws.world.force_create_overlord(trigger::to_nation(from_slot), trigger::to_nation(primary_slot));
		if(ws.world.nation_get_is_great_power(trigger::to_nation(primary_slot))) {
			auto sr = ws.world.force_create_gp_relationship(trigger::to_nation(from_slot), trigger::to_nation(primary_slot));
			auto& flags = ws.world.gp_relationship_get_status(sr);
			flags = uint8_t((flags & ~nations::influence::level_mask) | nations::influence::level_in_sphere);
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
		ws.world.force_create_overlord(holder, trigger::to_nation(primary_slot));
		if(ws.world.nation_get_is_great_power(trigger::to_nation(primary_slot))) {
			auto sr = ws.world.force_create_gp_relationship(holder, trigger::to_nation(primary_slot));
			auto& flags = ws.world.gp_relationship_get_status(sr);
			flags = uint8_t((flags & ~nations::influence::level_mask) | nations::influence::level_in_sphere);
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
		ws.world.force_create_overlord(holder, trigger::to_nation(primary_slot));
		if(ws.world.nation_get_is_great_power(trigger::to_nation(primary_slot))) {
			auto sr = ws.world.force_create_gp_relationship(holder, trigger::to_nation(primary_slot));
			auto& flags = ws.world.gp_relationship_get_status(sr);
			flags = uint8_t((flags & ~nations::influence::level_mask) | nations::influence::level_in_sphere);
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
uint32_t ef_change_province_name(EFFECT_PARAMTERS) {
	ws.world.province_set_name(trigger::to_prov(primary_slot), trigger::payload(tval[1]).text_id);
	return 0;
}
uint32_t ef_enable_canal(EFFECT_PARAMTERS) {
	province::enable_canal(ws, tval[2] - 1);
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
uint32_t ef_election(EFFECT_PARAMTERS) {
	politics::start_election(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_social_reform(EFFECT_PARAMTERS) {
	auto opt = trigger::payload(tval[1]).opt_id;
	politics::set_issue_option(ws, trigger::to_nation(primary_slot), opt);
	culture::update_nation_issue_rules(ws, trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_political_reform(EFFECT_PARAMTERS) {
	auto opt = trigger::payload(tval[1]).opt_id;
	politics::set_issue_option(ws, trigger::to_nation(primary_slot), opt);
	culture::update_nation_issue_rules(ws, trigger::to_nation(primary_slot));
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
	return 0;
}
uint32_t ef_economic_reform(EFFECT_PARAMTERS) {
	auto opt = trigger::payload(tval[1]).ropt_id;
	politics::set_reform_option(ws, trigger::to_nation(primary_slot), opt);
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
	}
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
	}
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
uint32_t ef_literacy(EFFECT_PARAMTERS) {
	auto& l = ws.world.pop_get_literacy(trigger::to_pop(primary_slot));
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));
	l = std::clamp(l + amount, 0.0f, 1.0f);
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
uint32_t ef_add_crisis_temperature(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));

	ws.crisis_temperature = std::clamp(ws.crisis_temperature + amount, 0.0f, 100.0f);
	return 0;
}
uint32_t ef_consciousness(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));

	auto& c = ws.world.pop_get_consciousness(trigger::to_pop(primary_slot));
	c = std::clamp(c + amount, 0.0f, 10.0f);
	return 0;
}
uint32_t ef_militancy(EFFECT_PARAMTERS) {
	auto amount = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(amount));

	auto& c = ws.world.pop_get_militancy(trigger::to_pop(primary_slot));
	c = std::clamp(c + amount, 0.0f, 10.0f);
	return 0;
}
uint32_t ef_rgo_size(EFFECT_PARAMTERS) {
	auto& s = ws.world.province_get_rgo_size(trigger::to_prov(primary_slot));
	s = std::max(s + float(trigger::payload(tval[2]).signed_value), 0.0f);
	return 0;
}
uint32_t ef_fort(EFFECT_PARAMTERS) {
	auto& fort_level = ws.world.province_get_fort_level(trigger::to_prov(primary_slot));
	fort_level = uint8_t(std::clamp(int32_t(fort_level) + int32_t(trigger::payload(tval[2]).signed_value), 0, 255));
	return 0;
}
uint32_t ef_naval_base(EFFECT_PARAMTERS) {
	auto& naval_base_level = ws.world.province_get_naval_base_level(trigger::to_prov(primary_slot));
	naval_base_level = uint8_t(std::clamp(int32_t(naval_base_level) + int32_t(trigger::payload(tval[2]).signed_value), 0, 255));
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
uint32_t ef_add_country_modifier(EFFECT_PARAMTERS) {
	sys::add_modifier_to_nation(ws, trigger::to_nation(primary_slot), trigger::payload(tval[1]).mod_id,
			ws.current_date + trigger::payload(tval[2]).signed_value);
	return 0;
}
uint32_t ef_add_country_modifier_no_duration(EFFECT_PARAMTERS) {
	sys::add_modifier_to_nation(ws, trigger::to_nation(primary_slot), trigger::payload(tval[1]).mod_id, sys::date{});
	return 0;
}
uint32_t ef_casus_belli_tag(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	auto tag_target = trigger::payload(tval[3]).tag_id;

	if(auto holder = ws.world.national_identity_get_nation_from_identity_holder(tag_target); holder) {
		ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot))
				.push_back(military::available_cb{holder, months > 0 ? ws.current_date + 31 * months : sys::date{}, type});
	}
	return 0;
}
uint32_t ef_casus_belli_int(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = ws.world.province_get_nation_from_province_ownership(trigger::payload(tval[3]).prov_id); holder) {
		ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot))
				.push_back(military::available_cb{holder, months > 0 ? ws.current_date + 31 * months : sys::date{}, type});
	}
	return 0;
}
uint32_t ef_casus_belli_this_nation(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot))
			.push_back(
					military::available_cb{trigger::to_nation(this_slot), months > 0 ? ws.current_date + 31 * months : sys::date{}, type});

	return 0;
}
uint32_t ef_casus_belli_this_state(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)); holder) {
		ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot))
				.push_back(military::available_cb{holder, months > 0 ? ws.current_date + 31 * months : sys::date{}, type});
	}
	return 0;
}
uint32_t ef_casus_belli_this_province(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)); holder) {
		ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot))
				.push_back(military::available_cb{holder, months > 0 ? ws.current_date + 31 * months : sys::date{}, type});
	}
	return 0;
}
uint32_t ef_casus_belli_this_pop(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = nations::owner_of_pop(ws, trigger::to_pop(this_slot)); holder) {
		ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot))
				.push_back(military::available_cb{holder, months > 0 ? ws.current_date + 31 * months : sys::date{}, type});
	}
	return 0;
}
uint32_t ef_casus_belli_from_nation(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot))
			.push_back(
					military::available_cb{trigger::to_nation(from_slot), months > 0 ? ws.current_date + 31 * months : sys::date{}, type});

	return 0;
}
uint32_t ef_casus_belli_from_province(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)); holder) {
		ws.world.nation_get_available_cbs(trigger::to_nation(primary_slot))
				.push_back(military::available_cb{holder, months > 0 ? ws.current_date + 31 * months : sys::date{}, type});
	}
	return 0;
}
uint32_t ef_add_casus_belli_tag(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;
	auto tag_target = trigger::payload(tval[3]).tag_id;

	if(auto holder = ws.world.national_identity_get_nation_from_identity_holder(tag_target); holder) {
		ws.world.nation_get_available_cbs(holder).push_back(
				military::available_cb{trigger::to_nation(primary_slot), months > 0 ? ws.current_date + 31 * months : sys::date{}, type});
	}
	return 0;
}
uint32_t ef_add_casus_belli_int(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = ws.world.province_get_nation_from_province_ownership(trigger::payload(tval[3]).prov_id); holder) {
		ws.world.nation_get_available_cbs(holder).push_back(
				military::available_cb{trigger::to_nation(primary_slot), months > 0 ? ws.current_date + 31 * months : sys::date{}, type});
	}
	return 0;
}
uint32_t ef_add_casus_belli_this_nation(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	ws.world.nation_get_available_cbs(trigger::to_nation(this_slot))
			.push_back(military::available_cb{trigger::to_nation(primary_slot),
					months > 0 ? ws.current_date + 31 * months : sys::date{}, type});
	return 0;
}
uint32_t ef_add_casus_belli_this_state(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)); holder) {
		ws.world.nation_get_available_cbs(holder).push_back(
				military::available_cb{trigger::to_nation(primary_slot), months > 0 ? ws.current_date + 31 * months : sys::date{}, type});
	}
	return 0;
}
uint32_t ef_add_casus_belli_this_province(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)); holder) {
		ws.world.nation_get_available_cbs(holder).push_back(
				military::available_cb{trigger::to_nation(primary_slot), months > 0 ? ws.current_date + 31 * months : sys::date{}, type});
	}
	return 0;
}
uint32_t ef_add_casus_belli_this_pop(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = nations::owner_of_pop(ws, trigger::to_pop(this_slot)); holder) {
		ws.world.nation_get_available_cbs(holder).push_back(
				military::available_cb{trigger::to_nation(primary_slot), months > 0 ? ws.current_date + 31 * months : sys::date{}, type});
	}
	return 0;
}
uint32_t ef_add_casus_belli_from_nation(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	ws.world.nation_get_available_cbs(trigger::to_nation(from_slot))
			.push_back(military::available_cb{trigger::to_nation(primary_slot),
					months > 0 ? ws.current_date + 31 * months : sys::date{}, type});
	return 0;
}
uint32_t ef_add_casus_belli_from_province(EFFECT_PARAMTERS) {
	auto type = trigger::payload(tval[1]).cb_id;
	auto months = trigger::payload(tval[2]).signed_value;

	if(auto holder = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)); holder) {
		ws.world.nation_get_available_cbs(holder).push_back(
				military::available_cb{trigger::to_nation(primary_slot), months > 0 ? ws.current_date + 31 * months : sys::date{}, type});
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

	auto holder = ws.world.province_get_nation_from_province_ownership(trigger::payload(tval[1]).prov_id);
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
uint32_t ef_war_tag(EFFECT_PARAMTERS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(!target)
		return 0;

	auto war = military::create_war(ws, trigger::to_nation(primary_slot), target, trigger::payload(tval[2]).cb_id,
			ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[3]).prov_id),
			trigger::payload(tval[4]).tag_id,
			ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[4]).tag_id));
	if(trigger::payload(tval[5]).cb_id) {
		military::add_wargoal(ws, war, target, trigger::to_nation(primary_slot), trigger::payload(tval[5]).cb_id,
				ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[6]).prov_id),
				trigger::payload(tval[7]).tag_id,
				ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[7]).tag_id));
	}
	military::call_defender_allies(ws, war);
	military::call_attacker_allies(ws, war);
	return 0;
}
uint32_t ef_war_this_nation(EFFECT_PARAMTERS) {
	auto target = trigger::to_nation(this_slot);

	auto war = military::create_war(ws, trigger::to_nation(primary_slot), target, trigger::payload(tval[1]).cb_id,
			ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[2]).prov_id),
			trigger::payload(tval[3]).tag_id,
			ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[3]).tag_id));
	if(trigger::payload(tval[4]).cb_id) {
		military::add_wargoal(ws, war, target, trigger::to_nation(primary_slot), trigger::payload(tval[4]).cb_id,
				ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[5]).prov_id),
				trigger::payload(tval[6]).tag_id,
				ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[6]).tag_id));
	}
	military::call_defender_allies(ws, war);
	military::call_attacker_allies(ws, war);
	return 0;
}
uint32_t ef_war_this_state(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)); owner)
		return ef_war_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_lo, r_hi);
	return 0;
}
uint32_t ef_war_this_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)); owner)
		return ef_war_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_lo, r_hi);
	return 0;
}
uint32_t ef_war_this_pop(EFFECT_PARAMTERS) {
	if(auto owner = nations::owner_of_pop(ws, trigger::to_pop(this_slot)); owner)
		return ef_war_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_lo, r_hi);
	return 0;
}
uint32_t ef_war_from_nation(EFFECT_PARAMTERS) {
	return ef_war_this_nation(tval, ws, primary_slot, from_slot, 0, r_lo, r_hi);
}
uint32_t ef_war_from_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)); owner)
		return ef_war_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_lo, r_hi);
	return 0;
}
uint32_t ef_war_no_ally_tag(EFFECT_PARAMTERS) {
	auto target = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	if(!target)
		return 0;

	auto war = military::create_war(ws, trigger::to_nation(primary_slot), target, trigger::payload(tval[2]).cb_id,
			ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[3]).prov_id),
			trigger::payload(tval[4]).tag_id,
			ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[4]).tag_id));
	if(trigger::payload(tval[5]).cb_id) {
		military::add_wargoal(ws, war, target, trigger::to_nation(primary_slot), trigger::payload(tval[5]).cb_id,
				ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[6]).prov_id),
				trigger::payload(tval[7]).tag_id,
				ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[7]).tag_id));
	}
	military::call_defender_allies(ws, war);
	return 0;
}
uint32_t ef_war_no_ally_this_nation(EFFECT_PARAMTERS) {
	auto target = trigger::to_nation(this_slot);

	auto war = military::create_war(ws, trigger::to_nation(primary_slot), target, trigger::payload(tval[1]).cb_id,
			ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[2]).prov_id),
			trigger::payload(tval[3]).tag_id,
			ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[3]).tag_id));
	if(trigger::payload(tval[4]).cb_id) {
		military::add_wargoal(ws, war, target, trigger::to_nation(primary_slot), trigger::payload(tval[4]).cb_id,
				ws.world.province_get_state_from_abstract_state_membership(trigger::payload(tval[5]).prov_id),
				trigger::payload(tval[6]).tag_id,
				ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[6]).tag_id));
	}
	military::call_defender_allies(ws, war);
	return 0;
}
uint32_t ef_war_no_ally_this_state(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot)); owner)
		ef_war_no_ally_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_lo, r_hi);
	return 0;
}
uint32_t ef_war_no_ally_this_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot)); owner)
		ef_war_no_ally_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_lo, r_hi);
	return 0;
}
uint32_t ef_war_no_ally_this_pop(EFFECT_PARAMTERS) {
	if(auto owner = nations::owner_of_pop(ws, trigger::to_pop(this_slot)); owner)
		ef_war_no_ally_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_lo, r_hi);
	return 0;
}
uint32_t ef_war_no_ally_from_nation(EFFECT_PARAMTERS) {
	ef_war_no_ally_this_nation(tval, ws, primary_slot, from_slot, 0, r_lo, r_hi);
	return 0;
}
uint32_t ef_war_no_ally_from_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)); owner)
		ef_war_no_ally_this_nation(tval, ws, primary_slot, trigger::to_generic(owner), 0, r_lo, r_hi);
	return 0;
}
uint32_t ef_country_event_this_nation(EFFECT_PARAMTERS) {
	event::trigger_national_event(ws, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), r_lo + 1, r_hi, this_slot,
			event::slot_type::nation);
	return 0;
}
uint32_t ef_country_event_immediate_this_nation(EFFECT_PARAMTERS) {
	event::trigger_national_event(ws, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), r_lo + 1, r_hi, this_slot,
			event::slot_type::nation);
	return 0;
}
uint32_t ef_province_event_this_nation(EFFECT_PARAMTERS) {
	event::trigger_provincial_event(ws, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), r_lo + 1, r_hi, this_slot,
			event::slot_type::nation);
	return 0;
}
uint32_t ef_province_event_immediate_this_nation(EFFECT_PARAMTERS) {
	event::trigger_provincial_event(ws, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), r_lo + 1, r_hi, this_slot,
			event::slot_type::nation);
	return 0;
}
uint32_t ef_country_event_this_state(EFFECT_PARAMTERS) {
	event::trigger_national_event(ws, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), r_lo + 1, r_hi, this_slot,
			event::slot_type::state);
	return 0;
}
uint32_t ef_country_event_immediate_this_state(EFFECT_PARAMTERS) {
	event::trigger_national_event(ws, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), r_lo + 1, r_hi, this_slot,
			event::slot_type::state);
	return 0;
}
uint32_t ef_province_event_this_state(EFFECT_PARAMTERS) {
	event::trigger_provincial_event(ws, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), r_lo + 1, r_hi, this_slot,
			event::slot_type::state);
	return 0;
}
uint32_t ef_province_event_immediate_this_state(EFFECT_PARAMTERS) {
	event::trigger_provincial_event(ws, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), r_lo + 1, r_hi, this_slot,
			event::slot_type::state);
	return 0;
}
uint32_t ef_country_event_this_province(EFFECT_PARAMTERS) {
	event::trigger_national_event(ws, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), r_lo + 1, r_hi, this_slot,
			event::slot_type::province);
	return 0;
}
uint32_t ef_country_event_immediate_this_province(EFFECT_PARAMTERS) {
	event::trigger_national_event(ws, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), r_lo + 1, r_hi, this_slot,
			event::slot_type::province);
	return 0;
}
uint32_t ef_province_event_this_province(EFFECT_PARAMTERS) {
	event::trigger_provincial_event(ws, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), r_lo + 1, r_hi, this_slot,
			event::slot_type::province);
	return 0;
}
uint32_t ef_province_event_immediate_this_province(EFFECT_PARAMTERS) {
	event::trigger_provincial_event(ws, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), r_lo + 1, r_hi, this_slot,
			event::slot_type::province);
	return 0;
}
uint32_t ef_country_event_this_pop(EFFECT_PARAMTERS) {
	event::trigger_national_event(ws, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), r_lo + 1, r_hi, this_slot,
			event::slot_type::pop);
	return 0;
}
uint32_t ef_country_event_immediate_this_pop(EFFECT_PARAMTERS) {
	event::trigger_national_event(ws, trigger::payload(tval[1]).nev_id, trigger::to_nation(primary_slot), r_lo + 1, r_hi, this_slot,
			event::slot_type::pop);
	return 0;
}
uint32_t ef_province_event_this_pop(EFFECT_PARAMTERS) {
	event::trigger_provincial_event(ws, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), r_lo + 1, r_hi, this_slot,
			event::slot_type::pop);
	return 0;
}
uint32_t ef_province_event_immediate_this_pop(EFFECT_PARAMTERS) {
	event::trigger_provincial_event(ws, trigger::payload(tval[1]).pev_id, trigger::to_prov(primary_slot), r_lo + 1, r_hi, this_slot,
			event::slot_type::pop);
	return 0;
}
uint32_t ef_country_event_province_this_nation(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_country_event_this_nation(tval, ws, trigger::to_generic(owner), this_slot, 0, r_lo, r_hi);
	return 0;
}
uint32_t ef_country_event_immediate_province_this_nation(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_country_event_immediate_this_nation(tval, ws, trigger::to_generic(owner), this_slot, 0, r_lo, r_hi);
	return 0;
}
uint32_t ef_country_event_province_this_state(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_country_event_this_state(tval, ws, trigger::to_generic(owner), this_slot, 0, r_lo, r_hi);
	return 0;
}
uint32_t ef_country_event_immediate_province_this_state(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_country_event_immediate_this_state(tval, ws, trigger::to_generic(owner), this_slot, 0, r_lo, r_hi);
	return 0;
}
uint32_t ef_country_event_province_this_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_country_event_this_province(tval, ws, trigger::to_generic(owner), this_slot, 0, r_lo, r_hi);
	return 0;
}
uint32_t ef_country_event_immediate_province_this_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_country_event_immediate_this_province(tval, ws, trigger::to_generic(owner), this_slot, 0, r_lo, r_hi);
	return 0;
}
uint32_t ef_country_event_province_this_pop(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_country_event_this_pop(tval, ws, trigger::to_generic(owner), this_slot, 0, r_lo, r_hi);
	return 0;
}
uint32_t ef_country_event_immediate_province_this_pop(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_country_event_immediate_this_pop(tval, ws, trigger::to_generic(owner), this_slot, 0, r_lo, r_hi);
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

	auto& s = ws.world.pop_get_demographics(trigger::to_pop(primary_slot), pop_demographics::to_key(ws, i));
	s = std::max(0.0f, s + factor);

	for(auto j : ws.world.in_ideology) {
		ws.world.pop_get_demographics(trigger::to_pop(primary_slot), pop_demographics::to_key(ws, j)) /= (1.0f + factor);
	}

	return 0;
}
uint32_t ef_upper_house(EFFECT_PARAMTERS) {
	auto i = trigger::payload(tval[1]).ideo_id;
	auto amount = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(amount));

	auto& u = ws.world.nation_get_upper_house(trigger::to_nation(primary_slot), i);
	u = std::max(0.0f, u + amount);

	for(auto j : ws.world.in_ideology) {
		ws.world.nation_get_upper_house(trigger::to_nation(primary_slot), j) *= 100.0f / (100.0f + amount);
	}

	return 0;
}
uint32_t ef_scaled_militancy_issue(EFFECT_PARAMTERS) {
	auto issue_demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);

	auto support = ws.world.pop_get_demographics(trigger::to_pop(primary_slot), issue_demo_tag);
	float adjustment = trigger::read_float_from_payload(tval + 2) * float(support);
	assert(std::isfinite(adjustment));
	auto& v = ws.world.pop_get_militancy(trigger::to_pop(primary_slot));
	v = std::clamp(v + adjustment, 0.0f, 10.0f);

	return 0;
}
uint32_t ef_scaled_militancy_ideology(EFFECT_PARAMTERS) {
	auto ideology_demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).ideo_id);

	auto support = ws.world.pop_get_demographics(trigger::to_pop(primary_slot), ideology_demo_tag);
	float adjustment = trigger::read_float_from_payload(tval + 2) * float(support);
	assert(std::isfinite(adjustment));
	auto& v = ws.world.pop_get_militancy(trigger::to_pop(primary_slot));
	v = std::clamp(v + adjustment, 0.0f, 10.0f);

	return 0;
}
uint32_t ef_scaled_militancy_unemployment(EFFECT_PARAMTERS) {
	auto pop_size = ws.world.pop_get_size(trigger::to_pop(primary_slot));

	if(pop_size != 0) {
		auto unemployed = pop_size - ws.world.pop_get_employment(trigger::to_pop(primary_slot));
		float adjustment = trigger::read_float_from_payload(tval + 1) * float(unemployed) / float(pop_size);
		assert(std::isfinite(adjustment));
		auto& v = ws.world.pop_get_militancy(trigger::to_pop(primary_slot));
		v = std::clamp(v + adjustment, 0.0f, 10.0f);
	}

	return 0;
}
uint32_t ef_scaled_consciousness_issue(EFFECT_PARAMTERS) {
	auto issue_demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);

	auto support = ws.world.pop_get_demographics(trigger::to_pop(primary_slot), issue_demo_tag);
	float adjustment = trigger::read_float_from_payload(tval + 2) * float(support);
	assert(std::isfinite(adjustment));
	auto& v = ws.world.pop_get_consciousness(trigger::to_pop(primary_slot));
	v = std::clamp(v + adjustment, 0.0f, 10.0f);

	return 0;
}
uint32_t ef_scaled_consciousness_ideology(EFFECT_PARAMTERS) {
	auto ideology_demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).ideo_id);

	auto support = ws.world.pop_get_demographics(trigger::to_pop(primary_slot), ideology_demo_tag);
	float adjustment = trigger::read_float_from_payload(tval + 2) * float(support);
	assert(std::isfinite(adjustment));
	auto& v = ws.world.pop_get_consciousness(trigger::to_pop(primary_slot));
	v = std::clamp(v + adjustment, 0.0f, 10.0f);

	return 0;
}
uint32_t ef_scaled_consciousness_unemployment(EFFECT_PARAMTERS) {
	auto pop_size = ws.world.pop_get_size(trigger::to_pop(primary_slot));

	if(pop_size != 0) {
		auto unemployed = pop_size - ws.world.pop_get_employment(trigger::to_pop(primary_slot));
		float adjustment = trigger::read_float_from_payload(tval + 1) * float(unemployed) / float(pop_size);
		auto& v = ws.world.pop_get_consciousness(trigger::to_pop(primary_slot));
		v = std::clamp(v + adjustment, 0.0f, 10.0f);
	}

	return 0;
}
uint32_t ef_scaled_militancy_nation_issue(EFFECT_PARAMTERS) {
	auto demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);
	auto factor = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(factor));

	for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
		for(auto pop : p.get_province().get_pop_location()) {
			auto support = pop.get_pop().get_demographics(demo_tag);
			float adjustment = factor * support;
			auto& v = pop.get_pop().get_militancy();
			v = std::clamp(v + adjustment, 0.0f, 10.0f);
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
			auto support = pop.get_pop().get_demographics(demo_tag);
			float adjustment = factor * support;
			auto& v = pop.get_pop().get_militancy();
			v = std::clamp(v + adjustment, 0.0f, 10.0f);
		}
	}

	return 0;
}
uint32_t ef_scaled_militancy_nation_unemployment(EFFECT_PARAMTERS) {
	auto factor = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(factor));

	for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
		for(auto pop : p.get_province().get_pop_location()) {
			if(auto pop_size = pop.get_pop().get_size(); pop_size > 0) {
				auto unemployed = pop_size - pop.get_pop().get_employment();
				float adjustment = factor * unemployed / pop_size;
				auto& v = pop.get_pop().get_militancy();
				v = std::clamp(v + adjustment, 0.0f, 10.0f);
			}
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
			auto support = pop.get_pop().get_demographics(demo_tag);
			float adjustment = factor * support;
			auto& v = pop.get_pop().get_consciousness();
			v = std::clamp(v + adjustment, 0.0f, 10.0f);
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
			auto support = pop.get_pop().get_demographics(demo_tag);
			float adjustment = factor * support;
			auto& v = pop.get_pop().get_consciousness();
			v = std::clamp(v + adjustment, 0.0f, 10.0f);
		}
	}

	return 0;
}
uint32_t ef_scaled_consciousness_nation_unemployment(EFFECT_PARAMTERS) {
	auto factor = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(factor));

	for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
		for(auto pop : p.get_province().get_pop_location()) {
			if(auto pop_size = pop.get_pop().get_size(); pop_size > 0) {
				auto unemployed = pop_size - pop.get_pop().get_employment();
				float adjustment = factor * unemployed / pop_size;
				auto& v = pop.get_pop().get_consciousness();
				v = std::clamp(v + adjustment, 0.0f, 10.0f);
			}
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
			auto support = pop.get_pop().get_demographics(demo_tag);
			float adjustment = factor * support;
			auto& v = pop.get_pop().get_militancy();
			v = std::clamp(v + adjustment, 0.0f, 10.0f);
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
			auto support = pop.get_pop().get_demographics(demo_tag);
			float adjustment = factor * support;
			auto& v = pop.get_pop().get_militancy();
			v = std::clamp(v + adjustment, 0.0f, 10.0f);
		}
	});

	return 0;
}
uint32_t ef_scaled_militancy_state_unemployment(EFFECT_PARAMTERS) {
	auto factor = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(factor));

	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
		for(auto pop : ws.world.province_get_pop_location(p)) {
			if(auto pop_size = pop.get_pop().get_size(); pop_size > 0) {
				auto unemployed = pop_size - pop.get_pop().get_employment();
				float adjustment = factor * unemployed / pop_size;
				auto& v = pop.get_pop().get_militancy();
				v = std::clamp(v + adjustment, 0.0f, 10.0f);
			}
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
			auto support = pop.get_pop().get_demographics(demo_tag);
			float adjustment = factor * support;
			auto& v = pop.get_pop().get_consciousness();
			v = std::clamp(v + adjustment, 0.0f, 10.0f);
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
			auto support = pop.get_pop().get_demographics(demo_tag);
			float adjustment = factor * support;
			auto& v = pop.get_pop().get_consciousness();
			v = std::clamp(v + adjustment, 0.0f, 10.0f);
		}
	});

	return 0;
}
uint32_t ef_scaled_consciousness_state_unemployment(EFFECT_PARAMTERS) {
	auto factor = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(factor));

	province::for_each_province_in_state_instance(ws, trigger::to_state(primary_slot), [&](dcon::province_id p) {
		for(auto pop : ws.world.province_get_pop_location(p)) {
			if(auto pop_size = pop.get_pop().get_size(); pop_size > 0) {
				auto unemployed = pop_size - pop.get_pop().get_employment();
				float adjustment = factor * unemployed / pop_size;
				auto& v = pop.get_pop().get_consciousness();
				v = std::clamp(v + adjustment, 0.0f, 10.0f);
			}
		}
	});

	return 0;
}
uint32_t ef_scaled_militancy_province_issue(EFFECT_PARAMTERS) {
	auto demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);
	auto factor = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(factor));

	for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
		auto support = pop.get_pop().get_demographics(demo_tag);
		float adjustment = factor * support;
		auto& v = pop.get_pop().get_militancy();
		v = std::clamp(v + adjustment, 0.0f, 10.0f);
	}

	return 0;
}
uint32_t ef_scaled_militancy_province_ideology(EFFECT_PARAMTERS) {
	auto demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).ideo_id);
	auto factor = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(factor));

	for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
		auto support = pop.get_pop().get_demographics(demo_tag);
		float adjustment = factor * support;
		auto& v = pop.get_pop().get_militancy();
		v = std::clamp(v + adjustment, 0.0f, 10.0f);
	}

	return 0;
}
uint32_t ef_scaled_militancy_province_unemployment(EFFECT_PARAMTERS) {
	auto factor = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(factor));

	for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
		if(auto pop_size = pop.get_pop().get_size(); pop_size > 0) {
			auto unemployed = pop_size - pop.get_pop().get_employment();
			float adjustment = factor * unemployed / pop_size;
			auto& v = pop.get_pop().get_militancy();
			v = std::clamp(v + adjustment, 0.0f, 10.0f);
		}
	}

	return 0;
}
uint32_t ef_scaled_consciousness_province_issue(EFFECT_PARAMTERS) {
	auto demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);
	auto factor = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(factor));

	for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
		auto support = pop.get_pop().get_demographics(demo_tag);
		float adjustment = factor * support;
		auto& v = pop.get_pop().get_consciousness();
		v = std::clamp(v + adjustment, 0.0f, 10.0f);
	}

	return 0;
}
uint32_t ef_scaled_consciousness_province_ideology(EFFECT_PARAMTERS) {
	auto demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).ideo_id);
	auto factor = trigger::read_float_from_payload(tval + 2);
	assert(std::isfinite(factor));

	for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
		auto support = pop.get_pop().get_demographics(demo_tag);
		float adjustment = factor * support;
		auto& v = pop.get_pop().get_consciousness();
		v = std::clamp(v + adjustment, 0.0f, 10.0f);
	}

	return 0;
}
uint32_t ef_scaled_consciousness_province_unemployment(EFFECT_PARAMTERS) {
	auto factor = trigger::read_float_from_payload(tval + 1);
	assert(std::isfinite(factor));

	for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
		if(auto pop_size = pop.get_pop().get_size(); pop_size > 0) {
			auto unemployed = pop_size - pop.get_pop().get_employment();
			float adjustment = factor * unemployed / pop_size;
			auto& v = pop.get_pop().get_consciousness();
			v = std::clamp(v + adjustment, 0.0f, 10.0f);
		}
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
uint32_t ef_define_general(EFFECT_PARAMTERS) {
	auto l = fatten(ws.world, ws.world.create_leader());
	l.set_since(ws.current_date);
	l.set_is_admiral(false);
	l.set_background(trigger::payload(tval[3]).lead_id);
	l.set_personality(trigger::payload(tval[2]).lead_id);
	l.set_name(trigger::payload(tval[1]).unam_id);
	l.set_nation_from_leader_loyalty(trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_define_admiral(EFFECT_PARAMTERS) {
	auto l = fatten(ws.world, ws.world.create_leader());
	l.set_since(ws.current_date);
	l.set_is_admiral(true);
	l.set_background(trigger::payload(tval[3]).lead_id);
	l.set_personality(trigger::payload(tval[2]).lead_id);
	l.set_name(trigger::payload(tval[1]).unam_id);
	l.set_nation_from_leader_loyalty(trigger::to_nation(primary_slot));
	return 0;
}
uint32_t ef_dominant_issue(EFFECT_PARAMTERS) {
	auto t = trigger::payload(tval[1]).opt_id;
	auto factor = trigger::read_float_from_payload(tval + 2);

	auto& s = ws.world.pop_get_demographics(trigger::to_pop(primary_slot), pop_demographics::to_key(ws, t));
	s = std::max(0.0f, s + factor);

	for(auto i : ws.world.in_issue_option) {
		ws.world.pop_get_demographics(trigger::to_pop(primary_slot), pop_demographics::to_key(ws, i)) /= (1.0f + factor);
	}

	return 0;
}
uint32_t ef_dominant_issue_nation(EFFECT_PARAMTERS) {
	auto demo_tag = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);
	auto factor = trigger::read_float_from_payload(tval + 2);

	for(auto p : ws.world.nation_get_province_ownership(trigger::to_nation(primary_slot))) {
		for(auto pop : p.get_province().get_pop_location()) {
			auto& s = pop.get_pop().get_demographics(demo_tag);
			s = std::max(0.0f, s + factor);

			for(auto i : ws.world.in_issue_option) {
				pop.get_pop().get_demographics(pop_demographics::to_key(ws, i)) /= (1.0f + factor);
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
			auto& s = pop.get_pop().get_demographics(from_issue);
			auto adjust = s * amount;
			s -= adjust;
			pop.get_pop().get_demographics(to_issue) += adjust;
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
			auto& s = pop.get_pop().get_demographics(from_issue);
			auto adjust = s * amount;
			s -= adjust;
			pop.get_pop().get_demographics(to_issue) += adjust;
		}
	});

	return 0;
}
uint32_t ef_move_issue_percentage_province(EFFECT_PARAMTERS) {
	auto from_issue = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);
	auto to_issue = pop_demographics::to_key(ws, trigger::payload(tval[2]).opt_id);
	auto amount = trigger::read_float_from_payload(tval + 3);

	for(auto pop : ws.world.province_get_pop_location(trigger::to_prov(primary_slot))) {
		auto& s = pop.get_pop().get_demographics(from_issue);
		auto adjust = s * amount;
		s -= adjust;
		pop.get_pop().get_demographics(to_issue) += adjust;
	}

	return 0;
}
uint32_t ef_move_issue_percentage_pop(EFFECT_PARAMTERS) {
	auto from_issue = pop_demographics::to_key(ws, trigger::payload(tval[1]).opt_id);
	auto to_issue = pop_demographics::to_key(ws, trigger::payload(tval[2]).opt_id);
	auto amount = trigger::read_float_from_payload(tval + 3);

	auto& s = ws.world.pop_get_demographics(trigger::to_pop(primary_slot), from_issue);
	auto adjust = s * amount;
	s -= adjust;
	ws.world.pop_get_demographics(trigger::to_pop(primary_slot), to_issue) += adjust;

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
			ws.world.province_get_railroad_level(p) += uint8_t(1);
	});
	return 0;
}
uint32_t ef_build_railway_in_capital_yes_whole_state_no_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	auto cs = ws.world.province_get_state_membership(c);
	province::for_each_province_in_state_instance(ws, cs, [&](dcon::province_id p) {
		if(ws.world.province_get_modifier_values(p, sys::provincial_mod_offsets::min_build_railroad) <= 1.0f)
			ws.world.province_get_railroad_level(p) += uint8_t(1);
	});
	return 0;
}
uint32_t ef_build_railway_in_capital_no_whole_state_yes_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	if(c) {
		if(ws.world.province_get_modifier_values(c, sys::provincial_mod_offsets::min_build_railroad) <= 1.0f)
			ws.world.province_get_railroad_level(c) += uint8_t(1);
	}
	return 0;
}
uint32_t ef_build_railway_in_capital_no_whole_state_no_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	if(c) {
		if(ws.world.province_get_modifier_values(c, sys::provincial_mod_offsets::min_build_railroad) <= 1.0f)
			ws.world.province_get_railroad_level(c) += uint8_t(1);
	}
	return 0;
}
uint32_t ef_build_fort_in_capital_yes_whole_state_yes_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	auto cs = ws.world.province_get_state_membership(c);
	province::for_each_province_in_state_instance(ws, cs,
			[&](dcon::province_id p) { ws.world.province_get_fort_level(p) += uint8_t(1); });
	return 0;
}
uint32_t ef_build_fort_in_capital_yes_whole_state_no_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	auto cs = ws.world.province_get_state_membership(c);
	province::for_each_province_in_state_instance(ws, cs,
			[&](dcon::province_id p) { ws.world.province_get_fort_level(p) += uint8_t(1); });
	return 0;
}
uint32_t ef_build_fort_in_capital_no_whole_state_yes_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	if(c) {
		ws.world.province_get_fort_level(c) += uint8_t(1);
	}
	return 0;
}
uint32_t ef_build_fort_in_capital_no_whole_state_no_limit(EFFECT_PARAMTERS) {
	auto c = ws.world.nation_get_capital(trigger::to_nation(primary_slot));
	if(c) {
		ws.world.province_get_fort_level(c) += uint8_t(1);
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
		return ef_set_country_flag(tval, ws, trigger::to_generic(owner), 0, 0, 0, 0);
	return 0;
}
uint32_t ef_add_country_modifier_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_add_country_modifier(tval, ws, trigger::to_generic(owner), 0, 0, 0, 0);
	return 0;
}
uint32_t ef_add_country_modifier_province_no_duration(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_add_country_modifier_no_duration(tval, ws, trigger::to_generic(owner), 0, 0, 0, 0);
	return 0;
}
uint32_t ef_relation_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_relation(tval, ws, trigger::to_generic(owner), 0, 0, 0, 0);
	return 0;
}
uint32_t ef_relation_province_this_nation(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_relation_this_nation(tval, ws, trigger::to_generic(owner), this_slot, 0, 0, 0);
	return 0;
}
uint32_t ef_relation_province_this_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_relation_this_province(tval, ws, trigger::to_generic(owner), this_slot, 0, 0, 0);
	return 0;
}
uint32_t ef_relation_province_from_nation(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_relation_this_nation(tval, ws, trigger::to_generic(owner), from_slot, 0, 0, 0);
	return 0;
}
uint32_t ef_relation_province_from_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_relation_this_province(tval, ws, trigger::to_generic(owner), from_slot, 0, 0, 0);
	return 0;
}
uint32_t ef_relation_province_reb(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_relation_reb(tval, ws, trigger::to_generic(owner), 0, from_slot, 0, 0);
	return 0;
}
uint32_t ef_treasury_province(EFFECT_PARAMTERS) {
	if(auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(primary_slot)); owner)
		return ef_treasury(tval, ws, trigger::to_generic(owner), 0, 0, 0, 0);
	return 0;
}

inline constexpr uint32_t (*effect_functions[])(EFFECT_PARAMTERS) = {
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
		es_tag_scope,												// constexpr inline uint16_t tag_scope = first_scope_code + 0x0031;
		es_integer_scope,										// constexpr inline uint16_t integer_scope = first_scope_code + 0x0032;
		es_pop_type_scope_nation,						// constexpr inline uint16_t pop_type_scope_nation = first_scope_code + 0x0033;
		es_pop_type_scope_state,						// constexpr inline uint16_t pop_type_scope_state = first_scope_code + 0x0034;
		es_pop_type_scope_province,					// constexpr inline uint16_t pop_type_scope_province = first_scope_code + 0x0035;
		es_region_scope,										// constexpr inline uint16_t region_scope = first_scope_code + 0x0036;
};

uint32_t internal_execute_effect(EFFECT_PARAMTERS) {
	return effect_functions[*tval & effect::code_mask](tval, ws, primary_slot, this_slot, from_slot, r_lo, r_hi);
}

void execute(sys::state& state, dcon::effect_key key, int32_t primary, int32_t this_slot, int32_t from_slot, uint32_t r_lo,
		uint32_t r_hi) {
	internal_execute_effect(state.effect_data.data() + key.index(), state, primary, this_slot, from_slot, r_lo, r_hi);
}

void execute(sys::state& state, uint16_t const* data, int32_t primary, int32_t this_slot, int32_t from_slot, uint32_t r_lo,
		uint32_t r_hi) {
	internal_execute_effect(data, state, primary, this_slot, from_slot, r_lo, r_hi);
}

} // namespace effect
