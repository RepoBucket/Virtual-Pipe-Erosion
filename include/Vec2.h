#ifndef ALWAY_VEC2_H
#define ALWAY_VEC2_H

namespace alway
{
	class Vec2
	{
	public:
		Vec2();
		Vec2(Vec2&);
		Vec2(float x, float y);
		float x, y;
	};
}
using namespace alway;

#endif