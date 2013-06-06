#ifndef ALWAY_RAY_H
#define ALWAY_RAY_H
#include "AlwayMath.h"
using namespace alway;
class MatData;

class Ray
{
public:
	~Ray();
	Ray();
	Ray(const Ray&);

	Ray operator=(Ray& r);

	float FindRayPlaneIntersect(Vec3* normal, Vec3* point);
	void BlendInColor(float alpha, Vec3 color);
	void GetColor(float* out);
	bool IsSaturated();
	void DiffuseReflection(Vec3 normal, int depth);		//monte-carlo rejection sampled direction change
	void SpecularReflection(Vec3 normal);				//reflect across given normal
	int PhotonReflection(MatData* matData, Vec3 normal, Vec3 position);

	int HeightmapIntersectionTest(int size, float* heightmap);
	float HeightmapIntersectionDistance(int size, float* heightmap);
	float HeightmapIntersectionDistance(int size, float* heightmap, float* lowRes1, float* lowRes2, float* lowRes3);

	Vec3 origin;
	Vec3 direction;
	Vec3 dirInv;
	Vec3 color;
	bool positive;
protected:
	float saturation;
	int HeightmapIntersectionTestGetNextTileIndex(int size);
	int HeightmapIntersectionTestGetNextTileIndex(int size, int scale);
	int HeightmapIntersectionTestGetThisTile(int size, int scale);
	void HeightmapIntersectionTestGotoFirstCell(int size);
};
#endif
