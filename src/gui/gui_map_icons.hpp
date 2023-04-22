#pragma once

#include "dcon_generated.hpp"
#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "province.hpp"
#include "text.hpp"
#include <algorithm>
#include <variant>

namespace ui {
class unit_panel_color : public generic_settable_element<image_element_base, dcon::nation_id> {
    int32_t get_frame(sys::state& state) noexcept {
        if(content == state.local_player_nation)
            return 0; // green, ourselves
        auto drid = state.world.get_diplomatic_relation_by_diplomatic_pair(state.local_player_nation, content);
        if(state.world.diplomatic_relation_get_are_allied(drid))
            return 0; // green, allies, puppets and us
        else {
            auto iid = state.world.nation_get_identity_from_identity_holder(content);
            for(auto wa : state.world.nation_get_war_participant(state.local_player_nation))
                for(auto o : wa.get_war().get_war_participant())
                    if(o.get_is_attacker() != wa.get_is_attacker() && o.get_nation().get_identity_from_identity_holder().id == iid)
                        return 1; // red, enemy
        }
        return 2; // gray, i.e not involved or irrelevant for us
    }
public:
	void on_update(sys::state& state) noexcept override {
        frame = get_frame(state);
	}
};

typedef std::variant<
    int32_t, // ships
    float // regiments
> unit_strength_wrapper;
class unit_strength_text : public generic_settable_element<simple_text_element_base, unit_strength_wrapper> {
public:
	void on_update(sys::state& state) noexcept override {
        if(std::holds_alternative<int32_t>(content))
            set_text(state, std::to_string(std::get<int32_t>(content)));
        else
            set_text(state, text::prettify(std::get<float>(content)));
	}
};

class unit_country_flag : public generic_settable_element<flag_button, dcon::nation_id> {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		return state.world.nation_get_identity_from_identity_holder(content);
	}

    void on_update(sys::state& state) noexcept override {
        set_current_nation(state, get_current_nation(state));
    }
};

class unit_icon_window : public generic_settable_element<window_element_base, dcon::province_id> {
    unit_strength_text* strength_text = nullptr;
    unit_country_flag* country_flag = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "unit_panel_country_flag") {
			auto ptr = make_element_by_type<unit_country_flag>(state, id);
            ptr->base_data.position.y -= 1; // Nudge
            country_flag = ptr.get();
            return ptr;
        } else if(name == "unit_panel_color")
			return make_element_by_type<unit_panel_color>(state, id);
        else if(name == "unit_strength") {
			auto ptr = make_element_by_type<unit_strength_text>(state, id);
            ptr->base_data.position.y -= 1; // Nudge
            strength_text = ptr.get();
            return ptr;
        } else
			return nullptr;
	}

	void on_update(sys::state& state) noexcept override {
        // Idempotency prohibits the widget from setting too much the elements into
        // otherwise redundant data, so only the first unit on a province will be shown
        // and only the first ship will be shown. Ships are given priority over armies.
        bool idempotency = false;
        bool has_navy = false;
        bool has_army = false;
        {
            int32_t strength = 0;
            state.world.province_for_each_navy_location_as_location(content, [&](dcon::navy_location_id id) {
                auto nid = state.world.navy_location_get_navy(id);
                if(!idempotency) {
                    Cyto::Any payload = state.world.navy_control_get_controller(state.world.navy_get_navy_control_as_navy(nid));
                    country_flag->impl_set(state, payload);
                }
                idempotency = has_navy = true;
                // Calculate strength
                state.world.navy_for_each_navy_membership_as_navy(nid, [&](dcon::navy_membership_id nmid) {
                    auto sid = state.world.navy_membership_get_ship(nmid);
                    ++strength;
                });
            });
            Cyto::Any payload = unit_strength_wrapper(strength);
            strength_text->impl_set(state, payload);
        }
        if(!has_navy) {
            float strength = 0.f;
            state.world.province_for_each_army_location_as_location(content, [&](dcon::army_location_id id) {
                auto aid = state.world.army_location_get_army(id);
                if(!idempotency) {
                    Cyto::Any payload = state.world.army_control_get_controller(state.world.army_get_army_control_as_army(aid));
                    country_flag->impl_set(state, payload);
                }
                idempotency = has_army = true;
                // Calculate strength
                state.world.army_for_each_army_membership_as_army(aid, [&](dcon::army_membership_id amid) {
                    auto rid = state.world.army_membership_get_regiment(amid);
                    strength += state.world.regiment_get_strength(rid) * state.defines.pop_size_per_regiment;
                });
            });
            Cyto::Any payload = unit_strength_wrapper(strength);
            strength_text->impl_set(state, payload);
        }
        set_visible(state, has_navy || has_army);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
        const auto mid_point = state.world.province_get_mid_point(content);
        const glm::vec2 screen_size{ float(state.x_size), float(state.y_size) };
        const auto screen_pos = state.map_display.map_to_screen(mid_point, screen_size);
		base_data.position.x = screen_pos.x;
        base_data.position.y = screen_pos.y;
        window_element_base::render(state, 0, 0);
	}
};
}
