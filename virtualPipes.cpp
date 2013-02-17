#include "virtualPipes.h"
#include <cmath>
#include "noise.h"
#include "colors.h"


using namespace ColorMath;

pipeCell::pipeCell()
  {

  }

pipeCell::pipeCell(const int& newx, const int& newy, const double& cellSize) 
  : x(newx), y(newy), fluxLeft(0), fluxRight(0), fluxTop(0), fluxBottom(0),
  suspendedSediment(0), terrainHeight(0), waterHeight(0), lengthX(cellSize), lengthY(cellSize)
  , tempKc(15), dissolvingConstant(0.3), depositingConstant(0.3)
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

void pipeCell::addToWaterHeight(const double& moreHeight)
  {
  terrainHeight += moreHeight;
  }

double pipeCell::getWaterHeight() const
  {
  return waterHeight;
  }

void pipeCell::addToWaterHeight(const double& moreWater)
  {
  waterHeight += moreWater;
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

/////////////
//
//
//
//
/////////////

VirtualPipeErosion::VirtualPipeErosion(const int& width, const int& height, const double& cellSize)
  : w(width), h(height), gravityConstant(9.8), pipeCrossSectionalArea(1)
  {
  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      list1.push_back(pipeCell(xcounter, ycounter, cellSize));
  list2 = list1;
  readList = &list1;
  writeList = &list2;
  terrain = al_create_bitmap(w, h);
 /* readQueue = &queue1;
  writeQueue = &queue2;*/
  }

void VirtualPipeErosion::step(const double& time)
  {
  currentTimeStep = time;
  stepThroughFlux(0, h-1);
  stepThroughVector(0, h-1);
  cleanUp(0, h-1);
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
       write(xcounter, ycounter).sedimentCapacity = findSedimentCapacity(write(xcounter, ycounter));
       write(xcounter, ycounter).sedimentCapacity = findNewSediment(write(xcounter, ycounter));
      }
    }
  }

double VirtualPipeErosion::findNewSediment(const pipeCell& thisCell)
  {
  return bilinearSediment(thisCell.x - thisCell.flow.x, thisCell.y - thisCell.flow.y);
  }

double VirtualPipeErosion::bilinearSediment(const double& x, const double& y)
  {
  int basex = floor(x);
  int basey = floor(y);
  double lerp1 = read(basex, basey).suspendedSediment * (x - basex) + read(basex+1, basey).suspendedSediment * (x+1 - basex);
  double lerp2 = read(basex, basey+1).suspendedSediment * (x - basex) + read(basex+1, basey+1).suspendedSediment * (x+1 - basex);
  return lerp1 * (y - basey) + lerp2 * (y+1 - basey);
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
  return thisCell.getSedimentCapacityConstant() * min(0.01,getSine(thisCell)) * thisCell.flow.length;
  }

double VirtualPipeErosion::getSine(const pipeCell& thisCell)
  {
  double angle1 = (write(thisCell.x - 1, thisCell.y).getTerrainHeight() - write(thisCell.x + 1, thisCell.y).getTerrainHeight())/2;
  double angle2 = (write(thisCell.x, thisCell.y - 1).getTerrainHeight() - write(thisCell.x, thisCell.y+1).getTerrainHeight())/2;
  angle1 = atan(angle1);
  angle2 = atan(angle2);
  return sin((angle1 + angle2)/2);
  }

void VirtualPipeErosion::erosionDeposition(pipeCell& thisCell)
  {
  if (thisCell.sedimentCapacity > thisCell.suspendedSediment)
    {
    double movedSediment = thisCell.getTerrainHeight() - thisCell.dissolvingConstant * (thisCell.sedimentCapacity - thisCell.suspendedSediment);
    thisCell.addToTerrainHeight(-movedSediment);
    thisCell.suspendedSediment += movedSediment;
    }
  else
    {
    double depositedSediment = thisCell.getTerrainHeight() + thisCell.depositingConstant * (thisCell.suspendedSediment - thisCell.sedimentCapacity);
    thisCell.addToTerrainHeight(depositedSediment);
    thisCell.suspendedSediment -= depositedSediment;
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
      temporaryMap->at(xcounter, ycounter) = Perlingen.GetValue((double)xcounter / 931.1f, (double)ycounter / 931.0f, 0.1f);
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
    renderMap.scale(0,1);

    ALLEGRO_COLOR red; ALLEGRO_COLOR darkRed, blue;
    red = al_map_rgb(255,0,0);
    blue = al_map_rgb(0,0,255);
    darkRed = al_map_rgb(50,0,0);

    al_set_target_bitmap(terrain);
    al_lock_bitmap(al_get_target_bitmap(), al_get_bitmap_format(terrain), ALLEGRO_LOCK_READWRITE);
    for (int ycounter = 0; ycounter < h; ycounter++)
      for (int xcounter = 0; xcounter < w; xcounter++)
        {
        if (read(xcounter, ycounter).getWaterHeight() > 0)
          al_put_pixel(xcounter, ycounter, blue);
        else
          al_put_pixel(xcounter, ycounter, heightmap::lerp(darkRed, red, renderMap.at(xcounter,ycounter)));
        }
      al_unlock_bitmap(al_get_target_bitmap());

  }

void VirtualPipeErosion::prepErosion(const double& time)
  {
  currentTimeStep = time;
  }

void VirtualPipeErosion::operator()(const int& startRow, const int& endRow, const int& mode)
  {
  if (mode == 0)
    stepThroughFlux(startRow, endRow);
  else if (mode == 1)
    stepThroughVector(startRow, endRow);
  else if (mode == 3)
    cleanUp(startRow, endRow);
  else
    return;
  }

void VirtualPipeErosion::finishErosion()
  {
  swapMaps();
  }

double VirtualPipeErosion::geth()
  {
  return h;
  }