#include "App.h"
#include <memory>

#include "imgui/imgui.h"

App::App()
	:
	wnd(1800, 1350, L"Blades3D Engine"),
	light(wnd.Gfx()),

	cube(wnd.Gfx(), "Models\\Primitives\\Cube.obj")
{
	wnd.DisableCursor();
	wnd.mouse.EnableRaw();

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 100.0f));
}

// Game logic here
void App::DoFrame()
{
	auto dt = timer.Mark();

	wnd.Gfx().BeginFrame(0.5f, 0.5f, 0.5f);
	wnd.Gfx().SetCamera(cam.GetMatrix());

	light.Bind(wnd.Gfx(), cam.GetMatrix());
	light.Draw(wnd.Gfx());

	cube.Draw(wnd.Gfx());

	while (const auto e = wnd.kbd.ReadKey())
	{
		if (e->IsPress())
		{
			continue;
		}

		switch (e->GetCode())
		{
		case VK_ESCAPE:
			if (wnd.CursorEnabled())
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
			break;

		default:
			break;
		}
	}

	if (!wnd.CursorEnabled())
	{
		//light.SetPos({ cam.GetPos().x * 2, cam.GetPos().y * 2, cam.GetPos().z});

		if (wnd.kbd.KeyIsPressed('W') || wnd.kbd.KeyIsPressed(VK_UP))
		{
			cam.Translate({ 0.0f, 0.0f, dt });
		}

		if (wnd.kbd.KeyIsPressed('A') || wnd.kbd.KeyIsPressed(VK_LEFT))
		{
			cam.Rotate(dt, 0.0f);
		}

		if (wnd.kbd.KeyIsPressed('S') || wnd.kbd.KeyIsPressed(VK_DOWN))
		{
			cam.Translate({ 0.0f, 0.0f, -dt });
		}

		if (wnd.kbd.KeyIsPressed('D') || wnd.kbd.KeyIsPressed(VK_RIGHT))
		{
			cam.Rotate(-dt , 0.0f);
		}
	}

	if (ImGui::Begin("Debug"))
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();

	cam.SpawnControlWindow();
	light.SpawnControlWindow();

	wnd.Gfx().EndFrame();
}

int App::Go()
{
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}

		DoFrame();
	}
}

App::~App()
{
}