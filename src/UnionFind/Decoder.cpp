#include "Decoder.h"

ap_uint<CORR_LEN> Decoder::decode(int syndrome[SYN_LEN])
{
	initialization(syndrome);
	UF();
	Vector<Edge> correction = peel(syndrome);
	ap_uint<CORR_LEN> FinalCorrection = translate(correction);
	return FinalCorrection;
}


void Decoder::initialization(int syndrome[SYN_LEN])
{
	Vector<uint32_t> syndrome_vertices;
READ_SYNDROME:
	for(uint32_t i = 0; i < SYN_LEN; ++i)
	{
#pragma HLS PIPELINE II=1
		uint32_t tmp = syndrome[i];
		if(tmp % 2 != 0)
		{
			syndrome_vertices.emplace(i);
		}
	}

	init_cluster(syndrome_vertices);
}

void Decoder::UF()
{
UNION_FIND:
	while(mngr.hasOddRoots())
	{
		for(int i = 0; i < mngr.oddRoots_()->getSize(); ++i)
		{
#pragma HLS PIPELINE II=1
			grow(*mngr.oddRoots_()->get(i));
		}
		fusion();
	}
}

void Decoder::init_cluster(Vector<uint32_t> roots)
{
	mngr.initializeRoots(roots);
BORDER_INIT:
	for(uint32_t i = 0; i < roots.getSize(); ++i)
	{
#pragma HLS PIPELINE II=1
		Vector<uint32_t> Border;
		uint32_t tmp = roots.at(i);
		Border.elementEmplace(tmp);
		border_vertices.add(tmp, Border);
	}

ROOT_INIT:
	for(uint32_t i = 0; i < SYN_LEN; ++i)
	{
#pragma HLS UNROLL
		root_of_vertex[i] = i;
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


void Decoder::grow(uint32_t root)
{
	Vector<uint32_t> borders;
	borders = border_vertices.find(root);
GROW:
	for(int i = 0; i < borders.getSize(); i++)
	{
#pragma HLS loop_tripcount min=1 max=128
		Vector<uint32_t> connections;
		uint32_t idk = borders.at(i);
		connections = Code.vertex_connections(idk);
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
				support[edgeIdx]++;
				elt = support[edgeIdx];
				if(elt == 2)
				{
					connection_counts[e.u]++;
					connection_counts[e.v]++;
					fuseList.write(e);
				}
			}

		}
	}
}

uint32_t Decoder::findRoot(uint32_t vertex)
{
	uint32_t tmp = root_of_vertex[vertex];

	if(tmp == vertex)
	{
		return vertex;
	}

	hls::stream<uint32_t> path;
#pragma HLS STREAM variable=path depth=64
	uint32_t root;
FIND_ROOT:
	do
	{
#pragma HLS PIPELINE II=1
		root = tmp;
		path.write(root);
		tmp = root_of_vertex[root];
	}while(tmp != root);

SET_ROOT:
	while(!path.empty())
	{
#pragma HLS PIPELINE II=1
		uint32_t tmp2 = path.read();
		root_of_vertex[tmp2] = root;
	}
	return root;
}

void Decoder::fusion()
{
FUSE:
	while(!fuseList.empty())
	{
#pragma HLS PIPELINE II=1
		Edge e = fuseList.read();
		uint32_t root1 = findRoot(e.u);
		uint32_t root2 = findRoot(e.v);

		if(root1 != root2)
		{
			peeling_edges.write(e);

			if(mngr.size(root1) < mngr.size(root2))
			{
				uint32_t tmp = root1;
				root1 = root2;
				root2 = tmp;
			}

			root_of_vertex[root2] = root1;

			if(!mngr.isRoot(root2))
			{
				mngr.growSize(root1);
				Vector<uint32_t> border;
				border = border_vertices.find(root1);
				border.elementEmplace(root2);
				border_vertices.update(root1, border);
			}
			else
			{
				mngr.merge(root1, root2);
				mergeBoundary(root1, root2);
				border_vertices.erase(root2);
			}
		}


	}
}



void Decoder::mergeBoundary(uint32_t r1, uint32_t r2)
{
#pragma HLS INLINE off
	Vector<uint32_t> borderR1;
	Vector<uint32_t> borderR2;
	borderR1 = border_vertices.find(r1);
	borderR2 = border_vertices.find(r2);
	uint32_t size2 = borderR2.getSize();

MERGE:
	for(int i = 0; i<size2; ++i)
	{
#pragma HLS loop_tripcount min=4 max=64
#pragma HLS PIPELINE II=1
		uint32_t vertex = borderR2.at(i);
		borderR1.elementEmplace(vertex);
	}
ERASE_LEFTOVERS:
	for(int i = 0; i<size2; ++i)
	{
#pragma HLS loop_tripcount min=4 max=64
#pragma HLS PIPELINE II=1
		uint32_t vertex = borderR2.at(i);
		if(connection_counts[vertex] == 4)
		{
			borderR1.elementErase(vertex);
		}
	}

	border_vertices.update(r1, borderR1);
}


Vector<Edge> Decoder::peel(int syndrome[SYN_LEN])
{
	Vector<Edge> corrections;


	int vertex_count[CORR_LEN] = {0};
	uint32_t size = peeling_edges.size();
PEEL_PREPARE:
	for(int i = 0; i < size; ++i)
	{
#pragma HLS PIPELINE II=1
		Edge e = peeling_edges.read();

		++vertex_count[e.u];
		++vertex_count[e.v];
		peeling_edges.write(e);
	}
PEELING:
	while(!peeling_edges.empty())
	{
#pragma HLS PIPELINE II=1
		Edge leaf_edge = peeling_edges.read();
		uint32_t u = 0;
		uint32_t v = 0;

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
		else
		{
			peeling_edges.write(leaf_edge);
			continue;
		}

		--vertex_count[u];
		--vertex_count[v];

		if(syndrome[u] == 1)
		{
			corrections.emplace(leaf_edge);
			syndrome[u] = 0;
			syndrome[v] ^= 1U;
		}
	}

	return corrections;
}


ap_uint<CORR_LEN> Decoder::translate(Vector<Edge> correctionEdges)
{
	ap_uint<CORR_LEN> correction = 0;
CORRECTION_TRANSLATION:
	for(int i = 0; i < correctionEdges.getSize(); ++i)
	{
		Edge e = correctionEdges.at(i);


		correction[Code.edge_idx(e)] = 1;
	}
	return correction;
}

void Decoder::clear()
{
CLEAR_LOOP:
	for(int i = 0; i < MAPLEN; i++)
	{
#pragma HLS PIPELINE II=1
		connection_counts[i] = 0;
		support[i] = 0;
		root_of_vertex[i] = 0;
	}
	border_vertices.reset();
	mngr.clear();
}


/*
void Decoder::buildCode()
{
	Code.buildCode();
}
*/





