#ifndef SURFACECODE_H
#define SURFACECODE_H

#include "../Defines.h"
#include "../Utility/Vector.h"
#include "../Utility/Map.h"

#include <assert.h>


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
	uint32_t num_vertex;
	uint32_t num_edge;
	uint32_t L;

	Vector<Vector<uint32_t>> vertex_connection;
	Map<Edge, uint32_t> edge_indx;


	void buildVertexConnections()
	{
		for(int i = 0; i < edge_indx.size(); ++i)
		{
			Edge e = edge_indx.get(i)->v1;
			vertex_connection.get(e.u)->emplace(e.v);
			vertex_connection.get(e.v)->emplace(e.u);
		}
	}

	void buildEdgeIdx(uint32_t dataqubits, Vector<Vector<uint32_t>> ancilla_associated_dataqbits)
	{
		for(int i = 0; i < dataqubits; ++i)
		{
			Vector<uint32_t>* q_parities = ancilla_associated_dataqbits.get(i);
			Edge edge = {q_parities->at(0), q_parities->at(1)};
			if(edge_indx.find(edge) == 0)
			{
				edge_indx.add(edge, i);
			}
		}
	}

	void setCode(uint32_t ancillas, uint32_t dataqubit, Vector<uint32_t> colIndices, Vector<uint32_t> indptr)
	{
		num_vertex = ancillas;
		num_edge = dataqubit;

		Vector<Vector<uint32_t>> ancilla_associated_dataqbits;
		for(uint32_t i = 0; i < ancillas; ++i)
		{
			for(uint32_t j = indptr.at(i); j < indptr.at(i+1); ++j)
			{
				ancilla_associated_dataqbits.get(colIndices.at(j))->emplace(i);
			}
		}

		buildEdgeIdx(dataqubit, ancilla_associated_dataqbits);
		buildVertexConnections();

		for(int i = 0; i < vertex_connection.getSize(); ++i)
		{
			assert(vertex_connection.at(i).getSize() == 4);
		}

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
		L = D;

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

	Vector<uint32_t> vertex_connections(uint32_t i)
	{
		return vertex_connection.at(i);
	}

	uint32_t vertex_connection_count(uint32_t i)
	{
		return vertex_connection.at(i).getSize();
	}

	uint32_t edge_idx(Edge e)
	{
		if(edge_indx.find(e) != 0)
		{
			return *edge_indx.find(e);
		}
		else
		{
			return 0;
		}
	}

	uint32_t num_vertices()
	{
			return L*L;
	}

	uint32_t num_edges()
	{
			return 2*L*L;
	}
};

#endif
