#pragma once
#include <math.h>
#include "Matrix.h"
#include "Defines.h"
#include "Quatern.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include <stdlib.h>

using namespace alway;

float GaussRand5Of1000();
Vec3 avg(Vec3& v1, Vec3& v2, Vec3& v3);
float dist(Vec3& v1, Vec3& v2);

void MaximizeAABB(Vec3* bounds, Vec3* newObj);
