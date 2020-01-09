#include "PixelShader.h"
#include <d3dcompiler.h>
#include "GraphicsThrowMacros.h"

PixelShader::PixelShader(Graphics& gfx, const std::wstring& path)
{
	HRESULT hr;

	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	D3DReadFileToBlob(path.c_str(), &pBlob);

	GFX_THROW_FAILED(GetDevice(gfx)->CreatePixelShader(
		pBlob->GetBufferPointer(), 
		pBlob->GetBufferSize(), 
		nullptr, 
		&pPixelShader
	));
}

void PixelShader::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShader(pPixelShader.Get(), nullptr, 0u);
}
