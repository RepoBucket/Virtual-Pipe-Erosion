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

#undef max
#undef min

// Implement advanced stuff later.
/*struct material
  {
  int ID;
  double sedimentCapacity;
  double deposition;
  double dissolving;
  string name;
  };

struct layer
  {
  map<int, material>::iterator mat; //Iterator to a lookup table.
  double height;
  };

struct transientLayer
  {
  vector<map<int,material>::iterator> materials;
  map<int, double> heights;
  double getHeight() const;
  };*/

struct pipeCell
  {
  pipeCell();
  pipeCell(const int& newx, const int& newy, const double& cellSize);

  int x, y; // 'Where am I?'

  // The simple doubles for height is to be replaced later by a more advanced model.
  virtual double getTerrainHeight() const;
  virtual double getWaterHeight() const;
  virtual double getTotalHeight() const;
  double getSedimentCapacityConstant() const;
  double sedimentCapacity;
  double dissolvingConstant;
  double depositingConstant;

  // Just because the border cells are different.
  virtual double getAngleHeight(const pipeCell& thisCell) const;

  void setWaterHeight(const double& newWaterHeight);
  void setTerrainHeight(const double& newTerrainHeight);
  double netFlux() const;
  double fluxLeft, fluxRight, fluxTop, fluxBottom; // The outflow flux.
  double suspendedSediment; // Probably will be replaced later by a more advanced model.
  vector3 flow;
  double cellArea() const;
  double cellLength() const;
  void scaleByK(const double& K);

  void addToWaterHeight(const double& moreWater);
  void addToTerrainHeight(const double& moreHeight);
  
  protected:
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

class VirtualPipeErosion
  {
  public:
    VirtualPipeErosion(const int& width, const int& height, const double& cellSize, const bool& random = false);
    void addWater(const int& x, const int& y, const double& amount);
    pipeCell& read(const int& x, const int& y);
    pipeCell& write(const int& x, const int& y);
    void step(const double& time);
    void evaporate(const double& amount);
    
    void render();

    //For threadedness
    void prepErosion(const double& time); // Sets time.
    void operator()(const int& startRow, const int& endRow, const int& mode);
    void finishErosion(); //Swaps maps.
    //
    
    void generate();
    void generateV();
    ALLEGRO_BITMAP* terrain;

    double geth();

    double erodeTimer;
    double maxErodeTimer;

  private:
    void stepThroughErosion(const int& startRow, const int& endRow);
    void stepThroughFlux(const int& startRow, const int& endRow); // For multithreading.
    void stepThroughVector(const int& startRow, const int& endRow);
    void calculateFlux(pipeCell& thisCell); //Equation 2, for all the fluxes.
    void cleanUp(const int& startRow, const int& endRow);
    double heightDifference(const int& x1, const int& y1, const int& x2, const int& y2);
    double scalingK(const pipeCell& thisCell);
    double getSine(const pipeCell& thisCell);
    double findNewSediment(const pipeCell& thisCell);
    double bilinearSediment(const double& x, const double&y);
    heightmap* temporaryMap; //Use in generation.
    void erosionDeposition(pipeCell& thisCell);

    double findSedimentCapacity(const pipeCell& thisCell);

    noise::module::Perlin Perlingen;
    int w, h;
    double currentTimeStep;
    double max(const double& left, const double& right);
    double min(const double& left, const double& right);
    void swapMaps();
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

    heightmap renderMap;
    WallCell nullcell;

    /*queue<pair<int,int>> queue1;
    queue<pair<int,int>> queue2;
    queue<pair<int,int>>* readQueue;
    queue<pair<int,int>>* writeQueue;*/
            
  };