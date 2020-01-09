#pragma once
#include "WinHeader.h"
#include "BladesException.h"
#include <d3d11.h>
#include <wrl.h>

#include <d3dcompiler.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

class Graphics
{
	friend class Bindable;
public:
	class Exception : public BladesException
	{
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;

		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;

	private:
		HRESULT hr;
	};

	class DeviceRemovedException : public Exception
	{
		using Exception::Exception;
	public:
		const char* GetType() const noexcept override;
	};

public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	void EndFrame();
	void BeginFrame(float red, float green, float blue) noexcept;

	void EnableImgui() noexcept;
	void DisableImgui() noexcept;
	bool IsImguiEnabled() const noexcept;

	void DrawIndexed(UINT count);

	void SetProjection(DirectX::XMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;

	void SetCamera(DirectX::XMMATRIX cam) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;


private:
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;

	bool imguiEnabled = true;

	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
};

