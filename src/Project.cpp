#include "Project.h"


HashMap decoderLUT;
Decoder decoderUF;

void test(int syndrome[SYN_LEN])
{
	Vector<uint32_t> test;
#pragma HLS ARRAY_PARTITION variable=test.array type=cyclic factor=16

	test.elementEmplace(1);
	test.elementEmplace(2);
	test.elementEmplace(3);

	test.elementErase(2);
}


void decoderTop(int syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction, bool insert)
{
#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.roots.array type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.oddRoots.array type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.peeling_edges.array type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.border_vertices.map type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderLUT.map type=cyclic factor = 16
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
