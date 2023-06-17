#include "host.h"

int main(int argc, char* argv[]){

	std::vector<uint8_t, aligned_allocator<uint8_t>> syndrome(SYN_LEN);
	std::vector<uint8_t, aligned_allocator<uint8_t>> correction_in(CORR_LEN);
	std::vector<uint8_t, aligned_allocator<uint8_t>> correction_out(CORR_LEN);
	bool insert;

	size_t syndrome_size = sizeof(bool) * SYN_LEN;
	size_t correction_in_size = sizeof(bool) * CORR_LEN;
	size_t correction_out_size = sizeof(bool) * CORR_LEN;
	//TARGET_DEVICE macro needs to be passed from gcc command line
    if(argc != 2) {
		std::cout << "Usage: " << argv[0] <<" <xclbin>" << std::endl;
		return EXIT_FAILURE;
	}

   	std::string binaryFile = argv[1];

    
    cl_int err;
    cl::Context context;
    cl::Kernel decoderUF;
    cl::CommandQueue q;

    auto devices = xcl::get_xil_devices();

    auto fileBuf = xcl::read_binary_file(binaryFile);

    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};

    bool valid_device = false;

    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
        OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            OCL_CHECK(err, decoderUF= cl::Kernel(program, "decoderTop", &err));
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    cl::Buffer buffer_syn(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, syndrome_size, syndrome.data());
    cl::Buffer correction_in_buf(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, correction_in_size, correction_in.data());
    cl::Buffer correction_out_buf(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, correction_out_size, correction_out.data());
	//cl::Buffer insert_buf(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, insert_size, &insert);

	
	

	//setting the input data
	printf("Measured syndrome:\n");
	noise=distribution(generator);
		for(int i=0;i<SYN_LEN;i++){
			syndrome[i]=0;
			for(int j=0;j<CORR_LEN;j++){
				noise=distribution(generator);
				syndrome[i]+=stabilizers[i][j]*noise;
			}
			syndrome[i]=syndrome[i]%2;
			printf("%d",syndrome[i]);
		}
	printf("\n");

	
	for(int i=0; i < CORR_LEN; i++)
	{
		correction_in[i] = 0;
		correction_out[i] = 0;
	}

	insert = 0;

	decoderUF.setArg(0, buffer_syn);
	decoderUF.setArg(1, correction_in_buf);
	decoderUF.setArg(2, correction_out_buf);
	decoderUF.setArg(3, insert);
	// Data will be migrated to kernel space
	q.enqueueMigrateMemObjects({buffer_syn, correction_in_buf, correction_out_buf}, 0); /*0 means from host*/

	//Launch the Kernel
	q.enqueueTask(decoderUF);
	
	// The result of the previous kernel execution will need to be retrieved in
	// order to view the results. This call will transfer the data from FPGA to
	// source_results vector
	q.enqueueMigrateMemObjects({correction_out_buf},CL_MIGRATE_MEM_OBJECT_HOST);

	q.finish();

	//verify result:
	printf("Correction to apply:\n");
	for(int i=0;i<CORR_LEN;i++){
		printf("%d",correction_out[i]);
	}
	printf("\n");

    q.finish();

	return 0;
}

