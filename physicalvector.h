#pragma once

#include <utility>

using namespace std;

//Always assumes bound vectors.
struct coord3
  {
  coord3(const double& x1, const double& y1, const double& z1) {x = x1; y = y1; z = z1;}
  coord3(){}
  double x, y, z;
  };

class vector3
{
public:
	double x,y,z;
  
  vector3();
  vector3(const double& x1,const double& y1, const double& z1);
  vector3(const vector3 & target);

  double length;

	void setVector(const double& x1, const double& y1, const double& z1, const double& x2, const double& y2, const double& z2); // finds the vector from two coordinates
	void setVector(const double& x1, const double& y1, const double& z1);
  void normalize();
  void clear();
  vector3& scale(const double& p); //multiplies vector by p
  vector3& divide(const double& p); //divides by p, unless it is zero, in which case ignore.
	//void sum(const vector3& addedVector); //adds a vector to this class's vector

	coord3 printvector(); //prints coords

  void moveByVector (double& x1, double& y1, double& z1); // moves a point by this vector
	double findLength(); //get length of vector. Also sets it.
	
	static double dot(const vector3& left, const vector3& right); //dotsum of vector
	 
	vector3 & operator+= (const vector3 &addedvector);
  vector3 & operator-= (const vector3 &subtractingvector);
  vector3 & operator*= (const double &var);
  vector3 & operator /= (const double &var);
  //vector3 * divideby(double &var);
  static vector3 cross (vector3 left, const vector3& right);
};
