#include "Decoder.h"
//#include "hls_print.h"
void Decoder::decode(bool syndrome[SYN_LEN], ap_uint<CORR_LEN>* correction)
{
	ap_uint<CORR_LEN> tmp;
	hls::stream<uint32_t> syn_stream("syn_stream");
#pragma HLS STREAM variable=syn_stream depth=9
	hls::stream<uint32_t> syn_vert_stream("syn_vert_stream");
#pragma HLS STREAM variable=syn_vert_stream depth=9
	hls::stream<Edge> fuseList("fuseList");
#pragma HLS STREAM variable=fuseList depth=18
	hls::stream<Edge> peeling_edges("peeling_edges");
#pragma HLS STREAM variable=peeling_edges depth=18
	hls::stream<Edge> correction_edges("correction_edges");
#pragma HLS STREAM variable=correction_edges depth=18


	//phase 0: clear ds
	clear();
	//phase 1: read syndrome and populate data structs
	initialization(syndrome, syn_stream);
	populate(syn_stream);
	//phase 2: grow and fuse
	UF(fuseList, peeling_edges);

	//phase 3: Peeling
	peel(peeling_edges, correction_edges);

	//phase 4: Translate
	translate(correction_edges, &tmp);

	*correction = tmp;


}


void Decoder::initialization(bool syndrome[SYN_LEN], hls::stream<uint32_t>& syn_stream)
{
READ_SYNDROME:
	for(uint32_t i = 0; i < SYN_LEN; ++i)
	{
		uint32_t tmp = syndrome[i];
		this->syndrome[i] = syndrome[i];
		root_of_vertex[i] = i;
		syn_stream.write(tmp);
	}
}

void Decoder::populate(hls::stream<uint32_t>& syn_stream)
{
	uint32_t i = 0;
POPULATE:
	while(!syn_stream.empty())
	{
		uint32_t tmp = syn_stream.read();
		if(tmp == 1)
		{
			border_vertices[i].emplace(i);
			mngr.fillNFO(i);
		}
		i++;
	}
}

void Decoder::UF(hls::stream<Edge>& fuseList, hls::stream<Edge>& peeling_edges)
{
UNION_FIND:
	while(mngr.hasOddRoots())
	{
		//hls::print("growing\n");
		for(int i = 0; i < mngr.oddRoots.getSize(); ++i)
		{
			grow(mngr.oddRoots.at(i), fuseList);
		}
		fusion(fuseList, peeling_edges);
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


void Decoder::grow(uint32_t root, hls::stream<Edge>& fuseList)
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
		connections = Code.vertex_connections(idk);
		//hls::print("entering INNERGROW loop\n");
INNER_GROW:
		for(int j = 0; j < connections.getSize(); ++j)
		{
#pragma HLS loop_tripcount min=1 max=4
#pragma HLS PIPELINE II=1
			Edge e;

			e.u = min(borders.at(i), connections.at(j));
			e.v = max(borders.at(i), connections.at(j));

			uint32_t edgeIdx = Code.edge_idx(e);
			uint32_t elt = support[edgeIdx];


			if(elt != 2)
			{
				elt++;
				if(elt == 2)
				{
					fuseList.write(e);
				}
				support[edgeIdx] = elt;
			}

		}
	}
}

uint32_t Decoder::findRoot(uint32_t vertex)
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
#pragma HLS PIPELINE II=1
		uint32_t tmp2= path.at(0);
		path.erase(0);
		root_of_vertex[tmp2] = root;
	}
	return root;
}

void Decoder::fusion(hls::stream<Edge>& fuseList, hls::stream<Edge>& peeling_edges)
{
FUSE:
	while(!fuseList.empty())
	{
		Edge e;
		fuseList.read(e);
		uint32_t tmp1 = findRoot(e.u);
		uint32_t tmp2 = findRoot(e.v);
		uint32_t root1=tmp1;
		uint32_t root2=tmp2;
		if(mngr.size(root1)<mngr.size(root2))
		{
			root1=tmp2;
			root2=tmp1;
		}

		if(root1!= root2)
		{
			//hls::print("entering Fuse\n");
			fuse(root1,root2,e);
			peeling_edges.write(e);
		}
	}
}

void Decoder::fuse(uint32_t root1, uint32_t root2, Edge e){
#pragma HLS INLINE
	root_of_vertex[root2] = root1;

	if(!mngr.isRoot(root2))
	{
		whenroot(root1,root2);
	}
	else
	{
		//hls::print("elseroot\n");
		elseroot(root1,root2);
	}
}

void Decoder::whenroot(uint32_t root1, uint32_t root2){
#pragma HLS INLINE
	//hls::print("whenroot\n");
	mngr.growSize(root1);
	border_vertices[root1].elementEmplace(root2);
}

void Decoder::elseroot(uint32_t root1, uint32_t root2){
#pragma HLS DATAFLOW
	mngr.merge(root1, root2);
	mergeBoundary(root1, root2);
}



void Decoder::mergeBoundary(uint32_t r1, uint32_t r2)
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

	border_vertices[r1] = borderR1;
}


void Decoder::peel(hls::stream<Edge>& peeling_edges, hls::stream<Edge>& corrections)
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

			if(syndrome[u] == 1)
			{
				corrections.write(leaf_edge);
				syndrome[u] = 0;
				syndrome[v] ^= 1U;
			}
		}
		else
		{
			peelingVec.emplace(leaf_edge);
		}

	}
	//hls::print("corrections done\n");
}


void Decoder::translate(hls::stream<Edge>& correctionEdges, ap_uint<CORR_LEN>* correction)
{
	//hls::print("translating\n");
	ap_uint<CORR_LEN> tmp = 0;
CORRECTION_TRANSLATION:
	while(!correctionEdges.empty())
	{
		Edge e = correctionEdges.read();


		tmp[Code.edge_idx(e)] = 1;
	}

	*correction = tmp;
}

void Decoder::clear()
{
CLEAR_LOOP:
	for(int i = 0; i < SYN_LEN; i++)
	{
#pragma HLS PIPELINE II=1
		root_of_vertex[i] = 0;
		border_vertices[i].fillnReset(0);
	}

	for(int i = 0; i < CORR_LEN; i++)
	{
		support[i] = 0;
	}
	//hls::print("cleared arrays\n");
	//hls::print("borderVertices cleared\n");
	mngr.clear();
	//hls::print("RootManager cleared\n");
}




