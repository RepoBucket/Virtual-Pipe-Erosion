#pragma once

#include <utility>

using namespace std;

/**This struct is used to bundle a vector3's x, y, z coordinates are return them as a single return value. All members are public. All members may be accessed by anyone.**/
struct coord3
  {
  /**The default constructor. x, y, z are initialized to 0.*/
  coord3(): x(0), y(0), z(0) {}
  /**x, y, z are initialized to x1, y1, z1, respectively.**/
  coord3(const double& x1, const double& y1, const double& z1): x(x1), y(y1), z(z1) {}

  double x, y, z;
  };

/**
The vector3 class is a three dimensional vector that assumes all vectors are bound vectors. It has functions for most common usages, including scaling, adding, crossing, and dotting. The length of the vector is calculated after any arithmetic function. Operators are overloaded, but binary operators are not, due to unresolved issues. Any help would be grateful.

Directly accessing x, y, and z is not recommended.
**/

class vector3
{
public:
  double x,y,z;

   /**The default constructor. x,y,z, and length are initialized as 0.**/
  vector3();
  /**The contents of vector3 are assigned as x1, y1, and z1, and the length is calculated.**/
  vector3(const double& x1,const double& y1, const double& z1);
  /**target is copied to vector3. The length is not recalculated, instead copying target's length.**/
  vector3(const vector3 & target);

  double length;

  /**The vector is calculated from (x1,y1,z1) to (x2,y2,z2). findLength() is called.**/
  void setVector(const double& x1, const double& y1, const double& z1, const double& x2, const double& y2, const double& z2); // finds the vector from two coordinates
  /**The vector is calculated from (0,0,0) to (x1,y1,z1). findLength() is called.**/
  void setVector(const double& x1, const double& y1, const double& z1);
  /**The vector is normalized, by dividing each coordinate by the length. The length is then set to 1. If the length is 0, no operations are performed.**/
  void normalize();
  /**x,y,z,length are all set to 0.**/
  void clear();
  /**x,y,z are all multiplied by p. The length is then calculated by calling findLength(). This function returns *this.**/
  vector3& scale(const double& p); //multiplies vector by p
  /**x,y,z are all divided by p. The length is then calculated by calling findLength(). No checking is done for when p == 0; x,y,z and length will all be assigned as #INF if that is the case.**/
  vector3& divide(const double& p); //divides by p, unless it is zero, in which case ignore.

	/**x,y,z are returned as a coord3.**/
  coord3 printvector(); //prints coords

  /**It effectively moves a particle (x1,y1,z1) by the vector, assigning (x1+x, y1+y, z1+z) to the respective arguments.**/
  void moveByVector (double& x1, double& y1, double& z1) const; // moves a point by this vector
  /**Same as the double version, but accepts a coord3.**/
  void moveByVector (coord3& coord) const;
  /**The length of the vector is calculated. This function also returns a double.**/
  double findLength(); //get length of vector. Also sets it.

	/**The two vectors are dotted, and the dot product is returned.**/
  static double dot(const vector3& left, const vector3& right); //dotsum of vector
  /**The two vectors are crossed, and the cross product is returned.**/
  static vector3 cross (vector3 left, const vector3& right);

  /**+= is overloaded. Adds addedvector's x, y, z to this vector's x, y, z, and calls findLength(). This operator returns *this. **/
  vector3 & operator+= (const vector3 &addedvector);
  /**-= is overloaded. This function is like +=, but subtracts instead of adding.**/
  vector3 & operator-= (const vector3 &subtractingvector);
  /** *= is overloaded. This function is equivalent to this->scale(var).**/
  vector3 & operator*= (const double &var);
  ///** */= is overloaded. This function is equivalent to this->divide(var).**/
  vector3 & operator /= (const double &var);

  /** sum of two vectors, implemented using += **/
  vector3 operator+(const vector3 &right) const;
  /** difference of two vectors, implemented using -= **/
  vector3 operator-(const vector3 &right) const;
  vector3 operator*(const double &right) const;
  vector3 operator/=(const double& right) const;
};