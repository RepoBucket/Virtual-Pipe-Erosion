#pragma once
#include <allegro5/allegro.h>
#include <noise.h>
#include <vector>
#include <boost/random/mersenne_twister.hpp>

using namespace std;

struct heightmap //Doesn't have to store height info.
  {
  heightmap();
  heightmap(const double &width, const double& height, const double& lower, const double& upper);
  heightmap(const heightmap &othermap);
  void initMap(const double &width, const double& height); //Blank map and resize it
  void generate(const double &lower, const double &upper); // Generate a map from a perlin noisemap.
  double* at(const int &x, const int& y); //Act like <vector>'s at.

  void translate(const double &p);//Shift all numbers by p
  void scale(double lower, double upper); //stretch to fit within min, max.
  void multiply(const double& p);
  double findMin();
  double findMax();
  
  void setPerlinSeed(const int &newSeed);
  void setPerlinSeed(); //Random
  void setPerlinFrequency();
  void setPerlinFrequency(const double &newFreq);
  void setGenSeed(const int &newseed);
  void setGenSeed(); //Sets the twister's seed.

  void copy(const heightmap & othermap); //copies othermap into this object. Does not copy generators.
  static void rotate_point(const pair<double, double>& pivot, double& x, double& y, const double &angle);
  
  
  vector<double> infomap; 
  protected:
     
    noise::module::Perlin mapPerlin;
    boost::random::mt19937 gen;
    int seed;
    int w;
    int h;
  };

class region
  {
  public:
    region(const int &width, const int &height);
    void generateTopography();
    ALLEGRO_BITMAP *bitmap;
    void tempRender(); //roughly convert a heightmap to RGB and draw it on the bitmap.
    int w;
    int h;
  protected:
    heightmap topograph;
  };