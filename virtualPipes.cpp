#include "virtualPipes.h"
#include <cmath>
#include "noise.h"
#include <ctime>
#include "colors.h"
#include <boost/thread.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <stdlib.h>
#include <ctime>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using namespace ColorMath;
/*


material& materialDict::lookup(const unsigned int& key)
  {
  if (key < tableSize && key >= 0)
    return table[key];
  else
    return table[0];
  }*/
  

/////////////
//
// Pipe cell functions.
//
/////////////

pipeCell::pipeCell()
  {

  }

pipeCell::pipeCell(const int& newx, const int& newy, const double& cellSize) 
  : x(newx), y(newy), fluxLeft(0), fluxRight(0), fluxTop(0), fluxBottom(0),
  suspendedSediment(0), terrainHeight(0), waterHeight(0), lengthX(cellSize), lengthY(cellSize)
  , tempKc(10.0f), dissolvingConstant(0.3), depositingConstant(0.3), sedimentCapacity(0),
  hasBeenEroded(false), soilSlippage(0.3)
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

void pipeCell::scaleSoilByK(const float& K)
  {
  soil_fluxLeft *= K;
  soil_fluxRight *= K;
  soil_fluxTop *= K;
  soil_fluxBottom *= K;
  }

double pipeCell::getSedimentCapacityConstant() const
  {
  return tempKc;
  }

double pipeCell::getSoilSlippage() const
  {
  return soilSlippage;
  }

void pipeCell::setSoilSlippage(const double& newSlippage)
  {
  soilSlippage = newSlippage;
  }

double pipeCell::getAngleHeight(const pipeCell& thisCell) const
  {
  return getTerrainHeight();
  }

double WallCell::getAngleHeight(const pipeCell& thisCell) const
  {
  return thisCell.getTerrainHeight();
  }

void drainCell::clear()
  {
  waterHeight = 0;
  suspendedSediment = 0;
  fluxLeft = fluxRight = fluxTop = fluxBottom = 0;
  }



/////////////
//
//  VirtualPipeErosion
//
/////////////

VirtualPipeErosion::VirtualPipeErosion(const int& width, const int& height, const double& cellSize, const bool& random)
  : w(width), h(height), gravityConstant(9.8), pipeCrossSectionalArea(1), erodeTimer(0), outputHeightmap(w*h), outputRGBMap(w*h*3)
  ,initialTerrainHeight(w*h+1), outputWatermap(w*h), numberOfThreads(4)
  {
  srand(time(0));
  Perlingen.SetSeed(rand());

  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      list1.push_back(pipeCell(xcounter, ycounter, cellSize));
  list2 = list1;
  readList = &list1;
  writeList = &list2;
  terrain = al_create_bitmap(w, h);
  sedimentCapacityRender = al_create_bitmap(w , h);
  sedimentRender = al_create_bitmap(w, h);
  sedimentFraction = al_create_bitmap(w, h);
  sedimentList.insert(sedimentList.begin(), w*h, 0);

  renderMap.initMap(w,h);
  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      renderMap.infomap.push_back(0);
 /* readQueue = &queue1;
  writeQueue = &queue2;*/


  }

VirtualPipeErosion::VirtualPipeErosion(ALLEGRO_BITMAP* inputTerrain, ALLEGRO_BITMAP* inputWater, const double& minScale, const double& maxScale)
  : gravityConstant(9.8), pipeCrossSectionalArea(1), erodeTimer(0), w(al_get_bitmap_width(inputTerrain)), h(al_get_bitmap_height(inputTerrain))
  , outputHeightmap(w*h), outputRGBMap(w*h*3), initialTerrainHeight(w*h+1), outputWatermap(w*h)
  {
  double cellSize = 1;
 /* w = al_get_bitmap_width(inputTerrain);
  h = al_get_bitmap_height(inputTerrain);*/
  srand(time(0));
 /* boost::random::mt19937 gen;
  boost::random::uniform_int_distribution<> dist(0, numeric_limits<int>::max()); */
  Perlingen.SetSeed(rand());

  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      list1.push_back(pipeCell(xcounter, ycounter, cellSize));
  list2 = list1;
  readList = &list1;
  writeList = &list2;
  terrain = al_create_bitmap(w, h);
  sedimentCapacityRender = al_create_bitmap(w , h);
  sedimentRender = al_create_bitmap(w, h);
  sedimentFraction = al_create_bitmap(w, h);
  

  sedimentList.insert(sedimentList.begin(), w*h, 0);

  renderMap.initMap(w,h);
  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      renderMap.infomap.push_back(0);

  // Actually load the terrain and the water from the maps.
  al_lock_bitmap(inputTerrain, al_get_bitmap_format(inputTerrain), ALLEGRO_LOCK_READONLY);
  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      read(xcounter, ycounter).setTerrainHeight(al_get_pixel(inputTerrain, xcounter, ycounter).r * (maxScale-minScale) + minScale);
      }
  al_unlock_bitmap(inputTerrain);

  // Water.
  al_lock_bitmap(inputWater, al_get_bitmap_format(inputWater), ALLEGRO_LOCK_READONLY);
  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      read(xcounter, ycounter).setWaterHeight(al_get_pixel(inputWater, xcounter, ycounter).r * (minScale+1));
      }
  al_unlock_bitmap(inputWater);

  
  }

void VirtualPipeErosion::step(const double& time)
  {
  currentTimeStep = time;

  ///
  /// 4 threads
  ///
  
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

//  updateTempTerrain();

  /*boost::thread slippage1(&VirtualPipeErosion::operator(), boost::ref(this), 0, h/4, 6);
  boost::thread slippage2(&VirtualPipeErosion::operator(), boost::ref(this), h/4, 2*h/4, 6);
  boost::thread slippage3(&VirtualPipeErosion::operator(), boost::ref(this), 2*h/4, 3*h/4, 6);
  boost::thread slippage4(&VirtualPipeErosion::operator(), boost::ref(this), 3*h/4, h, 6);

  slippage1.join();
  slippage2.join();
  slippage3.join();
  slippage4.join();

  boost::thread move1(&VirtualPipeErosion::operator(), boost::ref(this), 0, h/4, 7);
  boost::thread move2(&VirtualPipeErosion::operator(), boost::ref(this), h/4, 2*h/4, 7);
  boost::thread move3(&VirtualPipeErosion::operator(), boost::ref(this), 2*h/4, 3*h/4, 7);
  boost::thread move4(&VirtualPipeErosion::operator(), boost::ref(this), 3*h/4, h, 7);

  move1.join();
  move2.join();
  move3.join();
  move4.join();*/

  drain.clear();

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
  pipeCell* thisCell;
  for (int ycounter = startRow; ycounter < endRow; ycounter++)
    {
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      thisCell = &write(xcounter, ycounter);
      thisCell->suspendedSediment = findNewSediment(*thisCell);
      }
    }
  }

void VirtualPipeErosion::stepThroughSlippageCalc(const int& startRow, const int& endRow)
  {
  pipeCell* thisCell;
  for (int ycounter = startRow; ycounter < endRow; ycounter++)
    {
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      thisCell = &write(xcounter, ycounter);
      // Get the height difference that exceeds the slippage threshold.
      thisCell->soil_fluxTop = max(0, currentTimeStep * pipeCrossSectionalArea * 9.8 * max(0, terrainHeightDifference(*thisCell, thisCell->x, thisCell->y-1) - thisCell->getSoilSlippage()));
      thisCell->soil_fluxLeft = max(0, currentTimeStep * pipeCrossSectionalArea * 9.8 * max(0, terrainHeightDifference(*thisCell, thisCell->x-1, thisCell->y) - thisCell->getSoilSlippage()));
      thisCell->soil_fluxRight = max(0, currentTimeStep * pipeCrossSectionalArea * 9.8 * max(0, terrainHeightDifference(*thisCell, thisCell->x+1, thisCell->y) - thisCell->getSoilSlippage()));
      thisCell->soil_fluxBottom = max(0, currentTimeStep * pipeCrossSectionalArea * 9.8 * max(0, terrainHeightDifference(*thisCell, thisCell->x, thisCell->y+1) - thisCell->getSoilSlippage()));
      //thisCell->scaleSoilByK((float)min(1, thisCell->getTerrainHeight() * thisCell->cellArea() / (thisCell->soil_fluxTop+thisCell->soil_fluxBottom+thisCell->soil_fluxLeft+thisCell->soil_fluxRight) / currentTimeStep));
      }
    }
  }

void VirtualPipeErosion::stepThroughMoveTerrain(const int& startRow, const int& endRow)
  {
  pipeCell* thisCell;
  for (int ycounter = startRow; ycounter < endRow; ycounter++)
    {
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      thisCell = &write(xcounter, ycounter);
      // Write the new height.
      thisCell->addToTerrainHeight( currentTimeStep * ( write(thisCell->x - 1, thisCell->y).soil_fluxRight 
        + write(thisCell->x, thisCell->y + 1).soil_fluxTop
        + write(thisCell->x + 1, thisCell->y).soil_fluxLeft
        + write(thisCell->x, thisCell->y - 1).soil_fluxBottom
        -( thisCell->soil_fluxBottom + thisCell->soil_fluxTop + thisCell->soil_fluxLeft + thisCell->soil_fluxRight)));
      }
    }
  }

void VirtualPipeErosion::setThreads(const int& powersOfTwo)
  {
  numberOfThreads = pow((double)2.0f, (int)powersOfTwo);
  }

/*
void VirtualPipeErosion::updateTempTerrain()
  {
  for (int ycounter = 0; ycounter < h; ycounter++)
    {
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      renderMap.at(xcounter, ycounter) = write(xcounter, ycounter).getTerrainHeight();
      }
    }
  }*/

double VirtualPipeErosion::findNewSediment(const pipeCell& thisCell)
  {
  //vector3 flowvector (thisCell.flow);
  //flowvector.normalize();
  return bilinearSediment(thisCell.x - thisCell.flow.x * currentTimeStep * 10, thisCell.y - thisCell.flow.y * currentTimeStep * 10);
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
  thisCell.fluxTop = max(0, currentTimeStep * pipeCrossSectionalArea * 9.8 * heightDifference(thisCell, thisCell.x, thisCell.y-1));
  thisCell.fluxLeft = max(0, currentTimeStep * pipeCrossSectionalArea * 9.8 * heightDifference(thisCell, thisCell.x-1, thisCell.y));
  thisCell.fluxRight = max(0, currentTimeStep * pipeCrossSectionalArea * 9.8 * heightDifference(thisCell, thisCell.x+1, thisCell.y));
  thisCell.fluxBottom = max(0, currentTimeStep * pipeCrossSectionalArea * 9.8 * heightDifference(thisCell, thisCell.x, thisCell.y+1));
  }

void VirtualPipeErosion::cleanUp(const int& startRow, const int& endRow)
  {
  double buffer;
  for (int ycounter = startRow; ycounter <= endRow; ycounter++)
    {
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      pipeCell* currentCell = &write(xcounter, ycounter);
      buffer = currentCell->getWaterHeight();
      currentCell->setWaterHeight(buffer > 0.00001 ? buffer : 0);
      }
    }
  }

double VirtualPipeErosion::findSedimentCapacity(const pipeCell& thisCell)
  {
  return currentTimeStep * thisCell.getSedimentCapacityConstant() * max(0.001, abs(getSine(thisCell))) * thisCell.flow.length ;//* thisCell.getWaterHeight();
  }

double VirtualPipeErosion::getSine(const pipeCell& thisCell)
  {
  double angle1 = (write(thisCell.x - 1, thisCell.y).getAngleHeight(thisCell) - write(thisCell.x + 1, thisCell.y).getAngleHeight(thisCell))/2;
  double angle2 = (write(thisCell.x, thisCell.y - 1).getAngleHeight(thisCell) - write(thisCell.x, thisCell.y+1).getAngleHeight(thisCell))/2;
  angle1 = atan(angle1);
  angle2 = atan(angle2);
  return sin((abs(angle1) + abs(angle2))/2);
  }

void VirtualPipeErosion::erosionDeposition(pipeCell& thisCell)
  {
    if (thisCell.sedimentCapacity > thisCell.suspendedSediment)
      {
      double movedSediment = currentTimeStep * thisCell.dissolvingConstant * (thisCell.sedimentCapacity - thisCell.suspendedSediment);
      if (thisCell.getTerrainHeight() < movedSediment)
        movedSediment = thisCell.getTerrainHeight();
      thisCell.addToTerrainHeight(-movedSediment);
      thisCell.suspendedSediment += movedSediment;
      }
    else
      {
      double depositedSediment = currentTimeStep *  thisCell.depositingConstant * (thisCell.suspendedSediment - thisCell.sedimentCapacity);
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

double VirtualPipeErosion::heightDifference(const pipeCell& currentCell, const int& x2, const int& y2)
  {
  return currentCell.getTotalHeight() - read(x2, y2).getTotalHeight();
  }

double VirtualPipeErosion::terrainHeightDifference(const pipeCell& currentCell, const int& x2, const int& y2)
  {
  return currentCell.getTerrainHeight() - read(x2, y2).getTerrainHeight();
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

void VirtualPipeErosion::addWaterRect(const int& x, const int& y, const int& width, const int& height, const double& amount)
  {
  for (int ycounter = height; ycounter > 0; ycounter--)
    for (int xcounter = 0; xcounter < width; xcounter++)
      read(xcounter, ycounter -1).addToWaterHeight(amount);
  }

pipeCell& VirtualPipeErosion::read(const int& x, const int& y)
  {
  if (x < w && y < h && x >= 0 && y >= 0)
    return (*readList)[x + y * w];//readList->at(x + y * w);
  else
    return nullcell;
    //return drain;
  }

pipeCell& VirtualPipeErosion::write(const int& x, const int& y)
  {
  if (x < w && y < h && x >= 0 && y >= 0)
    return (*writeList)[x + y * w];
  else
   return nullcell;
  // return drain;
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

void VirtualPipeErosion::generate(const double& magnification)
  {
  temporaryMap = new heightmap;
  temporaryMap->initMap(w, h);

  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      temporaryMap->infomap.push_back((Perlingen.GetValue((double)xcounter / 153.1f / magnification / 4.0f, (double)ycounter / 180.2f / magnification / 4.0f, 2.1f)));
      }

    temporaryMap->scale(0, 100);

  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      read(xcounter, ycounter).setTerrainHeight(temporaryMap->at(xcounter, ycounter));
      }

    for (int ycounter = 0; ycounter < h; ycounter++)
      for (int xcounter = 0; xcounter < w; xcounter++)
        {
        read(xcounter, ycounter).setSoilSlippage(max(1, abs(Perlingen.GetValue(xcounter / 102.5, ycounter / 102.5, 12.1)+0.5f)));
        }


    *writeList = *readList;

    double size = readList->size();
   for (int counter = 0; counter < size; counter++)
     initialTerrainHeight[counter] = (*readList)[counter].getTerrainHeight();
  }

void VirtualPipeErosion::renderSedimentCapacity()
  {
  ALLEGRO_COLOR brown = al_map_rgb(200,150,100);
  ALLEGRO_COLOR black = al_map_rgb(0,0,0);
  ALLEGRO_COLOR orange = al_map_rgb(255,185,0);

  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      renderMap.at(xcounter, ycounter) = read(xcounter, ycounter).sedimentCapacity;
      }
   // renderMap.scale(0,1);

    
    double value;

    al_set_target_bitmap(sedimentCapacityRender);
    al_lock_bitmap(al_get_target_bitmap(), al_get_bitmap_format(sedimentCapacityRender), ALLEGRO_LOCK_READWRITE);
    for (int ycounter = 0; ycounter < h; ycounter++)
      for (int xcounter = 0; xcounter < w; xcounter++)
        {
        value = renderMap.at(xcounter, ycounter);
        if (value >= 0)
          al_put_pixel(xcounter, ycounter, ColorMath::lerp(black, brown, min(1, value * 100.0f)));
        else
          al_put_pixel(xcounter, ycounter, ColorMath::lerp(black, orange, min(1, value * 100.0f)));
        }
      al_unlock_bitmap(al_get_target_bitmap());
  }

void VirtualPipeErosion::render()
  {
  double maxWaterDepth = 2;
  double waterDepth;

 // renderMap.initMap(w,h);
  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      renderMap.at(xcounter, ycounter) = read(xcounter, ycounter).getTerrainHeight();
      }
   // renderMap.scale(0,1);
    
    ALLEGRO_COLOR red; ALLEGRO_COLOR darkRed, blue, green;
    red = al_map_rgb(255,0,0);
    blue = al_map_rgb(0,0,200);
    ALLEGRO_COLOR darkBlue = al_map_rgb(0,0,50);
    ALLEGRO_COLOR brown = al_map_rgb(200,150,100);
    darkRed = al_map_rgb(50,0,0);
    green = al_map_rgb(0,120,0);
    ALLEGRO_COLOR white = al_map_rgb(255,255,255);
    ALLEGRO_COLOR black = al_map_rgb(0,0,0);
    ALLEGRO_COLOR buffercolor;
    //ALLEGRO_COLOR orange = al_map_rgb(255,102,0);

    al_set_target_bitmap(terrain);
    al_lock_bitmap(al_get_target_bitmap(), al_get_bitmap_format(terrain), ALLEGRO_LOCK_READWRITE);
    for (int ycounter = 0; ycounter < h; ycounter++)
      for (int xcounter = 0; xcounter < w; xcounter++)
        {
        //Draw the map.
        al_put_pixel(xcounter, ycounter, lerp(black, brown, min(1.0,renderMap.at(xcounter,ycounter)/(double)64)));
       // al_put_pixel(xcounter, ycounter, brown);
        // If there's water ...
        if (read(xcounter, ycounter).getWaterHeight() > 0)
          {
            waterDepth = read(xcounter, ycounter).getWaterHeight() / maxWaterDepth;
            // The deeper the darker.
            buffercolor = lerp(blue, darkBlue, min(1.0f, waterDepth));
           // buffercolor = lerp(white, buffercolor, 0.8);
            al_put_pixel(xcounter, ycounter, lerp(al_get_pixel(terrain, xcounter, ycounter), buffercolor, min(1.0f, waterDepth + 0.5)));
          }
 //       else
          
        }

      // Now add drop shadows

 /*     double drop = 100;
  double max_height = 0;
  double height = 0;
  double shadowfactor = 0;
  ALLEGRO_COLOR shadowBlack = al_map_rgb(40,40,40);

  for (int ycounter = 0; ycounter < h; ycounter++)
    {
    max_height = 0;
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      height = read(xcounter, ycounter).getTerrainHeight();
      if (height >= max_height)
        max_height = height;
      else
        {
        buffercolor = al_get_pixel(terrain, xcounter, ycounter);
        shadowfactor = (max_height - height) <= 0 ? 0 : 1 / (max_height - height);
        if (shadowfactor <= 0.8)
          shadowfactor+= 0.2;
        shadowfactor = shadowfactor > 1 ? 1 : shadowfactor;
        buffercolor = buffercolor * (shadowBlack * shadowfactor);
        al_put_pixel(xcounter, ycounter, shadowBlack);
        max_height = max_height-drop > height ? max_height-drop : height;
        }
      }
    }*/

      al_unlock_bitmap(al_get_target_bitmap());
  }

void VirtualPipeErosion::renderFraction()
  {
  //ALLEGRO_COLOR brown = al_map_rgb(200,150,100);
  ALLEGRO_COLOR black = al_map_rgb(0,0,0);
  ALLEGRO_COLOR red = al_map_rgb(255, 0,0);
  ALLEGRO_COLOR green = al_map_rgb(0,0, 255);

  
  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      renderMap.at(xcounter, ycounter) = read(xcounter, ycounter).getTerrainHeight() - initialTerrainHeight[xcounter + ycounter * w];
      }
    
    double value;
    const double divideBy = 5;

    al_set_target_bitmap(sedimentFraction);
    al_lock_bitmap(al_get_target_bitmap(), al_get_bitmap_format(sedimentFraction), ALLEGRO_LOCK_READWRITE);
    for (int ycounter = 0; ycounter < h; ycounter++)
      for (int xcounter = 0; xcounter < w; xcounter++)
        {
        value = renderMap.at(xcounter, ycounter);
        if (value < 0)
          al_put_pixel(xcounter, ycounter, lerp(black, red, renderMap.at(xcounter, ycounter) / divideBy ));
        else
          al_put_pixel(xcounter, ycounter, lerp(black, green, renderMap.at(xcounter, ycounter) / divideBy));
        }
      al_unlock_bitmap(al_get_target_bitmap());
  }

void VirtualPipeErosion::renderSediment()
  {
  ALLEGRO_COLOR white = al_map_rgb(255,255,255);
  ALLEGRO_COLOR black = al_map_rgb(0,0,0);
  ALLEGRO_COLOR orange = al_map_rgb(255,185,0);

  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      renderMap.at(xcounter, ycounter) = read(xcounter, ycounter).suspendedSediment;
      }
   // renderMap.scale(0,1);

    
    double value;

    al_set_target_bitmap(sedimentRender);
    al_clear_to_color(al_map_rgb(0,0,0));
    al_lock_bitmap(al_get_target_bitmap(), al_get_bitmap_format(sedimentRender), ALLEGRO_LOCK_READWRITE);
    for (int ycounter = 0; ycounter < h; ycounter++)
      for (int xcounter = 0; xcounter < w; xcounter++)
        {
        value = renderMap.at(xcounter, ycounter);
        if (value >= 0)
          al_put_pixel(xcounter, ycounter, ColorMath::lerp(black, white, min(1, value * 100.0f)));
        else
          al_put_pixel(xcounter, ycounter, ColorMath::lerp(black, orange, min(1, value * 100.0f)));
        }
      al_unlock_bitmap(al_get_target_bitmap());
  }

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
  else if (mode == 6)
    stepThroughSlippageCalc(startRow, endRow);
  else if (mode == 7)
    stepThroughMoveTerrain(startRow, endRow);
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
    return sedimentList[x + y * w];
  else
    return nullcell.suspendedSediment;
  }

////////////////
//
//  DirectXWindow functions
//
////////////////

// Gets all the heights and converts it to a DirectXWindow compatible format.
void VirtualPipeErosion::packageHeightmaps()
  {

  pipeCell* tempbuffer;
  ALLEGRO_COLOR brown = al_map_rgb(200,150,100);
  ALLEGRO_COLOR otherbrown = al_map_rgb(150,75,0);
 // ALLEGRO_COLOR blue = al_map_rgb(0,0,150);
  ALLEGRO_COLOR darkBlue = al_map_rgb(0,0,50);
  ALLEGRO_COLOR temp;
  //ColorMath::float3 bufferfloat;

  for(int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      tempbuffer = &read(xcounter, ycounter);
      outputHeightmap[xcounter + ycounter * w] = tempbuffer->getTerrainHeight();
      outputWatermap[xcounter + ycounter * w] = tempbuffer->getTotalHeight();
      
      temp = ColorMath::lerp(otherbrown, brown, tempbuffer->getTerrainHeight() / 100);
      temp = ColorMath::lerp(temp, darkBlue, min(1, tempbuffer->getWaterHeight() / 3));



      outputRGBMap[(xcounter + ycounter * w )* 3 + 0] = temp.r;
      outputRGBMap[(xcounter + ycounter * w )* 3 + 1] = temp.g;
      outputRGBMap[(xcounter + ycounter * w )* 3 + 2] = temp.b;



      /*
      outputRGBMap[(xcounter + ycounter * w )* 3 + 0] = 0.784f;
      outputRGBMap[(xcounter + ycounter * w )* 3 + 1] = 0.588f;
      outputRGBMap[(xcounter + ycounter * w )* 3 + 2] = 0.392f;*/
      }
/*
  for(int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      temp
      outputRGBMap[(xcounter + ycounter * w )* 3 + 0] = 0.784f;
      outputRGBMap[(xcounter + ycounter * w )* 3 + 1] = 0.588f;
      outputRGBMap[(xcounter + ycounter * w )* 3 + 2] = 0.392f;
     // waterDepth = read(xcounter, ycounter).getWaterHeight() / maxWaterDepth;
      //outputRGBMap[(xcounter + ycounter * w) * 3 + 2] = waterDepth > 0 ? 1.0f : 0;
      /*waterDepth = read(xcounter, ycounter).suspendedSediment * 10000 / 255;
      if (waterDepth > 0)
        outputRGBMap[(xcounter + ycounter * w )* 3 + 1] = 0;
      outputRGBMap[(xcounter + ycounter * w) * 3 + 0] = waterDepth > 0 ? waterDepth : 0;*/
      //}
  }

float* VirtualPipeErosion::getHeightmap()
  {
  return &outputHeightmap[0];
  }

float* VirtualPipeErosion::getRGBMap()
  {
  return &outputRGBMap[0];
  }

float* VirtualPipeErosion::getWatermap()
  {
  return &outputWatermap[0];
  }


////////////////
//
//  Tools.
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

void VirtualPipeErosionTools::randomRainInRegion(VirtualPipeErosion& thisErosion, const int& howMany, const double& howMuchRain, const int& x, const int& y, const int& w, const int& h)
  {
  int width = thisErosion.w;
  int height = thisErosion.h;

  for (int counter = 0; counter < howMany; counter++)
    {
    thisErosion.addWater(random(x, x + w), random(y, y + h), random(howMuchRain * 0.9, howMuchRain * 1.1)); 
    }
  }

void VirtualPipeErosionTools::removeRainInRegion(VirtualPipeErosion& thisErosion, const int& x, const int& y, const int& width, const int& height)
  {
  pipeCell* bufferCell;

  for (int ycounter = height; ycounter > 0; ycounter--)
    for (int xcounter = 0; xcounter < width; xcounter++)
      {
      bufferCell = &(thisErosion.read(xcounter, ycounter -1));
      bufferCell->setWaterHeight(0);
      bufferCell->suspendedSediment = 0;
      //fluxLeft = fluxRight = fluxTop = fluxBottom = 0;
      }
  }