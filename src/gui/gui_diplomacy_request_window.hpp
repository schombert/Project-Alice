#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "diplomatic_messages.hpp"

namespace ui {

template<bool Left>
class diplomacy_request_lr_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		frame = Left ? 0 : 1;
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<bool>{Left};
			parent->impl_get(state, payload);
		}
	}
};

struct diplomacy_reply_taken_notification {
	int dummy = 0;
};
template<bool B>
class diplomacy_request_reply_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = diplomatic_message::message{};
			parent->impl_get(state, payload);
			diplomatic_message::message m = any_cast<diplomatic_message::message>(payload);
			command::respond_to_diplomatic_message(state, state.local_player_nation, m.from, m.type, B);

			Cyto::Any n_payload = diplomacy_reply_taken_notification{};
			parent->impl_get(state, n_payload);
		}
	}
};

class diplomacy_request_title_text : public generic_simple_text<diplomatic_message::message> {
public:
	std::string get_text(sys::state& state, diplomatic_message::message diplomacy_request) noexcept override {
		switch(diplomacy_request.type) {
		case diplomatic_message::type_t::none:
			return std::string("???");
		case diplomatic_message::type_t::access_request:
			return text::produce_simple_string(state, "askmilitaryaccess_button");
		case diplomatic_message::type_t::alliance_request:
			return text::produce_simple_string(state, "alliance_button");
		case diplomatic_message::type_t::call_ally_request:
			return text::produce_simple_string(state, "callally_button");
		case diplomatic_message::type_t::be_crisis_primary_defender:
			return text::produce_simple_string(state, "back_crisis_button");
		case diplomatic_message::type_t::be_crisis_primary_attacker:
			return text::produce_simple_string(state, "crisis_offer_button");
		case diplomatic_message::type_t::peace_offer:
			return text::produce_simple_string(state, "peace_di");
		case diplomatic_message::type_t::take_crisis_side_offer:
			return text::produce_simple_string(state, "crisis_take_side_offer");
		case diplomatic_message::type_t::crisis_peace_offer:
			return text::produce_simple_string(state, "crisis_peace_offer");
		}
		return std::string("???");
	}
};

class diplomacy_request_desc_text : public generic_multiline_text<diplomatic_message::message> {
public:
	void populate_layout(sys::state& state, text::endless_layout& contents,
			diplomatic_message::message diplomacy_request) noexcept override {
		auto box = text::open_layout_box(contents);

		text::substitution_map sub{};
		text::add_to_substitution_map(sub, text::variable_type::actor, state.world.nation_get_name(diplomacy_request.from));
		switch(diplomacy_request.type) {
		case diplomatic_message::type_t::none:
			break;
		case diplomatic_message::type_t::access_request:
			text::localised_format_box(state, contents, box, std::string_view("askmilitaryaccess_offer"), sub);
			break;
		case diplomatic_message::type_t::alliance_request:
			text::localised_format_box(state, contents, box, std::string_view("alliance_offer"), sub);
			break;
		case diplomatic_message::type_t::call_ally_request:
		{
			auto war = dcon::fatten(state.world, diplomacy_request.data.war);
			dcon::nation_id primary_attacker = state.world.war_get_primary_attacker(war);
			dcon::nation_id primary_defender = state.world.war_get_primary_defender(war);
			text::substitution_map wsub;
			text::add_to_substitution_map(wsub, text::variable_type::order, std::string_view(""));
			text::add_to_substitution_map(wsub, text::variable_type::second, state.world.nation_get_adjective(primary_defender));
			text::add_to_substitution_map(wsub, text::variable_type::second_country, primary_defender);
			text::add_to_substitution_map(wsub, text::variable_type::first, state.world.nation_get_adjective(primary_attacker));
			text::add_to_substitution_map(wsub, text::variable_type::third, war.get_over_tag());
			text::add_to_substitution_map(wsub, text::variable_type::state, war.get_over_state());
			auto war_name = text::resolve_string_substitution(state, state.world.war_get_name(war), wsub);
			text::add_to_substitution_map(sub, text::variable_type::list, war_name);
			text::localised_format_box(state, contents, box, std::string_view("callally_offer"), sub);
			break;
		}
		case diplomatic_message::type_t::be_crisis_primary_defender:
			text::localised_format_box(state, contents, box, std::string_view("back_crisis_offer"), sub);
			break;
		case diplomatic_message::type_t::be_crisis_primary_attacker:
			text::localised_format_box(state, contents, box, std::string_view("crisis_offer_offer"), sub);
			break;
		case diplomatic_message::type_t::peace_offer:
		{
			text::add_to_substitution_map(sub, text::variable_type::country, state.world.peace_offer_get_nation_from_pending_peace_offer(diplomacy_request.data.peace));
			text::localised_format_box(state, contents, box, std::string_view("peaceofferdesc"), sub);
			state.world.peace_offer_for_each_peace_offer_item_as_peace_offer(diplomacy_request.data.peace, [&state, &contents, &box](dcon::peace_offer_item_id poiid) {
				dcon::wargoal_id wg = state.world.peace_offer_item_get_wargoal(poiid);
				dcon::cb_type_id cbt = state.world.wargoal_get_type(wg);
				text::substitution_map sub;
				text::add_to_substitution_map(sub, text::variable_type::recipient, state.world.wargoal_get_target_nation(wg));
				text::add_to_substitution_map(sub, text::variable_type::from, state.world.wargoal_get_added_by(wg));
				if(state.world.wargoal_get_secondary_nation(wg))
					text::add_to_substitution_map(sub, text::variable_type::third, state.world.wargoal_get_secondary_nation(wg));
				else if(state.world.wargoal_get_associated_tag(wg))
					text::add_to_substitution_map(sub, text::variable_type::third, state.world.wargoal_get_associated_tag(wg));
				text::add_to_substitution_map(sub, text::variable_type::state, state.world.wargoal_get_associated_state(wg));
				text::add_to_layout_box(state, contents, box, state.world.cb_type_get_shortest_desc(cbt), sub);
			});
			break;
		}
		case diplomatic_message::type_t::take_crisis_side_offer:
			text::localised_format_box(state, contents, box, "crisis_take_side_offer_offer");
			break;
		case diplomatic_message::type_t::crisis_peace_offer:
			text::localised_format_box(state, contents, box, "crisis_peace_offer_offer");
			break;
		}
		text::close_layout_box(contents, box);
	}
};

class diplomacy_request_count_text : public simple_text_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		simple_text_element_base::on_create(state);
		black_text = false;
	}
};

class diplomacy_request_flag_button : public flag_button {
public:
	void on_create(sys::state& state) noexcept override {
		base_data.position.y -= 6;
		base_data.size.y += 32;

		base_data.position.x += 8;
		base_data.size.x -= 16;
	}

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
};

class diplomacy_request_player_flag_button : public diplomacy_request_flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, state.local_player_nation);
		return fat_id.get_identity_from_identity_holder();
	}
};

class diplomacy_request_window : public window_element_base {
	simple_text_element_base* count_text = nullptr;
	int32_t index = 0;

public:
	std::vector<diplomatic_message::message> messages;

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		xy_pair cur_pos{0, 0};
		{
			auto ptr = make_element_by_type<diplomacy_request_lr_button<false>>(state,
					state.ui_state.defs_by_name.find("alice_left_right_button")->second.definition);
			cur_pos.x = base_data.size.x - (ptr->base_data.size.x * 2);
			cur_pos.y = ptr->base_data.size.y * 1;
			ptr->base_data.position = cur_pos;
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<diplomacy_request_count_text>(state,
					state.ui_state.defs_by_name.find("alice_page_count")->second.definition);
			cur_pos.x -= ptr->base_data.size.x;
			ptr->base_data.position = cur_pos;
			count_text = ptr.get();
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<diplomacy_request_lr_button<true>>(state,
					state.ui_state.defs_by_name.find("alice_left_right_button")->second.definition);
			cur_pos.x -= ptr->base_data.size.x;
			ptr->base_data.position = cur_pos;
			add_child_to_front(std::move(ptr));
		}
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "title") {
			return make_element_by_type<diplomacy_request_title_text>(state, id);
		} else if(name == "description") {
			return make_element_by_type<diplomacy_request_desc_text>(state, id);
		} else if(name == "agreebutton") {
			return make_element_by_type<diplomacy_request_reply_button<true>>(state, id);
		} else if(name == "declinebutton") {
			return make_element_by_type<diplomacy_request_reply_button<false>>(state, id);
		} else if(name == "leftshield") {
			return make_element_by_type<diplomacy_request_player_flag_button>(state, id);
		} else if(name == "rightshield") {
			return make_element_by_type<diplomacy_request_flag_button>(state, id);
		} else if(name == "background") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			ptr->base_data.size = base_data.size;
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto it = std::remove_if(messages.begin(), messages.end(),
				[&](auto& m) { return m.when + diplomatic_message::expiration_in_days <= state.current_date; });
		auto r = std::distance(it, messages.end());
		messages.erase(it, messages.end());

		if(messages.empty()) {
			set_visible(state, false);
		}

		count_text->set_text(state, std::to_string(int32_t(index)) + "/" + std::to_string(int32_t(messages.size())));
	}
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(index >= int32_t(messages.size()))
			index = 0;
		else if(index < 0)
			index = int32_t(messages.size()) - 1;

		if(payload.holds_type<dcon::nation_id>()) {
			if(messages.empty()) {
				payload.emplace<dcon::nation_id>(dcon::nation_id{});
			} else {
				payload.emplace<dcon::nation_id>(messages[index].from);
			}
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<bool>>()) {
			bool b = any_cast<element_selection_wrapper<bool>>(payload).data;
			index += b ? -1 : +1;
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<diplomatic_message::message>()) {
			if(messages.empty()) {
				payload.emplace<diplomatic_message::message>(diplomatic_message::message{});
			} else {
				payload.emplace<diplomatic_message::message>(messages[index]);
			}
			return message_result::consumed;
		} else if(payload.holds_type<diplomacy_reply_taken_notification>()) {
			if(!messages.empty()) {
				messages.erase(messages.begin() + size_t(index));
				impl_on_update(state);
			}
			return message_result::consumed;
		}
		return window_element_base::get(state, payload);
	}
};

} // namespace ui
