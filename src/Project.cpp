#include "Project.h"

void decoderTop(int syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction, bool insert)
{
	static HashMap decoderLUT;
	Decoder decoderUF;
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
#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.sizes.map type=cyclic factor=64
#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.parity.map type=cyclic factor=64
		//Decoder
#pragma HLS ARRAY_PARTITION variable=decoderUF.syndrome type=complete
#pragma HLS ARRAY_PARTITION variable=decoderUF.connection_counts type=complete
#pragma HLS ARRAY_PARTITION variable=decoderUF.support type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.root_of_vertex type=complete
#pragma HLS ARRAY_PARTITION variable=decoderUF.border_vertices.map->array type=cyclic factor=16
#pragma HLS STREAM variable=decoderUF.fuseList depth=64
#pragma HLS STREAM variable=decoderUF.peeling_edges depth=64
	ap_uint<CORR_LEN> tmp;
	if(insert)
	{
		decoderLUT.insert(*correction, syndrome);
	}
	else
	{
		tmp = decoderLUT.retrieve(syndrome);
		if(tmp == 0)
		{
			decoderUF.clear();
			tmp = decoderUF.decode(syndrome);
		}
		*correction = tmp;
	}


}
