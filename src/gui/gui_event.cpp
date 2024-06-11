#include "gui_event.hpp"
#include "gui_leader_select.hpp"
#include "gui_politics_window.hpp"
#include "triggers.hpp"

namespace ui {

static std::vector<std::unique_ptr<base_event_window>> event_pool[uint32_t(event_pool_slot::count)];
static int32_t pending_events[uint32_t(event_pool_slot::count)];
static std::vector<base_event_window*> pending_closure;

void populate_event_submap(sys::state& state, text::substitution_map& sub, std::variant<event::pending_human_n_event, event::pending_human_f_n_event, event::pending_human_p_event, event::pending_human_f_p_event> const& phe) noexcept {
	dcon::nation_id target_nation{};
	dcon::nation_id from_nation{};
	dcon::province_id target_province{};
	dcon::province_id from_province{};
	dcon::state_instance_id target_state{};
	dcon::state_instance_id from_state{};
	dcon::province_id target_capital{};
	dcon::province_id from_capital{};
	int32_t target_pop = 0;

	event::slot_type ft = event::slot_type::none;
	int32_t from_slot = -1;

	event::slot_type pt = event::slot_type::none;
	int32_t primary_slot = -1;
	sys::date event_date;

	if(std::holds_alternative<event::pending_human_n_event>(phe)) {
		auto const& e = std::get<event::pending_human_n_event>(phe);
		target_nation = e.n;
		target_pop = int32_t(state.world.nation_get_demographics(target_nation, demographics::total));
		target_capital = state.world.nation_get_capital(target_nation);
		target_province = target_capital;
		if(e.pt != event::slot_type::state)
			target_state = state.world.province_get_state_membership(target_province);
		else
			target_state = trigger::to_state(e.primary_slot);

		from_slot = e.from_slot;
		ft = e.ft;
		primary_slot = e.primary_slot;
		pt = e.pt;
		event_date = e.date;
	} else if(std::holds_alternative<event::pending_human_f_n_event>(phe)) {
		auto const& e = std::get<event::pending_human_f_n_event>(phe);
		target_nation = e.n;
		target_pop = int32_t(state.world.nation_get_demographics(target_nation, demographics::total));
		target_capital = state.world.nation_get_capital(target_nation);
		target_province = target_capital;
		target_state = state.world.province_get_state_membership(target_province);

		event_date = e.date;
	} else if(std::holds_alternative<event::pending_human_p_event>(phe)) {
		auto const& e = std::get<event::pending_human_p_event>(phe);
		target_nation = state.world.province_get_nation_from_province_ownership(e.p);
		target_state = state.world.province_get_state_membership(e.p);
		target_capital = state.world.state_instance_get_capital(target_state);
		target_province = e.p;
		target_pop = int32_t(state.world.province_get_demographics(target_province, demographics::total));

		from_slot = e.from_slot;
		ft = e.ft;
		event_date = e.date;
	} else if(std::holds_alternative<event::pending_human_f_p_event>(phe)) {
		auto const& e = std::get<event::pending_human_f_p_event>(phe);
		target_nation = state.world.province_get_nation_from_province_ownership(e.p);
		target_state = state.world.province_get_state_membership(e.p);
		target_capital = state.world.state_instance_get_capital(target_state);
		target_province = e.p;
		target_pop = int32_t(state.world.province_get_demographics(target_province, demographics::total));
		event_date = e.date;
	}

	switch(ft) {
	case event::slot_type::nation:
		from_nation = trigger::to_nation(from_slot);
		from_capital = state.world.nation_get_capital(from_nation);
		switch(pt) {
		case event::slot_type::state:
			from_state = trigger::to_state(primary_slot);
			from_province = state.world.state_instance_get_capital(from_state);
			break;
		case event::slot_type::province:
			from_province = trigger::to_prov(primary_slot);
			from_state = state.world.province_get_state_membership(from_province);
			break;
		default:
			from_province = state.world.nation_get_capital(from_nation);
			from_state = state.world.province_get_state_membership(from_province);
			break;
		}
		break;
	case event::slot_type::state:
		from_state = trigger::to_state(from_slot);
		from_nation = state.world.state_instance_get_nation_from_state_ownership(from_state);
		from_capital = state.world.state_instance_get_capital(from_state);
		switch(pt) {
		case event::slot_type::province:
			from_province = trigger::to_prov(primary_slot);
			break;
		default:
			from_province = state.world.state_instance_get_capital(from_state);
			break;
		}
		break;
	case event::slot_type::province:
		from_province = trigger::to_prov(from_slot);
		from_capital = from_province;
		from_state = state.world.province_get_state_membership(from_province);
		from_nation = state.world.province_get_nation_from_province_ownership(from_province);
		break;
	default:
		// TODO: Handle rest
		break;
	}

	// Target
	text::add_to_substitution_map(sub, text::variable_type::continentname, state.world.province_get_continent(target_province).get_name());
	text::add_to_substitution_map(sub, text::variable_type::provincename, target_province);
	text::add_to_substitution_map(sub, text::variable_type::provinceculture, state.world.culture_get_name(state.world.province_get_dominant_culture(target_province)));
	text::add_to_substitution_map(sub, text::variable_type::provincereligion, state.world.religion_get_name(state.world.province_get_dominant_religion(target_province)));
	text::add_to_substitution_map(sub, text::variable_type::state, target_state);
	text::add_to_substitution_map(sub, text::variable_type::statename, target_state);
	text::add_to_substitution_map(sub, text::variable_type::country, target_nation);
	text::add_to_substitution_map(sub, text::variable_type::countryname, target_nation);
	text::add_to_substitution_map(sub, text::variable_type::thiscountry, target_nation);
	text::add_to_substitution_map(sub, text::variable_type::country_adj, state.world.nation_get_adjective(target_nation));
	text::add_to_substitution_map(sub, text::variable_type::cb_target_name, state.world.nation_get_constructing_cb_target(target_nation));
	text::add_to_substitution_map(sub, text::variable_type::cb_target_name_adj, state.world.nation_get_adjective(state.world.nation_get_constructing_cb_target(target_nation)));
	text::add_to_substitution_map(sub, text::variable_type::capital, target_capital);
	text::add_to_substitution_map(sub, text::variable_type::monarchtitle, state.world.national_identity_get_government_ruler_name(state.world.nation_get_identity_from_identity_holder(target_nation), state.world.nation_get_government_type(target_nation)));
	// TODO: Is this correct? I remember in vanilla it could vary
	auto pc = state.world.nation_get_primary_culture(target_nation);
	text::add_to_substitution_map(sub, text::variable_type::culture, state.world.culture_get_name(pc));
	text::add_to_substitution_map(sub, text::variable_type::culture_group_union, pc.get_group_from_culture_group_membership().get_identity_from_cultural_union_of().get_nation_from_identity_holder());
	text::add_to_substitution_map(sub, text::variable_type::union_adj, pc.get_group_from_culture_group_membership().get_identity_from_cultural_union_of().get_adjective());
	text::add_to_substitution_map(sub, text::variable_type::countryculture, state.world.culture_get_name(pc));
	auto sm = state.world.nation_get_in_sphere_of(target_nation);
	text::add_to_substitution_map(sub, text::variable_type::spheremaster, sm);
	text::add_to_substitution_map(sub, text::variable_type::spheremaster_adj, state.world.nation_get_adjective(sm));
	auto smpc = state.world.nation_get_primary_culture(sm);
	text::add_to_substitution_map(sub, text::variable_type::spheremaster_union_adj, smpc.get_group_from_culture_group_membership().get_identity_from_cultural_union_of().get_adjective());

	// From
	text::add_to_substitution_map(sub, text::variable_type::fromcountry, from_nation);
	text::add_to_substitution_map(sub, text::variable_type::fromcountry_adj, state.world.nation_get_adjective(from_nation));
	text::add_to_substitution_map(sub, text::variable_type::fromprovince, from_province);

	// Global crisis stuff
	text::add_to_substitution_map(sub, text::variable_type::crisistaker, state.crisis_liberation_tag);
	text::add_to_substitution_map(sub, text::variable_type::crisistaker_adj, state.world.national_identity_get_adjective(state.crisis_liberation_tag));
	text::add_to_substitution_map(sub, text::variable_type::crisistaker_capital, state.world.national_identity_get_capital(state.crisis_liberation_tag));
	text::add_to_substitution_map(sub, text::variable_type::crisistaker_continent, state.world.national_identity_get_capital(state.crisis_liberation_tag).get_continent().get_name());
	text::add_to_substitution_map(sub, text::variable_type::crisisattacker, state.primary_crisis_attacker);
	text::add_to_substitution_map(sub, text::variable_type::crisisattacker_capital, state.world.nation_get_capital(state.primary_crisis_attacker));
	text::add_to_substitution_map(sub, text::variable_type::crisisattacker_continent, state.world.nation_get_capital(state.primary_crisis_attacker).get_continent().get_name());
	text::add_to_substitution_map(sub, text::variable_type::crisisdefender, state.primary_crisis_defender);
	text::add_to_substitution_map(sub, text::variable_type::crisisdefender_capital, state.world.nation_get_capital(state.primary_crisis_defender));
	text::add_to_substitution_map(sub, text::variable_type::crisisdefender_continent, state.world.nation_get_capital(state.primary_crisis_defender).get_continent().get_name());
	text::add_to_substitution_map(sub, text::variable_type::crisistarget, state.primary_crisis_defender);
	text::add_to_substitution_map(sub, text::variable_type::crisistarget_adj, state.world.nation_get_adjective(state.primary_crisis_defender));
	text::add_to_substitution_map(sub, text::variable_type::crisisarea, state.crisis_state);
	text::add_to_substitution_map(sub, text::variable_type::temperature, text::fp_two_places{ state.crisis_temperature });
	// Dates
	text::add_to_substitution_map(sub, text::variable_type::year, int32_t(event_date.to_ymd(state.start_date).year));
	//text::add_to_substitution_map(sub, text::variable_type::month, text::localize_month(state, event_date.to_ymd(state.start_date).month));
	text::add_to_substitution_map(sub, text::variable_type::day, int32_t(event_date.to_ymd(state.start_date).day));

	// Non-vanilla
	text::add_to_substitution_map(sub, text::variable_type::government, state.world.nation_get_government_type(target_nation).get_name());
	text::add_to_substitution_map(sub, text::variable_type::ideology, state.world.nation_get_ruling_party(target_nation).get_ideology().get_name());
	text::add_to_substitution_map(sub, text::variable_type::party, state.world.nation_get_ruling_party(target_nation).get_name());
	text::add_to_substitution_map(sub, text::variable_type::pop, text::pretty_integer{ target_pop });
	text::add_to_substitution_map(sub, text::variable_type::fromcontinent, state.world.province_get_continent(from_province).get_name());
	text::add_to_substitution_map(sub, text::variable_type::fromcapital, state.world.province_get_name(from_capital));
	text::add_to_substitution_map(sub, text::variable_type::religion, state.world.religion_get_name(state.world.nation_get_religion(target_nation)));
	text::add_to_substitution_map(sub, text::variable_type::infamy, text::fp_two_places{ state.world.nation_get_infamy(target_nation) });
	text::add_to_substitution_map(sub, text::variable_type::badboy, text::fp_two_places{ state.world.nation_get_infamy(target_nation) });
	text::add_to_substitution_map(sub, text::variable_type::spheremaster, state.world.nation_get_in_sphere_of(target_nation));
	text::add_to_substitution_map(sub, text::variable_type::overlord, state.world.overlord_get_ruler(state.world.nation_get_overlord_as_subject(target_nation)));
	text::add_to_substitution_map(sub, text::variable_type::nationalvalue, state.world.nation_get_national_value(target_nation).get_name());
	text::add_to_substitution_map(sub, text::variable_type::good, state.world.province_get_rgo(target_province).get_name());
	text::add_to_substitution_map(sub, text::variable_type::resource, state.world.province_get_rgo(target_province).get_name());
	text::add_to_substitution_map(sub, text::variable_type::terrain, state.world.province_get_terrain(target_province).get_name());
	text::add_to_substitution_map(sub, text::variable_type::numfactories, text::pretty_integer{ economy::state_factory_count(state, target_state, target_nation) });
	text::add_to_substitution_map(sub, text::variable_type::fromruler, state.world.national_identity_get_government_ruler_name(state.world.nation_get_identity_from_identity_holder(from_nation), state.world.nation_get_government_type(from_nation)));
	text::add_to_substitution_map(sub, text::variable_type::focus, state.world.state_instance_get_owner_focus(target_state).get_name());
	text::add_to_substitution_map(sub, text::variable_type::nf, state.world.state_instance_get_owner_focus(target_state).get_name());
	auto names_pair = rng::get_random_pair(state, uint32_t(target_nation.index()) << 6, uint32_t(event_date.value));
	auto first_names = state.world.culture_get_first_names(state.world.nation_get_primary_culture(target_nation));
	if(first_names.size() > 0) {
		auto first_name = first_names.at(rng::reduce(uint32_t(names_pair.high), first_names.size()));
		auto last_names = state.world.culture_get_last_names(state.world.nation_get_primary_culture(target_nation));
		if(last_names.size() > 0) {
			auto last_name = last_names.at(rng::reduce(uint32_t(names_pair.high), last_names.size()));
			text::add_to_substitution_map(sub, text::variable_type::culture_first_name, state.to_string_view(first_name));
			text::add_to_substitution_map(sub, text::variable_type::culture_last_name, state.to_string_view(last_name));
		}
	}
	text::add_to_substitution_map(sub, text::variable_type::fromstatename, from_state);
	if(auto fl = state.world.province_get_factory_location_as_province(target_province); fl.begin() != fl.end()) {
		text::add_to_substitution_map(sub, text::variable_type::factory, (*fl.begin()).get_factory().get_building_type().get_name());
	}
	text::add_to_substitution_map(sub, text::variable_type::tech, state.world.nation_get_current_research(target_nation).get_name());
	//std::string list_provinces = "";
	//for(const auto p : state.world.state_definition_get_abstract_state_membership(state.world.state_instance_get_definition(target_state))) {
	//	list_provinces += text::produce_simple_string(state, p.get_province().get_name()) + ", ";
	//}
	//text::add_to_substitution_map(sub, text::variable_type::provinces, list_provinces);
	text::add_to_substitution_map(sub, text::variable_type::date, event_date);
	text::add_to_substitution_map(sub, text::variable_type::now, state.current_date);
	text::add_to_substitution_map(sub, text::variable_type::control, state.world.province_get_nation_from_province_control(target_province));
	text::add_to_substitution_map(sub, text::variable_type::owner, state.world.province_get_nation_from_province_ownership(target_province));
	if(auto plist = state.world.nation_get_province_ownership(target_nation); plist.begin() != plist.end()) {
		auto plist_size = uint32_t(plist.end() - plist.begin());
		uint32_t index = rng::reduce(uint32_t(names_pair.high), plist_size);
		text::add_to_substitution_map(sub, text::variable_type::anyprovince, (*(plist.begin() + index)).get_province());
	}
}

void event_option_button::on_update(sys::state& state) noexcept {
	event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);
	auto index = retrieve<int32_t>(state, parent);

	text::substitution_map sub;
	sys::event_option opt;
	if(std::holds_alternative<event::pending_human_n_event>(content)) {
		auto const& phe = std::get<event::pending_human_n_event>(content);
		opt = state.world.national_event_get_options(std::get<event::pending_human_n_event>(content).e)[index];
		populate_event_submap(state, sub, phe);
	} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
		auto const& phe = std::get<event::pending_human_f_n_event>(content);
		opt = state.world.free_national_event_get_options(std::get<event::pending_human_f_n_event>(content).e)[index];
		populate_event_submap(state, sub, phe);
	} else if(std::holds_alternative<event::pending_human_p_event>(content)) {
		auto const& phe = std::get<event::pending_human_p_event>(content);
		opt = state.world.provincial_event_get_options(std::get<event::pending_human_p_event>(content).e)[index];
		populate_event_submap(state, sub, phe);
	} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
		auto const& phe = std::get<event::pending_human_f_p_event>(content);
		opt = state.world.free_provincial_event_get_options(std::get<event::pending_human_f_p_event>(content).e)[index];
		populate_event_submap(state, sub, phe);
	}

	set_button_text(state, text::resolve_string_substitution(state, opt.name, sub));
}

void event_auto_button::button_action(sys::state& state) noexcept {
	event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);
	auto index = retrieve<int32_t>(state, parent);

	if(std::holds_alternative<event::pending_human_n_event>(content)) {
		auto const& phe = std::get<event::pending_human_n_event>(content);
		state.world.national_event_set_auto_choice(phe.e, uint8_t(index + 1));
		command::make_event_choice(state, phe, uint8_t(index));
	} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
		auto const& phe = std::get<event::pending_human_f_n_event>(content);
		state.world.free_national_event_set_auto_choice(phe.e, uint8_t(index + 1));
		command::make_event_choice(state, phe, uint8_t(index));
	} else if(std::holds_alternative<event::pending_human_p_event>(content)) {
		auto const& phe = std::get<event::pending_human_p_event>(content);
		state.world.provincial_event_set_auto_choice(phe.e, uint8_t(index + 1));
		command::make_event_choice(state, phe, uint8_t(index));
	} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
		auto const& phe = std::get<event::pending_human_f_p_event>(content);
		state.world.free_provincial_event_set_auto_choice(phe.e, uint8_t(index + 1));
		command::make_event_choice(state, phe, uint8_t(index));
	}
	send(state, parent, option_taken_notification{});
}

void event_option_button::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);
	auto index = retrieve<int32_t>(state, parent);

	if(std::holds_alternative<event::pending_human_n_event>(content)) {
		auto const& phe = std::get<event::pending_human_n_event>(content);
		if(index == 0) {
			text::add_line(state, contents, "event_taken_auto", text::variable_type::date, phe.date + 30);
		}
		effect_description(state, contents, state.world.national_event_get_options(phe.e)[index].effect, phe.primary_slot,
				trigger::to_generic(phe.n), phe.from_slot, phe.r_lo, phe.r_hi);
	} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
		auto const& phe = std::get<event::pending_human_f_n_event>(content);
		if(index == 0) {
			text::add_line(state, contents, "event_taken_auto", text::variable_type::date, phe.date + 30);
		}
		effect_description(state, contents, state.world.free_national_event_get_options(phe.e)[index].effect,
				trigger::to_generic(phe.n), trigger::to_generic(phe.n), -1, phe.r_lo, phe.r_hi);
	} else if(std::holds_alternative<event::pending_human_p_event>(content)) {
		auto const& phe = std::get<event::pending_human_p_event>(content);
		if(index == 0) {
			text::add_line(state, contents, "event_taken_auto", text::variable_type::date, phe.date + 30);
		}
		effect_description(state, contents, state.world.provincial_event_get_options(phe.e)[index].effect,
				trigger::to_generic(phe.p), trigger::to_generic(phe.p), phe.from_slot, phe.r_lo, phe.r_hi);
	} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
		auto const& phe = std::get<event::pending_human_f_p_event>(content);
		if(index == 0) {
			text::add_line(state, contents, "event_taken_auto", text::variable_type::date, phe.date + 30);
		}
		effect_description(state, contents, state.world.free_provincial_event_get_options(phe.e)[index].effect,
				trigger::to_generic(phe.p), trigger::to_generic(phe.p), -1, phe.r_lo, phe.r_hi);
	}
}

void event_option_button::button_action(sys::state& state) noexcept {
	event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);
	auto index = retrieve<int32_t>(state, parent);
	if(std::holds_alternative<event::pending_human_n_event>(content)) {
		auto phe = std::get<event::pending_human_n_event>(content);
		command::make_event_choice(state, phe, uint8_t(index));
	} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
		auto phe = std::get<event::pending_human_f_n_event>(content);
		command::make_event_choice(state, phe, uint8_t(index));
	} else if(std::holds_alternative<event::pending_human_p_event>(content)) {
		auto phe = std::get<event::pending_human_p_event>(content);
		command::make_event_choice(state, phe, uint8_t(index));
	} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
		auto phe = std::get<event::pending_human_f_p_event>(content);
		command::make_event_choice(state, phe, uint8_t(index));
	}
	send(state, parent, option_taken_notification{});
}

void event_image::on_update(sys::state& state) noexcept {
	event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);
	if(std::holds_alternative<event::pending_human_n_event>(content))
		base_data.data.image.gfx_object = state.world.national_event_get_image(std::get<event::pending_human_n_event>(content).e);
	else if(std::holds_alternative<event::pending_human_f_n_event>(content))
		base_data.data.image.gfx_object = state.world.free_national_event_get_image(std::get<event::pending_human_f_n_event>(content).e);
}

void event_desc_text::on_create(sys::state& state) noexcept {
	base_data.size.y = 200;
	base_data.size.x -= 11;
	scrollable_text::on_create(state);
	delegate->base_data.data.text.font_handle = text::name_into_font_id(state, "ToolTip_Font");
}

void event_desc_text::on_update(sys::state& state) noexcept {
	event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);

	auto contents = text::create_endless_layout(delegate->internal_layout,
			text::layout_parameters{0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y),
					delegate->base_data.data.text.font_handle, 0, text::alignment::left, delegate->black_text ? text::text_color::black : text::text_color::white, false});

	auto box = text::open_layout_box(contents);
	text::substitution_map sub{};
	dcon::text_sequence_id description{};
	if(std::holds_alternative<event::pending_human_n_event>(content)) {
		auto phe = std::get<event::pending_human_n_event>(content);
		description = state.world.national_event_get_description(phe.e);
		populate_event_submap(state, sub, phe);
	} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
		auto phe = std::get<event::pending_human_f_n_event>(content);
		description = state.world.free_national_event_get_description(phe.e);
		populate_event_submap(state, sub, phe);
	} else if(std::holds_alternative<event::pending_human_p_event>(content)) {
		auto phe = std::get<event::pending_human_p_event>(content);
		description = state.world.provincial_event_get_description(phe.e);
		populate_event_submap(state, sub, phe);
	} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
		auto phe = std::get<event::pending_human_f_p_event>(content);
		description = state.world.free_provincial_event_get_description(phe.e);
		populate_event_submap(state, sub, phe);
	}
	text::add_to_layout_box(state, contents, box, description, sub);
	text::close_layout_box(contents, box);

	if(std::holds_alternative<event::pending_human_n_event>(content)) {
		auto phe = std::get<event::pending_human_n_event>(content);
		auto imm = state.world.national_event_get_immediate_effect(phe.e);
		if(imm) {
			effect_description(state, contents, imm, phe.primary_slot, trigger::to_generic(phe.n), phe.from_slot, phe.r_lo, phe.r_hi);
			for(auto& l : delegate->internal_layout.contents) {
				if(l.color == (delegate->black_text ? text::text_color::white : text::text_color::black)) //Invert colours
					l.color = delegate->black_text ? text::text_color::black : text::text_color::white;
			}
		}
	} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
		auto phe = std::get<event::pending_human_f_n_event>(content);
		auto imm = state.world.free_national_event_get_immediate_effect(phe.e);
		if(imm) {
			effect_description(state, contents, imm, trigger::to_generic(phe.n), trigger::to_generic(phe.n), -1, phe.r_lo, phe.r_hi);
			for(auto& l : delegate->internal_layout.contents) {
				if(l.color == (delegate->black_text ? text::text_color::white : text::text_color::black)) //Invert colours
					l.color = delegate->black_text ? text::text_color::black : text::text_color::white;
			}
		}
	} else if(std::holds_alternative<event::pending_human_p_event>(content)) {
		auto phe = std::get<event::pending_human_p_event>(content);
		auto imm = state.world.provincial_event_get_immediate_effect(phe.e);
		if(imm) {
			effect_description(state, contents, imm, trigger::to_generic(phe.p), trigger::to_generic(phe.p), phe.from_slot, phe.r_lo, phe.r_hi);
			for(auto& l : delegate->internal_layout.contents) {
				if(l.color == (delegate->black_text ? text::text_color::white : text::text_color::black)) //Invert colours
					l.color = delegate->black_text ? text::text_color::black : text::text_color::white;
			}
		}
	} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
		auto phe = std::get<event::pending_human_f_p_event>(content);
		auto imm = state.world.free_provincial_event_get_immediate_effect(phe.e);
		if(imm) {
			effect_description(state, contents, imm, trigger::to_generic(phe.p), trigger::to_generic(phe.p), -1, phe.r_lo, phe.r_hi);
			for(auto& l : delegate->internal_layout.contents) {
				if(l.color == (delegate->black_text ? text::text_color::white : text::text_color::black)) //Invert colours
					l.color = delegate->black_text ? text::text_color::black : text::text_color::white;
			}
		}
	}

	calibrate_scrollbar(state);
}

void event_name_text::on_update(sys::state& state) noexcept {
	event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);

	auto contents = text::create_endless_layout(internal_layout,
			text::layout_parameters{0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y),
					base_data.data.text.font_handle, -15, text::alignment::center, black_text ? text::text_color::black : text::text_color::white, false});

	auto box = text::open_layout_box(contents);
	text::substitution_map sub{};
	dcon::text_sequence_id name{};
	if(std::holds_alternative<event::pending_human_n_event>(content)) {
		auto phe = std::get<event::pending_human_n_event>(content);
		name = state.world.national_event_get_name(phe.e);
		populate_event_submap(state, sub, phe);
	} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
		auto phe = std::get<event::pending_human_f_n_event>(content);
		name = state.world.free_national_event_get_name(phe.e);
		populate_event_submap(state, sub, phe);
	} else if(std::holds_alternative<event::pending_human_p_event>(content)) {
		auto phe = std::get<event::pending_human_p_event>(content);
		name = state.world.provincial_event_get_name(phe.e);
		populate_event_submap(state, sub, phe);
	} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
		auto phe = std::get<event::pending_human_f_p_event>(content);
		name = state.world.free_provincial_event_get_name(phe.e);
		populate_event_submap(state, sub, phe);
	}
	text::add_to_layout_box(state, contents, box, name, sub);
	text::close_layout_box(contents, box);
}

void event_requirements_icon::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, std::string_view("event_show_requirements"));
		text::close_layout_box(contents, box);
	}

	if(state.cheat_data.show_province_id_tooltip) {
		auto box = text::open_layout_box(contents);
		if(std::holds_alternative<event::pending_human_p_event>(content)) {
			auto phe = std::get<event::pending_human_p_event>(content);
			text::add_to_layout_box(state, contents, box, std::string_view("Province Event ID:"));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, std::to_string(phe.e.value));
		} else if(std::holds_alternative<event::pending_human_n_event>(content)) {
			auto phe = std::get<event::pending_human_n_event>(content);
			text::add_to_layout_box(state, contents, box, std::string_view("Nation Event ID:"));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, std::to_string(phe.e.value));
		} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
			auto phe = std::get<event::pending_human_f_p_event>(content);
			text::add_to_layout_box(state, contents, box, std::string_view("Free Province Event ID:"));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, std::to_string(phe.e.value));
		} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
			auto phe = std::get<event::pending_human_f_n_event>(content);
			text::add_to_layout_box(state, contents, box, std::string_view("Free Nation Event ID:"));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, std::to_string(phe.e.value));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, std::string_view("Legacy:"));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, std::to_string(state.world.free_national_event_get_legacy_id(phe.e)));
		}
		text::close_layout_box(contents, box);
	}

	if(std::holds_alternative<event::pending_human_p_event>(content) || std::holds_alternative<event::pending_human_n_event>(content)) {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, std::string_view("event_only_other_requirements"));
		text::close_layout_box(contents, box);
	} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
		auto phe = std::get<event::pending_human_f_p_event>(content);
		trigger_description(state, contents, state.world.free_provincial_event_get_trigger(phe.e), trigger::to_generic(phe.p),
				trigger::to_generic(phe.p), -1);
	} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
		auto phe = std::get<event::pending_human_f_n_event>(content);
		trigger_description(state, contents, state.world.free_national_event_get_trigger(phe.e), trigger::to_generic(phe.n),
				trigger::to_generic(phe.n), -1);
	}
}

void event_odds_icon::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, std::string_view("event_show_odds"));
		text::close_layout_box(contents, box);
	}
	if(std::holds_alternative<event::pending_human_p_event>(content) || std::holds_alternative<event::pending_human_n_event>(content)) {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, std::string_view("event_only_other_requirements"));
		text::close_layout_box(contents, box);
	} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
		auto phe = std::get<event::pending_human_f_p_event>(content);
		auto mod = state.world.free_provincial_event_get_mtth(phe.e);
		if(mod)
			multiplicative_value_modifier_description(state, contents, mod, trigger::to_generic(phe.p), trigger::to_generic(phe.p), 0);
		else
			text::add_line(state, contents, "always");
	} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
		auto phe = std::get<event::pending_human_f_n_event>(content);
		auto mod = state.world.free_national_event_get_mtth(phe.e);
		if(mod)
			multiplicative_value_modifier_description(state, contents, mod, trigger::to_generic(phe.n), trigger::to_generic(phe.n), 0);
		else
			text::add_line(state, contents, "always");
	}
}

void event_subtitle_text::on_update(sys::state& state) noexcept {
	auto iid = retrieve<dcon::issue_id>(state, parent);
	set_text(state, text::produce_simple_string(state, state.world.issue_get_name(iid)));
}

void event_state_name_text::on_update(sys::state& state) noexcept {
	event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);
	if(std::holds_alternative<event::pending_human_n_event>(content)) {
		auto n = dcon::fatten(state.world, std::get<event::pending_human_n_event>(content).n);
		set_text(state, text::produce_simple_string(state, n.get_name()));
	} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
		auto n = dcon::fatten(state.world, std::get<event::pending_human_f_n_event>(content).n);
		set_text(state, text::produce_simple_string(state, n.get_name()));
	} else if(std::holds_alternative<event::pending_human_p_event>(content)) {
		auto p = dcon::fatten(state.world, std::get<event::pending_human_p_event>(content).p);
		auto n = p.get_nation_from_province_ownership();
		set_text(state, text::produce_simple_string(state, n.get_name()));
	} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
		auto p = dcon::fatten(state.world, std::get<event::pending_human_f_p_event>(content).p);
		auto n = p.get_nation_from_province_ownership();
		set_text(state, text::produce_simple_string(state, n.get_name()));
	}
}

void event_population_amount_text::on_update(sys::state& state) noexcept {
	event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);
	if(std::holds_alternative<event::pending_human_n_event>(content)) {
		auto n = dcon::fatten(state.world, std::get<event::pending_human_n_event>(content).n);
		set_text(state, text::prettify(int64_t(n.get_demographics(demographics::total))));
	} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
		auto n = dcon::fatten(state.world, std::get<event::pending_human_f_n_event>(content).n);
		set_text(state, text::prettify(int64_t(n.get_demographics(demographics::total))));
	} else if(std::holds_alternative<event::pending_human_p_event>(content)) {
		auto p = dcon::fatten(state.world, std::get<event::pending_human_p_event>(content).p);
		auto n = p.get_nation_from_province_ownership();
		set_text(state, text::prettify(int64_t(n.get_demographics(demographics::total))));
	} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
		auto p = dcon::fatten(state.world, std::get<event::pending_human_f_p_event>(content).p);
		auto n = p.get_nation_from_province_ownership();
		set_text(state, text::prettify(int64_t(n.get_demographics(demographics::total))));
	}
}

void base_event_option_listbox::on_update(sys::state& state) noexcept {
	row_contents.clear();
	event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);
	if(std::holds_alternative<event::pending_human_n_event>(content)) {
		auto e = std::get<event::pending_human_n_event>(content).e;
		for(int32_t i = 0; i < sys::max_event_options; ++i) {
			if(!state.world.national_event_get_options(e)[i].effect && !state.world.national_event_get_options(e)[i].name)
				break;
			row_contents.push_back(i);
		}
	} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
		auto e = std::get<event::pending_human_f_n_event>(content).e;
		for(int32_t i = 0; i < sys::max_event_options; ++i) {
			if(!state.world.free_national_event_get_options(e)[i].effect && !state.world.free_national_event_get_options(e)[i].name)
				break;
			row_contents.push_back(i);
		}
	} else if(std::holds_alternative<event::pending_human_p_event>(content)) {
		auto e = std::get<event::pending_human_p_event>(content).e;
		for(int32_t i = 0; i < sys::max_event_options; ++i) {
			if(!state.world.provincial_event_get_options(e)[i].effect && !state.world.provincial_event_get_options(e)[i].name)
				break;
			row_contents.push_back(i);
		}
	} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
		auto e = std::get<event::pending_human_f_p_event>(content).e;
		for(int32_t i = 0; i < sys::max_event_options; ++i) {
			if(!state.world.free_provincial_event_get_options(e)[i].effect && !state.world.free_provincial_event_get_options(e)[i].name)
				break;
			row_contents.push_back(i);
		}
	}
	update(state);
}

void base_event_window::on_create(sys::state& state) noexcept {
	window_element_base::on_create(state);
	if(image) {
		move_child_to_front(image);
	}
	if(get_pool_slot() == event_pool_slot::province) {
		image->set_visible(state, false);
		{
			auto ptr = make_element_by_type<event_requirements_icon>(state, state.ui_state.defs_by_name.find("alice_event_requirements")->second.definition);
			ptr->base_data.position.y = int16_t(355);
			ptr->base_data.position.x += int16_t(ptr->base_data.size.x + 140);
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<event_odds_icon>(state, state.ui_state.defs_by_name.find("alice_event_odds")->second.definition);
			ptr->base_data.position.y = int16_t(355);
			ptr->base_data.position.x += int16_t(ptr->base_data.size.x * 2 + 140);
			add_child_to_front(std::move(ptr));
		}
		{
			xy_pair cur_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find(get_option_start_element_name())->second.definition].position;
			cur_offset.y += 22;
			auto ptr = make_element_by_type<provincial_event_listbox>(state, state.ui_state.defs_by_name.find("alice_povince_event_opts_list")->second.definition);
			ptr->base_data.position = cur_offset;
			ptr->base_data.position.y -= 150; // Omega nudge??
			add_child_to_front(std::move(ptr));
		}
	} else {
		{
			auto ptr = make_element_by_type<event_requirements_icon>(state, state.ui_state.defs_by_name.find("alice_event_requirements")->second.definition);
			ptr->base_data.position.y = 20;
			ptr->base_data.position.x += ptr->base_data.size.x;
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<event_odds_icon>(state, state.ui_state.defs_by_name.find("alice_event_odds")->second.definition);
			ptr->base_data.position.y = 20;
			ptr->base_data.position.x += ptr->base_data.size.x * 2;
			add_child_to_front(std::move(ptr));
		}
		xy_pair cur_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find(get_option_start_element_name())->second.definition].position;
		if(get_pool_slot() == event_pool_slot::country_election) {
			auto ptr = make_element_by_type<national_election_event_listbox>(state, state.ui_state.defs_by_name.find("alice_election_event_opts_list")->second.definition);
			ptr->base_data.position = cur_offset;
			ptr->base_data.position.y += 5;
			add_child_to_front(std::move(ptr));
		} else {
			auto ptr = make_element_by_type<national_event_listbox>(state, state.ui_state.defs_by_name.find("alice_nation_event_opts_list")->second.definition);
			ptr->base_data.position = cur_offset;
			ptr->base_data.position.y += 5;
			add_child_to_front(std::move(ptr));
		}
	}
}

class event_date_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);
		if(std::holds_alternative<event::pending_human_n_event>(content)) {
			set_text(state, text::date_to_string(state, std::get<event::pending_human_n_event>(content).date));
		} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
			set_text(state, text::date_to_string(state, std::get<event::pending_human_f_n_event>(content).date));
		} else if(std::holds_alternative<event::pending_human_p_event>(content)) {
			set_text(state, text::date_to_string(state, std::get<event::pending_human_p_event>(content).date));
		} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
			set_text(state, text::date_to_string(state, std::get<event::pending_human_f_p_event>(content).date));
		}
	}
};

class major_event_right_flag : public flag_button {
public:
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		dcon::gfx_object_id gid;
		if(base_data.get_element_type() == element_type::image) {
			gid = base_data.data.image.gfx_object;
		} else if(base_data.get_element_type() == element_type::button) {
			gid = base_data.data.button.button_image;
		}
		if(gid && flag_texture_handle > 0) {
			auto& gfx_def = state.ui_defs.gfx[gid];
			auto mask_handle = ogl::get_texture_handle(state, dcon::texture_id(gfx_def.type_dependent - 1), true);
			auto& mask_tex = state.open_gl.asset_textures[dcon::texture_id(gfx_def.type_dependent - 1)];
			ogl::render_masked_rect(state, get_color_modification(this == state.ui_state.under_mouse, disabled, interactable), float(x),
					float(y), float(base_data.size.x), float(base_data.size.y), flag_texture_handle, mask_handle, base_data.get_rotation(),
					gfx_def.is_vertically_flipped());
		}
		image_element_base::render(state, x, y);
	}
	void on_create(sys::state& state) noexcept override {
		std::swap(base_data.size.x, base_data.size.y);
		base_data.position.x += int16_t(24);
		base_data.position.y -= int16_t(25);
		flag_button::on_create(state);
	}
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		return state.world.nation_get_identity_from_identity_holder(n);
	}
	void button_action(sys::state& state) noexcept override {

	}
};

class major_event_left_flag : public flag_button {
public:
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		dcon::gfx_object_id gid;
		if(base_data.get_element_type() == element_type::image) {
			gid = base_data.data.image.gfx_object;
		} else if(base_data.get_element_type() == element_type::button) {
			gid = base_data.data.button.button_image;
		}
		if(gid && flag_texture_handle > 0) {
			auto& gfx_def = state.ui_defs.gfx[gid];
			auto mask_handle = ogl::get_texture_handle(state, dcon::texture_id(gfx_def.type_dependent - 1), true);
			auto& mask_tex = state.open_gl.asset_textures[dcon::texture_id(gfx_def.type_dependent - 1)];
			ogl::render_masked_rect(state, get_color_modification(this == state.ui_state.under_mouse, disabled, interactable), float(x),
				float(y), float(base_data.size.x), float(base_data.size.y), flag_texture_handle, mask_handle,
				ui::rotation::r90_right, false);

			ogl::render_textured_rect(state, get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
				float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
				ui::rotation::r90_right, false);
		}
	}
	void on_create(sys::state& state) noexcept override {
		std::swap(base_data.size.x, base_data.size.y);
		base_data.position.x += int16_t(21);
		base_data.position.y -= int16_t(25);
		flag_button::on_create(state);
	}
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		return state.world.nation_get_identity_from_identity_holder(n);
	}
	void button_action(sys::state& state) noexcept override {

	}
};

class event_province_text : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		color = text::text_color::dark_blue;
		event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);
		if(std::holds_alternative<event::pending_human_p_event>(content)) {
			set_text(state, text::produce_simple_string(state, state.world.province_get_name(std::get<event::pending_human_p_event>(content).p)));
		} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
			set_text(state, text::produce_simple_string(state, state.world.province_get_name(std::get<event::pending_human_f_p_event>(content).p)));
		}
	}

	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);
		dcon::province_id prov{};
		if(std::holds_alternative<event::pending_human_p_event>(content)) {
			prov = std::get<event::pending_human_p_event>(content).p;
		} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
			prov = std::get<event::pending_human_f_p_event>(content).p;
		}
		if(!prov || prov.value >= state.province_definitions.first_sea_province.value)
			return message_result::consumed;

		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
		state.map_state.set_selected_province(prov);
		static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, prov);
		if(state.map_state.get_zoom() < map::zoom_very_close)
			state.map_state.zoom = map::zoom_very_close;
		state.map_state.center_map_on_province(state, prov);
		return message_result::consumed;
	}
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		return type == mouse_probe_type::scroll ? message_result::unseen : message_result::consumed;
	}
};

class election_voter_issue_entry : public listbox_row_element_base<dcon::issue_option_id> {
private:
	simple_text_element_base* issname = nullptr;
	simple_text_element_base* issvalue = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "issue_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			issname = ptr.get();
			return ptr;
		} else if(name == "voters_perc") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			issvalue = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		issname->set_text(state, text::produce_simple_string(state, dcon::fatten(state.world, content).get_name()));
		auto total = state.world.nation_get_demographics(n, demographics::total);
		if(total > 0.f) {
			auto value = state.world.nation_get_demographics(n, demographics::to_key(state, content)) / total;
			issvalue->set_text(state, text::format_percentage(value));
		}
	}
};

class election_voter_issues_listbox : public listbox_element_base<election_voter_issue_entry, dcon::issue_option_id> {
protected:
	std::string_view get_row_element_name() override {
		return "ew_issue_option_window";
	}
public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		auto content = retrieve<dcon::issue_id>(state, parent);
		state.world.for_each_issue_option([&](dcon::issue_option_id id) {
			if(content == dcon::fatten(state.world, id).get_parent_issue().id) {
				row_contents.push_back(id);
			}
		});
		update(state);
	}
};
class election_party_entry : public listbox_row_element_base<dcon::political_party_id> {
	simple_text_element_base* partyname = nullptr;
	simple_text_element_base* partyvalue = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "legend_color") {
			return make_element_by_type<ideology_plupp>(state, id);
		} else if(name == "legend_title") {
			return make_element_by_type<generic_name_text<dcon::political_party_id>>(state, id);
		} else if(name == "legend_desc") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->base_data.position.x += 4;	// Nudge
			partyvalue = ptr.get();
			return ptr;
		} else if(name == "legend_value") {
			return make_element_by_type<invisible_element>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto issid = retrieve<dcon::issue_id>(state, parent);
		partyvalue->set_text(state, text::produce_simple_string(state, dcon::fatten(state.world, content).get_party_issues(issid).get_name()));
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::ideology_id>()) {
			payload.emplace<dcon::ideology_id>(state.world.political_party_get_ideology(content));
			return message_result::consumed;
		}
		return listbox_row_element_base<dcon::political_party_id>::get(state, payload);
	}

	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override {
		return parent->impl_on_scroll(state, x, y, amount, mods);
	}
};

class election_parties_listbox : public listbox_element_base<election_party_entry, dcon::political_party_id> {
protected:
	std::string_view get_row_element_name() override {
		return "eew_item";
	}
public:
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		nations::get_active_political_parties(state, n).swap(row_contents);
		update(state);
	}
};
class election_popularity_entry : public listbox_row_element_base<dcon::political_party_id> {
public:
	simple_text_element_base* partyname = nullptr;
	simple_text_element_base* partyvalue = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "legend_color") {
			return make_element_by_type<ideology_plupp>(state, id);
		} else if(name == "legend_title") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			partyname = ptr.get();
			return ptr;
		} else if(name == "legend_desc") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "legend_value") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->base_data.position.x += 3;	// Nudge
			partyvalue = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		auto fat = dcon::fatten(state.world, content);
		partyname->set_text(state, text::produce_simple_string(state, state.world.political_party_get_name(content)));
		auto const total = state.world.nation_get_demographics(n, demographics::total);
		if(total > 0.f) {
			partyvalue->set_text(state, text::format_percentage(state.world.nation_get_demographics(n, demographics::to_key(state, fat.get_ideology().id)) / total, 0));
		}
	}
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::ideology_id>()) {
			payload.emplace<dcon::ideology_id>(state.world.political_party_get_ideology(content));
			return message_result::consumed;
		}
		return listbox_row_element_base<dcon::political_party_id>::get(state, payload);
	}
};

class election_popularity_listbox : public listbox_element_base<election_popularity_entry, dcon::political_party_id> {
protected:
	std::string_view get_row_element_name() override {
		return "eew_item";
	}
public:
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		nations::get_active_political_parties(state, n).swap(row_contents);
		update(state);
	}
};

class election_voter_ideology_piechart : public voter_ideology_piechart {
public:
	void on_create(sys::state& state) noexcept override {
		base_data.position.x -= base_data.size.x / int16_t(2);
		base_data.position.x += int16_t(8);
		base_data.size.x *= 2;
		base_data.size.y *= 2;
		voter_ideology_piechart::on_create(state);
	}
};
class election_party_popularity_piechart : public piechart<dcon::political_party_id> {
public:
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		distribution.clear();
		auto total = state.world.nation_get_demographics(n, demographics::total);
		if(total > 0.f) {
			auto parties = nations::get_active_political_parties(state, n);
			for(auto ppid : state.world.in_political_party) {
				distribution.emplace_back(ppid.id, 0.0f);
			}
			for(auto ppid : parties) {
				auto const dkey = demographics::to_key(state, state.world.political_party_get_ideology(ppid));
				distribution[ppid.index()].value += state.world.nation_get_demographics(n, dkey) / total;
			}
		}
		update_chart(state);
	}
	void on_create(sys::state& state) noexcept override {
		piechart<dcon::political_party_id>::base_data.position.x -= piechart<dcon::political_party_id>::base_data.size.x / int16_t(2);
		piechart<dcon::political_party_id>::base_data.position.x += int16_t(8);
		piechart<dcon::political_party_id>::base_data.size.x *= 2;
		piechart<dcon::political_party_id>::base_data.size.y *= 2;
		piechart<dcon::political_party_id>::on_create(state);
	}
};

std::unique_ptr<element_base> election_issue_support_item::make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept {
	if(name == "issue_name") {
		return make_element_by_type<generic_name_text<dcon::issue_option_id>>(state, id);
	} else if(name == "people_perc") {
		return make_element_by_type<issue_option_popular_support>(state, id);
	} else if(name == "voters_perc") {
		return make_element_by_type<issue_option_voter_support>(state, id);
	} else {
		return nullptr;
	}
}

void elections_issue_option_listbox::on_update(sys::state& state) noexcept {
	row_contents.clear();
	auto iid = retrieve<dcon::issue_id>(state, parent);
	for(const auto ioid : state.world.in_issue_option) {
		if(ioid.get_parent_issue() == iid)
			row_contents.push_back(ioid);
	}
	update(state);
}

std::unique_ptr<element_base> base_event_window::make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept {
	if(name == "background") {
		return make_element_by_type<draggable_target>(state, id);
	} else if(name == "title") {
		return make_element_by_type<event_name_text>(state, id);
	} else if(name == "description") {
		return make_element_by_type<event_desc_text>(state, id);
	} else if(name == "event_images") {
		auto ptr = make_element_by_type<event_image>(state, id);
		image = ptr.get();
		return ptr;
	} else if(name == "event_separator_image") {
		auto ptr = make_element_by_type<image_element_base>(state, id);
		xy_pair cur_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find("event_province_option_start")->second.definition].position;
		ptr->base_data.position.y = cur_offset.y;
		ptr->base_data.position.y -= 150; // Omega nudge??
		return ptr;
	} else if(name == "date") {
		return make_element_by_type<event_date_text>(state, id);
	} else if(name == "state_name") {
		return make_element_by_type<event_state_name_text>(state, id);
	} else if(name == "subtitle") {
		return make_element_by_type<event_subtitle_text>(state, id);
	} else if(name == "population_amount") {
		return make_element_by_type<event_population_amount_text>(state, id);
	} else if(name == "country_flag1") {
		return make_element_by_type<major_event_left_flag>(state, id);
	} else if(name == "country_flag2") {
		return make_element_by_type<major_event_right_flag>(state, id);
	} else if(name == "province") {
		return make_element_by_type<event_province_text>(state, id);
	} else if(name == "perc_of_parlament") {
		return make_element_by_type<nation_ruling_party_ideology_text>(state, id);
	} else if(name == "parties_listbox") {
		return make_element_by_type<election_parties_listbox>(state, id);
	} else if(name == "chart_ideology") {
		return make_element_by_type<election_voter_ideology_piechart>(state, id);
	} else if(name == "chart_popularity") {
		return make_element_by_type<election_party_popularity_piechart>(state, id);
	} else if(name == "popularity_listbox") {
		return make_element_by_type<election_popularity_listbox>(state, id);
	} else if(name == "sort_by_issue_name") {
		auto ptr = make_element_by_type<politics_issue_sort_button>(state, id);
		ptr->order = politics_issue_sort_order::name;
		return ptr;
	} else if(name == "sort_by_people") {
		auto ptr = make_element_by_type<politics_issue_sort_button>(state, id);
		ptr->order = politics_issue_sort_order::popular_support;
		return ptr;
	} else if(name == "sort_by_voters") {
		auto ptr = make_element_by_type<politics_issue_sort_button>(state, id);
		ptr->order = politics_issue_sort_order::voter_support;
		return ptr;
	} else if(name == "issues_listbox") {
		auto ptr = make_element_by_type<elections_issue_option_listbox>(state, id);
		issues_listbox = ptr.get();
		return ptr;
	}
	return nullptr;
}

message_result base_event_window::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<dcon::nation_id>()) {
		payload.emplace<dcon::nation_id>(state.local_player_nation);
		return message_result::consumed;
	} else if(payload.holds_type<dcon::issue_id>()) {
		if(std::holds_alternative<event::pending_human_n_event>(event_data)) {
			auto e = std::get<event::pending_human_n_event>(event_data).e;
			auto iid = event::get_election_event_issue(state, e);
			payload.emplace<dcon::issue_id>(iid);
		}
		return message_result::consumed;
	} else if(payload.holds_type<event_data_wrapper>()) {
		if(std::holds_alternative<event::pending_human_n_event>(event_data))
			payload.emplace<event_data_wrapper>(std::get<event::pending_human_n_event>(event_data));
		else if(std::holds_alternative<event::pending_human_f_n_event>(event_data))
			payload.emplace<event_data_wrapper>(std::get<event::pending_human_f_n_event>(event_data));
		else if(std::holds_alternative<event::pending_human_p_event>(event_data))
			payload.emplace<event_data_wrapper>(std::get<event::pending_human_p_event>(event_data));
		else if(std::holds_alternative<event::pending_human_f_p_event>(event_data))
			payload.emplace<event_data_wrapper>(std::get<event::pending_human_f_p_event>(event_data));
		return message_result::consumed;
	} else if(payload.holds_type<option_taken_notification>()) {
		set_visible(state, false);
		auto uptr = state.ui_state.root->remove_child(this);
		if(uptr) {
			auto slot = uint8_t(get_pool_slot());
			std::unique_ptr<base_event_window> ptr(static_cast<base_event_window*>(uptr.release()));
			event_pool[slot].push_back(std::move(ptr));
			--pending_events[slot];
		}
		return message_result::consumed;
	} else if(payload.holds_type<politics_issue_sort_order>()) {
		auto enum_val = any_cast<politics_issue_sort_order>(payload);
		switch(enum_val) {
		case politics_issue_sort_order::name:
			std::sort(issues_listbox->row_contents.begin(), issues_listbox->row_contents.end(),
					[&](dcon::issue_option_id a, dcon::issue_option_id b) {
					auto a_name = text::get_name_as_string(state, dcon::fatten(state.world, a));
					auto b_name = text::get_name_as_string(state, dcon::fatten(state.world, b));
					return a_name < b_name;
					});
			issues_listbox->update(state);
			break;
		case politics_issue_sort_order::popular_support:
			std::sort(issues_listbox->row_contents.begin(), issues_listbox->row_contents.end(),
					[&](dcon::issue_option_id a, dcon::issue_option_id b) {
					auto a_support = politics::get_popular_support(state, state.local_player_nation, a);
					auto b_support = politics::get_popular_support(state, state.local_player_nation, b);
					return a_support > b_support;
					});
			issues_listbox->update(state);
			break;
		case politics_issue_sort_order::voter_support:
			std::sort(issues_listbox->row_contents.begin(), issues_listbox->row_contents.end(),
					[&](dcon::issue_option_id a, dcon::issue_option_id b) {
					auto a_support = politics::get_voter_support(state, state.local_player_nation, a);
					auto b_support = politics::get_voter_support(state, state.local_player_nation, b);
					return a_support > b_support;
					});
			issues_listbox->update(state);
			break;
		}
		return message_result::consumed;
	}
	return message_result::unseen;
}

void base_event_window::on_update(sys::state& state) noexcept {
	if(std::holds_alternative<event::pending_human_n_event>(event_data)) {
		if(std::get<event::pending_human_n_event>(event_data).date + event::expiration_in_days <= state.current_date) {
			pending_closure.push_back(this);
		}
	} else if(std::holds_alternative<event::pending_human_f_n_event>(event_data)) {
		if(std::get<event::pending_human_f_n_event>(event_data).date + event::expiration_in_days <= state.current_date) {
			pending_closure.push_back(this);
		}
	} else if(std::holds_alternative<event::pending_human_p_event>(event_data)) {
		if(std::get<event::pending_human_p_event>(event_data).date + event::expiration_in_days <= state.current_date) {
			pending_closure.push_back(this);
		}
	} else if(std::holds_alternative<event::pending_human_f_p_event>(event_data)) {
		if(std::get<event::pending_human_f_p_event>(event_data).date + event::expiration_in_days <= state.current_date) {
			pending_closure.push_back(this);
		}
	}
}

void close_expired_event_windows(sys::state& state) {
	for(auto e : pending_closure)
		send(state, e, option_taken_notification{});
	pending_closure.clear();
}

void clear_event_windows(sys::state& state) {
	for(auto const& slot_pool : ui::event_pool) {
		for(auto const& win : slot_pool) {
			pending_closure.push_back(win.get());
		}
	}
	close_expired_event_windows(state);
}

bool events_pause_test(sys::state& state) {
	bool a = ((state.user_settings.self_message_settings[int32_t(sys::message_setting_type::province_event)] & sys::message_response::pause) != 0 && ui::pending_events[uint8_t(ui::event_pool_slot::province)] > 0);
	bool b = ((state.user_settings.self_message_settings[int32_t(sys::message_setting_type::national_event)] & sys::message_response::pause) != 0 && ui::pending_events[uint8_t(ui::event_pool_slot::country)] > 0);
	bool c = ((state.user_settings.self_message_settings[int32_t(sys::message_setting_type::major_event)] & sys::message_response::pause) != 0 && ui::pending_events[uint8_t(ui::event_pool_slot::country_major)] > 0);
	return a || b || c;
}

template<typename T>
void new_event_window_1(sys::state& state, event_data_wrapper& dat, event_pool_slot e_slot) {
	auto const slot = uint8_t(e_slot);
	if(event_pool[slot].empty()) {
		auto new_elm = ui::make_element_by_type<T>(state, T::window_element_name);
		auto ptr = new_elm.get();
		T* actual = static_cast<T*>(ptr);
		actual->event_data = dat;
		actual->impl_on_update(state);
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	} else {
		std::unique_ptr<base_event_window> ptr = std::move(event_pool[slot].back());
		event_pool[slot].pop_back();
		ptr->event_data = dat;
		if(!ptr->is_visible())
			ptr->base_data.position = state.ui_defs.gui[state.ui_state.defs_by_name.find(T::window_element_name)->second.definition].position;
		ptr->set_visible(state, true);
		state.ui_state.root->add_child_to_front(std::move(ptr));
	}
}

void new_event_window(sys::state& state, event_data_wrapper dat) {
	auto slot = event_pool_slot::country;
	if(std::holds_alternative<event::pending_human_n_event>(dat)) {
		slot = event_pool_slot::country;
		auto e = std::get<event::pending_human_n_event>(dat).e;
		if(state.world.national_event_get_is_major(e)) {
			slot = event_pool_slot::country_major;
		} else if(event::get_election_event_issue(state, e)) {
			slot = event_pool_slot::country_election;
		}
	} else if(std::holds_alternative<event::pending_human_f_n_event>(dat)) {
		slot = event_pool_slot::country;
		auto e = std::get<event::pending_human_f_n_event>(dat).e;
		if(state.world.free_national_event_get_is_major(e)) {
			slot = event_pool_slot::country_major;
		}
	} else if(std::holds_alternative<event::pending_human_p_event>(dat)) {
		slot = event_pool_slot::province;
	} else if(std::holds_alternative<event::pending_human_f_p_event>(dat)) {
		slot = event_pool_slot::province;
	}
	switch(slot) {
	case event_pool_slot::country:
		new_event_window_1<national_event_window>(state, dat, slot);
		break;
	case event_pool_slot::country_major:
		new_event_window_1<national_major_event_window>(state, dat, slot);
		break;
	case event_pool_slot::country_election:
		new_event_window_1<national_election_event_window>(state, dat, slot);
		break;
	case event_pool_slot::province:
		new_event_window_1<provincial_event_window>(state, dat, slot);
		break;
	default:
		return;
	}
	++pending_events[uint8_t(slot)];
}

} // namespace ui
