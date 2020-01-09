#include "TransformConstantBuffer.h"

TransformConstantBuffer::TransformConstantBuffer(Graphics& gfx, const Drawable& parent, UINT slot)
	:
	parent(parent)
{
	if (!pVcbuf)
	{
		pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
	}
}

void TransformConstantBuffer::Bind(Graphics & gfx) noexcept
{
	const auto modelView = parent.GetTransformXM() * gfx.GetCamera();
	const Transforms tf =
	{
		DirectX::XMMatrixTranspose(modelView),
		DirectX::XMMatrixTranspose(
			modelView *
			gfx.GetCamera() *
			gfx.GetProjection()
		)
	};

	pVcbuf->Update(gfx, tf);
	pVcbuf->Bind(gfx);
}

std::unique_ptr<VertexConstantBuffer<TransformConstantBuffer::Transforms>> TransformConstantBuffer::pVcbuf;