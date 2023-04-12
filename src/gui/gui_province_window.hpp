#pragma once

#include "dcon_generated.hpp"
#include "demographics.hpp"
#include "gui_element_types.hpp"
#include "gui_common_elements.hpp"
#include "gui_graphics.hpp"
#include "nations.hpp"
#include "province.hpp"
#include "system_state.hpp"
#include "text.hpp"

namespace ui {

class province_close_button : public generic_close_button {
public:
	void button_action(sys::state& state) noexcept override {
		state.map_display.set_selected_province(dcon::province_id{});
        generic_close_button::button_action(state);
	}
};

class province_terrain_image : public opaque_element_base {
private:
	dcon::province_id province_id{};

public:
	void on_update(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, province_id);
		auto terrain_id = fat_id.get_terrain().id;
		auto terrain_image = state.province_definitions.terrain_to_gfx_map[terrain_id];
		if(base_data.get_element_type() == element_type::image) {
			base_data.data.image.gfx_object = terrain_image;
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			province_id = any_cast<dcon::province_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		// TODO: display terrain modifier values
		auto fat_id = dcon::fatten(state.world, province_id);
		auto name = fat_id.get_terrain().get_name();
		if(name) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(contents, state, box, name, text::substitution_map{});
			text::close_layout_box(contents, box);
		}
	}
};

class slave_state_icon : public pop_type_icon {
public:
	void on_create(sys::state& state) noexcept override {
		pop_type_icon::on_create(state);
		auto fat_id = dcon::fatten(state.world, state.culture_definitions.slaves);
		pop_type_id = fat_id.id;
		update(state);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			auto province_id = any_cast<dcon::province_id>(payload);
			auto fat_id = dcon::fatten(state.world, province_id);
			set_visible(state, fat_id.get_is_slave());
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class province_admin_icon : public pop_type_icon {
public:
	void on_create(sys::state& state) noexcept override {
		pop_type_icon::on_create(state);
		auto fat_id = dcon::fatten(state.world, state.culture_definitions.bureaucrat);
		pop_type_id = fat_id.id;
		update(state);
	}
};

class province_flashpoint_indicator : public standard_province_icon {
public:
	void on_update(sys::state& state) noexcept override {
		standard_province_icon::on_update(state);
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto tension = fat_id.get_state_membership().get_flashpoint_tension();
		set_visible(state, tension > 0.f);
	}
};

class province_controller_flag : public flag_button {
private:
	dcon::province_id province_id{};

public:
	void on_create(sys::state& state) noexcept override {
		flag_button::on_create(state);
		flag_size.x = int16_t(float(flag_size.x) / 1.3f);
		flag_size.y /= 2;
		flag_position.x += base_data.size.x / 7;
		flag_position.y += base_data.size.y / 4;
	}

	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, province_id);
		auto controller_id = fat_id.get_province_control_as_province().get_nation();
		return controller_id.get_identity_from_identity_holder().id;
	}
	
	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			province_id = any_cast<dcon::province_id>(payload);
			auto nation = get_current_nation(state);
			if(bool(nation)) {
				flag_button::set_current_nation(state, get_current_nation(state));
				set_visible(state, true);
			} else {
				set_visible(state, false);
			}
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}

	message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
		return message_result::unseen;
	}
};

class province_national_focus_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override;
};

class province_window_header : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "state_name") {
			return make_element_by_type<province_state_name_text>(state, id);
		} else if(name == "province_name") {
			return make_element_by_type<generic_name_text<dcon::province_id>>(state, id);
		} else if(name == "prov_terrain") {
			return make_element_by_type<province_terrain_image>(state, id);
		} else if(name == "slave_state_icon") {
			return make_element_by_type<slave_state_icon>(state, id);
		} else if(name == "admin_icon") {
			return make_element_by_type<province_admin_icon>(state, id);
		} else if(name == "controller_flag") {
			return make_element_by_type<province_controller_flag>(state, id);
		} else if(name == "flashpoint_indicator") {
			return make_element_by_type<province_flashpoint_indicator>(state, id);
		} else if(name == "occupation_progress") {
			auto ptr = make_element_immediate(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "occupation_icon") {
			auto ptr = make_element_immediate(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "occupation_flag") {
			auto ptr = make_element_immediate(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "colony_button") {
			auto ptr = make_element_immediate(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "national_focus") {
			return make_element_by_type<province_national_focus_button>(state, id);
		} else if(name == "admin_efficiency") {
			return make_element_by_type<state_admin_efficiency_text>(state, id);
		} else if(name == "owner_presence") {
			return make_element_by_type<state_aristocrat_presence_text>(state, id);
		} else if(name == "liferating") {
			return make_element_by_type<province_liferating_progress_bar>(state, id);
		} else {
			return nullptr;
		}
	}

	void update_province_info(sys::state& state, dcon::province_id prov_id) {
		dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
		auto state_fat_id = fat_id.get_state_membership();

		Cyto::Any prov_id_payload = prov_id;
		Cyto::Any state_id_payload = state_fat_id.id;
		impl_set(state, prov_id_payload);
		impl_set(state, state_id_payload);
	}
};

class province_send_diplomat_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			state.open_diplomacy(any_cast<dcon::nation_id>(payload));
		}
	}
};

class province_core_flags : public overlapping_flags_box {
private:
	void populate(sys::state& state, dcon::province_id prov_id) {
		contents.clear();
		auto fat_id = dcon::fatten(state.world, prov_id);
		fat_id.for_each_core_as_province([&](dcon::core_id core_id) {
			auto core_fat_id = dcon::fatten(state.world, core_id);
			auto identity = core_fat_id.get_identity();
			contents.push_back(identity.id);
		});
		update(state);
	}

public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			populate(state, any_cast<dcon::province_id>(payload));
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			populate(state, any_cast<dcon::province_id>(payload));
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class province_view_foreign_details : public window_element_base {
private:
	flag_button* country_flag_button = nullptr;
	culture_piechart<dcon::province_id>* culture_chart = nullptr;
	ideology_piechart<dcon::province_id>* ideology_chart = nullptr;
	workforce_piechart<dcon::province_id>* workforce_chart = nullptr;
	dcon::nation_id stored_nation{};
	dcon::province_id stored_province{};

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_name") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} else if(name == "country_flag") {
			auto ptr = make_element_by_type<flag_button>(state, id);
			country_flag_button = ptr.get();
			return ptr;
		} else if(name == "country_gov") {
			return make_element_by_type<nation_government_type_text>(state, id);
		} else if(name == "country_party") {
			return make_element_by_type<nation_ruling_party_text>(state, id);
		} else if(name == "country_prestige") {
			return make_element_by_type<nation_prestige_text>(state, id);
		} else if(name == "country_economic") {
			return make_element_by_type<nation_industry_score_text>(state, id);
		} else if(name == "country_military") {
			return make_element_by_type<nation_military_score_text>(state, id);
		} else if(name == "country_total") {
			return make_element_by_type<nation_total_score_text>(state, id);
		} else if(name == "selected_prestige_rank") {
			return make_element_by_type<nation_prestige_rank_text>(state, id);
		} else if(name == "selected_industry_rank") {
			return make_element_by_type<nation_industry_rank_text>(state, id);
		} else if(name == "selected_military_rank") {
			return make_element_by_type<nation_military_rank_text>(state, id);
		} else if(name == "selected_total_rank") {
			return make_element_by_type<nation_rank_text>(state, id);
		} else if(name == "country_status") {
			return make_element_by_type<nation_status_text>(state, id);
		} else if(name == "country_flag_overlay") {
			return make_element_by_type<nation_flag_frame>(state, id);
		} else if(name == "total_population") {
			return make_element_by_type<province_population_text>(state, id);
		} else if(name == "sphere_label") {
			return make_element_by_type<nation_sphere_list_label>(state, id);
		} else if(name == "puppet_label") {
			return make_element_by_type<nation_puppet_list_label>(state, id);
		} else if(name == "our_relation") {
			return make_element_by_type<nation_player_relations_text>(state, id);
		} else if(name == "workforce_chart") {
			auto ptr = make_element_by_type<workforce_piechart<dcon::province_id>>(state, id);
			workforce_chart = ptr.get();
			return ptr;
		} else if(name == "ideology_chart") {
			auto ptr = make_element_by_type<ideology_piechart<dcon::province_id>>(state, id);
			ideology_chart = ptr.get();
			return ptr;
		}  else if(name == "culture_chart") {
			auto ptr = make_element_by_type<culture_piechart<dcon::province_id>>(state, id);
			culture_chart = ptr.get();
			return ptr;
		} else if(name == "goods_type") {
			return make_element_by_type<province_rgo_icon>(state, id);
		} else if(name == "build_icon_fort") {
			return make_element_by_type<province_fort_icon>(state, id);
		} else if(name == "build_icon_navalbase") {
			return make_element_by_type<province_naval_base_icon>(state, id);
		} else if(name == "build_icon_infra") {
			return make_element_by_type<province_railroad_icon>(state, id);
		} else if(name == "infra_progress_win") {
			auto ptr = make_element_by_type<window_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "invest_build_infra") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->disabled = true;
			return ptr;
		} else if(name == "invest_factory_button") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->disabled = true;
			return ptr;
		} else if(name == "sphere_targets") {
			return make_element_by_type<overlapping_sphere_flags>(state, id);
		} else if(name == "puppet_targets") {
			return make_element_by_type<overlapping_puppet_flags>(state, id);
		} else if(name == "allied_targets") {
			return make_element_by_type<overlapping_ally_flags>(state, id);
		} else if(name == "war_targets") {
			return make_element_by_type<overlapping_enemy_flags>(state, id);
		}  else if(name == "send_diplomat") {
			return make_element_by_type<province_send_diplomat_button>(state, id);
		} else if(name == "core_icons") {
			return make_element_by_type<province_core_flags>(state, id);
		} else if(name == "supply_limit") {
			return make_element_by_type<province_supply_limit_text>(state, id);
		} else {
			return nullptr;
		}
	}

    void update_province_info(sys::state& state, dcon::province_id prov_id) {
		stored_province = prov_id;
		dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
		auto nation_id = fat_id.get_nation_from_province_ownership();
		stored_nation = nation_id;
		if(!bool(nation_id) || nation_id.id == state.local_player_nation) {
			set_visible(state, false);
		} else {
			country_flag_button->on_update(state);

			culture_chart->on_update(state);
			workforce_chart->on_update(state);
			ideology_chart->on_update(state);

			Cyto::Any nat_id_payload = nation_id.id;
			Cyto::Any prov_id_payload = prov_id;
			impl_set(state, nat_id_payload);
			impl_set(state, prov_id_payload);
			set_visible(state, true);
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(stored_nation);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(stored_province);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class province_view_statistics : public window_element_base {
private:
	culture_piechart<dcon::province_id>* culture_chart = nullptr;
	ideology_piechart<dcon::province_id>* ideology_chart = nullptr;
	workforce_piechart<dcon::province_id>* workforce_chart = nullptr;
	dcon::nation_id stored_nation{};
	dcon::province_id stored_province{};

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "goods_type") {
			return make_element_by_type<province_rgo_icon>(state, id);
		} else if(name == "total_population") {
			return make_element_by_type<province_population_text>(state, id);
		} else if(name == "workforce_chart") {
			auto ptr = make_element_by_type<workforce_piechart<dcon::province_id>>(state, id);
			workforce_chart = ptr.get();
			return ptr;
		} else if(name == "ideology_chart") {
			auto ptr = make_element_by_type<ideology_piechart<dcon::province_id>>(state, id);
			ideology_chart = ptr.get();
			return ptr;
		}  else if(name == "culture_chart") {
			auto ptr = make_element_by_type<culture_piechart<dcon::province_id>>(state, id);
			culture_chart = ptr.get();
			return ptr;
		} else if(name == "core_icons") {
			return make_element_by_type<province_core_flags>(state, id);
		} else if(name == "supply_limit") {
			return make_element_by_type<province_supply_limit_text>(state, id);
		} else if(name == "crime_icon") {
			return make_element_by_type<province_crime_icon>(state, id);
		} else if(name == "crime_name") {
			return make_element_by_type<province_crime_name_text>(state, id);
		} else if(name == "crimefight_percent") {
			return make_element_by_type<province_crime_fighting_text>(state, id);
		} else if(name == "rebel_percent") {
			return make_element_by_type<province_rebel_percent_text>(state, id);
		} else if(name == "employment_ratio") {
			return make_element_by_type<province_rgo_employment_progress_icon>(state, id);
		} else if(name == "rgo_population") {
			return make_element_by_type<province_rgo_workers_text>(state, id);
		} else if(name == "rgo_percent") {
			return make_element_by_type<province_rgo_employment_percent_text>(state, id);
		} else if(name == "produced") {
			return make_element_by_type<province_goods_produced_text>(state, id);
		} else if(name == "income") {
			return make_element_by_type<province_income_text>(state, id);
		} else if(name == "growth") {
			return make_element_by_type<province_pop_growth_text>(state, id);
		} else if(name == "migration") {
			return make_element_by_type<province_migration_text>(state, id);
		} else {
			return nullptr;
		}
	}

	void update_province_info(sys::state& state, dcon::province_id prov_id) {
		stored_province = prov_id;
		dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
		auto nation_id = fat_id.get_nation_from_province_ownership();
		stored_nation = nation_id;
		if(bool(nation_id) && nation_id.id == state.local_player_nation) {
			culture_chart->on_update(state);
			workforce_chart->on_update(state);
			ideology_chart->on_update(state);

			Cyto::Any nat_id_payload = nation_id.id;
			Cyto::Any prov_id_payload = prov_id;
			impl_set(state, nat_id_payload);
			impl_set(state, prov_id_payload);
			set_visible(state, true);
		} else {
			set_visible(state, false);
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(stored_province);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

template<class IconT>
class province_generic_building : public window_element_base {
public:
	virtual std::string get_icon_name() noexcept {
		return "build_icon0";
	}

	virtual std::string get_description(sys::state& state) noexcept {
		return "description";
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == get_icon_name()) {
			return make_element_by_type<IconT>(state, id);
		} else if(name == "build_icon0" || name == "build_icon1" || name == "build_icon2") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "underconstruction_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "building_progress") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "expand_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "description") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_text(state, get_description(state));
			return ptr;
		} else {
			return nullptr;
		}
	}
};

class province_fort_window : public province_generic_building<province_fort_icon> {
public:
	std::string get_icon_name() noexcept override {
		return "build_icon0";
	}

	std::string get_description(sys::state& state) noexcept override {
		return text::produce_simple_string(state, "fort");
	}
};

class province_naval_base_window : public province_generic_building<province_naval_base_icon> {
public:
	std::string get_icon_name() noexcept override {
		return "build_icon1";
	}

	std::string get_description(sys::state& state) noexcept override {
		return text::produce_simple_string(state, "naval_base");
	}
};

class province_railroad_window : public province_generic_building<province_railroad_icon> {
public:
	std::string get_icon_name() noexcept override {
		return "build_icon2";
	}

	std::string get_description(sys::state& state) noexcept override {
		return text::produce_simple_string(state, "railroad");
	}
};

class province_view_buildings : public window_element_base {
private:
	dcon::nation_id stored_nation{};
	dcon::province_id stored_province{};

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		{
			auto ptr = make_element_by_type<province_fort_window>(state, "building");
			ptr->base_data.position.y = 1;
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<province_naval_base_window>(state, "building");
			ptr->base_data.position.y = 36;
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<province_railroad_window>(state, "building");
			ptr->base_data.position.y = 71;
			add_child_to_front(std::move(ptr));
		}
    }

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "army_progress") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "army_progress_overlay") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "navy_progress") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "navy_progress_overlay") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "army_text") {  // this seems to be unused
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "navy_text") {  // this seems to be unused
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "army_size") {
			return make_element_by_type<province_army_size_text>(state, id);
		} else {
			return nullptr;
		}
	}

	void update_province_info(sys::state& state, dcon::province_id prov_id) {
		stored_province = prov_id;
		dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
		auto nation_id = fat_id.get_nation_from_province_ownership();
		stored_nation = nation_id;
		if(bool(nation_id) && nation_id.id == state.local_player_nation) {
			Cyto::Any nat_id_payload = nation_id.id;
			Cyto::Any prov_id_payload = prov_id;
			impl_set(state, nat_id_payload);
			impl_set(state, prov_id_payload);
			set_visible(state, true);
		} else {
			set_visible(state, false);
		}
	}
};

class province_window_colony : public window_element_base {
private:
	image_element_base* rgo_icon = nullptr;
	simple_text_element_base* population_box = nullptr;
	culture_piechart<dcon::province_id>* culture_chart = nullptr;
	dcon::province_id stored_province{};

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "total_population") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			population_box = ptr.get();
			return ptr;
		}  else if(name == "culture_chart") {
			auto ptr = make_element_by_type<culture_piechart<dcon::province_id>>(state, id);
			culture_chart = ptr.get();
			return ptr;
		} else if(name == "goods_type") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			rgo_icon = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

    void update_province_info(sys::state& state, dcon::province_id prov_id) {
		stored_province = prov_id;
		dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
		auto nation_id = fat_id.get_nation_from_province_ownership();
		if(bool(nation_id)) {
			set_visible(state, false);
		} else {
			rgo_icon->frame = fat_id.get_rgo().get_icon();
			auto total_pop = state.world.province_get_demographics(prov_id, demographics::total);
			population_box->set_text(state, text::prettify(int32_t(total_pop)));
			culture_chart->on_update(state);
			set_visible(state, true);
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(stored_province);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class national_focus_icon : public button_element_base {
private:
	dcon::national_focus_id focus_id{};

public:
	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::national_focus_id>()) {
			focus_id = any_cast<dcon::national_focus_id>(payload);
			auto fat_id = dcon::fatten(state.world, focus_id);
			frame = fat_id.get_icon() - 1;
			
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};

class province_focus_item : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "focus_icon") {
			return make_element_by_type<national_focus_icon>(state, id);
		} else {
			return nullptr;
		}
	}
};

class province_focus_category_list : public overlapping_listbox_element_base<province_focus_item, dcon::national_focus_id> {
public:
	std::string_view get_row_element_name() override {
		return "focus_item";
	}

	void update_subwindow(sys::state& state, province_focus_item* subwindow, dcon::national_focus_id content) override {
		Cyto::Any payload = content;
		subwindow->impl_set(state, payload);
	}
};

class province_focus_category : public window_element_base {
private:
	simple_text_element_base* category_label = nullptr;
	province_focus_category_list* focus_list = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			category_label = ptr.get();
			return ptr;
		} else if(name == "focus_icons") {
			auto ptr = make_element_by_type<province_focus_category_list>(state, id);
			focus_list = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<nations::focus_type>()) {
			auto category = any_cast<nations::focus_type>(payload);
			category_label->set_text(state, text::get_focus_category_name(state, category));

			focus_list->contents.clear();
			state.world.for_each_national_focus([&](dcon::national_focus_id focus_id) {
				auto fat_id = dcon::fatten(state.world, focus_id);
				if(fat_id.get_type() == uint8_t(category)) {
					focus_list->contents.push_back(focus_id);
				}
			});
			focus_list->update(state);

			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class province_national_focus_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		auto start = make_element_by_type<window_element_base>(state, "focuscategory_start");
		auto current_pos = start->base_data.position;
		auto step = make_element_by_type<window_element_base>(state, "focuscategory_step");
		auto step_y = step->base_data.position.y;

		for(uint8_t i = 1; i <= uint8_t(nations::focus_type::party_loyalty_focus); i++) {
			auto ptr = make_element_by_type<province_focus_category>(state, "focus_category");
			ptr->base_data.position = current_pos;
			current_pos = xy_pair{current_pos.x, int16_t(current_pos.y + step_y)};

			Cyto::Any foc_type_payload = nations::focus_type(i);
			ptr->impl_set(state, foc_type_payload);

			add_child_to_front(std::move(ptr));
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<province_close_button>(state, id);
		} else if(name == "background") {
			return make_element_by_type<opaque_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class province_view_window : public window_element_base {
private:
	dcon::province_id active_province{};
	province_window_header* header_window = nullptr;
	province_view_foreign_details* foreign_details_window = nullptr;
	province_view_statistics* local_details_window = nullptr;
	province_view_buildings* local_buildings_window = nullptr;
	province_window_colony* colony_window = nullptr;

public:
	province_national_focus_window* national_focus_window = nullptr;

    void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		state.ui_state.province_window = this;
		set_visible(state, false);
    }

    std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<province_close_button>(state, id);
		} else if(name == "background") {
			return make_element_by_type<opaque_element_base>(state, id);
		} else if(name == "province_view_header") {
			auto ptr = make_element_by_type<province_window_header>(state, id);
			header_window = ptr.get();
			return ptr;
		} else if(name == "province_other") {
			auto ptr = make_element_by_type<province_view_foreign_details>(state, id);
			ptr->set_visible(state, false);
			foreign_details_window = ptr.get();
			return ptr;
		} else if(name == "province_colony") {
			auto ptr = make_element_by_type<province_window_colony>(state, id);
			ptr->set_visible(state, false);
			colony_window = ptr.get();
			return ptr;
		} else if(name == "province_statistics") {
			auto ptr = make_element_by_type<province_view_statistics>(state, id);
			local_details_window = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "province_buildings") {
			auto ptr = make_element_by_type<province_view_buildings>(state, id);
			local_buildings_window = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "national_focus_window") {
			auto ptr = make_element_by_type<province_national_focus_window>(state, id);
			ptr->set_visible(state, false);
			national_focus_window = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void update_province_info(sys::state& state) {
		header_window->update_province_info(state, active_province);
		foreign_details_window->update_province_info(state, active_province);
		local_details_window->update_province_info(state, active_province);
		local_buildings_window->update_province_info(state, active_province);
		colony_window->update_province_info(state, active_province);
	}

    void set_active_province(sys::state& state, dcon::province_id map_province) {
		if(bool(map_province)) {
			active_province = map_province;
			update_province_info(state);
			set_visible(state, true);
		} else {
			set_visible(state, false);
		}
	}
};

}
