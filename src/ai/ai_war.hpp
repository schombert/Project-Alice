#pragma once
#include "dcon_generated.hpp"
#include "ai_types.hpp"

namespace sys {
struct state;
}

namespace ai {
void update_war_intervention(sys::state& state);
void update_cb_fabrication(sys::state& state);
bool will_join_war(sys::state& state, dcon::nation_id, dcon::war_id, bool as_attacker);
void add_free_ai_cbs_to_war(sys::state& state, dcon::nation_id n, dcon::war_id w);
void add_wargoals(sys::state& state);
bool will_accept_peace_offer(sys::state& state, dcon::nation_id n, dcon::nation_id from, dcon::peace_offer_id p);
void make_peace_offers(sys::state& state);
void make_war_decs(sys::state& state);
bool will_be_crisis_primary_attacker(sys::state& state, dcon::nation_id n);
bool will_be_crisis_primary_defender(sys::state& state, dcon::nation_id n);
bool will_accept_crisis_peace_offer(sys::state& state, dcon::nation_id to, dcon::peace_offer_id peace);
void update_crisis_leaders(sys::state& state);
bool will_join_crisis_with_offer(sys::state& state, dcon::nation_id n, sys::full_wg offer);
bool will_accept_peace_offer_value(sys::state& state,
	dcon::nation_id n, dcon::nation_id from,
	dcon::nation_id prime_attacker, dcon::nation_id prime_defender,
	float primary_warscore, float scoreagainst_me,
	bool offer_from_attacker, bool concession,
	int32_t overall_po_value, int32_t my_po_target,
	int32_t target_personal_po_value, int32_t potential_peace_score_against,
	int32_t my_side_against_target, int32_t my_side_peace_cost,
	int32_t war_duration, bool contains_sq);
bool will_accept_crisis_peace_offer(sys::state& state, dcon::nation_id to, bool is_concession, bool missing_wg);
void prepare_list_of_targets_for_cb(
	sys::state& state,
	std::vector<ai::possible_cb>& result,
	dcon::nation_id attacker,
	dcon::nation_id target,
	dcon::cb_type_id cb,
	dcon::war_id w // can be empty!!!
);
void sort_prepared_list_of_cb(
	sys::state& state,
	std::vector<ai::possible_cb>& result
);
void prepare_and_sort_list_of_desired_states(
	sys::state& state,
	std::vector<ai::weighted_state_instance>& result,
	dcon::nation_id attacker,
	dcon::nation_id target
);
}
