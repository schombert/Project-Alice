#pragma once

#include "date_interface.hpp"
#include "dcon_generated.hpp"
#include "system_state.hpp"
#include <string_view>

namespace politics {

float vote_total(sys::state& state, dcon::nation_id nation);
float get_weighted_vote_size(sys::state& state, dcon::nation_id nation, dcon::pop_id pop);
float get_popular_support(sys::state& state, dcon::nation_id nation, dcon::issue_option_id issue_option);
float get_voter_support(sys::state& state, dcon::nation_id nation, dcon::issue_option_id issue_option);

bool can_appoint_ruling_party(sys::state& state, dcon::nation_id nation);
bool is_election_ongoing(sys::state& state, dcon::nation_id nation);
bool has_elections(sys::state& state, dcon::nation_id nation);
sys::date next_election_date(sys::state& state, dcon::nation_id nation);

dcon::reform_id get_reform_by_name(sys::state& state, std::string_view name);
dcon::issue_id get_issue_by_name(sys::state& state, std::string_view name);
bool reform_is_selected(sys::state& state, dcon::nation_id nation, dcon::reform_option_id reform_option);
bool issue_is_selected(sys::state& state, dcon::nation_id nation, dcon::issue_option_id issue_option);

}
