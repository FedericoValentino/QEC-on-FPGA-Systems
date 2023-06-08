#include "Project.h"
#include "hls_print.h"


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
#pragma HLS INTERFACE m_axi port=syndrome offset=slave bundle=gmem0 depth=128
#pragma HLS INTERFACE m_axi port=correction offset=slave bundle=gmem1 depth=256

#pragma HLS INTERFACE s_axilite port=syndrome bundle=control
#pragma HLS INTERFACE s_axilite port=correction bundle=control
#pragma HLS INTERFACE s_axilite port=insert bundle=control
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
	ap_uint<CORR_LEN> tmp;
	if(insert)
	{
		decoderLUT.insert(*correction, syndrome);
		hls::print("Inserted in LUT");
	}
	else
	{
		tmp = decoderLUT.retrieve(syndrome);
		if(tmp == 0)
		{
			hls::print("Syndrome not found, starting decode");
			decoderUF.clear();
			hls::print("Cleared all buffers");
			tmp = decoderUF.decode(syndrome);
			hls::print("Decoded!");
		}
		*correction = tmp;
	}


}

void testhash(){
	ap_uint<SYN_LEN> test=100;
	HashMap testingMap;
	int x=testingMap.hash(test);
}
