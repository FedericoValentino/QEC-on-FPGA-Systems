# QEC on FPGA systems
Quantum computing is a promising paradigm where the computation adopts quantum mechanics instead of binary algebra. However, quantum computers suffer a significant drawback from a reliability perspective. Thus, quantum error correction (QEC) is the primary way to mitigate such effects, correct that error, and offer improved reliability. Especially, QEC demands high-speed and low-latency computations close to the quantum systems. Therefore, accelerating QEC on FPGAs represents one of the most exciting ways of the research panorama. This project aims at implementing QEC on high-performance FPGAs.

# How to use this
The code works on the latest Vitis_HLS version. If you are not using this then you need to get yourself a copy of the ap_uint.h library. To run the testbenches you need the absolute path of your file containing syndromes and decodes.
When importing the project into vitis you need to import the following:

 - Under Source:
	 - Decoder.cpp/.h
	 - Defines.h
	 - Hashmap.cpp/.h
	 - Map.h
	 - Project.cpp/.h
	 - RootManager.cpp/.h
	 - SurfaceCode.h
	 - Vector.h
 - Under TestBench:
	 - TestBench.cpp 

The decoder has been tested to work for Toric codes up to d=13. In case you want to use a different d the following parameters inside of Defines.h need to be changed: 

 - D = d;
 - N = 2*d^2
 - CORR_LEN = 2*d^2
 - SYN_LEN = d^2
