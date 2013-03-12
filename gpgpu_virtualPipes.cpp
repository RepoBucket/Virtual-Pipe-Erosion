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

  std::vector<cl_int> out_intArray(1024, 5);
  std::vector<cl_int> in_intArray(1024, 2);

  cl::Context* context = new cl::Context(devices->at(0), NULL, NULL);
  cl::Buffer* outbuffer = new cl::Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_int) * 1024, &out_intArray[0], &whatHappened);
  std::cout << "What happened with creating outbuffer: " << oclErrorString(whatHappened) << std::endl;
  cl::Buffer* inbuffer = new cl::Buffer(*context, CL_MEM_WRITE_ONLY, sizeof(cl_int) * 1024, NULL, &whatHappened);
  std::cout << "What happened with creating inbuffer: " << oclErrorString(whatHappened) << std::endl;

  cl_mem outbuffer = 

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
  std::cout << "What happened with creating a kernel: " << oclErrorString(whatHappened) << std::endl;

  (*kernels)[0].setArg(0, sizeof(cl_int) * 1024, outbuffer);
  (*kernels)[0].setArg(1, sizeof(cl_int) * 1024, inbuffer);
  
  const size_t global_item_size = 1024;
  const size_t local_item_size = 64;
  
  cl::NDRange globalRange(global_item_size);
  cl::NDRange localRange(local_item_size);
  cl::NDRange offset(0);

  cl::CommandQueue commandQueue((*context), (*devices)[0]);
  whatHappened = commandQueue.enqueueWriteBuffer(*outbuffer, true, 0, sizeof(cl_int) * 1024, &out_intArray[0]);
  std::cout << "What happened with writing buffer to gpu: " << oclErrorString(whatHappened) << std::endl;
  commandQueue.enqueueNDRangeKernel(kernels->at(0), offset, globalRange, localRange);
  std::cout << "What happened with excecuting kernel: " << oclErrorString(whatHappened) << std::endl;
  commandQueue.enqueueReadBuffer(*inbuffer, true, 0, sizeof(cl_int) * 1024, &(in_intArray)[0]);
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