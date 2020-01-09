#include "Sprite.h"
#include "ImGUI/imgui.h"

Sprite::Sprite(Graphics& gfx, std::string path, float x, float y, float scale)
	:
	path(path)
{
	transform.x = x;
	transform.y = y;
	transform.scale = scale;

	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			struct
			{
				float x;
				float y;
				float z;
			}pos;

			struct
			{
				float u;
				float v;
			} tex;
		};

		const std::vector<Vertex> vertices =
		{
			{-1.0f, -1.0f, 0.0f, 0.0f, 1.0f},
			{ 1.0f, -1.0f, 0.0f, 1.0f, 1.0f},
			{ 1.0f,  1.0f, 0.0f, 1.0f, 0.0f},
			{-1.0f,  1.0f, 0.0f, 0.0f, 0.0f}
		};

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));

		const std::vector<unsigned short> indices =
		{
			0, 2, 1,  2, 0, 3
		};

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));


		auto pVs = std::make_unique<VertexShader>(gfx, L"TexturedVS.cso");
		auto pvsbc = pVs->GetBytecode();

		AddStaticBind(std::move(pVs));
		AddStaticBind(std::make_unique<Sampler>(gfx)); // Texture Filtering
		AddStaticBind(std::make_unique<Blender>(gfx, true)); // Blend State
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"TexturedPS.cso"));


		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<Texture>(gfx, path));
	AddBind(std::make_unique<TransformConstantBuffer>(gfx, *this));
}

DirectX::XMMATRIX Sprite::GetTransformXM() const noexcept
{
	return
		DirectX::XMMatrixScaling(transform.scale, transform.scale, transform.scale) *
		DirectX::XMMatrixRotationRollPitchYaw(transform.pitch, transform.yaw, transform.roll) *
		DirectX::XMMatrixTranslation(transform.x, transform.y, 0.0f);
}