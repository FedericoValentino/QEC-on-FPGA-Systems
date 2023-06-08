#include "Project.h"


void test(int root1, int root2)
{
	//rootManager
	#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.roots.array type=cyclic factor=16
	#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.oddRoots.array type=cyclic factor=16
	#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.sizes.map type=cyclic factor=128
	#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.parity.map type=cyclic factor=128
		//Decoder
	#pragma HLS ARRAY_PARTITION variable=decoderUF.connection_counts type=cyclic factor=16
	#pragma HLS ARRAY_PARTITION variable=decoderUF.support type=cyclic factor=16
	#pragma HLS ARRAY_PARTITION variable=decoderUF.root_of_vertex type=cyclic factor=16
	#pragma HLS ARRAY_PARTITION variable=decoderUF.border_vertices.map->array type=cyclic factor=16
	#pragma HLS STREAM variable=decoderUF.fuseList depth=64
	#pragma HLS STREAM variable=decoderUF.peeling_edges depth=64

	decoderUF.mngr.merge(root1,root2);
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
//#pragma HLS ARRAY_PARTITION variable=decoderLUT.map type=complete
	//rootManager
	#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.roots.array type=cyclic factor=16
	#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.oddRoots.array type=cyclic factor=16
	#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.sizes.map type=cyclic factor=128
	#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.parity.map type=cyclic factor=128
		//Decoder
	#pragma HLS ARRAY_PARTITION variable=decoderUF.connection_counts type=cyclic factor=16
	#pragma HLS ARRAY_PARTITION variable=decoderUF.support type=cyclic factor=16
	#pragma HLS ARRAY_PARTITION variable=decoderUF.root_of_vertex type=cyclic factor=16
	#pragma HLS ARRAY_PARTITION variable=decoderUF.border_vertices.map->array type=cyclic factor=16
	#pragma HLS STREAM variable=decoderUF.fuseList depth=64
	#pragma HLS STREAM variable=decoderUF.peeling_edges depth=64
	ap_uint<CORR_LEN> tmp;
	decoderUF.clear();
	tmp = decoderUF.decode(syndrome);
	*correction = tmp;

	/*if(insert)
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
	}*/


}
