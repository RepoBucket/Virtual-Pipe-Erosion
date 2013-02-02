#include "Engine.h"
#include <allegro5/allegro_primitives.h>
#include "region.h"
#include <map>

Engine DisplayEngine;
region *thisRegion;

bool Engine::EngineInit(map<int,bool> errormap)
  {
  al_init_primitives_addon();
  thisRegion = new region(640, 640);
  thisRegion->generateTopography();
  thisRegion->tempRender();
  if (thisRegion->landmap != 0) return true;
  else return false;
  }

void Engine::Update()
  {
  thisRegion->landmap;
  }

void Engine::Render(ALLEGRO_DISPLAY *root)
  {
  
  al_draw_bitmap(thisRegion->landmap, 0, 0, 0);
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