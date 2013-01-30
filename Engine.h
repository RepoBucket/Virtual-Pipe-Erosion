#pragma once

#include "AllegroEngine.h"

using namespace std;

class Engine: public AllegroEngine
{
  bool EngineInit(map<int,bool> errormap);
	void Update();
	void Render(ALLEGRO_DISPLAY *root);
	void EngineEnd();
  //
  void Resize(int new_w,int new_h);
  void KeyUp(const int &key,const int &unicode, const int &mods);
  void KeyDown(const int &key,const int &unicode, const int &mods, const bool &repeated = 0);
  void MouseMoved(const int &iX,const int &iY,const int &iZ, const int &iRelX, const int &iRelY,const int &iRelZ);
  void MouseButtonUp(const int &iButton, const int &iX,const int &iY,const int &iZ, const int &iRelX, const int &iRelY,const int &iRelZ);
  void MouseButtonDown(const int &iButton, const int &iX,const int &iY,const int &iZ, const int &iRelX, const int &iRelY,const int &iRelZ);
  void TimerTick(ALLEGRO_TIMER *timer, int count);
};

extern Engine DisplayEngine;