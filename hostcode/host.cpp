#include "host.h"

int main(int argc, char* argv[]){

	//TARGET_DEVICE macro needs to be passed from gcc command line
    if(argc != 2) {
		std::cout << "Usage: " << argv[0] <<" <xclbin>" << std::endl;
		return EXIT_FAILURE;
	}

    char* xclbinFilename = argv[1];

    std::vector<cl::Device> devices;
    cl::Device device;
    std::vector<cl::Platform> platforms;
    bool found_device = false;

    //traversing all Platforms To find Xilinx Platform and device
    cl::Platform::get(&platforms);
    for(size_t i = 0; (i < platforms.size() ) & (found_device == false) ;i++){
        cl::Platform platform = platforms[i];
        std::string platformName = platform.getInfo<CL_PLATFORM_NAME>();
        if ( platformName == "Xilinx"){
            devices.clear();
            platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);
			if (devices.size()){
				device = devices[0];
				found_device = true;
				break;
			}
        }
    }

    if (found_device == false){
       std::cout << "Error: Unable to find Target Device " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
       return EXIT_FAILURE;
    }

    //DEVICE FOUND, START WORKING

    // Creating Context and Command Queue for selected device
	cl::Context context(device);
	cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE);

	// Load xclbin
	std::cout << "Loading: '" << xclbinFilename << "'\n";
	std::ifstream bin_file(xclbinFilename, std::ifstream::binary);
	bin_file.seekg (0, bin_file.end);
	unsigned nb = bin_file.tellg();
	bin_file.seekg (0, bin_file.beg);
	char *buf=new char[nb];
	bin_file.read(buf, nb);

	// Creating Program from Binary File
	cl::Program::Binaries bins;

	bins.push_back({buf,nb});
	devices.resize(1);
	cl::Program program(context, devices, bins);

    // This call will get the kernel object from program
    cl::Kernel krnl_decoder(program,"project");



    // Compute the size of array in bytes
    size_t size_in_bytes = DATA_SIZE * sizeof(int);

    // These commands will allocate memory on the Device. The cl::Buffer objects can
    // be used to reference the memory locations on the device.
    cl::Buffer buffer_syn(context, CL_MEM_READ_ONLY, size_in_bytes);
    cl::Buffer buffer_corr(context, CL_MEM_READ_WRITE, size_in_bytes);
    cl::Buffer insert(context, CL_MEM_READ_ONLY, size_in_bytes);

    // Creates a vector of DATA_SIZE elements with an initial value of 10 and 32
    // using customized allocator for getting buffer alignment to 4k boundary

    //set the kernel Arguments
	int narg=0;
	krnl_decoder.setArg(narg++,buffer_syn);
	krnl_decoder.setArg(narg++,buffer_corr);
	krnl_decoder.setArg(narg++,insert);
	krnl_decoder.setArg(narg++,DATA_SIZE); //what is this needed for?

	//We then need to map our OpenCL buffers to get the pointers
	bool *ptr_input = (bool *) q.enqueueMapBuffer (buffer_syn , CL_TRUE , CL_MAP_WRITE , 0, size_in_bytes);
	int *ptr_retrieve = (int *) q.enqueueMapBuffer (buffer_corr , CL_TRUE , CL_MAP_READ , 0, size_in_bytes);
	int *ptr_insertion = (int *) q.enqueueMapBuffer (buffer_corr, CL_TRUE, CL_MAP_WRITE, 0, size_in_bytes);
	bool *ptr_insert = (bool *) q.enqueueMapBuffer(insert, CL_TRUE, CL_MAP_READ, 0, size_in_bytes);

	//setting the input data
	printf("Measured syndrome:\n");

		for(int i=0;i<SYN_LEN;i++){
			syndrome[i]=0;
			for(int j=0;j<CORR_LEN;j++){
				noise=distribution(generator);
				syndrome[i]+=stabilizers[i][j]*noise;
			}
			syndrome[i]=syndrome[i]%2;
			ptr_input[i]=syndrome[i];
			printf("%d",syndrome[i]);
		}
	printf("\n");

	// Data will be migrated to kernel space
	q.enqueueMigrateMemObjects({buffer_syn},0); /*0 means from host*/

	//Launch the Kernel
	q.enqueueTask(krnl_decoder);

	// The result of the previous kernel execution will need to be retrieved in
	// order to view the results. This call will transfer the data from FPGA to
	// source_results vector
	q.enqueueMigrateMemObjects({buffer_corr},CL_MIGRATE_MEM_OBJECT_HOST);

	q.finish();

	//verify result:
	printf("Correction to apply:\n");
	for(int i=0;i<CORR_LEN;i++){
		correction[i]=ptr_retrieve[i];
		printf("%d,",correction[i]);
	}
	printf("\n");

	//close the connection
    q.enqueueUnmapMemObject(buffer_syn , ptr_input);
    q.enqueueUnmapMemObject(buffer_corr , ptr_retrieve);
    q.finish();


	return 0;
}

