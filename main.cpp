#include "Engine.h"
#include <iostream>
#include <string>
//#include "gpgpu_virtualPipes.h"
#include "include\DirectXWindow.h"
#include "include\Heightmap.h" 

using namespace std;

int main(int argc, char **argv)
  {
  //gpgpu_VirtualPipe Pipe(8); //2^8
  
  //
  //// DirectX Window stuff
  //

  /*DirectXWindow* p3DWindow;
  Heightmap* p3DHeightmap, *p3DWatermap;
  p3DWindow = new DirectXWindow("OpenCL Virtual Pipes", 0, 800, 640, 0, 0);
  p3DHeightmap = new Heightmap();
  p3DWatermap = new Heightmap();
  p3DWindow->CreateColoredHeightmap(p3DHeightmap, Pipe.getWidth(), Pipe.getWidth(), Pipe.getTerrain(), Pipe.getRGB());
  p3DWindow->CreateWaterHeightmap(p3DWatermap, Pipe.getWidth(), Pipe.getWidth(), Pipe.getWater());
  Pipe.generateV();

  for (int counter = 0; counter < 1000; counter++)
    {
    Pipe.step(0.001);
    p3DHeightmap->ModifyHeightsAndColors(Pipe.getTerrain(), Pipe.getRGB());
    p3DWatermap->ModifyHeights(Pipe.getWater());
    p3DWindow->step(0.001);
    }*/
//  Pipe.testKernel();
  //Pipe.startup();


  
  DisplayEngine.Init(800,640);
	DisplayEngine.Start();
	DisplayEngine.End();
  //return 0;

  cin.ignore(1);
  return 0;
  }