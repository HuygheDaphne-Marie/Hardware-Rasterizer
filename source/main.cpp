#include "pch.h"
//#undef main

//Standard includes
#include <iostream>

//Project includes
#include "ETimer.h"
#include "ERenderer.h"
#include "Vertex.h"
#include <vector>
#include "SceneManager.h"

using namespace Elite;

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;
	SDL_Window* pWindow = SDL_CreateWindow(
		"DirectX - Henri-Thibault Huyghe",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	auto pTimer{ std::make_unique<Elite::Timer>() };
	auto pRenderer{ std::make_unique<Elite::Renderer>(pWindow) };
	SceneManager& sceneManager{ SceneManager::GetInstance() };

	// Init Meshes
	SceneGraph& activeScene = sceneManager.GetActiveScene();
	activeScene.SetCamera(new Camera(width, height));
	{
		const std::vector<IVertex> vertices
		{
			{FPoint3{0.f, 3.f, 2.f}, RGBColor{1.f, 0.f, 0.f}},
			{FPoint3{3.f, -3.f, 2.f}, RGBColor{0.f, 0.f, 1.f}},
			{FPoint3{-3.f, -3.f, 2.f}, RGBColor{0.f, 1.f, 0.f}}
			//{FPoint3{0.f, 0.5f, 0.5f}, RGBColor{1.f, 0.f, 0.f}},
			//{FPoint3{0.5f, -0.5f, 0.5f}, RGBColor{0.f, 0.f, 1.f}},
			//{FPoint3{-0.5f, -0.5f, 0.5f}, RGBColor{0.f, 1.f, 0.f}}
		};
		const std::vector<uint32_t> indices{ 0, 1, 2 };
		
		activeScene.AddGeometryToScene(new Mesh(pRenderer->GetDevice(), vertices, indices, L"Resources/PosCol3D.fx"));
	}
	

	

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;

	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				break;
			}
		}

		// Update
		activeScene.GetCamera()->Update(pTimer->GetElapsed());

		//--------- Render ---------
		pRenderer->Render();

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "FPS: " << pTimer->GetFPS() << std::endl;
		}

	}
	pTimer->Stop();

	//Shutdown "framework"
	ShutDown(pWindow);
	return 0;
}