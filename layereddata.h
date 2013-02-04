#pragma once
#include "physicalvector.h"
#include <vector>
#include <queue>
#include <string>
#include <map>
#include "allegro5\allegro5.h"
#include "heightmap.h"

enum materialID {NOTHING = 0, SOFTMAT, MEDIUMMAT, HARDMAT};

struct material
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
  double getHeight();
  };

struct cell
  {
  cell();
  vector3 acceleration;
  vector3 velocity;
  vector<pair<vector3,double>> inputs; // <velocity, fluid>
  void calculateVelocity();

  double fluid;

  vector<layer> layers;
  transientLayer sediments;
  double height;
  double getHeight();
  double getTotalHeight();

  bool updated;
  bool heightChecked;
  bool nullcell;
  };

class materialDictionary
  {
  public:
    materialDictionary()
      {
      material temp;
      temp.ID = HARDMAT; temp.sedimentCapacity = 3; temp.deposition = 0.1; temp.dissolving = 0.05; temp.name.assign("Hard material");
      dict.insert(pair<int, material>(temp.ID, temp));

      temp.ID = MEDIUMMAT; temp.sedimentCapacity = 15; temp.deposition = 0.5; temp.dissolving = 0.3; temp.name.assign("Medium material");
      dict.insert(pair<int, material>(temp.ID, temp));

      temp.ID = SOFTMAT; temp.sedimentCapacity = 20; temp.deposition = 0.5; temp.dissolving = 0.5; temp.name.assign("Soft material");
      dict.insert(pair<int, material>(temp.ID, temp));
      }

    map<int, material>::iterator lookup(const int & ID);
    map<int, material> dict;
  };

class ErosionHeightmap
  {
  public:
  ALLEGRO_BITMAP* terrain;
  ALLEGRO_BITMAP* water;
  ErosionHeightmap(const int& width, const int& height);
  void generate(const int &layers);
  cell& at(const int & x, const int & y);
  cell& write(const int& x, const int &y); //To the write map.
  void addWater(const int& x, const int& y, const double& fluid);
  void step(); //Do a step of the simulation.
  void swapMaps(); //Swap the current map and the write-to map.
  void swapQueues();
  void render();
  int w, h;
  materialDictionary matDict;

  protected:
    cell nullcell;

    void resetUpdatedFlags();
    double deltaIn(cell& thisCell, const cell& neighborCell); //delta I
    double adjustedHeight(cell & input);
    vector3 averageGradient(cell & input, const int& x, const int& y);
    void distributeByGradient(cell & input, const int& x, const int& y);
    vector3 normal(cell& thisCell, const int& x, const int& y);
    vector3 normal(const vector3& gradient);
    void distribute(cell& thisCell, const pair<double, double>& coords);
    void dampVelocity();

    vector<cell> heightmap1;
    vector<cell> heightmap2;
    vector<cell>* currentMap;
    vector<cell>* writeToMap;

    heightmap renderMap; //Have to scale.
    vector<bool> waterMap;

    queue<pair<int, int>> updateQueue1;
    queue<pair<int, int>> updateQueue2;
    queue<pair<int, int>>* currentQueue;
    queue<pair<int, int>>* writeToQueue;

    double thresholdSpeed;
  };
