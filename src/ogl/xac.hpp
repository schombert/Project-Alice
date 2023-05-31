#include <stdint.h>
#include <stddef.h>
#include <string>
#include <vector>

#define XAC_DEBUG 1

namespace ogl {

enum class xac_chunk_type : uint32_t {
	unknown_0 = 0,
	object = 1,
	unknown_2 = 2,
	material_3 = 3,
	unknown_4 = 4,
	material = 5,
	metadata = 7,
	material_block = 13,
	count
};

struct xac_chunk_data_object {
	uint32_t reserved1[2];
	uint32_t vertex_count;
	uint32_t face_index_count;
	uint32_t reserved2;
	uint32_t vertex_block_count;
	uint32_t reserved3;
	// vertex blocks[vertex_block_count]
};
struct xac_chunk_data_material {
	uint32_t material; // materials...?
};
struct xac_chunk_data_0d {
	uint32_t reserved[2];
	uint32_t obj_count; // ???
};

struct xac_chunk_data_01_footer {
	uint32_t face_index_count;
	uint32_t vertex_count;
	uint32_t reserved[2];
	// uint32_t [face_index_count] vertex indices, each face may be defined by 3 vertex indices
};

struct xac_vertex_block_data_vector2f {
	uint32_t x; // transformed to ieee747 floats
	uint32_t y;
	uint32_t z;
};
struct xac_vertex_block_data_vector3f {
	uint32_t x; // transformed to ieee747 floats
	uint32_t y;
	uint32_t z;
};
struct xac_vertex_block_data_vector4f {
	uint32_t x; // transformed to ieee747 floats
	uint32_t y;
	uint32_t z;
	uint32_t w;
};
struct xac_vertex_block_data_02 {
	uint32_t reserved;
};

struct xac_vector2f {
	float x;
	float y;
};
struct xac_vector3f {
	float x;
	float y;
	float z;
};
struct xac_vector4f {
	float x;
	float y;
	float z;
	float w;
};
struct xac_object {
	std::vector<xac_vector2f> texcoords;
	std::vector<xac_vector3f> vertices;
	std::vector<xac_vector3f> normals;
	std::vector<xac_vector4f> weights;
};
struct xac_context {
	std::vector<xac_object> objects;
};

const char *parse_xac_cstring(const char *start, const char *end, parsers::error_handler& err) {
	auto len = *reinterpret_cast<const uint32_t*>(start);
	start += sizeof(len);
	return start + len;
}
void parse_xac(xac_context& context, const char *start, const char *end, parsers::error_handler& err) {
	struct xac_header {
		uint8_t ident[4];
		uint16_t minor_version;
		uint16_t major_version;
	};
	auto h = *reinterpret_cast<const xac_header*>(start);
	start += sizeof(h);
	if(h.ident[0] != uint8_t('X') || h.ident[1] != uint8_t('A') || h.ident[2] != uint8_t('C') || h.ident[3] != uint8_t(' ')) {
		err.accumulated_errors += "Invalid XAC identifier on " + err.file_name + "\n";
	}

	while(start < end) {
		struct xac_chunk_header {
			uint32_t ident;
			uint32_t len;
			uint32_t count;
			// data[len]
		};
		auto ch = *reinterpret_cast<const xac_chunk_header*>(start);
		start += sizeof(ch);

		const char *expected = start + ch.len;
		switch(xac_chunk_type(ch.ident)) {
		case xac_chunk_type::object: {
#ifdef XAC_DEBUG
			std::printf("CT,Object\n");
#endif
			xac_object obj{};

			auto cd = *reinterpret_cast<const xac_chunk_data_object*>(start); 
			start += sizeof(cd);
			// Parse vertex blocks
			for(uint32_t i = 0; i < cd.vertex_block_count; ++i) {
				struct xac_vertex_block_header {
					uint32_t ident;
					uint32_t size;
					int16_t reserved[2];
					// data[size * count];
				};
				auto vbh = *reinterpret_cast<const xac_vertex_block_header*>(start); 
				start += sizeof(vbh);

				enum class xac_vertex_block_type : uint32_t {
					vertex = 0,
					normal = 1,
					weight = 2,
					texcoord = 3,
					count
				};
				switch(xac_vertex_block_type(vbh.ident)) {
				case xac_vertex_block_type::normal: {
					auto vbd = *reinterpret_cast<const xac_vertex_block_data_vector3f*>(start); 
					start += sizeof(vbd);

					xac_vector3f normal;
					normal.x = *reinterpret_cast<float*>(&vbd.x);
					normal.y = *reinterpret_cast<float*>(&vbd.y);
					normal.z = *reinterpret_cast<float*>(&vbd.z);
					obj.normals.push_back(normal);
					break;
				}
				case xac_vertex_block_type::vertex: {
					auto vbd = *reinterpret_cast<const xac_vertex_block_data_vector3f*>(start); 
					start += sizeof(vbd);

					xac_vector3f vertex;
					vertex.x = *reinterpret_cast<float*>(&vbd.x);
					vertex.y = *reinterpret_cast<float*>(&vbd.y);
					vertex.z = *reinterpret_cast<float*>(&vbd.z);
					obj.vertices.push_back(vertex);
					break;
				}
				case xac_vertex_block_type::texcoord: {
					auto vbd = *reinterpret_cast<const xac_vertex_block_data_vector2f*>(start); 
					start += sizeof(vbd);

					xac_vector2f texcoord;
					texcoord.x = *reinterpret_cast<float*>(&vbd.x);
					texcoord.y = *reinterpret_cast<float*>(&vbd.y);
					obj.texcoords.push_back(texcoord);
					break;
				}
				case xac_vertex_block_type::weight: {
					auto vbd = *reinterpret_cast<const xac_vertex_block_data_vector4f*>(start); 
					start += sizeof(vbd);

					xac_vector4f weight;
					weight.x = *reinterpret_cast<float*>(&vbd.x);
					weight.y = *reinterpret_cast<float*>(&vbd.y);
					weight.z = *reinterpret_cast<float*>(&vbd.z);
					weight.w = *reinterpret_cast<float*>(&vbd.w);
					obj.weights.push_back(weight);
					break;
				}
				default:
					err.accumulated_warnings += "Unknown vertex block type " + std::to_string(vbh.ident) + " on " + err.file_name + "\n";
					start += vbh.size * cd.vertex_count;
					break;
				}
			}
			context.objects.push_back(obj);
			break;
		}
		case xac_chunk_type::metadata: {
#ifdef XAC_DEBUG
			std::printf("CT,Metadata\n");
#endif
			start += sizeof(uint32_t) * 4;
#ifdef XAC_DEBUG
			std::printf("program-name=%s\n", start + 4);
#endif
			start = parse_xac_cstring(start, end, err);
#ifdef XAC_DEBUG
			std::printf("workspace-folder-name=%s\n", start + 4);
#endif
			start = parse_xac_cstring(start, end, err);
#ifdef XAC_DEBUG
			std::printf("edition-or-creation-date=%s\n", start + 4);
#endif
			start = parse_xac_cstring(start, end, err);
			auto unknown5 = *reinterpret_cast<const uint32_t*>(start);
			start += sizeof(unknown5);
			break;
		}
		case xac_chunk_type::material_block: {
#ifdef XAC_DEBUG
			std::printf("CT,MatBlock\n");
#endif
			auto n_materials = *reinterpret_cast<const uint32_t*>(start);
			start += sizeof(n_materials);
			auto unknown1 = *reinterpret_cast<const uint32_t*>(start);
			start += sizeof(unknown1);
			auto unknown2 = *reinterpret_cast<const uint32_t*>(start);
			start += sizeof(unknown2);
			for(uint32_t i = 0; i < n_materials; ++i) {
				struct xac_material_block_material_data {
					uint32_t ident;
					uint32_t size;
					uint32_t count;
				};
				auto mch = *reinterpret_cast<const xac_material_block_material_data*>(start);
				switch(mch.ident) {
				case 3:
					break;
				case 5:
					break;
				}
			}
			break;
		}
		case xac_chunk_type::material_3: {
/*
        self.inFile.read('16f')
        power = self.inFile.readFloat()
        self.inFile.read('3f')
        self.inFile.read('3B')
        numTex = self.inFile.readByte()
        matName = self.read_name()
    
        for i in range(numTex):
            self.inFile.read('6f')
            self.inFile.read('2H')
            self.read_name()
        
        material = NoeMaterial(matName, "")
        self.matList.append(material)       
*/
			struct xac_material3_header {
				uint32_t unknown_4f[16];
				uint32_t power;
				uint32_t unknown_f[3]; // floats
				uint8_t unknown_b[3];
				uint8_t num_tex;
			};
			auto mh = *reinterpret_cast<const xac_material3_header*>(start);
			start += sizeof(mh);
#ifdef XAC_DEBUG
			std::printf("material-name=%s,nmat=%u\n", start + 4, mh.num_tex);
#endif
			start = parse_xac_cstring(start, end, err);
			for(uint8_t i = 0; i < mh.num_tex; ++i) {
				start += 6 * sizeof(uint32_t);
				start += 2 * sizeof(uint16_t);
#ifdef XAC_DEBUG
				std::printf("mats=%s\n", start + 4);
#endif
				start = parse_xac_cstring(start, end, err);
			}
			break;
		}
		default:
#ifdef XAC_DEBUG
			std::printf("CT,Unknown-(%i)\n", int16_t(ch.ident));
#endif
			err.accumulated_warnings += "Unknown vertex block type " + std::to_string(ch.ident) + " (size " + std::to_string(ch.len) + " @ offset " + std::to_string(uint32_t(start - start)) + ") on " + err.file_name + "\n";
			start += ch.len;
			break;
		}

		if(start != expected) {
			err.accumulated_errors += "Incorrect parsing for chunk ident " + std::to_string(uint32_t(ch.ident)) + " on " + err.file_name + "\n";
			start = expected;
		}
	}

#ifdef XAC_DEBUG
	std::printf("Objects: %zu\n", context.objects.size());
	for(const auto& o : context.objects) {
		std::printf("OBJECT\n");
		std::printf("* vertices: %zu\n", o.vertices.size());
		std::printf("* normals: %zu\n", o.normals.size());
		std::printf("* texcoords: %zu\n", o.texcoords.size());
	}
	std::printf("Errors:\n%s\n", err.accumulated_errors.c_str());
	std::printf("Warns:\n%s\n", err.accumulated_warnings.c_str());
#endif
}

}