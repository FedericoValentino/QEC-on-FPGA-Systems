#include "Project.h"
#include "hls_print.h"

void decoderTop(int syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction, bool insert)
{
	static HashMap decoderLUT;
	Decoder decoderUF;
	//axi
#pragma HLS INTERFACE m_axi port=syndrome offset=slave bundle=gmem0 depth=64
#pragma HLS INTERFACE m_axi port=correction offset=slave bundle=gmem1 depth=128

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
		hls::print("Inserted in LUT\n");
	}
	else
	{
		hls::print("Trying to retrieve from LUT\n");
		tmp = decoderLUT.retrieve(syndrome);
		if(tmp == 0)
		{
			hls::print("Retrieve was not successful, starting decode\n");
			decoderUF.clear();
			hls::print("Buffers have been cleared\n");
			tmp = decoderUF.decode(syndrome);
			hls::print("Syndrome has been decoded\n");
		}
		hls::print("Correction is being sent out!\n");
		*correction = tmp;
	}


}
