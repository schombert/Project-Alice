#pragma once

#include "gui_element_types.hpp"
namespace ui {

class fps_counter_text_box : public simple_text_element_base {
private:
	std::chrono::time_point<std::chrono::steady_clock> last_render_time{};
	std::vector<float> history_render;
	int index_other = 0;
	int index_cpu = 0;
	std::vector<float> history_other;
	std::vector<float> history_cpu;
	std::unique_ptr<line_graph> graph_other;
	std::unique_ptr<line_graph> graph_cpu;
	std::unique_ptr<line_graph> baseline;
	std::vector<float> baseline_data{ 16666.f, 16666.f };
	float max_frame_time = 16666.f * 10.f;
public:

	void on_update(sys::state& state) noexcept override {
		for(unsigned int i = 0; i < state.query_frame_time_other.size(); i++) {
			GLint result;
			auto query_id = state.query_frame_time_other[i];
			if(!state.query_frame_time_other_free[i]) {
				glGetQueryObjectiv(query_id, GL_QUERY_RESULT_AVAILABLE, &result);
				if(result == GL_TRUE) {
					GLint query_time; //nanoseconds
					glGetQueryObjectiv(query_id, GL_QUERY_RESULT, &query_time);
					state.query_frame_time_other_free[i] = true;
					history_other[index_cpu] = query_time / 1000.f;
					//index_other = (index_other + 1) % 300;
				}
			}
		}

		graph_other->set_data_points(state, history_other, 0.f, max_frame_time);
	}

	void on_create(sys::state& state) noexcept override {

		graph_other = ui::make_element_by_type<line_graph>(state, "fps_counter", 300);
		graph_cpu = ui::make_element_by_type<line_graph>(state, "fps_counter", 300);
		baseline = ui::make_element_by_type<line_graph>(state, "fps_counter", 2);

		simple_text_element_base::on_create(state);

		history_other.resize(300);
		history_cpu.resize(300);

		set_text(state, "0");

		graph_other->base_data.size.x = 400;
		graph_other->base_data.size.y = 200;
		graph_other->is_coloured = true;
		graph_other->b = 1;

		graph_cpu->base_data.size.x = 400;
		graph_cpu->base_data.size.y = 200;
		graph_cpu->is_coloured = true;

		baseline->base_data.size.x = 400;
		baseline->base_data.size.y = 200;
		baseline->set_data_points(state, baseline_data, 0.f, max_frame_time);
		baseline->is_coloured = true;
		baseline->g = 1;
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		

		std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
		if(last_render_time == std::chrono::time_point<std::chrono::steady_clock>{}) {
			last_render_time = now;
		}
		auto microseconds_since_last_render = std::chrono::duration_cast<std::chrono::microseconds>(now - last_render_time);
		auto frames_per_second = 1.f / float(microseconds_since_last_render.count() / 1e6);
		set_text(state, std::to_string(int32_t(frames_per_second)));
		history_cpu[index_cpu] = (float)microseconds_since_last_render.count();
		index_cpu = (index_cpu + 1) % 300;
		graph_cpu->set_data_points(state, history_cpu, 0.f, max_frame_time);
		last_render_time = now;

		simple_text_element_base::render(state, x + 420, y + 220);
		ogl::render_simple_rect(state, (float)x, (float)y, 400.f, 200.f, ui::rotation::upright, false, false);
		baseline->render(state, x, y);
		graph_other->render(state, x, y);
		graph_cpu->render(state, x, y);

		on_update(state);
	}
};

} // namespace ui
