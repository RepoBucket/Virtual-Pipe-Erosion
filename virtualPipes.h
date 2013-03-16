#pragma once
#include "enums.h"
//#include <string>
//#include <map>
#include <vector>
#include "vector3.h"
//#include <queue>
#include "allegro5/allegro5.h"
using namespace std;
#include "heightmap.h"
#include <limits>
//#include <map>

#include <boost/random/mersenne_twister.hpp>

#undef max
#undef min

struct pipeCell
  {
  pipeCell();
  pipeCell(const int& newx, const int& newy, const double& cellSize);

  int x, y; // 'Where am I?'

  // The simple doubles for height is to be replaced later by a more advanced model.
  virtual double getTerrainHeight() const;
  virtual double getWaterHeight() const;
  virtual double getTotalHeight() const;
  double getSoilSlippage() const; // This would be the threshold.
  double getSedimentCapacityConstant() const;
  double sedimentCapacity;
  double dissolvingConstant;
  double depositingConstant;

  // Just because the border cells are different.
  virtual double getAngleHeight(const pipeCell& thisCell) const;

  void setWaterHeight(const double& newWaterHeight);
  void setTerrainHeight(const double& newTerrainHeight);
  void setSoilSlippage(const double& newSlippage);
  virtual double netFlux() const;
  double fluxLeft, fluxRight, fluxTop, fluxBottom; // The outflow flux.
  double suspendedSediment; // Probably will be replaced later by a more advanced model.
  vector3 flow;
  double cellArea() const;
  double cellLength() const;
  void scaleByK(const double& K);

  void addToWaterHeight(const double& moreWater);
  void addToTerrainHeight(const double& moreHeight);

  bool hasBeenEroded;
  
  protected:
/*    vector<layer> layers;
    transientLayer transientlayer;*/
    double soilSlippage;
    double terrainHeight;
    double waterHeight;
    double lengthX, lengthY; // Length of the sides.
    double tempKc; //sediment capacity
  };

class WallCell : public pipeCell //Acts as a wall.
  {
  public:
    WallCell(): pipeCell(0,0,1){}
    virtual double getTerrainHeight() const {return numeric_limits<double>::max();}
    virtual double getWaterHeight() const {return 0;}
    virtual double getTotalHeight() const {return numeric_limits<double>::max();}
    virtual double getAngleHeight(const pipeCell& thisCell) const;
  };

class drainCell : public pipeCell //Throws away water.
  {
  public:
    drainCell() : pipeCell(0,0,1){}
    virtual double getTerrainHeight() const {return 0;}
   // virtual double netFlux() const {return 0;}
    void clear();
  };

class VirtualPipeErosion
  {
  public:
    VirtualPipeErosion(const int& width, const int& height, const double& cellSize, const bool& random = false);
    VirtualPipeErosion(ALLEGRO_BITMAP* inputTerrain, ALLEGRO_BITMAP* inputWater, const double& minScale, const double& maxScale);
    void addWater(const int& x, const int& y, const double& amount);
    void addWaterRect(const int& x, const int& y, const int& width, const int& height, const double& amount);
    pipeCell& read(const int& x, const int& y);
    pipeCell& write(const int& x, const int& y);
    double& sedimentAt(const int& x, const int& y);
    void step(const double& time);
    void singlethreaded_step(const double& time);
    void evaporate(const double& amount);
    
    void render();
    void renderSedimentCapacity();
    void renderSediment();
    void renderFraction();

    //For threadedness
   // void prepErosion(const double& time); // Sets time.
    void operator()(const int& startRow, const int& endRow, const int& mode);
    void finishErosion(); //Swaps maps.
    //
    
    void generate();
    void generateV();
    ALLEGRO_BITMAP* terrain;
    ALLEGRO_BITMAP* sedimentCapacityRender;
    ALLEGRO_BITMAP* sedimentRender;
    ALLEGRO_BITMAP* sedimentFraction;

   // double geth();

    double erodeTimer;
    double maxErodeTimer;
    void updateSedimentMap(const int& startRow, const int& endRow);
    void swapMaps();
    const int w, h;

    //For use with DirectXWindow
    void packageHeightmaps();
    float* getHeightmap();
    float* getWatermap();
    float* getRGBMap();

  private:
    void stepThroughErosion(const int& startRow, const int& endRow);
    void stepThroughFlux(const int& startRow, const int& endRow); // For multithreading.
    void stepThroughVector(const int& startRow, const int& endRow);
    void stepThroughTransport(const int& startRow, const int& endRow);
   // void stepThroughSlippage(const int& startRow, const int& endRow);
    
    void calculateFlux(pipeCell& thisCell); //Equation 2, for all the fluxes.
    void cleanUp(const int& startRow, const int& endRow);
    double heightDifference(const pipeCell& currentCell, const int& x2, const int& y2);
    double scalingK(const pipeCell& thisCell);
    double getSine(const pipeCell& thisCell);
    double findNewSediment(const pipeCell& thisCell);
    double bilinearSediment(const double& x, const double&y);
    heightmap* temporaryMap; //Use in generation.
    void erosionDeposition(pipeCell& thisCell);
   // void updateTempTerrain(); // Write the current terrain heights to the render vector. For use in slippage calcs.

    double findSedimentCapacity(const pipeCell& thisCell);

    noise::module::Perlin Perlingen;
    double currentTimeStep;
    double max(const double& left, const double& right);
    double min(const double& left, const double& right);
    
   // void swapQueues();
    double netVolumeChange(const pipeCell& thisCell);
    double updateWaterHeight(const pipeCell& inputCell);
    const double gravityConstant;
    double pipeCrossSectionalArea;
    vector3 findFlowVector(const pipeCell& thisCell);

    vector<pipeCell> list1;
    vector<pipeCell> list2;
    vector<pipeCell>* readList;
    vector<pipeCell>* writeList;
    vector<double> sedimentList;
    vector<float> initialTerrainHeight;
    

    heightmap renderMap;
    WallCell nullcell;
    drainCell drain;

    /////
    vector<float> outputHeightmap;
    vector<float> outputRGBMap;
    vector<float> outputWatermap;
  };

class VirtualPipeErosionTools
  {
  public:
    void randomRain(VirtualPipeErosion& thisErosion, const int& howMany, const double& howMuchRain);
    void randomRainInRegion(VirtualPipeErosion& thisErosion, const int& howMany, const double& howMuchRain, const int& x, const int& y, const int& w, const int& h);
    void removeRainInRegion(VirtualPipeErosion& thisErosion, const int& x, const int& y, const int& width, const int& height);
  private:
    boost::random::mt19937 rng;
    int random(const int& min, const int& max);
    double random(const double& min, const double& max);
  };