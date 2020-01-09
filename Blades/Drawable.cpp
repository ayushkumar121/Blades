#include "Drawable.h"

void Drawable::Draw(Graphics& gfx) const
{
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}

	for (auto& b : GetStaticBinds())
	{
		b->Bind(gfx);
	}

	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

std::string Drawable::GetTag() noexcept
{
	return tag;
}

void Drawable::SetTag(std::string _tag) noexcept
{
	tag = _tag;
}

// For adding binding other than index buffers
void Drawable::AddBind(std::unique_ptr<Bindable> bind)
{
	binds.push_back(std::move(bind));
}

// For adding index buffers
void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept
{
	pIndexBuffer = ibuf.get();
	binds.push_back(std::move(ibuf));
}
