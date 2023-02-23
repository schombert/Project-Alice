#include "texture.hpp"
#include "system_state.hpp"
#include "simple_fs.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO

#include "stb_image.h"

namespace ogl {

// DDS loader taken from SOIL2

#define DDSD_CAPS	0x00000001
#define DDSD_HEIGHT	0x00000002
#define DDSD_WIDTH	0x00000004
#define DDSD_PITCH	0x00000008
#define DDSD_PIXELFORMAT	0x00001000
#define DDSD_MIPMAPCOUNT	0x00020000
#define DDSD_LINEARSIZE	0x00080000
#define DDSD_DEPTH	0x00800000

/*	DirectDraw Pixel Format	*/
#define DDPF_ALPHAPIXELS	0x00000001
#define DDPF_FOURCC	0x00000004
#define DDPF_RGB	0x00000040

/*	The dwCaps1 member of the DDSCAPS2 structure can be
	set to one or more of the following values.	*/
#define DDSCAPS_COMPLEX	0x00000008
#define DDSCAPS_TEXTURE	0x00001000
#define DDSCAPS_MIPMAP	0x00400000

	/*	The dwCaps2 member of the DDSCAPS2 structure can be
		set to one or more of the following values.		*/
#define DDSCAPS2_CUBEMAP	0x00000200
#define DDSCAPS2_CUBEMAP_POSITIVEX	0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX	0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY	0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY	0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ	0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ	0x00008000
#define DDSCAPS2_VOLUME	0x00200000

#define SOIL_GL_SRGB			0x8C40
#define SOIL_GL_SRGB_ALPHA		0x8C42
#define SOIL_RGB_S3TC_DXT1		0x83F0
#define SOIL_RGBA_S3TC_DXT1		0x83F1
#define SOIL_RGBA_S3TC_DXT3		0x83F2
#define SOIL_RGBA_S3TC_DXT5		0x83F3

#define SOIL_TEXTURE_WRAP_R					0x8072
#define SOIL_CLAMP_TO_EDGE					0x812F
#define SOIL_REFLECTION_MAP					0x8512

typedef struct {
	unsigned int    dwMagic;
	unsigned int    dwSize;
	unsigned int    dwFlags;
	unsigned int    dwHeight;
	unsigned int    dwWidth;
	unsigned int    dwPitchOrLinearSize;
	unsigned int    dwDepth;
	unsigned int    dwMipMapCount;
	unsigned int    dwReserved1[11];

	/*  DDPIXELFORMAT	*/
	struct {
		unsigned int    dwSize;
		unsigned int    dwFlags;
		unsigned int    dwFourCC;
		unsigned int    dwRGBBitCount;
		unsigned int    dwRBitMask;
		unsigned int    dwGBitMask;
		unsigned int    dwBBitMask;
		unsigned int    dwAlphaBitMask;
	}
	sPixelFormat;

	/*  DDCAPS2	*/
	struct {
		unsigned int    dwCaps1;
		unsigned int    dwCaps2;
		unsigned int    dwDDSX;
		unsigned int    dwReserved;
	}
	sCaps;
	unsigned int    dwReserved2;
}
DDS_header;

unsigned int SOIL_direct_load_DDS_from_memory(
		const unsigned char* const buffer,
		unsigned int buffer_length,
		unsigned int& width,
		unsigned int& height,
		int flags) {
	/*	variables	*/
	DDS_header header;
	unsigned int buffer_index = 0;
	unsigned int tex_ID = 0;
	/*	file reading variables	*/
	unsigned int S3TC_type = 0;
	unsigned char* DDS_data;
	unsigned int DDS_main_size;
	unsigned int DDS_full_size;
	int mipmaps, cubemap, uncompressed, block_size = 16;
	unsigned int flag;
	unsigned int cf_target, ogl_target_start, ogl_target_end;
	unsigned int opengl_texture_type;
	int i;

	if(buffer_length < sizeof(DDS_header)) {
		return 0;
	}
	/*	try reading in the header	*/
	memcpy((void*)(&header), (const void*)buffer, sizeof(DDS_header));
	buffer_index = sizeof(DDS_header);

	/*	validate the header (warning, "goto"'s ahead, shield your eyes!!)	*/
	flag = ('D' << 0) | ('D' << 8) | ('S' << 16) | (' ' << 24);
	if(header.dwMagic != flag) {
		goto quick_exit;
	}
	if(header.dwSize != 124) {
		goto quick_exit;
	}
	/*	I need all of these	*/
	flag = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	if((header.dwFlags & flag) != flag) {
		goto quick_exit;
	}
	/*	According to the MSDN spec, the dwFlags should contain
		DDSD_LINEARSIZE if it's compressed, or DDSD_PITCH if
		uncompressed.  Some DDS writers do not conform to the
		spec, so I need to make my reader more tolerant	*/
		/*	I need one of these	*/
	flag = DDPF_FOURCC | DDPF_RGB;
	if((header.sPixelFormat.dwFlags & flag) == 0) {
		goto quick_exit;
	}
	if(header.sPixelFormat.dwSize != 32) {
		goto quick_exit;
	}
	if((header.sCaps.dwCaps1 & DDSCAPS_TEXTURE) == 0) {
		goto quick_exit;
	}
	/*	make sure it is a type we can upload	*/
	if((header.sPixelFormat.dwFlags & DDPF_FOURCC) &&
		!(
			(header.sPixelFormat.dwFourCC == (('D' << 0) | ('X' << 8) | ('T' << 16) | ('1' << 24))) ||
			(header.sPixelFormat.dwFourCC == (('D' << 0) | ('X' << 8) | ('T' << 16) | ('3' << 24))) ||
			(header.sPixelFormat.dwFourCC == (('D' << 0) | ('X' << 8) | ('T' << 16) | ('5' << 24)))
			)) {
		goto quick_exit;
	}
	/*	OK, validated the header, let's load the image data	*/
	width = header.dwWidth;
	height = header.dwHeight;
	uncompressed = 1 - (header.sPixelFormat.dwFlags & DDPF_FOURCC) / DDPF_FOURCC;
	cubemap = (header.sCaps.dwCaps2 & DDSCAPS2_CUBEMAP) / DDSCAPS2_CUBEMAP;
	if(uncompressed) {
		S3TC_type = GL_RGB;
		block_size = 3;
		if(header.sPixelFormat.dwFlags & DDPF_ALPHAPIXELS) {
			S3TC_type = GL_RGBA;
			block_size = 4;
		}
		DDS_main_size = width * height * block_size;
	} else {
		/*	can we even handle direct uploading to OpenGL DXT compressed images?	*/
		//
		// TODO: properly restore this check
		//

		//if(query_DXT_capability() != SOIL_CAPABILITY_PRESENT) {
		//	return 0;
		//}
		/*	well, we know it is DXT1/3/5, because we checked above	*/
		switch((header.sPixelFormat.dwFourCC >> 24) - '0') {
			case 1:
				S3TC_type = SOIL_RGBA_S3TC_DXT1;
				block_size = 8;
				break;
			case 3:
				S3TC_type = SOIL_RGBA_S3TC_DXT3;
				block_size = 16;
				break;
			case 5:
				S3TC_type = SOIL_RGBA_S3TC_DXT5;
				block_size = 16;
				break;
		}
		DDS_main_size = ((width + 3) >> 2) * ((height + 3) >> 2) * block_size;
	}
	if(cubemap) {
		return 0;
	} else {
		ogl_target_start = GL_TEXTURE_2D;
		ogl_target_end = GL_TEXTURE_2D;
		opengl_texture_type = GL_TEXTURE_2D;
	}
	if((header.sCaps.dwCaps1 & DDSCAPS_MIPMAP) != 0 && (header.dwMipMapCount > 1)) {
		mipmaps = header.dwMipMapCount - 1;
		DDS_full_size = DDS_main_size;
		for(i = 1; i <= mipmaps; ++i) {
			int w, h;
			w = width >> i;
			h = height >> i;
			if(w < 1) {
				w = 1;
			}
			if(h < 1) {
				h = 1;
			}
			if(uncompressed) {
				/*	uncompressed DDS, simple MIPmap size calculation	*/
				DDS_full_size += w * h * block_size;
			} else {
				/*	compressed DDS, MIPmap size calculation is block based	*/
				DDS_full_size += ((w + 3) / 4) * ((h + 3) / 4) * block_size;
			}
		}
	} else {
		mipmaps = 0;
		DDS_full_size = DDS_main_size;
	}
	DDS_data = (unsigned char*)malloc(DDS_full_size);
	/*	got the image data RAM, create or use an existing OpenGL texture handle	*/

	glGenTextures(1, &tex_ID);
	/*  bind an OpenGL texture ID	*/
	glBindTexture(opengl_texture_type, tex_ID);
	if(tex_ID) {
		/*	did I have MIPmaps?	*/
		if(mipmaps > 0 || (flags & SOIL_FLAG_MIPMAPS)) {
			/*	instruct OpenGL to use the MIPmaps	*/
			glTexParameteri(opengl_texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(opengl_texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		} else {
			/*	instruct OpenGL _NOT_ to use the MIPmaps	*/
			glTexParameteri(opengl_texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(opengl_texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		/*	does the user want clamping, or wrapping?	*/
		if(flags & SOIL_FLAG_TEXTURE_REPEATS) {
			glTexParameteri(opengl_texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(opengl_texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(opengl_texture_type, SOIL_TEXTURE_WRAP_R, GL_REPEAT);
		} else {
			unsigned int clamp_mode = SOIL_CLAMP_TO_EDGE;
			/* unsigned int clamp_mode = GL_CLAMP; */
			glTexParameteri(opengl_texture_type, GL_TEXTURE_WRAP_S, clamp_mode);
			glTexParameteri(opengl_texture_type, GL_TEXTURE_WRAP_T, clamp_mode);
			glTexParameteri(opengl_texture_type, SOIL_TEXTURE_WRAP_R, clamp_mode);
		}
	}

	/*	do this for each face of the cubemap!	*/
	for(cf_target = ogl_target_start; cf_target <= ogl_target_end; ++cf_target) {
		if(buffer_index + DDS_full_size <= (unsigned int)buffer_length) {
			unsigned int byte_offset = DDS_main_size;
			memcpy((void*)DDS_data, (const void*)(&buffer[buffer_index]), DDS_full_size);
			buffer_index += DDS_full_size;
			/*	upload the main chunk	*/
			if(uncompressed) {
				/*	and remember, DXT uncompressed uses BGR(A),
					so swap to RGB(A) for ALL MIPmap levels	*/
				for(i = 0; i < (int)DDS_full_size; i += block_size) {
					unsigned char temp = DDS_data[i];
					DDS_data[i] = DDS_data[i + 2];
					DDS_data[i + 2] = temp;
				}
				glTexImage2D(
					cf_target, 0,
					S3TC_type, width, height, 0,
					S3TC_type, GL_UNSIGNED_BYTE, DDS_data);
			} else {
				glCompressedTexImage2D(
					cf_target, 0,
					S3TC_type, width, height, 0,
					DDS_main_size, DDS_data);
			}
			/*	upload the mipmaps, if we have them	*/
			for(i = 1; i <= mipmaps; ++i) {
				int w, h, mip_size;
				w = width >> i;
				h = height >> i;
				if(w < 1) {
					w = 1;
				}
				if(h < 1) {
					h = 1;
				}
				/*	upload this mipmap	*/
				if(uncompressed) {
					mip_size = w * h * block_size;
					glTexImage2D(
						cf_target, i,
						S3TC_type, w, h, 0,
						S3TC_type, GL_UNSIGNED_BYTE, &DDS_data[byte_offset]);
				} else {
					mip_size = ((w + 3) / 4) * ((h + 3) / 4) * block_size;
					glCompressedTexImage2D(
						cf_target, i,
						S3TC_type, w, h, 0,
						mip_size, &DDS_data[byte_offset]);
				}
				/*	and move to the next mipmap	*/
				byte_offset += mip_size;
			}
		} else {
			glDeleteTextures(1, &tex_ID);
			tex_ID = 0;
			cf_target = ogl_target_end + 1;
		}
	}/* end reading each face */
	free(DDS_data);

	if(flags & SOIL_FLAG_MIPMAPS)
		glGenerateMipmap(opengl_texture_type);

quick_exit:
	/*	report success or failure	*/
	return tex_ID;
}

texture::~texture() {
	STBI_FREE(data);
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
		auto dds_name = native_name.substr(0, name_length - 3) + NATIVE("dds");
		auto file = open_file(root, dds_name);
		if(file) {
			auto content = simple_fs::view_contents(*file);

			uint32_t w = 0;
			uint32_t h = 0;
			asset_texture.texture_handle =
				SOIL_direct_load_DDS_from_memory(reinterpret_cast<uint8_t const*>(content.data), content.file_size, w, h, 0);

			if(asset_texture.texture_handle) {
				asset_texture.channels = 4;
				asset_texture.size_x = int32_t(w);
				asset_texture.size_y = int32_t(h);
				asset_texture.loaded = true;

				if(keep_data) {
					asset_texture.data = static_cast<uint8_t*>(STBI_MALLOC(4 * w * h));
					glGetTextureImage(asset_texture.texture_handle, 0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<int32_t>(4 * w * h), asset_texture.data);
				}
				return asset_texture.texture_handle;
			}
		}
	}

	auto file = open_file(root, native_name);
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
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, asset_texture.size_x, asset_texture.size_y, GL_RGBA, GL_UNSIGNED_BYTE, asset_texture.data);

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
	return 0;
}

GLuint get_flag_handle(sys::state& state, dcon::national_identity_id nat_id, culture::flag_type type) {
	dcon::texture_id id = dcon::texture_id{ dcon::texture_id::value_base_t(state.ui_defs.textures.size() + nat_id.index() * culture::flag_count + uint32_t(culture::flag_count)) };

	if(state.open_gl.asset_textures[id].loaded) {
		return state.open_gl.asset_textures[id].texture_handle;
	} else { // load from file

		native_string file_str;
		file_str += NATIVE("gfx") + NATIVE_DIR_SEPARATOR;
		file_str += NATIVE("flags") + NATIVE_DIR_SEPARATOR;
		file_str += simple_fs::win1250_to_native(nations::int_to_tag(state.world.national_identity_get_identifying_int(nat_id)));
		switch(type) {
			case culture::flag_type::communist:
				file_str += NATIVE("_communist"); break;
			case culture::flag_type::default_flag:
				break;
			case culture::flag_type::fascist:
				file_str += NATIVE("_fascist"); break;
			case culture::flag_type::monarchy:
				file_str += NATIVE("_monarchy"); break;
			case culture::flag_type::republic:
				file_str += NATIVE("_monarchy"); break;
		}
		file_str += NATIVE(".tga");

		return load_file_and_return_handle(file_str, state.common_fs, state.open_gl.asset_textures[id], false);
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
	} else  if(channels == 2) {
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG8, size, 1);
	} else {
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, size, 1);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);
}

data_texture::~data_texture() {
	delete[] data;
}

uint32_t data_texture::handle() {
	if(data && data_updated) {
		glBindTexture(GL_TEXTURE_2D, texture_handle);
		if(channels == 3) {
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
		} else if(channels == 4) {
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
		} else  if(channels == 2) {
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

}
