#pragma once
#include "DrawableBase.h"

#include "VertexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "InputLayout.h"
#include "Topology.h"
#include "TransformConstantBuffer.h"

class SolidBox : public DrawableBase<SolidBox>
{
public:
	SolidBox(Graphics& gfx,bool isWireframe = false);

	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

	void SetPos(DirectX::XMFLOAT3 pos) noexcept;

private:
	bool isWireframe;
	DirectX::XMFLOAT3 pos = { 10.0f,9.0f,2.5f };
};

