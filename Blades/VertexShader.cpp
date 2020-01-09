#include "VertexShader.h"
#include <d3dcompiler.h>
#include "GraphicsThrowMacros.h"

VertexShader::VertexShader(Graphics& gfx, const std::wstring& path)
{
	HRESULT hr;

	D3DReadFileToBlob(path.c_str(), &pBytecodeBlob);

	GFX_THROW_FAILED(GetDevice(gfx)->CreateVertexShader(
		pBytecodeBlob->GetBufferPointer(), 
		pBytecodeBlob->GetBufferSize(), 
		nullptr, 
		&pVertexShader
	));
}

void VertexShader::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->VSSetShader(pVertexShader.Get(), nullptr, 0u);
}

ID3DBlob* VertexShader::GetBytecode() const noexcept
{
	return pBytecodeBlob.Get();
}
