#pragma warning(disable: 4244)

#include "gpgpu_virtualPipes.h"
#include "parse.h"
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include "noise.h"
#include <cmath>

const char* oclErrorString(cl_int error)
{
    static const char* errorString[] = {
        "SUCCESS",
        "CL_DEVICE_NOT_FOUND",
        "CL_DEVICE_NOT_AVAILABLE",
        "CL_COMPILER_NOT_AVAILABLE",
        "CL_MEM_OBJECT_ALLOCATION_FAILURE",
        "CL_OUT_OF_RESOURCES",
        "CL_OUT_OF_HOST_MEMORY",
        "CL_PROFILING_INFO_NOT_AVAILABLE",
        "CL_MEM_COPY_OVERLAP",
        "CL_IMAGE_FORMAT_MISMATCH",
        "CL_IMAGE_FORMAT_NOT_SUPPORTED",
        "CL_BUILD_PROGRAM_FAILURE",
        "CL_MAP_FAILURE",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "CL_INVALID_VALUE",
        "CL_INVALID_DEVICE_TYPE",
        "CL_INVALID_PLATFORM",
        "CL_INVALID_DEVICE",
        "CL_INVALID_CONTEXT",
        "CL_INVALID_QUEUE_PROPERTIES",
        "CL_INVALID_COMMAND_QUEUE",
        "CL_INVALID_HOST_PTR",
        "CL_INVALID_MEM_OBJECT",
        "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
        "CL_INVALID_IMAGE_SIZE",
        "CL_INVALID_SAMPLER",
        "CL_INVALID_BINARY",
        "CL_INVALID_BUILD_OPTIONS",
        "CL_INVALID_PROGRAM",
        "CL_INVALID_PROGRAM_EXECUTABLE",
        "CL_INVALID_KERNEL_NAME",
        "CL_INVALID_KERNEL_DEFINITION",
        "CL_INVALID_KERNEL",
        "CL_INVALID_ARG_INDEX",
        "CL_INVALID_ARG_VALUE",
        "CL_INVALID_ARG_SIZE",
        "CL_INVALID_KERNEL_ARGS",
        "CL_INVALID_WORK_DIMENSION",
        "CL_INVALID_WORK_GROUP_SIZE",
        "CL_INVALID_WORK_ITEM_SIZE",
        "CL_INVALID_GLOBAL_OFFSET",
        "CL_INVALID_EVENT_WAIT_LIST",
        "CL_INVALID_EVENT",
        "CL_INVALID_OPERATION",
        "CL_INVALID_GL_OBJECT",
        "CL_INVALID_BUFFER_SIZE",
        "CL_INVALID_MIP_LEVEL",
        "CL_INVALID_GLOBAL_WORK_SIZE",
    };

    const int errorCount = sizeof(errorString) / sizeof(errorString[0]);

    const int index = -error;

    return (index >= 0 && index < errorCount) ? errorString[index] : "";

}

void gpgpu_VirtualPipe::buildReport(const int& index)
  {
  std::cout << "Building " << index << " ...";
  cl_int whatHappened = programsArray[index].build(deviceArray);
  std::cout << " " << oclErrorString(whatHappened) << std::endl;
  if(whatHappened != CL_SUCCESS){
    std::cout << "Build Status: " << programsArray[index].getBuildInfo<CL_PROGRAM_BUILD_STATUS>(deviceArray[0]) << std::endl;
    std::cout << "Build Options:\t" << programsArray[index].getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(deviceArray[0]) << std::endl;
    std::cout << "Build Log:\t " << programsArray[index].getBuildInfo<CL_PROGRAM_BUILD_LOG>(deviceArray[0]) << std::endl;
    }
  }

gpgpu_VirtualPipe::gpgpu_VirtualPipe(const int& powersOfTwo)
  : gravityConstant(9.8f), pipeCrossSectionalArea(1.0f), sizeOfCell(1), actualWidth(pow((float)2.0f, powersOfTwo)),
  fluxArray1(actualWidth, cl_float4()), fluxArray2(actualWidth, cl_float4()), heightmapArray1(actualWidth, cl_float2()), heightmapArray2(actualWidth, cl_float2()), 
  sedimentArray1(actualWidth, cl_float2()), sedimentArray2(actualWidth, cl_float2()), constantsArray(actualWidth, cl_float4())
  {

  std::string source;
  cl::Platform::get(&platformArray);
  std::cout << "Looking for a GPU... ";
  platformArray[1].getDevices(CL_DEVICE_TYPE_GPU, &deviceArray);
  // If there aren't any GPUs, gotta fallback on CPUs
  if (deviceArray.size() == 0)
    {
    std::cout << "Failed, resorting to CPU.\n";
    platformArray[1].getDevices(CL_DEVICE_TYPE_CPU, &deviceArray);
    }
  else
    std::cout << "Done.\n";

  context = new cl::Context(deviceArray[0], NULL, NULL);

  // Build programs!
  source.assign(IOTools::parse("stepFlux_kernel.cl"));
  programsArray.push_back(cl::Program(*context, source, true));
  buildReport(0);

  source.assign(IOTools::parse("updateVolume_kernel.cl"));
  programsArray.push_back(cl::Program(*context, source, true));
  buildReport(1);

  /*
  cl_int whatHappened = programsArray[0].build(deviceArray);
  std::cout << "What happened with build: " << oclErrorString(whatHappened) << std::endl;
  if(whatHappened != CL_SUCCESS){
    std::cout << "Build Status: " << programsArray[0].getBuildInfo<CL_PROGRAM_BUILD_STATUS>(deviceArray[0]) << std::endl;
    std::cout << "Build Options:\t" << programsArray[0].getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(deviceArray[0]) << std::endl;
    std::cout << "Build Log:\t " << programsArray[0].getBuildInfo<CL_PROGRAM_BUILD_LOG>(deviceArray[0]) << std::endl;
    }*/

  


  fluxArrayRead = &fluxArray1;
  fluxArrayWrite = &fluxArray2;
  heightmapArrayRead = &heightmapArray1;
  heightmapArrayWrite = &heightmapArray2;
  sedimentArrayRead = &sedimentArray1;
  sedimentArrayWrite = &sedimentArray2;


  }

gpgpu_VirtualPipe::~gpgpu_VirtualPipe()
  {
  }

void gpgpu_VirtualPipe::testKernel()
  {
  std::string source;
  }

void gpgpu_VirtualPipe::startup()
  {
  cl_int whatHappened;
  std::vector<cl::Platform>* platforms = new std::vector<cl::Platform>;
  std::string* param = new std::string;
  std::vector<cl::Device>* devices = new std::vector<cl::Device>;
  std::string source;
  source.assign(IOTools::parse("index_add_kernel.cl"));

  cl::Platform::get(platforms);
  (*platforms)[1].getDevices(CL_DEVICE_TYPE_GPU, devices);
  for (int counter = 0; counter < devices->size(); counter++)
    profile((*devices)[counter]);
  //profile((*devices)[1]);

  cl_int* out_intArrayA = new cl_int[1024];
  cl_int* out_intArrayB = new cl_int[1024];
  cl_int* in_intArray = new cl_int[1024];

  for (int counter = 0; counter < 1024; counter++)
    {
    out_intArrayA[counter] = counter;
    out_intArrayB[counter] = 1024 - counter;
    in_intArray[counter] = 32;
    }

  cl::Context* context = new cl::Context(devices->at(0), NULL, NULL);
  cl::Buffer outbufferA = cl::Buffer(*context, CL_MEM_READ_ONLY, sizeof(cl_int) * 1024, NULL, &whatHappened);
 // std::cout << "What happened with creating outbuffer: " << oclErrorString(whatHappened) << std::endl;
  cl::Buffer inbuffer = cl::Buffer(*context, CL_MEM_WRITE_ONLY, sizeof(cl_int) * 1024, NULL, &whatHappened);
  cl::Buffer outbufferB = cl::Buffer(*context, CL_MEM_READ_ONLY, sizeof(cl_int) * 1024, NULL, &whatHappened);
 // std::cout << "What happened with creating inbuffer: " << oclErrorString(whatHappened) << std::endl;

  std::vector<cl::Program>* programs = new std::vector<cl::Program>();
  std::vector<cl::Kernel>* kernels = new std::vector<cl::Kernel>();

  programs->push_back(cl::Program(*context, source, true));
  whatHappened = (*programs)[0].build(*devices);
  std::cout << "What happened with build: " << oclErrorString(whatHappened) << std::endl;
  if(whatHappened != CL_SUCCESS){
    std::cout << "Build Status: " << programs->at(0).getBuildInfo<CL_PROGRAM_BUILD_STATUS>((*devices)[0]) << std::endl;
    std::cout << "Build Options:\t" << programs->at(0).getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>((*devices)[0]) << std::endl;
    std::cout << "Build Log:\t " << programs->at(0).getBuildInfo<CL_PROGRAM_BUILD_LOG>((*devices)[0]) << std::endl;
}


  whatHappened = (*programs)[0].createKernels(kernels);
  std::cout << "What happened with creating a kernel: "<< oclErrorString(whatHappened) << std::endl;

  

  (*kernels)[0].setArg(0, outbufferA);
  (*kernels)[0].setArg(1, outbufferB);
  (*kernels)[0].setArg(2, inbuffer);
  
  const size_t global_item_size = 1024;
  const size_t local_item_size = 1;
  
  cl::NDRange globalRange(global_item_size);
  cl::NDRange localRange(local_item_size);
  cl::NDRange offset(0);

  cl::CommandQueue commandQueue((*context), (*devices)[0]);
  // write list out_intArray into buffer.
  whatHappened = commandQueue.enqueueWriteBuffer(outbufferA, false, 0, sizeof(cl_int) * 1024, out_intArrayA);
  whatHappened = commandQueue.enqueueWriteBuffer(outbufferB, false, 0, sizeof(cl_int) * 1024, out_intArrayB);
  commandQueue.finish();
  std::cout << "What happened with writing buffer to gpu: " << oclErrorString(whatHappened) << std::endl;
  commandQueue.enqueueNDRangeKernel((*kernels)[0], offset, globalRange, localRange);
  commandQueue.finish();
  std::cout << "What happened with excecuting kernel: " << oclErrorString(whatHappened) << std::endl;
  // write stuff from read buffer to in_intArray
  commandQueue.enqueueReadBuffer(inbuffer, false, 0, sizeof(cl_int) * 1024, in_intArray);
  commandQueue.finish();
  std::cout << "What happened with reading buffer from gpu: " << oclErrorString(whatHappened) << std::endl;

  for (int counter = 0; counter < 1024; counter++)
    std::cout << "Position " << counter << " is " << in_intArray[counter] << std::endl;
  /*
  size_t global_item_size = LIST_SIZE; // Process the entire lists
  size_t local_item_size = 64; // Divide work items into groups of 64
  ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, 
    &global_item_size, &local_item_size, 0, NULL, NULL);

  // Read the memory buffer C on the device to the local variable C
  int *C = (int*)malloc(sizeof(int)*LIST_SIZE);
  ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0, 
    LIST_SIZE * sizeof(int), C, 0, NULL, NULL);*/



  std::cin.ignore(0);
  }

void gpgpu_VirtualPipe::profile(const cl::Device & queree)
  {
  }

void gpgpu_VirtualPipe::generate()
  {

  }