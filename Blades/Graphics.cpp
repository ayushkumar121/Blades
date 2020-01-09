#include "Graphics.h"
#include "GraphicsThrowMacros.h"

#include <cstdlib>
#include <sstream>

#include "IndexBuffer.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

namespace wrl = Microsoft::WRL;

Graphics::Graphics(HWND hWnd)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	
	// For further usage
	HRESULT hr;

	DXGI_SWAP_CHAIN_DESC sd = {};

	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	// Antialising
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// Create device and front back/back buffers, swap chain and rendering context
	GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	));

	// Gain access to texture subresourse in swap chain
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	
	GFX_THROW_FAILED(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_FAILED(pDevice->CreateRenderTargetView(
		pBackBuffer.Get(),
		nullptr,
		&pTarget
	));

	// Create depth stencil state
	wrl::ComPtr<ID3D11DepthStencilState> pDSState;

	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	
	GFX_THROW_FAILED(pDevice->CreateDepthStencilState(&dsDesc, &pDSState));

	// Bind depth stencil state
	pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	// Create depth stencil texture
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};

	descDepth.Width = rect.right-rect.left;
	descDepth.Height = rect.bottom - rect.top;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;

	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;

	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	GFX_THROW_FAILED(pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));
	 
	// Create depth stencil view texture
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;

	GFX_THROW_FAILED(pDevice->CreateDepthStencilView(
		pDepthStencil.Get(),
		&descDSV,
		&pDSV
	));

	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());

	// Configure viewport

	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)(rect.right - rect.left);
	vp.Height = (FLOAT)(rect.bottom - rect.top);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;

	pContext->RSSetViewports(1u, &vp);

	// Init imgui d3d
	ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());
}

void Graphics::EndFrame()
{
	if (imguiEnabled)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	HRESULT hr;

	if (FAILED(hr = pSwap->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			GFX_THROW_FAILED(hr);
		}
	}
	pSwap->Present(2u, 0u);
}

void Graphics::BeginFrame(float red, float green, float blue) noexcept
{
	if (imguiEnabled)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	const float color[] = { red, green, blue, 1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color); // Clearing render target
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u); // Clearing depth buffer

}

void Graphics::EnableImgui() noexcept
{
	imguiEnabled = true;
}

void Graphics::DisableImgui() noexcept
{
	imguiEnabled = false;
}

bool Graphics::IsImguiEnabled() const noexcept
{
	return imguiEnabled;
}

void Graphics::DrawIndexed(UINT count)
{
	pContext->DrawIndexed(count, 0u, 0u);
}

void Graphics::SetProjection(DirectX::XMMATRIX proj) noexcept
{
	projection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return projection;;
}

void Graphics::SetCamera(DirectX::XMMATRIX cam) noexcept
{
	camera = cam;
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return camera;
}

// Graphics exception stuff
Graphics::Exception::Exception(
	int line,
	const char* file,
	HRESULT hr
) noexcept
	:
	BladesException(line, file),
	hr(hr)
{}

const char* Graphics::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();

	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::Exception::GetType() const noexcept
{
	return "Blades Graphics Exception";
}

std::string Graphics::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	wchar_t* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&pMsgBuf),
		0,
		nullptr
	);

	if (nMsgLen == 0)
	{
		return "Undefined error code";
	}

	char buffer[200];
	size_t x;

	buffer[0] = '\0';
	wcstombs_s(&x, buffer, pMsgBuf, sizeof(buffer));

	std::string errorString = buffer;
	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT Graphics::Exception::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Blades graphics exception [Device removed]";
}
