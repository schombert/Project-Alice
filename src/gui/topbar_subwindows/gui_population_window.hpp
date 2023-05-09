#pragma once

#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "gui_common_elements.hpp"
#include "province.hpp"

namespace ui {

enum class pop_list_sort : uint8_t {
	size,
	type,
	nationality,
	religion,
	location,
	mil,
	con,
	ideology,
	issues,
	unemployment,
	cash,
	subsistence,
	eve,
	luxury,
	revoltrisk,
	change,
	literacy
};

class pop_unemployment_progress_bar : public standard_pop_progress_bar {
protected:
	dcon::pop_id pop_id{};
public:
	float get_progress(sys::state& state) noexcept override {
		Cyto::Any pop_id_payload = dcon::pop_id{};
		if(parent) {
			parent->impl_get(state, pop_id_payload);
			if(pop_id_payload.holds_type<dcon::pop_id>()) {
				pop_id = any_cast<dcon::pop_id>(pop_id_payload);
				auto pfat_id = dcon::fatten(state.world, pop_id);
				if(state.world.pop_type_get_has_unemployment(state.world.pop_get_poptype(pop_id)))
					return (1 - pfat_id.get_employment() / pfat_id.get_size());
				else
					return 0.0f;
			}
		}
		return 0.0f;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);

		auto pfat_id = dcon::fatten(state.world, pop_id);
		float un_empl = state.world.pop_type_get_has_unemployment(state.world.pop_get_poptype(pop_id)) ? (1 - pfat_id.get_employment() / pfat_id.get_size()) : 0.0f;
		text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "unemployment"));
		text::add_space_to_layout_box(contents, state, box);
		text::add_to_layout_box(contents, state, box, text::fp_percentage{ un_empl });
		text::close_layout_box(contents, box);
	}
};

class pop_life_needs_progress_bar : public standard_pop_needs_progress_bar {
protected:
	dcon::pop_id pop_id{};
public:
	float get_progress(sys::state& state) noexcept override {
		Cyto::Any pop_id_payload = dcon::pop_id{};
		if(parent) {
			parent->impl_get(state, pop_id_payload);
			if(pop_id_payload.holds_type<dcon::pop_id>()) {
				pop_id = any_cast<dcon::pop_id>(pop_id_payload);
			}
		}
		auto pfat_id = dcon::fatten(state.world, pop_id);
		return pfat_id.get_life_needs_satisfaction();
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);

		auto pfat_id = dcon::fatten(state.world, pop_id);
		text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "life_needs"));
		text::add_space_to_layout_box(contents, state, box);
		text::add_to_layout_box(contents, state, box, text::fp_percentage{ pfat_id.get_life_needs_satisfaction() });
		text::close_layout_box(contents, box);
	}
};

class pop_everyday_needs_progress_bar : public standard_pop_needs_progress_bar {
protected:
	dcon::pop_id pop_id{};
public:
	float get_progress(sys::state& state) noexcept override {
		Cyto::Any pop_id_payload = dcon::pop_id{};
		if(parent) {
			parent->impl_get(state, pop_id_payload);
			if(pop_id_payload.holds_type<dcon::pop_id>()) {
				pop_id = any_cast<dcon::pop_id>(pop_id_payload);
			}
		}
		auto pfat_id = dcon::fatten(state.world, pop_id);
		return pfat_id.get_everyday_needs_satisfaction();
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);

		auto pfat_id = dcon::fatten(state.world, pop_id);
		text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "everyday_needs"));
		text::add_space_to_layout_box(contents, state, box);
		text::add_to_layout_box(contents, state, box, text::fp_percentage{ pfat_id.get_everyday_needs_satisfaction() });
		text::close_layout_box(contents, box);
	}
};

class pop_luxury_needs_progress_bar : public standard_pop_needs_progress_bar {
protected:
	dcon::pop_id pop_id{};
public:
	float get_progress(sys::state& state) noexcept override {
		Cyto::Any pop_id_payload = dcon::pop_id{};
		if(parent) {
			parent->impl_get(state, pop_id_payload);
			if(pop_id_payload.holds_type<dcon::pop_id>()) {
				pop_id = any_cast<dcon::pop_id>(pop_id_payload);
			}
		}
		auto pfat_id = dcon::fatten(state.world, pop_id);
		return pfat_id.get_luxury_needs_satisfaction();
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);

		auto pfat_id = dcon::fatten(state.world, pop_id);
		text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "luxury_needs"));
		text::add_space_to_layout_box(contents, state, box);
		text::add_to_layout_box(contents, state, box, text::fp_percentage{ pfat_id.get_luxury_needs_satisfaction() });
		text::close_layout_box(contents, box);
	}
};

class pop_item : public listbox_row_element_base<dcon::pop_id> {
private:
	simple_text_element_base* size = nullptr;
	image_element_base* type = nullptr;
	simple_text_element_base* nation = nullptr;
	image_element_base* religion = nullptr;
	simple_text_element_base* location = nullptr;
	simple_text_element_base* mil = nullptr;
	simple_text_element_base* con = nullptr;
	pop_ideology_piechart* ideology = nullptr;
	pop_issues_piechart* issues = nullptr;
	pop_unemployment_progress_bar* unemployment = nullptr;
	simple_text_element_base* cash = nullptr;
	pop_life_needs_progress_bar* life_needs = nullptr;
	pop_everyday_needs_progress_bar* everyday_needs = nullptr;
	pop_luxury_needs_progress_bar* luxury_needs = nullptr;
public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base<dcon::pop_id>::on_create(state);
	}

	std::unique_ptr<element_base>
		make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "pop_size") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			size = ptr.get();
			return ptr;
		} else if(name == "pop_nation") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			nation = ptr.get();
			return ptr;
		} else if(name == "pop_location") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			location = ptr.get();
			return ptr;
		} else if(name == "pop_mil") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			mil = ptr.get();
			return ptr;
		} else if(name == "pop_con") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			con = ptr.get();
			return ptr;
		} else if(name == "pop_type") {
			auto ptr = make_element_by_type<pop_type_icon>(state, id);
			type = ptr.get();
			return ptr;
		} else if(name == "pop_religion") {
			auto ptr = make_element_by_type<religion_type_icon>(state, id);
			religion = ptr.get();
			return ptr;
		} else if(name == "pop_ideology") {
			auto ptr = make_element_by_type<pop_ideology_piechart>(state, id);
			ideology = ptr.get();
			return ptr;
		} else if(name == "pop_issues") {
			auto ptr = make_element_by_type<pop_issues_piechart>(state, id);
			issues = ptr.get();
			return ptr;
		} else if(name == "pop_unemployment_bar") {
			auto ptr = make_element_by_type<pop_unemployment_progress_bar>(state, id);
			unemployment = ptr.get();
			return ptr;
		} else if(name == "pop_cash") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			cash = ptr.get();
			return ptr;
		} else if(name == "lifeneed_progress") {
			auto ptr = make_element_by_type<pop_life_needs_progress_bar>(state, id);
			life_needs = ptr.get();
			return ptr;
		} else if(name == "eveneed_progress") {
			auto ptr = make_element_by_type<pop_everyday_needs_progress_bar>(state, id);
			everyday_needs = ptr.get();
			return ptr;
		} else if(name == "luxneed_progress") {
			auto ptr = make_element_by_type<pop_luxury_needs_progress_bar>(state, id);
			luxury_needs = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	};


	void update(sys::state& state) noexcept override {
		dcon::pop_fat_id fat_id = dcon::fatten(state.world, content);
		dcon::culture_fat_id cfat_id = dcon::fatten(state.world, fat_id.get_culture());
		dcon::province_fat_id lcfat_id = (dcon::fatten(state.world, fat_id.get_pop_location())).get_province();

		Cyto::Any tpayload = fat_id.get_poptype().id;
		type->impl_set(state, tpayload);
		type->impl_on_update(state);

		Cyto::Any rpayload = fat_id.get_religion().id;
		religion->impl_set(state, rpayload);
		religion->impl_on_update(state);

		size->set_text(state, std::to_string(int32_t(fat_id.get_size())));
		nation->set_text(state, text::produce_simple_string(state, cfat_id.get_name()));
		location->set_text(state, text::produce_simple_string(state, lcfat_id.get_name()));
		mil->set_text(state, text::format_float(fat_id.get_militancy()));
		con->set_text(state, text::format_float(fat_id.get_consciousness()));
		cash->set_text(state, text::format_float(state.world.pop_get_savings(fat_id.id)));

		ideology->impl_on_update(state); // Necessary so that piecharts will update as you scroll.
		issues->impl_on_update(state);
		unemployment->impl_on_update(state);
		life_needs->impl_on_update(state);
		everyday_needs->impl_on_update(state);
		luxury_needs->impl_on_update(state);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::pop_id>()) {
			payload.emplace<dcon::pop_id>(content);
			return message_result::consumed;
		} else {
			return listbox_row_element_base<dcon::pop_id>::get(state, payload);
		}
	}
};

class pop_listbox : public listbox_element_base<pop_item, dcon::pop_id> {
protected:
	std::string_view get_row_element_name() override {
		return "popinfomember_popview";
	}
};

typedef std::variant<
	std::monostate,
	dcon::nation_id,
	dcon::state_instance_id,
	dcon::province_id
> pop_left_side_data;
template<typename T>
class pop_left_side_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = T{};
			parent->impl_get(state, payload);
			T id = any_cast<T>(payload);
			if(state.ui_state.population_subwindow) {
				Cyto::Any payload = pop_list_filter{};
				state.ui_state.population_subwindow->impl_set(state, payload);
				auto filter = any_cast<pop_list_filter>(payload);
				frame = std::holds_alternative<T>(filter) && std::get<T>(filter) == id
					? 1 : 0;
			}
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = T{};
			parent->impl_get(state, payload);
			T id = any_cast<T>(payload);
			if(state.ui_state.population_subwindow) {
				Cyto::Any new_payload = pop_list_filter(id);
				state.ui_state.population_subwindow->impl_set(state, new_payload);
			}
		}
	}
};
class pop_left_side_country_window : public generic_settable_element<window_element_base, dcon::nation_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "poplistbutton") {
			return make_element_by_type<pop_left_side_button<dcon::nation_id>>(state, id);
		} else if(name == "poplist_name") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} else if(name == "poplist_numpops") {
			return make_element_by_type<nation_population_text>(state, id);
		} else if(name == "growth_indicator") {
			return make_element_by_type<image_element_base>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(content);
			return message_result::consumed;
		}
		return window_element_base::get(state, payload);
	}
};
typedef std::variant<std::monostate, dcon::state_instance_id, bool> pop_left_side_expand_action;
class pop_left_side_expand_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, "");
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any id_payload = dcon::state_instance_id{};
			parent->impl_get(state, id_payload);
			auto id = any_cast<dcon::state_instance_id>(id_payload);

			Cyto::Any payload = pop_left_side_expand_action(id);
			parent->impl_get(state, payload);
			frame = std::get<bool>(any_cast<pop_left_side_expand_action>(payload)) ? 1 : 0;
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			auto id = any_cast<dcon::state_instance_id>(payload);
			if(state.ui_state.population_subwindow) {
				Cyto::Any new_payload = pop_left_side_expand_action(id);
				state.ui_state.population_subwindow->impl_set(state, new_payload);
			}
		}
	}
};
class pop_left_side_state_window : public generic_settable_element<window_element_base, dcon::state_instance_id> {
	image_element_base* colonial_icon = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "poplistbutton") {
			return make_element_by_type<pop_left_side_button<dcon::state_instance_id>>(state, id);
		} else if(name == "poplist_name") {
			return make_element_by_type<state_name_text>(state, id);
		} else if(name == "poplist_numpops") {
			return make_element_by_type<state_population_text>(state, id);
		} else if(name == "colonial_state_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			colonial_icon = ptr.get();
			return ptr;
		} else if(name == "state_focus") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "expand") {
			return make_element_by_type<pop_left_side_expand_button>(state, id);
		} else if(name == "growth_indicator") {
			return make_element_by_type<image_element_base>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		colonial_icon->set_visible(state, state.world.province_get_is_colonial(state.world.state_instance_get_capital(content)));
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::state_instance_id>()) {
			payload.emplace<dcon::state_instance_id>(content);
			return message_result::consumed;
		}
		return window_element_base::get(state, payload);
	}
};
class pop_left_side_province_window : public generic_settable_element<window_element_base, dcon::province_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "poplistbutton") {
			return make_element_by_type<pop_left_side_button<dcon::province_id>>(state, id);
		} else if(name == "poplist_name") {
			return make_element_by_type<generic_name_text<dcon::province_id>>(state, id);
		} else if(name == "poplist_numpops") {
			return make_element_by_type<province_population_text>(state, id);
		} else if(name == "growth_indicator") {
			return make_element_by_type<image_element_base>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(content);
			return message_result::consumed;
		}
		return window_element_base::get(state, payload);
	}
};

class pop_left_side_item : public listbox_row_element_base<pop_left_side_data> {
	pop_left_side_country_window* country_window = nullptr;
	pop_left_side_state_window* state_window = nullptr;
	pop_left_side_province_window* province_window = nullptr;
public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base<pop_left_side_data>::on_create(state);

		auto ptr1 = make_element_by_type<pop_left_side_country_window>(state, state.ui_state.defs_by_name.find("poplistitem_country")->second.definition);
		country_window = ptr1.get();
		add_child_to_back(std::move(ptr1));

		auto ptr2 = make_element_by_type<pop_left_side_state_window>(state, state.ui_state.defs_by_name.find("poplistitem_state")->second.definition);
		state_window = ptr2.get();
		add_child_to_back(std::move(ptr2));

		auto ptr3 = make_element_by_type<pop_left_side_province_window>(state, state.ui_state.defs_by_name.find("poplistitem_province")->second.definition);
		province_window = ptr3.get();
		add_child_to_back(std::move(ptr3));
		// After this, the widget will be immediately set by the parent
	}

	void update(sys::state& state) noexcept override {
		country_window->set_visible(state, std::holds_alternative<dcon::nation_id>(content));
		state_window->set_visible(state, std::holds_alternative<dcon::state_instance_id>(content));
		province_window->set_visible(state, std::holds_alternative<dcon::province_id>(content));
		if (std::holds_alternative<dcon::nation_id>(content)) {
			Cyto::Any new_payload = std::get<dcon::nation_id>(content);
			country_window->impl_set(state, new_payload);
		} else if (std::holds_alternative<dcon::state_instance_id>(content)) {
			Cyto::Any new_payload = std::get<dcon::state_instance_id>(content);
			state_window->impl_set(state, new_payload);
		} else if (std::holds_alternative<dcon::province_id>(content)) {
			Cyto::Any new_payload = std::get<dcon::province_id>(content);
			province_window->impl_set(state, new_payload);
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<pop_left_side_data>()) {
			payload.emplace<pop_left_side_data>(content);
			return message_result::consumed;
		} else {
			return listbox_row_element_base<pop_left_side_data>::get(state, payload);
		}
	}
};

class pop_left_side_listbox : public listbox_element_base<pop_left_side_item, pop_left_side_data> {
protected:
	std::string_view get_row_element_name() override {
		return "pop_left_side_list_base_window";
	}
};

class pop_legend_item : public window_element_base {
public:
};
class pop_legend_listbox : public listbox_element_base<pop_legend_item, std::monostate> {
public:
	std::string_view get_row_element_name() override {
		return "pop_legend_item";
	}
};

class pop_workforce_distrobution_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "item_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_text(state, text::produce_simple_string(state, "workforce_disttitle"));
			return ptr;
		} else if(name == "chart") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "member_names") {
			return make_element_by_type<pop_legend_listbox>(state, id);
		} else {
			return nullptr;
		}
	}
};
class pop_religion_distrobution_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "item_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_text(state, text::produce_simple_string(state, "religion_disttitle"));
			return ptr;
		} else if(name == "chart") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "member_names") {
			return make_element_by_type<pop_legend_listbox>(state, id);
		} else {
			return nullptr;
		}
	}
};
class pop_ideology_distrobution_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "item_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_text(state, text::produce_simple_string(state, "ideology_disttitle"));
			return ptr;
		} else if(name == "chart") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "member_names") {
			return make_element_by_type<pop_legend_listbox>(state, id);
		} else {
			return nullptr;
		}
	}
};
class pop_nationality_distrobution_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "item_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_text(state, text::produce_simple_string(state, "nationality_disttitle"));
			return ptr;
		} else if(name == "chart") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "member_names") {
			return make_element_by_type<pop_legend_listbox>(state, id);
		} else {
			return nullptr;
		}
	}
};
class pop_issues_piechart : public piechart<dcon::issue_option_id>{
protected:
    std::unordered_map<uint16_t, float> get_distribution(sys::state& state) noexcept override {
        std::unordered_map<uint16_t, float> distrib = {};
        Cyto::Any pop_id_payload = dcon::pop_id{};
        if(parent) {
            parent->impl_get(state, pop_id_payload);
            if(pop_id_payload.holds_type<dcon::pop_id>()) {
                auto pop_id = any_cast<dcon::pop_id>(pop_id_payload);
                auto fat_id = dcon::fatten(state.world, pop_id);
                state.world.for_each_issue_option([&](dcon::issue_option_id issue_id) {
                    auto weight =
                            state.world.pop_get_demographics(pop_id, pop_demographics::to_key(state, issue_id));
                    distrib[uint16_t(issue_id.index())] = weight;
                });
            }
        }
        return distrib;
    }
};
class pop_issue_distrobution_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "item_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_text(state, text::produce_simple_string(state, "dominant_issues_disttitle"));
			return ptr;
		} else if(name == "chart") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "member_names") {
			return make_element_by_type<pop_legend_listbox>(state, id);
		} else {
			return nullptr;
		}
	}
};
class pop_electorate_vote_distrobution_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "item_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_text(state, text::produce_simple_string(state, "electorate_vote_disttitle"));
			return ptr;
		} else if(name == "chart") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "member_names") {
			return make_element_by_type<pop_legend_listbox>(state, id);
		} else {
			return nullptr;
		}
	}
};

class population_window : public window_element_base {
private:
	pop_listbox* country_pop_listbox = nullptr;
	pop_left_side_listbox* left_side_listbox = nullptr;
	pop_list_filter filter = std::monostate{};
	// Whetever or not to show provinces below the state element in the listbox!
	ankerl::unordered_dense::map<decltype(dcon::state_instance_id::value), bool> view_expanded_state;

	void populate_pop_list(sys::state& state) {
		std::vector<dcon::state_instance_id> state_list;
		std::vector<dcon::province_id> province_list;

		auto nation_id = std::holds_alternative<dcon::nation_id>(filter)
			? std::get<dcon::nation_id>(filter)
			: state.local_player_nation;
		for(auto si : state.world.nation_get_state_ownership(nation_id))
			state_list.push_back(si.get_state().id);
		
		for(auto& state_id : state_list) {
			if(std::holds_alternative<dcon::state_instance_id>(filter)
			&& std::get<dcon::state_instance_id>(filter) != state_id)
				continue;
			auto fat_id = dcon::fatten(state.world, state_id);
			province::for_each_province_in_state_instance(state, fat_id, [&](dcon::province_id id) {
				province_list.push_back(id);
			});
		}

		for(auto& province_id : province_list) {
			if(std::holds_alternative<dcon::province_id>(filter)
			&& std::get<dcon::province_id>(filter) != province_id)
				continue;
			auto fat_id = dcon::fatten(state.world, province_id);
			fat_id.for_each_pop_location_as_province([&](dcon::pop_location_id id) {
				country_pop_listbox->row_contents.push_back(state.world.pop_location_get_pop(id));
			});
		}
	}
	void sort_pop_list(sys::state& state) {
		std::sort(country_pop_listbox->row_contents.begin(), country_pop_listbox->row_contents.end(), [&](auto a, auto b) {
			auto a_fat_id = dcon::fatten(state.world, a);
			auto b_fat_id = dcon::fatten(state.world, b);
			return a_fat_id.get_size() > b_fat_id.get_size();
		});
	}

	void populate_left_side_list(sys::state& state) {
		auto nation_id = std::holds_alternative<dcon::nation_id>(filter)
			? std::get<dcon::nation_id>(filter)
			: state.local_player_nation;

		// & then populate the separate, left side listbox
		left_side_listbox->row_contents.push_back(pop_left_side_data(nation_id));

		// States are sorted by total population
		std::vector<dcon::state_instance_id> state_list;
		for(auto si : state.world.nation_get_state_ownership(nation_id))
			state_list.push_back(si.get_state().id);
		std::sort(state_list.begin(), state_list.end(), [&](auto a, auto b) {
			// Colonial states go last
			if(state.world.province_get_is_colonial(state.world.state_instance_get_capital(a)) != state.world.province_get_is_colonial(state.world.state_instance_get_capital(b)))
				return !state.world.province_get_is_colonial(state.world.state_instance_get_capital(a));
			return state.world.state_instance_get_demographics(a, demographics::total) > state.world.state_instance_get_demographics(b, demographics::total);
		});

		std::vector<dcon::province_id> province_list;
		for(const auto state_id : state_list) {
			left_side_listbox->row_contents.push_back(pop_left_side_data(state_id));
			// Provinces are sorted by total population too
			province_list.clear();
			auto fat_id = dcon::fatten(state.world, state_id);
			province::for_each_province_in_state_instance(state, fat_id, [&](dcon::province_id id) {
				province_list.push_back(id);
			});
			std::sort(province_list.begin(), province_list.end(), [&](auto a, auto b) {
				return state.world.province_get_demographics(a, demographics::total) > state.world.province_get_demographics(b, demographics::total);
			});
			// Only put if the state is "expanded"
			if(view_expanded_state[state_id.value] == true)
				for(const auto province_id : province_list)
					left_side_listbox->row_contents.push_back(pop_left_side_data(province_id));
		}
	}
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);

		// Create the distrobution windows
		std::vector<element_base*> dist_windows;
		// Workforce
		auto win1 = make_element_by_type<pop_workforce_distrobution_window>(state, state.ui_state.defs_by_name.find("distribution_window")->second.definition);
		dist_windows.push_back(win1.get());
		add_child_to_front(std::move(win1));
		// Religion
		auto win2 = make_element_by_type<pop_religion_distrobution_window>(state, state.ui_state.defs_by_name.find("distribution_window")->second.definition);
		dist_windows.push_back(win2.get());
		add_child_to_front(std::move(win2));
		// Ideology
		auto win3 = make_element_by_type<pop_ideology_distrobution_window>(state, state.ui_state.defs_by_name.find("distribution_window")->second.definition);
		dist_windows.push_back(win3.get());
		add_child_to_front(std::move(win3));
		// Nationality
		auto win4 = make_element_by_type<pop_nationality_distrobution_window>(state, state.ui_state.defs_by_name.find("distribution_window")->second.definition);
		dist_windows.push_back(win4.get());
		add_child_to_front(std::move(win4));
		// Dominant issues
		auto win5 = make_element_by_type<pop_issue_distrobution_window>(state, state.ui_state.defs_by_name.find("distribution_window")->second.definition);
		dist_windows.push_back(win5.get());
		add_child_to_front(std::move(win5));
		// Electorate vote
		auto win6 = make_element_by_type<pop_electorate_vote_distrobution_window>(state, state.ui_state.defs_by_name.find("distribution_window")->second.definition);
		dist_windows.push_back(win6.get());
		add_child_to_front(std::move(win6));

		// It should be proper to reposition the windows now
		const xy_pair cell_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find("popdistribution_start")->second.definition].position;
		const xy_pair cell_size = state.ui_defs.gui[state.ui_state.defs_by_name.find("popdistribution_offset")->second.definition].position;
		xy_pair offset = cell_offset;
		for(const auto win : dist_windows) {
			win->base_data.position = offset;
			offset.x += cell_size.x;
			if(offset.x + cell_size.x >= base_data.size.x) {
				offset.x = cell_offset.x;
				offset.y += cell_size.y;
			}
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "pop_list") {
			auto ptr = make_element_by_type<pop_listbox>(state, id);
			country_pop_listbox = ptr.get();
			return ptr;
		} else if(name == "external_scroll_slider") {
			auto ptr = make_element_by_type<opaque_element_base>(state, id);
			return ptr;
		} else if(name == "sortby_size_button") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			return ptr;
		} else if(name == "sortby_type_button") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			return ptr;
		} else if(name == "sortby_nationality_button") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			return ptr;
		} else if(name == "sortby_religion_button") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			return ptr;
		} else if(name == "sortby_location_button") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			return ptr;
		} else if(name == "sortby_mil_button") {
			auto ptr = make_element_by_type<generic_opaque_checkbox_button<dcon::nation_id>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sortby_con_button") {
			auto ptr = make_element_by_type<generic_opaque_checkbox_button<dcon::nation_id>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sortby_ideology_button") {
			auto ptr = make_element_by_type<generic_opaque_checkbox_button<dcon::nation_id>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sortby_issues_button") {
			auto ptr = make_element_by_type<generic_opaque_checkbox_button<dcon::nation_id>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sortby_unemployment_button") {
			auto ptr = make_element_by_type<generic_opaque_checkbox_button<dcon::nation_id>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sortby_cash_button") {
			auto ptr = make_element_by_type<generic_opaque_checkbox_button<dcon::nation_id>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sortby_subsistence_button") {
			auto ptr = make_element_by_type<generic_opaque_checkbox_button<dcon::nation_id>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sortby_eve_button") {
			auto ptr = make_element_by_type<generic_opaque_checkbox_button<dcon::nation_id>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sortby_luxury_button") {
			auto ptr = make_element_by_type<generic_opaque_checkbox_button<dcon::nation_id>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sortby_revoltrisk_button") {
			auto ptr = make_element_by_type<generic_opaque_checkbox_button<dcon::nation_id>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sortby_change_button") {
			auto ptr = make_element_by_type<generic_opaque_checkbox_button<dcon::nation_id>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sortby_literacy_button") {
			auto ptr = make_element_by_type<generic_opaque_checkbox_button<dcon::nation_id>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "pop_province_list") {
			auto ptr = make_element_by_type<pop_left_side_listbox>(state, id);
			left_side_listbox = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(country_pop_listbox) {
			country_pop_listbox->row_contents.clear();
			populate_pop_list(state);
			sort_pop_list(state);
			country_pop_listbox->update(state);
		}
		if(left_side_listbox) {
			left_side_listbox->row_contents.clear();
			populate_left_side_list(state);
			left_side_listbox->update(state);
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<pop_list_filter>()) {
			filter = any_cast<pop_list_filter>(payload);
			on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<pop_left_side_expand_action>()) {
			auto expand_action = any_cast<pop_left_side_expand_action>(payload);
			auto sid = std::get<dcon::state_instance_id>(expand_action);
			view_expanded_state[sid.value] = !view_expanded_state[sid.value];
			on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<pop_list_filter>()) {
			payload.emplace<pop_list_filter>(filter);
			return message_result::consumed;
		} else if(payload.holds_type<pop_left_side_expand_action>()) {
			auto expand_action = any_cast<pop_left_side_expand_action>(payload);
			auto sid = std::get<dcon::state_instance_id>(expand_action);
			payload.emplace<pop_left_side_expand_action>(pop_left_side_expand_action(view_expanded_state[sid.value]));
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};
}
