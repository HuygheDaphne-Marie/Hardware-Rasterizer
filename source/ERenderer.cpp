#include "pch.h"

//Project includes
#include "ERenderer.h"

// DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

Elite::Renderer::Renderer(SDL_Window * pWindow)
	: m_pWindow{ pWindow }
	, m_Width{}
	, m_Height{}
	, m_IsInitialized{ false }
{
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);

	//Initialize DirectX pipeline
	const HRESULT initResult = InitialiseDirectX();

	if (!FAILED(initResult))
	{
		// Set initialized flag
		m_IsInitialized = true;
		std::cout << "DirectX is ready\n";
	}
}

Elite::Renderer::~Renderer()
{
	// Todo: Delete resources
}

void Elite::Renderer::Render()
{
	if (!m_IsInitialized) 
		return;
}

HRESULT Elite::Renderer::InitialiseDirectX()
{
	//Create Device and Device context, using hardware acceleration
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	uint32_t createDeviceFlags = 0;

	//Set Debug flag
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const HRESULT createDeviceResult = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, &m_pDevice, &featureLevel, &m_pDeviceContext);
	if (FAILED(createDeviceResult))
		return createDeviceResult;

}


