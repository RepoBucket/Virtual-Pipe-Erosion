#include "layereddata.h"
//#include "heightmap.h"
//#include "region.h" //For the color arithmetic.
#include <cmath>
#include <iostream>

cell::cell()
  :fluid(0), updated(false), height(0)
  {}

boolVector::boolVector(const int& width, const int& height)
  : w(width), h(height), boolvector(w*h)
  {
  }

bool boolVector::at(const int& x, const int& y)
  {
  if (x >= 0 && x < w && y >= 0 && y < h)
    return boolvector.at(x + y * w);
  else
    return nullcell;
  }

void boolVector::write(const int& x, const int& y, const bool& input)
  {
  if (x >= 0 && x < w && y >= 0 && y < h)
    boolvector.at(x + y * w) = input;
  }

void boolVector::clear()
  {
  for (auto it = boolvector.begin(); it < boolvector.end(); it++)
    *it = false;
  }

double transientLayer::getHeight() const
  {
  double theight = 0;
  for (int counter = 0; counter < heights.size(); counter++)
    {
    theight += heights.at(counter);
    }
  return theight;
  }

double cell::getHeight() const
  {
  double tempHeight = 0;
  for (int counter = 0; counter < layers.size(); counter++)
    {
    tempHeight += layers.at(counter).height;
    }
  tempHeight += sediments.getHeight();
  //height += fluid;
  return tempHeight;
  }

double cell::getTotalHeight() const
  {
  double totalheight = 0;
  totalheight = getHeight();
  totalheight += fluid;
  return totalheight;
  }

void cell::calculateVelocity()
  {
  //To empty the input vector and make it valid for another round of fluid simulation.
  fluid = 0;
  vector3 finalVelocity;
  for (auto it = inputs.begin(); it< inputs.end(); it++)
    {
    it->first.scale(it->second);
    finalVelocity += it->first;
    fluid += it->second;
    }
    velocity = finalVelocity / fluid;

  inputs.clear();
  }

ErosionHeightmap::ErosionHeightmap(const int& width, const int& height)
  : w(width), h(height), thresholdSpeed(0.1), waterMap(w, h)
  {
  cell buffercell;
  nullcell.nullcell = true;
  nullcell.height = 10000;
  terrain = al_create_bitmap(w, h);

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

  heightmap2 = heightmap1;
  }

void ErosionHeightmap::generateV()
  {
  layer bufferlayer;
  bufferlayer.mat = matDict.lookup(1);

  double current = 500;
  bool add = false;

  for (int ycounter = 0; ycounter < h; ycounter++)
    {
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      bufferlayer.height = current;
      at(xcounter, ycounter).layers.push_back(bufferlayer);
      at(xcounter, ycounter).getHeight();
      }
    if (add)
      current += 1000.0f / (double)h;
    else if (current > 0)
      current -= 1000.0f / (double)h;
    else add = true;
    }
    heightmap2 = heightmap1;  
  }

void ErosionHeightmap::generateTest()
  {
  layer bufferlayer;
  bufferlayer.mat = matDict.lookup(1);

  for (int ycounter = 0; ycounter < h; ycounter++)
    {
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      if (xcounter%3 == 0)
        bufferlayer.height = 10;
      else if (xcounter%3 == 1)
        bufferlayer.height = 0;
      else bufferlayer.height = 5;

        at(xcounter, ycounter).layers.push_back(bufferlayer);
        at(xcounter, ycounter).getHeight();
      }
    }
    heightmap2 = heightmap1;  
  }

cell& ErosionHeightmap::at(const int &x, const int &y)
  {
  if (x >= 0 && x < w && y >= 0 && y < h)
    return currentMap->at(y * w + x);
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

void ErosionHeightmap::addWater(const int& x, const int& y, const double& thisFluid)
  {
  //cell thisCell = at(x, y);
  //
  at(x,y).fluid += thisFluid;
  
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
  writeToQueue = temp;
  }

double ErosionHeightmap::deltaIn(cell& thisCell, const cell& neighborCell)
  {
  double returnval = adjustedHeight(thisCell) - neighborCell.getHeight() + neighborCell.fluid;
  if (returnval < 0)
    return 0;
  else 
    return returnval;
  }

double ErosionHeightmap::adjustedHeight(cell & input)
  {
  // I(x,y) = H(x,y) + Kf * F(x,y) /// v Choose the bigger one, 0 or 1-0.05*velocity.
  if (input.velocity.length <= 0.01)
    return input.getTotalHeight();
  else
    return input.getHeight() + (0 > (1 - 0.05 * input.velocity.length) ? 0 : (1 - 0.05 * input.velocity.length))
           * input.fluid;
  }

vector3 ErosionHeightmap::averageGradient(cell & input, const int& x, const int& y)
  {
  vector3 topleft, top, topright, left, right, bottomleft, bottom, bottomright;
  const double sqrt2(1.41421356237);

  // Per the paper.

  // This time per what I think it is.
  double In = deltaIn(input, at(x-1, y-1));
    if (In != 0)
      {
      topleft.setVector(+1 * In, -1 * In, 0);
      topleft /= sqrt2;
      }
  
  In = deltaIn(input, at(x, y-1));
    if (In != 0)
      top.setVector(0, -1 * In, 0);

  In = deltaIn(input, at(x+1, y-1));
    if (In != 0)
      {
      topright.setVector(-1 * In , -1 * In, 0);
      topright /= sqrt2;
      }

  In = deltaIn(input, at(x-1, y));
    if (In != 0)
      left.setVector(1 * In, 0, 0);

  In = deltaIn(input, at(x+1, y));
    if (In != 0)
      right.setVector(-1 * In, 0, 0);

  In = deltaIn(input, at(x-1, y+1));
    if (In != 0)
      {
      bottomleft.setVector(1 * In, -1 * In, 0);
      bottomleft /= sqrt2;
      }

  In = deltaIn(input, at(x, y+1));
    if (In != 0)
      bottom.setVector(0, In, 0);

  In = deltaIn(input, at(x, y+1));
    if (In != 0)
      {
      bottomright.setVector(-1 * In, In, 0);
      bottomright /= sqrt2;
      }

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

vector3 ErosionHeightmap::normal(cell& thisCell, const int& x, const int& y)
  {
  vector3 horizontal, vertical;
  
  //first the horizontal.
  cell buffer = at(x-1, y);
  cell otherbuffer = at(x+1,y);
  if (buffer.nullcell)
    //If the right side doesn't exist
    horizontal.setVector(x, y, thisCell.height, x+1, y, otherbuffer.height);
  else if (otherbuffer.nullcell) //left side?
    horizontal.setVector(x-1, y, buffer.height, x, y, thisCell.height);
  else
    horizontal.setVector(x-1, y, buffer.height, x+1, y, otherbuffer.height);
  
  //the the vertical
  buffer = at(x, y-1); // top
  otherbuffer = at(x, y+1); // bottom
  if (buffer.nullcell)
    vertical.setVector(x, y, thisCell.height, x, y-1, otherbuffer.height); 
  else if (otherbuffer.nullcell)
    vertical.setVector(x, y+1, buffer.height, x, y, thisCell.height); 
  else
    vertical.setVector(x, y+1, buffer.height, x, y-1, otherbuffer.height); 

  // / dx, /dy
  horizontal.divide(2);
  vertical.divide(2);

  return vector3::cross(horizontal, vertical);
  }

vector3 ErosionHeightmap::normal(const vector3& gradient)
  {
  vector3 horizontal, vertical;
  horizontal.setVector(gradient.x,0,0);
  vertical.setVector(0,gradient.y,0);
  return vector3::cross(horizontal, vertical);
  }

vector3 ErosionHeightmap::normal(const double& x, const double& y)
  {
  return vector3(x, y, -1);
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

//Gotta damp the writeTo cell's velocity~
void ErosionHeightmap::dampVelocity()
  {
  int maxcounter = writeToQueue->size();
  pair<int,int> current;

  for (int counter = 0; counter < maxcounter; counter++)
    {
    current = writeToQueue->front();
    writeToQueue->pop();
    write(current.first, current.second).velocity *= 0.7;// 0.7 = dampening vector.
    writeToQueue->push(current);
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
 
  //Find the x, y distances from the point to the centers of the surrounding cells. Centers equal coords
  //topleft
  topleft = abs((coords.first - baseCoords.first) * (coords.second - baseCoords.second));
  topright = abs((baseCoords.first + 1 - coords.first) * (baseCoords.second - coords.second));
  bottomleft = abs((baseCoords.first - coords.first) * (baseCoords.second + 1 - coords.second));
  bottomright = abs((baseCoords.first + 1 - coords.first) * (baseCoords.second + 1 - coords.second));
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

void ErosionHeightmap::distribute(cell &thisCell, const vector3& originalVector, const pair<double,double>& originalCoords, const pair<double,double>& targetCoords)
  {
  double topleft, topright, bottomleft, bottomright; //are areas
  pair<double, double> baseCoords;
  baseCoords.first = (int)targetCoords.first; //Basically floors the variables.
  baseCoords.second = (int)targetCoords.second;
 
  //Find the x, y distances from the point to the centers of the surrounding cells. Centers equal coords
  //topleft
  topleft = abs((targetCoords.first - baseCoords.first) * (targetCoords.second - baseCoords.second));
  topright = abs((baseCoords.first + 1 - targetCoords.first) * (baseCoords.second - targetCoords.second));
  bottomleft = abs((baseCoords.first - targetCoords.first) * (baseCoords.second + 1 - targetCoords.second));
  bottomright = abs((baseCoords.first + 1 - targetCoords.first) * (baseCoords.second + 1 - targetCoords.second));
  double sum = topleft + topright + bottomleft + bottomright;
  sum *= thisCell.velocity.length;

  //If the total moved water is more than the water in thisCell, cap it to thisCell's water.
  sum = sum > thisCell.fluid ? thisCell.fluid : sum; 
  // get percentages
  topleft /= sum;
  topright /= sum;
  bottomleft /= sum;
  bottomright /= sum;


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

  // Push the remainder of the water onto the cell's input.
  write(originalCoords.first, originalCoords.second).inputs.push_back(pair<vector3, double>(originalVector, thisCell.fluid - sum));
  writeToQueue->push(pair<int,int>(originalCoords.first, originalCoords.second));
  }

vector3 ErosionHeightmap::equation16(vector3 input)
  {
  input.setVector(input.x * -1, input.y * -1, -1 * pow(input.x,2) - pow(input.y, 2));
  return input;
  }

vector3 ErosionHeightmap::equation16_positive(vector3 input)
  {
  input.setVector(input.x, input.y, -1 * pow(input.x,2) - pow(input.y, 2));
  return input;
  }

void ErosionHeightmap::distributeByGradient(cell & input, const int& x, const int& y)
  {
  vector3 topleft, top, topright, left, right, bottomleft, bottom, bottomright;
  const double root2 = 1.41421356237;
  // Per the paper.
  double In = deltaIn(input, at(x-1, y-1));
    if (In != 0)
      {
      topleft.setVector(+1 * In, -1 * In, 0);
      topleft /= root2;
      topleft = equation16(topleft);
      }
  
  In = deltaIn(input, at(x, y-1));
    if (In != 0)
      {
      top.setVector(0, -1 * In, 0);
      top = equation16(top);
      }

  In = deltaIn(input, at(x+1, y-1));
    if (In != 0)
      {
      topright.setVector(-1 * In , -1 * In, 0); 
      topright /= root2;
      //equation 16.
      topright = equation16(topright);
      }

  In = deltaIn(input, at(x-1, y));
    if (In != 0)
      {
      left.setVector(1 * In, 0, 0);
      left = equation16(left);
      }

  In = deltaIn(input, at(x+1, y));
    if (In != 0)
      {
      right.setVector(-1 * In, 0, 0);
      right = equation16(right);
      }

  In = deltaIn(input, at(x-1, y+1));
    if (In != 0)
      {
      bottomleft.setVector(1 * In, -1 * In, 0);
      bottomleft  /= root2;
      bottomleft = equation16(bottomleft);
      }

  In = deltaIn(input, at(x, y+1));
    if (In != 0)
      {
      bottom.setVector(0, In, 0);
      bottom = equation16(bottom);
      }

  In = deltaIn(input, at(x, y+1));
    if (In != 0)
      {
      bottomright.setVector(-1 * In, In, 0);
      bottomright /= root2;
      bottomright = equation16(bottomright);
      }

    double magnitudeSum = topleft.length + top.length + topright.length + 
                          left.length + right.length + bottomleft.length +
                          bottom.length + bottomright.length;

    //If magnitude is larger than total fluid, cap to fluid.
    double adjustedSum = magnitudeSum > input.fluid ? input.fluid : magnitudeSum;

    pair<vector3, double> buffer;

    // Push it.
    if (topleft.length != 0) {   
      buffer.first = topleft;
      buffer.second = topleft.length / magnitudeSum * adjustedSum;
       
      write(x-1, y-1).inputs.push_back(buffer);
      writeToQueue->push(pair<int, int>(x-1, y-1));
      }

    if (top.length != 0) {
      buffer.first = top; 
      buffer.second = top.length / magnitudeSum * adjustedSum;
       
      write(x, y-1).inputs.push_back(buffer);
      writeToQueue->push(pair<int, int>(x, y-1));
      }

    if (topright.length != 0) {
      buffer.first = topright; 
      buffer.second = topright.length / magnitudeSum * adjustedSum;
       
      write(x+1, y-1).inputs.push_back(buffer);
      writeToQueue->push(pair<int, int>(x+1, y-1));
      }

    if (left.length != 0) {
      buffer.first = left; 
      buffer.second = left.length / magnitudeSum * adjustedSum;
       
      write(x-1, y).inputs.push_back(buffer);
      writeToQueue->push(pair<int, int>(x-1, y));
      }

    if (right.length != 0) {
      buffer.first = right; 
      buffer.second = right.length / magnitudeSum * adjustedSum;
       
      write(x+1, y).inputs.push_back(buffer);
      writeToQueue->push(pair<int, int>(x+1, y));
      }

    if (bottomleft.length != 0) {
      buffer.first = bottomleft; 
      buffer.second = bottomleft.length / magnitudeSum * adjustedSum;
       
      write(x-1, y+1).inputs.push_back(buffer);
      writeToQueue->push(pair<int, int>(x-1, y+1));
      }

    if (bottom.length != 0) {
      buffer.first = bottom;
      buffer.second = bottom.length / magnitudeSum * adjustedSum;
       
      write(x, y+1).inputs.push_back(buffer);
      writeToQueue->push(pair<int, int>(x, y+1));
      }

    if (bottomright.length != 0) {
      buffer.first = bottomright; 
      buffer.second = bottomright.length / magnitudeSum * adjustedSum;
       
      write(x+1, y+1).inputs.push_back(buffer);
      writeToQueue->push(pair<int, int>(x+1, y+1));
      }

    buffer.first = input.velocity; buffer.second = input.fluid - adjustedSum; //Write what's left back to the cell.
    if (buffer.second != 0)
      {
      write(x, y).inputs.push_back(buffer);
      writeToQueue->push(pair<int, int>(x, y));
      }

  }

void ErosionHeightmap::step()
  {
  //For each cell that has water in it...
  resetUpdatedFlags();

  pair<int, int> current;
  pair<double, double> movedCoords;
  cell currentcell;
 // cell* pointerToCurrentCellInArray;
  vector3 acceleration;
  vector3 finalAcceleration;
  vector3 copyOfCellVelocity;
  vector3 copy2;
  double minusone = -1;

  while (!currentQueue->empty())
    {
      current = currentQueue->front();
      currentcell = at(current.first, current.second);
      setCellToClear(&at(current.first, current.second));

      //If the speed is above the threshold, get the average gradient.
        // Drop cells whose H+F is higher than this cell's H+F.
        // Average the gradient vectors.
    if (!currentcell.updated) //if this cell has already been updated, skip it.
      {
      if (currentcell.fluid >= 0.01) //if above threshold
        {
       // if (!(currentcell.velocity.length < 0.0001)) //minimum
         // {
          if (currentcell.velocity.length >= thresholdSpeed)
            {
            copy2 = currentcell.velocity;
            // add the acceleration...
            acceleration = averageGradient(currentcell, current.first, current.second);
            //          acceleration *= minusone;
            acceleration = normal(acceleration.x, acceleration.y);
            // Equation 16 below.
            acceleration = equation16(acceleration);
            // Equation 18 below.
            finalAcceleration = acceleration;
            finalAcceleration /= acceleration.length;
            finalAcceleration *= (acceleration.z / acceleration.length * 9.81);
            // add velocity and acceleration.
            currentcell.velocity += acceleration;
            // After dividing the fluid into the neighboring cells, also divide the velocity and push it
            // onto the cell's input vector. Multiplied by the fluid amount.
            //This function also pushes coords onto the write-to queue.
            // First, add the velocity to this cell's coords.
            movedCoords.first = current.first;
            movedCoords.second = current.second;
            copyOfCellVelocity = currentcell.velocity;
            copyOfCellVelocity.normalize2();
            movedCoords.first += copyOfCellVelocity.x;
            movedCoords.second += copyOfCellVelocity.y;
            distribute(currentcell, copy2, current, movedCoords);
            //Distribute() pushes.
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
      //  }//If below threshold, procede straight to clearing the water.
      // Clear the cell's water and velocity.
      clearCell();
      }
    // For each cell that gets water, push it onto the write queue

    // Pop from queue.
      currentQueue->pop();
    }

  //Calc velocities for all the cells with water.
  pair<int,int> coords;
  for (int counter = 0; counter < writeToQueue->size(); counter++)
    {
    coords = writeToQueue->front();
    write(coords.first, coords.second).calculateVelocity();
    if (!write(coords.first, coords.second).velocity.isValid())
      std::cout << coords.first << "," << coords.second << ": invalid velocity vector.\n";
    writeToQueue->pop();
    writeToQueue->push(coords);
    }

  //Damped.
  dampVelocity(); // Dampens the writeToMap velocities.

  //swap the pointers.
  swapMaps();
  swapQueues();
  //reset updated flags 
  resetUpdatedFlags();
  
  }

void ErosionHeightmap::render()
  {
  renderMap.initMap(w,h);
  
  waterMap.clear();
  int numberofwaters = 0; // To check in watch mode.
  for (int ycounter = 0; ycounter < h; ycounter++)
    {
    for (int xcounter = 0; xcounter < w; xcounter++)
      //add shading or raytracing later~
      renderMap.at(xcounter, ycounter) = at(xcounter, ycounter).getHeight(); 
    }///Do it like this at first. Add shading for different depths of water laaater
   pair<int,int> coords;
    for (int counter = 0; counter < currentQueue->size(); counter++)
      {
      coords = currentQueue->front();
      waterMap.write(coords.first, coords.second, true);
      currentQueue->pop();
      currentQueue->push(coords);
      }

  renderMap.scale(0,1);


  ALLEGRO_COLOR sandcolor, blue, darkBlue;
  ALLEGRO_COLOR red; ALLEGRO_COLOR darkRed;
  sandcolor = al_map_rgb(194, 178, 128);
  blue = al_map_rgb(0,0,255);
  darkBlue = al_map_rgb(0,0,50);
  red = al_map_rgb(255,0,0);
  darkRed = al_map_rgb(50,0,0);
  double waterHeight = 0;
  
  //watercolor = al_map_rgb(0,0,255);

  al_set_target_bitmap(terrain);
  al_lock_bitmap(al_get_target_bitmap(), al_get_bitmap_format(terrain), ALLEGRO_LOCK_READWRITE);

  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      if (waterMap.at(xcounter, ycounter))
        {
        //al_put_pixel(xcounter, ycounter, al_map_rgb(0, 0, 255));
        waterHeight = at(xcounter, ycounter).fluid;
        waterHeight = log10(waterHeight);
        waterHeight = waterHeight > 1 ? 1 : waterHeight;
        waterHeight = waterHeight < 0 ? 0 : waterHeight;
        al_put_pixel(xcounter, ycounter, heightmap::lerp(blue, darkBlue, (int)waterHeight));

        }
      else //renderMap.at(xcounter, ycounter)
        al_put_pixel(xcounter, ycounter, heightmap::lerp(darkRed, red, renderMap.at(xcounter,ycounter)));
      }

  al_unlock_bitmap(al_get_target_bitmap());

 // addShadows(copymap);
  }

void ErosionHeightmap::setCellToClear(cell* clearerCell)
  {
  cellToClear = clearerCell;
  }

void ErosionHeightmap::clearCell()
  {
  cellToClear->fluid = 0;
  cellToClear->velocity.clear();
  }