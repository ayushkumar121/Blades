#pragma once
#include "Graphics.h"
#include "BladesMaths.h"


class Camera
{
public:
	Camera() noexcept;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	DirectX::XMFLOAT3 GetPos() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;

	void Translate(DirectX::XMFLOAT3 translation) noexcept;
	void Rotate(float dx, float dy) noexcept;

private:
	DirectX::XMFLOAT3 pos;
	float yaw;

	static constexpr float travelSpeed = 3.0f;
	static constexpr float rotationSpeed = 1.0f;
};

