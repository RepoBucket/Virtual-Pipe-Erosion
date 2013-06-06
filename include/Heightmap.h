#pragma once
class BaseHeightmap;

class Heightmap
{
public:
	void ModifyHeights(float* heightData);
	void ModifyColors(float* colorData);
	void ModifyHeightsAndColors(float* heightData, float* colorData);
protected:
	BaseHeightmap* data;
	void operator= (BaseHeightmap* data);

	friend class BaseHeightmapRenderer;
};