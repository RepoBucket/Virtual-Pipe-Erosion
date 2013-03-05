#ifndef ALWAY_VEC4_H
#define ALWAY_VEC4_H

namespace alway
{
	class Vec4
	{
	public:
		Vec4();
		Vec4(Vec4&);
		Vec4(float x, float y, float z, float w);
		float x,y,z,w;
	};
}
using namespace alway;
#endif