#pragma once
#include <d3d11.h>
#include <wincodec.h>


#ifdef _WIN64
#define DIRECTX_11


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
}
#endif
