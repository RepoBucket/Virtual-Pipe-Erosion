#include "Engine.h"
#include <allegro5/allegro_primitives.h>
#include "region.h"
#include <map>
#include "layereddata.h"

Engine DisplayEngine;
//region *thisRegion;
ErosionHeightmap *thisErosion;

bool Engine::EngineInit(map<int,bool> errormap)
  {
  al_init_primitives_addon();
  /*thisRegion = new region(640, 640);
  thisRegion->generateTopography();
  thisRegion->tempRender();
  if (thisRegion->landmap != 0) return true;
  else return false;*/
  thisErosion = new ErosionHeightmap(2,1);
  thisErosion->generateTest();
 /* thisErosion->addWater(64,1,10);
  thisErosion->addWater(12,23,10);
  thisErosion->addWater(72,4,10);*/
 /// thisErosion->step();
    thisErosion->addWater(0, 0, 10);
  thisErosion->render();
  

  return true;
  }



void Engine::Update()
  {
  // thisRegion->landmap;
  // thisErosion->addWater(1,1,10);
    

  thisErosion->step();
  thisErosion->render();
  }

void Engine::Render(ALLEGRO_DISPLAY *root)
  {
  al_draw_bitmap(thisErosion->terrain, 0, 0, 0);
  //al_draw_bitmap(thisRegion->landmap, 0, 0, 0);
  }

void Engine::EngineEnd()
  {
  }

void Engine::KeyUp(const int &key,const int &unicode, const int &mods)
  {
  }

void Engine::KeyDown(const int &key,const int &unicode, const int &mods, const bool &repeated)
  {
  }

void Engine::MouseMoved(const int &iX,const int &iY,const int &iZ, const int &iRelX, const int &iRelY,const int &iRelZ)
  {
  }

void Engine::MouseButtonUp(const int &iButton, const int &iX,const int &iY,const int &iZ, const int &iRelX, const int &iRelY,const int &iRelZ)
  {
  }

void Engine::MouseButtonDown(const int &iButton, const int &iX,const int &iY,const int &iZ, const int &iRelX, const int &iRelY,const int &iRelZ)
  {
  }

void Engine::TimerTick(ALLEGRO_TIMER *timer, int count)
  {
  }