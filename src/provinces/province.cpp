#include "province.hpp"
#include "province_templates.hpp"
#include "dcon_generated.hpp"
#include "demographics.hpp"
#include "nations.hpp"
#include "system_state.hpp"
#include <vector>
#include "rebels.hpp"
#include "math_fns.hpp"
#include "prng.hpp"
#include "triggers.hpp"
#include "economy_stats.hpp"
#include <set>

namespace province {

template auto is_overseas<ve::tagged_vector<dcon::province_id>>(sys::state const&, ve::tagged_vector<dcon::province_id>);
template void for_each_province_in_state_instance<std::function<void(dcon::province_id)>>(sys::state&, dcon::state_instance_id, std::function<void(dcon::province_id)> const&);

bool is_overseas(sys::state const& state, dcon::province_id ids) {
	auto owners = state.world.province_get_nation_from_province_ownership(ids);
	auto owner_cap = state.world.nation_get_capital(owners);
	return (state.world.province_get_continent(ids) != state.world.province_get_continent(owner_cap)) &&
				 (state.world.province_get_connected_region_id(ids) != state.world.province_get_connected_region_id(owner_cap));
}

bool nations_are_adjacent(sys::state& state, dcon::nation_id a, dcon::nation_id b) {
	auto it = state.world.get_nation_adjacency_by_nation_adjacency_pair(a, b);
	return bool(it);
}
void update_connected_regions(sys::state& state) {
	if(!state.adjacency_data_out_of_date)
		return;

	state.adjacency_data_out_of_date = false;
	state.world.nation_adjacency_resize(0);

	{
		state.world.for_each_province([&](dcon::province_id id) { state.world.province_set_connected_region_id(id, 0); });
		// TODO get a better allocator
		static std::vector<dcon::province_id> to_fill_list;
		uint16_t current_fill_id = 0;
		state.province_definitions.connected_region_is_coastal.clear();

		to_fill_list.reserve(state.world.province_size());

		for(int32_t i = state.province_definitions.first_sea_province.index(); i-- > 0;) {
			dcon::province_id id{dcon::province_id::value_base_t(i)};
			if(state.world.province_get_connected_region_id(id) == 0) {
				++current_fill_id;

				bool found_coast = false;

				to_fill_list.push_back(id);

				while(!to_fill_list.empty()) {
					auto current_id = to_fill_list.back();
					to_fill_list.pop_back();

					found_coast = found_coast || state.world.province_get_is_coast(current_id);

					state.world.province_set_connected_region_id(current_id, current_fill_id);
					for(auto rel : state.world.province_get_province_adjacency(current_id)) {
						if((rel.get_type() & (province::border::coastal_bit | province::border::impassible_bit)) ==
								0) { // not entering sea, not impassible
							auto owner_a = rel.get_connected_provinces(0).get_nation_from_province_ownership();
							auto owner_b = rel.get_connected_provinces(1).get_nation_from_province_ownership();
							if(owner_a == owner_b) { // both have the same owner
								if(rel.get_connected_provinces(0).get_connected_region_id() == 0)
									to_fill_list.push_back(rel.get_connected_provinces(0));
								if(rel.get_connected_provinces(1).get_connected_region_id() == 0)
									to_fill_list.push_back(rel.get_connected_provinces(1));
							} else {
								state.world.try_create_nation_adjacency(owner_a, owner_b);
							}
						}
					}
				}

				state.province_definitions.connected_region_is_coastal.push_back(found_coast);
				to_fill_list.clear();
			}
		}

		// we also invalidate wargoals here that are now unowned
		military::invalidate_unowned_wargoals(state);
	}

	{
		state.world.for_each_province([&](dcon::province_id id) { state.world.province_set_connected_coast_id(id, 0); });

		static std::vector<dcon::province_id> to_fill_list;
		uint16_t current_fill_id = 0;
		to_fill_list.reserve(state.world.province_size());

		for(int32_t i = state.province_definitions.first_sea_province.index(); i-- > 0;) {
			dcon::province_id id{ dcon::province_id::value_base_t(i) };
			if(state.world.province_get_connected_coast_id(id) == 0 && state.world.province_get_is_coast(id)) {
				++current_fill_id;

				to_fill_list.push_back(id);
				while(!to_fill_list.empty()) {
					auto current_id = to_fill_list.back();
					to_fill_list.pop_back();

					state.world.province_set_connected_coast_id(current_id, current_fill_id);
					for(auto rel : state.world.province_get_province_adjacency(current_id)) {
						if(
							(
								rel.get_type()
								&
								(province::border::coastal_bit | province::border::impassible_bit)
							) == 0
						) { // not entering sea, not impassible
							auto owner_a = rel.get_connected_provinces(0).get_nation_from_province_ownership();
							auto owner_b = rel.get_connected_provinces(1).get_nation_from_province_ownership();

							auto coast_a = rel.get_connected_provinces(0).get_is_coast();
							auto coast_b = rel.get_connected_provinces(1).get_is_coast();

							if(owner_a == owner_b && coast_a == coast_b) { // both have the same owner and are coastal
								if(rel.get_connected_provinces(0).get_connected_coast_id() == 0)
									to_fill_list.push_back(rel.get_connected_provinces(0));
								if(rel.get_connected_provinces(1).get_connected_coast_id() == 0)
									to_fill_list.push_back(rel.get_connected_provinces(1));
							}
						}
					}
				}

				to_fill_list.clear();
			}
		}
	}


	static ankerl::unordered_dense::map<dcon::province_id::value_base_t, std::vector<std::pair<size_t, size_t>>> province_to_borders{ };
	if(province_to_borders.empty()) {
		for(size_t i = 0; i < state.map_state.map_data.borders.size(); i++) {
			auto& border = state.map_state.map_data.borders[i];
			auto adj = border.adj;
			auto A = state.world.province_adjacency_get_connected_provinces(adj, 0);
			auto B = state.world.province_adjacency_get_connected_provinces(adj, 1);

			auto query_result_A = province_to_borders.find(A.value);
			if(query_result_A == province_to_borders.end()) {
				province_to_borders[A.value] = {{ i, 0 }};
			} else {
				query_result_A->second.push_back({ i, 0 });
			}

			auto query_result_B = province_to_borders.find(B.value);
			if(query_result_B == province_to_borders.end()) {
				province_to_borders[B.value] = {{ i, 1 }};
			} else {
				query_result_B->second.push_back({ i, 1 });
			}
		}
	}

	// start chaining borders:
	for(size_t i = 0; i < state.map_state.map_data.borders.size(); i++) {
		auto& border = state.map_state.map_data.borders[i];
		if(border.count == 0) {
			continue;
		}

		auto border_start_A = border.start_index;
		auto border_end_A = border.start_index + border.count / 2 - 2;
		auto border_start_B = border.start_index + border.count / 2;
		auto border_end_B = border.start_index + border.count - 2;

		auto A = province::from_map_id(state.map_state.map_data.border_vertices[border_start_A].province_index);
		auto B = province::from_map_id(state.map_state.map_data.border_vertices[border_start_B].province_index);
		auto n_A = state.world.province_get_nation_from_province_ownership(A);
		auto n_B = state.world.province_get_nation_from_province_ownership(B);

		if(n_A == n_B && A && B) {
			if(n_A) {
				continue;
			}
			if(
				A.index() >= state.province_definitions.first_sea_province.index()
				&&
				B.index() >= state.province_definitions.first_sea_province.index()
			) {
				continue;
			}
			if(
				A.index() < state.province_definitions.first_sea_province.index()
				&&
				B.index() < state.province_definitions.first_sea_province.index()
			) {
				continue;
			}
		}

		auto& PA1 = state.map_state.map_data.border_vertices[border_start_A];
		auto& PA2 = state.map_state.map_data.border_vertices[border_end_A];
		auto& PAs1 = state.map_state.map_data.border_vertices[border_start_A + 1];
		auto& PAs2 = state.map_state.map_data.border_vertices[border_end_A + 1];

		auto& PB1 = state.map_state.map_data.border_vertices[border_start_B];
		auto& PB2 = state.map_state.map_data.border_vertices[border_end_B];
		auto& PBs1 = state.map_state.map_data.border_vertices[border_start_B + 1];
		auto& PBs2 = state.map_state.map_data.border_vertices[border_end_B + 1];

		if(PA1.position == PA2.position) {
			if(PA1.previous_point == PA1.position) {
				PA1.previous_point = PA2.previous_point;
				PA2.next_point = PA1.next_point;
				PAs1.previous_point = PAs2.previous_point;
				PAs2.next_point = PAs1.next_point;

				PB1.previous_point = PB2.previous_point;
				PB2.next_point = PB1.next_point;
				PBs1.previous_point = PBs2.previous_point;
				PBs2.next_point = PBs1.next_point;
			}
			/*
			if(PA1.next_point == PA1.position) {
				PA1.next_point = PA2.next_point;
				PA2.previous_point = PA1.previous_point;
				PAs1.next_point = PAs2.next_point;
				PAs2.previous_point = PAs1.previous_point;

				PB1.next_point = PB2.next_point;
				PB2.previous_point = PB1.previous_point;
				PBs1.next_point = PBs2.next_point;
				PBs2.previous_point = PBs1.previous_point;
			}
			*/
			continue;
		}

		// find borders to weave:
		for(auto& candidate_index : province_to_borders[A.value]) {
			auto& candidate_border = state.map_state.map_data.borders[candidate_index.first];

			if(candidate_index.first == i) {
				continue;
			}

			if(candidate_border.count == 0) {
				continue;
			}

			auto candidate_start =
				candidate_border.start_index
				+ candidate_index.second * candidate_border.count / 2;
			auto candidate_end =
				candidate_border.start_index
				+ (1 + candidate_index.second) * candidate_border.count / 2 - 2;

			auto opposite_candidate_start =
				candidate_border.start_index
				+ (1 - candidate_index.second) * candidate_border.count / 2;
			auto opposite_candidate_end =
				candidate_border.start_index
				+ (1 + (1 - candidate_index.second)) * candidate_border.count / 2 - 2;

			auto opposite_province = province::from_map_id(state.map_state.map_data.border_vertices[opposite_candidate_start].province_index);
			auto opposite_nation = state.world.province_get_nation_from_province_ownership(opposite_province);

			if(opposite_province == A) {
				std::swap(candidate_start, opposite_candidate_start);
				std::swap(candidate_end, opposite_candidate_end);
				opposite_province = province::from_map_id(state.map_state.map_data.border_vertices[opposite_candidate_start].province_index);
				opposite_nation = state.world.province_get_nation_from_province_ownership(opposite_province);
			}

			auto& QT1 = state.map_state.map_data.border_vertices[candidate_start];
			auto& QT2 = state.map_state.map_data.border_vertices[candidate_end];
			auto& QTs1 = state.map_state.map_data.border_vertices[candidate_start + 1];
			auto& QTs2 = state.map_state.map_data.border_vertices[candidate_end + 1];

			auto& QW1 = state.map_state.map_data.border_vertices[opposite_candidate_start];
			auto& QW2 = state.map_state.map_data.border_vertices[opposite_candidate_end];
			auto& QWs1 = state.map_state.map_data.border_vertices[opposite_candidate_start + 1];
			auto& QWs2 = state.map_state.map_data.border_vertices[opposite_candidate_end + 1];			

			if(PA1.position == QT1.position) {
				if(opposite_nation != n_A) {
					PA1.previous_point = QT1.next_point;
					PAs1.previous_point = QTs1.next_point;
					QT1.previous_point = PA1.next_point;
					QTs1.previous_point = PAs1.next_point;
				}

				if(opposite_nation == n_B) {

					PB1.previous_point = QT1.next_point;
					PBs1.previous_point = QTs1.next_point;
					QW1.previous_point = PA1.next_point;
					QWs1.previous_point = PAs1.next_point;
				}
			}
			if(PA1.position == QT2.position) {
				if(opposite_nation != n_A) {
					PA1.previous_point = QT2.previous_point;
					PAs1.previous_point = QTs2.previous_point;
					QT2.next_point = PA1.next_point;
					QTs2.next_point = PAs1.next_point;
				}


				if(opposite_nation == n_B) {
					PB1.previous_point = QT2.previous_point;
					PBs1.previous_point = QTs2.previous_point;
					QW2.next_point = PA1.next_point;
					QWs2.next_point = PAs1.next_point;
				}
			}
			if(PA2.position == QT1.position) {
				if(opposite_nation != n_A) {
					PA2.next_point = QT1.next_point;
					PAs2.next_point = QTs1.next_point;
					QT1.previous_point = PA2.previous_point;
					QTs1.previous_point = PAs2.previous_point;
				}

				if(opposite_nation == n_B) {
					PB2.next_point = QT1.next_point;
					PBs2.next_point = QTs1.next_point;
					QW1.previous_point = PA2.previous_point;
					QWs1.previous_point = PAs2.previous_point;
				}
			}
			if(PA2.position == QT2.position) {
				if(opposite_nation != n_A) {
					PA2.next_point = QT2.previous_point;
					PAs2.next_point = QTs2.previous_point;
					QT2.next_point = PA2.previous_point;
					QTs2.next_point = PAs2.previous_point;
				}

				if(opposite_nation == n_B) {
					PB2.next_point = QT2.previous_point;
					PBs2.next_point = QTs2.previous_point;
					QW2.next_point = PA2.previous_point;
					QWs2.next_point = PAs2.previous_point;
				}
			}
		}

		for(auto& candidate_index : province_to_borders[B.value]) {
			auto& candidate_border = state.map_state.map_data.borders[candidate_index.first];

			if(candidate_index.first == i) {
				continue;
			}

			if(candidate_border.count == 0) {
				continue;
			}

			auto candidate_start =
				candidate_border.start_index
				+ candidate_index.second * candidate_border.count / 2;
			auto candidate_end =
				candidate_border.start_index
				+ (1 + candidate_index.second) * candidate_border.count / 2 - 2;

			auto opposite_candidate_start =
				candidate_border.start_index
				+ (1 - candidate_index.second) * candidate_border.count / 2;
			auto opposite_candidate_end =
				candidate_border.start_index
				+ (1 + (1 - candidate_index.second)) * candidate_border.count / 2 - 2;

			auto opposite_province = province::from_map_id(state.map_state.map_data.border_vertices[opposite_candidate_start].province_index);
			auto opposite_nation = state.world.province_get_nation_from_province_ownership(opposite_province);

			if(opposite_province == B) {
				std::swap(candidate_start, opposite_candidate_start);
				std::swap(candidate_end, opposite_candidate_end);
				opposite_province = province::from_map_id(state.map_state.map_data.border_vertices[opposite_candidate_start].province_index);
				opposite_nation = state.world.province_get_nation_from_province_ownership(opposite_province);
			}

			auto& QT1 = state.map_state.map_data.border_vertices[candidate_start];
			auto& QT2 = state.map_state.map_data.border_vertices[candidate_end];
			auto& QTs1 = state.map_state.map_data.border_vertices[candidate_start + 1];
			auto& QTs2 = state.map_state.map_data.border_vertices[candidate_end + 1];

			auto& QW1 = state.map_state.map_data.border_vertices[opposite_candidate_start];
			auto& QW2 = state.map_state.map_data.border_vertices[opposite_candidate_end];
			auto& QWs1 = state.map_state.map_data.border_vertices[opposite_candidate_start + 1];
			auto& QWs2 = state.map_state.map_data.border_vertices[opposite_candidate_end + 1];

			if(PA1.position == QT1.position) {
				if(opposite_nation != n_B) {
					PB1.previous_point = QT1.next_point;
					PBs1.previous_point = QTs1.next_point;
					QT1.previous_point = PA1.next_point;
					QTs1.previous_point = PAs1.next_point;
				}

				if(opposite_nation == n_A) {
					PA1.previous_point = QT1.next_point;
					PAs1.previous_point = QTs1.next_point;
					QW1.previous_point = PA1.next_point;
					QWs1.previous_point = PAs1.next_point;
				}
			}
			if(PA1.position == QT2.position) {
				if(opposite_nation != n_B) {
					PB1.previous_point = QT2.previous_point;
					PBs1.previous_point = QTs2.previous_point;
					QT2.next_point = PA1.next_point;
					QTs2.next_point = PAs1.next_point;
				}


				if(opposite_nation == n_A) {
					PA1.previous_point = QT2.previous_point;
					PAs1.previous_point = QTs2.previous_point;
					QW2.next_point = PA1.next_point;
					QWs2.next_point = PAs1.next_point;
				}
			}
			if(PA2.position == QT1.position) {
				if(opposite_nation != n_B) {
					PB2.next_point = QT1.next_point;
					PBs2.next_point = QTs1.next_point;
					QT1.previous_point = PA2.previous_point;
					QTs1.previous_point = PAs2.previous_point;
				}

				if(opposite_nation == n_A) {
					PA2.next_point = QT1.next_point;
					PAs2.next_point = QTs1.next_point;
					QW1.previous_point = PA2.previous_point;
					QWs1.previous_point = PAs2.previous_point;
				}
			}
			if(PA2.position == QT2.position) {
				if(opposite_nation != n_B) {
					PB2.next_point = QT2.previous_point;
					PBs2.next_point = QTs2.previous_point;
					QT2.next_point = PA2.previous_point;
					QTs2.next_point = PAs2.previous_point;
				}

				if(opposite_nation == n_A) {
					PA2.next_point = QT2.previous_point;
					PAs2.next_point = QTs2.previous_point;
					QW2.next_point = PA2.previous_point;
					QWs2.next_point = PAs2.previous_point;
				}
			}
		}
	}
	state.province_ownership_changed.store(true, std::memory_order::release);
}

dcon::province_id pick_capital(sys::state& state, dcon::nation_id n) {
	auto trad_cap = state.world.national_identity_get_capital(state.world.nation_get_identity_from_identity_holder(n));
	if(state.world.province_get_nation_from_province_ownership(trad_cap) == n) {
		return trad_cap;
	}
	dcon::province_id best_choice;
	for(auto prov : state.world.nation_get_province_ownership(n)) {
		if(prov.get_province().get_demographics(demographics::total) >
						state.world.province_get_demographics(best_choice, demographics::total) &&
				prov.get_province().get_is_owner_core() == state.world.province_get_is_owner_core(best_choice)) {
			best_choice = prov.get_province().id;
		} else if(prov.get_province().get_is_owner_core() && !state.world.province_get_is_owner_core(best_choice)) {
			best_choice = prov.get_province().id;
		}
	}
	return best_choice;
}

void set_province_controller(sys::state& state, dcon::province_id p, dcon::nation_id n) {
	auto old_con = state.world.province_get_nation_from_province_control(p);
	if(old_con != n) {
		state.world.province_set_last_control_change(p, state.current_date);
		auto rc = state.world.province_get_rebel_faction_from_province_rebel_control(p);
		auto owner = state.world.province_get_nation_from_province_ownership(p);
		if(rc && owner) {
			state.world.nation_get_rebel_controlled_count(owner) -= uint16_t(1);
			if(!is_overseas(state, p)) {
				state.world.nation_get_central_rebel_controlled(owner) -= uint16_t(1);
			}
		}
		if(owner) {
			if(old_con == owner) {
				state.world.nation_get_occupied_count(owner) += uint16_t(1);
				if(state.world.province_get_is_blockaded(p) && !is_overseas(state, p)) {
					state.world.nation_get_central_blockaded(owner) -= uint16_t(1);
				}
			} else if(n == owner) {
				state.world.nation_get_occupied_count(owner) -= uint16_t(1);
			}
		}
		state.world.province_set_rebel_faction_from_province_rebel_control(p, dcon::rebel_faction_id{});
		state.world.province_set_nation_from_province_control(p, n);
		state.military_definitions.pending_blackflag_update = true;
	}
}

void set_province_controller(sys::state& state, dcon::province_id p, dcon::rebel_faction_id rf) {
	auto old_con = state.world.province_get_rebel_faction_from_province_rebel_control(p);
	if(old_con != rf) {
		state.world.province_set_last_control_change(p, state.current_date);
		auto owner = state.world.province_get_nation_from_province_ownership(p);
		if(!old_con && owner) {
			state.world.nation_get_rebel_controlled_count(owner) += uint16_t(1);
			if(!is_overseas(state, p)) {
				state.world.nation_get_central_rebel_controlled(owner) += uint16_t(1);
			}
		}
		if(owner && state.world.province_get_nation_from_province_control(p) == owner) {
			state.world.nation_get_occupied_count(owner) += uint16_t(1);
			if(state.world.province_get_is_blockaded(p) && !is_overseas(state, p)) {
				state.world.nation_get_central_blockaded(owner) -= uint16_t(1);
			}
		}
		state.world.province_set_rebel_faction_from_province_rebel_control(p, rf);
		state.world.province_set_nation_from_province_control(p, dcon::nation_id{});
		state.military_definitions.pending_blackflag_update = true;
	}
}

void restore_cached_values(sys::state& state) {
	
	state.world.execute_serial_over_nation([&](auto ids) { state.world.nation_set_central_province_count(ids, ve::int_vector()); });
	state.world.execute_serial_over_nation([&](auto ids) { state.world.nation_set_central_blockaded(ids, ve::int_vector()); });
	state.world.execute_serial_over_nation(
			[&](auto ids) { state.world.nation_set_central_rebel_controlled(ids, ve::int_vector()); });
	state.world.execute_serial_over_nation([&](auto ids) { state.world.nation_set_rebel_controlled_count(ids, ve::int_vector()); });
	state.world.execute_serial_over_nation([&](auto ids) { state.world.nation_set_central_ports(ids, ve::int_vector()); });
	state.world.execute_serial_over_nation([&](auto ids) { state.world.nation_set_central_crime_count(ids, ve::int_vector()); });
	state.world.execute_serial_over_nation([&](auto ids) { state.world.nation_set_total_ports(ids, ve::int_vector()); });
	state.world.execute_serial_over_nation([&](auto ids) { state.world.nation_set_occupied_count(ids, ve::int_vector()); });
	state.world.execute_serial_over_nation([&](auto ids) { state.world.nation_set_owned_state_count(ids, ve::int_vector()); });
	state.world.execute_serial_over_nation([&](auto ids) { state.world.nation_set_is_colonial_nation(ids, ve::mask_vector()); });

	// need to set owner cores first because capital selection depends on them

	for(auto n : state.world.in_nation) {
		auto orange = n.get_province_ownership();
		n.set_owned_province_count(uint16_t(orange.end() - orange.begin()));
	}

	for(int32_t i = 0; i < state.province_definitions.first_sea_province.index(); ++i) {
		dcon::province_id pid{dcon::province_id::value_base_t(i)};

		auto owner = state.world.province_get_nation_from_province_ownership(pid);
		if(owner) {
			bool owner_core = false;
			for(auto c : state.world.province_get_core(pid)) {
				if(c.get_identity().get_nation_from_identity_holder() == owner) {
					owner_core = true;
					break;
				}
			}
			state.world.province_set_is_owner_core(pid, owner_core);
		} else {
			state.world.province_set_is_owner_core(pid, false);
		}
	}

	for(auto n : state.world.in_nation) {
		if(n.get_capital().get_nation_from_province_ownership() != n) {
			n.set_capital(pick_capital(state, n));
		}
	}

	for(int32_t i = 0; i < state.province_definitions.first_sea_province.index(); ++i) {
		dcon::province_id pid{dcon::province_id::value_base_t(i)};

		auto owner = state.world.province_get_nation_from_province_ownership(pid);
		if(owner) {

			bool reb_controlled = bool(state.world.province_get_rebel_faction_from_province_rebel_control(pid));

			if(reb_controlled) {
				state.world.nation_get_rebel_controlled_count(owner) += uint16_t(1);
			}
			if(state.world.province_get_is_coast(pid)) {
				state.world.nation_get_total_ports(owner) += uint16_t(1);
			}
			if(auto c = state.world.province_get_nation_from_province_control(pid); bool(c) && c != owner) {
				state.world.nation_get_occupied_count(owner) += uint16_t(1);
			}
			if(state.world.province_get_is_colonial(pid)) {
				state.world.nation_set_is_colonial_nation(owner, true);
			}
			if(!is_overseas(state, pid)) {
				state.world.nation_get_central_province_count(owner) += uint16_t(1);

				if(military::province_is_blockaded(state, pid)) {
					state.world.nation_get_central_blockaded(owner) += uint16_t(1);
				}
				if(state.world.province_get_is_coast(pid)) {
					state.world.nation_get_central_ports(owner) += uint16_t(1);
				}
				if(reb_controlled) {
					state.world.nation_get_central_rebel_controlled(owner) += uint16_t(1);
				}
				if(state.world.province_get_crime(pid)) {
					state.world.nation_get_central_crime_count(owner) += uint16_t(1);
				}
			}
		}
	}
	state.world.for_each_state_instance([&](dcon::state_instance_id s) {
		auto owner = state.world.state_instance_get_nation_from_state_ownership(s);
		state.world.nation_get_owned_state_count(owner) += uint16_t(1);
		dcon::province_id p;
		for(auto prv : state.world.state_definition_get_abstract_state_membership(state.world.state_instance_get_definition(s))) {
			if(state.world.province_get_nation_from_province_ownership(prv.get_province()) == owner) {
				p = prv.get_province().id;
				break;
			}
		}
		state.world.state_instance_set_capital(s, p);
	});
}

void update_cached_values(sys::state& state) {
	if(!state.national_cached_values_out_of_date)
		return;

	state.national_cached_values_out_of_date = false;

	restore_cached_values(state);
}

void update_blockaded_cache(sys::state& state) {
	state.world.execute_serial_over_nation([&](auto ids) { state.world.nation_set_central_blockaded(ids, ve::int_vector()); });
	for(int32_t i = 0; i < state.province_definitions.first_sea_province.index(); ++i) {
		dcon::province_id pid{ dcon::province_id::value_base_t(i) };

		auto owner = state.world.province_get_nation_from_province_ownership(pid);
		if(owner) {
			if(!is_overseas(state, pid)) {
				if(military::province_is_blockaded(state, pid)) {
					state.world.nation_get_central_blockaded(owner) += uint16_t(1);
				}
			}
		}
	}
}

void restore_unsaved_values(sys::state& state) {
	for(int32_t i = 0; i < state.province_definitions.first_sea_province.index(); ++i) {
		dcon::province_id pid{dcon::province_id::value_base_t(i)};

		for(auto adj : state.world.province_get_province_adjacency(pid)) {
			if((state.world.province_adjacency_get_type(adj) & province::border::coastal_bit) != 0 &&
					(state.world.province_adjacency_get_type(adj) & province::border::impassible_bit) == 0) {
				state.world.province_set_is_coast(pid, true);
				break;
			}
		}
	}

	for(auto si : state.world.in_state_instance) {
		province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
			if(state.world.province_get_building_level(p, uint8_t(economy::province_building_type::naval_base)) > 0) {
				state.world.state_instance_set_naval_base_is_taken(si, true);
			} else {
				for(auto pc : state.world.province_get_province_building_construction(p)) {
					if(pc.get_type() == uint8_t(economy::province_building_type::naval_base))
						state.world.state_instance_set_naval_base_is_taken(si, true);
				}
			}
		});
	}
	military::update_blockade_status(state);
	restore_cached_values(state);
}

bool has_railroads_being_built(sys::state& state, dcon::province_id id) {
	for(auto pb : state.world.province_get_province_building_construction(id)) {
		if(economy::province_building_type(pb.get_type()) == economy::province_building_type::railroad)
			return true;
	}
	return false;
}

bool generic_can_build_railroads(sys::state& state, dcon::province_id id, dcon::nation_id n) {
	if(n != state.world.province_get_nation_from_province_control(id))
		return false;
	if(military::province_is_under_siege(state, id))
		return false;

	int32_t current_rails_lvl = state.world.province_get_building_level(id, uint8_t(economy::province_building_type::railroad));
	int32_t max_local_rails_lvl = state.world.nation_get_max_building_level(n, uint8_t(economy::province_building_type::railroad));
	int32_t min_build_railroad =
			int32_t(state.world.province_get_modifier_values(id, sys::provincial_mod_offsets::min_build_railroad));

	return (max_local_rails_lvl - current_rails_lvl - min_build_railroad > 0) && !has_railroads_being_built(state, id);
}
bool can_build_railroads(sys::state& state, dcon::province_id id, dcon::nation_id n) {
	auto owner = state.world.province_get_nation_from_province_ownership(id);

	if(owner != state.world.province_get_nation_from_province_control(id))
		return false;
	if(military::province_is_under_siege(state, id))
		return false;

	if(owner != n) {
		if(state.world.nation_get_is_great_power(n) == false || state.world.nation_get_is_great_power(owner) == true)
			return false;
		if(state.world.nation_get_is_civilized(owner) == false)
			return false;

		auto rules = state.world.nation_get_combined_issue_rules(owner);
		if((rules & issue_rule::allow_foreign_investment) == 0)
			return false;

		if(military::are_at_war(state, n, owner))
			return false;
	} else {
		auto rules = state.world.nation_get_combined_issue_rules(n);
		if((rules & issue_rule::build_railway) == 0)
			return false;
	}

	int32_t current_rails_lvl = state.world.province_get_building_level(id, uint8_t(economy::province_building_type::railroad));
	int32_t max_local_rails_lvl = state.world.nation_get_max_building_level(n, uint8_t(economy::province_building_type::railroad));
	int32_t min_build_railroad =
			int32_t(state.world.province_get_modifier_values(id, sys::provincial_mod_offsets::min_build_railroad));

	return (max_local_rails_lvl - current_rails_lvl - min_build_railroad > 0) && !has_railroads_being_built(state, id);
}
bool has_fort_being_built(sys::state& state, dcon::province_id id) {
	for(auto pb : state.world.province_get_province_building_construction(id)) {
		if(economy::province_building_type(pb.get_type()) == economy::province_building_type::fort)
			return true;
	}
	return false;
}
bool can_build_fort(sys::state& state, dcon::province_id id, dcon::nation_id n) {
	if(state.world.province_get_nation_from_province_ownership(id) != n)
		return false;
	if(state.world.province_get_nation_from_province_ownership(id) != state.world.province_get_nation_from_province_control(id))
		return false;
	if(military::province_is_under_siege(state, id))
		return false;

	int32_t current_lvl = state.world.province_get_building_level(id, uint8_t(economy::province_building_type::fort));
	int32_t max_local_lvl = state.world.nation_get_max_building_level(n, uint8_t(economy::province_building_type::fort));
	int32_t min_build = int32_t(state.world.province_get_modifier_values(id, sys::provincial_mod_offsets::min_build_fort));

	return (max_local_lvl - current_lvl - min_build > 0) && !has_fort_being_built(state, id);
}
bool has_naval_base_being_built(sys::state& state, dcon::province_id id) {
	for(auto pb : state.world.province_get_province_building_construction(id)) {
		if(economy::province_building_type(pb.get_type()) == economy::province_building_type::naval_base)
			return true;
	}
	return false;
}
bool can_build_naval_base(sys::state& state, dcon::province_id id, dcon::nation_id n) {
	if(state.world.province_get_nation_from_province_ownership(id) != n)
		return false;
	if(state.world.province_get_is_coast(id) == false)
		return false;
	if(state.world.province_get_nation_from_province_ownership(id) != state.world.province_get_nation_from_province_control(id))
		return false;
	if(military::province_is_under_siege(state, id))
		return false;

	auto si = state.world.province_get_state_membership(id);

	int32_t current_lvl = state.world.province_get_building_level(id, uint8_t(economy::province_building_type::naval_base));
	int32_t max_local_lvl = state.world.nation_get_max_building_level(n, uint8_t(economy::province_building_type::naval_base));
	int32_t min_build = int32_t(state.world.province_get_modifier_values(id, sys::provincial_mod_offsets::min_build_naval_base));

	return (max_local_lvl - current_lvl - min_build > 0) && (current_lvl > 0 || !si.get_naval_base_is_taken()) && !has_naval_base_being_built(state, id);
}

bool has_province_building_being_built(sys::state& state, dcon::province_id id, economy::province_building_type t) {
	for(auto pb : state.world.province_get_province_building_construction(id)) {
		if(economy::province_building_type(pb.get_type()) == t)
			return true;
	}
	return false;
}

bool can_build_province_building(sys::state& state, dcon::province_id id, dcon::nation_id n, economy::province_building_type t) {
	auto owner = state.world.province_get_nation_from_province_ownership(id);

	if(owner != state.world.province_get_nation_from_province_control(id))
		return false;
	if(military::province_is_under_siege(state, id))
		return false;

	if(owner != n) {
		if(state.world.nation_get_is_great_power(n) == false || state.world.nation_get_is_great_power(owner) == true)
			return false;
		if(state.world.nation_get_is_civilized(owner) == false)
			return false;

		auto rules = state.world.nation_get_combined_issue_rules(owner);
		if((rules & issue_rule::allow_foreign_investment) == 0)
			return false;

		if(military::are_at_war(state, n, owner))
			return false;
	} else {
		auto rules = state.world.nation_get_combined_issue_rules(n);
		if(t == economy::province_building_type::bank && (rules & issue_rule::build_bank) == 0)
			return false;
		if(t == economy::province_building_type::university && (rules & issue_rule::build_university) == 0)
			return false;
	}

	int32_t current_lvl = state.world.province_get_building_level(id, uint8_t(t));
	int32_t max_local_lvl = state.world.nation_get_max_building_level(n, uint8_t(t));
	int32_t min_build = int32_t(state.world.province_get_modifier_values(id, sys::provincial_mod_offsets::min_build_railroad));
	return (max_local_lvl - current_lvl - min_build > 0) && !has_railroads_being_built(state, id);
}

bool has_an_owner(sys::state& state, dcon::province_id id) {
	return bool(dcon::fatten(state.world, id).get_nation_from_province_ownership());
}

float state_accepted_bureaucrat_size(sys::state& state, dcon::state_instance_id id) {
	float bsum = 0.f;
	for_each_province_in_state_instance(state, id, [&](dcon::province_id p) {
		for(auto po : state.world.province_get_pop_location(p)) {
			if(po.get_pop().get_is_primary_or_accepted_culture() &&
					po.get_pop().get_poptype() == state.culture_definitions.bureaucrat) {
				bsum += po.get_pop().get_size();
			}
		}
	});
	return bsum;
}

/* Vanilla State Admin efficiency: used for integrating colonial states and thus still counts only accepted/primary culture bureaucrats */
float state_admin_efficiency(sys::state& state, dcon::state_instance_id id) {
	// unused

	auto owner = state.world.state_instance_get_nation_from_state_ownership(id);

	auto admin_mod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::administrative_efficiency_modifier);

	float issue_sum = 0.0f;
	for(auto i : state.culture_definitions.social_issues) {
		issue_sum += state.world.issue_option_get_administrative_multiplier(state.world.nation_get_issues(owner, i));
	}
	auto from_issues = issue_sum * state.defines.bureaucracy_percentage_increment + state.defines.max_bureaucracy_percentage;
	float side_effects = 0.0f;
	float bsum = 0.0f;
	for_each_province_in_state_instance(state, id, [&](dcon::province_id p) {
		if(!state.world.province_get_is_owner_core(p)) {
			side_effects += state.defines.noncore_tax_penalty;
		}
		if(state.world.province_get_nationalism(p) > 0.f) {
			side_effects += state.defines.separatism_tax_penalty;
		}
		for(auto po : state.world.province_get_pop_location(p)) {
			if(po.get_pop().get_is_primary_or_accepted_culture() &&
					po.get_pop().get_poptype() == state.culture_definitions.bureaucrat && po.get_pop().get_rebel_faction_from_pop_rebellion_membership()) {
				bsum += po.get_pop().get_size();
			}
		}
	});
	auto total_pop = state.world.state_instance_get_demographics(id, demographics::total);
	auto total =
			total_pop > 0
					? std::clamp(admin_mod + side_effects + state.defines.base_country_admin_efficiency +
													 std::min(state.culture_definitions.bureaucrat_tax_efficiency * bsum / total_pop, 1.0f) / from_issues,
								0.0f, 1.0f)
					: 0.0f;

	return total;
}
float crime_fighting_efficiency(sys::state& state, dcon::province_id id) {
	// TODO
	/*
	Crime is apparently the single place where the following value matters:
	- state administrative efficiency: = define:NONCORE_TAX_PENALTY x number-of-non-core-provinces + (bureaucrat-tax-efficiency x
	total-number-of-primary-or-accepted-culture-bureaucrats / population-of-state)v1 / x
	(sum-of-the-administrative_multiplier-for-social-issues-marked-as-being-administrative x
	define:BUREAUCRACY_PERCENTAGE_INCREMENT + define:MAX_BUREAUCRACY_PERCENTAGE)), all clamped between 0 and 1. The crime fighting
	percent of a province is then calculated as: (state-administration-efficiency x define:ADMIN_EFFICIENCY_CRIMEFIGHT_PERCENT +
	administration-spending-setting x (1 - ADMIN_EFFICIENCY_CRIMEFIGHT_PERCENT)) x (define:MAX_CRIMEFIGHTING_PERCENT -
	define:MIN_CRIMEFIGHTING_PERCENT) + define:MIN_CRIMEFIGHTING_PERCENT
	*/
	// we have agreed to replace admin spending with national admin efficiency

	// changed to local control/enforcement ratio

	auto si = state.world.province_get_state_membership(id);
	auto owner = state.world.province_get_nation_from_province_ownership(id);
	if(si && owner)
		return state.world.province_get_control_ratio(id)
			* (1.f + state.world.nation_get_administrative_efficiency(owner))
			* (
				state.defines.max_crimefight_percent
				- state.defines.min_crimefight_percent
			)
			+
			state.defines.min_crimefight_percent;
	else
		return 0.0f;
}
float revolt_risk(sys::state& state, dcon::province_id id) {
	auto total_pop = state.world.province_get_demographics(id, demographics::total);
	if(total_pop == 0) {
		return 0;
	}

	auto militancy = state.world.province_get_demographics(id, demographics::militancy);
	return militancy / total_pop;
}

dcon::province_id get_connected_province(sys::state& state, dcon::province_adjacency_id adj, dcon::province_id curr) {
	auto first = state.world.province_adjacency_get_connected_provinces(adj, 0);
	if(first == curr) {
		return state.world.province_adjacency_get_connected_provinces(adj, 1);
	} else {
		return first;
	}
}

struct queue_node {
	float priority;
	dcon::province_id prov_id;
};

float state_distance(sys::state& state, dcon::state_instance_id state_id, dcon::province_id prov_id) {
	return direct_distance(state, state.world.state_instance_get_capital(state_id), prov_id);
}

float state_sorting_distance(sys::state& state, dcon::state_instance_id state_id, dcon::province_id prov_id) {
	return sorting_distance(state, state.world.state_instance_get_capital(state_id), prov_id);
}

bool can_integrate_colony(sys::state& state, dcon::state_instance_id id) {
	if(state.world.state_instance_get_capital(id).get_is_colonial() == false)
		return false;

	auto dkey = demographics::to_key(state, state.culture_definitions.bureaucrat);
	auto bureaucrat_size = state_accepted_bureaucrat_size(state, id);
	auto total_size = state.world.state_instance_get_demographics(id, demographics::total);
	if(bureaucrat_size / total_size >= state.defines.state_creation_admin_limit) {
		auto owner = state.world.state_instance_get_nation_from_state_ownership(id);
		auto cost = colony_integration_cost(state, id);
		return cost == 0.0f || cost <= nations::free_colonial_points(state, owner);
	} else {
		return false;
	}
}

float colony_integration_cost(sys::state& state, dcon::state_instance_id id) {
	/*
	The point requirement is: define:COLONIZATION_CREATE_STATE_COST x number of provinces x 1v(either distance to capital /
	COLONIZATION_COLONY_STATE_DISTANCE or 0 if it has a land connection to the capital).
	*/
	bool entirely_overseas = true;
	float prov_count = 0.0f;
	for_each_province_in_state_instance(state, id, [&](dcon::province_id prov) {
		entirely_overseas &= is_overseas(state, prov);
		prov_count++;
	});
	if(entirely_overseas) {
		auto owner = state.world.state_instance_get_nation_from_state_ownership(id);
		float distance = state_distance(state, id, state.world.nation_get_capital(owner).id);
		return state.defines.colonization_create_state_cost * prov_count * std::max(distance / state.defines.colonization_colony_state_distance, 1.0f);
	} else {
		return 0.0f;
	}
}

void upgrade_colonial_state(sys::state& state, dcon::nation_id source, dcon::state_instance_id si) {
	province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
		// Provinces in the state stop being colonial.
		state.world.province_set_is_colonial(p, false);

		// All timed modifiers active for provinces in the state expire
		auto timed_modifiers = state.world.province_get_current_modifiers(p);
		for(uint32_t i = timed_modifiers.size(); i-- > 0;) {
			if(bool(timed_modifiers[i].expiration)) {
				timed_modifiers.remove_at(i);
			}
		}
	});

	// Gain define:COLONY_TO_STATE_PRESTIGE_GAIN x(1.0 + colony - prestige - from - tech) x(1.0 + prestige - from - tech)
	nations::adjust_prestige(state, source,
			state.defines.colony_to_state_prestige_gain *
					(1.0f + state.world.nation_get_modifier_values(source, sys::national_mod_offsets::colonial_prestige)));

	// An event from `on_colony_to_state` happens(with the state in scope)
	event::fire_fixed_event(state, state.national_definitions.on_colony_to_state, trigger::to_generic(si), event::slot_type::state,
			source, -1, event::slot_type::none);

	// An event from `on_colony_to_state_free_slaves` happens(with the state in scope)
	event::fire_fixed_event(state, state.national_definitions.on_colony_to_state_free_slaves, trigger::to_generic(si),
			event::slot_type::state, source, -1, event::slot_type::none);

	// Update is colonial nation
	state.world.nation_set_is_colonial_nation(source, false);
	for(auto p : state.world.nation_get_province_ownership(source)) {
		if(p.get_province().get_is_colonial()) {
			state.world.nation_set_is_colonial_nation(source, true);
			return;
		}
	}
}

void change_province_owner(sys::state& state, dcon::province_id id, dcon::nation_id new_owner) {
	auto state_def = state.world.province_get_state_from_abstract_state_membership(id);
	auto old_si = state.world.province_get_state_membership(id);
	auto old_market = state.world.state_instance_get_market_from_local_market(old_si);
	auto old_owner = state.world.province_get_nation_from_province_ownership(id);

	if(new_owner == old_owner)
		return;

	state.adjacency_data_out_of_date = true;
	state.national_cached_values_out_of_date = true;

	bool state_is_new = false;
	dcon::state_instance_id new_si;

	auto pmods = state.world.province_get_current_modifiers(id);
	pmods.clear();

	bool will_be_colonial = state.world.province_get_is_colonial(id)
		|| (old_owner && state.world.nation_get_is_civilized(old_owner) == false
		&& state.world.nation_get_is_civilized(new_owner) == true)
		|| (!old_owner);
	if(old_si) {
		if(state.world.province_get_building_level(id, uint8_t(economy::province_building_type::naval_base)) > 0) {
			state.world.state_instance_set_naval_base_is_taken(old_si, false);
		} else {
			for(auto pc : state.world.province_get_province_building_construction(id)) {
				if(pc.get_type() == uint8_t(economy::province_building_type::naval_base)) {
					state.world.state_instance_set_naval_base_is_taken(old_si, false);
				}
			}
		}
	}

	if(new_owner) {
		for(auto si : state.world.nation_get_state_ownership(new_owner)) {
			if(si.get_state().get_definition().id == state_def) {
				new_si = si.get_state().id;
				break;
			}
		}
		bool was_slave_state = !old_owner || state.world.province_get_is_slave(id);
		if(!new_si) {
			new_si = state.world.create_state_instance();
			state.world.state_instance_set_definition(new_si, state_def);
			state.world.try_create_state_ownership(new_si, new_owner);

			// sanity check
			auto owner_has_administration = false;
			state.world.nation_for_each_nation_administration(new_owner, [&](auto id) { owner_has_administration = true; });

			if(owner_has_administration) {
				auto new_owner_capital = state.world.nation_get_capital(new_owner);
				auto capital_sid = state.world.province_get_state_membership(new_owner_capital);
				auto new_administration = state.world.state_instance_get_administration_from_local_administration(capital_sid);
				state.world.force_create_local_administration(new_si, new_administration);
			}
			// if nation does not have a central admin,
			// it doesn't have a capital either,
			// so we don't create central admin here to avoid out of bounds errors later			

			state.world.state_instance_set_capital(new_si, id);
			state.world.province_set_is_colonial(id, will_be_colonial);
			state.world.province_set_is_slave(id, false);
			if(will_be_colonial)
				state.world.nation_set_is_colonial_nation(new_owner, true);
			if(state.world.province_get_building_level(id, uint8_t(economy::province_building_type::naval_base)) > 0)
				state.world.state_instance_set_naval_base_is_taken(new_si, true);

			auto new_market = state.world.create_market();

			// set prices to something to avoid infinite demand:
			if(old_market) {
				state.world.for_each_commodity([&](auto cid){
					state.world.market_set_price(new_market, cid, state.world.market_get_price(old_market, cid));
				});
			} else {
				state.world.for_each_commodity([&](auto cid) {
					state.world.market_set_price(new_market, cid, economy::median_price(state, cid));
				});
			}

			auto new_local_market = state.world.force_create_local_market(new_market, new_si);

			// new state, new trade routes

			std::set<dcon::state_instance_id::value_base_t> trade_route_candidates{};

			// try to create trade routes to neighbors
			for(auto adj : state.world.province_get_province_adjacency(id)) {
				if((adj.get_type() & (province::border::impassible_bit | province::border::coastal_bit)) == 0) {
					auto other =
						adj.get_connected_provinces(0) != id
						? adj.get_connected_provinces(0)
						: adj.get_connected_provinces(1);

					if(!other.get_state_membership())
						continue;
					if(other.get_state_membership() == new_si)
						continue;
					if(trade_route_candidates.contains(other.get_state_membership().id.value))
						continue;

					trade_route_candidates.insert(other.get_state_membership().id.value);
				}
			}

			for(auto candidate_trade_partner_val : trade_route_candidates) {
				auto si = dcon::state_instance_id{ uint16_t(candidate_trade_partner_val - 1) };
				auto target_market = state.world.state_instance_get_market_from_local_market(si);
				auto new_route = state.world.force_create_trade_route(new_market, target_market);
				state.world.trade_route_set_land_distance(new_route, 99999.f);
				state.world.trade_route_set_sea_distance(new_route, 99999.f);
				if(province::state_is_coastal(state, new_si) && province::state_is_coastal(state, si)) {
					state.world.trade_route_set_is_sea_route(new_route, true);
				}
				state.world.trade_route_set_is_land_route(new_route, true);
			}

			state_is_new = true;
		} else {
			auto sc = state.world.state_instance_get_capital(new_si);
			state.world.province_set_is_colonial(id, state.world.province_get_is_colonial(sc));
			state.world.province_set_is_slave(id, state.world.province_get_is_slave(sc));
			if(state.world.province_get_building_level(id, uint8_t(economy::province_building_type::naval_base)) > 0) {
				if(state.world.state_instance_get_naval_base_is_taken(new_si)) {
					state.world.province_set_building_level(id, uint8_t(economy::province_building_type::naval_base), 0);
				} else {
					state.world.state_instance_set_naval_base_is_taken(new_si, true);
				}
			}

			//new province can potentially unlock new trade routes


			// save old trade routes
			auto market = state.world.state_instance_get_market_from_local_market(new_si);
			std::set<dcon::state_instance_id::value_base_t> old_trade_routes;
			for(auto item : state.world.market_get_trade_route(market)) {
				auto other =
					item.get_connected_markets(0) != market
					? item.get_connected_markets(0)
					: item.get_connected_markets(1);

				old_trade_routes.insert(other.get_zone_from_local_market().id.value);
			}

			std::set<dcon::state_instance_id::value_base_t> trade_route_candidates{};

			// try to create trade routes to neighbors
			for(auto adj : state.world.province_get_province_adjacency(id)) {
				if((adj.get_type() & (province::border::impassible_bit | province::border::coastal_bit)) == 0) {
					auto other =
						adj.get_connected_provinces(0) != id
						? adj.get_connected_provinces(0)
						: adj.get_connected_provinces(1);

					if(!other.get_state_membership())
						continue;
					if(other.get_state_membership() == new_si)
						continue;
					if(old_trade_routes.contains(other.get_state_membership().id.value))
						continue;
					if(trade_route_candidates.contains(other.get_state_membership().id.value))
						continue;

					trade_route_candidates.insert(other.get_state_membership().id.value);
				}
			}

			for(auto candidate_trade_partner_val : trade_route_candidates) {
				auto si = dcon::state_instance_id{ uint16_t(candidate_trade_partner_val - 1) };
				auto target_market = state.world.state_instance_get_market_from_local_market(si);
				auto new_route = state.world.force_create_trade_route(market, target_market);
				state.world.trade_route_set_land_distance(new_route, 99999.f);
				state.world.trade_route_set_sea_distance(new_route, 99999.f);
				if(province::state_is_coastal(state, new_si) && province::state_is_coastal(state, si)) {
					state.world.trade_route_set_is_sea_route(new_route, true);
				}
				state.world.trade_route_set_is_land_route(new_route, true);
			}
		}
		if(was_slave_state) {
			culture::fix_slaves_in_province(state, new_owner, id);
		}

		auto province_fac_range = state.world.province_get_factory_location(id);
		int32_t factories_in_province = int32_t(province_fac_range.end() - province_fac_range.begin());

		int32_t factories_in_new_state = 0;
		province::for_each_province_in_state_instance(state, new_si, [&](dcon::province_id pr) {
			auto fac_range = state.world.province_get_factory_location(pr);
			// Merge factories and accumulate levels of merged factories
			for(const auto pfac : province_fac_range) {
				for(int32_t i = 0; i < int32_t(fac_range.end() - fac_range.begin()); i++) {
					const auto fac = *(fac_range.begin() + i);
					if(fac.get_factory().get_building_type() == pfac.get_factory().get_building_type()) {
						float old_size = pfac.get_factory().get_size();
						float add_size = fac.get_factory().get_size();
						pfac.get_factory().set_size(old_size + add_size);
						state.world.delete_factory(fac.get_factory().id);
						--i;
					}
				}
			}
			factories_in_new_state += int32_t(fac_range.end() - fac_range.begin());
		});

		auto excess_factories = std::min((factories_in_new_state + factories_in_province) - int32_t(state.defines.factories_per_state), factories_in_province);
		while(excess_factories > 0) {
			state.world.delete_factory((*(province_fac_range.begin() + excess_factories - 1)).get_factory().id);
			--excess_factories;
		}

		state.world.province_set_state_membership(id, new_si);

		for(auto p : state.world.province_get_pop_location(id)) {
			[&]() {
				if(state.world.nation_get_primary_culture(new_owner) == p.get_pop().get_culture()) {
					p.get_pop().set_is_primary_or_accepted_culture(true);
					return;
				}
				if(state.world.nation_get_accepted_cultures(new_owner, p.get_pop().get_culture())) {
					p.get_pop().set_is_primary_or_accepted_culture(true);
					return;
				}
				p.get_pop().set_is_primary_or_accepted_culture(false);
			}();
		}
		state.world.nation_get_owned_province_count(new_owner) += uint16_t(1);
	} else {
		state.world.province_set_state_membership(id, dcon::state_instance_id{});
		for(auto t = economy::province_building_type::railroad; t != economy::province_building_type::last; t = economy::province_building_type(uint8_t(t) + 1)) {
			state.world.province_set_building_level(id, uint8_t(t), uint8_t(0));
		}

		auto province_fac_range = state.world.province_get_factory_location(id);
		while(province_fac_range.begin() != province_fac_range.end()) {
			state.world.delete_factory((*province_fac_range.begin()).get_factory().id);
		}

		for(auto p : state.world.province_get_pop_location(id)) {
			p.get_pop().set_is_primary_or_accepted_culture(false);
		}
	}

	
	if(new_owner) {
		for(auto p : state.world.province_get_pop_location(id)) {
			rebel::remove_pop_from_movement(state, p.get_pop());
			rebel::remove_pop_from_rebel_faction(state, p.get_pop());
			for(const auto src : p.get_pop().get_regiment_source()) {
				if(!src.get_regiment().get_army_from_army_membership().get_is_retreating()
				&& !src.get_regiment().get_army_from_army_membership().get_navy_from_army_transport()
				&& !src.get_regiment().get_army_from_army_membership().get_battle_from_army_battle_participation()
				&& !src.get_regiment().get_army_from_army_membership().get_controller_from_army_rebel_control()) {
					auto loc = src.get_regiment().get_army_from_army_membership().get_location_from_army_location();
					auto new_u = fatten(state.world, state.world.create_army());
					new_u.set_controller_from_army_control(new_owner);
					src.get_regiment().set_army_from_army_membership(new_u);
					src.get_regiment().set_org(0.01f);
					military::army_arrives_in_province(state, new_u, loc, military::crossing_type::none);
				} else {
					src.get_regiment().set_strength(0.f);
				}
			}
			auto lc = p.get_pop().get_province_land_construction();
			while(lc.begin() != lc.end()) {
				state.world.delete_province_land_construction(*(lc.begin()));
			}
		}
	}

	state.world.province_set_nation_from_province_ownership(id, new_owner);
	state.world.province_set_rebel_faction_from_province_rebel_control(id, dcon::rebel_faction_id{});
	state.world.province_set_last_control_change(id, state.current_date);
	state.world.province_set_nation_from_province_control(id, new_owner);
	state.world.province_set_siege_progress(id, 0.0f);
	state.world.province_set_control_ratio(id, 0.f);
	state.world.province_set_control_scale(id, 0.f);

	military::eject_ships(state, id);
	military::update_blackflag_status(state, id);

	state.world.province_set_is_owner_core(id,
		bool(state.world.get_core_by_prov_tag_key(id, state.world.nation_get_identity_from_identity_holder(new_owner))));

	if(old_si) {
		dcon::province_id a_province;
		province::for_each_province_in_state_instance(state, old_si, [&](auto p) { a_province = p; });
		if(!a_province) {

			if(state.crisis_attacker_wargoals.size() > 0) {
				auto first_wg = state.crisis_attacker_wargoals.at(0);
				if(old_si.get_definition() == first_wg.state)
					nations::cleanup_crisis(state);
			}
			auto local_market = state.world.state_instance_get_market_from_local_market(old_si);

			auto local_administration = state.world.state_instance_get_administration_from_local_administration(old_si);
			state.world.delete_market(local_market);
			state.world.delete_state_instance(old_si);

			if(local_administration) {
				// count states in local administration
				bool more_than_zero = false;
				state.world.administration_for_each_local_administration(local_administration, [&](auto ids) {
					more_than_zero = true;
				});
				if(!more_than_zero) {
					state.world.delete_administration(local_administration);
				}
			}

		} else if(state.world.state_instance_get_capital(old_si) == id) {
			state.world.state_instance_set_capital(old_si, a_province);
		}
	}

	if(old_owner) {
		state.world.nation_get_owned_province_count(old_owner) -= uint16_t(1);
		auto lprovs = state.world.nation_get_province_ownership(old_owner);
		if(lprovs.begin() == lprovs.end()) {
			state.world.nation_set_marked_for_gc(old_owner, true);
		}
	}

	// remove rally points
	state.world.province_set_naval_rally_point(id, false);
	state.world.province_set_land_rally_point(id, false);

	// update admin capitals of the victim nation
	// for now delete admin regions if they had lost their capital
	{
		std::vector<dcon::administration_id> to_delete{};
		state.world.nation_for_each_nation_administration(old_owner, [&](auto nadid) {
			auto adid = state.world.nation_administration_get_administration(nadid);
			auto capital = state.world.administration_get_capital(adid);
			auto current_owner_of_capital = state.world.province_get_nation_from_province_ownership(capital);
			if(current_owner_of_capital != old_owner) {
				to_delete.push_back(adid);
			}
		});
		for(auto deleted : to_delete) {
			state.world.delete_administration(deleted);
		}
	}

	// cancel constructions

	{
		auto rng = state.world.province_get_province_building_construction(id);
		while(rng.begin() != rng.end()) {
			state.world.delete_province_building_construction(*(rng.begin()));
		}
	}

	{
		auto rng = state.world.province_get_province_naval_construction(id);
		while(rng.begin() != rng.end()) {
			state.world.delete_province_naval_construction(*(rng.begin()));
		}
	}

	for(auto adj : state.world.province_get_province_adjacency(id)) {
		auto other = adj.get_connected_provinces(0) != id ? adj.get_connected_provinces(0) : adj.get_connected_provinces(1);
		if(other.get_nation_from_province_ownership() == new_owner) {
			adj.set_type(adj.get_type() & ~province::border::national_bit);
		} else {
			adj.set_type(adj.get_type() | province::border::national_bit);
		}
	}

	/* Properly cleanup rebels when the province ownership changes */
	for(auto ar : state.world.province_get_army_location_as_location(id)) {
		if(ar.get_army() && ar.get_army().get_army_rebel_control().get_controller()) {
			assert(!ar.get_army().get_army_control().get_controller());
			state.world.army_set_controller_from_army_control(ar.get_army(), dcon::nation_id{});
			state.world.army_set_controller_from_army_rebel_control(ar.get_army(), dcon::rebel_faction_id{});
			state.world.army_set_is_retreating(ar.get_army(), true);
		}
	}

	if(state_is_new && old_owner) {
		/*
		spawn event
		*/
		event::fire_fixed_event(state, state.national_definitions.on_state_conquest, trigger::to_generic(new_si),
				event::slot_type::state, new_owner, -1, event::slot_type::none);
	}
}
// returns true if a strait between the two provinces are blocked by an enemy navy from the perspective of thisnation
// Reads sea adjacency data from the v2 adjacencies file to determine if it is blocked
bool is_strait_blocked(sys::state& state, dcon::nation_id thisnation, dcon::province_id from, dcon::province_id to) {
	auto adjacency = state.world.get_province_adjacency_by_province_pair(to, from);
	return is_strait_blocked(state, thisnation, adjacency);
}

bool is_strait_blocked(sys::state& state, dcon::nation_id thisnation, dcon::province_adjacency_id adjacency) {
	auto path_bits = state.world.province_adjacency_get_type(adjacency);
 	auto strait_prov = state.world.province_adjacency_get_sea_adj_prov(adjacency);
	if((path_bits & province::border::non_adjacent_bit) != 0 && strait_prov) { // strait crossing
		if(military::province_has_enemy_fleet(state, strait_prov, thisnation)) {
			return true;
		}
	}
	return false;
}

void conquer_province(sys::state& state, dcon::province_id id, dcon::nation_id new_owner) {
	bool was_colonial = state.world.province_get_is_colonial(id);
	change_province_owner(state, id, new_owner);

	/*
	- The conqueror may gain research points:
	First, figure out how many research points the pops in the province would generate as if they were a tiny nation (i.e. for
	each pop type that generates research points, multiply that number by the fraction of the population it is compared to its
	optimal fraction (capped at one) and sum them all together). Then multiply that value by (1.0 + national modifier to research
	points modifier + tech increase research modifier). That value is then multiplied by define:RESEARCH_POINTS_ON_CONQUER_MULT
	and added to the conquering nation's research points. Ok, so what about the nations research points on conquer modifier??
	Yeah, that appears to be bugged. The nation gets research points only if that multiplier is positive, but otherwise it doesn't
	affect the result.
	*/

	if(!state.world.nation_get_is_civilized(new_owner)) {
		auto rp_mod_mod = 0.75f + state.world.nation_get_modifier_values(new_owner, sys::national_mod_offsets::research_points_modifier);

		float sum_from_pops = 0;
		float total_pop = state.world.province_get_demographics(id, demographics::total);
		state.world.for_each_pop_type([&](dcon::pop_type_id t) {
			auto rp = state.world.pop_type_get_research_points(t);
			if(rp > 0) {
				sum_from_pops += rp * std::min(1.0f, state.world.province_get_demographics(id, demographics::to_key(state, t)) / (total_pop * state.world.pop_type_get_research_optimum(t)));
			}
		});

		auto amount = total_pop > 0.0f ? (state.defines.research_points_on_conquer_mult * sum_from_pops) * (rp_mod_mod + 1.0f) : 0.0f;
		state.world.nation_get_research_points(new_owner) += amount;
	}

	/*
	- If the province is not a core of the new owner and is not a colonial province (prior to conquest), any pops that are not of
	an accepted or primary culture get define:MIL_HIT_FROM_CONQUEST militancy
	*/
	if(state.world.province_get_is_owner_core(id) == false && !was_colonial) {
		for(auto pop : state.world.province_get_pop_location(id)) {
			if(!pop.get_pop().get_is_primary_or_accepted_culture()) {
				pop_demographics::set_militancy(state, pop.get_pop(), std::clamp(pop_demographics::get_militancy(state, pop.get_pop()) + state.defines.mil_hit_from_conquest, 0.0f, 10.0f));
			}
		}
	}

	/*
	- The province gets nationalism equal to define:YEARS_OF_NATIONALISM
	*/
	state.world.province_set_nationalism(id, state.defines.years_of_nationalism);
}

void update_nationalism(sys::state& state) {
	province::ve_for_each_land_province(state, [&](auto ids) {
		auto old_n = state.world.province_get_nationalism(ids);
		auto new_nat = ve::max(old_n - 0.083f, 0.0f);
		state.world.province_set_nationalism(ids, new_nat);
	});
}

void update_crimes(sys::state& state) {
	for_each_land_province(state, [&](dcon::province_id p) {
		auto owner = state.world.province_get_nation_from_province_ownership(p);
		if(!owner)
			return;

		/*
		Once per month (the 1st) province crimes are updated. If the province has a crime, the crime fighting percent is the
		probability of that crime being removed. If there is no crime, the crime fighting percent is the probability that it will
		remain crime free. If a crime is added to the province, it is selected randomly (with equal probability) from the crimes
		that are possible for the province (determined by the crime being activated and its trigger passing).
		*/

		auto chance = uint32_t(province::crime_fighting_efficiency(state, p) * 256.0f);
		auto rvalues = rng::get_random_pair(state, uint32_t((p.index() << 2) + 1));
		if((rvalues.high & 0xFF) <= chance) {
			if(state.world.province_get_crime(p)) {
				if(!province::is_overseas(state, p))
					state.world.nation_get_central_crime_count(owner) -= uint16_t(1);
			}
			state.world.province_set_crime(p, dcon::crime_id{});
		} else {
			if(!state.world.province_get_crime(p)) {
				static std::vector<dcon::crime_id> possible_crimes;
				possible_crimes.clear();

				for(uint32_t i = 0; i < state.culture_definitions.crimes.size(); ++i) {
					dcon::crime_id c{dcon::crime_id::value_base_t(i)};
					if(state.culture_definitions.crimes[c].available_by_default || state.world.nation_get_active_crime(owner, c)) {
						if(auto t = state.culture_definitions.crimes[c].trigger; t) {
							if(trigger::evaluate(state, t, trigger::to_generic(p), trigger::to_generic(owner), 0))
								possible_crimes.push_back(c);
						} else {
							possible_crimes.push_back(c);
						}
					}
				}

				if(auto count = possible_crimes.size(); count != 0) {
					auto selected = possible_crimes[rvalues.low % count];
					state.world.province_set_crime(p, selected);
					if(!province::is_overseas(state, p))
						state.world.nation_get_central_crime_count(owner) += uint16_t(1);
				}
			}
		}
	});
}

bool is_colonizing(sys::state& state, dcon::nation_id n, dcon::state_definition_id d) {
	for(auto rel : state.world.state_definition_get_colonization(d)) {
		if(rel.get_colonizer() == n) {
			return true;
		}
	}
	return false;
}

bool can_invest_in_colony(sys::state& state, dcon::nation_id n, dcon::state_definition_id d) {
	// Your country must be of define:COLONIAL_RANK or less.
	if(state.world.nation_get_rank(n) > uint16_t(state.defines.colonial_rank))
		return false; // too low rank to colonize;

	if(state.world.state_definition_get_colonization_stage(d) > uint8_t(2))
		return false; // too late

	// The state may not be the current target of a crisis, nor may your country be involved in an active crisis war.

	auto first_wg = state.crisis_attacker_wargoals.at(0);
	if(first_wg.state == d)
		return false;
	for(auto par : state.world.war_get_war_participant(state.crisis_war)) {
		if(par.get_nation() == n)
			return false;
	}

	dcon::colonization_id colony_status;
	auto crange = state.world.state_definition_get_colonization(d);
	for(auto rel : crange) {
		if(rel.get_colonizer() == n) {
			colony_status = rel.id;
			break;
		}
	}

	if(!colony_status)
		return false;
	if(crange.end() - crange.begin() <= 1) // no competition
		return false;

	/*
	If you have put a colonist in the region, and colonization is in phase 1 or 2, you can invest if it has been at least
	define:COLONIZATION_DAYS_BETWEEN_INVESTMENT since your last investment, and you have enough free colonial points.
	*/

	if(state.world.colonization_get_last_investment(colony_status) + int32_t(state.defines.colonization_days_between_investment) > state.current_date)
		return false;

	/*
	Steps cost define:COLONIZATION_INTEREST_COST while in phase 1. In phase two, each point of investment cost
	define:COLONIZATION_INFLUENCE_COST up to the fourth point. After reaching the fourth point, further points cost
	define:COLONIZATION_EXTRA_GUARD_COST x (points - 4) + define:COLONIZATION_INFLUENCE_COST.
	*/

	auto free_points = nations::free_colonial_points(state, n);
	if(state.world.state_definition_get_colonization_stage(d) == 1) {
		return free_points >= int32_t(state.defines.colonization_interest_cost);
	} else if(state.world.colonization_get_level(colony_status) <= 4) {
		return free_points >= int32_t(state.defines.colonization_influence_cost);
	} else {
		return free_points >=
					 int32_t(state.defines.colonization_extra_guard_cost * (state.world.colonization_get_level(colony_status) - 4) +
									 state.defines.colonization_influence_cost);
	}
}

bool state_borders_nation(sys::state& state, dcon::nation_id n, dcon::state_instance_id si) {
	auto d = state.world.state_instance_get_definition(si);
	auto owner = state.world.state_instance_get_nation_from_state_ownership(si);
	for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
		if(p.get_province().get_nation_from_province_ownership() == owner) {
			for(auto adj : p.get_province().get_province_adjacency()) {
				auto indx = adj.get_connected_provinces(0) != p.get_province() ? 0 : 1;
				auto o = adj.get_connected_provinces(indx).get_nation_from_province_ownership();
				if(o == n)
					return true;
				if(o.get_overlord_as_subject().get_ruler() == n)
					return true;
			}
		}
	}
	return false;
}

bool can_start_colony(sys::state& state, dcon::nation_id n, dcon::state_definition_id d) {
	if(state.world.state_definition_get_colonization_stage(d) > uint8_t(1))
		return false; // too late

	auto mem = state.world.state_definition_get_abstract_state_membership(d);
	if(mem.begin() == mem.end() || (*mem.begin()).get_province().id.index() >= state.province_definitions.first_sea_province.index())
		return false;

	// Your country must be of define:COLONIAL_RANK or less.
	if(state.world.nation_get_rank(n) > uint16_t(state.defines.colonial_rank))
		return false; // too low rank to colonize;

	// The state may not be the current target of a crisis, nor may your country be involved in an active crisis war.
	if(state.crisis_attacker_wargoals.size() > 0) {
		auto first_wg = state.crisis_attacker_wargoals.at(0);
		if(first_wg.state == d)
			return false;
		for(auto par : state.world.war_get_war_participant(state.crisis_war)) {
			if(par.get_nation() == n)
				return false;
		}
	}

	float max_life_rating = -1.0f;
	for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
		if(!p.get_province().get_nation_from_province_ownership()) {
			max_life_rating = std::max(max_life_rating, float(p.get_province().get_life_rating()));
		}
	}

	if(max_life_rating < 0.0f) {
		return false; // no uncolonized province
	}

	/*
	You must have colonial life rating points from technology + define:COLONIAL_LIFERATING less than or equal to the *greatest*
	life rating of an unowned province in the state
	*/

	if(state.defines.colonial_liferating + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::colonial_life_rating) > max_life_rating) {
		return false;
	}

	auto colonizers = state.world.state_definition_get_colonization(d);
	auto num_colonizers = colonizers.end() - colonizers.begin();

	// You can invest colonially in a region if there are fewer than 4 other colonists there (or you already have a colonist
	// there)
	if(num_colonizers >= 4)
		return false;

	for(auto c : colonizers) {
		if(c.get_colonizer() == n)
			return false; // already started a colony
	}

	/*
	If you haven't yet put a colonist into the region, you must be in range of the region. Any region adjacent to your country or
	to one of your vassals or substates is considered to be in range. Otherwise it must be in range of one of your naval bases,
	with the range depending on the colonial range value provided by the naval base building x the level of the naval base.
	*/

	bool adjacent = [&]() {
		for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
			if(!p.get_province().get_nation_from_province_ownership()) {
				for(auto adj : p.get_province().get_province_adjacency()) {
					auto indx = adj.get_connected_provinces(0) != p.get_province() ? 0 : 1;
					auto o = adj.get_connected_provinces(indx).get_nation_from_province_ownership();
					if(o == n)
						return true;
					if(o.get_overlord_as_subject().get_ruler() == n)
						return true;
				}
			}
		}
		return false;
	}();

	/*
	// OLD WAY: just check if it is coastal
	bool nation_has_port = state.world.nation_get_central_ports(n) != 0;
	bool reachable_by_sea = nation_has_port && [&]() {
		for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
			if(!p.get_province().get_nation_from_province_ownership()) {
				if(p.get_province().get_is_coast())
					return true;
			}
		}
		return false;
	}();
	*/
	bool reachable_by_sea = false;

	dcon::province_id coastal_target = [&]() {
		for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
			if(!p.get_province().get_nation_from_province_ownership()) {
				if(p.get_province().get_is_coast())
					return p.get_province().id;
			}
		}
		return dcon::province_id{};
	}();

	if(!adjacent && coastal_target  && state.world.nation_get_central_ports(n) != 0) {
		for(auto p : state.world.nation_get_province_ownership(n)) {
			if(auto nb_level = p.get_province().get_building_level(uint8_t(economy::province_building_type::naval_base)); nb_level > 0 && p.get_province().get_nation_from_province_control() == n) {
				auto dist = province::direct_distance(state, p.get_province(), coastal_target);
				if(dist <= province::world_circumference *  state.defines.alice_naval_base_to_colonial_distance_factor * nb_level) {
					reachable_by_sea = true;
					break;
				}
			}
		}
	}

	if(!adjacent && !reachable_by_sea)
		return false;

	/*
	Investing in a colony costs define:COLONIZATION_INVEST_COST_INITIAL + define:COLONIZATION_INTEREST_COST_NEIGHBOR_MODIFIER (if
	a province adjacent to the region is owned) to place the initial colonist.
	*/

	auto free_points = nations::free_colonial_points(state, n);

	return free_points >= int32_t(state.defines.colonization_interest_cost_initial +
																(adjacent ? state.defines.colonization_interest_cost_neighbor_modifier : 0.0f));
}

bool fast_can_start_colony(sys::state& state, dcon::nation_id n, dcon::state_definition_id d, int32_t free_points, dcon::province_id coastal_target, bool& adjacent) {
	if(state.world.state_definition_get_colonization_stage(d) > uint8_t(1))
		return false; // too late

	if(free_points < int32_t(state.defines.colonization_interest_cost_initial + state.defines.colonization_interest_cost_neighbor_modifier))
		return false;

	auto mem = state.world.state_definition_get_abstract_state_membership(d);
	if(mem.begin() == mem.end() || (*mem.begin()).get_province().id.index() >= state.province_definitions.first_sea_province.index())
		return false;

	// Your country must be of define:COLONIAL_RANK or less.
	if(state.world.nation_get_rank(n) > uint16_t(state.defines.colonial_rank))
		return false; // too low rank to colonize;

	// The state may not be the current target of a crisis, nor may your country be involved in an active crisis war.
	if(state.crisis_attacker_wargoals.size() > 0) {
		auto first_wg = state.crisis_attacker_wargoals.at(0);
		if(first_wg.state == d)
			return false;
	}

	float max_life_rating = -1.0f;
	for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
		if(!p.get_province().get_nation_from_province_ownership()) {
			max_life_rating = std::max(max_life_rating, float(p.get_province().get_life_rating()));
		}
	}

	if(max_life_rating < 0.0f) {
		return false; // no uncolonized province
	}

	/*
	You must have colonial life rating points from technology + define:COLONIAL_LIFERATING less than or equal to the *greatest*
	life rating of an unowned province in the state
	*/

	if(state.defines.colonial_liferating + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::colonial_life_rating) > max_life_rating) {
		return false;
	}

	auto colonizers = state.world.state_definition_get_colonization(d);
	auto num_colonizers = colonizers.end() - colonizers.begin();

	// You can invest colonially in a region if there are fewer than 4 other colonists there (or you already have a colonist
	// there)
	if(num_colonizers >= 4)
		return false;

	for(auto c : colonizers) {
		if(c.get_colonizer() == n)
			return false; // already started a colony
	}

	/*
	If you haven't yet put a colonist into the region, you must be in range of the region. Any region adjacent to your country or
	to one of your vassals or substates is considered to be in range. Otherwise it must be in range of one of your naval bases,
	with the range depending on the colonial range value provided by the naval base building x the level of the naval base.
	*/

	adjacent = [&]() {
		for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
			if(!p.get_province().get_nation_from_province_ownership()) {
				for(auto adj : p.get_province().get_province_adjacency()) {
					auto indx = adj.get_connected_provinces(0) != p.get_province() ? 0 : 1;
					auto o = adj.get_connected_provinces(indx).get_nation_from_province_ownership();
					if(o == n)
						return true;
					if(o.get_overlord_as_subject().get_ruler() == n)
						return true;
				}
			}
		}
		return false;
	}();

	bool reachable_by_sea = false;

	if(!adjacent && coastal_target && state.world.nation_get_central_ports(n) != 0) {
		for(auto p : state.world.nation_get_province_ownership(n)) {
			if(auto nb_level = p.get_province().get_building_level(uint8_t(economy::province_building_type::naval_base)); nb_level > 0 && p.get_province().get_nation_from_province_control() == n) {
				auto dist = province::direct_distance(state, p.get_province(), coastal_target);
				if(dist <= province::world_circumference * 0.04f * nb_level) {
					reachable_by_sea = true;
					break;
				}
			}
		}
	}

	if(!adjacent && !reachable_by_sea)
		return false;

	/*
	Investing in a colony costs define:COLONIZATION_INVEST_COST_INITIAL + define:COLONIZATION_INTEREST_COST_NEIGHBOR_MODIFIER (if
	a province adjacent to the region is owned) to place the initial colonist.
	*/

	return free_points >= int32_t(state.defines.colonization_interest_cost_initial +
																(adjacent ? state.defines.colonization_interest_cost_neighbor_modifier : 0.0f));
}

void increase_colonial_investment(sys::state& state, dcon::nation_id source, dcon::state_definition_id state_def) {
	uint8_t greatest_other_level = 0;
	dcon::nation_id second_colonizer;
	for(auto rel : state.world.state_definition_get_colonization(state_def)) {
		if(rel.get_colonizer() != source) {
			if(rel.get_level() >= greatest_other_level) {
				greatest_other_level = rel.get_level();
				second_colonizer = rel.get_colonizer();
			}
		}
	}

	for(auto rel : state.world.state_definition_get_colonization(state_def)) {
		if(rel.get_colonizer() == source) {

			if(state.world.state_definition_get_colonization_stage(state_def) == 1) {
				rel.get_points_invested() += uint16_t(state.defines.colonization_interest_cost);
			} else if(rel.get_level() <= 4) {
				rel.get_points_invested() += uint16_t(state.defines.colonization_influence_cost);
			} else {
				rel.get_points_invested() += uint16_t(
						state.defines.colonization_extra_guard_cost * (rel.get_level() - 4) + state.defines.colonization_influence_cost);
			}

			rel.get_level() += uint8_t(1);
			rel.set_last_investment(state.current_date);

			/*
			If you get define:COLONIZATION_INTEREST_LEAD points it moves into phase 2, kicking out all but the second-most
			colonizer (in terms of points). In phase 2 if you get define:COLONIZATION_INFLUENCE_LEAD points ahead of the other
			colonizer, the other colonizer is kicked out and the phase moves to 3.
			*/
			if(state.world.state_definition_get_colonization_stage(state_def) == 1) {
				if(rel.get_level() >= int32_t(state.defines.colonization_interest_lead)) {

					state.world.state_definition_set_colonization_stage(state_def, uint8_t(2));
					auto col_range = state.world.state_definition_get_colonization(state_def);
					while(int32_t(col_range.end() - col_range.begin()) > 2) {
						for(auto r : col_range) {
							if(r.get_colonizer() != source && r.get_colonizer() != second_colonizer) {
								state.world.delete_colonization(r);
								break;
							}
						}
					}
				}
			} else if(rel.get_level() >= int32_t(state.defines.colonization_interest_lead) + greatest_other_level) {
				state.world.state_definition_set_colonization_stage(state_def, uint8_t(3));
				auto col_range = state.world.state_definition_get_colonization(state_def);
				while(int32_t(col_range.end() - col_range.begin()) > 1) {
					for(auto r : col_range) {
						if(r.get_colonizer() != source) {
							state.world.delete_colonization(r);
							break;
						}
					}
				}
			}
			return;
		}
	}
}

void update_colonization(sys::state& state) {
	for(auto d : state.world.in_state_definition) {
		auto colonizers = state.world.state_definition_get_colonization(d);
		auto num_colonizers = colonizers.end() - colonizers.begin();

		if(num_colonizers > 0) { // check for states that have become un-colonizable
			int32_t unowned_provs = 0;
			for(auto p : d.get_abstract_state_membership()) {
				if(!(p.get_province().get_nation_from_province_ownership())) {
					++unowned_provs;
					break;
				}
			}
			if(unowned_provs == 0) {
				while(colonizers.begin() != colonizers.end()) {
					state.world.delete_colonization(*(colonizers.begin()));
				}
				d.set_colonization_stage(uint8_t(0));
				continue;
			}
		}

		if(num_colonizers == 0 && d.get_colonization_stage() != 0) {
			d.set_colonization_stage(uint8_t(0));
		} else if(num_colonizers > 1 && d.get_colonization_stage() == uint8_t(2)) {
			/*
			In phase 2 if there are competing colonizers, the "temperature" in the colony will rise by
			define:COLONIAL_INFLUENCE_TEMP_PER_DAY + maximum-points-invested x define:COLONIAL_INFLUENCE_TEMP_PER_LEVEL +
			define:TENSION_WHILE_CRISIS (if there is some other crisis going on) + define:AT_WAR_TENSION_DECAY (if either of the
			two colonizers are at war or disarmed)
			*/

			int32_t max_points = 0;
			float at_war_adjust = 0.0f;
			for(auto c : colonizers) {
				max_points = std::max(max_points, int32_t(c.get_level()));
				if(state.world.nation_get_is_at_war(c.get_colonizer()) ||
						(state.world.nation_get_disarmed_until(c.get_colonizer()) &&
								state.current_date <= state.world.nation_get_disarmed_until(c.get_colonizer()))) {
					at_war_adjust = state.defines.at_war_tension_decay;
				}
			}

			float adjust = state.defines.colonization_influence_temperature_per_day +
										 float(max_points) * state.defines.colonization_influence_temperature_per_level +
										 (state.current_crisis_state == sys::crisis_state::inactive ? 0.0f : state.defines.tension_while_crisis) + at_war_adjust;

			bool anyone_has_army = false;
			for(auto c : colonizers) {
				if(c.get_colonizer().get_is_great_power())
					adjust *= 1.5f;

				if(c.get_colonizer().get_active_regiments())
					anyone_has_army = true;
			}

			if(!anyone_has_army)
				adjust *= 0.f;

			d.set_colonization_temperature(std::clamp(d.get_colonization_temperature() + adjust, 0.0f, 100.0f));
		} else if(num_colonizers == 1 &&
							(*colonizers.begin()).get_last_investment() + int32_t(state.defines.colonization_days_for_initial_investment) <=
									state.current_date) {
			/*
			If you have put in a colonist in a region and it goes at least define:COLONIZATION_DAYS_FOR_INITIAL_INVESTMENT without
			any other colonizers, it then moves into phase 3 with define:COLONIZATION_INTEREST_LEAD points.
			*/

			d.set_colonization_stage(uint8_t(3));
			(*colonizers.begin()).set_last_investment(state.current_date);
		} else if(d.get_colonization_stage() == uint8_t(3) && num_colonizers != 0) {
			/*
			If you leave a colony in phase 3 for define:COLONIZATION_MONTHS_TO_COLONIZE months, the colonization will reset to
			phase 0 (no colonization in progress).
			*/
			if((*colonizers.begin()).get_last_investment() + 31 * int32_t(state.defines.colonization_months_to_colonize) <=
					state.current_date) {

				d.set_colonization_stage(uint8_t(0));
				do {
					state.world.delete_colonization(*(colonizers.begin()));
				} while(colonizers.end() != colonizers.begin());
			} else if(state.world.nation_get_is_player_controlled((*colonizers.begin()).get_colonizer()) == false) { // ai colonization finishing
				auto source = (*colonizers.begin()).get_colonizer();

				for(auto pr : state.world.state_definition_get_abstract_state_membership(d)) {
					if(!pr.get_province().get_nation_from_province_ownership()) {
						province::change_province_owner(state, pr.get_province(), source);
					}
				}

				state.world.state_definition_set_colonization_temperature(d, 0.0f);
				state.world.state_definition_set_colonization_stage(d, uint8_t(0));

				while(colonizers.begin() != colonizers.end()) {
					state.world.delete_colonization(*colonizers.begin());
				}
			}
		}
	}
}

dcon::province_id state_get_coastal_capital(sys::state& state, dcon::state_instance_id s) {
	auto d = state.world.state_instance_get_definition(s);
	auto o = state.world.state_instance_get_nation_from_state_ownership(s);
	auto max_pop = 0.f;
	dcon::province_id result = { };
	for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
		if(p.get_province().get_nation_from_province_ownership() != o)
			continue;
		if(!p.get_province().get_port_to())
			continue;
		if(
			state.world.province_get_demographics(
				p.get_province(),
				demographics::total
			) > max_pop
		) {
			max_pop = state.world.province_get_demographics(
				p.get_province(),
				demographics::total
			);
			result = p.get_province();
		}
	}
	return result;
}

bool state_is_coastal(sys::state& state, dcon::state_instance_id s) {
	auto d = state.world.state_instance_get_definition(s);
	auto o = state.world.state_instance_get_nation_from_state_ownership(s);
	for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
		if(p.get_province().get_nation_from_province_ownership() == o) {
			if(p.get_province().get_port_to())
				return true;
		}
	}
	return false;
}

bool state_is_coastal_non_core_nb(sys::state& state, dcon::state_instance_id s) {
	auto d = state.world.state_instance_get_definition(s);
	auto o = state.world.state_instance_get_nation_from_state_ownership(s);
	bool coast = false;
	for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
		if(p.get_province().get_nation_from_province_ownership() == o) {
			if(p.get_province().get_port_to()) {
				if(p.get_province().get_is_owner_core())
					return false;
				coast = true;
			}
		}
	}
	return coast;
}

void add_core(sys::state& state, dcon::province_id prov, dcon::national_identity_id tag) {
	if(tag && prov) {
		state.world.try_create_core(prov, tag);
		if(state.world.province_get_nation_from_province_ownership(prov) ==
				state.world.national_identity_get_nation_from_identity_holder(tag)) {
			state.world.province_set_is_owner_core(prov, true);
		}
	}
}

void remove_core(sys::state& state, dcon::province_id prov, dcon::national_identity_id tag) {
	auto core_rel = state.world.get_core_by_prov_tag_key(prov, tag);
	if(core_rel) {
		state.world.delete_core(core_rel);
		if(state.world.province_get_nation_from_province_ownership(prov) ==
				state.world.national_identity_get_nation_from_identity_holder(tag)) {
			state.world.province_set_is_owner_core(prov, false);
		}
	}
}

void set_rgo(sys::state& state, dcon::province_id prov, dcon::commodity_id c) {
	auto old_rgo = state.world.province_get_rgo(prov);
	state.world.province_set_rgo(prov, c);
	auto next_size = state.world.province_get_rgo_base_size(prov) * 0.4f;
	float pop_amount = 0.0f;
	for(auto pt : state.world.in_pop_type) {
		if(pt == state.culture_definitions.slaves) {
			pop_amount += state.world.province_get_demographics(prov, demographics::to_key(state, state.culture_definitions.slaves));
		} else if(pt.get_is_paid_rgo_worker()) {
			pop_amount += state.world.province_get_demographics(prov, demographics::to_key(state, pt));
		}
	}
	if(pop_amount * 5.f < next_size) {
		next_size = pop_amount * 5.f;
	}
	state.world.province_get_rgo_size(prov, c) += next_size;
	// immediately employ workers
	state.world.province_get_rgo_target_employment(prov, c) += next_size;
	state.world.province_get_rgo_max_size(prov, c) += next_size;
	state.world.province_set_rgo_efficiency(prov, c, 1.f);
	if(state.world.commodity_get_is_mine(old_rgo) != state.world.commodity_get_is_mine(c)) {
		if(state.world.commodity_get_is_mine(c)) {
			for(auto pop : state.world.province_get_pop_location(prov)) {
				if(pop.get_pop().get_poptype() == state.culture_definitions.farmers) {
					pop.get_pop().set_poptype(state.culture_definitions.laborers);
				}
			}
		} else {
			for(auto pop : state.world.province_get_pop_location(prov)) {
				if(pop.get_pop().get_poptype() == state.culture_definitions.laborers) {
					pop.get_pop().set_poptype(state.culture_definitions.farmers);
				}
			}
		}
	}
}

void enable_canal(sys::state& state, int32_t id) {
	state.world.province_adjacency_get_type(state.province_definitions.canals[id]) &= ~province::border::impassible_bit;
}

// distance between to adjacent provinces
float distance(sys::state& state, dcon::province_adjacency_id pair) {
	return state.world.province_adjacency_get_distance(pair);
}

// direct distance between two provinces; does not pathfind
float direct_distance(sys::state& state, dcon::province_id a, dcon::province_id b) {
	auto apos = state.world.province_get_mid_point_b(a);
	auto bpos = state.world.province_get_mid_point_b(b);
	auto dot = (apos.x * bpos.x + apos.y * bpos.y) + apos.z * bpos.z;
	return math::acos(dot) * (world_circumference / (2.0f * math::pi));
}

float sorting_distance(sys::state& state, dcon::province_id a, dcon::province_id b) {
	auto apos = state.world.province_get_mid_point_b(a);
	auto bpos = state.world.province_get_mid_point_b(b);
	auto dot = (apos.x * bpos.x + apos.y * bpos.y) + apos.z * bpos.z;
	return -dot;
}

// whether a ship can dock at a land province
bool has_naval_access_to_province(sys::state& state, dcon::nation_id nation_as, dcon::province_id prov) {
	auto controller = state.world.province_get_nation_from_province_control(prov);

	if(!controller)
		return false;

	if(controller == nation_as)
		return true;

	if(state.world.nation_get_in_sphere_of(controller) == nation_as)
		return true;

	auto coverl = state.world.nation_get_overlord_as_subject(controller);
	if(state.world.overlord_get_ruler(coverl) == nation_as)
		return true;

	auto url = state.world.get_unilateral_relationship_by_unilateral_pair(controller, nation_as);
	if(state.world.unilateral_relationship_get_military_access(url))
		return true;

	if(military::are_allied_in_war(state, nation_as, controller))
		return true;

	return false;
}

// determines whether a land unit is allowed to move to / be in a province
bool has_access_to_province(sys::state& state, dcon::nation_id nation_as, dcon::province_id prov) {
	auto controller = state.world.province_get_nation_from_province_control(prov);

	if(!controller)
		return true;

	if(!nation_as) // rebels go everywhere
		return true;

	if(controller == nation_as)
		return true;

	if(state.world.nation_get_in_sphere_of(controller) == nation_as)
		return true;

	auto coverl = state.world.nation_get_overlord_as_subject(controller);
	if(state.world.overlord_get_ruler(coverl) == nation_as)
		return true;

	auto url = state.world.get_unilateral_relationship_by_unilateral_pair(controller, nation_as);
	if(state.world.unilateral_relationship_get_military_access(url))
		return true;

	if(military::are_in_common_war(state, nation_as, controller))
		return true;

	return false;
}

bool has_safe_access_to_province(sys::state& state, dcon::nation_id nation_as, dcon::province_id prov) {
	auto controller = state.world.province_get_nation_from_province_control(prov);

	if(!controller)
		return !bool(state.world.province_get_rebel_faction_from_province_rebel_control(prov));

	if(!nation_as) // rebels go everywhere
		return true;

	if(controller == nation_as)
		return true;

	if(state.world.nation_get_in_sphere_of(controller) == nation_as)
		return true;

	auto coverl = state.world.nation_get_overlord_as_subject(controller);
	if(state.world.overlord_get_ruler(coverl) == nation_as)
		return true;

	auto url = state.world.get_unilateral_relationship_by_unilateral_pair(controller, nation_as);
	if(state.world.unilateral_relationship_get_military_access(url))
		return true;

	if(military::are_allied_in_war(state, nation_as, controller))
		return true;

	return false;
}

struct province_and_distance {
	float distance_covered = 0.0f;
	float distance_to_target = 0.0f;
	dcon::province_id province;

	bool operator<(province_and_distance const& other) const noexcept {
		if(other.distance_covered + other.distance_to_target != distance_covered + distance_to_target)
			return distance_covered + distance_to_target > other.distance_covered + other.distance_to_target;
		return other.province.index() > province.index();
	}
};

static void assert_path_result(std::vector<dcon::province_id>& v) {
	for(auto const e : v)
		assert(bool(e));
}

// normal pathfinding
std::vector<dcon::province_id> make_land_path(sys::state& state, dcon::province_id start, dcon::province_id end, dcon::nation_id nation_as, dcon::army_id a) {

	std::vector<province_and_distance> path_heap;
	auto origins_vector = ve::vectorizable_buffer<dcon::province_id, dcon::province_id>(state.world.province_size());

	std::vector<dcon::province_id> path_result;

	if(start == end)
		return path_result;

	auto fill_path_result = [&](dcon::province_id i) {
		path_result.push_back(end);
		while(i && i != start) {
			path_result.push_back(i);
			i = origins_vector.get(i);
		}
	};

	path_heap.push_back(province_and_distance{0.0f, direct_distance(state, start, end), start});
	while(path_heap.size() > 0) {
		std::pop_heap(path_heap.begin(), path_heap.end());
		auto nearest = path_heap.back();
		path_heap.pop_back();

		for(auto adj : state.world.province_get_province_adjacency(nearest.province)) {
			auto other_prov =
					adj.get_connected_provinces(0) == nearest.province ? adj.get_connected_provinces(1) : adj.get_connected_provinces(0);
			auto bits = adj.get_type();
			auto distance = adj.get_distance();

			if((bits & province::border::impassible_bit) == 0 && !is_strait_blocked(state, nation_as, nearest.province, other_prov) && !origins_vector.get(other_prov)) {
				if(other_prov == end) {
					fill_path_result(nearest.province);
					assert_path_result(path_result);
					return path_result;
				}

				if(other_prov.id.index() < state.province_definitions.first_sea_province.index()) { // is land
					if(has_access_to_province(state, nation_as, other_prov)) {
						/* This will work fine for most instances, except, possibly, for allied nations or enemy ones */
						auto armies = state.world.province_get_army_location(other_prov);
						float danger_factor = (armies.begin() == armies.end() || (*armies.begin()).get_army().get_controller_from_army_control() == nation_as) ? 1.f : 4.f;
						path_heap.push_back(
								province_and_distance{nearest.distance_covered + distance * danger_factor, direct_distance(state, other_prov, end) * danger_factor, other_prov});
						std::push_heap(path_heap.begin(), path_heap.end());
						origins_vector.set(other_prov, nearest.province);
					} else {
						origins_vector.set(other_prov, dcon::province_id{0}); // exclude it from being checked again
					}
				} else { // is sea
					if(military::can_embark_onto_sea_tile(state, nation_as, other_prov, a)) {
						path_heap.push_back(
								province_and_distance{nearest.distance_covered + distance, direct_distance(state, other_prov, end), other_prov});
						std::push_heap(path_heap.begin(), path_heap.end());
						origins_vector.set(other_prov, nearest.province);
					} else {
						origins_vector.set(other_prov, dcon::province_id{0}); // exclude it from being checked again
					}
				}
			}
		}
	}

	assert_path_result(path_result);
	return path_result;
}

std::vector<dcon::province_id> make_safe_land_path(sys::state& state, dcon::province_id start, dcon::province_id end, dcon::nation_id nation_as) {

	std::vector<province_and_distance> path_heap;
	auto origins_vector = ve::vectorizable_buffer<dcon::province_id, dcon::province_id>(state.world.province_size());

	std::vector<dcon::province_id> path_result;

	if(start == end)
		return path_result;

	auto fill_path_result = [&](dcon::province_id i) {
		path_result.push_back(end);
		while(i && i != start) {
			path_result.push_back(i);
			i = origins_vector.get(i);
		}
	};

	path_heap.push_back(province_and_distance{ 0.0f, direct_distance(state, start, end), start });
	while(path_heap.size() > 0) {
		std::pop_heap(path_heap.begin(), path_heap.end());
		auto nearest = path_heap.back();
		path_heap.pop_back();

		for(auto adj : state.world.province_get_province_adjacency(nearest.province)) {
			auto other_prov =
				adj.get_connected_provinces(0) == nearest.province ? adj.get_connected_provinces(1) : adj.get_connected_provinces(0);
			auto bits = adj.get_type();
			auto distance = adj.get_distance();

			if((bits & province::border::impassible_bit) == 0 && !origins_vector.get(other_prov)) {
				if(other_prov == end) {
					fill_path_result(nearest.province);
					assert_path_result(path_result);
					return path_result;
				}

				if(other_prov.id.index() < state.province_definitions.first_sea_province.index()) { // is land
					if(other_prov.get_siege_progress() == 0 && has_safe_access_to_province(state, nation_as, other_prov)) {
						path_heap.push_back(
								province_and_distance{ nearest.distance_covered + distance, direct_distance(state, other_prov, end), other_prov });
						std::push_heap(path_heap.begin(), path_heap.end());
						origins_vector.set(other_prov, nearest.province);
					} else {
						origins_vector.set(other_prov, dcon::province_id{0}); // exclude it from being checked again
					}
				} else { // is sea
					origins_vector.set(other_prov, dcon::province_id{0}); // exclude it from being checked again
				}
			}
		}
	}

	assert_path_result(path_result);
	return path_result;
}

// used for land trade
std::vector<dcon::province_id> make_unowned_path(sys::state& state, dcon::province_id start, dcon::province_id end) {
	std::vector<province_and_distance> path_heap;
	auto origins_vector = ve::vectorizable_buffer<dcon::province_id, dcon::province_id>(state.world.province_size());

	std::vector<dcon::province_id> path_result;

	if(start == end)
		return path_result;

	auto fill_path_result = [&](dcon::province_id i) {
		path_result.push_back(end);
		while(i && i != start) {
			path_result.push_back(i);
			i = origins_vector.get(i);
		}
		};

	path_heap.push_back(province_and_distance{ 0.0f, direct_distance(state, start, end), start });
	while(path_heap.size() > 0) {
		std::pop_heap(path_heap.begin(), path_heap.end());
		auto nearest = path_heap.back();
		path_heap.pop_back();

		auto railroad_origin = state.world.province_get_building_level(nearest.province, uint8_t(economy::province_building_type::railroad));

		for(auto adj : state.world.province_get_province_adjacency(nearest.province)) {
			auto other_prov =
				adj.get_connected_provinces(0) == nearest.province ? adj.get_connected_provinces(1) : adj.get_connected_provinces(0);
			auto bits = adj.get_type();

			auto distance = adj.get_distance();
			auto railroad_target = state.world.province_get_building_level(other_prov, uint8_t(economy::province_building_type::railroad));
			if(railroad_origin > 0 && railroad_target > 0) {
				distance = distance / 2.f;
			}
			distance -= 0.03f * std::min(railroad_target, railroad_origin) * distance;

			if((bits & province::border::impassible_bit) == 0 && !origins_vector.get(other_prov)) {
				if(other_prov == end) {
					fill_path_result(nearest.province);
					assert_path_result(path_result);
					return path_result;
				}
				path_heap.push_back(
						province_and_distance{ nearest.distance_covered + distance, direct_distance(state, other_prov, end), other_prov });
				std::push_heap(path_heap.begin(), path_heap.end());
				origins_vector.set(other_prov, nearest.province);
			}
		}
	}

	assert_path_result(path_result);
	return path_result;
}

// used for rebel unit and black-flagged unit pathfinding
std::vector<dcon::province_id> make_unowned_land_path(sys::state& state, dcon::province_id start, dcon::province_id end) {
	std::vector<province_and_distance> path_heap;
	auto origins_vector = ve::vectorizable_buffer<dcon::province_id, dcon::province_id>(state.world.province_size());

	std::vector<dcon::province_id> path_result;

	if(start == end)
		return path_result;

	auto fill_path_result = [&](dcon::province_id i) {
		path_result.push_back(end);
		while(i && i != start) {
			path_result.push_back(i);
			i = origins_vector.get(i);
		}
	};

	path_heap.push_back(province_and_distance{0.0f, direct_distance(state, start, end), start});
	while(path_heap.size() > 0) {
		std::pop_heap(path_heap.begin(), path_heap.end());
		auto nearest = path_heap.back();
		path_heap.pop_back();

		for(auto adj : state.world.province_get_province_adjacency(nearest.province)) {
			auto other_prov =
					adj.get_connected_provinces(0) == nearest.province ? adj.get_connected_provinces(1) : adj.get_connected_provinces(0);
			auto bits = adj.get_type();
			auto distance = adj.get_distance();

			if((bits & province::border::impassible_bit) == 0 && !origins_vector.get(other_prov)) {
				if(other_prov == end) {
					fill_path_result(nearest.province);
					assert_path_result(path_result);
					return path_result;
				}
				if((bits & province::border::coastal_bit) == 0) { // doesn't cross coast -- i.e. is land province
					path_heap.push_back(
							province_and_distance{nearest.distance_covered + distance, direct_distance(state, other_prov, end), other_prov});
					std::push_heap(path_heap.begin(), path_heap.end());
					origins_vector.set(other_prov, nearest.province);
				}
			}
		}
	}

	assert_path_result(path_result);
	return path_result;
}

// naval unit pathfinding; start and end provinces may be land provinces; function assumes you have naval access to both
std::vector<dcon::province_id> make_naval_path(sys::state& state, dcon::province_id start, dcon::province_id end) {

	std::vector<province_and_distance> path_heap;
	auto origins_vector = ve::vectorizable_buffer<dcon::province_id, dcon::province_id>(state.world.province_size());

	std::vector<dcon::province_id> path_result;

	if(start == end)
		return path_result;

	auto fill_path_result = [&](dcon::province_id i) {
		path_result.push_back(end);
		while(i && i != start) {
			path_result.push_back(i);
			i = origins_vector.get(i);
		}
	};

	path_heap.push_back(province_and_distance{0.0f, direct_distance(state, start, end), start});
	while(path_heap.size() > 0) {
		std::pop_heap(path_heap.begin(), path_heap.end());
		auto nearest = path_heap.back();
		path_heap.pop_back();

		for(auto adj : state.world.province_get_province_adjacency(nearest.province)) {
			auto other_prov =
					adj.get_connected_provinces(0) == nearest.province ? adj.get_connected_provinces(1) : adj.get_connected_provinces(0);
			auto bits = adj.get_type();
			auto distance = adj.get_distance();

			// can't move over impassible connections; can't move directly from port to port
			if((bits & province::border::impassible_bit) == 0 && !origins_vector.get(other_prov) &&
					(other_prov.id.index() >= state.province_definitions.first_sea_province.index() ||
							nearest.province.index() >= state.province_definitions.first_sea_province.index())) {



				if((bits & province::border::coastal_bit) == 0) { // doesn't cross coast -- i.e. is sea province
					if(other_prov == end) {
						fill_path_result(nearest.province);
						assert_path_result(path_result);
						return path_result;
					} else {

						path_heap.push_back(province_and_distance{ nearest.distance_covered + distance, direct_distance(state, other_prov, end), other_prov });
						std::push_heap(path_heap.begin(), path_heap.end());
						origins_vector.set(other_prov, nearest.province);
					}
				} else if(other_prov.id.index() < state.province_definitions.first_sea_province.index() && other_prov == end && other_prov.get_port_to() == nearest.province) { // case: ending in a port

					fill_path_result(nearest.province);
					assert_path_result(path_result);
					return path_result;
				} else if(nearest.province.index() < state.province_definitions.first_sea_province.index() && state.world.province_get_port_to(nearest.province) == other_prov.id) { // case: leaving port

					if(other_prov == end) {
						fill_path_result(nearest.province);
						assert_path_result(path_result);
						return path_result;
					} else {
						path_heap.push_back(province_and_distance{ nearest.distance_covered + distance, direct_distance(state, other_prov, end), other_prov });
						std::push_heap(path_heap.begin(), path_heap.end());
						origins_vector.set(other_prov, nearest.province);
					}
				}
			}
		}
	}

	assert_path_result(path_result);
	return path_result;
}

struct retreat_province_and_distance {
	float distance_covered = 0.0f;
	dcon::province_id province;

	bool operator<(retreat_province_and_distance const& other) const noexcept {
		if(other.distance_covered != distance_covered)
			return distance_covered > other.distance_covered;
		return other.province.index() > province.index();
	}
};

std::vector<dcon::province_id> make_naval_retreat_path(sys::state& state, dcon::nation_id nation_as, dcon::province_id start) {

	std::vector<retreat_province_and_distance> path_heap;
	auto origins_vector = ve::vectorizable_buffer<dcon::province_id, dcon::province_id>(state.world.province_size());

	std::vector<dcon::province_id> path_result;

	auto fill_path_result = [&](dcon::province_id i) {
		while(i && i != start) {
			path_result.push_back(i);
			i = origins_vector.get(i);
		}
	};

	path_heap.push_back(retreat_province_and_distance{0.0f, start});
	while(path_heap.size() > 0) {
		std::pop_heap(path_heap.begin(), path_heap.end());
		auto nearest = path_heap.back();
		path_heap.pop_back();

		if(nearest.province.index() < state.province_definitions.first_sea_province.index()) {
			fill_path_result(nearest.province);
			assert_path_result(path_result);
			return path_result;
		}

		for(auto adj : state.world.province_get_province_adjacency(nearest.province)) {
			auto other_prov =
					adj.get_connected_provinces(0) == nearest.province ? adj.get_connected_provinces(1) : adj.get_connected_provinces(0);
			auto bits = adj.get_type();
			auto distance = adj.get_distance();

			if((bits & province::border::impassible_bit) == 0 && !origins_vector.get(other_prov)) {
				if((bits & province::border::coastal_bit) == 0) { // doesn't cross coast -- i.e. is sea province
					path_heap.push_back(retreat_province_and_distance{ nearest.distance_covered + distance, other_prov });
					std::push_heap(path_heap.begin(), path_heap.end());
					origins_vector.set(other_prov, nearest.province);
				} else if(other_prov.get_port_to() != nearest.province) { // province is not connected by a port here
					// skip
				} else if(has_naval_access_to_province(state, nation_as, other_prov)) { // possible land province destination
					path_heap.push_back(retreat_province_and_distance{nearest.distance_covered + distance, other_prov});
					std::push_heap(path_heap.begin(), path_heap.end());
					origins_vector.set(other_prov, nearest.province);
				} else {  // impossible land province destination
					origins_vector.set(other_prov, dcon::province_id{0}); // valid province prevents rechecks
				}
			}
		}
	}

	assert_path_result(path_result);
	return path_result;
}

std::vector<dcon::province_id> make_land_retreat_path(sys::state& state, dcon::nation_id nation_as, dcon::province_id start) {

	std::vector<retreat_province_and_distance> path_heap;
	auto origins_vector = ve::vectorizable_buffer<dcon::province_id, dcon::province_id>(state.world.province_size());

	origins_vector.set(start, dcon::province_id{0});

	std::vector<dcon::province_id> path_result;

	auto fill_path_result = [&](dcon::province_id i) {
		while(i && i != start) {
			path_result.push_back(i);
			i = origins_vector.get(i);
		}
	};

	path_heap.push_back(retreat_province_and_distance{0.0f, start});
	while(path_heap.size() > 0) {
		std::pop_heap(path_heap.begin(), path_heap.end());
		auto nearest = path_heap.back();
		path_heap.pop_back();

		if(nearest.province != start && has_naval_access_to_province(state, nation_as, nearest.province)) {
			fill_path_result(nearest.province);
			assert_path_result(path_result);
			return path_result;
		}

		for(auto adj : state.world.province_get_province_adjacency(nearest.province)) {
			auto other_prov =
					adj.get_connected_provinces(0) == nearest.province ? adj.get_connected_provinces(1) : adj.get_connected_provinces(0);
			auto bits = adj.get_type();
			auto distance = adj.get_distance();

			if((bits & province::border::impassible_bit) == 0 && !origins_vector.get(other_prov)) {
				if((bits & province::border::coastal_bit) == 0) { // doesn't cross coast -- i.e. is land province
					path_heap.push_back(retreat_province_and_distance{nearest.distance_covered + distance, other_prov});
					std::push_heap(path_heap.begin(), path_heap.end());
					origins_vector.set(other_prov, nearest.province);
				} else { // is sea province
								 // nothing
				}
			}
		}
	}

	assert_path_result(path_result);
	return path_result;
}

std::vector<dcon::province_id> make_path_to_nearest_coast(sys::state& state, dcon::nation_id nation_as, dcon::province_id start) {
	std::vector<retreat_province_and_distance> path_heap;
	auto origins_vector = ve::vectorizable_buffer<dcon::province_id, dcon::province_id>(state.world.province_size());

	origins_vector.set(start, dcon::province_id{0});

	std::vector<dcon::province_id> path_result;

	auto fill_path_result = [&](dcon::province_id i) {
		while(i && i != start) {
			path_result.push_back(i);
			i = origins_vector.get(i);
		}
	};

	path_heap.push_back(retreat_province_and_distance{ 0.0f, start });
	while(path_heap.size() > 0) {
		std::pop_heap(path_heap.begin(), path_heap.end());
		auto nearest = path_heap.back();
		path_heap.pop_back();

		if(state.world.province_get_is_coast(nearest.province)) {
			fill_path_result(nearest.province);
			assert_path_result(path_result);
			return path_result;
		}

		for(auto adj : state.world.province_get_province_adjacency(nearest.province)) {
			auto other_prov =
				adj.get_connected_provinces(0) == nearest.province ? adj.get_connected_provinces(1) : adj.get_connected_provinces(0);
			auto bits = adj.get_type();
			auto distance = adj.get_distance();

			if((bits & province::border::impassible_bit) == 0 && !origins_vector.get(other_prov)) {
				if((bits & province::border::coastal_bit) == 0) { // doesn't cross coast -- i.e. is land province
					if(has_naval_access_to_province(state, nation_as, other_prov)) {
						path_heap.push_back(retreat_province_and_distance{ nearest.distance_covered + distance, other_prov });
						std::push_heap(path_heap.begin(), path_heap.end());
						origins_vector.set(other_prov, nearest.province);
					} else {
						origins_vector.set(other_prov, dcon::province_id{0});
					}
				} else { // is sea province
					// nothing
				}
			}
		}
	}

	assert_path_result(path_result);
	return path_result;
}
std::vector<dcon::province_id> make_unowned_path_to_nearest_coast(sys::state& state, dcon::province_id start) {
	std::vector<retreat_province_and_distance> path_heap;
	auto origins_vector = ve::vectorizable_buffer<dcon::province_id, dcon::province_id>(state.world.province_size());

	origins_vector.set(start, dcon::province_id{0});

	std::vector<dcon::province_id> path_result;

	auto fill_path_result = [&](dcon::province_id i) {
		while(i && i != start) {
			path_result.push_back(i);
			i = origins_vector.get(i);
		}
	};

	path_heap.push_back(retreat_province_and_distance{ 0.0f, start });
	while(path_heap.size() > 0) {
		std::pop_heap(path_heap.begin(), path_heap.end());
		auto nearest = path_heap.back();
		path_heap.pop_back();

		if(state.world.province_get_is_coast(nearest.province)) {
			fill_path_result(nearest.province);
			assert_path_result(path_result);
			return path_result;
		}

		for(auto adj : state.world.province_get_province_adjacency(nearest.province)) {
			auto other_prov =
				adj.get_connected_provinces(0) == nearest.province ? adj.get_connected_provinces(1) : adj.get_connected_provinces(0);
			auto bits = adj.get_type();
			auto distance = adj.get_distance();

			if((bits & province::border::impassible_bit) == 0 && !origins_vector.get(other_prov)) {
				if((bits & province::border::coastal_bit) == 0) { // doesn't cross coast -- i.e. is land province
					path_heap.push_back(retreat_province_and_distance{ nearest.distance_covered + distance, other_prov });
					std::push_heap(path_heap.begin(), path_heap.end());
					origins_vector.set(other_prov, nearest.province);
				} else { // is sea province
					// nothing
				}
			}
		}
	}

	assert_path_result(path_result);
	return path_result;
}

void restore_distances(sys::state& state) {
	for(auto p : state.world.in_province) {
		auto tile_pos = p.get_mid_point();
		auto scaled_pos = tile_pos / glm::vec2{float(state.map_state.map_data.size_x), float(state.map_state.map_data.size_y)};

		glm::vec3 new_world_pos;
		float angle_x = 2 * scaled_pos.x * math::pi;
		new_world_pos.x = math::cos(angle_x);
		new_world_pos.y = math::sin(angle_x);

		float angle_y = scaled_pos.y * math::pi;
		new_world_pos.x *= math::sin(angle_y);
		new_world_pos.y *= math::sin(angle_y);
		new_world_pos.z = math::cos(angle_y);

		p.set_mid_point_b(new_world_pos);
	}
	for(auto adj : state.world.in_province_adjacency) {
		auto dist = direct_distance(state, adj.get_connected_provinces(0), adj.get_connected_provinces(1));
		adj.set_distance(dist);
	}
}

} // namespace province
