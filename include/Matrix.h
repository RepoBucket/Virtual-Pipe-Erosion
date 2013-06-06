#ifndef ALWAY_MATRIX_H
#define ALWAY_MATRIX_H
namespace alway
{
	class Vec3;


	class Matrix
	{
	public:
		Matrix();
		void CreateFromQuatern(Vec3& vect, float scalar);
		static Matrix CreateMatrixFromQuatern(Vec3& vect, float scalar);
		float GetElement(int index);
		void SetElement(float amount, int index);
		void CreateFromTranslation(float x, float y, float z);
		Matrix operator*(Matrix& m);
		void Transpose();
		void Translate(Vec3 v);
		void SetIdentity();

	protected:
		float data[4][4];
	};
}

#endif