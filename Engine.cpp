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
  
  Erosion = new VirtualPipeErosion(128,128,1);

  //thisErosion->generateTest();
  Erosion->generate();
  Erosion->addWater(0, 0, 10);
  Erosion->render();
  stepCounter = 0;

  return true;
  }



void Engine::Update()
  {
  // thisRegion->landmap;
  int h = Erosion->geth();  
  //Erosion->addWater(8, 64, 1);

 /* Erosion->prepErosion(0.1);
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

  Erosion->finishErosion();*/
  
  Erosion->step(0.1);
  if (stepCounter > 10)
    {
    Erosion->render();
    stepCounter = 0;
    }
  else stepCounter++;
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