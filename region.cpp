#include "region.h"

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
  double scaleFactor = (upper - lower) / findMax();
  multiply(scaleFactor); 
  translate(lower);
  }