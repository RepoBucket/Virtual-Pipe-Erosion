#include "heightmap.h"
#include <boost/random/uniform_int_distribution.hpp>
#include <limits>

#undef max
#undef min

heightmap::heightmap()
  {
  setGenSeed();
  setPerlinSeed();
  setPerlinFrequency();
  w=h=0;
  }

heightmap::heightmap(const double &width, const double& height, const double& lower, const double& upper)
  {
  setGenSeed();
  setPerlinSeed();
  setPerlinFrequency();
  generate(lower, upper);
  w = width;
  h = height;
  }

heightmap::heightmap(const heightmap &othermap)
  {
  copy(othermap);
  setGenSeed();
  setPerlinSeed(seed);
  setPerlinFrequency();
  }

void heightmap::initMap(const double &width, const double& height)
  {
  infomap.clear();
  w = width;
  h = height;

  try {
  for (int counter = 0; counter < w * h; counter++)
    infomap.push_back(0);
    }
  catch (bad_alloc& ba)
    {
    cerr << "bad_alloc caught: " << ba.what() << endl;
    }

  }

void heightmap::setGenSeed()
  {
  gen.seed(time(0));
  }

void heightmap::setGenSeed(const int &newseed)
  {
  gen.seed(newseed);
  }

void heightmap::scale(double lower, double upper)
  {
  //check for max/min
  if (lower > upper)
    {
    double temp = lower;
    lower = upper;
    upper = temp;
    }
  
  //First find min. Translate map to min.
  translate(-1 * findMin());
  //Min is now 0.

  double scaleFactor;
  if (findMax() == 0)
    scaleFactor = 0;
  else
    scaleFactor = (upper - lower) / findMax();

  multiply(scaleFactor); 
  translate(lower);
  }

double heightmap::findMin()
  {
  double min = numeric_limits<double>::max();
  for (auto it = infomap.begin(); it < infomap.end(); it++)
    min = min > *it ? *it : min; //If *it is smaller than min, assign *it.

  return min;
  }

double heightmap::findMax()
  {
  double max = -1 * numeric_limits<double>::max();
  for (auto it = infomap.begin(); it < infomap.end(); it++)
    max = max < *it ? *it : max; //If *it is larger than max, assign *it.

  return max;
  }

void heightmap::translate(const double &p)
  {
  for (auto it = infomap.begin(); it < infomap.end(); it++)
    *it += p;
  }

void heightmap::multiply(const double &p)
  {
  for (auto it = infomap.begin(); it < infomap.end(); it++)
    *it *= p;
  }

double& heightmap::at(const int &x, const int& y)
  {
  return infomap.at(y * h + x);
  }

void heightmap::setPerlinSeed()
  {
  boost::random::uniform_int_distribution<> dist(0, numeric_limits<int>::max());

  mapPerlin.SetSeed(dist(gen));
  }

void heightmap::setPerlinSeed(const int &newseed)
  {
  mapPerlin.SetSeed(newseed);
  }

void heightmap::generate(const double &lower, const double &upper)
  {
  pair<double,double> pivot;
  pivot.first = 0.5;
  pivot.second = 0.5;
  double angle = 0.01425728;

  double x; double y;

  for (int xcounter = 0; xcounter < w; xcounter++) //x
    for (int ycounter = 0; ycounter < h; ycounter++)
      {
      x = xcounter / (double)w / 50.0f;
      y = ycounter /(double)h / 50.0f;
      rotate_point(pivot, x, y, angle);
      at(xcounter, ycounter) = mapPerlin.GetValue(x, y, 1.23);
      }
  scale(lower, upper);
  }

void heightmap::copy(const heightmap &othermap)
  {
    infomap = othermap.infomap;
    seed = othermap.seed;
    w = othermap.w;
    h = othermap.h;
  }

void heightmap::rotate_point(const pair<double, double>& pivot, double& x, double& y, const double &angle)
  {
  double sine = sin(angle);
  double cosine = cos(angle);

  //translate to origin
  x -= pivot.first;
  y -= pivot.second;

  //rotate
  double newx = x * cosine - y * sine;
  double newy = x * sine - y * cosine;

  //translate back.
  x = newx + pivot.first;
  y = newy + pivot.second;
  }

void heightmap::setPerlinFrequency()
  {
  mapPerlin.SetFrequency(100);
  mapPerlin.SetOctaveCount(10);
  }

void heightmap::setPerlinFrequency(const double& newFreq)
  {
  mapPerlin.SetFrequency(newFreq);
  }