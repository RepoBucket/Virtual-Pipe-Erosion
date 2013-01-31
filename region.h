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
    ALLEGRO_BITMAP *bitmap;
    void tempRender(); //roughly convert a heightmap to RGB and draw it on the bitmap.
    int w;
    int h;
  protected:
    heightmap topograph;
    heightmap glowmap;
    double glowMultiplier;
    ALLEGRO_COLOR skyColor;

    void addShadows(heightmap &hmap); //smed
    void addGlow(heightmap &hmap);
  };