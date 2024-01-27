#include "xac.hpp"
#include "parsers.hpp"
#include <cassert>

/*
==========================================================================================================
FOR XAC FILES
https://web.archive.org/web/20140816034951/https://dl.dropboxusercontent.com/u/60681258/xr/xac.txt
vec3d = float x, float y, float z
vec4d = float x, float y, float z, float w
quat = float x, float y, float z, float w
string = uint32 len, char[len]
matrix44 = vec4d col1, vec4d col2, vec4d col3, vec4d pos

file:
	byte magic[4] = 58 41 43 20 ("XAC ")
	byte majorVersion = 1
	byte minorVersion = 0
	byte bBigEndian
	byte multiplyOrder

	chunk[...]
		int32 chunkType
		int32 length (sometimes incorrect!)
		int32 version
		byte data[length]

chunk 7: metadata (v2)
	uint32 repositionMask
		1 = repositionPos
		2 = repositionRot
		4 = repositionScale
	int32 repositioningNode
	byte exporterMajorVersion
	byte exporterMinorVersion
	byte unused[2]
	float retargetRootOffset
	string sourceApp
	string origFileName
	string exportDate
	string actorName

chunk B: node hierarchy (v1)
	int32 numNodes
	int32 numRootNodes (number of nodes with parentId = -1)

	NodeData[numNodes]
		quat rotation
		quat scaleRotation
		vec3d position
		vec3d scale
		float unused[3]
		int32 -1 (?)
		int32 -1 (?)
		int32 parentNodeId (index of parent node or -1 for root nodes)
		int32 numChildNodes (number of nodes with parentId = this node's index)
		int32 bIncludeInBoundsCalc
		matrix44 transform
		float fImportanceFactor
		string name

chunk D: material totals (v1)
	int32 numTotalMaterials
	int32 numStandardMaterials
	int32 numFxMaterials

chunk 3: material definition (v2)
	vec4d ambientColor
	vec4d diffuseColor
	vec4d specularColor
	vec4d emissiveColor
	float shine
	float shineStrength
	float opacity
	float ior
	byte bDoubleSided
	byte bWireframe
	byte unused
	byte numLayers
	string name

	Layer[numLayers]:
		float amount
		float uOffset
		float vOffset
		float uTiling
		float vTiling
		float rotationInRadians
		int16 materialId (index of the material this layer belongs to = number of preceding chunk 3's)
		byte mapType
		byte unused
		string texture

chunk 1: mesh (v1)
	int32 nodeId
	int32 numInfluenceRanges
	int32 numVertices (total number of vertices of submeshes)
	int32 numIndices  (total number of indices of submeshes)
	int32 numSubMeshes
	int32 numAttribLayers
	byte bIsCollisionMesh (each node can have 1 visual mesh and 1 collision mesh)
	byte pad[3]

	VerticesAttribute[numAttribLayers]
		int32 type (determines meaning of data)
			0 = positions (vec3d)
			1 = normals (vec3d)
			2 = tangents (vec4d)
			3 = uv coords (vec2d)
			4 = 32-bit colors (uint32)
			5 = influence range indices (uint32) - index into the InfluenceRange[] array of chunk 2, indicating the bones that affect it
			6 = 128-bit colors

			typically: 1x positions, 1x normals, 2x tangents, 2x uv, 1x colors, 1x influence range indices
		int32 attribSize (size of 1 attribute, for 1 vertex)
		byte bKeepOriginals
		byte bIsScaleFactor
		byte pad[2]
		byte data[numVertices * attribSize]

	SubMesh[numSubMeshes]
		int32 numIndices
		int32 numVertices
		int32 materialId
		int32 numBones
		int32 relativeIndices[numIndices] (actual index = relative index + total number of vertices of preceding submeshes. each group of 3 sequential indices (vertices) defines a polygon)
		int32 boneIds[numBones] (unused)

chunk 2: skinning (v3)
	int32 nodeId
	int32 numLocalBones (number of distinct boneId's in InfluenceData)
	int32 numInfluences
	byte bIsForCollisionMesh
	byte pad[3]

	InfluenceData[numInfluences]
		float fWeight (0..1)   (for every vertex, the resulting transformed position is calculated for every influencing bone;
		int16 boneId            the final position is the weighted average of these positions using fWeight as weight)
		byte pad[2]

	InfluenceRange[bIsForCollisionMesh ? nodes[nodeId].colMesh.numInfluenceRanges : nodes[nodeId].visualMesh.numInfluenceRanges]
		int32 firstInfluenceIndex (index into InfluenceData)
		int32 numInfluences (number of InfluenceData entries relevant for one or more vertices, starting at firstInfluenceIndex)

chunk C: morph targets (v1)
	int32 numMorphTargets
	int32 lodMorphTargetIdx (presumably always 0; this is the index of a *collection* of numMorphTargets morph targets, not an
							 individual target, and an EmoActor only has one such collection)

	MorphTarget[numMorphTargets]
		float fRangeMin (at runtime, fMorphAmount must be >= fRangeMin)
		float fRangeMax (at runtime, fMorphAmount must be <= fRangeMax)
		int32 lodLevel (LOD of visual mesh; presumably always 0)
		int32 numDeformations
		int32 numTransformations
		int32 phonemeSetBitmask (indicates which phonemes the morph target can be used for - facial animation)
			0x1: neutral
			0x2: M, B, P, X
			0x4: AA, AO, OW
			0x8: IH, AE, AH, EY, AY, H
			0x10: AW
			0x20: N, NG, CH, J, DH, D, G, T, K, Z, ZH, TH, S, SH
			0x40: IY, EH, Y
			0x80: UW, UH, OY
			0x100: F, V
			0x200: L, EL
			0x400: W
			0x800: R, ER

		string name

		Deformation[numDeformations]
			int32 nodeId
			float fMinValue
			float fMaxValue
			int32 numVertices
			DeformVertex16 positionOffsets[numVertices]
				uint16 x (fXOffset = fMinValue + (fMaxValue - fMinValue)*(x / 65535); vecDeformedPos.fX = vecPos.fX + fXOffset*fMorphAmount)
				uint16 y
				uint16 z
			DeformVertex8 normalOffsets[numVertices]
				byte x (fXOffset = x/127.5 - 1.0; vecDeformedNormal.fX = vecNormal.fX + fXOffset * fMorphAmount)
				byte y
				byte z
			DeformVertex8 tangentOffsets[numVertices] (offsets for first tangent)
			uint32 vertexIndices[numVertices] (index of the node's visual mesh vertex which the offsets apply to)

		Transformation[numTransformations] (appears to be unused, i.e. numTransformations = 0)
			int32 nodeId
			quat rotation
			quat scaleRotation
			vec3d pos
			vec3d scale

==========================================================================================================
FOR XSM FILES
https://web.archive.org/web/20140816034945/https://dl.dropboxusercontent.com/u/60681258/xr/xsm.txt
vec3d = float x, float y, float z
quat16 = int16 x, int16 y, int16 z, int16 w; fX = x / 32767
string = uint32 len, char[len]

file:
	byte magic[4] = 58 53 4D 20 ("XSM ")
	byte majorVersion = 1
	byte minorVersion = 0
	byte bBigEndian
	byte pad

	chunk[...]
		int32 chunkType
		int32 length
		int32 version
		byte data[length]

chunk C9: metadata (v2)
	float unused = 1.0f
	float fMaxAcceptableError
	int32 fps
	byte exporterMajorVersion
	byte exporterMinorVersion
	byte pad[2]
	string sourceApp
	string origFileName
	string exportDate
	string motionName

chunk CA: bone animation (v2)
	int32 numSubMotions
	SkeletalSubMotion[numSubMotions]:
		quat16 poseRot
		quat16 bindPoseRot
		quat16 poseScaleRot
		quat16 bindPoseScaleRot
		vec3D posePos
		vec3D poseScale
		vec3D bindPosePos
		vec3D bindPoseScale
		int32 numPosKeys
		int32 numRotKeys
		int32 numScaleKeys
		int32 numScaleRotKeys
		float fMaxError
		string nodeName

		// fTime of first item in each array must be 0

		PosKey[numPosKeys]:
			vec3d pos
			float fTime

		RotKey[numRotKeys]:
			quat16 rot
			float fTime

		ScaleKey[numScaleKeys]:
			vec3d scale
			float fTime

		ScaleRotKey[numScaleRotKeys]:
			quat16 rot
			float fTime
*/

namespace emfx {
const char* parse_xac_cstring(const char* start, const char* end, parsers::error_handler& err) {
	auto const len = parse_xac_any_binary<uint32_t>(&start, end, err);
	return start + len;
}

const char* parse_xac_cstring_nodiscard(std::string& out, const char* start, const char* end, parsers::error_handler& err) {
	auto const len = parse_xac_any_binary<uint32_t>(&start, end, err);
	out.resize(size_t(len), '\0');
	std::memcpy(&out[0], start, size_t(len));
	out[len] = '\0';
	return start + len;
}

const char* parse_xac_metadata_v2(xac_context& context, const char* start, const char* end, parsers::error_handler& err) {
	auto const mh = parse_xac_any_binary<xac_metadata_chunk_header>(&start, end, err);
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
#ifdef XAC_DEBUG
	std::printf("name-of-the-model=%s\n", start + 4);
#endif
	start = parse_xac_cstring(start, end, err);
	return start;
}

const char* parse_xac_material_block_v1(xac_context& context, const char* start, const char* end, parsers::error_handler& err) {
	auto const mh = parse_xac_any_binary<xac_material_block_v1_chunk_header>(&start, end, err);
#ifdef XAC_DEBUG
	std::printf("NumStd=%u,NumFx=%u\n", mh.num_standard_materials, mh.num_fx_materials);
#endif
	context.max_standard_materials = mh.num_standard_materials;
	context.max_fx_materials = mh.num_fx_materials;
	return start;
}

const char* parse_xac_material_v2(xac_context& context, const char* start, const char* end, parsers::error_handler& err) {
	auto const mh = parse_xac_any_binary<xac_material_v2_chunk_header>(&start, end, err);
	std::string name = "";
	start = parse_xac_cstring_nodiscard(name, start, end, err);
#ifdef XAC_DEBUG
	std::printf("Name=%s,NumLayers=%u,sz=%zu\n", name.c_str(), mh.num_layers, sizeof(xac_material_v2_chunk_header));
#endif
	xac_pp_actor_material mat;
	mat.ambient_color = mh.ambient_color;
	mat.diffuse_color = mh.diffuse_color;
	mat.double_sided = mh.double_sided;
	mat.emissive_color = mh.emissive_color;
	mat.ior = mh.ior;
	mat.opacity = mh.opacity;
	mat.shine = mh.shine;
	mat.shine_strength = mh.shine_strength;
	mat.specular_color = mh.specular_color;
	mat.wireframe = mh.wireframe;
	mat.name = name;
	for(uint8_t i = 0; i < mh.num_layers; ++i) {
		auto const mlh = parse_xac_any_binary<xac_material_layer_v2_header>(&start, end, err);
		std::string layer_name = "";
		start = parse_xac_cstring_nodiscard(layer_name, start, end, err);
#ifdef XAC_DEBUG
		std::printf("Name=%s,sz=%zu+%zu,MapType=%u\n", name.c_str(), sizeof(xac_material_layer_v2_header), name.size(), mlh.map_type);
#endif
		xac_pp_actor_material_layer layer;
		layer.amount = mlh.amount;
		layer.u_offset = mlh.u_offset;
		layer.v_offset = mlh.v_offset;
		layer.u_tiling = mlh.u_tiling;
		layer.v_tiling = mlh.v_tiling;
		layer.rotation = mlh.rotation;
		layer.material_id = mlh.material_id;
		layer.map_type = xac_pp_material_map_type(mlh.map_type);
		layer.texture = layer_name;
		mat.layers.push_back(layer);
	}
	context.materials.push_back(mat);
	if(context.materials.size() > context.max_standard_materials) {
		err.accumulated_errors += "File reports maximum standard materials = " + std::to_string(context.max_standard_materials) + ", but the file defines more materials than that on " + err.file_name + "\n";
	}
	// Length does NOT align with the true size of the chunk!
	context.ignore_length = true;
	return start;
}

const char* parse_xac_node_hierachy_v1(xac_context& context, const char* start, const char* end, parsers::error_handler& err) {
	auto const ch = parse_xac_any_binary<xac_node_hierachy_v1_chunk_header>(&start, end, err);
	if(int32_t(ch.num_nodes) <= 0) {
		err.accumulated_errors += "Unexpected number of nodes (on NodeHierachy) on " + err.file_name + "\n";
		return start;
	}
	if(int32_t(ch.num_root_nodes) <= 0) {
		err.accumulated_errors += "Unexpected number of RootNodes (on NodeHierachy) on " + err.file_name + "\n";
		return start;
	}
	for(uint32_t i = 0; i < ch.num_nodes; i++) {
		auto const mh = parse_xac_any_binary<xac_node_hierachy_v1_node_header>(&start, end, err);
		std::string name = "";
		start = parse_xac_cstring_nodiscard(name, start, end, err);
#ifdef XAC_DEBUG
		std::printf(">(Id=%u),Name=%s,IIBC=%x,Imp=%f,ParentId=%i,NumChild=%u,Unk=%x,%x\n", i, name.c_str(), mh.include_in_bounds_calc, mh.importance_factor, mh.parent_id, mh.num_children, mh.unknown[0], mh.unknown[1]);
#endif
		xac_pp_actor_node node;
		node.name = name;
		node.position = mh.position;
		node.rotation = mh.rotation;
		node.scale_rotation = mh.scale_rotation;
		node.scale = mh.scale;
		node.transform = mh.transform;
		node.parent_id = mh.parent_id;
		if(mh.parent_id == -1) {
			context.root_nodes.push_back(node);
		} else {
			if(size_t(mh.parent_id) >= context.nodes.size()) {
				err.accumulated_errors += "Specified actor node " + node.name + " before parent " + err.file_name + "\n";
				return start;
			}
		}
		context.nodes.push_back(node);
	}
	return start;
}

const char* parse_xac_mesh_v1(xac_context& context, const char* start, const char* end, parsers::error_handler& err) {
	auto const cd = parse_xac_any_binary<xac_mesh_v1_chunk_header>(&start, end, err);
	if(cd.node_id >= int32_t(context.nodes.size())) {
		err.accumulated_errors += "Object references OOB node (" + err.file_name + ")\n";
		return start;
	}
#ifdef XAC_DEBUG
	std::printf("N-AttribLayers=%u\n", cd.num_attribute_layers);
	std::printf("IsCollision=%u,Pad=%u,%u,%u\n", cd.is_collision_mesh, cd.unused[0], cd.unused[1], cd.unused[2]);
#endif
	// Parse vertex blocks
	xac_pp_actor_mesh obj{};
	obj.influence_starts.resize(size_t(cd.num_influence_ranges), 0);
	obj.influence_counts.resize(size_t(cd.num_influence_ranges), 0);
	for(uint32_t i = 0; i < cd.num_attribute_layers; ++i) {
		auto const vbh = parse_xac_any_binary<xac_vertex_block_v1_header>(&start, end, err);
#ifdef XAC_DEBUG
		std::printf("T=%u,AttribSize=%u,NumVertices=%u,IsKeep=%u,IsScale=%u\n", vbh.ident, vbh.size, cd.num_vertices, vbh.keep_original, vbh.is_scale_factor);
#endif
		for(uint32_t j = 0; j < cd.num_vertices; ++j) {
			switch(xac_vertex_block_v1_type(vbh.ident)) {
			case xac_vertex_block_v1_type::normal:
				if(vbh.size != sizeof(xac_vector3f)) {
					err.accumulated_errors += "Attribute size doesn't match! [normal] (" + err.file_name + ")\n";
					start += vbh.size;
				} else {
					auto const normal = parse_xac_any_binary<xac_vector3f>(&start, end, err);
					obj.normals.push_back(normal);
				}
				break;
			case xac_vertex_block_v1_type::vertex:
				if(vbh.size != sizeof(xac_vector3f)) {
					err.accumulated_errors += "Attribute size doesn't match! [vertex] (" + err.file_name + ")\n";
					start += vbh.size;
				} else {
					auto const vertex = parse_xac_any_binary<xac_vector3f>(&start, end, err);
					obj.vertices.push_back(vertex);
				}
				break;
			case xac_vertex_block_v1_type::texcoord:
				if(vbh.size != sizeof(xac_vector2f)) {
					err.accumulated_errors += "Attribute size doesn't match! [texcoord] (" + err.file_name + ")\n";
					start += vbh.size;
				} else {
					auto const texcoord = parse_xac_any_binary<xac_vector2f>(&start, end, err);
					obj.texcoords.push_back(texcoord);
				}
				break;
			case xac_vertex_block_v1_type::weight:
				if(vbh.size != sizeof(xac_vector4f)) {
					err.accumulated_errors += "Attribute size doesn't match! [weight] (" + err.file_name + ")\n";
					start += vbh.size;
				} else {
					auto const weight = parse_xac_any_binary<xac_vector4f>(&start, end, err);
					obj.weights.push_back(weight);
				}
				break;
			case xac_vertex_block_v1_type::influence_indices:
				if(vbh.size != sizeof(uint32_t)) {
					err.accumulated_errors += "Attribute size doesn't match! [influenceRange] (" + err.file_name + ")\n";
					start += vbh.size;
				} else {
					auto const influence_index = parse_xac_any_binary<uint32_t>(&start, end, err);
					obj.influence_indices.push_back(influence_index);
				}
				break;
			default:
				err.accumulated_warnings += "Unknown vertex block type " + std::to_string(vbh.ident) + " on " + err.file_name + "\n";
				start += vbh.size;
				break;
			}
		}
	}
	uint32_t vertex_offset = 0;
	for(uint32_t i = 0; i < cd.num_sub_meshes; i++) {
		auto const smh = parse_xac_any_binary<xac_submesh_v1_header>(&start, end, err);
#ifdef XAC_DEBUG
		std::printf("SubObj,NumInd=%u,NumVert=%u,NumBone=%u\n", smh.num_indices, smh.num_vertices, smh.num_bones);
#endif
		if((int32_t(smh.num_indices) % 3) != 0) {
			err.accumulated_warnings += "Indices not divisible by 3 " + std::to_string(smh.num_indices) + " (" + err.file_name + ")\n";
		}
		if(int32_t(smh.num_indices) < 0) {
			err.accumulated_warnings += "Invalid number of indices " + std::to_string(smh.num_indices) + " (" + err.file_name + ")\n";
		}
		if(int32_t(smh.num_bones) < 0) {
			err.accumulated_warnings += "Invalid number of bones " + std::to_string(smh.num_bones) + " (" + err.file_name + ")\n";
		}
		xac_pp_actor_submesh sub{};
		sub.material_id = smh.material_id;
		sub.num_vertices = smh.num_vertices;
		sub.vertex_offset = vertex_offset;
		for(uint32_t j = 0; j < smh.num_indices; j++) {
			auto index = parse_xac_any_binary<uint32_t>(&start, end, err);
			if(index >= smh.num_vertices) {
				err.accumulated_warnings += "submeshes index oob " + std::to_string(int32_t(index)) + " (" + err.file_name + ")\n";
			}
			sub.indices.push_back(index);
		}
		for(uint32_t j = 0; j < smh.num_bones; j++) {
			auto index = parse_xac_any_binary<uint32_t>(&start, end, err);
			sub.bone_ids.push_back(index);
		}
		obj.submeshes.push_back(sub);
		vertex_offset += smh.num_vertices;
	}
	auto& node = context.nodes[cd.node_id];
	if(cd.is_collision_mesh) {
		if(node.collision_mesh >= 0)
			err.accumulated_errors += "More than 1 collision object (" + err.file_name + ")\n";
		//assert(node.meshes.size() < std::numeric_limits<int32_t>::max());
		node.collision_mesh = int32_t(node.meshes.size());
	} else {
		if(node.visual_mesh >= 0)
			err.accumulated_errors += "More than 1 visual object (" + err.file_name + ")\n";
		//assert(node.meshes.size() < std::numeric_limits<int32_t>::max());
		node.visual_mesh = int32_t(node.meshes.size());
	}
	//
	if(vertex_offset != cd.num_vertices) {
		err.accumulated_warnings += "Object total meshes doesn't add up to subojects " + std::to_string(vertex_offset) + " != " + std::to_string(cd.num_vertices) + " (" + err.file_name + ")\n";
	}
	node.meshes.push_back(obj);
	return start;
}

const char* parse_xac_skinning_v3(xac_context& context, const char* start, const char* end, parsers::error_handler& err) {
	auto const sh = parse_xac_any_binary<xac_skinning_v3_chunk_header>(&start, end, err);
#ifdef XAC_DEBUG
	std::printf("NInfluences=%u\n", sh.num_influences);
#endif
	std::vector<xac_skinning_v3_influence_entry> influence_data;
	for(uint32_t i = 0; i < sh.num_influences; i++) {
		auto const influence = parse_xac_any_binary<xac_skinning_v3_influence_entry>(&start, end, err);
		influence_data.push_back(influence);
	}
	if(sh.node_id >= int32_t(context.nodes.size())) {
		err.accumulated_errors += "Referencing a node in bone data which is OOB (" + err.file_name + ")\n";
		return start;
	} else if(sh.node_id < 0) {
		err.accumulated_errors += "Bone with no associated node (" + err.file_name + ")\n";
		return start;
	}
	//
	auto& node = context.nodes[sh.node_id];
	if(sh.is_for_collision_mesh) {
		if(node.collision_mesh >= 0) {
			auto& obj = node.meshes[node.collision_mesh];
			for(const auto& influence : influence_data) {
				xac_pp_bone_influence bone_influence{};
				bone_influence.weight = influence.weight;
				bone_influence.bone_id = influence.bone_id;
				obj.influences.push_back(bone_influence);
			}
			for(uint32_t i = 0; i < uint32_t(obj.influence_starts.size()); i++) {
				auto const range = parse_xac_any_binary<xac_skinning_v3_influence_range>(&start, end, err);
				obj.influence_starts[i] = range.first_influence_index;
				obj.influence_counts[i] = range.num_influences;
			}
		} else {
			err.accumulated_errors += "Collision mesh not defined for \"" + node.name + "\" (" + err.file_name + ")\n";
			return start;
		}
	} else {
		if(node.visual_mesh >= 0) {
			auto& obj = node.meshes[node.visual_mesh];
			for(const auto& influence : influence_data) {
				xac_pp_bone_influence bone_influence{};
				bone_influence.weight = influence.weight;
				bone_influence.bone_id = influence.bone_id;
				obj.influences.push_back(bone_influence);
			}
			for(uint32_t i = 0; i < uint32_t(obj.influence_starts.size()); i++) {
				auto const range = parse_xac_any_binary<xac_skinning_v3_influence_range>(&start, end, err);
				obj.influence_starts[i] = range.first_influence_index;
				obj.influence_counts[i] = range.num_influences;
			}
		} else {
			err.accumulated_errors += "Visual mesh not defined for \"" + node.name + "\" (" + err.file_name + ")\n";
			return start;
		}
	}
	return start;
}

void parse_xac(xac_context& context, const char* start, const char* end, parsers::error_handler& err) {
	const char* file_start = start;
	auto const h = parse_xac_any_binary<xac_header>(&start, end, err);
	if(h.ident[0] != uint8_t('X') || h.ident[1] != uint8_t('A') || h.ident[2] != uint8_t('C') || h.ident[3] != uint8_t(' ')) {
		err.accumulated_errors += "Invalid XAC identifier on " + err.file_name + "\n";
		return;
	}
#ifdef XAC_DEBUG
	std::printf("XacFile-> version %u.%u, totalSize=%u\n", h.major_version, h.minor_version, uint32_t(end - start));
#endif
	while(start < end) {
		auto const ch = parse_xac_any_binary<xac_chunk_header>(&start, end, err);
#ifdef XAC_DEBUG
		std::printf(">>> Id=%u,ChunkVersion=%u(Len=%u)\n", ch.ident, ch.version, ch.len);
#endif
		context.ignore_length = false; // Reset
		const char* expected = start + ch.len;
		switch(xac_chunk_type(ch.ident)) {
		case xac_chunk_type::mesh:
			if(ch.version == 1) {
				start = parse_xac_mesh_v1(context, start, end, err);
			} else {
				err.accumulated_errors += "unsupported version " + err.file_name + "\n";
			}
			break;
		case xac_chunk_type::metadata:
			if(ch.version == 2) {
				start = parse_xac_metadata_v2(context, start, end, err);
			} else {
				err.accumulated_errors += "unsupported version " + err.file_name + "\n";
			}
			break;
		case xac_chunk_type::material_block:
			if(ch.version == 1) {
				start = parse_xac_material_block_v1(context, start, end, err);
			} else {
				err.accumulated_errors += "unsupported version " + err.file_name + "\n";
			}
			break;
		case xac_chunk_type::material_3:
			if(ch.version == 2) {
				start = parse_xac_material_v2(context, start, end, err);
			} else {
				err.accumulated_errors += "unsupported version " + err.file_name + "\n";
			}
			break;
		case xac_chunk_type::node_hierachy:
		{
			if(ch.version == 1) {
				start = parse_xac_node_hierachy_v1(context, start, end, err);
			} else {
				err.accumulated_errors += "unsupported version " + err.file_name + "\n";
			}
			break;
		case xac_chunk_type::skinning:
			if(ch.version == 3) {
				start = parse_xac_skinning_v3(context, start, end, err);
			} else {
				err.accumulated_errors += "Unsupported version (" + err.file_name + ")\n";
			}
			break;
		}
		default:
#ifdef XAC_DEBUG
			std::printf("CT,Unknown-(%i)\n", int16_t(ch.ident));
#endif
			err.accumulated_warnings += "Unknown chunk block type " + std::to_string(int32_t(ch.ident)) + " (size " + std::to_string(ch.len) + " @ offset " + std::to_string(uint32_t(start - file_start)) + ") on " + err.file_name + "\n";
			start += ch.len;
			break;
		}
		if(!context.ignore_length && start != expected) {
			err.accumulated_errors += "Incorrect parsing for chunk ident " + std::to_string(int32_t(ch.ident)) + " (difference from expected " + std::to_string(int32_t(expected - start)) + ") on " + err.file_name + "\n";
			start = expected;
		}
	}
#ifdef XAC_DEBUG
	for(const auto& node : context.nodes) {
		std::printf("Node: %s\n", node.name.c_str());
		std::printf("* Meshes: %zu\n", node.meshes.size());
		for(const auto& o : node.meshes) {
			std::printf("\tMesh\n");
			std::printf("\t* vertices: %zu\n", o.vertices.size());
			std::printf("\t* normals: %zu\n", o.normals.size());
			std::printf("\t* texcoords: %zu\n", o.texcoords.size());
		}
	}
	std::printf("Errors:\n%s\n", err.accumulated_errors.c_str());
	std::printf("Warns:\n%s\n", err.accumulated_warnings.c_str());
#endif
}

void finish(xac_context& context) {
	// Post-proccessing step: Transform ALL vectors using the given matrices
	// for rotation and position, and scale too!
	for(auto& node : context.nodes) {
		for(auto& o : node.meshes) {
			const emfx::xac_vector4f q = node.rotation;
			const emfx::xac_mat4x4 qm{
				2.f * (q.x * q.x + q.y * q.y) - 1.f, //0 * 4 + 0 = 0
				2.f * (q.y * q.z - q.x * q.w), //0 * 4 + 1 = 1
				2.f * (q.y * q.w + q.x * q.z), //0 * 4 + 2 = 2
				0.f, //0 * 4 + 3 = 3
				2.f * (q.y * q.z + q.x * q.w), //1 * 4 + 0 = 4
				2.f * (q.x * q.x + q.z * q.z) - 1.f, //1 * 4 + 1 = 5
				2.f * (q.z * q.w - q.x * q.y), //1 * 4 + 2 = 6
				0.f, //1 * 4 + 3 = 7
				2.f * (q.y * q.w - q.x * q.z), //2 * 4 + 0 = 8
				2.f * (q.z * q.w + q.x * q.y), //2 * 4 + 1 = 9
				2.f * (q.x * q.x + q.w * q.w) - 1.f, //2 * 4 + 2 = 10
				0.f, //2 * 4 + 3 = 11
			};
			const emfx::xac_mat4x4 tm{
				qm.m[0 * 4 + 0] * node.scale.x, //0 * 4 + 0 = 0
				qm.m[0 * 4 + 1] * node.scale.y, //0 * 4 + 1 = 1
				qm.m[0 * 4 + 2] * node.scale.z, //0 * 4 + 2 = 2
				node.position.x, //0 * 4 + 3 = 3
				qm.m[1 * 4 + 0] * node.scale.x, //1 * 4 + 0 = 4
				qm.m[1 * 4 + 1] * node.scale.y, //1 * 4 + 1 = 5
				qm.m[1 * 4 + 2] * node.scale.z, //1 * 4 + 2 = 6
				node.position.y, //1 * 4 + 3 = 7
				qm.m[2 * 4 + 0] * node.scale.x, //2 * 4 + 0 = 8
				qm.m[2 * 4 + 1] * node.scale.y, //2 * 4 + 1 = 9
				qm.m[2 * 4 + 2] * node.scale.z, //2 * 4 + 2 = 10
				node.position.z, //2 * 4 + 3 = 11
			};
			for(uint32_t i = 0; i < o.vertices.size(); i++) {
				emfx::xac_vector3f v = o.vertices[i];
				v.x = tm.m[0 * 4 + 0] * v.x + tm.m[0 * 4 + 1] * v.y + tm.m[0 * 4 + 2] * v.z + tm.m[0 * 4 + 3] * 1.f; // w = 1.f
				v.y = tm.m[1 * 4 + 0] * v.x + tm.m[1 * 4 + 1] * v.y + tm.m[1 * 4 + 2] * v.z + tm.m[1 * 4 + 3] * 1.f;
				v.z = tm.m[2 * 4 + 0] * v.x + tm.m[2 * 4 + 1] * v.y + tm.m[2 * 4 + 2] * v.z + tm.m[2 * 4 + 3] * 1.f;
				//v.w = tm.m[3 * 4 + 0] * v.x + tm.m[3 * 4 + 1] * v.y + tm.m[3 * 4 + 2] * v.z + tm.m[3 * 4 + 3] * 1.f;
				o.vertices[i] = v;
			}
		}
	}
}
}
