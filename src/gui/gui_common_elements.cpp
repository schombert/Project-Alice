#include "gui_common_elements.hpp"

namespace ui {
void sort_countries(sys::state& state, std::vector<dcon::nation_id>& list, country_list_sort sort, bool sort_ascend) {
	std::function<bool(dcon::nation_id, dcon::nation_id)> fn;
	switch(sort) {
	case country_list_sort::country:
		fn = [&](dcon::nation_id a, dcon::nation_id b) {
			dcon::nation_fat_id a_fat_id = dcon::fatten(state.world, a);
			auto a_name = text::produce_simple_string(state, a_fat_id.get_name());
			dcon::nation_fat_id b_fat_id = dcon::fatten(state.world, b);
			auto b_name = text::produce_simple_string(state, b_fat_id.get_name());
			return a_name < b_name;
		};
		break;
	case country_list_sort::economic_rank:
		fn = [&](dcon::nation_id a, dcon::nation_id b) {
			return state.world.nation_get_industrial_rank(a) < state.world.nation_get_industrial_rank(b);
		};
		break;
	case country_list_sort::military_rank:
		fn = [&](dcon::nation_id a, dcon::nation_id b) {
			return state.world.nation_get_military_rank(a) < state.world.nation_get_military_rank(b);
		};
		break;
	case country_list_sort::prestige_rank:
		fn = [&](dcon::nation_id a, dcon::nation_id b) {
			return state.world.nation_get_prestige_rank(a) < state.world.nation_get_prestige_rank(b);
		};
		break;
	case country_list_sort::total_rank:
		fn = [&](dcon::nation_id a, dcon::nation_id b) {
			return state.world.nation_get_rank(a) < state.world.nation_get_rank(b);
		};
		break;
	case country_list_sort::relation:
		fn = [&](dcon::nation_id a, dcon::nation_id b) {
			auto rid_a = state.world.get_diplomatic_relation_by_diplomatic_pair(state.local_player_nation, a);
			auto va = bool(rid_a) ? state.world.diplomatic_relation_get_value(rid_a) : 0;
			auto rid_b = state.world.get_diplomatic_relation_by_diplomatic_pair(state.local_player_nation, b);
			auto vb = bool(rid_b) ? state.world.diplomatic_relation_get_value(rid_b) : 0;
			return va < vb;
		};
		break;
	case country_list_sort::opinion:
		fn = [&](dcon::nation_id a, dcon::nation_id b) {
			auto grid_a = state.world.get_gp_relationship_by_gp_influence_pair(a, state.local_player_nation);
			auto va = bool(grid_a) ? state.world.gp_relationship_get_status(grid_a) : 0;
			auto grid_b = state.world.get_gp_relationship_by_gp_influence_pair(b, state.local_player_nation);
			auto vb = bool(grid_b) ? state.world.gp_relationship_get_status(grid_b) : 0;
			return va < vb;
		};
		break;
	case country_list_sort::priority:
		fn = [&](dcon::nation_id a, dcon::nation_id b) {
			auto rel_a = state.world.get_gp_relationship_by_gp_influence_pair(a, state.local_player_nation);
			uint8_t rel_flags_a = bool(rel_a) ? state.world.gp_relationship_get_status(rel_a) : 0;
			auto rel_b = state.world.get_gp_relationship_by_gp_influence_pair(b, state.local_player_nation);
			uint8_t rel_flags_b = bool(rel_b) ? state.world.gp_relationship_get_status(rel_b) : 0;
			return (rel_flags_a & nations::influence::priority_mask) < (rel_flags_b & nations::influence::priority_mask);
		};
		break;
	case country_list_sort::boss:
		fn = [&](dcon::nation_id a, dcon::nation_id b) {
			return state.world.nation_get_in_sphere_of(a).id.index() < state.world.nation_get_in_sphere_of(b).id.index();
		};
		break;
	default:
		if((uint8_t(sort) & uint8_t(country_list_sort::gp_influence)) != 0) {
			fn = [&](dcon::nation_id a, dcon::nation_id b) {
				uint8_t rank = uint8_t(sort) & (~uint8_t(country_list_sort::gp_influence));
				dcon::nation_id gp = nations::get_nth_great_power(state, rank);

				auto rel_a = state.world.get_gp_relationship_by_gp_influence_pair(a, gp);
				auto rel_b = state.world.get_gp_relationship_by_gp_influence_pair(b, gp);
				return state.world.gp_relationship_get_influence(rel_a) < state.world.gp_relationship_get_influence(rel_b);
			};
		} else if((uint8_t(sort) & uint8_t(country_list_sort::gp_investment)) != 0) {
			fn = [&](dcon::nation_id a, dcon::nation_id b) {
				uint8_t rank = uint8_t(sort) & (~uint8_t(country_list_sort::gp_investment));
				dcon::nation_id gp = nations::get_nth_great_power(state, rank);

				auto urel_a = state.world.get_unilateral_relationship_by_unilateral_pair(a, gp);
				auto urel_b = state.world.get_unilateral_relationship_by_unilateral_pair(b, gp);
				return state.world.unilateral_relationship_get_foreign_investment(urel_a) <
					   state.world.unilateral_relationship_get_foreign_investment(urel_b);
			};
		} else {
			assert(0);
		}
		break;
	}
	std::stable_sort(list.begin(), list.end(), [&](dcon::nation_id a, dcon::nation_id b) {
		bool r = fn(a, b);
		return sort_ascend ? r : !r;
	});
}
} // namespace ui
