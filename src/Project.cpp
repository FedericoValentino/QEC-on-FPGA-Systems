#include "Project.h"


HashMap decoderLUT;
Decoder decoderUF;

void test(int root1, int root2)
{
#pragma HLS ARRAY_PARTITION variable=decoderLUT.map type=complete
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
#pragma HLS ARRAY_PARTITION variable=decoderUF.border_vertices.map.array->v2 type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.peeling_edges.array type=cyclic factor=16


	decoderUF.mngr.merge(root1, root2);
	decoderUF.mergeBoundary(root1, root2);
	decoderUF.border_vertices.erase(root2);
}


void decoderTop(int syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction, bool insert)
{
	//axi
#pragma HLS INTERFACE mode=m_axi port=syndrome depth=64
#pragma HLS INTERFACE mode=s_axilite port=insert
#pragma HLS INTERFACE mode=s_axilite port=correction
	//hashmap
#pragma HLS ARRAY_PARTITION variable=decoderLUT.map type=complete
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
#pragma HLS ARRAY_PARTITION variable=decoderUF.border_vertices.map.array->v2.array type=cyclic factor=16
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
			decoderUF.clear();
		}
	}

}

void testhash(){
	ap_uint<SYN_LEN> test=100;
	HashMap testingMap;
	int x=testingMap.hash(test);
}
