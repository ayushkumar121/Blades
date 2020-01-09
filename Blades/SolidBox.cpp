#include "SolidBox.h"

SolidBox::SolidBox(Graphics& gfx, bool isWireframe)
	:
	isWireframe(isWireframe)
{
	if (!IsStaticInitialized())
	{
		const std::vector<DirectX::XMFLOAT3> vertices =
		{
			{-0.3f, -0.3f, -0.3f},
			{ 0.3f, -0.3f, -0.3f},
			{-0.3f,  0.3f, -0.3f},
			{ 0.3f,  0.3f, -0.3f},

			{-0.3f, -0.3f,  0.3f},
			{ 0.3f, -0.3f,  0.3f},
			{-0.3f,  0.3f,  0.3f},
			{ 0.3f,  0.3f,  0.3f}
		};

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));

		auto pVs = std::make_unique<VertexShader>(gfx, L"SolidVS.cso");
		auto pvsbc = pVs->GetBytecode();

		AddStaticBind(std::move(pVs)); // Vertex Shader
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"SolidPS.cso"));

		const std::vector<unsigned short> indices =
		{
			0, 2, 3,  0, 3, 1,
			0, 1, 5,  0, 5, 4,
			0, 4, 6,  0, 6, 2,
			1, 3, 7,  1, 7, 5,
			2, 6, 7,  2, 7, 3,
			4, 5, 7,  4, 7, 6
		};

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		if (isWireframe)
			topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;

		AddStaticBind(std::make_unique<Topology>(gfx, topology));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformConstantBuffer>(gfx, *this));
}

void SolidBox::Update(float dt) noexcept {}

void SolidBox::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

DirectX::XMMATRIX SolidBox::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}
