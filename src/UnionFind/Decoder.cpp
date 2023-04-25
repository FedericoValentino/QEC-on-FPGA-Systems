#include "Decoder.h"

ap_uint<CORR_LEN> Decoder::decode(int syndrome[SYN_LEN])
{
	Vector<uint32_t> syndrome_vertices;

	for(uint32_t i = 0; i < SYN_LEN; ++i)
	{
		if(syndrome[i])
		{
			syndrome_vertices.emplace(i);
		}
	}

	init_cluster(syndrome_vertices);



	return 0;
}

void Decoder::init_cluster(Vector<uint32_t> roots)
{
	mngr.initializeRoots(roots);
	for(uint32_t i = 0; i < roots.getSize(); ++i)
	{

	}
}


