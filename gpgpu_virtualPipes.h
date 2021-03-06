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
    void step(const float& timeStep);
    void profile(const cl::Device & queree);
    void randomRain(const int& howMany, const double& howMuch);
    //void testKernel();
    
    void generateV();

    //
    // Getters
    //

    cl_float2& readHeights(const int& x, const int& y);
    float* getWater();
    float* getTerrain();
    float* getRGB();
    long long unsigned int getWidth();
    
    
  private:
    //
    // Functions
    //

    void swap(); //swaps the read and write maps.
    void package(); // Package the data arrays into 1D ones for rendering.
    int random(const int& min, const int& max);
    float random(const float& min, const float& max);

    //
    // Variables
    //

    const float gravityConstant, pipeCrossSectionalArea, sizeOfCell;
    long long unsigned int actualWidth;
    cl::Context* context;
    cl::CommandQueue* CommandQueue;
    

    // (buffers)
    cl::Buffer* buffer_flux;
    cl::Buffer* buffer_heightmap;
    cl::Buffer* buffer_sediment;
    cl::Buffer* buffer_constants;
    cl::Buffer* buffer_flowVector;
    cl::Buffer* dimensions;
    cl::Buffer* timeStep;

    // (read/write points)
    std::vector<cl_float4>* fluxArrayRead; //0: fluxTop, 1: fluxBottom, 2: fluxLeft, 3: fluxRight
    std::vector<cl_float2>* heightmapArrayRead; //0: terrain 1: water
    std::vector<cl_float2>* sedimentArrayRead; // 0: sediment, 1: capacity
    std::vector<cl_float4>* constantsArrayRead; // 0: sedimentcapacityconstant, 1: dissolving constant, 2: depositing constant, 3: soilslippagetheshold
    std::vector<cl_float4>* fluxArrayWrite; //0: fluxTop, 1: fluxBottom, 2: fluxLeft, 3: fluxRight
    std::vector<cl_float2>* heightmapArrayWrite; //0: terrain 1: water
    std::vector<cl_float2>* sedimentArrayWrite; // 0: sediment, 1: capacity
    

    // (data arrays)
    std::vector<cl_float4> fluxArray1; // 0: fluxTop, 1: fluxBottom, 2: fluxLeft, 3: fluxRight
    std::vector<cl_float2> heightmapArray1; //0: terrain 1: water
    std::vector<cl_float2> sedimentArray1; // 0: sediment, 1: capacity
    std::vector<cl_float4> constantsArray; // 0: sedimentcapacityconstant, 1: dissolving constant, 2: depositing constant, 3: soilslippagetheshold
    std::vector<cl_float2> flowVectorArray; 
    std::vector<cl_float4> fluxArray2; // 0: fluxTop, 1: fluxBottom, 2: fluxLeft, 3: fluxRight
    std::vector<cl_float2> heightmapArray2; //0: terrain 1: water
    std::vector<cl_float2> sedimentArray2; // 0: sediment, 1: capacity'

    std::vector<float> terrain;
    std::vector<float> water;
    std::vector<float> rgb;
    
    // (OpenCL arrays)
    std::vector<cl::Platform> platformArray;
    std::vector<cl::Device> deviceArray;
    std::vector<cl::Program> programsArray;

    bool buildReport(const int& index);

    // (RNG)
    //noise::module::Perlin Perlingen;
    boost::random::mt19937 gen;
  };






#undef MAX_SOURCE_SIZE