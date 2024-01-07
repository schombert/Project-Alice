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

void create_directx_context(sys::state& state);
void shutdown_directx(sys::state& state);
HRESULT compile_shader_from_file(sys::state& state, _In_ LPCWSTR file_name, _In_ LPCSTR entry_point, _Outptr_ ID3DBlob** blob);
HRESULT load_dds_from_file_content(IWICImagingFactory* iwici_factory, IWICStream* iwici_stream, simple_fs::file_contents content, IWICBitmapDecoder* decoder);
void load_special_icons(sys::state& state);
}
#endif
