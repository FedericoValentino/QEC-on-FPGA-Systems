#ifndef DECODER_H
#define DECODER_H


#include "../Defines.h"
#include "../Utility/Vector.h"
#include "../Utility/Map.h"
#include "RootManager.h"
#include "SurfaceCode.h"

#include "hls_stream.h"

class Decoder
{
public:

	int syndrome[SYN_LEN];

	SurfaceCode Code;

	uint32_t connection_counts[SYN_LEN] = {0};
	uint32_t support[MAPLEN] = {0};
	uint32_t root_of_vertex[SYN_LEN] = {0};

	hls::stream<Edge> fuseList;


	RootManager mngr;

	Map<Vector<uint32_t>> border_vertices;

	hls::stream<Edge> peeling_edges;

	void init_cluster(Vector<uint32_t> roots);

	void fusion();

	uint32_t findRoot(uint32_t vertex);

	void grow(uint32_t root);

	void mergeBoundary(uint32_t r1, uint32_t r2);

	ap_uint<CORR_LEN> decode(int syndrome[SYN_LEN]);

	void clear();

	void read_syndrome(int syndrome[SYN_LEN]);

	void initialization();

	void UF();

	Vector<Edge> peel();

	ap_uint<CORR_LEN> translate(Vector<Edge> correctionEdges);

	//void buildCode();

};

#endif
