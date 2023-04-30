#ifndef SURFACECODE_H
#define SURFACECODE_H

#include "../Defines.h"
#include "../Utility/Vector.h"
#include "../Utility/Map.h"


struct Edge
{
	uint32_t u;
	uint32_t v;

	bool operator==(const Edge& other) const
	{
		return (u == other.u && v == other.v);
	}
};

class SurfaceCode
{
private:
	uint32_t num_vertices;
	uint32_t num_edges;

	Vector<Vector<uint32_t>> vertex_connections;
	Map<Edge, uint32_t> edge_idx;


	void buildVertexConnections()
	{
		for(int i = 0; i < edge_idx.size(); ++i)
		{
			Edge e = edge_idx.get(i)->v1;
			vertex_connections.get(e.u)->emplace(e.v);
			vertex_connections.get(e.v)->emplace(e.u);
		}
	}

	void buildEdgeIdx(uint32_t ancillas, Vector<Vector<uint32_t>> ancilla_associated_dataqbits)
	{
		for(int i = 0; i < ancillas; ++i)
		{
			Vector<uint32_t>* q_parities = ancilla_associated_dataqbits.get(i);
			Edge edge = {q_parities->at(0), q_parities->at(1)};
			if(edge_idx.find(edge) == 0)
			{
				edge_idx.add(edge, i);
			}
		}
	}

	void setCode(uint32_t dataqbits, uint32_t ancillas, Vector<uint32_t> colIndices, Vector<uint32_t> indptr)
	{
		num_vertices = dataqbits;
		num_edges = ancillas;

		Vector<Vector<uint32_t>> ancilla_associated_dataqbits;
		for(uint32_t i = 0; i < dataqbits; ++i)
		{
			for(uint32_t j = indptr.at(i); j < indptr.at(i+1); ++j)
			{
				ancilla_associated_dataqbits.get(colIndices.at(j))->emplace(i);
			}
		}

		buildEdgeIdx(ancillas, ancilla_associated_dataqbits);
		buildVertexConnections();

	}

	Vector<uint32_t> toric_x_stabilizers_qubits(const uint32_t L, uint32_t vertex)
	{
		uint32_t row = vertex / L;
		uint32_t col = vertex % L;

		Vector<uint32_t> toReturn;

		toReturn.emplace(2 * row * L + col);
		toReturn.emplace(2 * row * L + (col + L));
		toReturn.emplace(2 * row * L + (col - 1 + L) % L);
		toReturn.emplace(2 * ((row - 1 + L) % L) * L + col + L);

		return toReturn;
	}

public:

	void buildCode()
	{
		uint32_t L = D;
		Vector<uint32_t> col_indices;
		Vector<uint32_t> indptr;

		indptr.emplace(0);
		for(uint32_t nx = 0; nx < L; ++nx)
		{
			for(uint32_t ny = 0; ny < L; ++ny)
			{
				Vector<uint32_t> m = toric_x_stabilizers_qubits(L, nx * L + ny);
				indptr.emplace(indptr.back()+ m.getSize());
				for(int i = 0; i < m.getSize(); ++i)
				{
					col_indices.emplace(m.at(i));
				}
			}
		}
		setCode(L * L, 2 * L * L, col_indices, indptr);

	}

	Vector<uint32_t> vertexConnectionsOf(uint32_t i)
	{
		return vertex_connections.at(i);
	}

	uint32_t vertexConnectionCountOf(uint32_t i)
	{
		return vertex_connections.at(i).getSize();
	}

	uint32_t edgeIdx(Edge e)
	{
		if(edge_idx.find(e) != 0)
		{
			return *edge_idx.find(e);
		}
		else
		{
			return 0;
		}
	}
};

#endif
