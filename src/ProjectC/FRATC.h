#include "../Defines.h"
#include "../Utility/Vector.h"

#ifndef FRATC
#define FRATC

struct entry
{
    ap_uint<SYN_LEN> syndrome = 0;
	ap_uint<CORR_LEN> correction = 0;
	bool full = false;
};

typedef struct entry Entry;

struct Edge
{
	uint32_t u;
	uint32_t v;

	bool operator==(const Edge& other) const
	{
		return (u == other.u && v == other.v);
	}
};

struct TreeNode{
	Vector<uint32_t> connections;
	uint32_t total_conn = 0;
};

typedef struct TreeNode TreeNode;

struct Coord
{
	uint32_t x;
	uint32_t y;
};

ap_uint<SYN_LEN> binToDec(bool syndrome[SYN_LEN]);

int hash(ap_uint<SYN_LEN> synDec);

void hashInsert(ap_uint<CORR_LEN> correction, bool syndrome[SYN_LEN], Entry map[MAX_SIZE]);

bool hashRetrieve(bool syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction, Entry map[MAX_SIZE]);

uint32_t to_vertex_index(int row, int col);

uint32_t vertex_connection_count(uint32_t vertex);

Vector<uint32_t> vertex_connections(uint32_t v);

uint32_t num_vertices();

Coord vertex_to_coord(const uint32_t vidx);

uint32_t num_edges();

bool is_horizontal(Edge e);

uint32_t left(Edge e);

uint32_t lower(Edge e);

uint32_t to_edge_idx(Edge e);

uint32_t edge_idx(Edge e);

void clear(uint32_t syndrome_cpy[SYN_LEN],
		   uint32_t support[CORR_LEN],
		   uint32_t root_of_vertex[SYN_LEN],
		   Vector<uint32_t> border_vertices[SYN_LEN],
		   Vector<uint32_t>& roots,
		   Vector<uint32_t>& oddRoots,
		   uint32_t sizes[SYN_LEN],
		   uint32_t parity[SYN_LEN],
		   uint32_t connection_counts[SYN_LEN]);

void initialization(bool syndrome[SYN_LEN],
					hls::stream<uint32_t>& syn_stream,
					uint32_t syndrome_cpy[SYN_LEN],
					uint32_t root_of_vertex[SYN_LEN]);

void fillNFO(uint32_t tmp,
		Vector<uint32_t>& roots,
		Vector<uint32_t>& oddRoots,
		uint32_t sizes[SYN_LEN],
		uint32_t parity[SYN_LEN]);

void populate(hls::stream<uint32_t>& syn_stream,
			  Vector<uint32_t> border_vertices[SYN_LEN],
			  Vector<uint32_t>& roots,
			  Vector<uint32_t>& oddRoots,
			  uint32_t sizes[SYN_LEN],
			  uint32_t parity[SYN_LEN]);

uint32_t min(uint32_t a, uint32_t b);

uint32_t max(uint32_t a, uint32_t b);


void grow(uint32_t root,
		  hls::stream<Edge>& fuseList,
		  Vector<uint32_t> border_vertices[SYN_LEN],
		  uint32_t support[CORR_LEN],
		  uint32_t connection_counts[SYN_LEN]);

uint32_t findRoot(uint32_t vertex,
				  uint32_t root_of_vertex[SYN_LEN]);

bool isRoot(uint32_t root, Vector<uint32_t>& roots);

void growSize(uint32_t root, uint32_t sizes[SYN_LEN], Vector<uint32_t>& roots);

void EraseFromAll(uint32_t root,
				  Vector<uint32_t>& oddRoots,
				  Vector<uint32_t>& roots,
				  uint32_t sizes[SYN_LEN],
				  uint32_t parity[SYN_LEN]);

void merge(uint32_t r1,
		   uint32_t r2,
		   Vector<uint32_t>& oddRoots,
		   Vector<uint32_t>& roots,
		   uint32_t sizes[SYN_LEN],
		   uint32_t parity[SYN_LEN]);

void mergeBoundary(uint32_t r1, uint32_t r2, Vector<uint32_t> border_vertices[SYN_LEN], uint32_t connection_counts[SYN_LEN]);


void elseroot(uint32_t root1,
			  uint32_t root2,
		   	  Vector<uint32_t>& oddRoots,
			  Vector<uint32_t>& roots,
			  uint32_t sizes[SYN_LEN],
			  uint32_t parity[SYN_LEN],
			  Vector<uint32_t> border_vertices[SYN_LEN],
			  uint32_t connection_counts[SYN_LEN]);

void fuse(uint32_t root1,
		  uint32_t root2,
		  Edge e,
		  uint32_t root_of_vertex[SYN_LEN],
		  Vector<uint32_t> border_vertices[SYN_LEN],
		  uint32_t sizes[SYN_LEN],
		  uint32_t parity[SYN_LEN],
		  Vector<uint32_t>& roots,
		  Vector<uint32_t>& oddRoots,
		  uint32_t connection_counts[SYN_LEN]);

void fusion(hls::stream<Edge>& fuseList,
			hls::stream<Edge>& peeling_edges,
			uint32_t root_of_vertex[SYN_LEN],
			Vector<uint32_t> border_vertices[SYN_LEN],
			uint32_t sizes[SYN_LEN],
			uint32_t parity[SYN_LEN],
			Vector<uint32_t>& roots,
			Vector<uint32_t>& oddRoots,
			uint32_t connection_counts[SYN_LEN]);

void UF(hls::stream<Edge>& fuseList,
		hls::stream<Edge>& peeling_edges,
		Vector<uint32_t> border_vertices[SYN_LEN],
		uint32_t support[CORR_LEN],
		uint32_t root_of_vertex[SYN_LEN],
		uint32_t sizes[SYN_LEN],
		uint32_t parity[SYN_LEN],
		Vector<uint32_t>& roots,
		Vector<uint32_t>& oddRoots,
		uint32_t connection_counts[SYN_LEN],
		bool& status);

void peel(hls::stream<Edge>& peeling_edges, hls::stream<Edge>& corrections, uint32_t syndrome_cpy[SYN_LEN]);

void translate(hls::stream<Edge>& correctionEdges, ap_uint<CORR_LEN>* correction);

void decode(bool syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction);

extern "C"
{
void decoderTop(bool syndrome[SYN_LEN], bool correction_in[CORR_LEN], bool correction_out[CORR_LEN], bool insert);
}

#endif
