#include <d3d11.h>


#ifndef DIRECTX_11
#define DIRECTX_11
#endif

namespace directx {
struct data {
	ID3D11Device* device;
	ID3D11DeviceContext* context;
};

void create_directx_context(sys::state& state);
void shutdown_directx(sys::state& state);
}
