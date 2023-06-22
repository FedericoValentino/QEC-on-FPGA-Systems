#include "host.h"

float startExecution(cl::CommandQueue& q, cl::Kernel& decoderUF, cl::Buffer& syn, cl::Buffer& corrIn, cl::Buffer& corrOut, bool insert)
{
	decoderUF.setArg(0, syn);
	decoderUF.setArg(1, corrIn);
	decoderUF.setArg(2, corrOut);
	decoderUF.setArg(3, insert);

	// Data will be migrated to kernel space
	q.enqueueMigrateMemObjects({syn, corrIn}, 0); /*0 means from host*/
	q.finish();
	
	auto start=std::chrono::high_resolution_clock::now();
	
	
	//Launch the Kernel
	q.enqueueTask(decoderUF);
	q.finish();
	
	auto stop=std::chrono::high_resolution_clock::now();
        auto duration=std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start);
        
        printf("Operation concluded in %f nanoseconds\n", (float)duration.count());
	
	//Data from Kernel to Host
	q.enqueueMigrateMemObjects({corrOut},CL_MIGRATE_MEM_OBJECT_HOST);

	q.finish();
	
	return (float)duration.count();
	
}	

int main(int argc, char* argv[]){

	std::vector<uint8_t, aligned_allocator<uint8_t>> syndrome_in(SYN_LEN);
	std::vector<uint8_t, aligned_allocator<uint8_t>> correction_in(CORR_LEN);
	std::vector<uint8_t, aligned_allocator<uint8_t>> correction_out(CORR_LEN);
	bool insert;

	float insertAVG = 0;
	float retrieveAVG = 0;
	float decodeAVG = 0;

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

    cl::Buffer buffer_syn(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, syndrome_size, syndrome_in.data());
    cl::Buffer correction_in_buf(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, correction_in_size, correction_in.data());
    cl::Buffer correction_out_buf(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, correction_out_size, correction_out.data());
	//cl::Buffer insert_buf(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, insert_size, &insert);

    FILE* f=fopen("/home/users/federico.valentino/git/QEC-on-FPGA-Systems/testBench/LUT.txt","r");

    bool correctionTestLUT[CORR_LEN] = {0};

    int size = 0;
/*
    while(!feof(f) && size < 50)
    {
	fgetc(f); //square bracket
	for(int i=0;i<SYN_LEN;i++)
	{
		syndrome_in[i]=fgetc(f)-48;
		fgetc(f); //space
		}
		fgetc(f); //end of line
		fgetc(f); //square bracket
		for(int i=0;i<CORR_LEN;i++)
		{
			correction_in[i]=fgetc(f)-48;
			fgetc(f);
		}
		fgetc(f);

		insertAVG += startExecution(q, decoderUF, buffer_syn, correction_in_buf, correction_out_buf, 1);
		retrieveAVG += startExecution(q, decoderUF, buffer_syn, correction_in_buf, correction_out_buf, 0);
		
		for(int i = 0; i < CORR_LEN; i++)
		{
			//printf("%d == %d\n", correction_in[i], correction_out[i]);
			assert(correction_in[i] == correction_out[i]);
		}

		size++;
			
	}
	printf("LUT is loaded\n");
*/

	for(int it = 0; it < 100; it++)
	{
		int noiseVec[CORR_LEN] = {0};

		for(int i = 0; i < CORR_LEN; i++)
		{
			noiseVec[i] = distribution(generator);
		}

		//setting the input data
		printf("Measured syndrome:\n");
		noise=distribution(generator);
			for(int i=0;i<SYN_LEN;i++){
				syndrome_in[i]=0;
				for(int j=0;j<CORR_LEN;j++){
					syndrome_in[i]+=stabilizers[i][j]*noiseVec[j];
				}
				syndrome_in[i]=syndrome_in[i]%2;
				printf("%d",syndrome_in[i]);
			}
		printf("\n");

		
		for(int i=0; i < CORR_LEN; i++)
		{
			correction_in[i] = 0;
			correction_out[i] = 0;
		}

		decodeAVG += startExecution(q, decoderUF, buffer_syn, correction_in_buf, correction_out_buf, 1);

		//verify result:
		printf("Correction to apply:\n");
		for(int i=0;i<CORR_LEN;i++){
			printf("%d",correction_out[i]);
		}
		printf("\n");
	}
	
	printf("Insert AVG: %f\nRetrieve AVG: %f\nDecode AVG: %f\n", insertAVG/50.0f, retrieveAVG/50.0f, decodeAVG/100.0f); 
    q.finish();

	return 0;
}

