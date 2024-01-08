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

	// Initializing DirectXTex
	result = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

	if(FAILED(result)) {
		std::abort();
	}


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
	{ {
		NULL, NULL
	} };

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

// to load a single special icon
HRESULT load_dds_from_file_content(IWICImagingFactory* iwici_factory, IWICStream* iwici_stream, simple_fs::file_contents content, IWICBitmapDecoder* decoder) {

	auto image_stream = const_cast<BYTE*>(reinterpret_cast<BYTE const*>(content.data));

	HRESULT result = iwici_stream->InitializeFromMemory(image_stream, content.file_size);

	if(FAILED(result)) {
		return result;
	}

	result = iwici_factory->CreateDecoderFromStream(iwici_stream, NULL, WICDecodeMetadataCacheOnLoad, &decoder);
	return result;
}

// to load them all
void load_special_icons(sys::state& state) {
	HRESULT result;

	IWICImagingFactory* iwici_factory;
	IWICStream* iwici_stream = NULL;

	result = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&iwici_factory)
	);

	if(FAILED(result)) {
		// TODO
	}

	result = iwici_factory->CreateStream(&iwici_stream);

	if(FAILED(result)) {
		// TODO
	}

	auto root = get_root(state.common_fs);
	auto gfx_dir = simple_fs::open_directory(root, NATIVE("gfx"));

	auto interface_dir = simple_fs::open_directory(gfx_dir, NATIVE("interface"));
	auto money_dds = simple_fs::open_file(interface_dir, NATIVE("icon_money_big.dds"));
	if(money_dds) {
		auto content = simple_fs::view_contents(*money_dds);
		result = load_dds_from_file_content(iwici_factory, iwici_stream, content, state.directx.money_icon_decoder);
		if(FAILED(result)) {
			// TODO
		}
	}

	auto n_dds = simple_fs::open_file(interface_dir, NATIVE("politics_foreign_naval_units.dds"));
	if(n_dds) {
		auto content = simple_fs::view_contents(*n_dds);
		result = load_dds_from_file_content(iwici_factory, iwici_stream, content, state.directx.navy_icon_decoder);
		if(FAILED(result)) {
			// TODO
		}
	}

	auto a_dds = simple_fs::open_file(interface_dir, NATIVE("topbar_army.dds"));
	if(a_dds) {
		auto content = simple_fs::view_contents(*a_dds);
		result = load_dds_from_file_content(iwici_factory, iwici_stream, content, state.directx.army_icon_decoder);
		if(FAILED(result)) {
			// TODO
		}
	}


	auto assets_dir = simple_fs::open_directory(root, NATIVE("assets"));
	auto cross_dds = simple_fs::open_file(assets_dir, NATIVE("trigger_not.dds"));
	if(cross_dds) {
		auto content = simple_fs::view_contents(*cross_dds);
		result = load_dds_from_file_content(iwici_factory, iwici_stream, content, state.directx.cross_icon_decoder);
		if(FAILED(result)) {
			// TODO
		}
	}

	auto checkmark_dds = simple_fs::open_file(assets_dir, NATIVE("trigger_yes.dds"));
	if(checkmark_dds) {
		auto content = simple_fs::view_contents(*checkmark_dds);
		result = load_dds_from_file_content(iwici_factory, iwici_stream, content, state.directx.checkmark_icon_decoder);
		if(FAILED(result)) {
			// TODO
		}
	}
}

image load_stb_image(simple_fs::file& file) {
	int32_t file_channels = 4;
	int32_t size_x = 0;
	int32_t size_y = 0;
	auto content = simple_fs::view_contents(file);
	auto data = stbi_load_from_memory(reinterpret_cast<uint8_t const*>(content.data), int32_t(content.file_size), &size_x, &size_y, &file_channels, 4);
	return image(data, size_x, size_y, 4);
}

ID3D11Texture2D* make_texture(sys::state& state, BYTE* data, uint32_t size_x, uint32_t size_y, uint32_t channels) {
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = size_x;
	desc.Height = size_y;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA texture_data;
	texture_data.pSysMem = data;


	ID3D11Texture2D* texture = NULL;
	HRESULT result = state.directx.device->CreateTexture2D(&desc, &texture_data, &texture);

	if(FAILED(result)) {
		// TODO
	}

	return texture;
}

ID3D11Texture2D* make_texture(sys::state& state, simple_fs::directory const& dir, native_string_view file_name) {
	auto file = open_file(dir, file_name);
	auto image = load_stb_image(*file);
	return make_texture(state, image.data, image.size_x, image.size_y, image.channels);
}


ID3D11Texture2D* load_texture_array_from_file(sys::state& state, simple_fs::file& file, int32_t tiles_x, int32_t tiles_y) {
	auto image = load_stb_image(file);
	size_t p_dx = image.size_x / tiles_x; // Pixels of each tile in x
	size_t p_dy = image.size_y / tiles_y; // Pixels of each tile in y

	
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = tiles_x;
	desc.Height = tiles_y;
	desc.MipLevels =  1;
	desc.ArraySize = tiles_x * tiles_y; // total number of textures
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA texture_data;
	texture_data.pSysMem = image.data; 

	ID3D11Texture2D* texture = NULL;
	state.directx.device->CreateTexture2D(&desc, &texture_data, &texture);

	return texture;
}
} // namespace directx
