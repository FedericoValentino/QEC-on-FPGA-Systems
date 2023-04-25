#ifndef DECODER_H
#define DECODER_H


#include "../Defines.h"
#include "../Utility/Vector.h"
#include "RootManager.h"


struct VertexNeighbors
{
	uint32_t vertex;
	Vector<uint32_t> borders;
};

class Decoder
{
private:
	Vector<uint32_t> connection_counts;
	Vector<uint32_t> support;


	RootManager mngr;

	Vector<VertexNeighbors> border_vertices;


public:
	ap_uint<CORR_LEN> decode(int syndrome[SYN_LEN]);
	void init_cluster(Vector<uint32_t> roots);
};

#endif
