#pragma once

#include "dcon_generated.hpp"
#include "demographics.hpp"
#include "gui_element_types.hpp"
#include "province.hpp"
#include "system_state.hpp"
#include "text.hpp"

namespace ui {

class province_close_button : public generic_close_button {
public:
	void button_action(sys::state& state) noexcept override {
		state.map_display.set_selected_province(0);
        generic_close_button::button_action(state);
	}
};

class province_window_header : public window_element_base {
private:
	simple_text_element_base* state_name_box = nullptr;
	simple_text_element_base* province_name_box = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "state_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			state_name_box = ptr.get();
			return ptr;
		} else if(name == "province_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			province_name_box = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}
	
	void update_province_info(sys::state& state, dcon::province_id prov_id) {
		dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
		auto state_fat_id = fat_id.get_state_from_abstract_state_membership();
		province_name_box->set_text(state, text::get_name_as_string(state, fat_id));
		state_name_box->set_text(state, text::get_name_as_string(state, state_fat_id));
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
	image_element_base* rgo_icon = nullptr;
	simple_text_element_base* country_name_box = nullptr;
	simple_text_element_base* country_gov_box = nullptr;
	simple_text_element_base* country_party_box = nullptr;
	simple_text_element_base* population_box = nullptr;
	culture_piechart<dcon::province_id>* culture_chart = nullptr;
	ideology_piechart<dcon::province_id>* ideology_chart = nullptr;
	workforce_piechart<dcon::province_id>* workforce_chart = nullptr;
	dcon::nation_id stored_nation{};
	dcon::province_id stored_province{};

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			country_name_box = ptr.get();
			return ptr;
		} else if(name == "country_flag") {
			auto ptr = make_element_by_type<flag_button>(state, id);
			country_flag_button = ptr.get();
			return ptr;
		} else if(name == "country_gov") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			country_gov_box = ptr.get();
			return ptr;
		} else if(name == "country_party") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			country_party_box = ptr.get();
			return ptr;
		} else if(name == "total_population") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			population_box = ptr.get();
			return ptr;
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
			auto ptr = make_element_by_type<image_element_base>(state, id);
			rgo_icon = ptr.get();
			return ptr;
		} else if(name == "sphere_targets") {
			return make_element_by_type<overlapping_sphere_flags>(state, id);
		} else if(name == "send_diplomat") {
			return make_element_by_type<province_send_diplomat_button>(state, id);
		} else if(name == "core_icons") {
			return make_element_by_type<province_core_flags>(state, id);
		} else {
			return nullptr;
		}
	}

    void update_province_info(sys::state& state, dcon::province_id prov_id) {
		stored_province = prov_id;
		dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
		auto nation_id = fat_id.get_nation_from_province_ownership();
		stored_nation = nation_id;
		if(bool(nation_id)) {
			country_name_box->set_text(state, text::get_name_as_string(state, nation_id));
			country_party_box->set_text(state, text::get_name_as_string(state, nation_id.get_ruling_party()));

			country_flag_button->on_update(state);

			auto gov_type_id = nation_id.get_government_type();
			auto gov_name_seq = state.culture_definitions.governments[gov_type_id].name;
			auto gov_name = text::produce_simple_string(state, gov_name_seq);
			country_gov_box->set_text(state, gov_name);

			auto total_pop = state.world.province_get_demographics(prov_id, demographics::total);
			population_box->set_text(state, text::prettify(int32_t(total_pop)));

			rgo_icon->frame = fat_id.get_rgo().get_icon();

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

class province_view_window : public window_element_base {
private:
	dcon::province_id active_province{};
	province_window_header* header_window = nullptr;
	province_view_foreign_details* foreign_details_window = nullptr;
	province_window_colony* colony_window = nullptr;

public:
    void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		state.ui_state.province_window = this;
		set_visible(state, false);
    }

    std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<province_close_button>(state, id);
		} else if(name == "province_view_header") {
			auto ptr = make_element_by_type<province_window_header>(state, id);
			header_window = ptr.get();
			return ptr;
		} else if(name == "province_other") {
			auto ptr = make_element_by_type<province_view_foreign_details>(state, id);
			foreign_details_window = ptr.get();
			return ptr;
		} else if(name == "province_colony") {
			auto ptr = make_element_by_type<province_window_colony>(state, id);
			ptr->set_visible(state, false);
			colony_window = ptr.get();
			return ptr;
		} else if(name == "province_statistics") {
			auto ptr = make_element_by_type<window_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "province_buildings") {
			auto ptr = make_element_by_type<window_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "national_focus_window") {
			auto ptr = make_element_by_type<window_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else {
			return nullptr;
		}
	}

	void update_province_info(sys::state& state) {
		header_window->update_province_info(state, active_province);
		foreign_details_window->update_province_info(state, active_province);
		colony_window->update_province_info(state, active_province);
	}

    void set_active_province(sys::state& state, int16_t map_province) {
		active_province = province::from_map_id(map_province);
		if(bool(active_province)) {
			update_province_info(state);
			set_visible(state, true);
		} else {
			set_visible(state, false);
		}
	}
};

}
