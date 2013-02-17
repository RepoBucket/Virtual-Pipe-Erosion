#include "Engine.h"
#include <allegro5/allegro_primitives.h>
//#include "region.h"
#include <map>
//#include "layereddata.h"
#include "virtualPipes.h"
#include <boost/thread.hpp>

Engine DisplayEngine;
//region *thisRegion;

VirtualPipeErosion *Erosion;

int stepCounter;

bool Engine::EngineInit(map<int,bool> errormap)
  {
  al_init_primitives_addon();
  
  Erosion = new VirtualPipeErosion(256,256,1);

  //thisErosion->generateTest();
  Erosion->generate();
  //Erosion->addWater(0, 0, 10);
  Erosion->render();
  stepCounter = 0;

  return true;
  }



void Engine::Update()
  {
  // thisRegion->landmap;
  int h = Erosion->geth();  
 // Erosion->addWater(64, 0, 5);
  //Erosion->addWater(31, 0, 5);
  Erosion->addWater(90, 105, 5);
  Erosion->addWater(88, 43, 5);

  //Erosion->evaporate(0.7);
  
  Erosion->prepErosion(0.1);
  boost::thread flux1(&VirtualPipeErosion::operator(), boost::ref(Erosion), 0, h/4, 0);
  boost::thread flux2(&VirtualPipeErosion::operator(), boost::ref(Erosion), h/4, 2*h/4, 0);
  boost::thread flux3(&VirtualPipeErosion::operator(), boost::ref(Erosion), 2*h/4, 3*h/4, 0);
  boost::thread flux4(&VirtualPipeErosion::operator(), boost::ref(Erosion), 3*h/4, h, 0);

  flux1.join();
  flux2.join();
  flux3.join();
  flux4.join();

  boost::thread vector1(&VirtualPipeErosion::operator(), boost::ref(Erosion), 0, h/4, 1);
  boost::thread vector2(&VirtualPipeErosion::operator(), boost::ref(Erosion), h/4, 2*h/4, 1);
  boost::thread vector3(&VirtualPipeErosion::operator(), boost::ref(Erosion), 2*h/4, 3*h/4, 1);
  boost::thread vector4(&VirtualPipeErosion::operator(), boost::ref(Erosion), 3*h/4, h, 1);

  vector1.join();
  vector2.join();
  vector3.join();
  vector4.join();

  Erosion->operator()(0, h, 3);

  boost::thread erosion1(&VirtualPipeErosion::operator(), boost::ref(Erosion), 0, h/4, 4);
  boost::thread erosion2(&VirtualPipeErosion::operator(), boost::ref(Erosion), h/4, 2*h/4, 4);
  boost::thread erosion3(&VirtualPipeErosion::operator(), boost::ref(Erosion), 2*h/4, 3*h/4, 4);
  boost::thread erosion4(&VirtualPipeErosion::operator(), boost::ref(Erosion), 3*h/4, h, 4);

  erosion1.join();
  erosion2.join();
  erosion3.join();
  erosion4.join();

  Erosion->finishErosion();
  
 // Erosion->step(0.1);
  if (stepCounter > 2)
    {
    Erosion->render();
    stepCounter = 0;
    }
  else stepCounter++;
  //Erosion->render();
  }

void Engine::Render(ALLEGRO_DISPLAY *root)
  {
  al_draw_bitmap(Erosion->terrain, 10, 10, 0);
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