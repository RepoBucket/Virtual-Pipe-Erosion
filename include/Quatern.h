#ifndef ALWAY_QUATERN_H
#define ALWAY_QUATERN_H
#include "Vec3.h"

namespace alway
{
	class Matrix;

	class Quatern
	{
	public:
		Quatern();
		Quatern(float i, float j, float k, float w);
		Quatern(const Quatern &from);
		~Quatern();

		void SetRotation(float axisX, float axisY, float axisZ, float radians);
		void SetRotation(Vec3 direction, float radians);
		void ApplyRotation(Quatern &second);

		void Invert();
		Quatern Inverse();

		void Normalize();

		Matrix GetMatrix();
		Vec3 TransformVector(Vec3 &point);
		Vec3 InverseTransformVector(Vec3& point);

		Quatern operator*(Quatern &q);
		Quatern operator*(float n);
		Quatern operator=(const Quatern &second);
		Quatern operator+(Quatern &q);
		Quatern operator/(float n);
		float& operator[](unsigned int index);

		float Dot(Quatern& q);
		float FindAngle(Quatern& q);

		bool IsZero();

		static Quatern FindRotationFrom2Vectors(Vec3 from, Vec3 to);
		static Quatern ApplyRotation(Quatern& first, Quatern& second);

		Quatern Interpolate(Quatern& q, float t);
		void Renormalize();
	protected:
		Vec3 vect;
		float scalar;
	};
}
#endif