#include "Engine.h"
#include <iostream>
#include <string>
//#include "gpgpu_virtualPipes.h"

using namespace std;

int main(int argc, char **argv)
  {
  /*gpgpu_VirtualPipe Pipe;
  Pipe.startup();*/

  
  DisplayEngine.Init(640,640);
	DisplayEngine.Start();
	DisplayEngine.End();
  return 0;

  cin.ignore(1);
  return 0;
  }