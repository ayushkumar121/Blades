#pragma once
#include "DrawableBase.h"

#include "VertexBuffer.h"
#include "VertexShader.h"
#include "Texture.h"
#include "Sampler.h"
#include "Blender.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "InputLayout.h"
#include "Topology.h"
#include "TransformConstantBuffer.h"

class Sprite : public DrawableBase<Sprite>
{
public:
	Sprite(Graphics& gfx, std::string path, float x = 0.0f, float y = 0.0f, float scale = 1.0f);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

public:
	struct
	{
		float x = 0.0f;
		float y = 0.0f;

		float yaw = 0.0f;
		float roll = 0.0f;
		float pitch = 0.0f;

		float scale = 1.0f;
	} transform;

	std::string path;
};

