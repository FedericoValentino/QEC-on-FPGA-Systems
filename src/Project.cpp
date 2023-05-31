#include "Project.h"


HashMap decoderLUT;
Decoder decoderUF;

void test(int syndrome[SYN_LEN])
{
	Map<uint32_t, uint32_t> TestMap;

	TestMap.add(0, 1);
	TestMap.add(1, 2);
	TestMap.add(2, 3);

	uint32_t v1 = TestMap.find(0);

	TestMap.erase(v1);

	TestMap.reset();

}


void decoderTop(int syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction, bool insert)
{
#pragma HLS ARRAY_PARTITION variable=decoderLUT.map type=complete
	//rootManager
#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.roots.array type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.oddRoots.array type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.sizes.map.array type=cyclic factor=256
#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.parity.map.array type=cyclic factor=256
	//Decoder
#pragma HLS ARRAY_PARTITION variable=decoderUF.connection_counts.array type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.support.array type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.root_of_vertex.array type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.fuseList.array type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.border_vertices.map.array type=cyclic factor=256
#pragma HLS ARRAY_PARTITION variable=decoderUF.peeling_edges.array type=cyclic factor=16
	if(insert)
	{
		decoderLUT.insert(*correction, syndrome);
	}
	else
	{
		*correction = decoderLUT.retrieve(syndrome);
		if(*correction == 0)
		{
			*correction = decoderUF.decode(syndrome);
		}
	}

}

void testhash(){
	ap_uint<SYN_LEN> test=100;
	HashMap testingMap;
	int x=testingMap.hash(test);
}
