#include "dcon_generated.hpp"
#include "gui_element_types.hpp"
#include "province.hpp"
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
		auto prov_name = text::produce_simple_string(state, fat_id.get_name());
		province_name_box->set_text(state, prov_name);
		
		auto state_name_seq = fat_id.get_state_from_abstract_state_membership().get_name();
		auto state_name = text::produce_simple_string(state, state_name_seq);
		state_name_box->set_text(state, state_name);
	}
};

class province_view_foreign_details : public window_element_base {
private:
	simple_text_element_base* country_name_box = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			country_name_box = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

    void update_province_info(sys::state& state, dcon::province_id prov_id) {
		dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
		auto nation_id = fat_id.get_nation_from_province_ownership();
		if(bool(nation_id)) {
			auto country_name_seq = nation_id.get_name();
			auto country_name = text::produce_simple_string(state, country_name_seq);
			country_name_box->set_text(state, country_name);
			set_visible(state, true);
		} else {
			set_visible(state, false);
		}
	}
};

class province_window_colony : public window_element_base {
public:
      void update_province_info(sys::state& state, dcon::province_id prov_id) {
		dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
		auto nation_id = fat_id.get_nation_from_province_ownership();
		if(bool(nation_id)) {
			set_visible(state, false);
		} else {
			set_visible(state, true);
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
