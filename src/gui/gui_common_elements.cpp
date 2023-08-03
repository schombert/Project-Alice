#include "gui_common_elements.hpp"

namespace ui {
void sort_countries(sys::state& state, std::vector<dcon::nation_id>& list, country_list_sort sort, bool sort_ascend) {
	switch(sort) {
	case country_list_sort::country: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			dcon::nation_fat_id a_fat_id = dcon::fatten(state.world, a);
			auto a_name = text::produce_simple_string(state, a_fat_id.get_name());
			dcon::nation_fat_id b_fat_id = dcon::fatten(state.world, b);
			auto b_name = text::produce_simple_string(state, b_fat_id.get_name());
			return a_name < b_name;
		};
		std::stable_sort(list.begin(), list.end(), f);
	} break;
	case country_list_sort::economic_rank: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) { return state.world.nation_get_industrial_rank(a) < state.world.nation_get_industrial_rank(b); };
		std::stable_sort(list.begin(), list.end(), f);
	} break;
	case country_list_sort::military_rank: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) { return state.world.nation_get_military_rank(a) < state.world.nation_get_military_rank(b); };
		std::stable_sort(list.begin(), list.end(), f);
	} break;
	case country_list_sort::prestige_rank: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) { return state.world.nation_get_prestige_rank(a) < state.world.nation_get_prestige_rank(b); };
		std::stable_sort(list.begin(), list.end(), f);
	} break;
	case country_list_sort::total_rank: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) { return state.world.nation_get_rank(a) < state.world.nation_get_rank(b); };
		std::stable_sort(list.begin(), list.end(), f);
	} break;
	case country_list_sort::relation: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			auto rid_a = state.world.get_diplomatic_relation_by_diplomatic_pair(state.local_player_nation, a);
			auto va = bool(rid_a) ? state.world.diplomatic_relation_get_value(rid_a) : 0;
			auto rid_b = state.world.get_diplomatic_relation_by_diplomatic_pair(state.local_player_nation, b);
			auto vb = bool(rid_b) ? state.world.diplomatic_relation_get_value(rid_b) : 0;
			return va > vb;
		};
		std::stable_sort(list.begin(), list.end(), f);
	} break;
	case country_list_sort::opinion: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			auto grid_a = state.world.get_gp_relationship_by_gp_influence_pair(a, state.local_player_nation);
			auto va = bool(grid_a) ? state.world.gp_relationship_get_status(grid_a) : 0;
			auto grid_b = state.world.get_gp_relationship_by_gp_influence_pair(b, state.local_player_nation);
			auto vb = bool(grid_b) ? state.world.gp_relationship_get_status(grid_b) : 0;
			return nations::influence::is_influence_level_greater(va & nations::influence::level_mask, vb & nations::influence::level_mask);
		};
		std::stable_sort(list.begin(), list.end(), f);
	} break;
	case country_list_sort::priority: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			auto rel_a = state.world.get_gp_relationship_by_gp_influence_pair(a, state.local_player_nation);
			uint8_t rel_flags_a = bool(rel_a) ? state.world.gp_relationship_get_status(rel_a) : 0;
			auto rel_b = state.world.get_gp_relationship_by_gp_influence_pair(b, state.local_player_nation);
			uint8_t rel_flags_b = bool(rel_b) ? state.world.gp_relationship_get_status(rel_b) : 0;
			return (rel_flags_a & nations::influence::priority_mask) > (rel_flags_b & nations::influence::priority_mask);
		};
		std::stable_sort(list.begin(), list.end(), f);
	} break;
	case country_list_sort::boss: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) { return state.world.nation_get_in_sphere_of(a).id.index() > state.world.nation_get_in_sphere_of(b).id.index(); };
		std::stable_sort(list.begin(), list.end(), f);
	} break;
	case country_list_sort::player_influence: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			auto rel_a = state.world.get_gp_relationship_by_gp_influence_pair(a, state.local_player_nation);
			auto rel_b = state.world.get_gp_relationship_by_gp_influence_pair(b, state.local_player_nation);
			return state.world.gp_relationship_get_influence(rel_a) > state.world.gp_relationship_get_influence(rel_b);
		};
		std::stable_sort(list.begin(), list.end(), f);
	} break;
	case country_list_sort::player_investment: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			auto urel_a = state.world.get_unilateral_relationship_by_unilateral_pair(a, state.local_player_nation);
			auto urel_b = state.world.get_unilateral_relationship_by_unilateral_pair(b, state.local_player_nation);
			return state.world.unilateral_relationship_get_foreign_investment(urel_a) > state.world.unilateral_relationship_get_foreign_investment(urel_b);
		};
		std::stable_sort(list.begin(), list.end(), f);
	} break;
	case country_list_sort::factories: {
		auto num_factories = [&](dcon::nation_id n) {
			int32_t total = 0;
			for(auto p : state.world.nation_get_province_ownership(n)) {
				auto fac = p.get_province().get_factory_location();
				total += int32_t(fac.end() - fac.begin());
			}
			return total;
		};
		auto f = [&](dcon::nation_id a, dcon::nation_id b) { return num_factories(a) > num_factories(b); };
		std::stable_sort(list.begin(), list.end(), f);
	} break;
	default:
		uint8_t rank = uint8_t(sort) & 0x3F;
		dcon::nation_id gp = nations::get_nth_great_power(state, rank);

		if((uint8_t(sort) & uint8_t(country_list_sort::gp_influence)) != 0) {
			auto f = [&](dcon::nation_id a, dcon::nation_id b) {
				auto rel_a = state.world.get_gp_relationship_by_gp_influence_pair(a, gp);
				auto rel_b = state.world.get_gp_relationship_by_gp_influence_pair(b, gp);
				return state.world.gp_relationship_get_influence(rel_a) > state.world.gp_relationship_get_influence(rel_b);
			};
			std::sort(list.begin(), list.end(), f);
		} else if((uint8_t(sort) & uint8_t(country_list_sort::gp_investment)) != 0) {
			auto f = [&](dcon::nation_id a, dcon::nation_id b) {
				auto urel_a = state.world.get_unilateral_relationship_by_unilateral_pair(a, gp);
				auto urel_b = state.world.get_unilateral_relationship_by_unilateral_pair(b, gp);
				return state.world.unilateral_relationship_get_foreign_investment(urel_a) > state.world.unilateral_relationship_get_foreign_investment(urel_b);
			};
			std::stable_sort(list.begin(), list.end(), f);
		} else {
			assert(0);
		}
		break;
	}

	if(!sort_ascend) {
		std::reverse(list.begin(), list.end());
	}
}
} // namespace ui
