#pragma once
#include "dcon_generated.hpp"
#include "commands.hpp"
#include "text.hpp"

namespace ui {

enum class leader_select_sort {
	attack, defense, recon, morale, reliability, org, speed, experience
};

class unset_leader_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(auto a = retrieve<dcon::army_id>(state, parent);  a) {
			command::change_general(state, state.local_player_nation, a, dcon::leader_id{});
		} else if(auto v = retrieve<dcon::navy_id>(state, parent); v){
			command::change_admiral(state, state.local_player_nation, v, dcon::leader_id{});
		}
		state.ui_state.change_leader_window->set_visible(state, false);
	}
};

class passive_leader_image : public image_element_base {
public:
	dcon::gfx_object_id default_img;

	void on_update(sys::state& state) noexcept override {
		if(!default_img)
			default_img = base_data.data.image.gfx_object;

		dcon::leader_id lid = retrieve<dcon::leader_id>(state, parent);
		
		auto pculture = state.world.nation_get_primary_culture(state.local_player_nation);
		auto ltype = pculture.get_group_from_culture_group_membership().get_leader();

		if(ltype && lid) {
			auto admiral = state.world.leader_get_is_admiral(lid);
			if(admiral) {
				auto arange = ltype.get_admirals();
				if(arange.size() > 0) {
					auto rval = rng::get_random(state, uint32_t(state.world.leader_get_since(lid).value), uint32_t(lid.value));
					auto in_range = rng::reduce(uint32_t(rval), arange.size());
					base_data.data.image.gfx_object = arange[in_range];
				}
			} else {
				auto grange = ltype.get_generals();
				if(grange.size() > 0) {
					auto rval = rng::get_random(state, uint32_t(state.world.leader_get_since(lid).value), uint32_t(lid.value));
					auto in_range = rng::reduce(uint32_t(rval), grange.size());
					base_data.data.image.gfx_object = grange[in_range];
				}
			}
		} else {
			base_data.data.image.gfx_object = default_img;
		}
	}
};

class passive_leader_name : public  simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		dcon::leader_id lid = retrieve<dcon::leader_id>(state, parent);
		if(lid) {
			set_text(state, std::string(state.to_string_view(state.world.leader_get_name(lid))));
		} else {
			set_text(state, text::produce_simple_string(state, "no_leader"));
		}
	}
};

class passive_leader_attack : public  color_text_element {
	void on_update(sys::state& state) noexcept override {
		dcon::leader_id lid = retrieve<dcon::leader_id>(state, parent);
		if(lid) {
			auto attack = state.world.leader_trait_get_attack(state.world.leader_get_background(lid)) + state.world.leader_trait_get_attack(state.world.leader_get_personality(lid));
			if(attack > 0) {
				set_text(state, std::string("+") + text::format_float(attack, 2));
				color = text::text_color::dark_green;
			} else if(attack == 0) {
				set_text(state, "0");
				color = text::text_color::black;
			} else {
				set_text(state, text::format_float(attack, 2));
				color = text::text_color::red;
			}
		} else {
			set_text(state, "0");
			color = text::text_color::black;
		}
	}
};
class passive_leader_defense : public  color_text_element {
	void on_update(sys::state& state) noexcept override {
		dcon::leader_id lid = retrieve<dcon::leader_id>(state, parent);
		if(lid) {
			auto v = state.world.leader_trait_get_defense(state.world.leader_get_background(lid)) + state.world.leader_trait_get_defense(state.world.leader_get_personality(lid));
			if(v > 0) {
				set_text(state, std::string("+") + text::format_float(v, 2));
				color = text::text_color::dark_green;
			} else if(v == 0) {
				set_text(state, "0");
				color = text::text_color::black;
			} else {
				set_text(state, text::format_float(v, 2));
				color = text::text_color::red;
			}
		} else {
			set_text(state, "0");
			color = text::text_color::black;
		}
	}
};
class passive_leader_org : public  color_text_element {
	void on_update(sys::state& state) noexcept override {
		dcon::leader_id lid = retrieve<dcon::leader_id>(state, parent);
		if(lid) {
			auto v = state.world.leader_trait_get_organisation(state.world.leader_get_background(lid)) + state.world.leader_trait_get_organisation(state.world.leader_get_personality(lid));
			if(v > 0) {
				set_text(state, std::string("+") + text::format_float(v, 2));
				color = text::text_color::dark_green;
			} else if(v == 0) {
				set_text(state, "0");
				color = text::text_color::black;
			} else {
				set_text(state, text::format_float(v, 2));
				color = text::text_color::red;
			}
		} else {
			set_text(state, "0");
			color = text::text_color::black;
		}
	}
};
class passive_leader_morale : public  color_text_element {
	void on_update(sys::state& state) noexcept override {
		dcon::leader_id lid = retrieve<dcon::leader_id>(state, parent);
		if(lid) {
			auto v = state.world.leader_trait_get_morale(state.world.leader_get_background(lid)) + state.world.leader_trait_get_morale(state.world.leader_get_personality(lid));
			if(v > 0) {
				set_text(state, std::string("+") + text::format_float(v, 2));
				color = text::text_color::dark_green;
			} else if(v == 0) {
				set_text(state, "0");
				color = text::text_color::black;
			} else {
				set_text(state, text::format_float(v, 2));
				color = text::text_color::red;
			}
		} else {
			set_text(state, "0");
			color = text::text_color::black;
		}
	}
};
class passive_leader_speed : public  color_text_element {
	void on_update(sys::state& state) noexcept override {
		dcon::leader_id lid = retrieve<dcon::leader_id>(state, parent);
		if(lid) {
			auto v = state.world.leader_trait_get_speed(state.world.leader_get_background(lid)) + state.world.leader_trait_get_speed(state.world.leader_get_personality(lid));
			if(v > 0) {
				set_text(state, std::string("+") + text::format_float(v, 2));
				color = text::text_color::dark_green;
			} else if(v == 0) {
				set_text(state, "0");
				color = text::text_color::black;
			} else {
				set_text(state, text::format_float(v, 2));
				color = text::text_color::red;
			}
		} else {
			set_text(state, "0");
			color = text::text_color::black;
		}
	}
};
class passive_leader_recon : public  color_text_element {
	void on_update(sys::state& state) noexcept override {
		dcon::leader_id lid = retrieve<dcon::leader_id>(state, parent);
		if(lid) {
			auto v = state.world.leader_trait_get_reconnaissance(state.world.leader_get_background(lid)) + state.world.leader_trait_get_reconnaissance(state.world.leader_get_personality(lid));
			if(v > 0) {
				set_text(state, std::string("+") + text::format_float(v, 2));
				color = text::text_color::dark_green;
			} else if(v == 0) {
				set_text(state, "0");
				color = text::text_color::black;
			} else {
				set_text(state, text::format_float(v, 2));
				color = text::text_color::red;
			}
		} else {
			set_text(state, "0");
			color = text::text_color::black;
		}
	}
};
class passive_leader_reliability : public  color_text_element {
	void on_update(sys::state& state) noexcept override {
		dcon::leader_id lid = retrieve<dcon::leader_id>(state, parent);
		if(lid) {
			auto v = state.world.leader_trait_get_reliability(state.world.leader_get_background(lid)) + state.world.leader_trait_get_reliability(state.world.leader_get_personality(lid));
			if(v > 0) {
				set_text(state, std::string("+") + text::format_float(v, 2));
				color = text::text_color::dark_green;
			} else if(v == 0) {
				set_text(state, "0");
				color = text::text_color::black;
			} else {
				set_text(state, text::format_float(v, 2));
				color = text::text_color::red;
			}
		} else {
			set_text(state, "0");
			color = text::text_color::black;
		}
	}
};
class passive_leader_exp : public  color_text_element {
	void on_update(sys::state& state) noexcept override {
		dcon::leader_id lid = retrieve<dcon::leader_id>(state, parent);
		if(lid) {
			auto v = state.world.leader_trait_get_experience(state.world.leader_get_background(lid)) + state.world.leader_trait_get_experience(state.world.leader_get_personality(lid));
			if(v > 0) {
				set_text(state, std::string("+") + text::format_float(v, 2));
				color = text::text_color::dark_green;
			} else if(v == 0) {
				set_text(state, "0");
				color = text::text_color::black;
			} else {
				set_text(state, text::format_float(v, 2));
				color = text::text_color::red;
			}
		} else {
			set_text(state, "0");
			color = text::text_color::black;
		}
	}
};

class sort_leader_attack : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		send(state, parent, leader_select_sort::attack);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "sort_by");
		text::add_space_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "trait_attack");
		text::close_layout_box(contents, box);
	}
};
class sort_leader_def : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		send(state, parent, leader_select_sort::defense);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "sort_by");
		text::add_space_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "trait_defend");
		text::close_layout_box(contents, box);
	}
};
class sort_leader_org : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		send(state, parent, leader_select_sort::org);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "sort_by");
		text::add_space_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "trait_organisation");
		text::close_layout_box(contents, box);
	}
};
class sort_leader_morale : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		send(state, parent, leader_select_sort::morale);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "sort_by");
		text::add_space_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "trait_morale");
		text::close_layout_box(contents, box);
	}
};
class sort_leader_speed : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		send(state, parent, leader_select_sort::speed);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "sort_by");
		text::add_space_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "trait_speed");
		text::close_layout_box(contents, box);
	}
};
class sort_leader_recon : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		send(state, parent, leader_select_sort::recon);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "sort_by");
		text::add_space_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "trait_reconaissance");
		text::close_layout_box(contents, box);
	}
};
class sort_leader_reliable : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		send(state, parent, leader_select_sort::reliability);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "sort_by");
		text::add_space_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "trait_reliability");
		text::close_layout_box(contents, box);
	}
};
class sort_leader_exp : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		send(state, parent, leader_select_sort::experience);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "sort_by");
		text::add_space_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "trait_experience");
		text::close_layout_box(contents, box);
	}
};

class set_leader_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto lid = retrieve<dcon::leader_id>(state, parent);
		if(auto a = retrieve<dcon::army_id>(state, parent);  a) {
			command::change_general(state, state.local_player_nation, a, lid);
		} else if(auto v = retrieve<dcon::navy_id>(state, parent); v) {
			command::change_admiral(state, state.local_player_nation, v, lid);
		}
		state.ui_state.change_leader_window->set_visible(state, false);
	}
};

class leader_select_row : public listbox_row_element_base<dcon::leader_id> {
public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base::on_create(state);
		base_data.position.y = base_data.position.x = 0;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "leader_button") {
			return make_element_by_type<set_leader_button>(state, id);
		} else if(name == "photo") {
			return make_element_by_type<passive_leader_image>(state, id);
		} else if(name == "leader_name") { 
			return make_element_by_type<passive_leader_name>(state, id);
		} else if(name == "unitleader_a") {
			return make_element_by_type < passive_leader_attack>(state, id);
		} else if(name == "unitleader_b") {
			return make_element_by_type <passive_leader_defense >(state, id);
		} else if(name == "unitleader_c") {
			return make_element_by_type < passive_leader_org>(state, id);
		} else if(name == "unitleader_d") {
			return make_element_by_type < passive_leader_morale>(state, id);
		} else if(name == "unitleader_e") {
			return make_element_by_type < passive_leader_speed>(state, id);
		} else if(name == "unitleader_f") {
			return make_element_by_type < passive_leader_recon>(state, id);
		} else if(name == "unitleader_g") {
			return make_element_by_type < passive_leader_reliability>(state, id);
		} else if(name == "unitleader_h") {
			return make_element_by_type < passive_leader_exp>(state, id);
		} else {
			return nullptr;
		}
	}
};

class leader_select_listbox : public listbox_element_base<leader_select_row, dcon::leader_id> {
protected:
	std::string_view get_row_element_name() override {
		return "alice_leader_entry";
	}

public:
	leader_select_sort sort_type = leader_select_sort::attack;

	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		if(auto a = retrieve<dcon::army_id>(state, parent);  a) {
			for(auto l : state.world.nation_get_leader_loyalty(state.local_player_nation)) {
				if(l.get_leader().get_is_admiral() == false) {
					if(bool(l.get_leader().get_army_from_army_leadership()) == false)
						row_contents.push_back(l.get_leader());
				}
			}
		} else if(auto v = retrieve<dcon::navy_id>(state, parent); v) {
			for(auto l : state.world.nation_get_leader_loyalty(state.local_player_nation)) {
				if(l.get_leader().get_is_admiral() == true) {
					if(bool(l.get_leader().get_navy_from_navy_leadership()) == false)
						row_contents.push_back(l.get_leader());
				}
			}
		}

		switch(sort_type) {
			case leader_select_sort::attack:
				std::sort(row_contents.begin(), row_contents.end(), [&](dcon::leader_id a, dcon::leader_id b) {
					auto av = state.world.leader_trait_get_attack(state.world.leader_get_background(a)) + state.world.leader_trait_get_attack(state.world.leader_get_personality(a));
					auto bv = state.world.leader_trait_get_attack(state.world.leader_get_background(b)) + state.world.leader_trait_get_attack(state.world.leader_get_personality(b));
					if(av != bv)
						return av > bv;
					else
						return a.index() < b.index();
				});
				break;
			case leader_select_sort::defense:
				std::sort(row_contents.begin(), row_contents.end(), [&](dcon::leader_id a, dcon::leader_id b) {
					auto av = state.world.leader_trait_get_defense(state.world.leader_get_background(a)) + state.world.leader_trait_get_defense(state.world.leader_get_personality(a));
					auto bv = state.world.leader_trait_get_defense(state.world.leader_get_background(b)) + state.world.leader_trait_get_defense(state.world.leader_get_personality(b));
					if(av != bv)
						return av > bv;
					else
						return a.index() < b.index();
				});
				break;
			case leader_select_sort::recon:
				std::sort(row_contents.begin(), row_contents.end(), [&](dcon::leader_id a, dcon::leader_id b) {
					auto av = state.world.leader_trait_get_reconnaissance(state.world.leader_get_background(a)) + state.world.leader_trait_get_reconnaissance(state.world.leader_get_personality(a));
					auto bv = state.world.leader_trait_get_reconnaissance(state.world.leader_get_background(b)) + state.world.leader_trait_get_reconnaissance(state.world.leader_get_personality(b));
					if(av != bv)
						return av > bv;
					else
						return a.index() < b.index();
				});
				break;
			case leader_select_sort::morale:
				std::sort(row_contents.begin(), row_contents.end(), [&](dcon::leader_id a, dcon::leader_id b) {
					auto av = state.world.leader_trait_get_morale(state.world.leader_get_background(a)) + state.world.leader_trait_get_morale(state.world.leader_get_personality(a));
					auto bv = state.world.leader_trait_get_morale(state.world.leader_get_background(b)) + state.world.leader_trait_get_morale(state.world.leader_get_personality(b));
					if(av != bv)
						return av > bv;
					else
						return a.index() < b.index();
				});
				break;
			case leader_select_sort::reliability:
				std::sort(row_contents.begin(), row_contents.end(), [&](dcon::leader_id a, dcon::leader_id b) {
					auto av = state.world.leader_trait_get_reliability(state.world.leader_get_background(a)) + state.world.leader_trait_get_reliability(state.world.leader_get_personality(a));
					auto bv = state.world.leader_trait_get_reliability(state.world.leader_get_background(b)) + state.world.leader_trait_get_reliability(state.world.leader_get_personality(b));
					if(av != bv)
						return av > bv;
					else
						return a.index() < b.index();
				});
				break;
			case leader_select_sort::org:
				std::sort(row_contents.begin(), row_contents.end(), [&](dcon::leader_id a, dcon::leader_id b) {
					auto av = state.world.leader_trait_get_organisation(state.world.leader_get_background(a)) + state.world.leader_trait_get_organisation(state.world.leader_get_personality(a));
					auto bv = state.world.leader_trait_get_organisation(state.world.leader_get_background(b)) + state.world.leader_trait_get_organisation(state.world.leader_get_personality(b));
					if(av != bv)
						return av > bv;
					else
						return a.index() < b.index();
				});
				break;
			case leader_select_sort::speed:
				std::sort(row_contents.begin(), row_contents.end(), [&](dcon::leader_id a, dcon::leader_id b) {
					auto av = state.world.leader_trait_get_speed(state.world.leader_get_background(a)) + state.world.leader_trait_get_speed(state.world.leader_get_personality(a));
					auto bv = state.world.leader_trait_get_speed(state.world.leader_get_background(b)) + state.world.leader_trait_get_speed(state.world.leader_get_personality(b));
					if(av != bv)
						return av > bv;
					else
						return a.index() < b.index();
				});
				break;
			case leader_select_sort::experience:
				std::sort(row_contents.begin(), row_contents.end(), [&](dcon::leader_id a, dcon::leader_id b) {
					auto av = state.world.leader_trait_get_experience(state.world.leader_get_background(a)) + state.world.leader_trait_get_experience(state.world.leader_get_personality(a));
					auto bv = state.world.leader_trait_get_experience(state.world.leader_get_background(b)) + state.world.leader_trait_get_experience(state.world.leader_get_personality(b));
					if(av != bv)
						return av > bv;
					else
						return a.index() < b.index();
				});
				break;
		}

		update(state);
	}
};

class leader_selection_window : public window_element_base {
public:

	dcon::army_id a;
	dcon::navy_id v;
	leader_select_listbox* lb = nullptr;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "select_leader_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "prestige_bar_frame") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "current_leader_prestige_bar") {
			return make_element_by_type<vertical_progress_bar>(state, id);
		} else if(name == "selected_photo") {
			return make_element_by_type<passive_leader_image>(state, id);
		} else if(name == "selected_leader_name") {
			return make_element_by_type<passive_leader_name>(state, id);
		} else if(name == "unitleader_a") {
			return make_element_by_type < passive_leader_attack>(state, id);
		} else if(name == "unitleader_b") {
			return make_element_by_type <passive_leader_defense >(state, id);
		} else if(name == "unitleader_c") {
			return make_element_by_type < passive_leader_org>(state, id);
		} else if(name == "unitleader_d") {
			return make_element_by_type < passive_leader_morale>(state, id);
		} else if(name == "unitleader_e") {
			return make_element_by_type < passive_leader_speed>(state, id);
		} else if(name == "unitleader_f") {
			return make_element_by_type < passive_leader_recon>(state, id);
		} else if(name == "unitleader_g") {
			return make_element_by_type < passive_leader_reliability>(state, id);
		} else if(name == "unitleader_h") {
			return make_element_by_type < passive_leader_exp>(state, id);
		} else if(name == "sort_prestige") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "sort_prestige_icon") {
			return nullptr;
		} else if(name == "sort_name") {
			return nullptr;
		} else if(name == "sort_a") {
			return make_element_by_type<sort_leader_attack>(state, id);
		} else if(name == "sort_b") {
			return make_element_by_type<sort_leader_def>(state, id);
		} else if(name == "sort_c") {
			return make_element_by_type<sort_leader_org>(state, id);
		} else if(name == "sort_d") {
			return make_element_by_type<sort_leader_morale>(state, id);
		} else if(name == "sort_e") {
			return make_element_by_type<sort_leader_speed>(state, id);
		} else if(name == "sort_f") {
			return make_element_by_type<sort_leader_recon>(state, id);
		} else if(name == "sort_g") {
			return make_element_by_type<sort_leader_reliable>(state, id);
		} else if(name == "sort_h") {
			return make_element_by_type<sort_leader_exp>(state, id);
		} else if(name == "back_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "noleader_button") {
			return make_element_by_type<unset_leader_button>(state, id);
		} else if(name == "leaderlist") {
			auto ptr =  make_element_by_type<leader_select_listbox>(state, id);
			lb = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::leader_id>()) {
			if(a)
				payload.emplace<dcon::leader_id>(state.world.army_get_general_from_army_leadership(a));
			else if(v)
				payload.emplace<dcon::leader_id>(state.world.navy_get_admiral_from_navy_leadership(v));
			return message_result::consumed;
		} else if(payload.holds_type<dcon::army_id>()) {
			payload.emplace<dcon::army_id>(a);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::navy_id>()) {
			payload.emplace<dcon::navy_id>(v);
			return message_result::consumed;
		} else if(payload.holds_type<leader_select_sort>()) {
			lb->sort_type = any_cast<leader_select_sort>(payload);
			lb->impl_on_update(state);
			return message_result::consumed;
		}
		return window_element_base::get(state, payload);
	}
};

void open_leader_selection(sys::state& state, dcon::army_id a, dcon::navy_id v, int32_t x, int32_t y) {
	leader_selection_window* win = static_cast<leader_selection_window*>(state.ui_state.change_leader_window);
	win->a = a;
	win->v = v;
	if(state.ui_state.change_leader_window->is_visible()) {
		state.ui_state.change_leader_window->impl_on_update(state);
	} else {
		state.ui_state.change_leader_window->set_visible(state, true);
	}
	state.ui_state.change_leader_window->base_data.position.x = int16_t(x);
	state.ui_state.change_leader_window->base_data.position.y= int16_t(std::clamp(y, 64, int32_t(state.ui_state.root->base_data.size.y - state.ui_state.change_leader_window->base_data.size.y)));
	state.ui_state.root->move_child_to_front(state.ui_state.change_leader_window);
}

}
