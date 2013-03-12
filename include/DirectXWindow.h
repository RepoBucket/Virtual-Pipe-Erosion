#include "BuildOptions.h"
#pragma once
#ifdef DIRECTX_BUILD
#include "Window.h"

class BaseRenderer;
class Heightmap;

class DirectXWindow : public Window
{
public:
	DirectXWindow(string title, bool manualResize, int width, int height, int xOffset = 0, int yOffset = 0);

	void CreateHeightmap(Heightmap* heightmapOut, unsigned int width, unsigned int height, float* heightData = nullptr);
	void CreateColoredHeightmap(Heightmap* heightmapOut, unsigned int width, unsigned int height, float* heightData = nullptr, float* rgbColorData = nullptr);
	void CreateWaterHeightmap(Heightmap* heightmapOut, unsigned int width, unsigned int height, float* heightData = nullptr);

	virtual void step(float dt);
	virtual bool ProcessWindowMessages();

protected:
	void InitializeDirectX();

	BaseRenderer* renderer;
};

#endif