#pragma once
#include "dcon_generated.hpp"
#include "container_types.hpp"
#include "text.hpp"

namespace ai {

void update_ai_general_status(sys::state& state);
void form_alliances(sys::state& state);
void prune_alliances(sys::state& state);
bool ai_will_accept_alliance(sys::state& state, dcon::nation_id target, dcon::nation_id from);
void explain_ai_alliance_reasons(sys::state& state, dcon::nation_id target, text::layout_base& contents, int32_t indent);
bool ai_will_grant_access(sys::state& state, dcon::nation_id target, dcon::nation_id from);
void explain_ai_access_reasons(sys::state& state, dcon::nation_id target, text::layout_base& contents, int32_t indent);
void update_ai_research(sys::state& state);
void initialize_ai_tech_weights(sys::state& state);
void update_influence_priorities(sys::state& state);
void perform_influence_actions(sys::state& state);
void update_focuses(sys::state& state);
void identify_focuses(sys::state& state);
void take_ai_decisions(sys::state& state);
void update_ai_ruling_party(sys::state& state);
void get_desired_factory_types(sys::state& state, dcon::nation_id nid, std::vector<dcon::factory_type_id>& desired_types);
void update_ai_econ_construction(sys::state& state);
void update_ai_colonial_investment(sys::state& state);
void update_ai_colony_starting(sys::state& state);
void upgrade_colonies(sys::state& state);
void civilize(sys::state& state);
void take_reforms(sys::state& state);
bool will_be_crisis_primary_attacker(sys::state& state, dcon::nation_id n);
bool will_be_crisis_primary_defender(sys::state& state, dcon::nation_id n);
bool will_accept_crisis_peace_offer(sys::state& state, dcon::nation_id to, dcon::peace_offer_id peace);
void update_crisis_leaders(sys::state& state);
bool will_join_crisis_with_offer(sys::state& state, dcon::nation_id n, sys::crisis_join_offer const& offer);
void update_war_intervention(sys::state& state);
void update_cb_fabrication(sys::state& state);
bool will_join_war(sys::state& state, dcon::nation_id, dcon::war_id, bool as_attacker);
void add_free_ai_cbs_to_war(sys::state& state, dcon::nation_id n, dcon::war_id w);
void add_gw_goals(sys::state& state);
bool will_accept_peace_offer(sys::state& state, dcon::nation_id n, dcon::nation_id from, dcon::peace_offer_id p);
void make_peace_offers(sys::state& state);
void make_war_decs(sys::state& state);
void update_budget(sys::state& state);
void remove_ai_data(sys::state& state, dcon::nation_id n);
void update_ships(sys::state& state);
void build_ships(sys::state& state);
void refresh_home_ports(sys::state& state);
void daily_cleanup(sys::state& state);
void move_idle_guards(sys::state& state);
void update_land_constructions(sys::state& state);
void update_naval_transport(sys::state& state);
void move_gathered_attackers(sys::state& state);
void gather_to_battle(sys::state& state, dcon::nation_id n, dcon::province_id p);
void make_attacks(sys::state& state);
void make_defense(sys::state& state);
void perform_battle_cycling(sys::state& state);
void general_ai_unit_tick(sys::state& state);

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

}
