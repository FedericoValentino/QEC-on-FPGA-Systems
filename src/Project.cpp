#include "Project.h"

void decoderTop(bool syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction, bool insert)
{
	static HashMap decoderLUT;
	static Decoder decoderUF;
	//axi
#pragma HLS INTERFACE m_axi port=syndrome offset=slave bundle=gmem0 depth=9
#pragma HLS INTERFACE m_axi port=correction offset=slave bundle=gmem1 depth=18

#pragma HLS INTERFACE s_axilite port=syndrome bundle=control
#pragma HLS INTERFACE s_axilite port=correction bundle=control
#pragma HLS INTERFACE s_axilite port=insert bundle=control
	//hashmap
#pragma HLS ARRAY_PARTITION variable=decoderLUT.map type=complete
	//rootManager
#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.roots.array type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.oddRoots.array type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.sizes type=cyclic factor=64
#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.parity type=cyclic factor=64
		//Decoder
#pragma HLS ARRAY_PARTITION variable=decoderUF.syndrome type=complete
#pragma HLS ARRAY_PARTITION variable=decoderUF.support type=cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=decoderUF.root_of_vertex type=complete
#pragma HLS ARRAY_PARTITION variable=decoderUF.border_vertices->array type=cyclic factor=16
	bool tmp = false;

	//hls::print("insert: %d\n", insert);
	switch(insert)
	{
		case 1:
			decoderLUT.insert(*correction, syndrome);
			tmp = true;
			//hls::print("Inserted in LUT\n");
			break;
		case 0:
			//hls::print("Retrieving from LUT\n");
			tmp = decoderLUT.retrieve(syndrome, correction);
			break;
	}

	switch(tmp)
	{
		case 0:
			decoderUF.decode(syndrome, correction);
			//hls::print("Syndrome has been decoded\n");
			break;
		case 1:
			//hls::print("Nothing else to do\n");
			break;
	}


}
