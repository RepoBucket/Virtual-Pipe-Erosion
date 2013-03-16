#include "Engine.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
//#include "region.h"
#include <map>
//#include "layereddata.h"
#include "virtualPipes.h"
#include <boost/thread.hpp>

#include <iostream>
#include <string>

#include "include\DirectXWindow.h"
#include "include\Heightmap.h" 

Engine DisplayEngine;
DirectXWindow* p3DWindow;
Heightmap* p3DHeightmap, *p3DWatermap;

VirtualPipeErosion *Erosion;
VirtualPipeErosionTools toolbox;
int counter;
int rendercounter;
bool render;

string otherbuffer;

bool Engine::EngineInit(map<int,bool> errormap)
  {
  al_init_primitives_addon();
  al_init_image_addon();

  ALLEGRO_BITMAP* inputTerrain, *inputWater;
  string str;
  string buffer;

  cout << "Input file base name. Eg, if you have a pair of files this_terrain.bmp and this_water.bmp, input ""this"".\nPress any key if no file is to be loaded.\n";
  cin >> str;

  buffer = str;
  buffer.append("_terrain.bmp");
  inputTerrain = al_load_bitmap(buffer.c_str());
  buffer = str;
  buffer.append("_water.bmp");
  inputWater = al_load_bitmap(buffer.c_str());
  if (inputTerrain && inputWater) // If both loaded successfully
    Erosion = new VirtualPipeErosion(inputTerrain, inputWater, 10, 100);
  else
    Erosion = new VirtualPipeErosion(128, 128, 1);

 /* if (POD.inputBitmap)
    Erosion->readFromBitmap(POD.inputBitmap, POD.scaleConstant);
  else
    Erosion->generateV();*/

  counter = 0;
  //thisErosion->generateTest();
 Erosion->generateV();
//  Erosion->addWater(0, 0, 100);
 //Erosion->addWaterRect(0, 0, 256, 256, 10);
  Erosion->render();
  Erosion->packageHeightmaps();

  str.assign("Testing");
  p3DWindow = new DirectXWindow(str, 0, 640, 640, 0, 0);
  p3DHeightmap = new Heightmap();
  p3DWatermap = new Heightmap();
  p3DWindow->CreateColoredHeightmap(p3DHeightmap, Erosion->w, Erosion->h, Erosion->getHeightmap(), Erosion->getRGBMap());
  p3DWindow->CreateWaterHeightmap(p3DWatermap, Erosion->w, Erosion->h, Erosion->getWatermap());

  return true;
  }

void Engine::Update()
  {
  if (counter < 20000)
    {
    otherbuffer.assign(to_string((long double)counter));
    toolbox.randomRain(*Erosion, 20, 0.1f);
    //toolbox.randomRainInRegion(*Erosion, 50, 0.1f, 0, 64, 255, 64);
    //Erosion->addWaterRect(0, 0, 256, 256, 0.01f);
    //Erosion->addWater(0, 0, 0.4f);
    counter++;
    }
  else
    Erosion->evaporate(0.9f);
   // Erosion->evaporate(0.999);

  Erosion->step(0.05f);
  toolbox.removeRainInRegion(*Erosion, 0, 0, 1, 255);
  toolbox.removeRainInRegion(*Erosion, 0, 0, 255, 1);

//  Erosion->evaporate(0.99);
  // Erosion->singlethreaded_step(0.05);

  if (rendercounter > 1)
    {
      Erosion->render();
      Erosion->renderSediment();
      Erosion->renderSedimentCapacity();
      Erosion->renderFraction();
      Erosion->packageHeightmaps();
      rendercounter = 0;
    }
  else
    rendercounter++;

 /* if (render)
    Erosion->render();
  render = !render;*/
  }

void Engine::Render(ALLEGRO_DISPLAY *root)
  {
  al_clear_to_color(al_map_rgb(255,255,255));
  al_draw_bitmap(Erosion->terrain, 0, 0, 0);
  al_draw_bitmap(Erosion->sedimentCapacityRender, Erosion->h, 0, 0);
  al_draw_bitmap(Erosion->sedimentRender, 0, Erosion->h, 0);
  al_draw_bitmap(Erosion->sedimentFraction, Erosion->h, Erosion->h, 0);
  p3DHeightmap->ModifyHeightsAndColors(Erosion->getHeightmap(), Erosion->getRGBMap());
  p3DWatermap->ModifyHeights(Erosion->getWatermap());
  p3DWindow->step(0.05);
  al_draw_text(font, al_map_rgb(0,0,0), 10, 512, ALLEGRO_ALIGN_LEFT, otherbuffer.c_str());
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