#include "Engine.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv)
  {

  DisplayEngine.Init(640,640);
	DisplayEngine.Start();
	DisplayEngine.End();
  return 0;
  }