#include "Decoder.h"

ap_uint<CORR_LEN> Decoder::decode(int syndrome[SYN_LEN])
{
	Vector<uint32_t> syndrome_vertices;

	for(uint32_t i = 0; i < SYN_LEN; ++i)
	{
		if(syndrome[i] % 2 != 0)
		{
			syndrome_vertices.emplace(i);
		}
	}

	init_cluster(syndrome_vertices);

	while(mngr.hasOddRoots())
	{
		for(int i = 0; i < mngr.oddRoots_()->getSize(); ++i)
		{
			grow(*mngr.oddRoots_()->get(i));
		}
		fusion();
	}

	Vector<Edge> correction = peel(syndrome);


	ap_uint<CORR_LEN> FinalCorrection = translate(correction);

	return FinalCorrection;
}

void Decoder::init_cluster(Vector<uint32_t> roots)
{
	mngr.initializeRoots(roots);
	for(uint32_t i = 0; i < roots.getSize(); ++i)
	{
		Vector<uint32_t> Border;
		Border.elementEmplace(roots.at(i));
		border_vertices.add(roots.at(i), Border);
	}

	for(uint32_t i = 0; i < Code.num_vertices(); ++i)
	{
		root_of_vertex.set(i, i);
	}
}


void Decoder::grow(uint32_t root)
{
	Vector<uint32_t>* borders = border_vertices.find(root);
	for(int i = 0; i < borders->getSize(); i++)
	{
		Vector<uint32_t> connections = Code.vertex_connections(borders->at(i));
		for(int j = 0; j < connections.getSize(); ++j)
		{
			Edge e;

			e.u = std::min(borders->at(i), connections.at(j));
			e.v = std::max(borders->at(i), connections.at(j));

			uint32_t edgeIdx = Code.edge_idx(e);
			uint32_t* elt = support.get(edgeIdx);
			if(*elt == 2)
			{
				continue;
			}
			if(++*elt == 2)
			{
				++*connection_counts.get(e.u);
				++*connection_counts.get(e.v);
				fuseList.emplace(e);
			}
		}
	}
}

uint32_t Decoder::findRoot(uint32_t vertex)
{
	uint32_t tmp = root_of_vertex.at(vertex);

	if(tmp == vertex)
	{
		return vertex;
	}

	Vector<uint32_t> path;
	uint32_t root;
	do
	{
		root = tmp;
		path.emplace(root);
		tmp = root_of_vertex.at(root);
	}while(tmp != root);

	for(uint32_t i = 0; i < path.getSize(); ++i)
	{
		root_of_vertex.set(root, path.at(i));
	}
	return root;
}

void Decoder::fusion()
{
	while(fuseList.getSize() != 0)
	{
		Edge e = *fuseList.get(0);
		fuseList.erase(0);
		uint32_t root1 = findRoot(e.u);
		uint32_t root2 = findRoot(e.v);

		if(root1 == root2)
		{
			continue;
		}

		peeling_edges.emplace(e);

		if(mngr.size(root1) < mngr.size(root2))
		{
			uint32_t tmp = root1;
			root1 = root2;
			root2 = tmp;
		}

		root_of_vertex.set(root1, root2);

		if(!mngr.isRoot(root2))
		{
			mngr.growSize(root1);
			border_vertices.find(root1)->emplace(root2);
		}
		else
		{
			mngr.merge(root1, root2);
			mergeBoundary(root1, root2);
		}
	}
}



void Decoder::mergeBoundary(uint32_t r1, uint32_t r2)
{
	Vector<uint32_t>* borderR1 = border_vertices.find(r1);
	Vector<uint32_t>* borderR2 = border_vertices.find(r2);

	for(int i = 0; i<borderR2->getSize(); ++i)
	{
		borderR1->emplace(borderR2->at(i));
	}

	for(int i = 0; i<borderR2->getSize(); ++i)
	{
		uint32_t vertex = borderR2->at(i);
		if(connection_counts.at(vertex) == Code.vertex_connection_count(vertex))
		{
			borderR1->elementErase(vertex);
		}
	}
	border_vertices.erase(r2);
}


Vector<Edge> Decoder::peel(int syndrome[SYN_LEN])
{
	Vector<Edge> corrections;


	int vertex_count[CORR_LEN] = {0};

	for(int i = 0; i < peeling_edges.getSize(); ++i)
	{
		Edge* e = peeling_edges.get(i);

		++vertex_count[e->u];
		++vertex_count[e->v];
	}

	while(peeling_edges.getSize() != 0)
	{
		Edge leaf_edge = peeling_edges.at(peeling_edges.getSize()-1);
		peeling_edges.erase(peeling_edges.getSize()-1);
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
			peeling_edges.pushFront(leaf_edge);
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
	for(int i = 0; i < correctionEdges.getSize(); ++i)
	{
		Edge e = correctionEdges.at(i);


		correction[Code.edge_idx(e)] = 1;
	}
	return correction;
}

void Decoder::clear()
{
	connection_counts.fillnReset(0);
	support.fillnReset(0);
	root_of_vertex.fillnReset(0);
	border_vertices.reset();

	Edge e = {0,0};
	fuseList.fillnReset(e);
	peeling_edges.fillnReset(e);

	mngr.clear();
}


/*
void Decoder::buildCode()
{
	Code.buildCode();
}
*/





