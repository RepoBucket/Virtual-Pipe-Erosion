#pragma once

#include <string>
#include "AlwayMath.h"
#include <vector>
using namespace alway;
using namespace std;



class MatData
{
public:
	~MatData();
	MatData(const MatData& m);
	MatData(Vec3 c, float t, float r);
	MatData();
	Vec3 color;
	float transparency;
	float reflectivity;
protected:

};


class Material
{
public:
	Material();
	Material(const Material& m);
	virtual bool LoadFromFile(string filename);
	virtual MatData Sample(Vec3 uv);
	static void LoadAllMaterials(vector<Material>* materials);
protected:
	MatData mat;
};