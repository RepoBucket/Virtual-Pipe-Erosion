#include <utility>
//#define __NO_STD_VECTOR // Use cl::vector instead of STL version
#include "CL/cl.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
 
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

class gpgpu_VirtualPipe
  {
  public:
    void startup();
    int step(const float& timeStep);
    void profile(const cl::Device & queree);
  };

inline void checkErr(cl_int err, const char * name)
  {
  if (err != CL_SUCCESS) 
    {
    std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
    exit(EXIT_FAILURE);
    }
}



#undef MAX_SOURCE_SIZE