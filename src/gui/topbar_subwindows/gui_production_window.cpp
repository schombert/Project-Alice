#include "gui_production_window.hpp"

namespace ui {

void populate_production_states_list(sys::state& state, std::vector<dcon::state_instance_id>& row_contents, dcon::nation_id n, bool show_empty, production_sort_order sort_order) {

	for(auto const fat_id : state.world.nation_get_state_ownership(n)) {
		if(show_empty && !fat_id.get_state().get_capital().get_is_colonial()) {
			row_contents.push_back(fat_id.get_state());
		} else if(economy::has_factory(state, fat_id.get_state().id)) {
			// Then account for factories **hidden** by the filter from goods...
			size_t count = 0;
			province::for_each_province_in_state_instance(state, fat_id.get_state(), [&](dcon::province_id pid) {
				auto ffact_id = dcon::fatten(state.world, pid);
				ffact_id.for_each_factory_location_as_province([&](dcon::factory_location_id flid) {
					auto fid = state.world.factory_location_get_factory(flid);
					Cyto::Any payload = commodity_filter_query_data{
							state.world.factory_type_get_output(state.world.factory_get_building_type(fid)).id, false};
					state.ui_state.production_subwindow->impl_get(state, payload);
					auto content = any_cast<commodity_filter_query_data>(payload);
					count += content.filter ? 1 : 0;
				});
			});
			for(auto construct : fat_id.get_state().get_state_building_construction()) {
				Cyto::Any payload = commodity_filter_query_data{construct.get_type().get_output().id, false};
				state.ui_state.production_subwindow->impl_get(state, payload);
				auto content = any_cast<commodity_filter_query_data>(payload);
				count += content.filter ? 1 : 0;
			}


			if(count > 0)
				row_contents.push_back(fat_id.get_state());
		}
	}

	auto sort_by_name = [&](dcon::state_instance_id a, dcon::state_instance_id b) {
		auto a_name =
				text::produce_simple_string(state, state.world.state_definition_get_name(state.world.state_instance_get_definition(a)));
		auto b_name =
				text::produce_simple_string(state, state.world.state_definition_get_name(state.world.state_instance_get_definition(b)));
		return a_name < b_name;
	};
	auto sort_by_factories = [&](dcon::state_instance_id a, dcon::state_instance_id b) {
		size_t acount = 0;
		province::for_each_province_in_state_instance(state, a, [&](dcon::province_id pid) {
			auto ffact_id = dcon::fatten(state.world, pid);
			ffact_id.for_each_factory_location_as_province([&](dcon::factory_location_id flid) {
				auto fid = state.world.factory_location_get_factory(flid);
				Cyto::Any payload = commodity_filter_query_data{
						state.world.factory_type_get_output(state.world.factory_get_building_type(fid)).id, false};
				state.ui_state.production_subwindow->impl_get(state, payload);
				auto content = any_cast<commodity_filter_query_data>(payload);
				acount += content.filter ? 1 : 0;
			});
		});

		size_t bcount = 0;
		province::for_each_province_in_state_instance(state, b, [&](dcon::province_id pid) {
			auto ffact_id = dcon::fatten(state.world, pid);
			ffact_id.for_each_factory_location_as_province([&](dcon::factory_location_id flid) {
				auto fid = state.world.factory_location_get_factory(flid);
				Cyto::Any payload = commodity_filter_query_data{
						state.world.factory_type_get_output(state.world.factory_get_building_type(fid)).id, false};
				state.ui_state.production_subwindow->impl_get(state, payload);
				auto content = any_cast<commodity_filter_query_data>(payload);
				bcount += content.filter ? 1 : 0;
			});
		});
		return acount > bcount;
	};
	auto sort_by_primary_workers = [&](dcon::state_instance_id a, dcon::state_instance_id b) {
		return state.world.state_instance_get_demographics(a,
							 demographics::to_key(state, state.culture_definitions.primary_factory_worker)) >
					 state.world.state_instance_get_demographics(b,
							 demographics::to_key(state, state.culture_definitions.primary_factory_worker));
	};
	auto sort_by_secondary_workers = [&](dcon::state_instance_id a, dcon::state_instance_id b) {
		return state.world.state_instance_get_demographics(a,
							 demographics::to_key(state, state.culture_definitions.secondary_factory_worker)) >
					 state.world.state_instance_get_demographics(b,
							 demographics::to_key(state, state.culture_definitions.secondary_factory_worker));
	};
	auto sort_by_owners = [&](dcon::state_instance_id a, dcon::state_instance_id b) {
		auto atotal = state.world.state_instance_get_demographics(a, demographics::total);
		auto btotal = state.world.state_instance_get_demographics(b, demographics::total);
		return state.world.state_instance_get_demographics(a, demographics::to_key(state, state.culture_definitions.capitalists)) /
							 atotal >
					 state.world.state_instance_get_demographics(b, demographics::to_key(state, state.culture_definitions.capitalists)) /
							 btotal;
	};
	auto sort_by_infrastructure = [&](dcon::state_instance_id a, dcon::state_instance_id b) {
		float atotal = 0.0f;
		float ap_total = 0.0f;
		province::for_each_province_in_state_instance(state, a, [&](dcon::province_id p) {
			atotal += float(state.world.province_get_building_level(p, economy::province_building_type::railroad));
			ap_total += 1.0f;
		});
		float btotal = 0.0f;
		float bp_total = 0.0f;
		province::for_each_province_in_state_instance(state, b, [&](dcon::province_id p) {
			btotal += float(state.world.province_get_building_level(p, economy::province_building_type::railroad));
			bp_total += 1.0f;
		});
		return atotal / ap_total > btotal / bp_total;
	};

	switch(sort_order) {
	case production_sort_order::name:
		std::sort(row_contents.begin(), row_contents.end(), sort_by_name);
		break;
	case production_sort_order::factories:
		std::sort(row_contents.begin(), row_contents.end(), sort_by_factories);
		break;
	case production_sort_order::primary_workers:
		std::sort(row_contents.begin(), row_contents.end(), sort_by_primary_workers);
		break;
	case production_sort_order::secondary_workers:
		std::sort(row_contents.begin(), row_contents.end(), sort_by_secondary_workers);
		break;
	case production_sort_order::owners:
		std::sort(row_contents.begin(), row_contents.end(), sort_by_owners);
		break;
	case production_sort_order::infrastructure:
		std::sort(row_contents.begin(), row_contents.end(), sort_by_infrastructure);
		break;
	}
}

void open_foreign_investment(sys::state& state, dcon::nation_id n) {
	if(state.ui_state.topbar_subwindow->is_visible()) {
		state.ui_state.topbar_subwindow->set_visible(state, false);
	}
	state.ui_state.production_subwindow->set_visible(state, true);
	state.ui_state.root->move_child_to_front(state.ui_state.production_subwindow);
	state.ui_state.topbar_subwindow = state.ui_state.production_subwindow;

	send(state, state.ui_state.production_subwindow, open_investment_nation{n});
}

void open_build_foreign_factory(sys::state& state, dcon::state_instance_id st) {
	if(state.ui_state.topbar_subwindow->is_visible()) {
		state.ui_state.topbar_subwindow->set_visible(state, false);
	}
	state.ui_state.production_subwindow->set_visible(state, true);
	state.ui_state.root->move_child_to_front(state.ui_state.production_subwindow);
	state.ui_state.topbar_subwindow = state.ui_state.production_subwindow;

	send(state, state.ui_state.production_subwindow,
			open_investment_nation{state.world.state_instance_get_nation_from_state_ownership(st)});

	send(state, state.ui_state.production_subwindow, production_selection_wrapper{st, true, xy_pair{0, 0}});
}

} // namespace ui
