#include "Texture.h"
#include <memory>
#include <vector>
#include <DirectXTex.h>
#include <sstream>

Texture::Texture(Graphics& gfx, const std::string& path)
	:
	path(path)
{
	HRESULT hr;

	std::wstringstream oss;
	oss << path.c_str();

	DirectX::TexMetadata info;
	auto scrachImage = std::make_unique<DirectX::ScratchImage>();
	GFX_THROW_FAILED(LoadFromWICFile(oss.str().c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &info, *scrachImage));

	auto texture = scrachImage.get()->GetImage(0, 0, 0);

	UINT width = info.width;
	UINT height = info.height;


	D3D11_TEXTURE2D_DESC textureDesc = {};

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = texture->pixels;
	sd.SysMemPitch = texture->rowPitch;
	
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	GFX_THROW_FAILED(GetDevice(gfx)->CreateTexture2D(
		&textureDesc, &sd, &pTexture
	));

	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	GFX_THROW_FAILED(GetDevice(gfx)->CreateShaderResourceView(
		pTexture.Get(), &srvDesc, &pTextureView
	));
}

void Texture::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShaderResources(0u, 1u, pTextureView.GetAddressOf());
}
