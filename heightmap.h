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
  double& at(const int &x, const int& y); //Act like <vector>'s at.

  void translate(const double &p);//Shift all numbers by p
  void scale(double lower, double upper); //stretch to fit within min, max.
  void multiply(const double& p);
  double findMin();
  double findMax();

  static ALLEGRO_COLOR lerp(const ALLEGRO_COLOR &c1, const ALLEGRO_COLOR &c2, const double &interpolate);
  static ALLEGRO_COLOR addColors(ALLEGRO_COLOR c1, const ALLEGRO_COLOR &c2);
  static ALLEGRO_COLOR multiplyColors(ALLEGRO_COLOR color, const double &mult);
  static ALLEGRO_COLOR multiplyColors(ALLEGRO_COLOR color1, const ALLEGRO_COLOR &color2);
  double ringAngularAverage(const double &x, const double &y, const double &ringSize);
  double findDistance(const double & x1,const double & y1, const double &x2, const double &y2);

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