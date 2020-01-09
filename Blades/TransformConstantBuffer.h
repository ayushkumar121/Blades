#pragma once
#include "Bindable.h"
#include "Drawable.h"
#include "ConstantBuffer.h"
#include <DirectXMath.h>

class TransformConstantBuffer : public Bindable
{
public:
	TransformConstantBuffer(Graphics& gfx, const Drawable& parent, UINT slot = 0u);
	void Bind(Graphics& gfx) noexcept override;

private:
	struct Transforms
	{
		DirectX::XMMATRIX modelViewProj;
		DirectX::XMMATRIX model;
	};

private:
	static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
	const Drawable& parent;
};