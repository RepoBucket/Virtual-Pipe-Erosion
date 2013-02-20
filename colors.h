#pragma once
#include "allegro5/allegro5.h"

namespace ColorMath
  {
  ALLEGRO_COLOR multiply(ALLEGRO_COLOR c1, const double &mult);
  ALLEGRO_COLOR multiply(ALLEGRO_COLOR c1, const ALLEGRO_COLOR &c2);
  ALLEGRO_COLOR operator* (ALLEGRO_COLOR c1, const double &mult);
  ALLEGRO_COLOR operator* (const double &mult, ALLEGRO_COLOR c1);
  ALLEGRO_COLOR operator* (ALLEGRO_COLOR c1, const ALLEGRO_COLOR &c2);
  ALLEGRO_COLOR lerp(ALLEGRO_COLOR c1, const ALLEGRO_COLOR &c2, const double &interpolate);
  }