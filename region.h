#pragma once
#include <allegro5/allegro.h>
#include <noise.h>
#include <vector>

using namespace std;

struct heightmap //Doesn't have to store height info.
  {
  void initMap(const double &x, const double& y); //Blank map and resize it
  void translate(const double &p);//Shift all numbers by p
  void scale(double lower, double upper); //stretch to fit within min, max.
  double findMin();
  double findMax();
  double* at(const int &x, const int& y); //Act like <vector>'s at.
  void multiply(const double& p);

  protected:
    vector<double> infomap;
    noise::module::Perlin Perlin;
  };

class region
  {
  public:
    ALLEGRO_BITMAP *map;
  protected:
  };