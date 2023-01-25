#include "texture.hpp"
#include "system_state.hpp"
#include "simple_fs.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO

#include "stb_image.h"

namespace ogl {

// DDS loader based on https://gist.github.com/tilkinsc/13191c0c1e5d6b25fbe79bbd2288a673
GLuint texture_loadDDS(uint8_t const* file_data, uint32_t& w, uint32_t& h, uint32_t file_size) {
	// lay out variables to be used
	unsigned char const* header;

	unsigned int width;
	unsigned int height;
	unsigned int mipMapCount;

	unsigned int blockSize;
	unsigned int format;

	unsigned char const* buffer = 0;

	GLuint tid = 0;


	// allocate new unsigned char space with 4 (file code) + 124 (header size) bytes
	// read in 128 bytes from the file
	header = file_data;

	// compare the `DDS ` signature
	if(memcmp(header, "DDS ", 4) != 0)
		return 0;

	// extract height, width, and amount of mipmaps - yes it is stored height then width
	height = (header[12]) | (header[13] << 8) | (header[14] << 16) | (header[15] << 24);
	width = (header[16]) | (header[17] << 8) | (header[18] << 16) | (header[19] << 24);
	mipMapCount = (header[28]) | (header[29] << 8) | (header[30] << 16) | (header[31] << 24);

	// figure out what format to use for what fourCC file type it is
	// block size is about physical chunk storage of compressed data in file (important)
	if(header[84] == 'D') {
		switch(header[87]) {
			case '1': // DXT1
				format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				blockSize = 8;
				break;
			case '3': // DXT3
				format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				blockSize = 16;
				break;
			case '5': // DXT5
				format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				blockSize = 16;
				break;
			case '0': // DX10
				// unsupported, else will error
				// as it adds sizeof(struct DDS_HEADER_DXT10) between pixels
				// so, buffer = malloc((file_size - 128) - sizeof(struct DDS_HEADER_DXT10));
			default:
				return 0;
		}
	} else { // BC4U/BC4S/ATI2/BC55/R8G8_B8G8/G8R8_G8B8/UYVY-packed/YUY2-packed unsupported
		return 0;
	}

	// allocate new unsigned char space with file_size - (file_code + header_size) magnitude
	// read rest of file
	buffer = file_data + 128;

	// prepare new incomplete texture
	glGenTextures(1, &tid);
	if(tid == 0)
		return 0;

	// bind the texture
	// make it complete by specifying all needed parameters and ensuring all mipmaps are filled
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1); // opengl likes array length of mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // don't forget to enable mipmaping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// prepare some variables
	unsigned int offset = 0;
	unsigned int size = 0;
	w = width;
	h = height;

	// loop through sending block at a time with the magic formula
	// upload to opengl properly, note the offset transverses the pointer
	// assumes each mipmap is 1/2 the size of the previous mipmap
	for(unsigned int i = 0; i < mipMapCount; i++) {
		if(w == 0 || h == 0) { // discard any odd mipmaps 0x1 0x2 resolutions
			mipMapCount--;
			continue;
		}
		size = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, i, format, w, h, 0, size, buffer + offset);
		offset += size;
		w /= 2;
		h /= 2;
	}
	// discard any odd mipmaps, ensure a complete texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1);
	// unbind
	glBindTexture(GL_TEXTURE_2D, 0);

	return tid;
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

GLuint get_texture_handle(sys::state& state, dcon::texture_id id, bool keep_data) {
	if(state.open_gl.asset_textures[id].loaded) {
		return state.open_gl.asset_textures[id].texture_handle;
	} else { // load from file
		auto fname = state.ui_defs.textures[id];
		auto fname_view = state.to_string_view(fname);
		auto native_name = simple_fs::win1250_to_native(fname_view);
		auto name_length = native_name.length();

		auto root = get_root(state.common_fs);
		if(name_length > 4) { // try loading as a dds
			auto dds_name = native_name.substr(0, name_length - 3) + NATIVE("dds");
			auto file = open_file(root, dds_name);
			if(file) {
				auto content = simple_fs::view_contents(*file);

				uint32_t w = 0;
				uint32_t h = 0;
				state.open_gl.asset_textures[id].texture_handle =
					texture_loadDDS(reinterpret_cast<uint8_t const*>(content.data), w, h, content.file_size);

				if(state.open_gl.asset_textures[id].texture_handle) {
					state.open_gl.asset_textures[id].channels = 4;
					state.open_gl.asset_textures[id].size_x = int32_t(w);
					state.open_gl.asset_textures[id].size_y = int32_t(h);
					state.open_gl.asset_textures[id].loaded = true;

					if(keep_data) {
						state.open_gl.asset_textures[id].data = static_cast<uint8_t*>(STBI_MALLOC(4 * w * h));
						glGetTextureImage(state.open_gl.asset_textures[id].texture_handle, 0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<int32_t>(4 * w * h), state.open_gl.asset_textures[id].data);
					}
					return state.open_gl.asset_textures[id].texture_handle;
				}
			}
		}

		auto file = open_file(root, native_name);
		if(file) {
			auto content = simple_fs::view_contents(*file);

			int32_t file_channels = 4;

			state.open_gl.asset_textures[id].data = stbi_load_from_memory(reinterpret_cast<uint8_t const*>(content.data), int32_t(content.file_size),
				&(state.open_gl.asset_textures[id].size_x), &(state.open_gl.asset_textures[id].size_y), &file_channels, 4);


			state.open_gl.asset_textures[id].channels = 4;
			state.open_gl.asset_textures[id].loaded = true;

			glGenTextures(1, &state.open_gl.asset_textures[id].texture_handle);
			if(state.open_gl.asset_textures[id].texture_handle) {
				glBindTexture(GL_TEXTURE_2D, state.open_gl.asset_textures[id].texture_handle);

				glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, state.open_gl.asset_textures[id].size_x, state.open_gl.asset_textures[id].size_y);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, state.open_gl.asset_textures[id].size_x, state.open_gl.asset_textures[id].size_y, GL_RGBA, GL_UNSIGNED_BYTE, state.open_gl.asset_textures[id].data);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				glBindTexture(GL_TEXTURE_2D, 0);
			}

			if(!keep_data) {
				STBI_FREE(state.open_gl.asset_textures[id].data);
				state.open_gl.asset_textures[id].data = nullptr;
			}

			return state.open_gl.asset_textures[id].texture_handle;
		}
		return 0;
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
