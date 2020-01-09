#pragma once
#include "Bindable.h"

class Texture : public Bindable
{
public:
	Texture(Graphics& gfx, const std::string& path);
	void Bind(Graphics& gfx) noexcept override;

protected:
	std::string path;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};