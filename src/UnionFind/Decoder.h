#ifndef DECODER_H
#define DECODER_H


#include "../Defines.h"
#include "../Utility/Vector.h"
#include "../Utility/Map.h"
#include "RootManager.h"
#include "SurfaceCode.h"

class Decoder
{
public:

	SurfaceCode Code;

	Vector<uint32_t> connection_counts;
	Vector<uint32_t> support;
	Vector<uint32_t> root_of_vertex;

	Vector<Edge> fuseList;


	RootManager mngr;

	Map<uint32_t, Vector<uint32_t>> border_vertices;

	Vector<Edge> peeling_edges;

	void init_cluster(Vector<uint32_t> roots);



	void fusion();

	uint32_t findRoot(uint32_t vertex);


	Vector<Edge> peel(int syndrome[SYN_LEN]);

	ap_uint<CORR_LEN> translate(Vector<Edge> correctionEdges);

	void grow(uint32_t root);

	void mergeBoundary(uint32_t r1, uint32_t r2);

	ap_uint<CORR_LEN> decode(int syndrome[SYN_LEN]);

	void clear();

	//void buildCode();

};

#endif
