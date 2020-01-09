#include "Camera.h"
#include "imgui/imgui.h"
#include <algorithm>

Camera::Camera() noexcept
{
	Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	using namespace DirectX;

	const auto camPosition = XMLoadFloat3(&pos);
	const auto lookVector = XMVector3Transform(
		XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
		XMMatrixRotationY(yaw)
	);

	return
		XMMatrixLookAtLH(
			camPosition,
			lookVector,
			XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
		);
}

DirectX::XMFLOAT3 Camera::GetPos() const noexcept
{
	return pos;
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -200.0f, 200.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -200.0f, 200.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -200.0f, 200.0f, "%.1f");
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f, "%.1f");

		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	pos = { 0.0f, 0.0f, -10.0f };
	yaw = 0.0f;
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	DirectX::XMStoreFloat3(&translation,
		DirectX::XMVector3Transform(
			DirectX::XMLoadFloat3(&translation),
			DirectX::XMMatrixRotationRollPitchYaw(0.0f, yaw, 0.0f) *
			DirectX::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)
		));

	pos =
	{
		pos.x + translation.x,
		pos.y + translation.y,
		pos.z + translation.z,
	};
}

void Camera::Rotate(float dx, float dy) noexcept
{
	yaw = wrap_angle(yaw + dx * rotationSpeed);
}
