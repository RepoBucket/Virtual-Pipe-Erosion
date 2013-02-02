#include "region.h"
#include <limits>
#include <time.h>
#include <cstdlib>

ALLEGRO_COLOR multiply(ALLEGRO_COLOR c1, const double &mult)
  {
  c1.r *= mult;
  c1.g *= mult;
  c1.b *= mult;
  c1.a *= mult;
  return c1;
  }

ALLEGRO_COLOR multiply(ALLEGRO_COLOR c1, const ALLEGRO_COLOR &c2)
  {
  c1.r *= c2.r;
  c1.g *= c2.g;
  c1.b *= c2.b;
  c1.a *= c2.a;
  return c1;
  }

ALLEGRO_COLOR operator* (ALLEGRO_COLOR c1, const double &mult)
  {
  return multiply(c1, mult);
  }

ALLEGRO_COLOR operator* (const double &mult, ALLEGRO_COLOR c1)
  {
  return multiply(c1, mult);
  }

ALLEGRO_COLOR operator* (ALLEGRO_COLOR c1, const ALLEGRO_COLOR &c2)
  {
  return multiply(c1, c2);
  }


///////////////////////////
//
//
//
//
///////////////////////////

region::region(const int &width, const int &height)
  {
  landmap = al_create_bitmap(width, height);
  w = width;
  h = height;
  waterColor = al_map_rgb(32, 107, 164);
  }

void region::tempRender()
  {
  heightmap copymap;

  copymap.copy(topograph);
  copymap.scale(0,255);

  ALLEGRO_COLOR sandcolor;
  sandcolor = al_map_rgb(194, 178, 128);

  al_set_target_bitmap(landmap);
  al_lock_bitmap(al_get_target_bitmap(), al_get_bitmap_format(landmap), ALLEGRO_LOCK_READWRITE);

  for (int ycounter = 0; ycounter < w; ycounter++)
    for (int xcounter = 0; xcounter < h; xcounter++)
      {
      al_put_pixel(xcounter, ycounter, sandcolor);
      }

  al_unlock_bitmap(al_get_target_bitmap());

  addShadows(copymap);
  }

void region::generateTopography()
  {
  topograph.initMap(w, h);
  topograph.generate(1,30);

  shallowaquifer.initMap(w, h);
  shallowaquifer.generate(0,30);
  }

void region::addShadows(heightmap &hmap)
  {
  double drop = 0.01;
  double max_height = 0;
  double height = 0;
  double shadowfactor = 0;
  ALLEGRO_COLOR colorbuffer;
  ALLEGRO_COLOR black = al_map_rgb(80,80,80);

  al_set_target_bitmap(landmap);
  al_lock_bitmap(al_get_target_bitmap(), al_get_bitmap_format(landmap), ALLEGRO_LOCK_READWRITE);

  for (int ycounter = 0; ycounter < h; ycounter++)
    {
    max_height = 0;
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      height = topograph.at(xcounter, ycounter);
      if (height >= max_height)
        max_height = height;
      else
        {
        colorbuffer = al_get_pixel(landmap, xcounter, ycounter);
        shadowfactor = (max_height - height) <= 0 ? 0 : 1 / (max_height - height);
        if (shadowfactor <= 0.8)
          shadowfactor+= 0.2;
        shadowfactor = shadowfactor > 1 ? 1 : shadowfactor;
        colorbuffer = colorbuffer * (black * shadowfactor);
        al_put_pixel(xcounter, ycounter, colorbuffer);
        max_height = max_height-drop > height ? max_height-drop : height;
        }
      }
    }

  al_unlock_bitmap(al_get_target_bitmap());
  }