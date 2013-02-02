#pragma once
#include <allegro5/allegro.h>
//#include <noise.h>
//#include <vector>
//#include <boost/random/mersenne_twister.hpp>
#include "heightmap.h"


using namespace std;

class region
  {
  public:
    region(const int &width, const int &height);
    void generateTopography();
    ALLEGRO_BITMAP *landmap;
    void tempRender(); //roughly convert a heightmap to RGB and draw it on the bitmap.
    int w;
    int h;
  protected:
    heightmap topograph;
    heightmap glowmap;
    heightmap shallowaquifer;
    double glowMultiplier;
    ALLEGRO_COLOR skyColor;
    ALLEGRO_COLOR waterColor;

    void addShadows(heightmap &hmap); //smed
   // void addGlow(heightmap &hmap);
  };