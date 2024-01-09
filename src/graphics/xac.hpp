#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string>
#include <vector>
#include "parsers.hpp"

namespace emfx {
	enum class xac_chunk_type : uint32_t {
		unknown_0 = 0,
		mesh = 1,
		skinning = 2,
		material_3 = 3,
		unknown_4 = 4,
		material = 5,
		metadata = 7,
		node_hierachy = 11,
		material_block = 13,
		count
	};

	struct xac_vector2f {
		float x = 0.f;
		float y = 0.f;
	};
	struct xac_vector3f {
		float x = 0.f;
		float y = 0.f;
		float z = 0.f;
	};
	struct xac_vector4f {
		float x = 0.f;
		float y = 0.f;
		float z = 0.f;
		float w = 0.f;
	};
	struct xac_color_rgba {
		float r = 0.f;
		float g = 0.f;
		float b = 0.f;
		float a = 0.f;
	};
	struct xac_color_rgb {
		float r = 0.f;
		float g = 0.f;
		float b = 0.f;
	};
	struct xac_mat4x4 {
		float m[16] = { 0.f };
	};

	//post-parse data
	struct xac_pp_actor_submesh {
		std::vector<uint32_t> indices;
		std::vector<uint32_t> bone_ids;
		int32_t material_id = -1;
		uint32_t num_vertices = 0;
		uint32_t vertex_offset = 0;
	};
	struct xac_pp_bone_influence {
		float weight = 0.f;
		int32_t bone_id = -1;
	};
	struct xac_pp_actor_mesh {
		std::vector<xac_vector4f> weights;
		std::vector<xac_vector3f> vertices;
		std::vector<xac_vector3f> normals;
		std::vector<xac_vector2f> texcoords;
		std::vector<xac_pp_bone_influence> influences;
		std::vector<xac_pp_actor_submesh> submeshes;
		std::vector<uint32_t> influence_indices;
		std::vector<uint32_t> influence_starts;
		std::vector<uint32_t> influence_counts;
	};
	struct xac_pp_actor_node {
		std::string name;
		std::vector<xac_pp_actor_mesh> meshes;
		xac_vector4f rotation;
		xac_vector4f scale_rotation;
		xac_vector3f position;
		xac_vector3f scale;
		xac_mat4x4 transform;
		int32_t parent_id = -1;
		int32_t collision_mesh = -1;
		int32_t visual_mesh = -1;
	};
	enum class xac_pp_material_map_type : uint8_t {
		none = 0,
		diffuse = 2,
		specular = 3
	};
	struct xac_pp_actor_material_layer {
		std::string texture;
		float amount = 0.f;
		float u_offset = 0.f;
		float v_offset = 0.f;
		float u_tiling = 0.f;
		float v_tiling = 0.f;
		float rotation = 0.f; //radians?
		int16_t material_id = -1;
		xac_pp_material_map_type map_type = xac_pp_material_map_type::none;
	};
	struct xac_pp_actor_material {
		std::string name;
		std::vector<xac_pp_actor_material_layer> layers;
		xac_color_rgba ambient_color;
		xac_color_rgba diffuse_color;
		xac_color_rgba specular_color;
		xac_color_rgba emissive_color;
		float shine = 1.f;
		float shine_strength = 1.f;
		float opacity = 1.f;
		float ior = 1.f;
		bool double_sided = false;
		bool wireframe = false;
	};
	struct xac_context {
		std::vector<xac_pp_actor_node> root_nodes;
		std::vector<xac_pp_actor_node> nodes;
		std::vector<xac_pp_actor_material> materials;
		uint32_t max_standard_materials = 0; // Normal standard materials (diffuse, ambient, normal, etc)
		uint32_t max_fx_materials = 0; // Especial effect materials (smoke, fire, glow, etc)
		bool ignore_length = false; // Length of chunks sometimes doesn't align with their true size
	};

	template<typename T>
	T parse_xac_any_binary(const char** start, const char* end, parsers::error_handler& err) {
		if (*start + sizeof(T) >= end) {
			err.accumulated_errors += "buffer overflow access! (" + err.file_name + ")\n";
			return T{};
		}
		T obj = *reinterpret_cast<const T*>(*start);
		*start += sizeof(T);
		return obj;
	}

	const char* parse_xac_cstring(const char* start, const char* end, parsers::error_handler& err);
	const char* parse_xac_cstring_nodiscard(std::string& out, const char* start, const char* end, parsers::error_handler& err);

	struct xac_metadata_chunk_header {
		uint32_t reposition_mask = 0;
		int32_t reposition_node = 0;
		uint8_t exporter_major_version = 0;
		uint8_t exporter_minor_version = 0;
		uint8_t unused[2] = { 0 };
		float retarget_root_offset = 0.f;
	};

	struct xac_material_block_v1_chunk_header {
		uint32_t num_total_materials = 0;
		uint32_t num_standard_materials = 0;
		uint32_t num_fx_materials = 0;
	};

	struct xac_material_v2_chunk_header {
		xac_color_rgba ambient_color;
		xac_color_rgba diffuse_color;
		xac_color_rgba specular_color;
		xac_color_rgba emissive_color;
		float shine = 1.f;
		float shine_strength = 1.f;
		float opacity = 1.f;
		float ior = 1.f;
		bool double_sided = false;
		bool wireframe = false;
		uint8_t unused = 0;
		uint8_t num_layers = 0;
	};
	struct xac_material_layer_v2_header {
		float amount = 1.f;
		float u_offset = 0.f;
		float v_offset = 0.f;
		float u_tiling = 0.f;
		float v_tiling = 0.f;
		float rotation = 0.f; //radians?
		int16_t material_id = -1;
		uint8_t map_type = 0;
		uint8_t unused = 0;
	};

	struct xac_node_hierachy_v1_chunk_header {
		uint32_t num_nodes = 0;
		uint32_t num_root_nodes = 0;
	};
	struct xac_node_hierachy_v1_node_header {
		xac_vector4f rotation;
		xac_vector4f scale_rotation;
		xac_vector3f position;
		xac_vector3f scale;
		float unused[3] = { 0 };
		uint32_t unknown[2] = { 0 };
		int32_t parent_id = -1;
		uint32_t num_children = 0;
		int32_t include_in_bounds_calc = 0;
		xac_mat4x4 transform;
		float importance_factor = 0.f;
	};

	struct xac_mesh_v1_chunk_header {
		int32_t node_id = -1;
		uint32_t num_influence_ranges = 0;
		uint32_t num_vertices = 0;
		uint32_t num_indices = 0;
		uint32_t num_sub_meshes = 0;
		uint32_t num_attribute_layers = 0;
		uint8_t is_collision_mesh = 0;
		uint8_t unused[3] = { 0 };
		// vertex blocks[vertex_block_count]
	};
	struct xac_vertex_block_v1_header {
		uint32_t ident = 0;
		uint32_t size = 0;
		uint8_t keep_original = 0;
		uint8_t is_scale_factor = 0;
		uint8_t unused[2] = { 0, 0 };
		// data[size * count];
	};
	enum class xac_vertex_block_v1_type : uint32_t {
		vertex = 0,
		normal = 1,
		weight = 2,
		texcoord = 3,
		color32 = 4,
		influence_indices = 5,
		color128 = 6,
		count
	};
	struct xac_submesh_v1_header {
		uint32_t num_indices = 0;
		uint32_t num_vertices = 0;
		int32_t material_id = -1;
		uint32_t num_bones = 0;
	};

	struct xac_skinning_v3_chunk_header {
		int32_t node_id = -1;
		uint32_t num_local_bones = 0;
		uint32_t num_influences = 0;
		uint8_t is_for_collision_mesh = 0;
		uint8_t unused[3] = { 0, 0, 0 };
		// vertex blocks[vertex_block_count]
	};
	struct xac_skinning_v3_influence_entry {
		float weight = 0.f;
		int16_t bone_id = -1;
		uint8_t unused[2] = { 0 };
	};
	struct xac_skinning_v3_influence_range {
		uint32_t first_influence_index = 0;
		uint32_t num_influences = 0;
	};

	struct xac_header {
		uint8_t ident[4] = { 'X', 'A', 'C', ' ' };
		uint8_t major_version = 1;
		uint8_t minor_version = 0;
		uint8_t big_endian = 0;
		uint8_t multiply_order = 0;
	};
	struct xac_chunk_header {
		uint32_t ident = 0;
		uint32_t len = 0;
		uint32_t version = 0;
		// data[len]
	};
	
	void parse_xac(xac_context& context, const char* start, const char* end, parsers::error_handler& err);
	void finish(xac_context& context);
}


