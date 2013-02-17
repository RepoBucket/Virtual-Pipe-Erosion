#pragma once
#include "allegro5/allegro5.h"

namespace ColorMath
  {
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
  }