#pragma once
#include "Bindable.h"
#include <vector>

// Ordered list of indesxes that are to be bounded;

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices);
	void Bind(Graphics& gfx) noexcept override;
	UINT GetCount() const noexcept;
protected:
	UINT count;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
};

