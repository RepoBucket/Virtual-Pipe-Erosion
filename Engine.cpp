#include "Engine.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
//#include "region.h"
#include <map>
//#include "layereddata.h"
#include "virtualPipes.h"
//#include "gpgpu_virtualPipes.h"
#include <boost/thread.hpp>

#include <iostream>
#include <string>

#include "include\DirectXWindow.h"
#include "include\Heightmap.h" 

Engine DisplayEngine;

DirectXWindow* p3DWindow;
Heightmap* p3DHeightmap, *p3DWatermap;
//gpgpu_VirtualPipe* Pipe;

VirtualPipeErosion *Erosion;
VirtualPipeErosionTools toolbox;

int rendercounter;
bool render;

string otherbuffer;
int counter, howMuchRain;
int maxSteps;
float rainAmount;


bool Engine::EngineInit(map<int,bool> errormap)
  {
  al_init_primitives_addon();
  al_init_image_addon();

  string str;
  string buffer;
  char input;

  cout << "HOW BIG MAP?! 1 for 256x256, 2 for 128x128. There are no other options!\n(Actually, there's 64x64 mode if your computer is actually a potato in disguise. Input 3 for that.)\n";
  bool validinput = false;
  while(!validinput)
    {
    cin >> input;
    if (input == '1'|| input == '2'|| input == '3' || input == '4')
      validinput = true;
    else
      cout << "I'M GOING TO KEEP PESTERING YOU UNTIL YOU INPUT EITHER 1 OR 2 :D\n";
    }
  
  if (input == '1')
    Erosion = new VirtualPipeErosion(256, 256, 1);
  else if (input == '2')
    Erosion = new VirtualPipeErosion(128, 128, 1);
  else if (input == '4')
    Erosion = new VirtualPipeErosion(512, 512, 1);
  else
    Erosion = new VirtualPipeErosion(64, 64, 1);

  /*
  if (POD.inputBitmap)
    Erosion->readFromBitmap(POD.inputBitmap, POD.scaleConstant);
  else*/
   // Erosion->generate();

  counter = 0;
  //thisErosion->generateTest();
  cout << "Magnification? (1 is default, <1 is zoomed out, >1 is zoomed in. 0 is \"please kill my computer\".)\n";
  cin >> rainAmount;
 Erosion->generate(rainAmount);
 cout << "Would you like to customize? Enter Y or N. Default is N. (that means pressing any key will make you say no.)\n";
   char customize;
   cin >> customize;
   int threads = 4;
   if (customize == 'Y' || customize == 'y')
     {
     cout << "How many steps do you want to simulate? Default is 20000 steps. It takes a while, actually.\n";
     cin >> maxSteps;
     cout << "How many drops of rain per step do you want to add? Default is 20.\n";
     cin >> howMuchRain;
     cout << "How much water per drop do you want to add? Default is 0.4.\n";
     cin >> rainAmount;
     cout << "How many threads, in a power of two? eg, 2^x, where you input x. Default 4.\n";
     cin >> threads;
     }
   else
     {
     cout << "Going with the defaults: 20000 steps, 20 rain drops per step, and 0.4 water added per step.\n";
     maxSteps = 20000;
     howMuchRain = 20;
     rainAmount = 0.4f;
     }

  Erosion->render();
  Erosion->packageHeightmaps();
  Erosion->setThreads(threads);

  str.assign("Erosion");
  p3DWindow = new DirectXWindow(str, 0, 640, 640, 0, 0);
  p3DHeightmap = new Heightmap();
  p3DWatermap = new Heightmap();
  p3DWindow->CreateColoredHeightmap(p3DHeightmap, Erosion->w, Erosion->h, Erosion->getHeightmap(), Erosion->getRGBMap());
  p3DWindow->CreateWaterHeightmap(p3DWatermap, Erosion->w, Erosion->h, Erosion->getWatermap());

  cout << "A few instructions before you watch water flow and do stuff.\nYou can scroll around with WASD. If you press 'l', you can lock your mouse and rotate the camera.\n"
    << "Pressing T makes the water look more like aluminium foil. Pressing Shift+T makes it look less like foil. This also is useful when you are impatient for the simulation to finish and want to see how the water is affecting your landscape.\n";

  return true;
  }

void Engine::Update()
  {
  
  if (counter < maxSteps)
    {
    otherbuffer.assign("Steps: ");
    otherbuffer.append(to_string((long double)counter));
    toolbox.randomRain(*Erosion, howMuchRain, rainAmount);
    //toolbox.randomRainInRegion(*Erosion, 50, 0.1f, 0, 64, 255, 64);
    //Erosion->addWaterRect(0, 0, 256, 256, 0.01f);
    //Erosion->addWater(0, 0, 0.4f);
    counter++;
    }
  else
    Erosion->evaporate(0.9f);
   // Erosion->evaporate(0.999);

  Erosion->step(0.05f);
  toolbox.removeRainInRegion(*Erosion, 0, 0, 1, Erosion->w - 1); // Left side
  toolbox.removeRainInRegion(*Erosion, 0, 0, Erosion->w - 1, 1); // Top side
  toolbox.removeRainInRegion(*Erosion, 0, Erosion->w - 1, Erosion->w - 1, 1); // Bottom side
  toolbox.removeRainInRegion(*Erosion, Erosion->w - 1, 0, 1, Erosion->w - 1); // Right side

//  Erosion->evaporate(0.99);
  // Erosion->singlethreaded_step(0.05);

      Erosion->render();
      Erosion->renderSediment();
      Erosion->renderSedimentCapacity();
      Erosion->renderFraction();
      Erosion->packageHeightmaps();
      rendercounter = 0;

 /* if (render)
    Erosion->render();
  render = !render;*/
 /* if (counter > 0)
      {
      Pipe->randomRain(10, 0.1f);
      Pipe->step(0.001);
      counter--;
      }*/

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

 /* otherbuffer.assign(to_string((long double)counter));
  al_draw_text(font, al_map_rgb(0,0,0), 10, 512, ALLEGRO_ALIGN_LEFT, otherbuffer.c_str());
  p3DHeightmap->ModifyHeightsAndColors(Pipe->getTerrain(), Pipe->getRGB());
  p3DWatermap->ModifyHeights(Pipe->getWater());
  p3DWindow->step(0.001);*/
    

 // al_draw_bitmap(thisRegion->landmap, 0, 0, 0);
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