#pragma once
#include "vector3.h"
#include "heightmap.h"
#include <vector>
#include <queue>
#include <string>
#include <map>
#include "allegro5\allegro5.h"
#include "enums.h"



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
  double getHeight() const;
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
  virtual double getHeight() const;
  virtual double getTotalHeight() const;

  bool updated;
  bool heightChecked;
  bool nullcell;
  };

struct nullCell: public cell
  {
  nullCell() {height = 10000;}
  double getHeight() const  {return height;};
  double getTotalHeight() const {return height;};
  };

struct materialDictionary
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

struct boolVector //A simple wrapper for vector<bool> to support writing to an (x,y) coordinate.
  {
  boolVector(const int& width, const int& height);
  vector<bool> boolvector;
  bool at(const int& x, const int& y);
  void write(const int& x, const int& y, const bool& input);
  void clear(); //sets all to false.
  const int w;
  const int h;
  bool nullcell;
  };

class ErosionHeightmap
  {
  public:
  ALLEGRO_BITMAP* terrain;
  ALLEGRO_BITMAP* water;
  ErosionHeightmap(const int& width, const int& height);
  void generate(const int &layers);
  void generateV(); //Test. Generates a V-shaped heightmap.
  void generateTest(); // ad hoc
  cell& at(const int & x, const int & y);
  cell& write(const int& x, const int &y); //To the write map.
  void addWater(const int& x, const int& y, const double& thisFluid);
  void step(); //Do a step of the simulation.
  void swapMaps(); //Swap the current map and the write-to map.
  void swapQueues();
  void render();
  int w, h;
  materialDictionary matDict;

  protected:
    nullCell nullcell;

    void resetUpdatedFlags();
    double deltaIn(cell& thisCell, const cell& neighborCell); //delta I
    //double deltaIn2(cell& thisCell, const cell& neighborCell); //delta I, without the quickfix for slow speed flow.
    double adjustedHeight(cell & input);
    double adjustedHeight2(cell & input); //adjustedHeight without the quickfix.
    vector3 averageGradient(cell & input, const int& x, const int& y);
    void distributeByGradient(cell & input, const int& x, const int& y);
    vector3 normal(cell& thisCell, const int& x, const int& y);
    vector3 normal(const vector3& gradient);
    vector3 normal(const double& x, const double& y);
    vector3 equation16 (vector3 input);
    vector3 equation16_positive (vector3 input);
    void distribute(cell& thisCell, const pair<double, double>& coords);
    void distribute(cell &thisCell, const vector3& originalVector, const pair<double,double>& originalCoords, const pair<double,double>& targetCoords); //Newer version of distribution.
    void dampVelocity();
    void clearCell();
    void setCellToClear(cell* clearerCell);

    vector<cell> heightmap1;
    vector<cell> heightmap2;
    vector<cell>* currentMap;
    vector<cell>* writeToMap;

    heightmap renderMap; //Have to scale.
    boolVector waterMap;
    cell* cellToClear;

    queue<pair<int, int>> updateQueue1;
    queue<pair<int, int>> updateQueue2;
    queue<pair<int, int>>* currentQueue;
    queue<pair<int, int>>* writeToQueue;

    double thresholdSpeed;
  };
