#pragma once
#include "system_state.hpp"
#include "gui_element_types.hpp"
#include "color_templates.hpp"
#include "politics.hpp"

namespace ui {

template<class T>
class piechart : public element_base {
public:

	static constexpr int32_t resolution = 200;
	static constexpr size_t channels = 3;

	struct entry {
		float value;
		T key;
		uint8_t slices;
		entry(T key, float value) : value(value), key(key), slices(0) {
		}
		entry() : value(0.0f), key(), slices(0) {
		}
	};

	ogl::data_texture data_texture{ resolution, channels };
	std::vector<entry> distribution;
	float radius = 0.f;

	void update_chart(sys::state& state);

	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	virtual void populate_tooltip(sys::state& state, T t, float percentage, text::columnar_layout& contents) noexcept;

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::position_sensitive_tooltip;
	}
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(type == mouse_probe_type::scroll)
			return message_result::unseen;

		float dx = float(x) - radius;
		float dy = float(y) - radius;
		auto dist = sqrt(dx * dx + dy * dy);
		return dist <= radius ? message_result::consumed : message_result::unseen;
	}
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		return message_result::consumed;
	}
	message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		return message_result::consumed;
	}
};

template<class T>
void piechart<T>::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(distribution.size() > 0)
		ogl::render_piechart(state, ogl::color_modification::none, float(x), float(y), float(base_data.size.x), data_texture);
}

template<class T>
void piechart<T>::on_create(sys::state& state) noexcept {
	base_data.position.x -= base_data.size.x;
	radius = float(base_data.size.x);
	base_data.size.x *= 2;
	base_data.size.y *= 2;
}

template<class T>
void piechart<T>::update_chart(sys::state& state) {
	std::sort(distribution.begin(), distribution.end(), [](auto const& a, auto const& b) { return a.value > b.value; });
	float total = 0.0f;
	for(auto& e : distribution) {
		total += e.value;
	}
	int32_t int_total = 0;

	if(total != 0.0f) {
		for(auto& e : distribution) {
			auto ivalue = int32_t(e.value * float(resolution) / total);
			e.slices = uint8_t(ivalue);
			e.value /= total;
			int_total += ivalue;
		}
	} else {
		distribution.clear();
	}

	if(int_total < resolution && distribution.size() > 0) {
		auto rem = resolution - int_total;
		while(rem > 0) {
			for(auto& e : distribution) {
				e.slices += uint8_t(1);
				rem -= 1;
				if(rem == 0)
					break;
			}
		}
	} else if(int_total > resolution) {
		assert(false);
	}

	size_t i = 0;
	for(auto& e : distribution) {
		uint32_t color = ogl::get_ui_color<T>(state, e.key);
		auto slice_count = size_t(e.slices);

		for(size_t j = 0; j < slice_count; j++) {
			data_texture.data[(i + j) * channels] = uint8_t(color & 0xFF);
			data_texture.data[(i + j) * channels + 1] = uint8_t(color >> 8 & 0xFF);
			data_texture.data[(i + j) * channels + 2] = uint8_t(color >> 16 & 0xFF);
		}

		i += slice_count;
	}
	for(; i < resolution; i++) {
		data_texture.data[i * channels] = uint8_t(0);
		data_texture.data[i * channels + 1] = uint8_t(0);
		data_texture.data[i * channels + 2] = uint8_t(0);
	}

	data_texture.data_updated = true;
}

template<class T>
void piechart<T>::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	float const PI = 3.141592653589793238463f;
	float dx = float(x) - radius;
	float dy = float(y) - radius;
	size_t index = 0;
	if(dx != 0.0f || dy != 0.0f) {
		float dist = std::sqrt(dx * dx + dy * dy);
		float angle = std::acos(-dx / dist);
		if(dy > 0.f) {
			angle = PI + (PI - angle);
		}
		index = size_t(angle / (2.f * PI) * float(resolution));
	}
	for(auto const& e : distribution) {
		if(index < size_t(e.slices)) {
			populate_tooltip(state, e.key, e.value, contents);
			return;
		}
		index -= size_t(e.slices);
	}
}

template<class T>
void piechart<T>::populate_tooltip(sys::state& state, T t, float percentage, text::columnar_layout& contents) noexcept {
	auto fat_t = dcon::fatten(state.world, t);
	auto box = text::open_layout_box(contents, 0);
	if constexpr(!std::is_same_v<dcon::nation_id, T>)
		text::add_to_layout_box(state, contents, box, fat_t.get_name(), text::substitution_map{});
	else
		text::add_to_layout_box(state, contents, box, text::get_name(state, t), text::substitution_map{});
	text::add_to_layout_box(state, contents, box, std::string(":"), text::text_color::white);
	text::add_space_to_layout_box(state, contents, box);
	text::add_to_layout_box(state, contents, box, text::format_percentage(percentage, 1), text::text_color::white);
	text::close_layout_box(contents, box);
}


template<class SrcT, class DemoT>
class demographic_piechart : public piechart<DemoT> {
public:
	void on_update(sys::state& state) noexcept override {
		this->distribution.clear();

		Cyto::Any obj_id_payload = SrcT{};
		size_t i = 0;
		if(this->parent) {
			this->parent->impl_get(state, obj_id_payload);
			float total_pops = 0.f;

			for_each_demo(state, [&](DemoT demo_id) {
				float volume = 0.f;
				if(obj_id_payload.holds_type<dcon::province_id>()) {
					auto demo_key = demographics::to_key(state, demo_id);
					auto prov_id = any_cast<dcon::province_id>(obj_id_payload);
					volume = state.world.province_get_demographics(prov_id, demo_key);
				} else if(obj_id_payload.holds_type<dcon::nation_id>()) {
					auto demo_key = demographics::to_key(state, demo_id);
					auto nat_id = any_cast<dcon::nation_id>(obj_id_payload);
					volume = state.world.nation_get_demographics(nat_id, demo_key);
				}

				if constexpr(std::is_same_v<SrcT, dcon::pop_id>) {
					if(obj_id_payload.holds_type<dcon::pop_id>()) {
						auto demo_key = pop_demographics::to_key(state, demo_id);
						auto pop_id = any_cast<dcon::pop_id>(obj_id_payload);
						volume = pop_demographics::get_demo(state, pop_id, demo_key);
					}
				}
				if(volume > 0)
					this->distribution.emplace_back(demo_id, volume);
			});
		}

		this->update_chart(state);
	}
	virtual void for_each_demo(sys::state& state, std::function<void(DemoT)> fun) {
	}
};

template<class SrcT>
class culture_piechart : public demographic_piechart<SrcT, dcon::culture_id> {
protected:
	void for_each_demo(sys::state& state, std::function<void(dcon::culture_id)> fun) override {
		state.world.for_each_culture(fun);
	}
};

template<class SrcT>
class workforce_piechart : public demographic_piechart<SrcT, dcon::pop_type_id> {
protected:
	void for_each_demo(sys::state& state, std::function<void(dcon::pop_type_id)> fun) override {
		state.world.for_each_pop_type(fun);
	}
};

template<class SrcT>
class ideology_piechart : public demographic_piechart<SrcT, dcon::ideology_id> {
protected:
	void for_each_demo(sys::state& state, std::function<void(dcon::ideology_id)> fun) override {
		state.world.for_each_ideology(fun);
	}
};


class upper_house_piechart : public piechart<dcon::ideology_id> {
protected:
	void on_update(sys::state& state) noexcept override {
		auto nat_id = retrieve<dcon::nation_id>(state, parent);
		distribution.clear();
		for(auto id : state.world.in_ideology) {
			distribution.emplace_back(id.id, float(state.world.nation_get_upper_house(nat_id, id)));
		}
		update_chart(state);
	}
};

class voter_ideology_piechart : public piechart<dcon::ideology_id> {
protected:
	void on_update(sys::state& state) noexcept override {
		auto nat_id = retrieve<dcon::nation_id>(state, parent);
		distribution.clear();
		for(auto id : state.world.in_ideology) {
			distribution.emplace_back(id.id, 0.0f);
		}
		for(auto p : state.world.nation_get_province_ownership(nat_id)) {
			for(auto pop_loc : state.world.province_get_pop_location(p.get_province())) {
				auto pop_id = pop_loc.get_pop();
				float vote_size = politics::pop_vote_weight(state, pop_id, nat_id);
				if(vote_size > 0) {
					state.world.for_each_ideology([&](dcon::ideology_id iid) {
						auto dkey = pop_demographics::to_key(state, iid);
						distribution[iid.index()].value += pop_demographics::get_demo(state, pop_id.id, dkey) * vote_size;
					});
				}
			}
		}

		update_chart(state);
	}
};


}
