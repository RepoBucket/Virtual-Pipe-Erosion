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

#include "noise.h"
#include <boost/random/mersenne_twister.hpp>

//#define MAX_SOURCE_SIZE (0x100000)

class gpgpu_VirtualPipe
  {
  public:
    gpgpu_VirtualPipe(const int& powersOfTwo);
    ~gpgpu_VirtualPipe();
    void startup();
    int step(const float& timeStep);
    void profile(const cl::Device & queree);
    
    //
    // Getters
    //

    cl_float2& readHeights(const int& x, const int& y);
    
    
  private:
    //
    // Functions
    //

    void generate();
    void swap(); //swaps the read and write maps.
    //
    // Variables
    //

    const float gravityConstant, pipeCrossSectionalArea, sizeOfCell;
    long long unsigned int actualWidth;


    // (read/write points)
    std::vector<cl_float4>* fluxArrayRead; //0: fluxLeft, 1: fluxRight, 2: fluxDown, 3: fluxUp
    std::vector<cl_float2>* heightmapArrayRead; //0: terrain 1: water
    std::vector<cl_float2>* sedimentArrayRead; // 0: sediment, 1: capacity
    std::vector<cl_float4>* constantsArrayRead; // 0: sedimentcapacityconstant, 1: dissolving constant, 2: depositing constant, 3: soilslippagetheshold
    std::vector<cl_float4>* fluxArrayWrite; //0: fluxLeft, 1: fluxRight, 2: fluxDown, 3: fluxUp
    std::vector<cl_float2>* heightmapArrayWrite; //0: terrain 1: water
    std::vector<cl_float2>* sedimentArrayWrite; // 0: sediment, 1: capacity
    
    

    // (data arrays)
    std::vector<cl_float4> fluxArray1; //0: fluxLeft, 1: fluxRight, 2: fluxDown, 3: fluxUp
    std::vector<cl_float2> heightmapArray1; //0: terrain 1: water
    std::vector<cl_float2> sedimentArray1; // 0: sediment, 1: capacity
    std::vector<cl_float4> constantsArray; // 0: sedimentcapacityconstant, 1: dissolving constant, 2: depositing constant, 3: soilslippagetheshold
    std::vector<cl_float4> fluxArray2; //0: fluxLeft, 1: fluxRight, 2: fluxDown, 3: fluxUp
    std::vector<cl_float2> heightmapArray2; //0: terrain 1: water
    std::vector<cl_float2> sedimentArray2; // 0: sediment, 1: capacity
    
    
    
    std::vector<cl::Platform> platformArray;
    std::vector<cl::Device> deviceArray;
    



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