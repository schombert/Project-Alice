#include "gui_event.hpp"
#include "triggers.hpp"

namespace ui {

static void populate_event_submap(sys::state& state, text::substitution_map& sub, const std::variant<event::pending_human_n_event, event::pending_human_f_n_event, event::pending_human_p_event, event::pending_human_f_p_event>& phe) noexcept {
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

	if(std::holds_alternative<event::pending_human_n_event>(phe)) {
		const auto& e = std::get<event::pending_human_n_event>(phe);
		target_nation = e.n;
		target_capital = state.world.nation_get_capital(target_nation);

		from_slot = e.from_slot;
		ft = e.ft;
		primary_slot = e.primary_slot;
		pt = e.pt;
	} else if(std::holds_alternative<event::pending_human_f_n_event>(phe)) {
		const auto& e = std::get<event::pending_human_f_n_event>(phe);
		target_nation = e.n;
		target_capital = state.world.nation_get_capital(target_nation);
	} else if(std::holds_alternative<event::pending_human_p_event>(phe)) {
		const auto& e = std::get<event::pending_human_p_event>(phe);
		target_nation = state.world.province_get_nation_from_province_ownership(e.p);
		target_state = state.world.province_get_state_membership(e.p);
		target_capital = state.world.state_instance_get_capital(target_state);
		target_province = e.p;

		from_slot = e.from_slot;
		ft = e.ft;
	} else if(std::holds_alternative<event::pending_human_f_p_event>(phe)) {
		const auto& e = std::get<event::pending_human_f_p_event>(phe);
		target_nation = state.world.province_get_nation_from_province_ownership(e.p);
		target_state = state.world.province_get_state_membership(e.p);
		target_capital = state.world.state_instance_get_capital(target_state);
		target_province = e.p;
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
	text::add_to_substitution_map(sub, text::variable_type::provincename, target_province);
	text::add_to_substitution_map(sub, text::variable_type::state, target_state);
	text::add_to_substitution_map(sub, text::variable_type::statename, target_state);
	text::add_to_substitution_map(sub, text::variable_type::country, target_nation);
	text::add_to_substitution_map(sub, text::variable_type::country_adj, state.world.nation_get_adjective(target_nation));
	text::add_to_substitution_map(sub, text::variable_type::cb_target_name, state.world.nation_get_constructing_cb_target(target_nation));
	text::add_to_substitution_map(sub, text::variable_type::cb_target_name_adj, state.world.nation_get_adjective(state.world.nation_get_constructing_cb_target(target_nation)));
	text::add_to_substitution_map(sub, text::variable_type::capital, target_capital);
	// From
	text::add_to_substitution_map(sub, text::variable_type::fromcountry, from_nation);
	text::add_to_substitution_map(sub, text::variable_type::fromcountry_adj, state.world.nation_get_adjective(from_nation));
	text::add_to_substitution_map(sub, text::variable_type::fromprovince, from_province);

	// Global crisis stuff
	// TODO: crisisarea
	//text::add_to_substitution_map(sub, text::variable_type::crisisarea, state.crisis_colony);
	text::add_to_substitution_map(sub, text::variable_type::crisistaker, state.crisis_liberation_tag);
	text::add_to_substitution_map(sub, text::variable_type::crisistaker_adj, state.world.national_identity_get_adjective(state.crisis_liberation_tag));
	text::add_to_substitution_map(sub, text::variable_type::crisisattacker, state.primary_crisis_attacker);
	text::add_to_substitution_map(sub, text::variable_type::crisisdefender, state.primary_crisis_defender);
	text::add_to_substitution_map(sub, text::variable_type::crisistarget, state.primary_crisis_defender);
	text::add_to_substitution_map(sub, text::variable_type::crisistarget_adj, state.world.nation_get_adjective(state.primary_crisis_defender));
}

void national_event_option_button::on_update(sys::state& state) noexcept {
	if(parent) {
		Cyto::Any payload = national_event_data_wrapper{};
		parent->impl_get(state, payload);
		national_event_data_wrapper content = any_cast<national_event_data_wrapper>(payload);
		dcon::text_sequence_id name{};
		if(std::holds_alternative<event::pending_human_n_event>(content))
			name = state.world.national_event_get_options(std::get<event::pending_human_n_event>(content).e)[index].name;
		else if(std::holds_alternative<event::pending_human_f_n_event>(content))
			name = state.world.free_national_event_get_options(std::get<event::pending_human_f_n_event>(content).e)[index].name;
		if(bool(name)) {
			set_button_text(state, text::produce_simple_string(state, name));
		} else {
			set_visible(state, false);
		}
	}
}

tooltip_behavior national_event_option_button::has_tooltip(sys::state& state) noexcept {
	return tooltip_behavior::variable_tooltip;
}

void national_event_option_button::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	if(parent) {
		Cyto::Any payload = national_event_data_wrapper{};
		parent->impl_get(state, payload);
		national_event_data_wrapper content = any_cast<national_event_data_wrapper>(payload);
		if(std::holds_alternative<event::pending_human_n_event>(content)) {
			auto phe = std::get<event::pending_human_n_event>(content);
			effect_description(state, contents, state.world.national_event_get_options(phe.e)[index].effect, phe.primary_slot, trigger::to_generic(phe.n), phe.from_slot, phe.r_lo, phe.r_hi);
		} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
			auto phe = std::get<event::pending_human_f_n_event>(content);
			effect_description(state, contents, state.world.free_national_event_get_options(phe.e)[index].effect, trigger::to_generic(phe.n), -1, -1, phe.r_lo, phe.r_hi);
		}
	}
}

void national_event_option_button::button_action(sys::state& state) noexcept {
	if(parent) {
		Cyto::Any payload = national_event_data_wrapper{};
		parent->impl_get(state, payload);
		national_event_data_wrapper content = any_cast<national_event_data_wrapper>(payload);
		if(std::holds_alternative<event::pending_human_n_event>(content)) {
			auto phe = std::get<event::pending_human_n_event>(content);
			command::make_event_choice(state, phe, index);
		} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
			auto phe = std::get<event::pending_human_f_n_event>(content);
			command::make_event_choice(state, phe, index);
		}
		Cyto::Any n_payload = option_taken_notification{};
		parent->impl_get(state, n_payload);
	}
}

void national_event_image::on_update(sys::state& state) noexcept {
	if(parent) {
		Cyto::Any payload = national_event_data_wrapper{};
		parent->impl_get(state, payload);
		national_event_data_wrapper content = any_cast<national_event_data_wrapper>(payload);
		if(std::holds_alternative<event::pending_human_n_event>(content))
			base_data.data.image.gfx_object = state.world.national_event_get_image(std::get<event::pending_human_n_event>(content).e);
		else if(std::holds_alternative<event::pending_human_f_n_event>(content))
			base_data.data.image.gfx_object = state.world.free_national_event_get_image(std::get<event::pending_human_f_n_event>(content).e);
	}
}

void national_event_desc_text::on_create(sys::state& state) noexcept {
	multiline_text_element_base::on_create(state);
	base_data.data.text.font_handle = text::name_into_font_id(state, "ToolTip_Font");
}

void national_event_desc_text::on_update(sys::state& state) noexcept {
	if(parent) {
		Cyto::Any payload = national_event_data_wrapper{};
		parent->impl_get(state, payload);
		national_event_data_wrapper content = any_cast<national_event_data_wrapper>(payload);

		auto contents = text::create_endless_layout(
			internal_layout,
			text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::left, text::text_color::black }
		);

		dcon::text_sequence_id description{};

		auto box = text::open_layout_box(contents);
		text::substitution_map sub{};
		if(std::holds_alternative<event::pending_human_n_event>(content)) {
			auto phe = std::get<event::pending_human_n_event>(content);
			description = state.world.national_event_get_description(phe.e);
			populate_event_submap(state, sub, phe);
		} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
			auto phe = std::get<event::pending_human_f_n_event>(content);
			description = state.world.free_national_event_get_description(phe.e);
			populate_event_submap(state, sub, phe);
		}
		text::add_to_layout_box(contents, state, box, description, sub);
		text::close_layout_box(contents, box);
	}
}

void national_event_name_text::on_update(sys::state& state) noexcept {
	if(parent) {
		Cyto::Any payload = national_event_data_wrapper{};
		parent->impl_get(state, payload);
		national_event_data_wrapper content = any_cast<national_event_data_wrapper>(payload);

		auto contents = text::create_endless_layout(
			internal_layout,
			text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::center, text::text_color::black }
		);

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
		}
		text::add_to_layout_box(contents, state, box, name, sub);
		text::close_layout_box(contents, box);
	}
}

template<bool IsMajor>
void national_event_window<IsMajor>::on_create(sys::state& state) noexcept {
	window_element_base::on_create(state);
	auto s1 = IsMajor ? "event_major_option_start" : "event_country_option_start";
	auto s2 = IsMajor ? "event_major_option_offset" : "event_country_option_offset";
	auto s3 = IsMajor ? "event_major_optionbutton" : "event_country_optionbutton";

	xy_pair cur_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find(s1)->second.definition].position;
	xy_pair offset = state.ui_defs.gui[state.ui_state.defs_by_name.find(s2)->second.definition].position;
	for(size_t i = 0; i < size_t(sys::max_event_options); ++i) {
		auto ptr = make_element_by_type<national_event_option_button>(state, state.ui_state.defs_by_name.find(s3)->second.definition);
		ptr->base_data.position = cur_offset;
		ptr->index = uint8_t(i);
		option_buttons[i] = ptr.get();
		add_child_to_front(std::move(ptr));
		cur_offset.x += offset.x;
		cur_offset.y += offset.y;
	}
	set_visible(state, false);
}

template<bool IsMajor>
std::unique_ptr<element_base> national_event_window<IsMajor>::make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept {
	if(name == "background") {
		auto bg_ptr = make_element_by_type<draggable_target>(state, id);
		xy_pair cur_pos{ 0, 0 };
		{
			auto ptr = make_element_by_type<event_lr_button<false>>(state, state.ui_state.defs_by_name.find("alice_left_right_button")->second.definition);
			cur_pos.x = bg_ptr->base_data.size.x - (ptr->base_data.size.x * 2);
			cur_pos.y = ptr->base_data.size.y * 1;
			ptr->base_data.position = cur_pos;
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<simple_text_element_base>(state, state.ui_state.defs_by_name.find("alice_page_count")->second.definition);
			cur_pos.x -= ptr->base_data.size.x;
			ptr->base_data.position = cur_pos;
			count_text = ptr.get();
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<event_lr_button<true>>(state, state.ui_state.defs_by_name.find("alice_left_right_button")->second.definition);
			cur_pos.x -= ptr->base_data.size.x;
			ptr->base_data.position = cur_pos;
			add_child_to_front(std::move(ptr));
		}
		return bg_ptr;
	} else if(name == "title") {
		return make_element_by_type<national_event_name_text>(state, id);
	} else if(name == "description") {
		return make_element_by_type<national_event_desc_text>(state, id);
	} else if(name == "image") {
		return make_element_by_type<national_event_image>(state, id);
	} else if(name == "date") {
		return make_element_by_type<simple_text_element_base>(state, id);
	} else {
		// Handling for the case of "major national" flavour
		// TODO: correct rotation for this flag
		if(name == "country_flag1") {
			auto ptr = make_element_by_type<flag_button>(state, id);
			ptr->base_data.flags &= ui::element_data::rotation_mask;
			ptr->base_data.flags |= uint8_t(ui::rotation::r90_left);
			ptr->base_data.position.x += 21;
			ptr->base_data.position.y -= 23;
			ptr->base_data.size.x -= 32;
			ptr->base_data.size.y += 42;
			return ptr;
		} else if(name == "country_flag2") {
			auto ptr = make_element_by_type<flag_button>(state, id);
			ptr->base_data.position.x += 21;
			ptr->base_data.position.y -= 23;
			ptr->base_data.size.x -= 32;
			ptr->base_data.size.y += 42;
			return ptr;
		}
		return nullptr;
	}
}

template<bool IsMajor>
void national_event_window<IsMajor>::on_update(sys::state& state) noexcept {
	for(auto e : option_buttons)
		e->set_visible(state, true);

	auto it = std::remove_if(events.begin(), events.end(), [&](auto& e) {
		sys::date date{};
		if(std::holds_alternative<event::pending_human_n_event>(e))
			date = std::get<event::pending_human_n_event>(e).date;
		else if(std::holds_alternative<event::pending_human_f_n_event>(e))
			date = std::get<event::pending_human_f_n_event>(e).date;
		return date + event::expiration_in_days <= state.current_date;
	});
	auto r = std::distance(it, events.end());
	events.erase(it, events.end());

	if(events.empty()) {
		set_visible(state, false);
	}
	count_text->set_text(state, std::to_string(index + 1) + "/" + std::to_string(events.size()));
}

template<bool IsMajor>
message_result national_event_window<IsMajor>::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(index >= int32_t(events.size()))
		index = 0;
	else if(index < 0)
		index = int32_t(events.size()) - 1;
	
	if(payload.holds_type<dcon::nation_id>()) {
		payload.emplace<dcon::nation_id>(state.local_player_nation);
		return message_result::consumed;
	} else if(payload.holds_type<national_event_data_wrapper>()) {
		if(events.empty()) {
			payload.emplace<national_event_data_wrapper>(national_event_data_wrapper{});
		} else {
			payload.emplace<national_event_data_wrapper>(events[index]);
		}
		return message_result::consumed;
	} else if(payload.holds_type<element_selection_wrapper<bool>>()) {
		bool b = any_cast<element_selection_wrapper<bool>>(payload).data;
		index += b ? -1 : +1;
		impl_on_update(state);
		return message_result::consumed;
	} else if(payload.holds_type<option_taken_notification>()) {
		if(!events.empty()) {
			events.erase(events.begin() + size_t(index));
			impl_on_update(state);
		}
		return message_result::consumed;
	}
	return message_result::unseen;
}

void provincial_event_option_button::on_update(sys::state& state) noexcept {
	if(parent) {
		Cyto::Any payload = provincial_event_data_wrapper{};
		parent->impl_get(state, payload);
		provincial_event_data_wrapper content = any_cast<provincial_event_data_wrapper>(payload);
		dcon::text_sequence_id name{};
		if(std::holds_alternative<event::pending_human_p_event>(content))
			name = state.world.provincial_event_get_options(std::get<event::pending_human_p_event>(content).e)[index].name;
		else if(std::holds_alternative<event::pending_human_f_p_event>(content))
			name = state.world.free_provincial_event_get_options(std::get<event::pending_human_f_p_event>(content).e)[index].name;
		if(bool(name)) {
			set_button_text(state, text::produce_simple_string(state, name));
			set_visible(state, true);
		} else {
			set_visible(state, false);
		}
	}
}

tooltip_behavior provincial_event_option_button::has_tooltip(sys::state& state) noexcept {
	return tooltip_behavior::variable_tooltip;
}

void provincial_event_option_button::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	if(parent) {
		Cyto::Any payload = provincial_event_data_wrapper{};
		parent->impl_get(state, payload);
		provincial_event_data_wrapper content = any_cast<provincial_event_data_wrapper>(payload);
		if(std::holds_alternative<event::pending_human_p_event>(content)) {
			auto phe = std::get<event::pending_human_p_event>(content);
			effect_description(state, contents, state.world.provincial_event_get_options(phe.e)[index].effect, trigger::to_generic(phe.p), trigger::to_generic(phe.p), phe.from_slot, phe.r_lo, phe.r_hi);
		} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
			auto phe = std::get<event::pending_human_f_p_event>(content);
			effect_description(state, contents, state.world.free_provincial_event_get_options(phe.e)[index].effect, trigger::to_generic(phe.p), trigger::to_generic(phe.p), -1, phe.r_lo, phe.r_hi);
		}
	}
}

void provincial_event_option_button::button_action(sys::state& state) noexcept {
	if(parent) {
		Cyto::Any payload = provincial_event_data_wrapper{};
		parent->impl_get(state, payload);
		provincial_event_data_wrapper content = any_cast<provincial_event_data_wrapper>(payload);
		if(std::holds_alternative<event::pending_human_p_event>(content)) {
			auto phe = std::get<event::pending_human_p_event>(content);
			command::make_event_choice(state, phe, index);
		} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
			auto phe = std::get<event::pending_human_f_p_event>(content);
			command::make_event_choice(state, phe, index);
		}
		Cyto::Any n_payload = option_taken_notification{};
		parent->impl_get(state, n_payload);
	}
}

void provincial_event_desc_text::on_create(sys::state& state) noexcept {
	multiline_text_element_base::on_create(state);
	base_data.data.text.font_handle = text::name_into_font_id(state, "ToolTip_Font");
}

void provincial_event_desc_text::on_update(sys::state& state) noexcept {
	if(parent) {
		Cyto::Any payload = provincial_event_data_wrapper{};
		parent->impl_get(state, payload);
		provincial_event_data_wrapper content = any_cast<provincial_event_data_wrapper>(payload);

		auto contents = text::create_endless_layout(
			internal_layout,
			text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::left, text::text_color::black }
		);

		auto box = text::open_layout_box(contents);
		text::substitution_map sub{};
		dcon::text_sequence_id description{};
		if(std::holds_alternative<event::pending_human_p_event>(content)) {
			auto phe = std::get<event::pending_human_p_event>(content);
			description = state.world.provincial_event_get_description(phe.e);
			populate_event_submap(state, sub, phe);
		} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
			auto phe = std::get<event::pending_human_f_p_event>(content);
			description = state.world.free_provincial_event_get_description(phe.e);
			populate_event_submap(state, sub, phe);
		}
		text::add_to_layout_box(contents, state, box, description, sub);
		text::close_layout_box(contents, box);
	}
}

void provincial_event_name_text::on_update(sys::state& state) noexcept {
	if(parent) {
		Cyto::Any payload = provincial_event_data_wrapper{};
		parent->impl_get(state, payload);
		provincial_event_data_wrapper content = any_cast<provincial_event_data_wrapper>(payload);

		auto contents = text::create_endless_layout(
			internal_layout,
			text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::center, text::text_color::black }
		);

		auto box = text::open_layout_box(contents);
		text::substitution_map sub{};
		dcon::text_sequence_id name{};
		if(std::holds_alternative<event::pending_human_p_event>(content)) {
			auto phe = std::get<event::pending_human_p_event>(content);
			name = state.world.provincial_event_get_name(phe.e);
			populate_event_submap(state, sub, phe);
		} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
			auto phe = std::get<event::pending_human_f_p_event>(content);
			name = state.world.free_provincial_event_get_name(phe.e);
			populate_event_submap(state, sub, phe);
		}
		text::add_to_layout_box(contents, state, box, name, sub);
		text::close_layout_box(contents, box);
	}
}

void provincial_event_window::on_create(sys::state& state) noexcept {
	window_element_base::on_create(state);
	xy_pair cur_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find("event_province_option_start")->second.definition].position;
	xy_pair offset = state.ui_defs.gui[state.ui_state.defs_by_name.find("event_province_option_offset")->second.definition].position;
	for(size_t i = 0; i < size_t(sys::max_event_options); ++i) {
		auto ptr = make_element_by_type<provincial_event_option_button>(state, state.ui_state.defs_by_name.find("event_province_optionbutton")->second.definition);
		ptr->base_data.position = cur_offset;
		ptr->base_data.position.y -= 150; // Omega nudge??
		ptr->index = uint8_t(i);
		option_buttons[i] = ptr.get();
		add_child_to_front(std::move(ptr));
		cur_offset.x += offset.x;
		cur_offset.y += offset.y;
	}
	set_visible(state, false);
}
std::unique_ptr<element_base> provincial_event_window::make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept {
	if(name == "background") {
		auto bg_ptr = make_element_by_type<draggable_target>(state, id);
		xy_pair cur_pos{ 0, 0 };
		{
			auto ptr = make_element_by_type<event_lr_button<false>>(state, state.ui_state.defs_by_name.find("alice_left_right_button")->second.definition);
			cur_pos.x = bg_ptr->base_data.size.x - (ptr->base_data.size.x * 2);
			cur_pos.y = ptr->base_data.size.y * 1;
			ptr->base_data.position = cur_pos;
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<simple_text_element_base>(state, state.ui_state.defs_by_name.find("alice_page_count")->second.definition);
			cur_pos.x -= ptr->base_data.size.x;
			ptr->base_data.position = cur_pos;
			count_text = ptr.get();
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<event_lr_button<true>>(state, state.ui_state.defs_by_name.find("alice_left_right_button")->second.definition);
			cur_pos.x -= ptr->base_data.size.x;
			ptr->base_data.position = cur_pos;
			add_child_to_front(std::move(ptr));
		}
		return bg_ptr;
	} else if(name == "title") {
		return make_element_by_type<provincial_event_name_text>(state, id);
	} else if(name == "description") {
		return make_element_by_type<provincial_event_desc_text>(state, id);
	} else if(name == "date") {
		return make_element_by_type<simple_text_element_base>(state, id);
	} else {
		return nullptr;
	}
}
void provincial_event_window::on_update(sys::state& state) noexcept {
	for(auto e : option_buttons)
		e->set_visible(state, true);
	if(events.empty()) {
		set_visible(state, false);
	} else {
		for(auto it = events.begin(); it != events.end(); it++) {
			sys::date date{};
			if(std::holds_alternative<event::pending_human_p_event>(*it))
				date = std::get<event::pending_human_p_event>(*it).date;
			else if(std::holds_alternative<event::pending_human_f_p_event>(*it))
				date = std::get<event::pending_human_f_p_event>(*it).date;
			if(date + event::expiration_in_days <= state.current_date) {
				// Remove expired events
				events.erase(it);
				it--;
			}
		}
	}
	count_text->set_text(state, std::to_string(index + 1) + "/" + std::to_string(events.size()));
}
message_result provincial_event_window::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(index >= int32_t(events.size()))
		index = 0;
	else if(index < 0)
		index = int32_t(events.size()) - 1;
	
	if(payload.holds_type<dcon::nation_id>()) {
		payload.emplace<dcon::nation_id>(state.local_player_nation);
		return message_result::consumed;
	} else if(payload.holds_type<provincial_event_data_wrapper>()) {
		if(events.empty()) {
			payload.emplace<provincial_event_data_wrapper>(provincial_event_data_wrapper{});
		} else {
			payload.emplace<provincial_event_data_wrapper>(events[index]);
		}
		return message_result::consumed;
	} else if(payload.holds_type<element_selection_wrapper<bool>>()) {
		bool b = any_cast<element_selection_wrapper<bool>>(payload).data;
		index += b ? -1 : +1;
		impl_on_update(state);
		return message_result::consumed;
	} else if(payload.holds_type<option_taken_notification>()) {
		if(!events.empty()) {
			events.erase(events.begin() + size_t(index));
			impl_on_update(state);
		}
		return message_result::consumed;
	}
	return message_result::unseen;
}

}
