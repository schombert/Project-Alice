#pragma once
#include "text.hpp"
#include "system_state.hpp"

namespace ui {

void display_leader_attributes(sys::state& state, dcon::leader_id lid, text::layout_base& contents, int32_t indent = 0) {
	auto leader_per = state.world.leader_get_personality(lid);
	auto leader_bak = state.world.leader_get_background(lid);
	auto attack = state.world.leader_trait_get_attack(leader_per) + state.world.leader_trait_get_attack(leader_bak);
	auto organisation = state.world.leader_trait_get_organisation(leader_per) + state.world.leader_trait_get_organisation(leader_bak);
	auto morale = state.world.leader_trait_get_morale(leader_per) + state.world.leader_trait_get_morale(leader_bak);
	auto defense = state.world.leader_trait_get_defense(leader_per) + state.world.leader_trait_get_defense(leader_bak);
	auto reconnaissance = state.world.leader_trait_get_reconnaissance(leader_per) + state.world.leader_trait_get_reconnaissance(leader_bak);
	auto speed = state.world.leader_trait_get_speed(leader_per) + state.world.leader_trait_get_speed(leader_bak);
	auto experience = state.world.leader_trait_get_experience(leader_per) + state.world.leader_trait_get_experience(leader_bak);
	auto reliability = state.world.leader_trait_get_reliability(leader_per) + state.world.leader_trait_get_reliability(leader_bak);

	if(attack != 0) {
		auto box = text::open_layout_box(contents, indent);
		text::localised_format_box(state, contents, box, "trait_attack");
		text::add_space_to_layout_box(state, contents, box);
		if(attack > 0) {
			text::add_to_layout_box(state, contents, box, std::string_view("+"), text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ attack }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ attack }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	if(defense != 0) {
		auto box = text::open_layout_box(contents, indent);
		text::localised_format_box(state, contents, box, "trait_defend");
		text::add_space_to_layout_box(state, contents, box);
		if(defense > 0) {
			text::add_to_layout_box(state, contents, box, std::string_view("+"), text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ defense }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ defense }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	if(morale != 0) {
		auto box = text::open_layout_box(contents, indent);
		text::localised_format_box(state, contents, box, "trait_morale");
		text::add_space_to_layout_box(state, contents, box);
		if(morale > 0) {
			text::add_to_layout_box(state, contents, box, std::string_view("+"), text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ morale }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ morale }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	if(organisation != 0) {
		auto box = text::open_layout_box(contents, indent);
		text::localised_format_box(state, contents, box, "trait_organisation");
		text::add_space_to_layout_box(state, contents, box);
		if(organisation > 0) {
			text::add_to_layout_box(state, contents, box, std::string_view("+"), text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ organisation }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ organisation }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	if(reconnaissance != 0) {
		auto box = text::open_layout_box(contents, indent);
		text::localised_format_box(state, contents, box, "trait_reconaissance");
		text::add_space_to_layout_box(state, contents, box);
		if(reconnaissance > 0) {
			text::add_to_layout_box(state, contents, box, std::string_view("+"), text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ reconnaissance }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ reconnaissance }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	if(speed != 0) {
		auto box = text::open_layout_box(contents, indent);
		text::localised_format_box(state, contents, box, "trait_speed");
		text::add_space_to_layout_box(state, contents, box);
		if(speed > 0) {
			text::add_to_layout_box(state, contents, box, std::string_view("+"), text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ speed }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ speed }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	if(experience != 0) {
		auto box = text::open_layout_box(contents, indent);
		text::localised_format_box(state, contents, box, "trait_experience");
		text::add_space_to_layout_box(state, contents, box);
		if(experience > 0) {
			text::add_to_layout_box(state, contents, box, std::string_view("+"), text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ experience }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ experience }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	if(reliability != 0) {
		auto box = text::open_layout_box(contents, indent);
		text::localised_format_box(state, contents, box, "trait_reliability");
		text::add_space_to_layout_box(state, contents, box);
		if(reliability > 0) {
			text::add_to_layout_box(state, contents, box, std::string_view("+"), text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ reliability }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ reliability }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
}

void display_leader_full(sys::state& state, dcon::leader_id lid, text::layout_base& contents, int32_t indent = 0) {
	auto lname = state.world.leader_get_name(lid);
	auto resolved = state.to_string_view(lname);

	auto box = text::open_layout_box(contents, indent);
	text::add_to_layout_box(state, contents, box, resolved);
	text::close_layout_box(contents, box);

	display_leader_attributes(state, lid, contents, indent + 15);
}

}
