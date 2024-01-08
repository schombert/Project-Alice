#pragma once

#ifdef _WIN64
#define DIRECTX_11

#include <d3d11.h>
#include <wincodec.h>

namespace directx {
struct data {
	ID3D11Device* device;
	ID3D11DeviceContext* context;

	IWICBitmapDecoder* money_icon_decoder;
	IWICBitmapDecoder* navy_icon_decoder;
	IWICBitmapDecoder* army_icon_decoder;
	IWICBitmapDecoder* cross_icon_decoder;
	IWICBitmapDecoder* checkmark_icon_decoder;

};

struct image {
	BYTE* data = nullptr;
	int32_t size_x = 0;
	int32_t size_y = 0;
	int32_t channels = 0;

	image() { }

	image(BYTE* data, int32_t size_x, int32_t size_y, int32_t channels) {
		this->data = data;
		this->size_x = size_x;
		this->size_y = size_y;
		this->channels = channels;
	}
	image(image const& other) = delete;

	image(image&& other) noexcept {
		data = other.data;
		size_x = other.size_x;
		size_y = other.size_y;
		channels = other.channels;
		other.data = nullptr;
	}

	image& operator=(image&& other) noexcept {
		data = other.data;
		size_x = other.size_x;
		size_y = other.size_y;
		channels = other.channels;
		other.data = nullptr;
		return *this;
	}
	image& operator=(image const& other) = delete;

	~image() {
		if(data)
			free(data);
	}
};

void create_directx_context(sys::state& state);
void shutdown_directx(sys::state& state);
HRESULT compile_shader_from_file(sys::state& state, _In_ LPCWSTR file_name, _In_ LPCSTR entry_point, _Outptr_ ID3DBlob** blob);
HRESULT load_dds_from_file_content(IWICImagingFactory* iwici_factory, IWICStream* iwici_stream, simple_fs::file_contents content, IWICBitmapDecoder* decoder);
void load_special_icons(sys::state& state);

image load_stb_image(simple_fs::file& file);
ID3D11Texture2D* make_texture(sys::state& state, BYTE* data, uint32_t size_x, uint32_t size_y, uint32_t channels);
}
#endif
