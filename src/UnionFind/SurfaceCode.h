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
	uint32_t L = D;

public:

	uint32_t to_vertex_index(uint32_t row, uint32_t col)
	{
		return (((row + L) % L)) * L + (col + L) % L;
	}

	uint32_t vertex_connection_count(uint32_t vertex)
	{
		return 4;
	}

	Vector<uint32_t> vertex_connections(uint32_t v)
	{
		uint32_t row = v/L;
		uint32_t col = v%L;

		Vector<uint32_t> vector;

		vector.emplace(to_vertex_index(row - 1, col));
		vector.emplace(to_vertex_index(row + 1, col));
		vector.emplace(to_vertex_index(row, col - 1));
		vector.emplace(to_vertex_index(row, col + 1));

		return vector;
	}

	uint32_t num_vertices()
	{
		return L*L;
	}

	uint32_t num_edges()
	{
		return 2*L*L;
	}

	uint32_t edge_idx(Edge e)
	{
		return to_edge_idx(L, e);
	}

	uint32_t to_edge_idx(uint32_t L, Edge e)
	{
		if(is_horizontal(L, e))
			{
				auto u = left(L, e);
				Edge tmp = vertex_to_coord(L, u);
				uint32_t row = tmp.u;
				uint32_t col = tmp.v;
				return L * row + col + L * L;
			}
			else
			{
				auto u = upper(L, e);
				Edge tmp = vertex_to_coord(L, u);
				uint32_t row = tmp.u;
				uint32_t col = tmp.v;
				return L * row + col;
			}
	}

	uint32_t decoded_edge_to_qubit_idx(Edge e)
	{
		if(is_horizontal(L, e))
		{
			auto u = left(L, e);
			Edge tmp = vertex_to_coord(L, u);
			uint32_t row = tmp.u;
			uint32_t col = tmp.v;
			return L * ((row + 1) % L) + ((col + 1) % L);
		}
		else
		{
			auto u = upper(L, e);
			Edge tmp = vertex_to_coord(L, u);
			uint32_t row = tmp.u;
			uint32_t col = tmp.v;
			return L * (row % L) + col + L * L;
		}

	}

	bool is_horizontal(uint32_t L, Edge e)
	{
		return ((e.v - e.u) == 1) || ((e.v - e.u) == (L - 1));
	}

	uint32_t left(uint32_t L, Edge e)
	{
		if((e.v - e.u) == 1)
		{
			return e.u;
		}
		return e.v;
	}

	uint32_t upper(uint32_t L, Edge e)
	{
		if((e.v - e.u) == L)
		{
			return e.v;
		}
		return e.u;
	}

	Edge vertex_to_coord(const uint32_t L, const uint32_t vidx)
	{
		return {vidx / L, vidx % L};
	}
};

#endif
