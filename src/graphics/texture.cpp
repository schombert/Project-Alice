#include "texture.hpp"
#include "system_state.hpp"
#include "simple_fs.hpp"

#define STB_IMAGE_IMPLEMENTATION 1
#define STBI_NO_STDIO 1
#define STBI_NO_LINEAR 1
#define STBI_NO_JPEG 1
//#define STBI_NO_PNG 1
//#define STBI_NO_BMP 1
#define STBI_NO_PSD 1
//#define STBI_NO_TGA 1
#define STBI_NO_GIF 1
#define STBI_NO_HDR 1
#define STBI_NO_PIC 1
#define STBI_NO_PNM 1
#define STBI_NO_THREAD_LOCALS 1

#include "stb_image.h"

namespace ogl {

// DDS loader taken from SOIL2

#define ALICE_DDSD_CAPS 0x00000001
#define ALICE_DDSD_HEIGHT 0x00000002
#define ALICE_DDSD_WIDTH 0x00000004
#define ALICE_DDSD_PITCH 0x00000008
#define ALICE_DDSD_PIXELFORMAT 0x00001000
#define ALICE_DDSD_MIPMAPCOUNT 0x00020000
#define ALICE_DDSD_LINEARSIZE 0x00080000
#define ALICE_DDSD_DEPTH 0x00800000

/*	DirectDraw Pixel Format	*/
#define ALICE_DDPF_ALPHAPIXELS 0x00000001
#define ALICE_DDPF_FOURCC 0x00000004
#define ALICE_DDPF_RGB 0x00000040

/*	The dwCaps1 member of the ALICE_DDSCAPS2 structure can be
	set to one or more of the following values.	*/
#define ALICE_DDSCAPS_COMPLEX 0x00000008
#define ALICE_DDSCAPS_TEXTURE 0x00001000
#define ALICE_DDSCAPS_MIPMAP 0x00400000

	/*	The dwCaps2 member of the ALICE_DDSCAPS2 structure can be
		set to one or more of the following values.		*/
#define ALICE_DDSCAPS2_CUBEMAP 0x00000200
#define ALICE_DDSCAPS2_CUBEMAP_POSITIVEX 0x00000400
#define ALICE_DDSCAPS2_CUBEMAP_NEGATIVEX 0x00000800
#define ALICE_DDSCAPS2_CUBEMAP_POSITIVEY 0x00001000
#define ALICE_DDSCAPS2_CUBEMAP_NEGATIVEY 0x00002000
#define ALICE_DDSCAPS2_CUBEMAP_POSITIVEZ 0x00004000
#define ALICE_DDSCAPS2_CUBEMAP_NEGATIVEZ 0x00008000
#define ALICE_DDSCAPS2_VOLUME 0x00200000

#define SOIL_GL_SRGB 0x8C40
#define SOIL_GL_SRGB_ALPHA 0x8C42
#define SOIL_RGB_S3TC_DXT1 0x83F0
#define SOIL_RGBA_S3TC_DXT1 0x83F1
#define SOIL_RGBA_S3TC_DXT3 0x83F2
#define SOIL_RGBA_S3TC_DXT5 0x83F3

#define SOIL_TEXTURE_WRAP_R 0x8072
#define SOIL_CLAMP_TO_EDGE 0x812F
#define SOIL_REFLECTION_MAP 0x8512

typedef struct {
	unsigned int dwMagic;
	unsigned int dwSize;
	unsigned int dwFlags;
	unsigned int dwHeight;
	unsigned int dwWidth;
	unsigned int dwPitchOrLinearSize;
	unsigned int dwDepth;
	unsigned int dwMipMapCount;
	unsigned int dwReserved1[11];

	/*  DDPIXELFORMAT	*/
	struct {
		unsigned int dwSize;
		unsigned int dwFlags;
		unsigned int dwFourCC;
		unsigned int dwRGBBitCount;
		unsigned int dwRBitMask;
		unsigned int dwGBitMask;
		unsigned int dwBBitMask;
		unsigned int dwAlphaBitMask;
	} sPixelFormat;

	/*  DDCAPS2	*/
	struct {
		unsigned int dwCaps1;
		unsigned int dwCaps2;
		unsigned int dwDDSX;
		unsigned int dwReserved;
	} sCaps;
	unsigned int dwReserved2;
} DDS_header;

GLuint SOIL_direct_load_DDS_from_memory(unsigned char const* const buffer, uint32_t buffer_length, uint32_t& width, uint32_t& height, int soil_flags) {
	/*	file reading variables	*/
	uint32_t block_size = 16;
	uint32_t flag;
	if(buffer_length < sizeof(DDS_header)) {
		return 0;
	}

	/*	try reading in the header */
	DDS_header header;
	std::memcpy((void*)(&header), (void const*)buffer, sizeof(DDS_header));
	uint32_t buffer_index = sizeof(DDS_header);

	/*	validate the header (warning, "goto"'s ahead, shield your eyes!!)	*/
	if(header.dwMagic != (('D' << 0) | ('D' << 8) | ('S' << 16) | (' ' << 24))) {
		return 0;
	}
	if(header.dwSize != 124) {
		return 0;
	}
	/*	I need all of these	*/
	flag = ALICE_DDSD_CAPS | ALICE_DDSD_HEIGHT | ALICE_DDSD_WIDTH | ALICE_DDSD_PIXELFORMAT;
	if((header.dwFlags & flag) != flag) {
		return 0;
	}
	/*	According to the MSDN spec, the dwFlags should contain
		ALICE_DDSD_LINEARSIZE if it's compressed, or ALICE_DDSD_PITCH if
		uncompressed.  Some DDS writers do not conform to the
		spec, so I need to make my reader more tolerant	*/
	if(header.sPixelFormat.dwSize != 32) {
		return 0;
	}
	/*	I need one of these	*/
	if((header.sPixelFormat.dwFlags & (ALICE_DDPF_FOURCC | ALICE_DDPF_RGB | ALICE_DDPF_ALPHAPIXELS)) == 0) {
		return 0;
	}
	/*	make sure it is a type we can upload	*/
	if((header.sPixelFormat.dwFlags & ALICE_DDPF_FOURCC) &&
		!((header.sPixelFormat.dwFourCC == (('D' << 0) | ('X' << 8) | ('T' << 16) | ('1' << 24))) ||
			(header.sPixelFormat.dwFourCC == (('D' << 0) | ('X' << 8) | ('T' << 16) | ('3' << 24))) ||
			(header.sPixelFormat.dwFourCC == (('D' << 0) | ('X' << 8) | ('T' << 16) | ('5' << 24))))) {
		return 0;
	}
	if((header.sCaps.dwCaps1 & ALICE_DDSCAPS_TEXTURE) == 0) {
		return 0;
	}
	if((header.sCaps.dwCaps2 & ALICE_DDSCAPS2_CUBEMAP) != 0) {
		return 0;
	}
	/*	OK, validated the header, let's load the image data	*/
	width = header.dwWidth;
	height = header.dwHeight;
	bool uncompressed = (header.sPixelFormat.dwFlags & ALICE_DDPF_FOURCC) == 0;
	GLint s3tc_format = 0; //How we want to give it to shaders
	GLint s3tc_format_layout = 0; //How's it laid on memory
	GLint s3tc_type = GL_UNSIGNED_BYTE;
	uint32_t dds_main_size = 0;
	if(uncompressed) {
		s3tc_format = GL_RGB;
		s3tc_format_layout = GL_RGB;
		block_size = 3;
		if(header.sPixelFormat.dwFlags & ALICE_DDPF_ALPHAPIXELS) {
			s3tc_format = GL_RGBA;
			s3tc_format_layout = GL_RGBA;
			block_size = 4;
			if(header.sPixelFormat.dwRGBBitCount == 16) {
				//s3tc_format_layout = GL_RGBA;
				//s3tc_type = GL_UNSIGNED_BYTE;
				block_size = 2;
			}
		}
		dds_main_size = width * height * block_size;
	} else {
		/*	can we even handle direct uploading to OpenGL DXT compressed images?	*/
		//
		// TODO: properly restore this check
		//

		// if(query_DXT_capability() != SOIL_CAPABILITY_PRESENT) {
		//	return 0;
		// }
		/*	well, we know it is DXT1/3/5, because we checked above	*/
		switch((header.sPixelFormat.dwFourCC >> 24) - '0') {
		case 1:
			s3tc_format = SOIL_RGBA_S3TC_DXT1;
			block_size = 8;
			break;
		case 3:
			s3tc_format = SOIL_RGBA_S3TC_DXT3;
			block_size = 16;
			break;
		case 5:
			s3tc_format = SOIL_RGBA_S3TC_DXT5;
			block_size = 16;
			break;
		}
		dds_main_size = ((width + 3) >> 2) * ((height + 3) >> 2) * block_size;
	}
	uint32_t dds_full_size = dds_main_size;
	uint32_t mipmaps = 0;
	if((header.sCaps.dwCaps1 & ALICE_DDSCAPS_MIPMAP) != 0 && (header.dwMipMapCount > 1)) {
		mipmaps = header.dwMipMapCount - 1;
		for(uint32_t i = 1; i <= mipmaps; ++i) {
			uint32_t w = std::max<uint32_t>(width >> i, 1);
			uint32_t h = std::max<uint32_t>(height >> i, 1);
			if(uncompressed) {
				/*	uncompressed DDS, simple MIPmap size calculation	*/
				dds_full_size += w * h * block_size;
			} else {
				/*	compressed DDS, MIPmap size calculation is block based	*/
				dds_full_size += ((w + 3) / 4) * ((h + 3) / 4) * block_size;
			}
		}
	}
	/*	do this for each face of the cubemap!	*/
	if(buffer_index + dds_full_size <= uint32_t(buffer_length)) {
		/*	got the image data RAM, create or use an existing OpenGL texture handle	*/
		GLuint texid = 0;
		glGenTextures(1, &texid);
		/*  bind an OpenGL texture ID	*/
		glBindTexture(GL_TEXTURE_2D, texid);
		if(!texid)
			return 0;
		/*	did I have MIPmaps?	*/
		if(mipmaps > 0) {
			/*	instruct OpenGL to use the MIPmaps	*/
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		} else {
			/*	instruct OpenGL _NOT_ to use the MIPmaps	*/
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		/*	does the user want clamping, or wrapping? */
		if((soil_flags & SOIL_FLAG_TEXTURE_REPEATS) != 0) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, SOIL_TEXTURE_WRAP_R, GL_REPEAT);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, SOIL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
		/*	upload the main chunk	*/
		if(uncompressed) {
			/*	and remember, DXT uncompressed uses BGR(A), so swap to (A)BGR for ALL MIPmap levels	*/
			std::unique_ptr<uint8_t[]> dds_dest_data = std::unique_ptr<uint8_t[]>(new uint8_t[dds_full_size]);
			switch(block_size) {
			case 4:
			{
				for(uint32_t i = 0; i < dds_full_size; i += block_size) {
					uint32_t data = *(uint32_t const*)(buffer + buffer_index + i);
					uint32_t r = (data & header.sPixelFormat.dwRBitMask) >> std::countr_zero(header.sPixelFormat.dwRBitMask);
					uint32_t g = (data & header.sPixelFormat.dwGBitMask) >> std::countr_zero(header.sPixelFormat.dwGBitMask);
					uint32_t b = (data & header.sPixelFormat.dwBBitMask) >> std::countr_zero(header.sPixelFormat.dwBBitMask);
					uint32_t a = (data & header.sPixelFormat.dwAlphaBitMask) >> std::countr_zero(header.sPixelFormat.dwAlphaBitMask);
					dds_dest_data[i + 0] = static_cast<uint8_t>(r);
					dds_dest_data[i + 1] = static_cast<uint8_t>(g);
					dds_dest_data[i + 2] = static_cast<uint8_t>(b);
					dds_dest_data[i + 3] = static_cast<uint8_t>(a);
				}
				break;
			}
			case 2:
			{
				dds_dest_data.reset();
				dds_dest_data = std::unique_ptr<uint8_t[]>(new uint8_t[dds_full_size * 2]);
				uint16_t mr1 = uint16_t(header.sPixelFormat.dwRBitMask >> std::countr_zero(header.sPixelFormat.dwRBitMask));
				float mr2 = mr1 == 0 ? 0.f : 255.f / float(mr1);
				uint16_t mg1 = uint16_t(header.sPixelFormat.dwGBitMask >> std::countr_zero(header.sPixelFormat.dwGBitMask));
				float mg2 = mg1 == 0 ? 0.f : 255.f / float(mg1);
				uint16_t mb1 = uint16_t(header.sPixelFormat.dwBBitMask >> std::countr_zero(header.sPixelFormat.dwBBitMask));
				float mb2 = mb1 == 0 ? 0.f : 255.f / float(mb1);
				uint16_t ma1 = uint16_t(header.sPixelFormat.dwAlphaBitMask >> std::countr_zero(header.sPixelFormat.dwAlphaBitMask));
				float ma2 = ma1 == 0 ? 0.f : 255.f / float(ma1);
				for(uint32_t i = 0; i < dds_full_size; i += block_size) {
					uint16_t data = *(uint16_t const*)(buffer + buffer_index + i);
					uint16_t r = (data & header.sPixelFormat.dwRBitMask) >> std::countr_zero(header.sPixelFormat.dwRBitMask);
					uint16_t g = (data & header.sPixelFormat.dwGBitMask) >> std::countr_zero(header.sPixelFormat.dwGBitMask);
					uint16_t b = (data & header.sPixelFormat.dwBBitMask) >> std::countr_zero(header.sPixelFormat.dwBBitMask);
					uint16_t a = (data & header.sPixelFormat.dwAlphaBitMask) >> std::countr_zero(header.sPixelFormat.dwAlphaBitMask);
					dds_dest_data[i * 2 + 0] = uint8_t(float(r) * mr2);
					dds_dest_data[i * 2 + 1] = uint8_t(float(g) * mg2);
					dds_dest_data[i * 2 + 2] = uint8_t(float(b) * mb2);
					dds_dest_data[i * 2 + 3] = uint8_t(float(a) * ma2);
				}
				break;
			}
			default:
			{
				std::memcpy(dds_dest_data.get(), buffer + buffer_index, dds_full_size);
				for(uint32_t i = 0; i < dds_full_size; i += block_size) {
					uint8_t temp = dds_dest_data[i];
					dds_dest_data[i] = dds_dest_data[i + 2];
					dds_dest_data[i + 2] = temp;
				}
				break;
			}
			}
			glTexImage2D(GL_TEXTURE_2D, 0, s3tc_format, width, height, 0, s3tc_format_layout, s3tc_type, dds_dest_data.get());
			uint32_t buffer_offset = dds_main_size * (block_size == 2 ? 2 : 1);
			/*	upload the mipmaps, if we have them	*/
			for(uint32_t i = 1; i <= mipmaps; ++i) {
				uint32_t w = std::max<uint32_t>(width >> i, 1);
				uint32_t h = std::max<uint32_t>(height >> i, 1);
				/*	upload this mipmap	*/
				uint32_t mip_size = w * h * block_size;
				switch(block_size) {
				case 2:
					mip_size = w * h * 4;
					break;
				default:
					break;
				}
				glTexImage2D(GL_TEXTURE_2D, i, s3tc_format, w, h, 0, s3tc_format_layout, s3tc_type, dds_dest_data.get() + buffer_offset);
				/*	and move to the next mipmap	*/
				buffer_offset += mip_size;
			}
		} else {
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, s3tc_format, width, height, 0, dds_main_size, buffer + buffer_index);
			buffer_index += dds_main_size;
			/*	upload the mipmaps, if we have them	*/
			for(uint32_t i = 1; i <= mipmaps; ++i) {
				uint32_t w = std::max<uint32_t>(width >> i, 1);
				uint32_t h = std::max<uint32_t>(height >> i, 1);
				/*	upload this mipmap	*/
				uint32_t mip_size = ((w + 3) / 4) * ((h + 3) / 4) * block_size;
				glCompressedTexImage2D(GL_TEXTURE_2D, i, s3tc_format, w, h, 0, mip_size, buffer + buffer_index);
				/*	and move to the next mipmap	*/
				buffer_index += mip_size;
			}
		}
		return texid;
	}
	return 0;
}

texture::~texture() {
	STBI_FREE(data);
	data = nullptr;
}

texture::texture(texture&& other) noexcept {
	channels = other.channels;
	loaded = other.loaded;
	size_x = other.size_x;
	size_y = other.size_y;
	data = other.data;
	texture_handle = other.texture_handle;

	other.data = nullptr;
}
texture& texture::operator=(texture&& other) noexcept {
	channels = other.channels;
	loaded = other.loaded;
	size_x = other.size_x;
	size_y = other.size_y;
	data = other.data;
	texture_handle = other.texture_handle;

	other.data = nullptr;
	return *this;
}

GLuint texture::get_texture_handle() const {
	assert(loaded);
	return texture_handle;
}

GLuint load_file_and_return_handle(native_string const& native_name, simple_fs::file_system const& fs, texture& asset_texture, bool keep_data) {
	auto name_length = native_name.length();

	auto root = get_root(fs);
	if(name_length > 4) { // try loading as a dds
		auto dds_name = native_name;
		if(auto pos = dds_name.find_last_of('.'); pos != native_string::npos) {
			dds_name[pos + 1] = NATIVE('d');
			dds_name[pos + 2] = NATIVE('d');
			dds_name[pos + 3] = NATIVE('s');
			dds_name.resize(pos + 4);
		}
		auto file = open_file(root, dds_name);
		if(file) {
			auto content = simple_fs::view_contents(*file);

			uint32_t w = 0;
			uint32_t h = 0;
			asset_texture.texture_handle = SOIL_direct_load_DDS_from_memory(reinterpret_cast<uint8_t const*>(content.data), content.file_size, w, h, 0);

			if(asset_texture.texture_handle) {
				asset_texture.channels = 4;
				asset_texture.size_x = int32_t(w);
				asset_texture.size_y = int32_t(h);
				asset_texture.loaded = true;

				if(keep_data) {
					asset_texture.data = static_cast<uint8_t*>(STBI_MALLOC(4 * w * h));
					glGetTextureImage(asset_texture.texture_handle, 0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<int32_t>(4 * w * h),
						asset_texture.data);
				}
				return asset_texture.texture_handle;
			}
		}
	}

	auto file = open_file(root, native_name);
	if(!file && name_length > 4) {
		auto png_name = native_name;
		if(auto pos = png_name.find_last_of('.'); pos != native_string::npos) {
			png_name[pos + 1] = NATIVE('p');
			png_name[pos + 2] = NATIVE('n');
			png_name[pos + 3] = NATIVE('g');
			png_name.resize(pos + 4);
		}
		file = open_file(root, png_name);
	}
	if(file) {
		auto content = simple_fs::view_contents(*file);

		int32_t file_channels = 4;

		asset_texture.data = stbi_load_from_memory(reinterpret_cast<uint8_t const*>(content.data), int32_t(content.file_size),
			&(asset_texture.size_x), &(asset_texture.size_y), &file_channels, 4);

		asset_texture.channels = 4;
		asset_texture.loaded = true;

		glGenTextures(1, &asset_texture.texture_handle);
		if(asset_texture.texture_handle) {
			glBindTexture(GL_TEXTURE_2D, asset_texture.texture_handle);

			glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, asset_texture.size_x, asset_texture.size_y);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, asset_texture.size_x, asset_texture.size_y, GL_RGBA, GL_UNSIGNED_BYTE,
					asset_texture.data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		if(!keep_data) {
			STBI_FREE(asset_texture.data);
			asset_texture.data = nullptr;
		}

		return asset_texture.texture_handle;
	}
	asset_texture.loaded = true; // because we tried to load it (and failed) and trying again will be wasteful
	return 0;
}

native_string flag_type_to_name(sys::state& state, culture::flag_type type) {
	switch(type) {
	case culture::flag_type::count:
	case culture::flag_type::default_flag:
		return NATIVE("");
	case culture::flag_type::communist:
		return NATIVE("_communist");
	case culture::flag_type::fascist:
		return NATIVE("_fascist");
	case culture::flag_type::monarchy:
		return NATIVE("_monarchy");
	case culture::flag_type::republic:
		return NATIVE("_republic");
		// Non-vanilla
	case culture::flag_type::theocracy:
		return NATIVE("_theocracy");
		break;
	case culture::flag_type::special:
		return NATIVE("_special");
		break;
	case culture::flag_type::spare:
		return NATIVE("_spare");
		break;
	case culture::flag_type::populist:
		return NATIVE("_populist");
		break;
	case culture::flag_type::realm:
		return NATIVE("_realm");
		break;
	case culture::flag_type::other:
		return NATIVE("_other");
		break;
	case culture::flag_type::monarchy2:
		return NATIVE("_monarchy2");
		break;
	case culture::flag_type::monarchy3:
		return NATIVE("_monarchy3");
		break;
	case culture::flag_type::republic2:
		return NATIVE("_republic2");
		break;
	case culture::flag_type::republic3:
		return NATIVE("_republic3");
		break;
	case culture::flag_type::communist2:
		return NATIVE("_communist2");
		break;
	case culture::flag_type::communist3:
		return NATIVE("_communist3");
		break;
	case culture::flag_type::fascist2:
		return NATIVE("_fascist2");
		break;
	case culture::flag_type::fascist3:
		return NATIVE("_fascist3");
		break;
	case culture::flag_type::theocracy2:
		return NATIVE("_theocracy2");
		break;
	case culture::flag_type::theocracy3:
		return NATIVE("_theocracy3");
		break;
	case culture::flag_type::cosmetic_1:
		return NATIVE("_cosmetic_1");
		break;
	case culture::flag_type::cosmetic_2:
		return NATIVE("_cosmetic_2");
		break;
	case culture::flag_type::colonial:
		return NATIVE("_colonial");
		break;
	case culture::flag_type::nationalist:
		return NATIVE("_nationalist");
		break;
	case culture::flag_type::sectarian:
		return NATIVE("_sectarian");
		break;
	case culture::flag_type::socialist:
		return NATIVE("_socialist");
		break;
	case culture::flag_type::dominion:
		return NATIVE("_dominion");
		break;
	case culture::flag_type::agrarism:
		return NATIVE("_agrarism");
		break;
	case culture::flag_type::national_syndicalist:
		return NATIVE("_national_syndicalist");
		break;
	case culture::flag_type::theocratic:
		return NATIVE("_theocratic");
		break;
	case culture::flag_type::slot1:
		return NATIVE("_slot1");
		break;
	case culture::flag_type::slot2:
		return NATIVE("_slot2");
		break;
	case culture::flag_type::slot3:
		return NATIVE("_slot3");
		break;
	case culture::flag_type::slot4:
		return NATIVE("_slot4");
		break;
	case culture::flag_type::anarcho_liberal:
		return NATIVE("_anarcho_liberal");
		break;
	case culture::flag_type::green:
		return NATIVE("_green");
		break;
	case culture::flag_type::traditionalist:
		return NATIVE("_traditionalist");
		break;
	case culture::flag_type::ultranationalist:
		return NATIVE("_ultranationalist");
	}
	return NATIVE("");
}

GLuint get_flag_handle(sys::state& state, dcon::national_identity_id nat_id, culture::flag_type type) {
	auto masq_nat_id = state.world.nation_get_masquerade_identity(state.world.national_identity_get_nation_from_identity_holder(nat_id));
	if(!masq_nat_id) {
		masq_nat_id = nat_id;
	}

	auto const offset = culture::get_remapped_flag_type(state, type);
	dcon::texture_id id = dcon::texture_id{ dcon::texture_id::value_base_t(state.ui_defs.textures.size() + (1 + masq_nat_id.index()) * state.flag_types.size() + offset) };
	if(state.open_gl.asset_textures[id].loaded) {
		return state.open_gl.asset_textures[id].texture_handle;
	} else { // load from file
		native_string file_str;
		file_str += NATIVE("gfx");
		file_str += NATIVE_DIR_SEPARATOR;
		file_str += NATIVE("flags");
		file_str += NATIVE_DIR_SEPARATOR;
		file_str += simple_fs::win1250_to_native(nations::int_to_tag(state.world.national_identity_get_identifying_int(masq_nat_id)));
		native_string default_file_str = file_str;
		file_str += flag_type_to_name(state, type);
		GLuint p_tex = load_file_and_return_handle(file_str + NATIVE(".png"), state.common_fs, state.open_gl.asset_textures[id], false);
		if(!p_tex) {
			p_tex = load_file_and_return_handle(file_str + NATIVE(".tga"), state.common_fs, state.open_gl.asset_textures[id], false);
			if(!p_tex) {
				p_tex = load_file_and_return_handle(default_file_str + NATIVE(".png"), state.common_fs, state.open_gl.asset_textures[id], false);
				if(!p_tex) {
					return load_file_and_return_handle(default_file_str + NATIVE(".tga"), state.common_fs, state.open_gl.asset_textures[id], false);
				}
			}
		}
		return p_tex;
	}
}

GLuint get_texture_handle(sys::state& state, dcon::texture_id id, bool keep_data) {
	if(state.open_gl.asset_textures[id].loaded) {
		return state.open_gl.asset_textures[id].texture_handle;
	} else { // load from file
		auto fname = state.ui_defs.textures[id];
		auto fname_view = state.to_string_view(fname);
		auto native_name = simple_fs::win1250_to_native(fname_view);

		return load_file_and_return_handle(native_name, state.common_fs, state.open_gl.asset_textures[id], keep_data);
	} // end else (not already loaded)
}

data_texture::data_texture(int32_t sz, int32_t ch) {
	size = sz;
	channels = ch;
	data = new uint8_t[size * channels];

	glGenTextures(1, &texture_handle);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	if(channels == 3) {
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, size, 1);
	} else if(channels == 4) {
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, size, 1);
	} else if(channels == 2) {
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG8, size, 1);
	} else {
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, size, 1);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}

data_texture::~data_texture() {
	delete[] data;
	data = nullptr;
}

uint32_t data_texture::handle() {
	if(data && data_updated) {
		glBindTexture(GL_TEXTURE_2D, texture_handle);
		if(channels == 3) {
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
		} else if(channels == 4) {
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
		} else if(channels == 2) {
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, 1, GL_RG, GL_UNSIGNED_BYTE, data);
		} else {
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, 1, GL_RED, GL_UNSIGNED_BYTE, data);
		}
		data_updated = false;
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	return texture_handle;
}

data_texture::data_texture(data_texture&& other) noexcept {
	data = other.data;
	size = other.size;
	channels = other.channels;
	texture_handle = other.texture_handle;

	other.data = nullptr;
}

data_texture& data_texture::operator=(data_texture&& other) noexcept {
	data = other.data;
	size = other.size;
	channels = other.channels;
	texture_handle = other.texture_handle;

	other.data = nullptr;

	return *this;
}

font_texture_result make_font_texture(simple_fs::file& f) {
	auto content = simple_fs::view_contents(f);

	int32_t file_channels = 4;
	int32_t size_x = 0;
	int32_t size_y = 0;
	uint8_t* data = stbi_load_from_memory(reinterpret_cast<uint8_t const*>(content.data), int32_t(content.file_size), &(size_x), &(size_y), &file_channels, 4);
	uint32_t ftexid = 0;

	glGenTextures(1, &ftexid);
	if(data && ftexid) {
		glBindTexture(GL_TEXTURE_2D, ftexid);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, size_x, size_y);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size_x, size_y, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	STBI_FREE(data);

	return font_texture_result{ ftexid, uint32_t(size_x) };
}

} // namespace ogl
