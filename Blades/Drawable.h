#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include "IndexBuffer.h"
#include <memory>

class Bindable;

class Drawable
{
	template<class T>
	friend class DrawableBase;
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	
	void Draw(Graphics& gfx) const;
	std::string GetTag() noexcept;
	void SetTag(std::string tag) noexcept;

	virtual void Update(float dt) noexcept {};
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	virtual ~Drawable() = default;

protected:
	void AddBind(std::unique_ptr<Bindable> bind);
	void AddIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept;

private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;

private:
	const IndexBuffer* pIndexBuffer = nullptr;
	std::string tag;
	std::vector<std::unique_ptr<Bindable>> binds;
};

