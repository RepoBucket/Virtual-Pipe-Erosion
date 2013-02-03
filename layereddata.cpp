#include "layereddata.h"
#include "heightmap.h"
#include <cmath>

cell::cell()
  :fluid(0), updated(false)
  {}

double transientLayer::getHeight()
  {
  double height = 0;
  for (int counter = 0; counter < heights.size(); counter++)
    {
    height += heights.at(counter);
    }
  return height;
  }

double cell::getHeight()
  {
  height = 0;
  for (int counter = 0; counter < layers.size(); counter++)
    {
    height += layers.at(counter).height;
    }
  height += sediments.getHeight();
  height += fluid;
  return height;
  }

ErosionHeightmap::ErosionHeightmap(const int& width, const int& height)
  : w(width), h(height), thresholdSpeed(3)
  {
  cell buffercell;

  for(int xcounter = 0; xcounter < w; xcounter++)
    for (int ycounter = 0; ycounter < h; ycounter++)
      {
      heightmap1.push_back(buffercell);
      heightmap2.push_back(buffercell);
      }
    heightmap2 = heightmap1;
    currentMap = &heightmap1;
    writeToMap = &heightmap2;
    currentQueue = &updateQueue1;
    writeToQueue = &updateQueue2;
  }

void ErosionHeightmap::generate(const int &layers)
  {
  heightmap perlinInfo;
  perlinInfo.initMap(w,h);
  heightmap whichLayer; //Used to tell what material is in what layer.
  whichLayer.initMap(w,h);
  whichLayer.generate(1,3.9); //1~3 when floored.
  layer bufferlayer;

  for (int counter = 0; counter < layers; counter++)
    {
    perlinInfo.generate(0,10);
    for (int xcounter = 0; xcounter < w; xcounter++)
      for (int ycounter = 0; ycounter < w; ycounter++)
        {
        bufferlayer.height = perlinInfo.at(xcounter, ycounter);
        bufferlayer.mat = matDict.lookup((int)whichLayer.at(xcounter, ycounter));
        at(xcounter, ycounter).layers.push_back(bufferlayer);
        }
    }
  }

cell& ErosionHeightmap::at(const int &x, const int &y)
  {
  if (x >= 0 && x < w && y >= 0 && y < h)
    return currentMap->at(y * h + x);
  else
    return nullcell;
  }

cell& ErosionHeightmap::write(const int &x, const int &y)
  {
  if (x >= 0 && x < w && y >= 0 && y < h)
    return writeToMap->at(y * h + x);
  else
    return nullcell;
  }

map<int, material>::iterator materialDictionary::lookup(const int &ID)
  {
  map<int, material>::iterator returnIt = dict.find(ID);
  return returnIt;
  }

void ErosionHeightmap::addWater(const int& x, const int& y, const double& fluid)
  {
  at(x, y).fluid += fluid;
  currentQueue->push(pair<int,int>(x, y));
  }

void ErosionHeightmap::swapMaps()
  {
  vector<cell> * temp;
  temp = currentMap;
  currentMap = writeToMap;
  writeToMap = temp;
  }

void ErosionHeightmap::swapQueues()
  {
  queue<pair<int, int>>* temp;
  temp = currentQueue;
  currentQueue = writeToQueue;
  writeToQueue = currentQueue;
  }

double ErosionHeightmap::deltaIn(cell& thisCell, const cell& neighborCell)
  {
  double returnval = adjustedHeight(thisCell) - neighborCell.height + neighborCell.fluid;
  if (returnval < 0)
    return 0;
  else 
    return returnval;
  }

double ErosionHeightmap::adjustedHeight(cell & input)
  {
  // I(x,y) = H(x,y) - Kf * F(x,y)
  return input.getHeight() - (0 > 1 - 0.05 * input.velocity.length ? 1 : input.velocity.length)
           * input.fluid;
  }

vector3 ErosionHeightmap::averageGradient(cell & input, const int& x, const int& y)
  {
  vector3 topleft, top, topright, left, right, bottomleft, bottom, bottomright;

  // Per the paper.
  double In = deltaIn(input, at(x-1, y-1));
    if (In != 0)
      topleft.setVector(+1 * In, -1 * In, 0);
  
  In = deltaIn(input, at(x, y-1));
    if (In != 0)
      top.setVector(0, -1 * In, 0);

  In = deltaIn(input, at(x+1, y-1));
    if (In != 0)
      topright.setVector(-1 * In , -1 * In, 0);

  In = deltaIn(input, at(x-1, y));
    if (In != 0)
      left.setVector(1 * In, 0, 0);

  In = deltaIn(input, at(x+1, y));
    if (In != 0)
      right.setVector(-1 * In, 0, 0);

  In = deltaIn(input, at(x-1, y+1));
    if (In != 0)
      bottomleft.setVector(1 * In, -1 * In, 0);

  In = deltaIn(input, at(x, y+1));
    if (In != 0)
      bottom.setVector(0, In, 0);

  In = deltaIn(input, at(x, y+1));
    if (In != 0)
      bottomright.setVector(-1 * In, In, 0);

    int counter = 0;
    vector3 output;
  if (topleft.length != 0)
    {
    counter++;
    output += topleft;
    }
  if (top.length != 0)
    {
    counter++;
    output += top;
    }
  if (topright.length != 0)
    {
    counter++;
    output += topright;
    }
  if (left.length != 0)
    {
    counter++;
    output += left;
    }
  if (bottomleft.length != 0)
    {
    counter++;
    output += bottomleft;
    }
  if (bottom.length != 0)
    {
    counter++;
    output += bottom;
    }
  if (bottomright.length != 0)
    {
    counter++;
    output += bottomright;
    }
  output /= counter;
  return output;
  }

/*vector3 ErosionHeightmap::normal(const int& x, const int& y)
  {
  vector3 horizontal, vertical;
  vector3 *phor, *pvec; //To use in case the destination doesn't exist.

  //first the horizontal.
  cell buffer = at(x-1, y);
  cell otherbuffer = at(x+1,y);
  horizontal.setVector(buffer.x, buffer.y, buffer.height, otherbuffer.x, otherbuffer.y, otherbuffer.height);
  //the the vertical
  buffer = at(x, y+1);
  other = at(x, y-1);
  vertical.setVector(buffer.x, buffer.y, buffer.height, otherbuffer.x, otherbuffer.y, otherbuffer.height); 

  return vector3::cross(horizontal, vertical);
  }*/

vector3 ErosionHeightmap::normal(const vector3& gradient)
  {
  vector3 horizontal, vertical;
  horizontal.setVector(gradient.x,0,0);
  vertical.setVector(0,gradient.y,0);
  return vector3::cross(horizontal, vertical);
  }

void ErosionHeightmap::resetUpdatedFlags()
  {
  int maxcounter = currentQueue->size();
  pair<int,int> current;

  for (int counter = 0; counter < maxcounter; counter++)
    {
    current = currentQueue->front();
    currentQueue->pop();
    at(current.first, current.second).updated = false;
    currentQueue->push(current);
    }
  }

void ErosionHeightmap::dampVelocity()
  {
  int maxcounter = currentQueue->size();
  pair<int,int> current;

  for (int counter = 0; counter < maxcounter; counter++)
    {
    current = currentQueue->front();
    currentQueue->pop();
    at(current.first, current.second).velocity *= 0.7f; // 0.7 = dampening vector.
    currentQueue->push(current);
    }
  }

//Distribute cell contents into the neighboring four cells.
void ErosionHeightmap::distribute(cell& thisCell, const pair<double, double>& coords)
  {
 // pair<double, double> baseCoords, topright, bottomleft, bottomright;
  double topleft, topright, bottomleft, bottomright; //are areas
  pair<double, double> baseCoords;
  baseCoords.first = (int)coords.first; //Basically floors the variables.
  baseCoords.second = (int)coords.second;
  
 /* topright = baseCoords;
  topright.first += 1;

  bottomleft = baseCoords;
  bottomleft.second += 1;

  bottomright = baseCoords;
  bottomright.second += 1;
  bottomright.first += 1;*/

  //Find the x, y distances from the point to the centers of the surrounding cells. Centers equal coords
  //topleft
  topleft = (coords.first - baseCoords.first) * (coords.second - baseCoords.second);
  topright = (baseCoords.first + 1 - coords.first) * (baseCoords.second - coords.second);
  bottomleft = (baseCoords.first - coords.first) * (baseCoords.second + 1 - coords.second);
  bottomright = (baseCoords.first + 1 - coords.first) * (baseCoords.second + 1 - coords.second);
  double sum = topleft + topright + bottomleft + bottomright;
  // get percentages
  topleft /= sum;
  topright /= sum;
  bottomleft /= sum;
  bottomright /= sum;
  topleft *= thisCell.fluid;
  topright *= thisCell.fluid;
  bottomleft *= thisCell.fluid;
  bottomright *= thisCell.fluid;
  if (topleft > 0)
    {
    write(baseCoords.first, baseCoords.second).inputs.push_back(pair<vector3, double>(thisCell.velocity, topleft));
    writeToQueue->push(baseCoords);
    }
  if (topright > 0)
    {
    write(baseCoords.first + 1, baseCoords.second).inputs.push_back(pair<vector3, double>(thisCell.velocity, topright));
    writeToQueue->push(pair<int,int>(baseCoords.first + 1, baseCoords.second));
    }
  if (bottomleft > 0)
    {
    write(baseCoords.first, baseCoords.second + 1).inputs.push_back(pair<vector3, double>(thisCell.velocity, bottomleft));
    writeToQueue->push(pair<int,int>(baseCoords.first, baseCoords.second + 1));
    }
  if (bottomright > 0)
    {
    write(baseCoords.first + 1, baseCoords.second + 1).inputs.push_back(pair<vector3, double>(thisCell.velocity, bottomright));
    writeToQueue->push(pair<int,int>(baseCoords.first + 1, baseCoords.second + 1));
    }
  }

void ErosionHeightmap::distributeByGradient(cell & input, const int& x, const int& y)
  {
  vector3 topleft, top, topright, left, right, bottomleft, bottom, bottomright;

  // Per the paper.
  double In = deltaIn(input, at(x-1, y-1));

    if (In != 0)
      topleft.setVector(+1 * In, -1 * In, 0);
  
  In = deltaIn(input, at(x, y-1));
    if (In != 0)
      top.setVector(0, -1 * In, 0);

  In = deltaIn(input, at(x+1, y-1));
    if (In != 0)
      topright.setVector(-1 * In , -1 * In, 0);

  In = deltaIn(input, at(x-1, y));
    if (In != 0)
      left.setVector(1 * In, 0, 0);

  In = deltaIn(input, at(x+1, y));
    if (In != 0)
      right.setVector(-1 * In, 0, 0);

  In = deltaIn(input, at(x-1, y+1));
    if (In != 0)
      bottomleft.setVector(1 * In, -1 * In, 0);

  In = deltaIn(input, at(x, y+1));
    if (In != 0)
      bottom.setVector(0, In, 0);

  In = deltaIn(input, at(x, y+1));
    if (In != 0)
      bottomright.setVector(-1 * In, In, 0);

    double magnitudeSum = topleft.length + top.length + topright.length + 
                          left.length + right.length + bottomleft.length +
                          bottom.length + bottomright.length;
    pair<vector3, double> buffer;

    //Split the fluid and push it.
    if (topleft.length != 0) {
      buffer.first = topleft; buffer.second = topleft.length / magnitudeSum * input.fluid;
      write(x-1, y-1).inputs.push_back(buffer);
      writeToQueue->push(pair<int, int>(x-1, y-1));
      }

    if (top.length != 0) {
      buffer.first = top; buffer.second = top.length / magnitudeSum * input.fluid;
      write(x, y-1).inputs.push_back(buffer);
      writeToQueue->push(pair<int, int>(x, y-1));
      }

    if (topright.length != 0) {
      buffer.first = topright; buffer.second = topright.length / magnitudeSum * input.fluid;
      write(x+1, y-1).inputs.push_back(buffer);
      writeToQueue->push(pair<int, int>(x+1, y-1));
      }

    if (left.length != 0) {
      buffer.first = left; buffer.second = left.length / magnitudeSum * input.fluid;
      write(x-1, y).inputs.push_back(buffer);
      writeToQueue->push(pair<int, int>(x-1, y));
      }

    if (right.length != 0) {
      buffer.first = right; buffer.second = right.length / magnitudeSum * input.fluid;
      write(x+1, y).inputs.push_back(buffer);
      writeToQueue->push(pair<int, int>(x+1, y));
      }

    if (bottomleft.length != 0) {
      buffer.first = bottomleft; buffer.second = bottomleft.length / magnitudeSum * input.fluid;
      write(x-1, y+1).inputs.push_back(buffer);
      writeToQueue->push(pair<int, int>(x-1, y+1));
      }

    if (bottom.length != 0) {
      buffer.first = bottom; buffer.second = bottom.length / magnitudeSum * input.fluid;
      write(x, y+1).inputs.push_back(buffer);
      writeToQueue->push(pair<int, int>(x, y+1));
      }

    if (bottomright.length != 0) {
      buffer.first = bottomright; buffer.second = bottomright.length / magnitudeSum * input.fluid;
      write(x+1, y+1).inputs.push_back(buffer);
      writeToQueue->push(pair<int, int>(x+1, y+1));
      }
  }

void ErosionHeightmap::step()
  {
  //For each cell that has water in it...
  pair<int, int> current;
  pair<double, double> movedCoords;
  cell currentcell;
  vector3 acceleration;

  while (!currentQueue->empty())
    {
      current = currentQueue->front();
      currentcell = at(current.first, current.second);

      //If the speed is above the threshold, get the average gradient.
        // Drop cells whose H+F is higher than this cell's H+F.
        // Average the gradient vectors.
    if (!currentcell.updated) //if this cell has already been updated, skip it.
      {
      if (currentcell.velocity.length >= thresholdSpeed)
        {
        // add the acceleration...
        acceleration = averageGradient(currentcell, current.first, current.second) * -1.0;
        acceleration = normal(acceleration);
        // Equation 16 below.
        acceleration.setVector(acceleration.x * -1, acceleration.y * -1, -1 * acceleration.x * acceleration.x - acceleration.y * acceleration.y);
        // Equation 18 below.
        acceleration = acceleration.z / acceleration.length * 9.81 * acceleration / acceleration.length;
        // add velocity and acceleration.
        currentcell.velocity += acceleration;
         // After dividing the fluid into the neighboring cells, also divide the velocity and push it
            // onto the cell's input vector. Multiplied by the fluid amount.
        //This function also pushes coords onto the write-to queue.
        // First, add the velocity to this cell's coords.
        movedCoords.first = current.first;
        movedCoords.second = current.second;
        movedCoords.first += currentcell.velocity.x;
        movedCoords.second += currentcell.velocity.y;
        distribute(currentcell, movedCoords);
        }
      else
        {
      //If the velocity is below the threshold, proportion the water and move them accordingly.
         // Again, ignore cells whose H+F is higher than this cell's H+F
         // When each gradient is found, proportion the fluid according to each gradient's magnitude.
         // Move.
            // Push the velocity onto the cell's input vector, multiplied by the fluid amount.
        distributeByGradient(currentcell, current.first, current.second); 
        }
      at(current.first, current.second).updated = true;
      }
    // For each cell that gets water, push it onto the write queue
    // Pop from queue.
      currentQueue->pop();
    }

  //swap the pointers.
  swapMaps();
  swapQueues();
  //reset updated flags
  resetUpdatedFlags();
  //Damped.
  dampVelocity();
  }