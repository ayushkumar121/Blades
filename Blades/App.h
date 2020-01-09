#pragma once
#include "Window.h"

#include "Sprite.h"
#include "BladesTimer.h"

#include "ImguiManager.h"
#include "Camera.h"
#include "Mesh.h"
#include "PointLight.h"
#include "Mesh.h"

class App
{
public:
	App();
	int Go();
	~App();

private:
	void DoFrame(); 

private:
	ImguiManager imgui;
	Window wnd;
	BladesTimer timer;
	Camera cam;
	PointLight light;

	Model cube;
};

