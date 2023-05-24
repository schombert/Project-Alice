
#include "politics.hpp"
#include "dcon_generated.hpp"
#include "demographics.hpp"
#include "triggers.hpp"

namespace politics {

float vote_total(sys::state& state, dcon::nation_id nation) {
    float total = 0.f;
    state.world.for_each_province([&](dcon::province_id province) {
        if(nation == state.world.province_get_nation_from_province_ownership(province)) {
            for(auto pop_loc : state.world.province_get_pop_location(province)) {
                auto pop_id = pop_loc.get_pop();
                total += get_weighted_vote_size(state, nation, pop_id.id);
            }
        }
    });
    return total;
}

float get_weighted_vote_size(sys::state& state, dcon::nation_id nation, dcon::pop_id pop) {
    // TODO
    return 1.f * state.world.pop_get_size(pop);
}

float get_popular_support(sys::state& state, dcon::nation_id nation, dcon::issue_option_id issue_option) {
    auto total = state.world.nation_get_demographics(nation, demographics::total);
    if(total <= 0.f) {
        return 0.f;
    }
    auto dkey = demographics::to_key(state, issue_option);
    return state.world.nation_get_demographics(nation, dkey) / total;
}

float get_voter_support(sys::state& state, dcon::nation_id nation, dcon::issue_option_id issue_option) {
    auto total = vote_total(state, nation);
    if(total <= 0.f) {
        return 0.f;
    }
    auto support = 0.f;
    state.world.for_each_province([&](dcon::province_id province) {
        if(nation == state.world.province_get_nation_from_province_ownership(province)) {
            for(auto pop_loc : state.world.province_get_pop_location(province)) {
                auto pop_id = pop_loc.get_pop();
                auto vote_size = get_weighted_vote_size(state, nation, pop_id.id);
                auto dkey = pop_demographics::to_key(state, issue_option);
                support += state.world.pop_get_demographics(pop_id.id, dkey);
            }
        }
    });
    return support / total;
}

bool can_appoint_ruling_party(sys::state& state, dcon::nation_id nation) {
    auto fat_id = dcon::fatten(state.world, nation);
    auto gov_type_id = fat_id.get_government_type();
    return state.culture_definitions.governments[gov_type_id].can_appoint_ruling_party;
}

bool is_election_ongoing(sys::state& state, dcon::nation_id nation) {
    auto election_end_date = dcon::fatten(state.world, nation).get_election_ends();
    return election_end_date && election_end_date > state.current_date;
}

bool has_elections(sys::state& state, dcon::nation_id nation) {
    auto fat_id = dcon::fatten(state.world, nation);
    auto gov_type_id = fat_id.get_government_type();
    return state.culture_definitions.governments[gov_type_id].has_elections;
}

sys::date next_election_date(sys::state& state, dcon::nation_id nation) {
    auto end_date = state.world.nation_get_election_ends(nation);
    return end_date + 365 * 5;
}

dcon::reform_id get_reform_by_name(sys::state& state, std::string_view name) {
    dcon::reform_id result{};
    auto it = state.key_to_text_sequence.find(name);
    if(it != state.key_to_text_sequence.end()) {
        state.world.for_each_reform([&](dcon::reform_id reform_id) {
            auto key = state.world.reform_get_name(reform_id);
            if(it->second == key) {
                result = reform_id;
            }
        });
    }
    return result;
}

dcon::issue_id get_issue_by_name(sys::state& state, std::string_view name) {
    dcon::issue_id result{};
    auto it = state.key_to_text_sequence.find(name);
    if(it != state.key_to_text_sequence.end()) {
        state.world.for_each_issue([&](dcon::issue_id issue_id) {
            auto key = state.world.issue_get_name(issue_id);
            if(it->second == key) {
                result = issue_id;
            }
        });
    }
    return result;
}

bool reform_is_selected(sys::state& state, dcon::nation_id nation, dcon::reform_option_id reform_option) {
    auto reform = state.world.reform_option_get_parent_reform(reform_option);
    return reform && reform_option == state.world.nation_get_reforms(nation, reform);
}

bool issue_is_selected(sys::state& state, dcon::nation_id nation, dcon::issue_option_id issue_option) {
    auto issue = state.world.issue_option_get_parent_issue(issue_option);
    return issue && issue_option == state.world.nation_get_issues(nation, issue);
}

bool can_enact_political_reform(sys::state& state, dcon::nation_id nation, dcon::issue_option_id issue_option) {
	if(!issue_option)
		return false;

    auto issue = state.world.issue_option_get_parent_issue(issue_option);
    auto current = state.world.nation_get_issues(nation, issue.id).id;
    auto allow = state.world.issue_option_get_allow(issue_option);
    if(
        (!state.world.issue_get_is_next_step_only(issue.id) || current.index() + 1 == issue_option.index() || current.index() - 1 == issue_option.index())
        &&
        (!allow || trigger::evaluate(state, allow, trigger::to_generic(nation), trigger::to_generic(nation), 0))
        ) {

        float total = 0.0f;
        for(uint32_t icounter = state.world.ideology_size(); icounter-- > 0;) {
            dcon::ideology_id iid{ dcon::ideology_id::value_base_t(icounter) };
            auto condition = issue_option.index() > current.index() ? state.world.ideology_get_add_political_reform(iid) : state.world.ideology_get_remove_political_reform(iid);
            auto upperhouse_weight = 0.01f * state.world.nation_get_upper_house(nation, iid);
            if(condition && upperhouse_weight > 0.0f)
                total += upperhouse_weight * trigger::evaluate_additive_modifier(state, condition, trigger::to_generic(nation), trigger::to_generic(nation), 0);
            if(total > 0.5f)
                return true;
        }
    }
    return false;
}

bool can_enact_social_reform(sys::state& state, dcon::nation_id n, dcon::issue_option_id o) {
	if(!o)
		return false;

    auto issue = state.world.issue_option_get_parent_issue(o);
    auto current = state.world.nation_get_issues(n, issue.id).id;
    auto allow = state.world.issue_option_get_allow(o);
    if(
        (!state.world.issue_get_is_next_step_only(issue.id) || current.index() + 1 == o.index() || current.index() - 1 == o.index())
        &&
        (!allow || trigger::evaluate(state, allow, trigger::to_generic(n), trigger::to_generic(n), 0))
        ) {

        float total = 0.0f;
        for(uint32_t icounter = state.world.ideology_size(); icounter-- > 0;) {
            dcon::ideology_id iid{ dcon::ideology_id::value_base_t(icounter) };
            auto condition = o.index() > current.index() ? state.world.ideology_get_add_social_reform(iid) : state.world.ideology_get_remove_social_reform(iid);
            auto upperhouse_weight = 0.01f * state.world.nation_get_upper_house(n, iid);
            if(condition && upperhouse_weight > 0.0f)
                total += upperhouse_weight * trigger::evaluate_additive_modifier(state, condition, trigger::to_generic(n), trigger::to_generic(n), 0);
            if(total > 0.5f)
                return true;
        }
    }
    return false;
}

bool can_enact_military_reform(sys::state& state, dcon::nation_id n, dcon::reform_option_id o) {
	if(!o)
		return false;

    auto reform = state.world.reform_option_get_parent_reform(o);
    auto current = state.world.nation_get_reforms(n, reform.id).id;
    auto allow = state.world.reform_option_get_allow(o);
    auto stored_rp = state.world.nation_get_research_points(n);
    if(
        o.index() > current.index()
        &&
        (!state.world.reform_get_is_next_step_only(reform.id) || current.index() + 1 == o.index())
        &&
        (!allow || trigger::evaluate(state, allow, trigger::to_generic(n), trigger::to_generic(n), 0))
        ) {

        float base_cost = float(state.world.reform_option_get_technology_cost(o));
        float reform_factor = politics::get_military_reform_multiplier(state, n);

        if(base_cost * reform_factor < stored_rp)
            return true;
    }
    return false;
}

bool can_enact_economic_reform(sys::state& state, dcon::nation_id n, dcon::reform_option_id o) {
	if(!o)
		return false;

    auto reform = state.world.reform_option_get_parent_reform(o);
    auto current = state.world.nation_get_reforms(n, reform.id).id;
    auto allow = state.world.reform_option_get_allow(o);
    auto stored_rp = state.world.nation_get_research_points(n);
    if(
        o.index() > current.index()
        &&
        (!state.world.reform_get_is_next_step_only(reform.id) || current.index() + 1 == o.index())
        &&
        (!allow || trigger::evaluate(state, allow, trigger::to_generic(n), trigger::to_generic(n), 0))
        ) {

        float base_cost = float(state.world.reform_option_get_technology_cost(o));
        float reform_factor = politics::get_economic_reform_multiplier(state, n);

        if(base_cost * reform_factor < stored_rp)
            return true;
    }
    return false;
}

float get_military_reform_multiplier(sys::state& state, dcon::nation_id n) {
    float reform_factor =
        1.0f
        + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::self_unciv_military_modifier)
        + state.world.nation_get_modifier_values(state.world.nation_get_in_sphere_of(n), sys::national_mod_offsets::unciv_military_modifier);

    for(uint32_t icounter = state.world.ideology_size(); icounter-- > 0;) {
        dcon::ideology_id iid{ dcon::ideology_id::value_base_t(icounter) };
        auto condition = state.world.ideology_get_add_military_reform(iid);
        auto upperhouse_weight = 0.01f * state.world.nation_get_upper_house(n, iid);
        if(condition && upperhouse_weight > 0.0f)
            reform_factor += state.defines.military_reform_uh_factor * upperhouse_weight * trigger::evaluate_additive_modifier(state, condition, trigger::to_generic(n), trigger::to_generic(n), 0);
    }
    return reform_factor;
}

float get_economic_reform_multiplier(sys::state& state, dcon::nation_id n) {
    float reform_factor =
        1.0f
        + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::self_unciv_economic_modifier)
        + state.world.nation_get_modifier_values(state.world.nation_get_in_sphere_of(n), sys::national_mod_offsets::unciv_economic_modifier);

    for(uint32_t icounter = state.world.ideology_size(); icounter-- > 0;) {
        dcon::ideology_id iid{ dcon::ideology_id::value_base_t(icounter) };
        auto condition = state.world.ideology_get_add_economic_reform(iid);
        auto upperhouse_weight = 0.01f * state.world.nation_get_upper_house(n, iid);
        if(condition && upperhouse_weight > 0.0f)
            reform_factor += state.defines.economic_reform_uh_factor * upperhouse_weight * trigger::evaluate_additive_modifier(state, condition, trigger::to_generic(n), trigger::to_generic(n), 0);
    }
    return reform_factor;
}

bool political_party_is_active(sys::state& state, dcon::political_party_id p) {
	auto start_date = state.world.political_party_get_start_date(p);
	auto end_date = state.world.political_party_get_end_date(p);
	return (!start_date || start_date <= state.current_date) && (!end_date || end_date > state.current_date);
}

void set_ruling_party(sys::state& state, dcon::nation_id n, dcon::political_party_id p) {
	state.world.nation_set_ruling_party(n, p);
	for(auto pi : state.culture_definitions.party_issues) {
		state.world.nation_set_issues(n, pi, state.world.political_party_get_party_issues(p, pi));
	}
	culture::update_nation_issue_rules(state, n);
	sys::update_single_nation_modifiers(state, n);
	economy::bound_budget_settings(state, n);
}

void force_ruling_party_ideology(sys::state& state, dcon::nation_id n, dcon::ideology_id id) {
	auto tag = state.world.nation_get_identity_from_identity_holder(n);
	auto start = state.world.national_identity_get_political_party_first(tag).id.index();
	auto end = start + state.world.national_identity_get_political_party_count(tag);

	for(int32_t i = start; i < end; i++) {
		auto pid = dcon::political_party_id(dcon::political_party_id::value_base_t(i));
		if(politics::political_party_is_active(state, pid) && state.world.political_party_get_ideology(pid) == id) {
			set_ruling_party(state, n, pid);
			return;
		}
	}
}

void force_nation_ideology(sys::state& state, dcon::nation_id n, dcon::ideology_id id) {
	state.world.for_each_ideology([&](auto iid) {
		state.world.nation_set_upper_house(n, iid, 0.0f);
	});
	state.world.nation_set_upper_house(n, id, 100.0f);

	force_ruling_party_ideology(state, n, id);
}

void update_displayed_identity(sys::state& state, dcon::nation_id id) {
	auto ident = state.world.nation_get_identity_from_identity_holder(id);
	state.world.nation_set_name(id, state.world.national_identity_get_name(ident));
	state.world.nation_set_adjective(id, state.world.national_identity_get_adjective(ident));
	state.world.nation_set_color(id, state.world.national_identity_get_color(ident));
}


void change_government_type(sys::state& state, dcon::nation_id n, dcon::government_type_id new_type) {
	auto old_gov = state.world.nation_get_government_type(n);
	if(old_gov != new_type) {
		state.world.nation_set_government_type(n, new_type);
		recalculate_upper_house(state, n);
		// TODO: notify player ?
		update_displayed_identity(state, n);
	}
}

float pop_vote_weight(sys::state& state, dcon::pop_id p, dcon::nation_id n) {
	/*
	When a pop's "votes" in any form, the weight of that vote is the product of the size of the pop and the national modifier for voting for their strata (this could easily result in a strata having no votes). If the nation has primary culture voting set then primary culture pops get a full vote, accepted culture pops get a half vote, and other culture pops get no vote. If it has culture voting, primary and accepted culture pops get a full vote and no one else gets a vote. If neither is set, all pops get an equal vote.
	*/
	
	auto type = state.world.pop_get_poptype(p);
	if(state.world.pop_type_get_voting_forbidden(type))
		return 0.0f;

	auto size = state.world.pop_get_size(p);
	auto strata = culture::pop_strata(state.world.pop_type_get_strata(type));
	auto vmod = [&]() {
		switch(strata) {
			case culture::pop_strata::poor:
				return state.world.nation_get_modifier_values(n, sys::national_mod_offsets::poor_vote);
			case culture::pop_strata::middle:
				return state.world.nation_get_modifier_values(n, sys::national_mod_offsets::middle_vote);
			case culture::pop_strata::rich:
				return state.world.nation_get_modifier_values(n, sys::national_mod_offsets::rich_vote);
			default:
				return 0.0f;
		}
	}();

	auto rules = state.world.nation_get_combined_issue_rules(n);
	if((rules & issue_rule::primary_culture_voting) != 0) {
		if(state.world.pop_get_culture(p) == state.world.nation_get_primary_culture(n))
			return size * vmod;
		else if(state.world.pop_get_is_primary_or_accepted_culture(p))
			return size * vmod * 0.5f;
		else
			return 0.0f;
	} else if((rules & issue_rule::culture_voting) != 0) {
		if(state.world.pop_get_is_primary_or_accepted_culture(p))
			return size * vmod;
		else
			return 0.0f;
	} else {
		return size * vmod;
	}
}

void recalculate_upper_house(sys::state& state, dcon::nation_id n) {
	/*
	Every year, the upper house of each nation is updated. If the "same as ruling party" rule is set, the upper house becomes 100% the ideology of the ruling party. If the rule is "state vote", then for each non-colonial state: for each pop in the state that is not prevented from voting by its type we distribute its weighted vote proportionally to its ideology support, giving us an ideology distribution for each of those states. The state ideology distributions are then normalized and summed to form the distribution for the upper house. For "population_vote" and "rich_only" the voting weight of each non colonial pop (or just the rich ones for "rich only") is distributed proportionally to its ideological support, with the sum for all eligible pops forming the distribution for the upper house.
	*/
	static std::vector<float> accumulated_in_state;
	accumulated_in_state.resize(state.world.ideology_size());

	auto rules = state.world.nation_get_combined_issue_rules(n);
	if((rules & issue_rule::same_as_ruling_party) != 0) {
		auto rp_ideology = state.world.political_party_get_ideology(state.world.nation_get_ruling_party(n));
		for(auto i : state.world.in_ideology) {
			state.world.nation_set_upper_house(n, i, 0.0f);
		}
		state.world.nation_set_upper_house(n, rp_ideology, 100.0f);
	} else if((rules & issue_rule::state_vote) != 0) {
		for(auto i : state.world.in_ideology) {
			state.world.nation_set_upper_house(n, i, 0.0f);
		}
		float state_total = 0.0f;
		
		for(auto si : state.world.nation_get_state_ownership(n)) {
			if(si.get_state().get_capital().get_is_colonial())
				continue; // skip colonial states

			for(auto i : state.world.in_ideology) {
				accumulated_in_state[i.id.index()] = 0.0f;
			}
			float total = 0.0f;
			province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id p) {
				for(auto pop : state.world.province_get_pop_location(p)) {
					auto weight = pop_vote_weight(state, pop.get_pop(), n);
					if(weight > 0) {
						total += weight;
						for(auto i : state.world.in_ideology) {
							accumulated_in_state[i.id.index()] += weight * state.world.pop_get_demographics(pop.get_pop(), pop_demographics::to_key(state, i));
						}
					}
				}
			});
			if(total > 0) {
				for(auto i : state.world.in_ideology) {
					auto scaled = accumulated_in_state[i.id.index()] / total;
					state_total += scaled;
					state.world.nation_get_upper_house(n, i) += scaled;
				}
			}
		}

		if(state_total > 0) {
			auto scale_factor = 100.0f / state_total;
			for(auto i : state.world.in_ideology) {
				state.world.nation_get_upper_house(n, i) *= scale_factor;
			}
		}
	} else if((rules & issue_rule::rich_only) != 0) {
		for(auto i : state.world.in_ideology) {
			state.world.nation_set_upper_house(n, i, 0.0f);
		}
		float total = 0.0f;
		for(auto p : state.world.nation_get_province_ownership(n)) {
			if(p.get_province().get_is_colonial())
				continue; // skip colonial provinces

			for(auto pop : state.world.province_get_pop_location(p.get_province())) {
				if(pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::rich)) {
					auto weight = pop_vote_weight(state, pop.get_pop(), n);
					if(weight > 0) {
						total += weight;
						for(auto i : state.world.in_ideology) {
							state.world.nation_get_upper_house(n, i) += weight * state.world.pop_get_demographics(pop.get_pop(), pop_demographics::to_key(state, i));
						}
					}
				}
			}
		}
		if(total > 0) {
			auto scale_factor = 100.0f / total;
			for(auto i : state.world.in_ideology) {
				state.world.nation_get_upper_house(n, i) *= scale_factor;
			}
		}
	} else {
		for(auto i : state.world.in_ideology) {
			state.world.nation_set_upper_house(n, i, 0.0f);
		}
		float total = 0.0f;
		for(auto p : state.world.nation_get_province_ownership(n)) {
			if(p.get_province().get_is_colonial())
				continue; // skip colonial provinces

			for(auto pop : state.world.province_get_pop_location(p.get_province())) {
				auto weight = pop_vote_weight(state, pop.get_pop(), n);
				if(weight > 0) {
					total += weight;
					for(auto i : state.world.in_ideology) {
						state.world.nation_get_upper_house(n, i) += weight * state.world.pop_get_demographics(pop.get_pop(), pop_demographics::to_key(state, i));
					}
				}
			}
		}
		if(total > 0) {
			auto scale_factor = 100.0f / total;
			for(auto i : state.world.in_ideology) {
				state.world.nation_get_upper_house(n, i) *= scale_factor;
			}
		}
	}

	// TODO: notify player
}

void daily_party_loyalty_update(sys::state& state) {
	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto si = state.world.province_get_state_membership(p);
		auto nf = state.world.state_instance_get_owner_focus(si);
		auto party = state.world.national_focus_get_ideology(nf);
		if(party) {
			auto& l = state.world.province_get_party_loyalty(p, party);
			l = std::clamp(l + state.world.national_focus_get_loyalty_value(nf), -1.0f, 1.0f);
		}
	});
}

float party_total_support(sys::state& state, dcon::pop_id pop, dcon::political_party_id par_id, dcon::nation_id nat_id, dcon::province_id prov_id) {
	auto n = dcon::fatten(state.world, nat_id);
	auto p = dcon::fatten(state.world, prov_id);

	auto weight = pop_vote_weight(state, pop, n);
	if(weight > 0.f) {
		auto ideological_share = state.world.pop_get_consciousness(pop) / 20.0f;

		float ruling_party_support = p.get_modifier_values(sys::provincial_mod_offsets::local_ruling_party_support) + n.get_modifier_values(sys::national_mod_offsets::ruling_party_support) + 1.0f;
		float prov_vote_mod = p.get_modifier_values(sys::provincial_mod_offsets::number_of_voters) + 1.0f;

		auto pid = state.world.political_party_get_ideology(par_id);
		auto base_support = (p.get_party_loyalty(pid) + 1.0f) * prov_vote_mod * (par_id == n.get_ruling_party() ? ruling_party_support : 1.0f) * weight;
		auto issue_support = 0.0f;
		for(auto pi : state.culture_definitions.party_issues) {
			auto party_pos = state.world.political_party_get_party_issues(par_id, pi);
			issue_support += state.world.pop_get_demographics(pop, pop_demographics::to_key(state, party_pos));
		}
		auto ideology_support = state.world.pop_get_demographics(pop, pop_demographics::to_key(state, pid));
		return base_support * (issue_support * (1.0f - ideological_share) + ideology_support * ideological_share);
	} else {
		return 0.f;
	}
}

struct party_vote {
	dcon::political_party_id par;
	float vote = 0.0f;
};

void start_election(sys::state& state, dcon::nation_id n) {
	if(state.world.nation_get_election_ends(n) < state.current_date) {
		state.world.nation_set_election_ends(n, state.current_date + int32_t(state.defines.campaign_duration) * 30);
		// TODO: Notify player
	}
}

void update_elections(sys::state& state) {
	static std::vector<party_vote> party_votes;
	static std::vector<party_vote> provincial_party_votes;

	for(auto n : state.world.in_nation) {
		/*
		A country with elections starts one every 5 years.
		Elections last define:CAMPAIGN_DURATION months.
		*/
		if(has_elections(state, n)) {
			if(n.get_election_ends() == state.current_date) {
				// make election results

				party_votes.clear();
				float total_vote = 0.0f;

				auto tag = state.world.nation_get_identity_from_identity_holder(n);
				auto start = state.world.national_identity_get_political_party_first(tag).id.index();
				auto end = start + state.world.national_identity_get_political_party_count(tag);

				for(int32_t i = start; i < end; i++) {
					auto pid = dcon::political_party_id(dcon::political_party_id::value_base_t(i));
					if(politics::political_party_is_active(state, pid)) {
						party_votes.push_back(party_vote{pid, 0.0f});
					}
				}

				if(party_votes.size() == 0)
					std::abort(); // ERROR: no valid parties

				// - Determine the vote in each province. Note that voting is *by active party* not by ideology.
				for(auto p : n.get_province_ownership()) {
					if(p.get_province().get_is_colonial())
						continue; // skip colonial provinces

					provincial_party_votes.clear();
					float province_total = 0.0f;
					for(auto& par : party_votes) {
						provincial_party_votes.push_back(party_vote{ par.par, 0.0f });
					}

					float ruling_party_support = p.get_province().get_modifier_values(sys::provincial_mod_offsets::local_ruling_party_support) + n.get_modifier_values(sys::national_mod_offsets::ruling_party_support) + 1.0f;
					float prov_vote_mod = p.get_province().get_modifier_values(sys::provincial_mod_offsets::number_of_voters) + 1.0f;

					for(auto pop : p.get_province().get_pop_location()) {
						auto weight = pop_vote_weight(state, pop.get_pop(), n);
						if(weight > 0) {
							auto ideological_share = pop.get_pop().get_consciousness() / 20.0f;
							for(auto& par : provincial_party_votes) {
								/*
								- For each party we do the following: figure out the pop's ideological support for the party and its issues based support for the party (by summing up its support for each issue that the party has set, except that pops of non-accepted cultures will never support more restrictive culture voting parties). The pop then votes for the party (i.e. contributes its voting weight in support) based on the sum of its issue and ideological support, except that the greater consciousness the pop has, the more its vote is based on ideological support (pops with 0 consciousness vote based on issues alone). The support for the party is then multiplied by (provincial-modifier-ruling-party-support + national-modifier-ruling-party-support + 1), if it is the ruling party, and by (1 + province-party-loyalty) for its ideology.
								- Pop votes are also multiplied by (provincial-modifier-number-of-voters + 1)
								*/
								auto pid = state.world.political_party_get_ideology(par.par);
								auto base_support = (p.get_province().get_party_loyalty(pid) + 1.0f) * prov_vote_mod * (par.par == n.get_ruling_party() ? ruling_party_support : 1.0f) * weight;
								auto issue_support = 0.0f;
								for(auto pi : state.culture_definitions.party_issues) {
									auto party_pos = state.world.political_party_get_party_issues(par.par, pi);
									issue_support += pop.get_pop().get_demographics(pop_demographics::to_key(state, party_pos));
								}
								auto ideology_support = pop.get_pop().get_demographics(pop_demographics::to_key(state, pid));
								auto total_support = base_support * (issue_support * (1.0f - ideological_share) + ideology_support * ideological_share);

								province_total += total_support;
								par.vote += total_support;
							}
						}
					}

					if(province_total > 0) {
						/*
						- After the vote has occurred in each province, the winning party there has the province's ideological loyalty for its ideology increased by define:LOYALTY_BOOST_ON_PARTY_WIN x (provincial-boost-strongest-party-modifier + 1) x fraction-of-vote-for-winning-party
						- If voting rule "largest_share" is in effect: all votes are added to the sum towards the party that recieved the most votes in the province. If it is "dhont", then the votes in each province are normalized to the number of votes from the province, and for "sainte_laque" the votes from the provinces are simply summed up.
						*/

						uint32_t winner = 0;
						float winner_amount = provincial_party_votes[0].vote;
						for(uint32_t i = 1; i < provincial_party_votes.size(); ++i) {
							if(provincial_party_votes[i].vote > winner_amount) {
								winner = i;
								winner_amount = provincial_party_votes[i].vote;
							}
						}

						auto pid = state.world.political_party_get_ideology(provincial_party_votes[winner].par);
						auto& l = p.get_province().get_party_loyalty(pid);
						l = std::clamp(l + state.defines.loyalty_boost_on_party_win * (p.get_province().get_modifier_values(sys::provincial_mod_offsets::boost_strongest_party) + 1.0f) * winner_amount / province_total, -1.0f, 1.0f);
						auto national_rule = n.get_combined_issue_rules();

						if((national_rule & issue_rule::largest_share) != 0) {
							party_votes[winner].vote += winner_amount;
						} else if((national_rule & issue_rule::dhont) != 0) {
							for(uint32_t i = 0; i < provincial_party_votes.size(); ++i) {
								party_votes[i].vote += provincial_party_votes[i].vote / province_total;
							}
						} else /*if((national_rule & issue_rule::sainte_laque) != 0)*/ {
							for(uint32_t i = 0; i < provincial_party_votes.size(); ++i) {
								party_votes[i].vote += provincial_party_votes[i].vote;
							}
						}
					}
					
				}

				/*
				What happens with the election result depends partly on the average militancy of the nation. If it is less than 5: We find the ideology group with the greatest support (each active party gives their support to their group), then the party with the greatest support from the winning group is elected. If average militancy is greater than 5: the party with the greatest individual support wins. (Note: so at average militancy less than 5, having parties with duplicate ideologies makes an ideology group much more likely to win, because they get double counted.)
				*/

				auto total_pop = n.get_demographics(demographics::total);
				auto avg_mil = total_pop > 0.0f ? n.get_demographics(demographics::militancy) / total_pop : 0.0f;

				if(avg_mil <= 5.0f) {
					static auto per_group = state.world.ideology_group_make_vectorizable_float_buffer();
					for(auto ig : state.world.in_ideology_group) {
						per_group.set(ig, 0.0f);
					}
					for(uint32_t i = 0; i < party_votes.size(); ++i) {
						per_group.get(state.world.ideology_get_ideology_group_from_ideology_group_membership(state.world.political_party_get_ideology(party_votes[i].par))) += party_votes[i].vote;
					}
					dcon::ideology_group_id winner;
					float winner_amount = -1.0f;
					for(auto ig : state.world.in_ideology_group) {
						if(per_group.get(ig) > winner_amount) {
							winner_amount = per_group.get(ig);
							winner = ig;
						}
					}

					uint32_t winner_b = 0;
					float winner_amount_b = -1.0f;
					for(uint32_t i = 0; i < party_votes.size(); ++i) {
						if(state.world.ideology_get_ideology_group_from_ideology_group_membership(state.world.political_party_get_ideology(party_votes[i].par)) == winner && party_votes[i].vote > winner_amount_b) {
							winner_b = i;
							winner_amount_b = party_votes[i].vote;
						}
					}

					set_ruling_party(state, n, party_votes[winner_b].par);
				} else {
					uint32_t winner = 0;
					float winner_amount = party_votes[0].vote;
					for(uint32_t i = 1; i < party_votes.size(); ++i) {
						if(party_votes[i].vote > winner_amount) {
							winner = i;
							winner_amount = party_votes[i].vote;
						}
					}

					set_ruling_party(state, n, party_votes[winner].par);
				}

			} else if(next_election_date(state, n) <= state.current_date) {
				start_election(state, n);
			}
		}
	}

}

void set_issue_option(sys::state& state, dcon::nation_id n, dcon::issue_option_id opt) {
	auto parent = state.world.issue_option_get_parent_issue(opt);
	state.world.nation_set_issues(n, parent, opt);
	auto effect_t = state.world.issue_option_get_on_execute_trigger(opt);
	auto effect_k = state.world.issue_option_get_on_execute_effect(opt);
	if(effect_k && (!effect_t || trigger::evaluate(state, effect_t, trigger::to_generic(n), trigger::to_generic(n), 0))) {
		effect::execute(state, effect_k, trigger::to_generic(n), trigger::to_generic(n), 0, uint32_t(state.current_date.value), uint32_t((opt.index() << 2) ^ n.index()));
	}
}
void set_reform_option(sys::state& state, dcon::nation_id n, dcon::reform_option_id opt) {
	auto parent = state.world.reform_option_get_parent_reform(opt);
	state.world.nation_set_reforms(n, parent, opt);
	auto effect_t = state.world.reform_option_get_on_execute_trigger(opt);
	auto effect_k = state.world.reform_option_get_on_execute_effect(opt);
	if(effect_k && (!effect_t || trigger::evaluate(state, effect_t, trigger::to_generic(n), trigger::to_generic(n), 0))) {
		effect::execute(state, effect_k, trigger::to_generic(n), trigger::to_generic(n), 0, uint32_t(state.current_date.value), uint32_t((opt.index() << 2) ^ n.index()));
	}
}

}
