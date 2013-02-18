#include "virtualPipes.h"
#include <cmath>
#include "noise.h"
#include <ctime>
#include "colors.h"
#include <boost/thread.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>

using namespace ColorMath;

pipeCell::pipeCell()
  {

  }

pipeCell::pipeCell(const int& newx, const int& newy, const double& cellSize) 
  : x(newx), y(newy), fluxLeft(0), fluxRight(0), fluxTop(0), fluxBottom(0),
  suspendedSediment(0), terrainHeight(0), waterHeight(0), lengthX(cellSize), lengthY(cellSize)
  , tempKc(1), dissolvingConstant(0.1), depositingConstant(0.2), sedimentCapacity(0),
  hasBeenEroded(false)
  {

  }

double pipeCell::getTerrainHeight() const
  {
  // temp code.
  return terrainHeight;
  }

void pipeCell::setTerrainHeight(const double& newTerrainHeight)
  {
  terrainHeight = newTerrainHeight;
  }

void pipeCell::addToTerrainHeight(const double& moreHeight)
  {
  terrainHeight += moreHeight;
  }

void pipeCell::addToWaterHeight(const double& moreWater)
  {
  waterHeight += moreWater;
  }

double pipeCell::getWaterHeight() const
  {
  return waterHeight;
  }

void pipeCell::setWaterHeight(const double& newWaterHeight)
  {
  waterHeight = newWaterHeight;
  }

double pipeCell::getTotalHeight() const
  {
  return terrainHeight + waterHeight;
  }

double pipeCell::cellArea() const
  {
  return lengthX * lengthY;
  }

double pipeCell::cellLength() const
  {
  return lengthX;
  }

double pipeCell::netFlux() const
  {
  return fluxLeft + fluxRight + fluxTop + fluxBottom;
  }

void pipeCell::scaleByK(const double& K)
  {
  fluxLeft *= K;
  fluxRight *= K;
  fluxTop *= K;
  fluxBottom *= K;
  }

double pipeCell::getSedimentCapacityConstant() const
  {
  return tempKc;
  }

double pipeCell::getAngleHeight(const pipeCell& thisCell) const
  {
  return getTerrainHeight();
  }

double WallCell::getAngleHeight(const pipeCell& thisCell) const
  {
  return thisCell.getTerrainHeight();
  }

/////////////
//
//
//
//
/////////////

VirtualPipeErosion::VirtualPipeErosion(const int& width, const int& height, const double& cellSize, const bool& random)
  : w(width), h(height), gravityConstant(9.8), pipeCrossSectionalArea(1), erodeTimer(0)
  {
  if (random)
    Perlingen.SetSeed(time(0));

  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      list1.push_back(pipeCell(xcounter, ycounter, cellSize));
  list2 = list1;
  readList = &list1;
  writeList = &list2;
  terrain = al_create_bitmap(w, h);
  sedimentList.insert(sedimentList.begin(), w*h, 0);
 /* readQueue = &queue1;
  writeQueue = &queue2;*/
  }

void VirtualPipeErosion::step(const double& time)
  {
  currentTimeStep = time;

  boost::thread flux1(&VirtualPipeErosion::operator(), boost::ref(this), 0, h/4, 0);
  boost::thread flux2(&VirtualPipeErosion::operator(), boost::ref(this), h/4, 2*h/4, 0);
  boost::thread flux3(&VirtualPipeErosion::operator(), boost::ref(this), 2*h/4, 3*h/4, 0);
  boost::thread flux4(&VirtualPipeErosion::operator(), boost::ref(this), 3*h/4, h, 0);

  flux1.join();
  flux2.join();
  flux3.join();
  flux4.join();

  boost::thread vector1(&VirtualPipeErosion::operator(), boost::ref(this), 0, h/4, 1);
  boost::thread vector2(&VirtualPipeErosion::operator(), boost::ref(this), h/4, 2*h/4, 1);
  boost::thread vector3(&VirtualPipeErosion::operator(), boost::ref(this), 2*h/4, 3*h/4, 1);
  boost::thread vector4(&VirtualPipeErosion::operator(), boost::ref(this), 3*h/4, h, 1);

  vector1.join();
  vector2.join();
  vector3.join();
  vector4.join();

  operator()(0, h, 3);

  boost::thread erosion1(&VirtualPipeErosion::operator(), boost::ref(this), 0, h/4, 4);
  boost::thread erosion2(&VirtualPipeErosion::operator(), boost::ref(this), h/4, 2*h/4, 4);
  boost::thread erosion3(&VirtualPipeErosion::operator(), boost::ref(this), 2*h/4, 3*h/4, 4);
  boost::thread erosion4(&VirtualPipeErosion::operator(), boost::ref(this), 3*h/4, h, 4);

  erosion1.join();
  erosion2.join();
  erosion3.join();
  erosion4.join();

  updateSedimentMap(0, h);

  boost::thread transport1(&VirtualPipeErosion::operator(), boost::ref(this), 0, h/4, 5);
  boost::thread transport2(&VirtualPipeErosion::operator(), boost::ref(this), h/4, 2*h/4, 5);
  boost::thread transport3(&VirtualPipeErosion::operator(), boost::ref(this), 2*h/4, 3*h/4, 5);
  boost::thread transport4(&VirtualPipeErosion::operator(), boost::ref(this), 3*h/4, h, 5);

  transport1.join();
  transport2.join();
  transport3.join();
  transport4.join();

  swapMaps();
  }

void VirtualPipeErosion::singlethreaded_step(const double& time)
  {
  currentTimeStep = time;
  //maxErodeTimer = 1;// / time;
  stepThroughFlux(0, h);
  stepThroughVector(0, h);
  stepThroughErosion(0, h);
  updateSedimentMap(0, h);
  stepThroughTransport(0, h);
  cleanUp(0, h);
  swapMaps();
  }

void VirtualPipeErosion::stepThroughFlux(const int& startRow, const int& endRow)
  {
  pipeCell buffer;
  for (int ycounter = startRow; ycounter < endRow; ycounter++)
    {
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      buffer = read(xcounter, ycounter);
      //Calculate the flow vectors, T/B/L/R.
      calculateFlux(buffer);
      buffer.scaleByK(scalingK(buffer));
      write(xcounter, ycounter) = buffer;
      }
    }
  }

void VirtualPipeErosion::stepThroughVector(const int& startRow, const int& endRow)
  {
  //pipeCell buffer;
  for (int ycounter = startRow; ycounter < endRow; ycounter++)
    {
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      // Change the volumes.
       write(xcounter, ycounter).setWaterHeight(updateWaterHeight(write(xcounter, ycounter)));
       write(xcounter, ycounter).flow = findFlowVector(write(xcounter, ycounter));
      }
    }
  }

void VirtualPipeErosion::stepThroughErosion(const int& startRow, const int& endRow)
  {
  for (int ycounter = startRow; ycounter < endRow; ycounter++)
    {
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      write(xcounter, ycounter).sedimentCapacity = findSedimentCapacity(write(xcounter, ycounter));
      erosionDeposition(write(xcounter,ycounter));
      }
    }
  }

void VirtualPipeErosion::stepThroughTransport(const int& startRow, const int& endRow)
  {
  for (int ycounter = startRow; ycounter < endRow; ycounter++)
    {
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      write(xcounter, ycounter).suspendedSediment = findNewSediment(write(xcounter, ycounter));
      }
    }
  }

void VirtualPipeErosion::updateSedimentMap(const int& startRow, const int& endRow)
  {
  for (int ycounter = startRow; ycounter < endRow; ycounter++)
    {
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      sedimentAt(xcounter, ycounter) = write(xcounter, ycounter).suspendedSediment;
      }
    }
  }

double VirtualPipeErosion::findNewSediment(const pipeCell& thisCell)
  {
  //vector3 flowvector (thisCell.flow);
  //flowvector.normalize();
  return bilinearSediment(thisCell.x - thisCell.flow.x * currentTimeStep, thisCell.flow.y - thisCell.y * currentTimeStep);
  }

double VirtualPipeErosion::bilinearSediment(const double& x, const double& y)
  {
  int basex = floor(x);
  int basey = floor(y);
  double int1,int2;
  int1 = 1 - (x - basex);
  int2 = 1 - (y - basey);
  double lerp1 = sedimentAt(basex, basey) * (int1) + sedimentAt(basex+1, basey) * (1 - int1);
  double lerp2 = sedimentAt(basex, basey+1) * (int1) + sedimentAt(basex+1, basey+1) * (1 - int1);
  return lerp1 * (int2) + lerp2 * (1 - int2);
  //Temp test code by just clamping the sediment.
  //return sedimentAt(basex, basey);
  }

void VirtualPipeErosion::calculateFlux(pipeCell& thisCell)
  {
  // Do all four.
  thisCell.fluxTop = max(0, currentTimeStep * pipeCrossSectionalArea * 9.8 * heightDifference(thisCell.x, thisCell.y, thisCell.x, thisCell.y-1));
  thisCell.fluxLeft = max(0, currentTimeStep * pipeCrossSectionalArea * 9.8 * heightDifference(thisCell.x, thisCell.y, thisCell.x-1, thisCell.y));
  thisCell.fluxRight = max(0, currentTimeStep * pipeCrossSectionalArea * 9.8 * heightDifference(thisCell.x, thisCell.y, thisCell.x+1, thisCell.y));
  thisCell.fluxBottom = max(0, currentTimeStep * pipeCrossSectionalArea * 9.8 * heightDifference(thisCell.x, thisCell.y, thisCell.x, thisCell.y+1));
  }

void VirtualPipeErosion::cleanUp(const int& startRow, const int& endRow)
  {
  for (int ycounter = startRow; ycounter <= endRow; ycounter++)
    {
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      write(xcounter, ycounter).setWaterHeight(write(xcounter, ycounter).getWaterHeight() > 0.00001 ? write(xcounter, ycounter).getWaterHeight() : 0);
      }
    }
  }

double VirtualPipeErosion::findSedimentCapacity(const pipeCell& thisCell)
  {
  return currentTimeStep * thisCell.getSedimentCapacityConstant() * max(0.001, getSine(thisCell)) * thisCell.flow.length * thisCell.getWaterHeight();
  }

double VirtualPipeErosion::getSine(const pipeCell& thisCell)
  {
  double angle1 = (write(thisCell.x - 1, thisCell.y).getAngleHeight(thisCell) - write(thisCell.x + 1, thisCell.y).getAngleHeight(thisCell))/2;
  double angle2 = (write(thisCell.x, thisCell.y - 1).getAngleHeight(thisCell) - write(thisCell.x, thisCell.y+1).getAngleHeight(thisCell))/2;
  angle1 = atan(angle1);
  angle2 = atan(angle2);
  return sin((angle1 + angle2)/2);
  }

void VirtualPipeErosion::erosionDeposition(pipeCell& thisCell)
  {
    if (thisCell.sedimentCapacity > thisCell.suspendedSediment)
      {
      double movedSediment = thisCell.dissolvingConstant * (thisCell.sedimentCapacity - thisCell.suspendedSediment);
      if (thisCell.getTerrainHeight() < movedSediment)
        movedSediment = thisCell.getTerrainHeight();
      thisCell.addToTerrainHeight(-movedSediment);
      thisCell.suspendedSediment += movedSediment;
      }
    else
      {
      double depositedSediment = thisCell.depositingConstant * (thisCell.suspendedSediment - thisCell.sedimentCapacity);
      thisCell.addToTerrainHeight(depositedSediment);
      thisCell.suspendedSediment -= depositedSediment;
      if (depositedSediment > 0.01)
        thisCell.hasBeenEroded = true;
      }
  }

double VirtualPipeErosion::max(const double& left, const double& right)
  {
  return left > right ? left : right;
  }

double VirtualPipeErosion::min(const double& left, const double& right)
  {
  return left > right ? right : left;
  }

double VirtualPipeErosion::heightDifference(const int& x1, const int& y1, const int& x2, const int& y2)
  {
  return read(x1, y1).getTotalHeight() - read(x2, y2).getTotalHeight();
  }

double VirtualPipeErosion::scalingK(const pipeCell& thisCell)
  {
  return min(1, thisCell.getWaterHeight() * thisCell.cellArea() / thisCell.netFlux() / currentTimeStep);
  }

double VirtualPipeErosion::netVolumeChange(const pipeCell& thisCell)
  {
  return currentTimeStep * ( write(thisCell.x - 1, thisCell.y).fluxRight 
    + write(thisCell.x, thisCell.y + 1).fluxTop
    + write(thisCell.x + 1, thisCell.y).fluxLeft
    + write(thisCell.x, thisCell.y - 1).fluxBottom
    - thisCell.netFlux());
  }

double VirtualPipeErosion::updateWaterHeight(const pipeCell& inputCell)
  {
  return inputCell.getWaterHeight() + netVolumeChange(inputCell) / inputCell.cellArea();
  }

void VirtualPipeErosion::addWater(const int& x, const int& y, const double& amount)
  {
  read(x, y).addToWaterHeight(amount);
  }

pipeCell& VirtualPipeErosion::read(const int& x, const int& y)
  {
  if (x < w && y < h && x >= 0 && y >= 0)
    return readList->at(x + y * w);
  else
    return nullcell;
  }

pipeCell& VirtualPipeErosion::write(const int& x, const int& y)
  {
  if (x < w && y < h && x >= 0 && y >= 0)
    return writeList->at(x + y * w);
  else
    return nullcell;
  }

vector3 VirtualPipeErosion::findFlowVector(const pipeCell& thisCell)
  {
  double x = write(thisCell.x - 1, thisCell.y).fluxRight + thisCell.fluxRight
              - write(thisCell.x + 1, thisCell.y).fluxLeft - thisCell.fluxLeft;
  double y = write(thisCell.x, thisCell.y - 1).fluxBottom + thisCell.fluxBottom
              - write(thisCell.x, thisCell.y + 1).fluxTop - thisCell.fluxTop;

  x /= 2;
  y /= 2;

  return vector3(x, y, 0);
  }

void VirtualPipeErosion::swapMaps()
  {
  vector<pipeCell> * temp;
  temp = readList;
  readList = writeList;
  writeList = temp;
  }

//void VirtualPipeErosion::swapMaps()

void VirtualPipeErosion::generateV()
  {
  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      read(xcounter, ycounter).setTerrainHeight(abs(h/2 - ycounter));
      }


    *writeList = *readList;
  }

void VirtualPipeErosion::generate()
  {
  temporaryMap = new heightmap;
  temporaryMap->initMap(w, h);

  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      temporaryMap->at(xcounter, ycounter) = Perlingen.GetValue((double)xcounter / 183.1f, (double)ycounter / 180.0f, 0.1f);
      }

    temporaryMap->scale(1, 50);

  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      read(xcounter, ycounter).setTerrainHeight(temporaryMap->at(xcounter, ycounter));
      }


    *writeList = *readList;
  }

void VirtualPipeErosion::render()
  {
  renderMap.initMap(w,h);
  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      renderMap.at(xcounter, ycounter) = read(xcounter, ycounter).getTerrainHeight();
      }
   // renderMap.scale(0,1);

    ALLEGRO_COLOR red; ALLEGRO_COLOR darkRed, blue, green;
    red = al_map_rgb(255,0,0);
    blue = al_map_rgb(0,0,200);
    darkRed = al_map_rgb(50,0,0);
    green = al_map_rgb(0,120,0);
    ALLEGRO_COLOR black = al_map_rgb(0,0,0);
    ALLEGRO_COLOR orange = al_map_rgb(255,102,0);


    al_set_target_bitmap(terrain);
    al_lock_bitmap(al_get_target_bitmap(), al_get_bitmap_format(terrain), ALLEGRO_LOCK_READWRITE);
    for (int ycounter = 0; ycounter < h; ycounter++)
      for (int xcounter = 0; xcounter < w; xcounter++)
        {
        if (read(xcounter, ycounter).getWaterHeight() > 0)
          {
          if (read(xcounter, ycounter).suspendedSediment > 0)
            
            al_put_pixel(xcounter, ycounter, green);
          else
            al_put_pixel(xcounter, ycounter, blue);
          }
        else if (/*read(xcounter, ycounter).hasBeenEroded*/ false)
          {
          al_put_pixel(xcounter, ycounter, orange);
          }
        else
          al_put_pixel(xcounter, ycounter, heightmap::lerp(black, red, renderMap.at(xcounter,ycounter)/(double)64));
        }
      al_unlock_bitmap(al_get_target_bitmap());

  }

/*
void VirtualPipeErosion::prepErosion(const double& time)
  {
  currentTimeStep = time;
  }*/

void VirtualPipeErosion::operator()(const int& startRow, const int& endRow, const int& mode)
  {
  if (mode == 0)
    stepThroughFlux(startRow, endRow);
  else if (mode == 1)
    stepThroughVector(startRow, endRow);
  else if (mode == 3)
    cleanUp(startRow, endRow);
  else if (mode == 4)
    stepThroughErosion(startRow, endRow);
  else if (mode == 5)
    stepThroughTransport(startRow, endRow); // Must swap before and after Transport.
  else
    return;
  }

void VirtualPipeErosion::evaporate(const double& amount)
  {
  if (amount > 1 || amount < 0)
    return;
  else
    for (int ycounter = 0; ycounter < h; ycounter++)
      for (int xcounter = 0; xcounter < w; xcounter++)
        {
        read(xcounter, ycounter).setWaterHeight(read(xcounter, ycounter).getWaterHeight()*amount);
        if (read(xcounter, ycounter).getWaterHeight() < 0.001)
          read(xcounter, ycounter).setWaterHeight(0);
        }
  }

double& VirtualPipeErosion::sedimentAt(const int& x, const int& y)
  {
  if (x < w && y < h && x >= 0 && y >= 0)
    return sedimentList.at(x + y * w);
  else
    return nullcell.suspendedSediment;
  }

////////////////
//
//
//
//
////////////////

void VirtualPipeErosionTools::randomRain(VirtualPipeErosion& thisErosion, const int& howMany, const double& howMuchRain)
  {
  int width = thisErosion.w;
  int height = thisErosion.h;

  for (int counter = 0; counter < howMany; counter++)
    {
    thisErosion.addWater(random(0, width), random(0, height), random(howMuchRain * 0.9, howMuchRain * 1.1)); 
    }
  };

int VirtualPipeErosionTools::random(const int& min, const int& max)
  {
  boost::random::uniform_int_distribution<> dist(min, max);
  return dist(rng);
  }

double VirtualPipeErosionTools::random(const double& min, const double& max)
  {
  boost::random::uniform_real_distribution<> dist(min, max);
  return dist(rng);
  }