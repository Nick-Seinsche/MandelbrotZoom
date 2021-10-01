// //read source code
// size_t size_source = -1;
// char *source_str = readFile("kernels/kernel.txt", &size_source);
//
// if (size_source == -1) printf("[ERROR] Could not read file\n");
//
// // create variables
// int* x = (int*) malloc(sizeof(int));
// int* y = (int*) malloc(sizeof(int));
// int* z = (int*) malloc(sizeof(int));
//
// // get platform and device information
// cl_platform_id platform_id = NULL;
// cl_device_id device_id = NULL;
// cl_uint ret_num_devices;
// cl_uint ret_num_platforms;
// cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
// ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1,
//             &device_id, &ret_num_devices);
// // create opecl context
// cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
//
// // create command queue
// cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
//
// // create memory buffers for variables
// cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int), NULL, &ret);
// cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int), NULL, &ret);
// cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int), NULL, &ret);
//
// // copy the variables to the memory spaces
// ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0,
//                             sizeof(int), x, 0, NULL, NULL);
// ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0,
//                             sizeof(int), y, 0, NULL, NULL);
// ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0,
//                             sizeof(int), z, 0, NULL, NULL);
//
// // Create a program form the kernel source
// cl_program program = clCreateProgramWithSource(context, 1,
//                         (const char**) &source_str, &size_source, &ret);
//
// // Build the program
// ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
//
// // Create the OpenCL kernel
// cl_kernel kernel = cleCreateKernel(program, "addition", &ret);
//
// //set the arguments of the kernel
// ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &a_mem_obj)
// ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &b_mem_obj)
// ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &c_mem_obj)
//
// // Execute the OpenGL kernel
// size_t global_item_size = sizeof(int);
// size_t local_item_size = sizeof(int);
// ret = clEnqueueNDRangeKernel(command_queue, kernel, 1 NULL,
//                     &global_item_size, &local_item_size, 0, NULL, NULL);
//
