#include "Project.h"


HashMap decoderLUT;
Decoder decoderUF;

void test(int syndrome[SYN_LEN])
{
	decoderUF.decode(syndrome);
}


void decoderTop(int syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction, bool insert)
{
#pragma HLS ARRAY_PARTITION variable=decoderUF.connection_counts.array type=complete
#pragma HLS ARRAY_PARTITION variable=decoderUF.support.array type=complete
#pragma HLS ARRAY_PARTITION variable=decoderUF.root_of_vertex.array type=complete
#pragma HLS ARRAY_PARTITION variable=decoderUF.fuseList.array type=complete
#pragma HLS ARRAY_PARTITION variable=decoderUF.border_vertices.map.array type=complete
#pragma HLS ARRAY_PARTITION variable=decoderUF.peeling_edges.array type=complete

#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.roots.array type=complete
#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.oddRoots.array type=complete
#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.sizes.map.array type=complete
#pragma HLS ARRAY_PARTITION variable=decoderUF.mngr.parity.map.array type=complete
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
