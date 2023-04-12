#pragma once

#include "gui_element_types.hpp"

namespace dcon {
class pop_satisfaction_wrapper_id {
public:
	using value_base_t = uint8_t;
	uint8_t value = 0;
	pop_satisfaction_wrapper_id() { }
	pop_satisfaction_wrapper_id(uint8_t v) : value(v) { }
	value_base_t index() {
		return value;
	}
};
class pop_satisfaction_wrapper_fat {
	static text_sequence_id names[5];
public:
	uint8_t value = 0;
	uint32_t get_color() noexcept {
		switch(value) {
		case 0: // red
			return sys::pack_color(0.9f, 0.2f, 0.1f);
		case 1: // yellow
			return sys::pack_color(0.9f, 0.9f, 0.1f);
		case 2: // green
			return sys::pack_color(0.2f, 0.95f, 0.2f);
		case 3: // blue
			return sys::pack_color(0.5f, 0.5f, 1.0f);
		case 4: // light blue
			return sys::pack_color(0.2f, 0.2f, 0.8f);
		}
		return 0;
	}
	void set_name(text_sequence_id text) noexcept {
		names[value] = text;
	}
	text_sequence_id get_name() noexcept {
		switch(value) {
		case 0: // No needs fullfilled
		case 1: // Some life needs
		case 2: // All life needs
		case 3: // All everyday
		case 4: // All luxury
			return names[value];
		}
		return text_sequence_id{ 0 };
	}
};
pop_satisfaction_wrapper_fat fatten(data_container const& c, pop_satisfaction_wrapper_id id) noexcept {
	return pop_satisfaction_wrapper_fat{ id.value };
}
}

namespace ui {
template<culture::pop_strata Strata>
class pop_satisfaction_piechart : public piechart<dcon::pop_satisfaction_wrapper_id> {
protected:
	std::unordered_map<uint8_t, float> get_distribution(sys::state& state) noexcept override {
		std::unordered_map<uint8_t, float> distrib = {};
		Cyto::Any nat_id_payload = dcon::nation_id{};

		enabled = true;
		if(parent == nullptr)
			return distrib;
		
		parent->impl_get(state, nat_id_payload);
		if(nat_id_payload.holds_type<dcon::nation_id>()) {
			auto nat_id = any_cast<dcon::nation_id>(nat_id_payload);
			auto total = 0.f;
			auto sat_pool = std::vector<float>(5);
			state.world.for_each_province([&](dcon::province_id province) {
				if(nat_id != state.world.province_get_nation_from_province_ownership(province))
					return;
				
				for(auto pop_loc : state.world.province_get_pop_location(province)) {
					auto pop_id = pop_loc.get_pop();
					auto pop_strata = state.world.pop_type_get_strata(state.world.pop_get_poptype(pop_id));
					auto pop_size = pop_strata == uint8_t(Strata) ? state.world.pop_get_size(pop_id) : 0.f;
					// All luxury needs
					// OR All everyday needs
					// OR All life needs
					// OR Some life needs
					// OR No needs fullfilled...
					sat_pool[(pop_id.get_luxury_needs_satisfaction() > 0.f)
						? 4
						: (pop_id.get_everyday_needs_satisfaction() > 0.f)
							? 3
							: (pop_id.get_life_needs_satisfaction() >= 1.f)
								? 2
								: (pop_id.get_life_needs_satisfaction() > 0.f)
									? 1
									: 0] += pop_size;
					total += pop_size;
				}
			});
			if(total <= 0.f) {
				enabled = false;
				return distrib;
			}

			for(size_t i = 0; i < sat_pool.size(); i++)
				distrib[uint8_t(i)] = sat_pool[i] / total;
		}
		return distrib;
	}
public:
	void on_create(sys::state& state) noexcept override {
		// Fill-in static information...
		dcon::fatten(state.world, dcon::pop_satisfaction_wrapper_id{ 0 }).set_name(text::find_or_add_key(state, "BUDGET_STRATA_NO_NEED"));
		dcon::fatten(state.world, dcon::pop_satisfaction_wrapper_id{ 1 }).set_name(text::find_or_add_key(state, "BUDGET_STRATA_NEED"));
		dcon::fatten(state.world, dcon::pop_satisfaction_wrapper_id{ 2 }).set_name(text::find_or_add_key(state, "BUDGET_STRATA_NEED"));
		dcon::fatten(state.world, dcon::pop_satisfaction_wrapper_id{ 3 }).set_name(text::find_or_add_key(state, "BUDGET_STRATA_NEED"));
		dcon::fatten(state.world, dcon::pop_satisfaction_wrapper_id{ 4 }).set_name(text::find_or_add_key(state, "BUDGET_STRATA_NEED"));

		piechart::on_create(state);
	}
};

class budget_tariffs_percent : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto tariffs = state.world.nation_get_tariffs(state.local_player_nation);
		set_text(state, std::to_string(tariffs) + "%");
	}
};

class budget_window : public window_element_base {
	dcon::nation_id current_nation{};
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "tariffs_percent") {
			return make_element_by_type<budget_tariffs_percent>(state, id);
		} else if(name == "total_funds_val") {
			return make_element_by_type<nation_budget_funds_text>(state, id);
		} else if(name == "national_bank_val") {
			return make_element_by_type<nation_budget_bank_text>(state, id);
		} else if(name == "debt_val") {
			return make_element_by_type<nation_budget_debt_text>(state, id);
		} else if(name == "chart_0") {
			return make_element_by_type<pop_satisfaction_piechart<culture::pop_strata::poor>>(state, id);
		} else if(name == "chart_1") {
			return make_element_by_type<pop_satisfaction_piechart<culture::pop_strata::middle>>(state, id);
		} else if(name == "chart_2") {
			return make_element_by_type<pop_satisfaction_piechart<culture::pop_strata::rich>>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(state.local_player_nation != current_nation) {
			current_nation = state.local_player_nation;
			Cyto::Any payload = current_nation;
			impl_set(state, payload);
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(current_nation);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

}
