#pragma once

#include "ConstantBuffer.h"
#include "SolidBox.h"

class PointLight
{
public:
	PointLight(Graphics& gfx);
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;

	void Draw(Graphics& gfx) const;
	void Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept;
	void SetPos(DirectX::XMFLOAT3 pos) const;

private:
	struct PointLightCBuf
	{
		// alignas(n) adds n bits padding

		alignas(16) DirectX::XMFLOAT3 pos; 
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16) DirectX::XMFLOAT3 diffuseColor;

		float diffuseIntensity;
		float attConst;
		float attLin;
		float attQuad;
	};
private:
	bool showGizmo;
	mutable PointLightCBuf cbData;
	mutable SolidBox gizmo;
	mutable PixelConstantBuffer<PointLightCBuf> cbuf;
};

