#include "colors.h"

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

    ALLEGRO_COLOR lerp(ALLEGRO_COLOR c1, const ALLEGRO_COLOR &c2, const double &interpolate)
      {
      c1.r *= 1 - interpolate;
      c1.g *= 1 - interpolate;
      c1.b *= 1 - interpolate;
      c1.a *= 1 - interpolate;
      c1.r += c2.r * interpolate;
      c1.b += c2.b * interpolate;
      c1.g += c2.g * interpolate;
      c1.a += c2.a * interpolate;
      return c1;
      }

    /*float3 lerp(float3 c1, const float3& c2, const float &interpolate)
      {
      float3 output;
      output.r = 
      c1.r *= 1 - interpolate;
      c1.g *= 1 - interpolate;
      c1.b *= 1 - interpolate;
      //c1.a *= 1 - interpolate;
      c1.r += c2.r * interpolate;
      c1.b += c2.b * interpolate;
      c1.g += c2.g * interpolate;
     // c1.a += c2.a * interpolate;
      return c1;
      }*/
  }