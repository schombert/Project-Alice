namespace ai {

constexpr inline float ally_overestimate = 2.f;

static void internal_get_alliance_targets_by_adjacency(sys::state& state, dcon::nation_id n, dcon::nation_id adj, std::vector<dcon::nation_id>& alliance_targets) {
	for(auto nb : state.world.nation_get_nation_adjacency(adj)) {
		auto other = nb.get_connected_nations(0) != adj ? nb.get_connected_nations(0) : nb.get_connected_nations(1);

		auto our_strength = estimate_strength(state, n);
		auto other_strength = estimate_strength(state, other);

		// We really shouldn't ally very weak neighbours
		// Surrounding ourselves with useless alliances stops expansion
		if(other_strength <= our_strength * 0.2f) {
			continue;
		}

		bool b = other.get_is_player_controlled()
			? state.world.unilateral_relationship_get_interested_in_alliance(state.world.get_unilateral_relationship_by_unilateral_pair(n, other))
			: ai_will_accept_alliance(state, other, n);
		if(other != n && !(other.get_overlord_as_subject().get_ruler()) && !nations::are_allied(state, n, other) && !military::are_at_war(state, other, n) && b) {
			alliance_targets.push_back(other.id);
		}
	}
}
static void internal_get_alliance_targets(sys::state& state, dcon::nation_id n, std::vector<dcon::nation_id>& alliance_targets) {
	// Adjacency with us
	internal_get_alliance_targets_by_adjacency(state, n, n, alliance_targets);
	if(!alliance_targets.empty())
		return;

	// Adjacency with rival (useful for e.x, Chile allying Paraguay to fight bolivia)
	if(auto rival = state.world.nation_get_ai_rival(n); bool(rival)) {
		internal_get_alliance_targets_by_adjacency(state, n, rival, alliance_targets);
		if(!alliance_targets.empty())
			return;
	}

	// Adjacency with people who are at war with us
	for(auto wp : state.world.nation_get_war_participant(n)) {
		for(auto p : state.world.war_get_war_participant(wp.get_war())) {
			if(p.get_is_attacker() == !wp.get_is_attacker()) {
				internal_get_alliance_targets_by_adjacency(state, n, p.get_nation(), alliance_targets);
				if(!alliance_targets.empty())
					return;
			}
		}
	}
}

void form_alliances(sys::state& state) {
	static std::vector<dcon::nation_id> alliance_targets;
	for(auto n : state.world.in_nation) {
		if(!n.get_is_player_controlled() && n.get_ai_is_threatened() && !(n.get_overlord_as_subject().get_ruler())) {
			alliance_targets.clear();
			internal_get_alliance_targets(state, n, alliance_targets);
			if(!alliance_targets.empty()) {
				std::sort(alliance_targets.begin(), alliance_targets.end(), [&](dcon::nation_id a, dcon::nation_id b) {
					if(estimate_strength(state, a) != estimate_strength(state, b))
						return estimate_strength(state, a) > estimate_strength(state, b);
					else
						return a.index() > b.index();
				});
				if(state.world.nation_get_is_player_controlled(alliance_targets[0])) {
					notification::post(state, notification::message{
						[source = n](sys::state& state, text::layout_base& contents) {
							text::add_line(state, contents, "msg_entered_automatic_alliance_1", text::variable_type::x, source);
						},
						"msg_entered_automatic_alliance_title",
						n, dcon::nation_id{}, dcon::nation_id{},
						sys::message_base_type::entered_automatic_alliance
					});
				}
				nations::make_alliance(state, n, alliance_targets[0]);
			}
		}
	}
}

void prune_alliances(sys::state& state) {
	static std::vector<dcon::nation_id> prune_targets;
	for(auto n : state.world.in_nation) {
		if(!n.get_is_player_controlled()
		&& !n.get_ai_is_threatened()
		&& !(n.get_overlord_as_subject().get_ruler())) {
			prune_targets.clear();
			for(auto dr : n.get_diplomatic_relation()) {
				if(dr.get_are_allied()) {
					auto other = dr.get_related_nations(0) != n ? dr.get_related_nations(0) : dr.get_related_nations(1);
					if(other.get_in_sphere_of() != n
					&& !military::are_allied_in_war(state, n, other)) {
						prune_targets.push_back(other);
					}
				}
			}

			if(prune_targets.empty())
				continue;

			std::sort(prune_targets.begin(), prune_targets.end(), [&](dcon::nation_id a, dcon::nation_id b) {
				if(estimate_strength(state, a) != estimate_strength(state, b))
					return estimate_strength(state, a) < estimate_strength(state, b);
				else
					return a.index() > b.index();
			});

			float greatest_neighbor = 0.0f;
			auto in_sphere_of = state.world.nation_get_in_sphere_of(n);

			for(auto b : state.world.nation_get_nation_adjacency_as_connected_nations(n)) {
				auto other = b.get_connected_nations(0) != n ? b.get_connected_nations(0) : b.get_connected_nations(1);
				if(!nations::are_allied(state, n, other) && (!in_sphere_of || in_sphere_of != other.get_in_sphere_of())) {
					greatest_neighbor = std::max(greatest_neighbor, estimate_strength(state, other));
				}
			}

			float defensive_str = estimate_defensive_strength(state, n);
			auto ll = state.world.nation_get_last_war_loss(n);
			float safety_factor = 1.2f;
			if(ll && state.current_date < ll + 365 * 4) {
				safety_factor = 1.8f;
			}

			auto safety_margin = defensive_str - safety_factor * greatest_neighbor;

			for(auto pt : prune_targets) {
				auto weakest_str = estimate_strength(state, pt);
				if(weakest_str * 1.25 < safety_margin) {
					safety_margin -= weakest_str;
					assert(command::can_cancel_alliance(state, n, pt, true));
					command::execute_cancel_alliance(state, n, pt);
				} else if(state.world.nation_get_infamy(pt) >= state.defines.badboy_limit) {
					safety_margin -= weakest_str;
					assert(command::can_cancel_alliance(state, n, pt, true));
					command::execute_cancel_alliance(state, n, pt);
				} else {
					break;
				}
			}
		}
	}
}


bool ai_is_close_enough(sys::state& state, dcon::nation_id target, dcon::nation_id from) {
	auto target_continent = state.world.province_get_continent(state.world.nation_get_capital(target));
	auto source_continent = state.world.province_get_continent(state.world.nation_get_capital(from));
	return (target_continent == source_continent) || bool(state.world.get_nation_adjacency_by_nation_adjacency_pair(target, from));
}

static bool ai_has_mutual_enemy(sys::state& state, dcon::nation_id from, dcon::nation_id target) {
	auto rival_a = state.world.nation_get_ai_rival(target);
	auto rival_b = state.world.nation_get_ai_rival(from);
	// Same rival equates to instantaneous alliance (we benefit from more allies against a common enemy)
	if(rival_a && rival_a == rival_b)
		return true;
	// // Our rivals are allied?
	// if(rival_a && rival_b && rival_a != rival_b && nations::are_allied(state, rival_a, rival_b))
	// 	return true;

	// // One of the allies of our rivals can be declared on?
	// for(auto n : state.world.in_nation) {
	// 	if(n.id != target && n.id != from && n.id != rival_a && n.id != rival_b) {
	// 		if(nations::are_allied(state, rival_a, n.id) || nations::are_allied(state, rival_b, n.id)) {
	// 			bool enemy_a = military::can_use_cb_against(state, from, n.id);
	// 			bool enemy_b = military::can_use_cb_against(state, target, n.id);
	// 			if(enemy_a || enemy_b)
	// 				return true;
	// 		}
	// 	}
	// }
	return false;
}



bool ai_will_accept_alliance(sys::state& state, dcon::nation_id target, dcon::nation_id from) {
	if(!state.world.nation_get_ai_is_threatened(target))
		return false;

	// Has not surpassed infamy limit
	if(state.world.nation_get_infamy(target) >= state.defines.badboy_limit * 0.75f)
		return false;

	// Won't ally our rivals
	if(state.world.nation_get_ai_rival(target) == from || state.world.nation_get_ai_rival(from) == target)
		return false;

	// No more than 2 GP allies
	// No more than 4 alliances
	if(bool(state.defines.alice_artificial_gp_limitant) && state.world.nation_get_is_great_power(target)) {
		int32_t gp_count = 0;
		for(const auto rel : state.world.nation_get_diplomatic_relation(from)) {
			auto n = rel.get_related_nations(rel.get_related_nations(0) == from ? 1 : 0);
			if(rel.get_are_allied() && n.get_is_great_power()) {
				if(gp_count >= 2) {
					return false;
				}
				++gp_count;
			}
		}
	}
	if(bool(state.defines.alice_spherelings_only_ally_sphere)) {
		auto spherelord = state.world.nation_get_in_sphere_of(from);
		//If no spherelord -> Then must not ally spherelings
		//If spherelord -> Then must not ally non-spherelings
		if(state.world.nation_get_in_sphere_of(target) != spherelord && target != spherelord)
			return false;
		if(target == spherelord)
			return true; //always ally spherelord
	}

	auto natid = state.world.nation_get_identity_from_identity_holder(from);
	for(auto prov_owner : state.world.nation_get_province_ownership(target)) {
		auto prov = prov_owner.get_province();

		for(auto core : prov.get_core_as_province()) {
			if(core.get_identity() == natid) {
				return false; // holds our cores
			}
		}
	}

	if(ai_has_mutual_enemy(state, from, target))
		return true;

	// Otherwise we may consider alliances only iff they are close to our continent or we are adjacent
	if(!ai_is_close_enough(state, target, from))
		return false;

	// And also if they're powerful enough to be considered for an alliance
	auto target_score = estimate_strength(state, target);
	auto source_score = estimate_strength(state, from);
	return std::max<float>(source_score, 1.f) * ally_overestimate >= target_score;
}

void explain_ai_alliance_reasons(sys::state& state, dcon::nation_id target, text::layout_base& contents, int32_t indent) {

	text::add_line_with_condition(state, contents, "ai_alliance_1", state.world.nation_get_ai_is_threatened(target), indent);

	text::add_line(state, contents, "kierkegaard_1", indent);

	text::add_line_with_condition(state, contents, "ai_alliance_5", ai_has_mutual_enemy(state, state.local_player_nation, target), indent + 15);

	text::add_line(state, contents, "kierkegaard_2", indent);

	text::add_line_with_condition(state, contents, "ai_alliance_2", ai_is_close_enough(state, target, state.local_player_nation), indent + 15);

	text::add_line_with_condition(state, contents, "ai_alliance_3", state.world.nation_get_ai_rival(target) != state.local_player_nation && state.world.nation_get_ai_rival(state.local_player_nation) != target, indent + 15);

	auto target_score = estimate_strength(state, target);
	auto source_score = estimate_strength(state, state.local_player_nation);
	text::add_line_with_condition(state, contents, "ai_alliance_4", std::max<float>(source_score, 1.f) * ally_overestimate >= target_score, indent + 15);

	auto holdscores = false;
	auto natid = state.world.nation_get_identity_from_identity_holder(state.local_player_nation);
	for(auto prov_owner : state.world.nation_get_province_ownership(target)) {
		auto prov = prov_owner.get_province();

		for(auto core : prov.get_core_as_province()) {
			if(core.get_identity() == natid) {
				holdscores = true; // holds our cores
			}
		}
	}

	text::add_line_with_condition(state, contents, "ai_alliance_5", !holdscores, indent + 15);
}

}
