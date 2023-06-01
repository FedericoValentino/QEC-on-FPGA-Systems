#include "Project.h"


HashMap decoderLUT;
Decoder decoderUF;

void test(int root1, int root2)
{
	//rootManager
#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.roots.array type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.oddRoots.array type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.sizes.map.array type=cyclic factor=128
#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.parity.map.array type=cyclic factor=128
	//Decoder
#pragma HLS ARRAY_PARTITION variable=decoderUF.connection_counts.array type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.support.array type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.root_of_vertex.array type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.fuseList.array type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.border_vertices.map.array type=complete
#pragma HLS ARRAY_PARTITION variable=decoderUF.peeling_edges.array type=cyclic factor=16


	Vector<uint32_t> borderR1;
	borderR1 = decoderUF.border_vertices.find(root1);
	Vector<uint32_t> borderR2;
	borderR2 = decoderUF.border_vertices.find(root2);
	uint32_t size2 = borderR2.getSize();

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
