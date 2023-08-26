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
		} else
		if(result == military::unit_type::cavalry) {
			amounts.type2++;
		} else
		if(result == military::unit_type::support || result == military::unit_type::special) {
			amounts.type3++;
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
		} else
		if(result == military::unit_type::light_ship) {
			amounts.type2++;
		} else
		if(result == military::unit_type::transport) {
			amounts.type3++;
		}
	}
	return amounts;
}

void single_unit_tooltip(sys::state& state, text::columnar_layout& contents, dcon::army_id a) {
	auto army = dcon::fatten(state.world, a);
	unitamounts amounts = calc_amounts_from_army(state, army);

	text::substitution_map sub;
	auto tag_str = std::string("@") + nations::int_to_tag(army.get_controller_from_army_control().get_identity_from_identity_holder().get_identifying_int()) + "\x03";
	text::add_to_substitution_map(sub, text::variable_type::m, std::string_view{ tag_str });
	text::add_to_substitution_map(sub, text::variable_type::n, int64_t(amounts.type1));
	text::add_to_substitution_map(sub, text::variable_type::x, int64_t(amounts.type2));
	text::add_to_substitution_map(sub, text::variable_type::y, int64_t(amounts.type3));
	if(army.get_arrival_time()) {
		text::add_to_substitution_map(sub, text::variable_type::prov, *(army.get_path().end() - 1));
		text::add_to_substitution_map(sub, text::variable_type::date, army.get_arrival_time());
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "unit_moving_text", sub);
		text::close_layout_box(contents, box);
	} else {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "unit_standing_text", sub);
		text::close_layout_box(contents, box);
	}
}
void single_unit_tooltip(sys::state& state, text::columnar_layout& contents, dcon::navy_id n) {
	auto navy = dcon::fatten(state.world, n);
	unitamounts amounts = calc_amounts_from_navy(state, navy);

	text::substitution_map sub;
	auto tag_str = std::string("@") + nations::int_to_tag(navy.get_controller_from_navy_control().get_identity_from_identity_holder().get_identifying_int()) + "\x04";
	text::add_to_substitution_map(sub, text::variable_type::m, std::string_view{ tag_str });
	text::add_to_substitution_map(sub, text::variable_type::n, int64_t(amounts.type1));
	text::add_to_substitution_map(sub, text::variable_type::x, int64_t(amounts.type2));
	text::add_to_substitution_map(sub, text::variable_type::y, int64_t(amounts.type3));
	if(navy.get_arrival_time()) {
		text::add_to_substitution_map(sub, text::variable_type::prov, *(navy.get_path().end() - 1));
		text::add_to_substitution_map(sub, text::variable_type::date, navy.get_arrival_time());
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "unit_moving_text", sub);
		text::close_layout_box(contents, box);
	} else {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "unit_standing_text", sub);
		text::close_layout_box(contents, box);
	}
}

void populate_armies(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	for(auto armyloc : fat.get_army_location()) {
		auto army = armyloc.get_army();
		single_unit_tooltip(state, contents, army);
	}
}

void populate_navies(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	auto fat = dcon::fatten(state.world, prov);
	for(auto navyloc : fat.get_navy_location()) {
		auto navy = navyloc.get_navy();
		single_unit_tooltip(state, contents, navy);
	}
}

void populate_unit_tooltip(sys::state& state, text::columnar_layout& contents, dcon::province_id prov) {
	populate_armies(state, contents, prov);
	populate_navies(state, contents, prov);
}

}
