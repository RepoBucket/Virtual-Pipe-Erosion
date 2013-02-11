#include "physicalvector.h"

#include <cmath>

vector3::vector3(): length(0)
  {x = y = z = 0; }

vector3::vector3(const double& x1,const double& y1, const double& z1)
  {
  x = x1;
  y = y1;
  z = z1;
  findLength();
  }

vector3::vector3(const vector3 & target)
  {
  x = target.x;
  y = target.y;
  z = target.z;
  length = target.length;
  }

void vector3::setVector(const double& x1, const double& y1, const double& z1, const double& x2, const double& y2, const double& z2)
	{
  x = x1 - x2;
  y = y1 - y2;
  z = z1 - z2;
  findLength();
	}

void vector3::setVector(const double& x1, const double& y1, const double& z1)
  {
  x = x1;
  y = y1;
  z = z1;
  findLength();
  }

void vector3::normalize()
	{
	//if the length of the vector is 0, you cannot normalize it. Therefore check that x and y are not zero.
	if (length != 0)
		{
		x = x / length;
    y = y / length;
    z = z / length;
    length = 1;
		}
	}

vector3& vector3::scale(const double& p)
	{
  x *= p;
  y *= p;
  z *= p;
	findLength();
  return *this;
	}

vector3& vector3::divide(const double& p)
  {
  x /= p;
  y /= p;
  z /= p;
	findLength();
  return *this;
  }

coord3 vector3::printvector()
	{
	return coord3(x,y,z);
	}

void vector3::moveByVector (double& x1, double& y1, double& z1)
	{
	x1 += x;
  y1 += y;
  z1 += z;
	}

double vector3::findLength()
	{
  length = sqrt( pow(x,2) + pow (y,2) + pow (z,2) );
	return length;
	}

vector3 &vector3::operator+= (const vector3 &addedvector)
	{
	x += addedvector.x;
  y += addedvector.y;
  z += addedvector.z;
  findLength();
	return *this;
	}

vector3 operator+ (vector3 leftsidevector, const vector3 &rightsidevector)
  {
  leftsidevector += rightsidevector;
  return leftsidevector;
  }

vector3 & vector3::operator-= (const vector3 &subtractingvector)
	{
	x -= subtractingvector.x;
  y -= subtractingvector.y;
  z -= subtractingvector.z;
  findLength();
	return *this;
	}

vector3 operator- (vector3 leftvector, const vector3 &rightvector)
  {
  leftvector -= rightvector;
  return leftvector;
  }

vector3 &vector3::operator*= (const double &var)
	{
	scale(var);
  findLength();
	return *this;
	}

vector3 & vector3::operator /= (const double &var)
  {    
  x /= var;
  y /= var;
  z /= var;
  findLength(); 
  return *this;
  }

vector3 operator* (vector3 leftside, const double &var)
	{
	leftside *= var;
	return leftside;
	}

vector3 operator* (const double& var, vector3 rightside)
  {
  rightside *= var;
  return rightside;
  }

vector3 operator / (vector3 leftvector, const double &var)
  {
  leftvector /= var;
  return leftvector;
  }

double findDistance(vector3 vector1, const vector3 &vector2)
  { 
  vector1 -= vector2; 
  return vector1.length;
  }

double vector3::dot(const vector3& left, const vector3& right)
  {
  return left.x * right.x + left.y * right.y + left.z * right.z;
  }

vector3 vector3::cross (vector3 left, const vector3& right)
  {
  left.setVector(
                  left.y * right.z - right.y * left.z, 
                  left.z * right.x - right.z * left.x,
                  left.x * right.y - right.x * left.y
                );
  return left;
  }

void vector3::clear()
  {
  x = y = z = length = 0;
  }