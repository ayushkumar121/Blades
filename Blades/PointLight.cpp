#include "PointLight.h"
#include "imgui/imgui.h"

PointLight::PointLight(Graphics& gfx)
	:
	cbuf(gfx, 0u),
	gizmo(gfx, true)
{
	Reset();
}

void PointLight::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Light"))
	{
		ImGui::Checkbox("Show gizmo", &showGizmo);

		ImGui::Text("Position");
		ImGui::SliderFloat("X", &cbData.pos.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &cbData.pos.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &cbData.pos.z, -60.0f, 60.0f, "%.1f");

		ImGui::Text("Intensity/Color");
		ImGui::SliderFloat("Intensity", &cbData.diffuseIntensity, 0.01f, 5.0f, "%.2f", 2);
		ImGui::ColorEdit3("Diffuse Color", &cbData.diffuseColor.x);
		ImGui::ColorEdit3("Ambient", &cbData.ambient.x);

		ImGui::Text("Falloff");
		ImGui::SliderFloat("Constant", &cbData.attConst, 0.05f, 10.0f, "%.2f", 4);
		ImGui::SliderFloat("Linear", &cbData.attLin, 0.0001f, 4.0f, "%.4f", 8);
		ImGui::SliderFloat("Quadratic", &cbData.attQuad, 0.0000001f, 10.0f, "%.7f", 10);

		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	showGizmo = true;
	cbData = {
		{0.0f,  0.0f, -10.0f },
		{0.05f, 0.05f, 0.05f },
		{1.0f,  1.0f,  1.0f  },
		1.5f,
		1.0f,
		0.045f,
		0.0075f,
	};
}

void PointLight::Draw(Graphics& gfx) const
{
	gizmo.SetPos(cbData.pos);
	if(showGizmo)
		gizmo.Draw(gfx);
}

void PointLight::Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept
{
	auto dataCopy = cbData;
	const auto pos = DirectX::XMLoadFloat3(&cbData.pos);
	DirectX::XMStoreFloat3(&dataCopy.pos, DirectX::XMVector3Transform(pos, view));

	cbuf.Update(gfx, dataCopy);
	cbuf.Bind(gfx);
}

void PointLight::SetPos(DirectX::XMFLOAT3 pos) const
{
	cbData.pos = { pos.x, pos.y, pos.z };
}
