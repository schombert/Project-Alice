#include "system_state.hpp"
#include "unit_tooltip.hpp"

namespace ui {

unitamounts calc_amounts_from_army(sys::state& state, dcon::army_fat_id army) {
	unitamounts amounts;
	for(auto n : army.get_army_membership()) {
		dcon::unit_type_id utid = n.get_regiment().get_type();
		auto result = state.military_definitions.unit_base_definitions[utid].type;
		if(result == military::unit_type::infantry) {
			amounts.type1++;
			amounts.type1_pop += uint32_t(state.world.regiment_get_strength(n.get_regiment().id) * state.defines.pop_size_per_regiment);
		} else if(result == military::unit_type::cavalry) {
			amounts.type2++;
			amounts.type2_pop += uint32_t(state.world.regiment_get_strength(n.get_regiment().id) * state.defines.pop_size_per_regiment);
		} else if(result == military::unit_type::support || result == military::unit_type::special) {
			amounts.type3++;
			amounts.type3_pop += uint32_t(state.world.regiment_get_strength(n.get_regiment().id) * state.defines.pop_size_per_regiment);
		}
	}
	return amounts;
}

unitamounts calc_amounts_from_navy(sys::state& state, dcon::navy_fat_id navy) {
	unitamounts amounts;
	for(auto n : navy.get_navy_membership()) {
		dcon::unit_type_id utid = n.get_ship().get_type();
		auto result = state.military_definitions.unit_base_definitions[utid].type;
		if(result == military::unit_type::big_ship) {
			amounts.type1++;
		} else if(result == military::unit_type::light_ship) {
			amounts.type2++;
		} else if(result == military::unit_type::transport) {
			amounts.type3++;
		}
	}
	return amounts;
}

void populate_armies(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	static std::string no_leader = text::produce_simple_string(state, "utt_noleader");

	for(auto armyloc : fat.get_army_location()) {
		auto army = armyloc.get_army();

		auto box = text::open_layout_box(contents);

		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::country, army.get_controller_from_army_control().id);
		text::add_to_substitution_map(sub, text::variable_type::unit, state.to_string_view(army.get_name()));
		text::add_to_substitution_map(sub, text::variable_type::leader, state.to_string_view(army.get_general_from_army_leadership().get_name()));
		if(!army.get_general_from_army_leadership().is_valid()) {
			text::add_to_substitution_map(sub, text::variable_type::leader, std::string_view{no_leader});
		}
		unitamounts amounts = calc_amounts_from_army(state, army);
		text::add_to_substitution_map(sub, text::variable_type::infantry, text::int_wholenum{int32_t(amounts.type1_pop)});
		text::add_to_substitution_map(sub, text::variable_type::cavalry, text::int_wholenum{int32_t(amounts.type2_pop)});
		text::add_to_substitution_map(sub, text::variable_type::special, text::int_wholenum{int32_t(amounts.type3_pop)});

		// TODO - add army controller flag
		text::localised_format_box(state, contents, box, std::string_view("utt_unit"), sub);
		text::add_line_break_to_layout_box(state, contents, box);

		auto path = army.get_path();
		if(path.size() > 0) {
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_substitution_map(sub, text::variable_type::prov, *path.end());
			text::add_to_substitution_map(sub, text::variable_type::date, army.get_arrival_time());
			text::localised_format_box(state, contents, box, std::string_view("estimate_arrival"), sub);
			text::add_line_break_to_layout_box(state, contents, box);
		}

		text::close_layout_box(contents, box);
	}
}

void populate_navies(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	static std::string no_leader = text::produce_simple_string(state, "utt_noleader");

	for(auto navyloc : fat.get_navy_location()) {
		auto navy = navyloc.get_navy();

		auto box = text::open_layout_box(contents);

		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::country, navy.get_controller_from_navy_control().id);
		text::add_to_substitution_map(sub, text::variable_type::unit, state.to_string_view(navy.get_name()));
		text::add_to_substitution_map(sub, text::variable_type::leader, state.to_string_view(navy.get_admiral_from_navy_leadership().get_name()));
		if(!navy.get_admiral_from_navy_leadership().is_valid()) {
			text::add_to_substitution_map(sub, text::variable_type::leader, std::string_view{no_leader});
		}
		unitamounts amounts = calc_amounts_from_navy(state, navy);
		text::add_to_substitution_map(sub, text::variable_type::infantry, text::int_wholenum{int32_t(amounts.type1)});
		text::add_to_substitution_map(sub, text::variable_type::cavalry, text::int_wholenum{int32_t(amounts.type2)});
		text::add_to_substitution_map(sub, text::variable_type::special, text::int_wholenum{int32_t(amounts.type3)});

		// TODO - add navy controller flag
		text::localised_format_box(state, contents, box, std::string_view("utt_unit"), sub);
		text::add_line_break_to_layout_box(state, contents, box);

		auto path = navy.get_path();
		if(path.size() > 0) {
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_substitution_map(sub, text::variable_type::prov, *path.end());
			text::add_to_substitution_map(sub, text::variable_type::date, navy.get_arrival_time());
			text::localised_format_box(state, contents, box, std::string_view("estimate_arrival"), sub);
			text::add_line_break_to_layout_box(state, contents, box);
		}

		text::close_layout_box(contents, box);
	}
}

void populate_unit_tooltip(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	populate_armies(state, contents, prov);
	populate_navies(state, contents, prov);
}

} // namespace ui
