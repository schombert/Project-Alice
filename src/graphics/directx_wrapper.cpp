#include <d3d11.h>

namespace directx {
void create_directx_context(sys::state& state) {
	ID3D11Device* device;
	ID3D11DeviceContext* context;

	D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0 };
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

	
}
}
