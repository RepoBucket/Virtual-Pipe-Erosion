#ifndef ALWAY_VEC3_H
#define ALWAY_VEC3_H
#pragma warning(disable : 4521)
namespace alway
{
	class Vec2;

	class Vec3
	{
	public:
		~Vec3();
		Vec3();
		Vec3(const Vec3&);
		Vec3(double*);
		Vec3(float x, float y, float z);
		Vec3(Vec2& v1, float z);
		float &x, &y, &z;
		float xyz[3];
		Vec3 operator-(Vec3& v);
		Vec3 operator+(Vec3& v);
		Vec3 operator*(float f);
		Vec3 operator*(Vec3& v);
		Vec3 operator/(float f);
		Vec3& operator+=(Vec3& v);
		Vec3& operator-=(Vec3& v);
		Vec3& operator*=(Vec3& v);
		Vec3& operator*=(float f);
		Vec3& operator=(const Vec3& v);
		float& operator[](int x);
		void Normalize();
		Vec3 GetNormalized();
		float Length();
		float Length2();
		Vec3 Inv();
		bool IsZero();

		static Vec3 up;
	};
}
#pragma warning(default : 4521)
using namespace alway;	

Vec3 operator -(Vec3&v1);
float dot(Vec3& v1, Vec3& v2);
float dot(Vec3* v1, Vec3* v2);
Vec3 cross(Vec3& v1, Vec3& v2);
Vec3 cross(Vec3* v1, Vec3* v2);

/// <remarks>
/// You may have some additional information about this class.
/// </remarks>
void checkBounds(Vec3* v1, Vec3* bounds);


#endif

