#include "parse.h"
#include <iostream>
#include <fstream>

using namespace std;

namespace IOTools{
  string parse(const string& filename)
    {
    fstream file;
    string source;
    string buffer;
    file.open(filename.c_str());
    if(file.is_open())
      {
      bool continuer = true;
      source.clear();
      while (continuer)
        {
        getline(file, buffer, '\n');
        source.append(buffer);
        source.append("\n");

        if (file.eof())
          continuer = false;
        }
      file.close();
      return source;
      }
    else 
      {
      file.close();
      return source.assign("Failed.");
      }
    }
  }