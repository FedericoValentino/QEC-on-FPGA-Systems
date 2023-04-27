#ifndef DECODER_H
#define DECODER_H


#include "../Defines.h"
#include "../Utility/Vector.h"
#include "../Utility/Map.h"
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
	Vector<uint32_t> root_of_vertex;


	RootManager mngr;

	Map<uint32_t, Vector<uint32_t>> border_vertices;

	void init_cluster(Vector<uint32_t> roots);

	void grow(uint32_t root);
public:
	ap_uint<CORR_LEN> decode(int syndrome[SYN_LEN]);

};

#endif
