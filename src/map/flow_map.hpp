#pragma once
#include <atomic>
#include <map>

namespace flow_map {

enum data_source {
	external,
	commodity,
	administration,
	none
};

struct flow_particle {
	glm::vec2 position_;
	glm::vec2 target_;
	int graph_node_current;
	int graph_node_prev = -1;
	int graph_node_next;
	int adj_index = -1;
	int adj_count = 0;
	int adj_direction = 1;
	std::array<glm::vec2, 5> vagon_positions{};
};

struct flow_map_data {
	std::atomic<bool> update_requested = true;
	data_source source = data_source::commodity;

	float cutoff = 0.f;

	// particles
	int amount_of_particles;
	std::vector<flow_particle> flow_particles_positions {};
	std::vector<int32_t> flow_particles_content {};

	std::vector<ankerl::unordered_dense::map<int32_t, std::vector<float>>> flow_graph {};
	std::vector<ankerl::unordered_dense::map<int32_t, std::vector<float>>> particle_next_node_probability{};
	std::vector<std::vector<float>> node_probability_create{};
	std::vector<float> edge_layer_probability{};
	std::vector<std::vector<float>> node_total_out{};
	std::vector<std::vector<float>> node_total_in{};

	std::vector<glm::vec2> node_position{};

	void request_update(data_source s) {
		source = s;
		update_requested.store(true, std::memory_order::release);
	}
};

void update(sys::state& state);

}

