
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
    return sys::date(uint16_t(end_date.to_raw_value() + 1825));
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
    return reform_option == state.world.nation_get_reforms(nation, reform.id).id;
}

bool issue_is_selected(sys::state& state, dcon::nation_id nation, dcon::issue_option_id issue_option) {
    auto issue = state.world.issue_option_get_parent_issue(issue_option);
    return issue && issue_option == state.world.nation_get_issues(nation, issue);
}

bool can_enact_political_reform(sys::state& state, dcon::nation_id nation, dcon::issue_option_id issue_option) {
    auto issue = state.world.issue_option_get_parent_issue(issue_option);
    auto current = state.world.nation_get_issues(nation, issue.id).id;
    auto allow = state.world.issue_option_get_allow(issue_option);
    if(
        (!state.world.issue_get_is_next_step_only(issue.id) || current.index() + 1 == issue_option.index() || current.index() - 1 == issue_option.index())
        &&
        (!allow || trigger::evaluate_trigger(state, allow, trigger::to_generic(nation), trigger::to_generic(nation), 0))
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
    auto issue = state.world.issue_option_get_parent_issue(o);
    auto current = state.world.nation_get_issues(n, issue.id).id;
    auto allow = state.world.issue_option_get_allow(o);
    if(
        (!state.world.issue_get_is_next_step_only(issue.id) || current.index() + 1 == o.index() || current.index() - 1 == o.index())
        &&
        (!allow || trigger::evaluate_trigger(state, allow, trigger::to_generic(n), trigger::to_generic(n), 0))
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
    auto reform = state.world.reform_option_get_parent_reform(o);
    auto current = state.world.nation_get_reforms(n, reform.id).id;
    auto allow = state.world.reform_option_get_allow(o);
    auto stored_rp = state.world.nation_get_research_points(n);
    if(
        o.index() > current.index()
        &&
        (!state.world.reform_get_is_next_step_only(reform.id) || current.index() + 1 == o.index())
        &&
        (!allow || trigger::evaluate_trigger(state, allow, trigger::to_generic(n), trigger::to_generic(n), 0))
        ) {

        float base_cost = float(state.world.reform_option_get_technology_cost(o));
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

        if(base_cost * reform_factor < stored_rp)
            return true;
    }
    return false;
}

bool can_enact_economic_reform(sys::state& state, dcon::nation_id n, dcon::reform_option_id o) {
    auto reform = state.world.reform_option_get_parent_reform(o);
    auto current = state.world.nation_get_reforms(n, reform.id).id;
    auto allow = state.world.reform_option_get_allow(o);
    auto stored_rp = state.world.nation_get_research_points(n);
    if(
        o.index() > current.index()
        &&
        (!state.world.reform_get_is_next_step_only(reform.id) || current.index() + 1 == o.index())
        &&
        (!allow || trigger::evaluate_trigger(state, allow, trigger::to_generic(n), trigger::to_generic(n), 0))
        ) {

        float base_cost = float(state.world.reform_option_get_technology_cost(o));
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

        if(base_cost * reform_factor < stored_rp)
            return true;
    }
    return false;
}

}
