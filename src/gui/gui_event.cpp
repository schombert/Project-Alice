#include "gui_event.hpp"
#include "triggers.hpp"

namespace ui {

std::vector<std::unique_ptr<national_major_event_window>> national_major_event_window::event_pool;
std::vector<ui::element_base*> pending_closure;

void  national_major_event_window::new_event(sys::state& state, event::pending_human_n_event const& dat) {
	if(event_pool.empty()) {
		auto new_elm = ui::make_element_by_type<ui::national_major_event_window>(state, "event_major_window");
		auto ptr = new_elm.get();
		national_major_event_window* actual = static_cast<national_major_event_window*>(ptr);
		actual->event_data = dat;
		actual->impl_on_update(state);
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	} else {
		std::unique_ptr<national_major_event_window> ptr = std::move(event_pool.back());
		event_pool.pop_back();
		ptr->event_data = dat;
		ptr->set_visible(state, true);
		state.ui_state.root->add_child_to_front(std::move(ptr));
	}
}
void  national_major_event_window::new_event(sys::state& state, event::pending_human_f_n_event const& dat) {
	if(event_pool.empty()) {
		auto new_elm = ui::make_element_by_type<ui::national_major_event_window>(state, "event_major_window");
		auto ptr = new_elm.get();
		national_major_event_window* actual = static_cast<national_major_event_window*>(ptr);
		actual->event_data = dat;
		actual->impl_on_update(state);
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	} else {
		std::unique_ptr<national_major_event_window> ptr = std::move(event_pool.back());
		event_pool.pop_back();
		ptr->event_data = dat;
		ptr->set_visible(state, true);
		state.ui_state.root->add_child_to_front(std::move(ptr));
	}
}

std::vector<std::unique_ptr<national_event_window>> national_event_window::event_pool;

void  national_event_window::new_event(sys::state& state, event::pending_human_n_event const& dat) {
	if(event_pool.empty()) {
		auto new_elm = ui::make_element_by_type<ui::national_event_window>(state, "event_country_window");
		auto ptr = new_elm.get();
		national_event_window* actual = static_cast<national_event_window*>(ptr);
		actual->event_data = dat;
		actual->impl_on_update(state);
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	} else {
		std::unique_ptr<national_event_window> ptr = std::move(event_pool.back());
		event_pool.pop_back();
		ptr->event_data = dat;
		ptr->set_visible(state, true);
		state.ui_state.root->add_child_to_front(std::move(ptr));
	}
}
void  national_event_window::new_event(sys::state& state, event::pending_human_f_n_event const& dat) {
	if(event_pool.empty()) {
		auto new_elm = ui::make_element_by_type<ui::national_event_window>(state, "event_country_window");
		auto ptr = new_elm.get();
		national_event_window* actual = static_cast<national_event_window*>(ptr);
		actual->event_data = dat;
		actual->impl_on_update(state);
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	} else {
		std::unique_ptr<national_event_window> ptr = std::move(event_pool.back());
		event_pool.pop_back();
		ptr->event_data = dat;
		ptr->set_visible(state, true);
		state.ui_state.root->add_child_to_front(std::move(ptr));
	}
}

std::vector<std::unique_ptr<provincial_event_window>> provincial_event_window::event_pool;

void  provincial_event_window::new_event(sys::state& state, event::pending_human_p_event const& dat) {
	if(event_pool.empty()) {
		auto new_elm = ui::make_element_by_type<ui::provincial_event_window>(state, "event_province_window");
		auto ptr = new_elm.get();
		provincial_event_window* actual = static_cast<provincial_event_window*>(ptr);
		actual->event_data = dat;
		actual->impl_on_update(state);
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	} else {
		std::unique_ptr<provincial_event_window> ptr = std::move(event_pool.back());
		event_pool.pop_back();
		ptr->event_data = dat;
		ptr->set_visible(state, true);
		state.ui_state.root->add_child_to_front(std::move(ptr));
	}
}
void  provincial_event_window::new_event(sys::state& state, event::pending_human_f_p_event const& dat) {
	if(event_pool.empty()) {
		auto new_elm = ui::make_element_by_type<ui::provincial_event_window>(state, "event_province_window");
		auto ptr = new_elm.get();
		provincial_event_window* actual = static_cast<provincial_event_window*>(ptr);
		actual->event_data = dat;
		actual->impl_on_update(state);
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	} else {
		std::unique_ptr<provincial_event_window> ptr = std::move(event_pool.back());
		event_pool.pop_back();
		ptr->event_data = dat;
		ptr->set_visible(state, true);
		state.ui_state.root->add_child_to_front(std::move(ptr));
	}
}

void populate_event_submap(sys::state& state, text::substitution_map& sub,
		std::variant<event::pending_human_n_event, event::pending_human_f_n_event, event::pending_human_p_event,
				event::pending_human_f_p_event> const& phe) noexcept {
	dcon::nation_id target_nation{};
	dcon::nation_id from_nation{};
	dcon::province_id target_province{};
	dcon::province_id from_province{};
	dcon::state_instance_id target_state{};
	dcon::state_instance_id from_state{};
	dcon::province_id target_capital{};

	event::slot_type ft = event::slot_type::none;
	int32_t from_slot = -1;

	event::slot_type pt = event::slot_type::none;
	int32_t primary_slot = -1;
	sys::date event_date;

	if(std::holds_alternative<event::pending_human_n_event>(phe)) {
		auto const& e = std::get<event::pending_human_n_event>(phe);
		target_nation = e.n;
		target_capital = state.world.nation_get_capital(target_nation);
		target_province = target_capital;
		target_state = state.world.province_get_state_membership(target_province);

		from_slot = e.from_slot;
		ft = e.ft;
		primary_slot = e.primary_slot;
		pt = e.pt;
		event_date = e.date;
	} else if(std::holds_alternative<event::pending_human_f_n_event>(phe)) {
		auto const& e = std::get<event::pending_human_f_n_event>(phe);
		target_nation = e.n;
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

		from_slot = e.from_slot;
		ft = e.ft;
		event_date = e.date;
	} else if(std::holds_alternative<event::pending_human_f_p_event>(phe)) {
		auto const& e = std::get<event::pending_human_f_p_event>(phe);
		target_nation = state.world.province_get_nation_from_province_ownership(e.p);
		target_state = state.world.province_get_state_membership(e.p);
		target_capital = state.world.state_instance_get_capital(target_state);
		target_province = e.p;
		event_date = e.date;
	}

	switch(ft) {
	case event::slot_type::nation:
		from_nation = trigger::to_nation(from_slot);
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
	text::add_to_substitution_map(sub, text::variable_type::country_adj, state.world.nation_get_adjective(target_nation));
	text::add_to_substitution_map(sub, text::variable_type::cb_target_name,
			state.world.nation_get_constructing_cb_target(target_nation));
	text::add_to_substitution_map(sub, text::variable_type::cb_target_name_adj,
			state.world.nation_get_adjective(state.world.nation_get_constructing_cb_target(target_nation)));
	text::add_to_substitution_map(sub, text::variable_type::capital, target_capital);
	text::add_to_substitution_map(sub, text::variable_type::monarchtitle, state.world.government_type_get_ruler_name(state.world.nation_get_government_type(target_nation)));
	// TODO: Is this correct? I remember in vanilla it could vary
	text::add_to_substitution_map(sub, text::variable_type::culture, state.world.culture_get_name(state.world.nation_get_primary_culture(target_nation)));
	text::add_to_substitution_map(sub, text::variable_type::culture_group_union, state.world.culture_get_name(state.world.nation_get_primary_culture(target_nation)));
	auto pc = state.world.nation_get_primary_culture(target_nation);
	auto pcg_adj = pc.get_group_from_culture_group_membership().get_identity_from_cultural_union_of().get_adjective();
	text::add_to_substitution_map(sub, text::variable_type::union_adj, pcg_adj);
	text::add_to_substitution_map(sub, text::variable_type::countryculture, state.world.culture_get_name(pc));

	// From
	text::add_to_substitution_map(sub, text::variable_type::fromcountry, from_nation);
	text::add_to_substitution_map(sub, text::variable_type::fromcountry_adj, state.world.nation_get_adjective(from_nation));
	text::add_to_substitution_map(sub, text::variable_type::fromprovince, from_province);
	
	// Global crisis stuff
	// TODO: crisisarea
	// text::add_to_substitution_map(sub, text::variable_type::crisisarea, state.crisis_colony);
	text::add_to_substitution_map(sub, text::variable_type::crisistaker, state.crisis_liberation_tag);
	text::add_to_substitution_map(sub, text::variable_type::crisistaker_adj,
			state.world.national_identity_get_adjective(state.crisis_liberation_tag));
	text::add_to_substitution_map(sub, text::variable_type::crisisattacker, state.primary_crisis_attacker);
	text::add_to_substitution_map(sub, text::variable_type::crisisdefender, state.primary_crisis_defender);
	text::add_to_substitution_map(sub, text::variable_type::crisistarget, state.primary_crisis_defender);
	text::add_to_substitution_map(sub, text::variable_type::crisistarget_adj,
			state.world.nation_get_adjective(state.primary_crisis_defender));
	text::add_to_substitution_map(sub, text::variable_type::crisisarea, state.crisis_state);
	// Dates
	text::add_to_substitution_map(sub, text::variable_type::year, int32_t(event_date.to_ymd(state.start_date).year));
	//text::add_to_substitution_map(sub, text::variable_type::month, text::localize_month(state, event_date.to_ymd(state.start_date).month));
	text::add_to_substitution_map(sub, text::variable_type::day, int32_t(event_date.to_ymd(state.start_date).day));
}

void event_option_button::on_update(sys::state& state) noexcept {
	event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);
	auto index = retrieve<int32_t>(state, parent);

	text::substitution_map sub;
	sys::event_option opt;
	if(std::holds_alternative<event::pending_human_n_event>(content)) {
		auto phe = std::get<event::pending_human_n_event>(content);
		opt = state.world.national_event_get_options(std::get<event::pending_human_n_event>(content).e)[index];
		populate_event_submap(state, sub, phe);
	} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
		auto phe = std::get<event::pending_human_f_n_event>(content);
		opt = state.world.free_national_event_get_options(std::get<event::pending_human_f_n_event>(content).e)[index];
		populate_event_submap(state, sub, phe);
	} else if(std::holds_alternative<event::pending_human_p_event>(content)) {
		auto phe = std::get<event::pending_human_p_event>(content);
		opt = state.world.provincial_event_get_options(std::get<event::pending_human_p_event>(content).e)[index];
		populate_event_submap(state, sub, phe);
	} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
		auto phe = std::get<event::pending_human_f_p_event>(content);
		opt = state.world.free_provincial_event_get_options(std::get<event::pending_human_f_p_event>(content).e)[index];
		populate_event_submap(state, sub, phe);
	}

	set_button_text(state, text::resolve_string_substitution(state, opt.name, sub));
}

void event_auto_button::button_action(sys::state& state) noexcept {

	event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);
	auto index = retrieve<int32_t>(state, parent);

	if(std::holds_alternative<event::pending_human_n_event>(content)) {
		auto phe = std::get<event::pending_human_n_event>(content);
		state.world.national_event_set_auto_choice(phe.e, uint8_t(index + 1));
		command::make_event_choice(state, phe, uint8_t(index));
	} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
		auto phe = std::get<event::pending_human_f_n_event>(content);
		state.world.free_national_event_set_auto_choice(phe.e, uint8_t(index + 1));
		command::make_event_choice(state, phe, uint8_t(index));
	} else if(std::holds_alternative<event::pending_human_p_event>(content)) {
		auto phe = std::get<event::pending_human_p_event>(content);
		state.world.provincial_event_set_auto_choice(phe.e, uint8_t(index + 1));
		command::make_event_choice(state, phe, uint8_t(index));
	} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
		auto phe = std::get<event::pending_human_f_p_event>(content);
		state.world.free_provincial_event_set_auto_choice(phe.e, uint8_t(index + 1));
		command::make_event_choice(state, phe, uint8_t(index));
	}
	Cyto::Any n_payload = option_taken_notification{};
	parent->impl_get(state, n_payload);
}

void event_option_button::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {

		event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);
		auto index = retrieve<int32_t>(state, parent);

		if(std::holds_alternative<event::pending_human_n_event>(content)) {
			auto phe = std::get<event::pending_human_n_event>(content);
			effect_description(state, contents, state.world.national_event_get_options(phe.e)[index].effect, phe.primary_slot,
					trigger::to_generic(phe.n), phe.from_slot, phe.r_lo, phe.r_hi);
		} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
			auto phe = std::get<event::pending_human_f_n_event>(content);
			effect_description(state, contents, state.world.free_national_event_get_options(phe.e)[index].effect,
					trigger::to_generic(phe.n), trigger::to_generic(phe.n), -1, phe.r_lo, phe.r_hi);
		} else if(std::holds_alternative<event::pending_human_p_event>(content)) {
			auto phe = std::get<event::pending_human_p_event>(content);
			effect_description(state, contents, state.world.provincial_event_get_options(phe.e)[index].effect,
					trigger::to_generic(phe.p), trigger::to_generic(phe.p), phe.from_slot, phe.r_lo, phe.r_hi);
		} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
			auto phe = std::get<event::pending_human_f_p_event>(content);
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
	Cyto::Any n_payload = option_taken_notification{};
	parent->impl_get(state, n_payload);
}

void event_image::on_update(sys::state& state) noexcept {
	event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);
	if(std::holds_alternative<event::pending_human_n_event>(content))
		base_data.data.image.gfx_object = state.world.national_event_get_image(std::get<event::pending_human_n_event>(content).e);
	else if(std::holds_alternative<event::pending_human_f_n_event>(content))
		base_data.data.image.gfx_object =
				state.world.free_national_event_get_image(std::get<event::pending_human_f_n_event>(content).e);
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
					delegate->base_data.data.text.font_handle, 0, text::alignment::left, text::text_color::black, false});

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
	calibrate_scrollbar(state);
}

void event_name_text::on_update(sys::state& state) noexcept {
	event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);

	auto contents = text::create_endless_layout(internal_layout,
			text::layout_parameters{0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y),
					base_data.data.text.font_handle, -15, text::alignment::center, text::text_color::black, false});

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

	if(std::holds_alternative<event::pending_human_p_event>(content) ||
			std::holds_alternative<event::pending_human_n_event>(content)) {
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
	if(std::holds_alternative<event::pending_human_p_event>(content) ||
			std::holds_alternative<event::pending_human_n_event>(content)) {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, std::string_view("event_only_other_requirements"));
		text::close_layout_box(contents, box);
	} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
		auto phe = std::get<event::pending_human_f_p_event>(content);
		multiplicative_value_modifier_description(state, contents, state.world.free_provincial_event_get_mtth(phe.e),
				trigger::to_generic(phe.p), trigger::to_generic(phe.p), 0);
	} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
		auto phe = std::get<event::pending_human_f_n_event>(content);
		multiplicative_value_modifier_description(state, contents, state.world.free_national_event_get_mtth(phe.e),
				trigger::to_generic(phe.n), trigger::to_generic(phe.n), 0);
	}
}


class alice_button_row : public listbox_row_element_base<int32_t> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "opt_button") {
			return make_element_by_type<event_option_button>(state, id);
		} else if(name == "auto_button") {
			return make_element_by_type<event_auto_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class nation_event_list : public listbox_element_base<alice_button_row, int32_t> {
protected:
	std::string_view get_row_element_name() override {
		return "alice_nation_event_button";
	}

public:
	leader_select_sort sort_type = leader_select_sort::attack;

	void on_update(sys::state& state) noexcept override {
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
};

void national_event_window::on_create(sys::state& state) noexcept {
	window_element_base::on_create(state);

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

	{
		xy_pair cur_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find("event_country_option_start")->second.definition].position;
		auto ptr = make_element_by_type<nation_event_list>(state, state.ui_state.defs_by_name.find("alice_nation_event_opts_list")->second.definition);
		ptr->base_data.position = cur_offset;
		ptr->base_data.position.y += 5;
		add_child_to_front(std::move(ptr));
	}
}

void national_major_event_window::on_create(sys::state& state) noexcept {
	window_element_base::on_create(state);

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

	//auto s3 = IsMajor ? "event_major_optionbutton" : "event_country_optionbutton";

	{
		xy_pair cur_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find("event_major_option_start")->second.definition].position;
		auto ptr = make_element_by_type<nation_event_list>(state, state.ui_state.defs_by_name.find("alice_nation_event_opts_list")->second.definition);
		ptr->base_data.position = cur_offset;
		add_child_to_front(std::move(ptr));
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

std::unique_ptr<element_base> national_event_window::make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept {
	if(name == "background") {
		return make_element_by_type<draggable_target>(state, id);
	} else if(name == "title") {
		return make_element_by_type<event_name_text>(state, id);
	} else if(name == "description") {
		return make_element_by_type<event_desc_text>(state, id);
	} else if(name == "event_images") {
		return make_element_by_type<event_image>(state, id);
	} else if(name == "date") {
		return make_element_by_type<event_date_text>(state, id);
	}
	return nullptr;
}

std::unique_ptr<element_base> national_major_event_window::make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept {
	if(name == "background") {
		return make_element_by_type<draggable_target>(state, id);
	} else if(name == "title") {
		return make_element_by_type<event_name_text>(state, id);
	} else if(name == "description") {
		return make_element_by_type<event_desc_text>(state, id);
	} else if(name == "event_images") {
		return make_element_by_type<event_image>(state, id);
	} else if(name == "date") {
		return make_element_by_type<event_date_text>(state, id);
	} else if(name == "country_flag1") {
		return make_element_by_type<major_event_left_flag>(state, id);
	} else if(name == "country_flag2") {
		return make_element_by_type<major_event_right_flag>(state, id);
	}
	return nullptr;
}

message_result national_event_window::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<dcon::nation_id>()) {
		payload.emplace<dcon::nation_id>(state.local_player_nation);
		return message_result::consumed;
	} else if(payload.holds_type<event_data_wrapper>()) {
		if(std::holds_alternative<event::pending_human_n_event>(event_data))
			payload.emplace<event_data_wrapper>(std::get<event::pending_human_n_event>(event_data));
		else if(std::holds_alternative<event::pending_human_f_n_event>(event_data))
			payload.emplace<event_data_wrapper>(std::get<event::pending_human_f_n_event>(event_data));
		return message_result::consumed;
	} else if(payload.holds_type<option_taken_notification>()) {
		set_visible(state, false);
		auto uptr = state.ui_state.root->remove_child(this);
		assert(uptr);
		std::unique_ptr<national_event_window> ptr(static_cast<national_event_window*>(uptr.release()));
		national_event_window::event_pool.push_back(std::move(ptr));
		return message_result::consumed;
	}
	return message_result::unseen;
}

message_result national_major_event_window::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<dcon::nation_id>()) {
		payload.emplace<dcon::nation_id>(state.local_player_nation);
		return message_result::consumed;
	} else if(payload.holds_type<event_data_wrapper>()) {
		if(std::holds_alternative<event::pending_human_n_event>(event_data))
			payload.emplace<event_data_wrapper>(std::get<event::pending_human_n_event>(event_data));
		else if(std::holds_alternative<event::pending_human_f_n_event>(event_data))
			payload.emplace<event_data_wrapper>(std::get<event::pending_human_f_n_event>(event_data));
		return message_result::consumed;
	} else if(payload.holds_type<option_taken_notification>()) {
		set_visible(state, false);
		auto uptr = state.ui_state.root->remove_child(this);
		assert(uptr);
		std::unique_ptr<national_major_event_window> ptr(static_cast<national_major_event_window*>(uptr.release()));
		national_major_event_window::event_pool.push_back(std::move(ptr));
		return message_result::consumed;
	}
	return message_result::unseen;
}

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
		dcon::province_id prov;
		event_data_wrapper content = retrieve<event_data_wrapper>(state, parent);
		if(std::holds_alternative<event::pending_human_p_event>(content)) {
			prov = std::get<event::pending_human_p_event>(content).p;
		} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
			prov = std::get<event::pending_human_f_p_event>(content).p;
		}
		if(!prov)
			return message_result::consumed;

		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
		state.map_state.set_selected_province(prov);
		static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, prov);

		if(state.map_state.get_zoom() < 8)
			state.map_state.zoom = 8.0f;

		auto map_pos = state.world.province_get_mid_point(prov);
		map_pos.x /= float(state.map_state.map_data.size_x);
		map_pos.y /= float(state.map_state.map_data.size_y);
		map_pos.y = 1.0f - map_pos.y;
		state.map_state.set_pos(map_pos);

		return message_result::consumed;
	}
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		return type == mouse_probe_type::scroll ? message_result::unseen : message_result::consumed;
	}
};

class province_event_list : public listbox_element_base<alice_button_row, int32_t> {
protected:
	std::string_view get_row_element_name() override {
		return "alice_prov_event_button";
	}

public:
	leader_select_sort sort_type = leader_select_sort::attack;

	void on_update(sys::state& state) noexcept override {
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
};

void provincial_event_window::on_create(sys::state& state) noexcept {
	window_element_base::on_create(state);

	{
		auto ptr = make_element_by_type<event_requirements_icon>(state,
				state.ui_state.defs_by_name.find("alice_event_requirements")->second.definition);
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
		xy_pair cur_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find("event_province_option_start")->second.definition].position;
		cur_offset.y += 22;

		auto ptr = make_element_by_type<province_event_list>(state, state.ui_state.defs_by_name.find("alice_povince_event_opts_list")->second.definition);
		ptr->base_data.position = cur_offset;
		ptr->base_data.position.y -= 150; // Omega nudge??
		add_child_to_front(std::move(ptr));
	}
}

std::unique_ptr<element_base> provincial_event_window::make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept {
	if(name == "background") {
		return make_element_by_type<draggable_target>(state, id);
	} else if(name == "event_images") {
		auto ptr = make_element_by_type<element_base>(state, id);
		ptr->set_visible(state, false);
		return ptr;
	} else if(name == "event_separator_image") {
		auto ptr = make_element_by_type<image_element_base>(state, id);

		xy_pair cur_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find("event_province_option_start")->second.definition].position;
		ptr->base_data.position.y = cur_offset.y;
		ptr->base_data.position.y -= 150; // Omega nudge??

		return ptr;
	} else if(name == "title") {
		auto ptr = make_element_by_type<event_name_text>(state, id);
		ptr->base_data.position.y += 6;
		return ptr;
	} else if(name == "description") {
		return make_element_by_type<event_desc_text>(state, id);
	} else if(name == "date") {
		return make_element_by_type<event_date_text>(state, id);
	} else if(name == "province") {
		return make_element_by_type<event_province_text>(state, id);
	} else {
		return nullptr;
	}
}

message_result provincial_event_window::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<dcon::nation_id>()) {
		payload.emplace<dcon::nation_id>(state.local_player_nation);
		return message_result::consumed;
	} else if(payload.holds_type<event_data_wrapper>()) {
		if(std::holds_alternative<event::pending_human_p_event>(event_data))
			payload.emplace<event_data_wrapper>(std::get<event::pending_human_p_event>(event_data));
		else if(std::holds_alternative<event::pending_human_f_p_event>(event_data))
			payload.emplace<event_data_wrapper>(std::get<event::pending_human_f_p_event>(event_data));
		return message_result::consumed;
	} else if(payload.holds_type<option_taken_notification>()) {
		set_visible(state, false);
		auto uptr = state.ui_state.root->remove_child(this);
		assert(uptr);
		std::unique_ptr<provincial_event_window> ptr(static_cast<provincial_event_window*>(uptr.release()));
		provincial_event_window::event_pool.push_back(std::move(ptr));
		return message_result::consumed;
	}
	return message_result::unseen;
}

void national_major_event_window::on_update(sys::state& state) noexcept {
	if(std::holds_alternative<event::pending_human_n_event>(event_data)) {
		if(std::get<event::pending_human_n_event>(event_data).date + event::expiration_in_days <= state.current_date) {
			pending_closure.push_back(this);
		}
	} else if(std::holds_alternative<event::pending_human_f_n_event>(event_data)) {
		if(std::get<event::pending_human_f_n_event>(event_data).date + event::expiration_in_days <= state.current_date) {
			pending_closure.push_back(this);
		}
	}
}
void national_event_window::on_update(sys::state& state) noexcept {
	if(std::holds_alternative<event::pending_human_n_event>(event_data)) {
		if(std::get<event::pending_human_n_event>(event_data).date + event::expiration_in_days <= state.current_date) {
			pending_closure.push_back(this);
		}
	} else if(std::holds_alternative<event::pending_human_f_n_event>(event_data)) {
		if(std::get<event::pending_human_f_n_event>(event_data).date + event::expiration_in_days <= state.current_date) {
			pending_closure.push_back(this);
		}
	}
}
void provincial_event_window::on_update(sys::state& state) noexcept {
	if(std::holds_alternative<event::pending_human_p_event>(event_data)) {
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
	for(auto e : pending_closure) {
		Cyto::Any p = option_taken_notification{ };
		e->impl_get(state, p);
	}
	pending_closure.clear();
}

} // namespace ui
