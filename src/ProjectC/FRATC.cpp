#include "FRATC.h"


ap_uint<SYN_LEN> binToDec(bool syndrome[SYN_LEN])
{
	ap_uint<SYN_LEN> sum = 0;
BINARY_TO_DECIMAL_LOOP:
	for(int i = 0; i < SYN_LEN; i++)
	{
#pragma HLS UNROLL
		sum += syndrome[i] * (1<<i);
	}
	return sum;
}

int hash(ap_uint<SYN_LEN> synDec)
{
#pragma HLS INLINE
	return synDec % MAX_SIZE;
}

void hashInsert(ap_uint<CORR_LEN> correction, bool syndrome[SYN_LEN], Entry map[MAX_SIZE])
{
	ap_uint<SYN_LEN> synDec = binToDec(syndrome);
    int index = hash(synDec);
    int counter = 0;
    int finalidx = 0;
    bool found = 0;

HASHMAP_INSERT_LOOP:
	while(counter < MAX_SIZE && !found){
#pragma HLS loop_tripcount min=1 max=100000
#pragma HLS PIPELINE II=1
		counter++;
		if (!map[index].full && !found){
			finalidx = index;
			found=1;
		}
		index++;
		if(index==MAX_SIZE){
			index=0;
		}
	}

    map[finalidx].correction = correction;
    map[finalidx].syndrome = synDec;
    map[finalidx].full = true;


}

bool hashRetrieve(bool syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction, Entry map[MAX_SIZE])
{
	ap_uint<SYN_LEN> synDec = binToDec(syndrome);
	int index = hash(synDec);
	int counter = 0;
	int finalidx = 0;
	bool found = 0;



HASHMAP_RETRIEVE_LOOP:
	while(counter < MAX_SIZE && !found){
#pragma HLS loop_tripcount min=1 max=100000
#pragma HLS PIPELINE II=1
		counter++;
		if (synDec == map[index].syndrome){
			finalidx = index;
			found = 1;
		}
		index++;
		if(index==MAX_SIZE){
			index=0;
		}
	}

	if(!found){
		return 0;
	}
	else{
		Entry oof = map[finalidx];
		*correction = map[finalidx].correction;
		return 1;
	}
}

uint32_t to_vertex_index(int row, int col)
{
	if(row < 0)
	{
		row += L;
	}
	else if(row >= L)
	{
		row -= L;
	}
	if(col < 0)
	{
		col += L;
	}
	else if(col >= L)
	{
		col -= L;
	}


	return L * row + col;
}

uint32_t vertex_connection_count(uint32_t vertex)
{
	return 4;
}

Vector<uint32_t> vertex_connections(uint32_t v)
{
	int row = v/L;
	int col = v%L;

	Vector<uint32_t> vector;
#pragma HLS ARRAY_PARTITION variable=vector.array type=complete

	uint32_t idx1 = to_vertex_index(row - 1, col);
	vector.emplace(idx1);
	uint32_t idx2 = to_vertex_index(row + 1, col);
	vector.emplace(idx2);
	uint32_t idx3 = to_vertex_index(row, col - 1);
	vector.emplace(idx3);
	uint32_t idx4 = to_vertex_index(row, col + 1);
	vector.emplace(idx4);

	return vector;
}

uint32_t num_vertices()
{
	return L*L;
}

Coord vertex_to_coord(const uint32_t vidx)
{
	return {vidx / L, vidx % L};
}

uint32_t num_edges()
{
	return 2*L*L;
}

bool is_horizontal(Edge e)
{
	return ((e.v - e.u) == 1) || ((e.v - e.u) == (L - 1));
}

uint32_t left(Edge e)
{
	if((e.v - e.u) == 1)
	{
		return e.u;
	}
	return e.v;
}

uint32_t lower(Edge e)
{
	if((e.v - e.u) == L)
	{
		return e.v;
	}
	return e.u;
}

uint32_t to_edge_idx(Edge e)
{
	if(is_horizontal(e))
	{
		Coord tmp=vertex_to_coord(left(e));
		return L*(L+tmp.x)+tmp.y;
	}
	else
	{
		return lower(e);
	}
}

uint32_t edge_idx(Edge e)
{
	return to_edge_idx(e);
}

void clear(uint32_t syndrome_cpy[SYN_LEN],
		   uint32_t support[CORR_LEN],
		   uint32_t root_of_vertex[SYN_LEN],
		   Vector<uint32_t> border_vertices[SYN_LEN],
		   Vector<uint32_t>& roots,
		   Vector<uint32_t>& oddRoots,
		   uint32_t sizes[SYN_LEN],
		   uint32_t parity[SYN_LEN],
		   uint32_t connection_counts[SYN_LEN])
{
	roots.fillnReset(0);
	oddRoots.fillnReset(0);
CLEAR_LOOP:
	for(int i = 0; i < SYN_LEN; i++)
	{
		syndrome_cpy[i] = 0;
		root_of_vertex[i] = 0;
		sizes[i] = 0;
		parity[i] = 0;
		connection_counts[i] = 0;
		border_vertices[i].fillnReset(0);
	}

	for(int i = 0; i < CORR_LEN; i++)
	{
		support[i] = 0;
	}
}

void initialization(bool syndrome[SYN_LEN],
					hls::stream<uint32_t>& syn_stream,
					uint32_t syndrome_cpy[SYN_LEN],
					uint32_t root_of_vertex[SYN_LEN])
{
READ_SYNDROME:
	for(uint32_t i = 0; i < SYN_LEN; ++i)
	{
		uint32_t tmp = syndrome[i];
		syndrome_cpy[i] = tmp;
		root_of_vertex[i] = i;
		syn_stream.write(tmp);
	}
}

void fillNFO(uint32_t tmp,
		Vector<uint32_t>& roots,
		Vector<uint32_t>& oddRoots,
		uint32_t sizes[SYN_LEN],
		uint32_t parity[SYN_LEN])
{
#pragma HLS DATAFLOW
	roots.elementEmplace(tmp);
	oddRoots.elementEmplace(tmp);
	sizes[tmp] = 1;
	parity[tmp] = 1;
}

void populate(hls::stream<uint32_t>& syn_stream,
			  Vector<uint32_t> border_vertices[SYN_LEN],
			  Vector<uint32_t>& roots,
			  Vector<uint32_t>& oddRoots,
			  uint32_t sizes[SYN_LEN],
			  uint32_t parity[SYN_LEN])
{
	uint32_t i = 0;
POPULATE:
	while(!syn_stream.empty())
	{
		uint32_t tmp = syn_stream.read();
		if(tmp == 1)
		{
			border_vertices[i].emplace(i);
			fillNFO(i,roots, oddRoots, sizes, parity);
		}
		i++;
	}
}

uint32_t min(uint32_t a, uint32_t b){
	if(a<b)
		return a;
	else
		return b;
}

uint32_t max(uint32_t a, uint32_t b){
	if(a>b)
		return a;
	else
		return b;
}


void grow(uint32_t root,
		  hls::stream<Edge>& fuseList,
		  Vector<uint32_t> border_vertices[SYN_LEN],
		  uint32_t support[CORR_LEN],
		  uint32_t connection_counts[SYN_LEN])
{
	static Vector<uint32_t> borders;
	borders = border_vertices[root];
	//hls::print("entering GROW loop\n");
GROW:
	for(int i = 0; i < borders.getSize(); i++)
	{
#pragma HLS loop_tripcount min=1 max=128
		static Vector<uint32_t> connections;
		uint32_t idk = borders.at(i);
		connections = vertex_connections(idk);
		//hls::print("entering INNERGROW loop\n");
INNER_GROW:
		for(int j = 0; j < connections.getSize(); ++j)
		{
#pragma HLS loop_tripcount min=1 max=4
#pragma HLS PIPELINE II=1
			Edge e;

			e.u = min(borders.at(i), connections.at(j));
			e.v = max(borders.at(i), connections.at(j));

			uint32_t edgeIdx = edge_idx(e);
			uint32_t elt = support[edgeIdx];

			uint32_t u = connection_counts[e.u]+1;
			uint32_t v = connection_counts[e.v]+1;


			if(elt != 2)
			{
				elt++;
				if(elt == 2)
				{
					connection_counts[e.u]=u;
					connection_counts[e.v]=v;
					fuseList.write(e);
				}
				support[edgeIdx] = elt;
			}

		}
	}
}

uint32_t findRoot(uint32_t vertex,
				  uint32_t root_of_vertex[SYN_LEN])
{
	//hls::print("Finding root for vertex %d\n", vertex);
	uint32_t tmp = root_of_vertex[vertex];

	if(tmp == vertex)
	{
		return vertex;
	}

	static Vector<uint32_t> path;
	path.fillnReset(0);
	uint32_t root=tmp;
	tmp = root_of_vertex[root];
	path.emplace(root);
FIND_ROOT:
	while(tmp != root)
	{
#pragma HLS PIPELINE II=1
		root = tmp;
		tmp = root_of_vertex[root];
		path.emplace(root);
	}

SET_ROOT:
	while(path.getSize() > 0)
	{
		uint32_t tmp2= path.at(0);
		path.erase(0);
		root_of_vertex[tmp2] = root;
	}
	return root;
}

bool isRoot(uint32_t root, Vector<uint32_t>& roots)
{
	uint32_t count = 0;
	uint32_t tmp = roots.at(0);
IS_ROOT:
	for(int i = 0; i < roots.getSize(); ++i)
	{
		if(root == tmp)
		{
			++count;
		}
		tmp = roots.at(i+1);
	}
	return count==1;
}

void growSize(uint32_t root, uint32_t sizes[SYN_LEN], Vector<uint32_t>& roots)
{
	if(isRoot(root, roots))
	{
		sizes[root]++;
	}

}

void EraseFromAll(uint32_t root,
				  Vector<uint32_t>& oddRoots,
				  Vector<uint32_t>& roots,
				  uint32_t sizes[SYN_LEN],
				  uint32_t parity[SYN_LEN])
{
#pragma HLS DATAFLOW
	oddRoots.elementErase(root);
	sizes[root] = 0;
	parity[root] = 0;
	roots.elementErase(root);
}

void merge(uint32_t r1,
		   uint32_t r2,
		   Vector<uint32_t>& oddRoots,
		   Vector<uint32_t>& roots,
		   uint32_t sizes[SYN_LEN],
		   uint32_t parity[SYN_LEN])
{
	uint32_t p1 = parity[r1];
	uint32_t p2 = parity[r2];
	uint32_t newParity = p1 + p2;

	if((newParity % 2)== 1)
	{
		oddRoots.elementEmplace(r1);
	}
	else
	{
		oddRoots.elementErase(r1);
	}


	if(isRoot(r2, roots))
	{
		uint32_t s1 = sizes[r1];
		uint32_t s2 = sizes[r2];
		sizes[r1] = s1+s2;
	}
	parity[r1] = newParity;

	EraseFromAll(r2, oddRoots, roots, sizes, parity);

}

void mergeBoundary(uint32_t r1, uint32_t r2, Vector<uint32_t> border_vertices[SYN_LEN], uint32_t connection_counts[SYN_LEN])
{
	//hls::print("Merging\n");
#pragma HLS INLINE off
	static Vector<uint32_t> borderR1;
	static Vector<uint32_t> borderR2;
	borderR1 = border_vertices[r1];
	borderR2 = border_vertices[r2];
	uint32_t size2 = borderR2.getSize();

MERGE:
	for(int i = 0; i<size2; ++i)
	{
#pragma HLS loop_tripcount min=4 max=64
		uint32_t vertex = borderR2.at(i);
		borderR1.elementEmplace(vertex);
	}

ERASE_LEFTOVERS:
	for(int i = 0; i<borderR2.getSize(); ++i)
	{
		uint32_t vertex = borderR2.at(i);
		if(connection_counts[vertex] == 4)
		{
			borderR1.elementErase(vertex);
		}
	}

	border_vertices[r1] = borderR1;
	border_vertices[r2].fillnReset(0);
}


void elseroot(uint32_t root1,
			  uint32_t root2,
		   	  Vector<uint32_t>& oddRoots,
			  Vector<uint32_t>& roots,
			  uint32_t sizes[SYN_LEN],
			  uint32_t parity[SYN_LEN],
			  Vector<uint32_t> border_vertices[SYN_LEN],
			  uint32_t connection_counts[SYN_LEN])
{
#pragma HLS DATAFLOW
	merge(root1, root2, oddRoots, roots, sizes, parity);
	mergeBoundary(root1, root2, border_vertices, connection_counts);
}

void fuse(uint32_t root1,
		  uint32_t root2,
		  Edge e,
		  uint32_t root_of_vertex[SYN_LEN],
		  Vector<uint32_t> border_vertices[SYN_LEN],
		  uint32_t sizes[SYN_LEN],
		  uint32_t parity[SYN_LEN],
		  Vector<uint32_t>& roots,
		  Vector<uint32_t>& oddRoots,
		  uint32_t connection_counts[SYN_LEN])
{
#pragma HLS INLINE
	root_of_vertex[root2] = root1;

	if(!isRoot(root2, roots))
	{
		growSize(root1, sizes, roots);
		border_vertices[root1].elementEmplace(root2);
	}
	else
	{
		//hls::print("elseroot\n");
		elseroot(root1,root2, oddRoots, roots, sizes, parity, border_vertices, connection_counts);
	}
}

void fusion(hls::stream<Edge>& fuseList,
			hls::stream<Edge>& peeling_edges,
			uint32_t root_of_vertex[SYN_LEN],
			Vector<uint32_t> border_vertices[SYN_LEN],
			uint32_t sizes[SYN_LEN],
			uint32_t parity[SYN_LEN],
			Vector<uint32_t>& roots,
			Vector<uint32_t>& oddRoots,
			uint32_t connection_counts[SYN_LEN])
{
FUSE:
	while(!fuseList.empty())
	{
		Edge e;
		fuseList.read(e);
		uint32_t tmp1 = findRoot(e.u, root_of_vertex);
		uint32_t tmp2 = findRoot(e.v, root_of_vertex);
		uint32_t root1=tmp1;
		uint32_t root2=tmp2;
		if(sizes[root1]<sizes[root2])
		{
			root1=tmp2;
			root2=tmp1;
		}

		if(root1!= root2)
		{
			//hls::print("entering Fuse\n");
			fuse(root1,root2,e, root_of_vertex, border_vertices, sizes, parity, roots, oddRoots, connection_counts);
			peeling_edges.write(e);
		}
	}
}

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
		bool& status)
{
	hls::stream<Edge> fuse;
#pragma HLS STREAM variable=fuse depth=128
	int iterations = 0;
UNION_FIND:
	while(oddRoots.getSize() > 0 && iterations < MAX_ITERATIONS)
	{
		//hls::print("growing\n");
		for(int i = 0; i < oddRoots.getSize(); ++i)
		{
			grow(oddRoots.at(i), fuse, border_vertices, support, connection_counts);
		}
		fusion(fuse, peeling_edges, root_of_vertex, border_vertices, sizes, parity, roots, oddRoots, connection_counts);
		iterations++;
	}
}

void peel(hls::stream<Edge>& peeling_edges, hls::stream<Edge>& corrections, uint32_t syndrome_cpy[SYN_LEN])
{
	//hls::print("starting peeling process\n");

	static Vector<Edge> peelingVec;
	peelingVec.fillnReset({0,0});

	int vertex_count[SYN_LEN] = {0};
#pragma HLS ARRAY_PARTITION variable=vertex_count type=complete

PEEL_PREPARE:
	while(!peeling_edges.empty())
	{
#pragma HLS PIPELINE II=1
		Edge e = peeling_edges.read();
		uint32_t tmp1 = vertex_count[e.u];
		uint32_t tmp2 = vertex_count[e.v];
		tmp1++;
		tmp2++;
		vertex_count[e.u] = tmp1;
		vertex_count[e.v] = tmp2;
		peelingVec.emplace(e);
	}
	//hls::print("read peelng_edges\n");
PEELING:
	while(peelingVec.getSize() > 0)
	{
#pragma HLS DEPENDENCE variable=vertex_count inter false
		Edge leaf_edge = peelingVec.at(0);
		peelingVec.erase(0);
		uint32_t u = 0;
		uint32_t v = 0;

		if(vertex_count[leaf_edge.u] == 1 || vertex_count[leaf_edge.v] == 1)
		{
			if(vertex_count[leaf_edge.u] == 1)
			{
				u = leaf_edge.u;
				v = leaf_edge.v;
			}
			else if(vertex_count[leaf_edge.v] == 1)
			{
				u = leaf_edge.v;
				v = leaf_edge.u;
			}

			--vertex_count[u];
			--vertex_count[v];

			if(syndrome_cpy[u] == 1)
			{
				corrections.write(leaf_edge);
				syndrome_cpy[u] = 0;
				syndrome_cpy[v] ^= 1U;
			}
		}
		else
		{
			peelingVec.emplace(leaf_edge);
		}

	}
	//hls::print("corrections done\n");
}

void translate(hls::stream<Edge>& correctionEdges, ap_uint<CORR_LEN>* correction)
{
	//hls::print("translating\n");
	ap_uint<CORR_LEN> tmp = 0;
CORRECTION_TRANSLATION:
	while(!correctionEdges.empty())
	{
		Edge e = correctionEdges.read();


		tmp[edge_idx(e)] = 1;
	}
	*correction = tmp;
}

void perfCounterProc(hls::stream<int64_t>& cmd, int64_t* out) {
    int64_t val;
    // wait to receive a value to start counting
    int64_t cnt = cmd.read();
// keep counting until a value is available
    count:
    while (cmd.read_nb(val) == false) {
        cnt++;
    }

    *out = cnt;
}

void populateCounter(hls::stream<uint32_t>& syn_stream,
		Vector<uint32_t> border_vertices[SYN_LEN],
		uint32_t sizes[SYN_LEN],
		uint32_t parity[SYN_LEN],
		Vector<uint32_t>& roots,
		Vector<uint32_t>& oddRoots,
		hls::stream<Edge>& fuseList,
		hls::stream<Edge>& peeling_edges,
		uint32_t support[CORR_LEN],
		uint32_t root_of_vertex[SYN_LEN],
		uint32_t connection_counts[SYN_LEN],
		bool& status,
		hls::stream<Edge>& correction_edges,
		uint32_t syndrome_cpy[SYN_LEN],
              hls::stream<int64_t>& cmd)
{
    cmd.write(1);
    populate(syn_stream, border_vertices, roots, oddRoots, sizes, parity);
    cmd.write(1);
}


void populateCounterDF(hls::stream<uint32_t>& syn_stream,
		Vector<uint32_t> border_vertices[SYN_LEN],
		uint32_t sizes[SYN_LEN],
		uint32_t parity[SYN_LEN],
		Vector<uint32_t>& roots,
		Vector<uint32_t>& oddRoots,
		hls::stream<Edge>& fuseList,
		hls::stream<Edge>& peeling_edges,
		uint32_t support[CORR_LEN],
		uint32_t root_of_vertex[SYN_LEN],
		uint32_t connection_counts[SYN_LEN],
		bool& status,
		hls::stream<Edge>& correction_edges,
		uint32_t syndrome_cpy[SYN_LEN],
               int64_t* cc)
{
    hls::stream<int64_t> cmdForCounter;

#pragma HLS DATAFLOW
    populateCounter(syn_stream, border_vertices, sizes, parity, roots, oddRoots, fuseList,
		    peeling_edges, support, root_of_vertex, connection_counts, status, correction_edges, syndrome_cpy, cmdForCounter);
    perfCounterProc(cmdForCounter, cc);
}

void UFCounter(hls::stream<uint32_t>& syn_stream,
		Vector<uint32_t> border_vertices[SYN_LEN],
		uint32_t sizes[SYN_LEN],
		uint32_t parity[SYN_LEN],
		Vector<uint32_t>& roots,
		Vector<uint32_t>& oddRoots,
		hls::stream<Edge>& fuseList,
		hls::stream<Edge>& peeling_edges,
		uint32_t support[CORR_LEN],
		uint32_t root_of_vertex[SYN_LEN],
		uint32_t connection_counts[SYN_LEN],
		bool& status,
		hls::stream<Edge>& correction_edges,
		uint32_t syndrome_cpy[SYN_LEN],
              hls::stream<int64_t>& cmd)
{
    cmd.write(1);
    UF(fuseList,peeling_edges, border_vertices, support, root_of_vertex, sizes, parity, roots, oddRoots, connection_counts, status);
    cmd.write(1);
}


void UFCounterDF(hls::stream<uint32_t>& syn_stream,
		Vector<uint32_t> border_vertices[SYN_LEN],
		uint32_t sizes[SYN_LEN],
		uint32_t parity[SYN_LEN],
		Vector<uint32_t>& roots,
		Vector<uint32_t>& oddRoots,
		hls::stream<Edge>& fuseList,
		hls::stream<Edge>& peeling_edges,
		uint32_t support[CORR_LEN],
		uint32_t root_of_vertex[SYN_LEN],
		uint32_t connection_counts[SYN_LEN],
		bool& status,
		hls::stream<Edge>& correction_edges,
		uint32_t syndrome_cpy[SYN_LEN],
               int64_t* cc)
{
    hls::stream<int64_t> cmdForCounter;

#pragma HLS DATAFLOW
    UFCounter(syn_stream, border_vertices, sizes, parity, roots, oddRoots, fuseList,
		    peeling_edges, support, root_of_vertex, connection_counts, status, correction_edges, syndrome_cpy, cmdForCounter);
    perfCounterProc(cmdForCounter, cc);
}

void peelCounter(hls::stream<uint32_t>& syn_stream,
		Vector<uint32_t> border_vertices[SYN_LEN],
		uint32_t sizes[SYN_LEN],
		uint32_t parity[SYN_LEN],
		Vector<uint32_t>& roots,
		Vector<uint32_t>& oddRoots,
		hls::stream<Edge>& fuseList,
		hls::stream<Edge>& peeling_edges,
		uint32_t support[CORR_LEN],
		uint32_t root_of_vertex[SYN_LEN],
		uint32_t connection_counts[SYN_LEN],
		bool& status,
		hls::stream<Edge>& correction_edges,
		uint32_t syndrome_cpy[SYN_LEN],
              hls::stream<int64_t>& cmd)
{
    cmd.write(1);
    peel(peeling_edges, correction_edges, syndrome_cpy);
    cmd.write(1);
}


void peelCounterDF(hls::stream<uint32_t>& syn_stream,
		Vector<uint32_t> border_vertices[SYN_LEN],
		uint32_t sizes[SYN_LEN],
		uint32_t parity[SYN_LEN],
		Vector<uint32_t>& roots,
		Vector<uint32_t>& oddRoots,
		hls::stream<Edge>& fuseList,
		hls::stream<Edge>& peeling_edges,
		uint32_t support[CORR_LEN],
		uint32_t root_of_vertex[SYN_LEN],
		uint32_t connection_counts[SYN_LEN],
		bool& status,
		hls::stream<Edge>& correction_edges,
		uint32_t syndrome_cpy[SYN_LEN],
               int64_t* cc)
{
    hls::stream<int64_t> cmdForCounter;

#pragma HLS DATAFLOW
    peelCounter(syn_stream, border_vertices, sizes, parity, roots, oddRoots, fuseList,
		    peeling_edges, support, root_of_vertex, connection_counts, status, correction_edges, syndrome_cpy, cmdForCounter);
    perfCounterProc(cmdForCounter, cc);
}

void decode(bool syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction, int64_t* cc)
{
	static uint32_t syndrome_cpy[SYN_LEN];
	//decoder
	static uint32_t support[CORR_LEN];
	static uint32_t root_of_vertex[SYN_LEN];
	static Vector<uint32_t> border_vertices[SYN_LEN];
	static uint32_t connection_counts[SYN_LEN];
#pragma ARRAY_PARTITION variable=connection_counts type=complete
	//manager
	static Vector<uint32_t> roots;
	static Vector<uint32_t> oddRoots;
	static uint32_t sizes[SYN_LEN];
	static uint32_t parity[SYN_LEN];

	bool status = true;

	int64_t CCPopulate;
	int64_t CCUF;
	int64_t CCPeel;


	ap_uint<CORR_LEN> tmp = 0;
	hls::stream<uint32_t> syn_stream("syn_stream");
#pragma HLS STREAM variable=syn_stream depth=64
	hls::stream<uint32_t> syn_vert_stream("syn_vert_stream");
#pragma HLS STREAM variable=syn_vert_stream depth=64
	hls::stream<Edge> fuseList("fuseList");
#pragma HLS STREAM variable=fuseList depth=128
	hls::stream<Edge> peeling_edges("peeling_edges");
#pragma HLS STREAM variable=peeling_edges depth=128
	hls::stream<Edge> correction_edges("correction_edges");
#pragma HLS STREAM variable=correction_edges depth=128


	//phase 0: clear ds
	clear(syndrome_cpy, support, root_of_vertex, border_vertices, roots, oddRoots, sizes, parity, connection_counts);
	//phase 1: read syndrome and populate data structs
	initialization(syndrome, syn_stream, syndrome_cpy, root_of_vertex);
	populateCounterDF(syn_stream, border_vertices, sizes, parity, roots, oddRoots, fuseList,
			peeling_edges, support, root_of_vertex, connection_counts, status, correction_edges, syndrome_cpy, &CCPopulate);
	//phase 2
	UFCounterDF(syn_stream, border_vertices, sizes, parity, roots, oddRoots, fuseList,
				peeling_edges, support, root_of_vertex, connection_counts, status, correction_edges, syndrome_cpy, &CCUF);

	//phase 3
	peelCounterDF(syn_stream, border_vertices, sizes, parity, roots, oddRoots, fuseList,
					peeling_edges, support, root_of_vertex, connection_counts, status, correction_edges, syndrome_cpy, &CCPeel);


	//phase 4: Translate
	translate(correction_edges, &tmp);

	*cc = CCPopulate + CCUF + CCPeel;


	*correction = tmp;
}


void decoderTop(bool syndrome[SYN_LEN], bool correction_out[CORR_LEN], int64_t* totalClocks)
{
	/*static Entry map[MAX_SIZE];
#pragma HLS ARRAY_PARTITION variable=map type=complete
#pragma HLS BIND_STORAGE variable=map type=RAM_2P impl=LUTRAM*/
	ap_uint<CORR_LEN> correction_input = 0;
	ap_uint<CORR_LEN> correction_output = 0;


	//axi
#pragma HLS INTERFACE m_axi port=syndrome offset=slave bundle=gmem0 depth=64
#pragma HLS INTERFACE m_axi port=correction_out offset=slave bundle=gmem1 depth=128
#pragma HLS INTERFACE m_axi port=totalClocks offset=slave bundle=gmem2 depth=2

#pragma HLS INTERFACE s_axilite port=syndrome bundle=control
#pragma HLS INTERFACE s_axilite port=correction_out bundle=control
#pragma HLS INTERFACE s_axilite port=totalClocks bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

	bool tmp = false;

	decode(syndrome, &correction_output, totalClocks);
	for(int i = 0; i < CORR_LEN; ++i)
	{
		correction_out[i] = 0;
		correction_out[i] = correction_output[i];
	}
}
