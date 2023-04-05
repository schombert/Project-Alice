
#include "politics.hpp"
#include "dcon_generated.hpp"
#include "demographics.hpp"

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

}
