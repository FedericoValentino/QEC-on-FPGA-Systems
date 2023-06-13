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

	uint32_t support[CORR_LEN];
	uint32_t root_of_vertex[SYN_LEN];
	Vector<uint32_t> border_vertices[SYN_LEN];


	RootManager mngr;

	void decode(int syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction);

	void initialization(int syndrome[SYN_LEN], hls::stream<uint32_t>& syn_stream);
	void populate(hls::stream<uint32_t>& syn_stream);

	void UF(hls::stream<Edge>& fuseList, hls::stream<Edge>& peeling_edges);
	void grow(uint32_t root, hls::stream<Edge>& fuseList);
	uint32_t findRoot(uint32_t vertex);

	void fusion(hls::stream<Edge>& fuseList, hls::stream<Edge>& peeling_edges);
	void fuse(uint32_t root1, uint32_t root2, Edge e);
	void whenroot(uint32_t root1, uint32_t root2);
	void elseroot(uint32_t root1, uint32_t root2);

	void mergeBoundary(uint32_t r1, uint32_t r2);

	void peel(hls::stream<Edge>& peeling_edges, hls::stream<Edge>& corrections);

	void translate(hls::stream<Edge>& correctionEdges, ap_uint<CORR_LEN>* correction);

	void clear();




};

#endif
