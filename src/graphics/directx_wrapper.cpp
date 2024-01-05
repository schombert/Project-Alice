#include "directx_wrapper.hpp"
#include <d3dcompiler.h>


namespace directx {

void create_directx_context(sys::state& state) {
	ID3D11Device* device;
	ID3D11DeviceContext* context;

	D3D_FEATURE_LEVEL feature_levels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	UINT flags = 0;
#ifdef _DEBUG
	flags = D3D11_CREATE_DEVICE_DEBUG;
#endif 

	HRESULT result = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0,
									   flags, feature_levels,
									   ARRAYSIZE(feature_levels),
									   D3D11_SDK_VERSION, &device, 0,
									   &context);


	if(FAILED(result)) {
		MessageBox(nullptr, L"Failed to create DirectX 11 device", 0, 0);
		std::abort();
	}
	state.directx.device = device;
	state.directx.context = context;
}

void shutdown_directx(sys::state& state) {
	state.directx.device->Release();
	state.directx.context->Release();
}

/*
	file_name: the path to the shader file
	entry_point: the name of the function that acts as entry point for the shader
	blob: the resulting compiled shader
*/
HRESULT compile_shader_from_file(sys::state& state, _In_ LPCWSTR file_name, _In_ LPCSTR entry_point, _Outptr_ ID3DBlob** blob) {
	// Getting higher cs shader profile when possible
	LPCSTR profile = (state.directx.device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";

	// TODO: define required macros
	const D3D_SHADER_MACRO defines[] =
	{
		NULL, NULL
	};

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* shader_blob = nullptr;
	ID3DBlob* error_blob = nullptr;

	HRESULT result = D3DCompileFromFile(file_name, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point, profile,
									 flags, 0, &shader_blob, &error_blob);

	if(FAILED(result)) {
		if(error_blob) {
			MessageBox(state.win_ptr->hwnd, L"An error has occurred during shader compilation", L"Shader compilation error", MB_OK);
			error_blob->Release();
		}

		if(shader_blob)
			shader_blob->Release();

		return result;
	}

	*blob = shader_blob;
	return result;
}
// TODO
void create_program();

void load_special_icons(sys::state& state) {
	auto root = get_root(state.common_fs);
	auto gfx_dir = simple_fs::open_directory(root, NATIVE("gfx"));

	auto interface_dir = simple_fs::open_directory(gfx_dir, NATIVE("interface"));
	auto money_dds = simple_fs::open_file(interface_dir, NATIVE("icon_money_big.dds"));
	if(money_dds) {
		
	}

}
}
